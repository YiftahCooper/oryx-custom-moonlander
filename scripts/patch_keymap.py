import os
import re
import sys

PATCH_MARKER = "ORYX_FN24_NUMDOT_SPACE_PATCH"
LANGUAGE_TOGGLE_MARKER = "ORYX_LANG_TOGGLE_PATCH"
LANGUAGE_RESYNC_MARKER = "ORYX_LANG_RESYNC_PATCH"
LANGUAGE_RGB_MARKER = "ORYX_LANG_RGB_PATCH"
LANGUAGE_TAP_TERM_MARKER = "ORYX_LANG_TAP_TERM_PATCH"
TAPHOLD_COMPAT_MARKER = "ORYX_TAPHOLD_FALLBACK_PATCH"
DOUBLETAP_COMPAT_MARKER = "ORYX_DOUBLETAP_FALLBACK_PATCH"
SPACESHIFT_HOLD_PREF_MARKER = "ORYX_SPACESHIFT_HOLD_PREF_PATCH"
SPACE_DOT_TERM_MARKER = "ORYX_SPACE_DOT_TERM_PATCH"
LANGUAGE_SWITCH_TAPPING_TERM_MS = 2000
SPACE_DOT_TERM_SCALE_NUM = 6
SPACE_DOT_TERM_SCALE_DEN = 5
# Keep per-key tap windows from collapsing into impractically short ranges.
MAX_TAPPING_TERM_SUBTRACT = 40
RELAX_AGGRESSIVE_TAPPING_TERMS = True


def _find_matching_brace(content: str, open_idx: int) -> int:
    """
    Return index of the matching '}' for the '{' at open_idx.
    Skips braces inside strings and comments to keep block matching stable.
    """
    if open_idx < 0 or open_idx >= len(content) or content[open_idx] != "{":
        return -1

    depth = 0
    i = open_idx
    in_string = False
    in_char = False
    in_line_comment = False
    in_block_comment = False
    escape = False

    while i < len(content):
        ch = content[i]
        nxt = content[i + 1] if i + 1 < len(content) else ""

        if in_line_comment:
            if ch == "\n":
                in_line_comment = False
            i += 1
            continue

        if in_block_comment:
            if ch == "*" and nxt == "/":
                in_block_comment = False
                i += 2
                continue
            i += 1
            continue

        if in_string:
            if escape:
                escape = False
            elif ch == "\\":
                escape = True
            elif ch == '"':
                in_string = False
            i += 1
            continue

        if in_char:
            if escape:
                escape = False
            elif ch == "\\":
                escape = True
            elif ch == "'":
                in_char = False
            i += 1
            continue

        if ch == "/" and nxt == "/":
            in_line_comment = True
            i += 2
            continue
        if ch == "/" and nxt == "*":
            in_block_comment = True
            i += 2
            continue
        if ch == '"':
            in_string = True
            i += 1
            continue
        if ch == "'":
            in_char = True
            i += 1
            continue

        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0:
                return i
            if depth < 0:
                return -1
        i += 1

    return -1


def _replace_function_body(content: str, function_name: str, body: str) -> str:
    function_pat = re.compile(rf"\b{re.escape(function_name)}\s*\([^)]*\)\s*\{{")
    m = function_pat.search(content)
    if not m:
        return content

    open_brace_idx = content.find("{", m.start())
    if open_brace_idx == -1:
        return content

    close_brace_idx = _find_matching_brace(content, open_brace_idx)
    if close_brace_idx == -1:
        return content

    return content[: open_brace_idx + 1] + body + content[close_brace_idx:]


def _get_function_body(content: str, function_name: str) -> tuple[str, bool]:
    function_pat = re.compile(rf"\b{re.escape(function_name)}\s*\([^)]*\)\s*\{{")
    m = function_pat.search(content)
    if not m:
        return "", False

    open_brace_idx = content.find("{", m.start())
    if open_brace_idx == -1:
        return "", False

    close_brace_idx = _find_matching_brace(content, open_brace_idx)
    if close_brace_idx == -1:
        return "", False

    return content[open_brace_idx + 1 : close_brace_idx], True


def _replace_fn24_in_space_tap_dance(content: str) -> tuple[str, bool]:
    """
    Replace FN24 in the generated right-thumb tap dance with:
      DOUBLE_TAP and DOUBLE_SINGLE_TAP => num-dot then space.
    Target only dance_<n>_finished/reset function bodies.
    """
    def _replace_case(
        body: str,
        case_name: str,
        replacement_builder: callable,
    ) -> tuple[str, bool]:
        case_pat = re.compile(
            rf"(?P<indent>^[ \t]*)case\s+{case_name}\s*:\s*.*?(?=^[ \t]*case\s+|^[ \t]*default\s*:|}})",
            flags=re.MULTILINE | re.DOTALL,
        )
        match = case_pat.search(body)
        if not match:
            return body, False

        indent = match.group("indent")
        replacement = replacement_builder(indent)
        return case_pat.sub(replacement + "\n", body, count=1), True

    for dance_idx in range(0, 24):
        finished_name = f"dance_{dance_idx}_finished"
        reset_name = f"dance_{dance_idx}_reset"

        finished_body, has_finished = _get_function_body(content, finished_name)
        if not has_finished:
            continue

        if "KC_F24" not in finished_body and PATCH_MARKER not in finished_body:
            continue

        finished_body_new = finished_body
        finished_body_new, replaced_double_tap = _replace_case(
            finished_body_new,
            "DOUBLE_TAP",
            lambda indent: (
                f"{indent}case DOUBLE_TAP: tap_code16(KC_KP_DOT); tap_code16(KC_SPACE); "
                f"break; /* {PATCH_MARKER} */"
            ),
        )
        finished_body_new, replaced_double_single = _replace_case(
            finished_body_new,
            "DOUBLE_SINGLE_TAP",
            lambda indent: (
                f"{indent}case DOUBLE_SINGLE_TAP: tap_code16(KC_KP_DOT); tap_code16(KC_SPACE); "
                f"break; /* {PATCH_MARKER} */"
            ),
        )

        if not replaced_double_tap and not replaced_double_single:
            continue

        content = _replace_function_body(content, finished_name, finished_body_new)

        reset_body, has_reset = _get_function_body(content, reset_name)
        if has_reset:
            reset_body_new = reset_body
            reset_body_new, _ = _replace_case(
                reset_body_new,
                "DOUBLE_TAP",
                lambda indent: f"{indent}case DOUBLE_TAP: break; /* {PATCH_MARKER} */",
            )
            reset_body_new, _ = _replace_case(
                reset_body_new,
                "DOUBLE_SINGLE_TAP",
                lambda indent: f"{indent}case DOUBLE_SINGLE_TAP: break; /* {PATCH_MARKER} */",
            )
            content = _replace_function_body(content, reset_name, reset_body_new)

        return content, True

    return content, False


def _inject_custom_language_prototypes(content: str) -> tuple[str, bool]:
    if "void custom_language_toggled(void);" in content:
        return content, True

    prototype_block = (
        "\n// --- Custom language hooks (injected) ---\n"
        "void custom_language_toggled(void);\n"
        "void custom_language_resync(void);\n"
        "void custom_language_rgb_indicator(void);\n"
        "// ----------------------------------------\n"
    )

    include_matches = list(re.finditer(r"^\s*#include[^\n]*\n", content, flags=re.MULTILINE))
    if include_matches:
        insert_idx = include_matches[-1].end()
    else:
        insert_idx = 0

    return content[:insert_idx] + prototype_block + content[insert_idx:], True


def _patch_language_switch_tap_dance(content: str) -> tuple[str, bool, bool]:
    """
    Patch Oryx dance_1 language key behavior:
      - append custom_language_toggled() to Alt+Shift outputs
      - replace KC_F23 double-hold branch with custom_language_resync()
    """
    any_toggle_patch = False
    any_resync_patch = False

    language_dance_idx = _find_language_switch_dance_index(content)
    if language_dance_idx is None:
        return content, any_toggle_patch, any_resync_patch

    finished_name = f"dance_{language_dance_idx}_finished"
    on_name = f"on_dance_{language_dance_idx}"
    reset_name = f"dance_{language_dance_idx}_reset"

    finished_body, has_finished = _get_function_body(content, finished_name)
    if has_finished:
        alt_shift_call = r"(?:register_code16|tap_code16)\s*\(\s*LALT\s*\(\s*KC_LEFT_SHIFT\s*\)\s*\)\s*;"

        def _append_toggle(match: re.Match[str]) -> str:
            return f"{match.group(0)} custom_language_toggled(); /* {LANGUAGE_TOGGLE_MARKER} */"

        finished_body_new, toggle_n = re.subn(alt_shift_call, _append_toggle, finished_body)
        if toggle_n > 0:
            any_toggle_patch = True

        finished_body_new, resync_n = re.subn(
            r"case\s+DOUBLE_HOLD\s*:\s*(?:register_code16|tap_code16)\s*\(\s*KC_F23\s*\)\s*;\s*break\s*;",
            f"case DOUBLE_HOLD: custom_language_resync(); break; /* {LANGUAGE_RESYNC_MARKER} */",
            finished_body_new,
            count=1,
        )
        if resync_n > 0:
            any_resync_patch = True

        finished_body_new, single_hold_resync_n = re.subn(
            r"case\s+SINGLE_HOLD\s*:\s*register_code16\s*\(\s*KC_LEFT_CTRL\s*\)\s*;\s*break\s*;",
            f"case SINGLE_HOLD: custom_language_resync(); register_code16(KC_LEFT_CTRL); break; /* {LANGUAGE_RESYNC_MARKER} */",
            finished_body_new,
            count=1,
        )
        if single_hold_resync_n > 0:
            any_resync_patch = True

        content = _replace_function_body(content, finished_name, finished_body_new)

    on_body, has_on = _get_function_body(content, on_name)
    if has_on:
        alt_shift_tap_call = r"tap_code16\s*\(\s*LALT\s*\(\s*KC_LEFT_SHIFT\s*\)\s*\)\s*;"

        def _append_toggle_on(match: re.Match[str]) -> str:
            return f"{match.group(0)} custom_language_toggled(); /* {LANGUAGE_TOGGLE_MARKER} */"

        on_body_new, on_toggle_n = re.subn(alt_shift_tap_call, _append_toggle_on, on_body)
        if on_toggle_n > 0:
            any_toggle_patch = True
            content = _replace_function_body(content, on_name, on_body_new)

    reset_body, has_reset = _get_function_body(content, reset_name)
    if has_reset:
        reset_body_new, _ = re.subn(
            r"case\s+DOUBLE_HOLD\s*:\s*unregister_code16\s*\(\s*KC_F23\s*\)\s*;\s*break\s*;",
            f"case DOUBLE_HOLD: break; /* {LANGUAGE_RESYNC_MARKER} */",
            reset_body,
            count=1,
        )
        content = _replace_function_body(content, reset_name, reset_body_new)

    return content, any_toggle_patch, any_resync_patch


def _patch_rgb_indicator_hook(content: str) -> tuple[str, bool]:
    body, has_fn = _get_function_body(content, "rgb_matrix_indicators_user")
    if not has_fn:
        return content, False

    if "custom_language_rgb_indicator();" in body:
        return content, True

    body_new, return_n = re.subn(
        r"\breturn\s+true\s*;",
        f"custom_language_rgb_indicator(); /* {LANGUAGE_RGB_MARKER} */\n  return true;",
        body,
        count=1,
    )
    if return_n == 0:
        body_new = body + f"\n  custom_language_rgb_indicator(); /* {LANGUAGE_RGB_MARKER} */\n"

    return _replace_function_body(content, "rgb_matrix_indicators_user", body_new), True


def _clone_single_tap_to_single_hold(body: str) -> tuple[str, bool]:
    if "case SINGLE_HOLD:" in body or TAPHOLD_COMPAT_MARKER in body:
        return body, False

    single_tap_case = re.search(
        r"(?P<indent>[ \t]*)case\s+SINGLE_TAP\s*:\s*(?P<action>.*?)\s*break\s*;",
        body,
        flags=re.DOTALL,
    )
    if not single_tap_case:
        return body, False

    indent = single_tap_case.group("indent")
    action = single_tap_case.group("action").strip()
    if not action:
        return body, False

    injected_case = (
        f"{single_tap_case.group(0)}\n"
        f"{indent}case SINGLE_HOLD: {action} break; /* {TAPHOLD_COMPAT_MARKER} */"
    )
    return body[: single_tap_case.start()] + injected_case + body[single_tap_case.end() :], True


def _normalize_tap_dance_hold_resolution(content: str) -> tuple[str, int]:
    """
    For tap-dance keys that have SINGLE_TAP but no SINGLE_HOLD branch, mirror the
    old keymap behavior by treating SINGLE_HOLD as SINGLE_TAP.
    """
    patched_finished = 0

    for dance_idx in range(0, 24):
        finished_name = f"dance_{dance_idx}_finished"
        finished_body, has_finished = _get_function_body(content, finished_name)
        if not has_finished:
            continue

        finished_body_new, finished_changed = _clone_single_tap_to_single_hold(finished_body)
        if not finished_changed:
            continue

        content = _replace_function_body(content, finished_name, finished_body_new)
        patched_finished += 1

        reset_name = f"dance_{dance_idx}_reset"
        reset_body, has_reset = _get_function_body(content, reset_name)
        if not has_reset:
            continue

        reset_body_new, reset_changed = _clone_single_tap_to_single_hold(reset_body)
        if reset_changed:
            content = _replace_function_body(content, reset_name, reset_body_new)

    return content, patched_finished


def _prefer_hold_for_space_shift_dance(content: str) -> tuple[str, bool]:
    """
    For the dance that is SPACE on tap and SHIFT on hold, prefer hold when
    the key is interrupted by another key (fast chord typing).
    """
    for dance_idx in range(0, 24):
        finished_name = f"dance_{dance_idx}_finished"
        finished_body, has_finished = _get_function_body(content, finished_name)
        if not has_finished:
            continue

        if SPACESHIFT_HOLD_PREF_MARKER in finished_body:
            return content, True

        if "case SINGLE_TAP: register_code16(KC_SPACE);" not in finished_body:
            continue
        if "case SINGLE_HOLD: register_code16(KC_LEFT_SHIFT);" not in finished_body:
            continue

        step_assign_pat = re.compile(
            rf"dance_state\s*\[\s*{dance_idx}\s*\]\.step\s*=\s*dance_step\s*\(\s*state\s*\)\s*;"
        )
        replacement = (
            "if (state->count == 1 && state->interrupted) {\n"
            f"        dance_state[{dance_idx}].step = SINGLE_HOLD; /* {SPACESHIFT_HOLD_PREF_MARKER} */\n"
            "    } else {\n"
            f"        dance_state[{dance_idx}].step = dance_step(state);\n"
            "    }"
        )
        finished_body_new, replaced = step_assign_pat.subn(replacement, finished_body, count=1)
        if replaced == 0:
            return content, False

        content = _replace_function_body(content, finished_name, finished_body_new)
        return content, True

    return content, False


def _increase_space_dot_tapping_term(content: str) -> tuple[str, bool]:
    """
    Increase the dot+space dance tapping term by ~20%.
    Targets the same dance that was patched from KC_F24 to KP_DOT+SPACE.
    """
    target_dance_idx = None
    for dance_idx in range(0, 24):
        finished_name = f"dance_{dance_idx}_finished"
        finished_body, has_finished = _get_function_body(content, finished_name)
        if has_finished and PATCH_MARKER in finished_body:
            target_dance_idx = dance_idx
            break

    if target_dance_idx is None:
        return content, False

    tapping_body, has_tapping = _get_function_body(content, "get_tapping_term")
    if not has_tapping:
        return content, False

    if SPACE_DOT_TERM_MARKER in tapping_body:
        return content, True

    dance_case_pat = re.compile(
        rf"case\s+TD\s*\(\s*DANCE_{target_dance_idx}\s*\)\s*:\s*return\s+(?P<expr>[^;]+)\s*;"
    )
    case_match = dance_case_pat.search(tapping_body)
    if case_match:
        base_expr = case_match.group("expr").strip()
        replacement = (
            f"case TD(DANCE_{target_dance_idx}): "
            f"return (uint16_t)((({base_expr}) * {SPACE_DOT_TERM_SCALE_NUM}) / {SPACE_DOT_TERM_SCALE_DEN}); "
            f"/* {SPACE_DOT_TERM_MARKER} */"
        )
        tapping_body_new = dance_case_pat.sub(replacement, tapping_body, count=1)
        return _replace_function_body(content, "get_tapping_term", tapping_body_new), True

    default_pat = re.compile(r"^(?P<indent>\s*)default\s*:", flags=re.MULTILINE)

    def _insert_before_default(match: re.Match[str]) -> str:
        indent = match.group("indent")
        return (
            f"{indent}case TD(DANCE_{target_dance_idx}): "
            f"return (uint16_t)(((TAPPING_TERM) * {SPACE_DOT_TERM_SCALE_NUM}) / {SPACE_DOT_TERM_SCALE_DEN}); "
            f"/* {SPACE_DOT_TERM_MARKER} */\n"
            f"{indent}default:"
        )

    tapping_body_new, inserted = default_pat.subn(_insert_before_default, tapping_body, count=1)
    if inserted == 0:
        return content, False

    return _replace_function_body(content, "get_tapping_term", tapping_body_new), True


def _find_language_switch_dance_index(content: str) -> int | None:
    """
    Resolve the language-switch tap dance index from generated Oryx code.
    Oryx can renumber dance slots across revisions.
    """
    for dance_idx in range(0, 24):
        finished_name = f"dance_{dance_idx}_finished"
        finished_body, has_finished = _get_function_body(content, finished_name)
        if not has_finished:
            continue

        if "LALT(KC_LEFT_SHIFT)" in finished_body and "KC_F23" in finished_body:
            return dance_idx

    for dance_idx in range(0, 24):
        finished_name = f"dance_{dance_idx}_finished"
        finished_body, has_finished = _get_function_body(content, finished_name)
        if has_finished and "LALT(KC_LEFT_SHIFT)" in finished_body:
            return dance_idx

    return None


def _set_language_switch_tapping_term(content: str) -> tuple[str, bool]:
    """
    Set a very long tapping term for the language switch key so tap wins unless
    the key is intentionally held for around two seconds.
    """
    tapping_body, has_tapping = _get_function_body(content, "get_tapping_term")
    if not has_tapping:
        return content, False

    language_dance_idx = _find_language_switch_dance_index(content)
    if language_dance_idx is None:
        return content, False

    cleanup_pat = re.compile(
        rf"^\s*case\s+TD\s*\(\s*DANCE_\d+\s*\)\s*:\s*return\s+[^;]+\s*;\s*/\*\s*{LANGUAGE_TAP_TERM_MARKER}\s*\*/\s*$",
        flags=re.MULTILINE,
    )
    tapping_body = cleanup_pat.sub("", tapping_body)

    dance_case_pat = re.compile(
        rf"case\s+TD\s*\(\s*DANCE_{language_dance_idx}\s*\)\s*:\s*return\s+[^;]+\s*;"
    )
    replacement = (
        f"case TD(DANCE_{language_dance_idx}): "
        f"return (uint16_t){LANGUAGE_SWITCH_TAPPING_TERM_MS}; "
        f"/* {LANGUAGE_TAP_TERM_MARKER} */"
    )
    tapping_body_new, replaced = dance_case_pat.subn(replacement, tapping_body, count=1)
    if replaced > 0:
        return _replace_function_body(content, "get_tapping_term", tapping_body_new), True

    default_pat = re.compile(r"^(?P<indent>\s*)default\s*:", flags=re.MULTILINE)

    def _insert_before_default(match: re.Match[str]) -> str:
        indent = match.group("indent")
        return (
            f"{indent}case TD(DANCE_{language_dance_idx}): "
            f"return (uint16_t){LANGUAGE_SWITCH_TAPPING_TERM_MS}; "
            f"/* {LANGUAGE_TAP_TERM_MARKER} */\n"
            f"{indent}default:"
        )

    tapping_body_new, inserted = default_pat.subn(_insert_before_default, tapping_body, count=1)
    if inserted == 0:
        return content, False

    return _replace_function_body(content, "get_tapping_term", tapping_body_new), True


def _clone_double_tap_to_double_single(body: str) -> tuple[str, bool]:
    if "case DOUBLE_TAP:" not in body:
        return body, False

    # Preserve keys that intentionally differentiate double tap from double hold.
    if "case DOUBLE_HOLD:" in body:
        return body, False

    # Preserve dances with custom double-tap handling patched elsewhere.
    if PATCH_MARKER in body:
        return body, False

    double_single_pat = re.compile(
        r"(?P<indent>^[ \t]*)case\s+DOUBLE_SINGLE_TAP\s*:\s*.*?(?=^[ \t]*case\s+|^[ \t]*default\s*:|})",
        flags=re.MULTILINE | re.DOTALL,
    )

    # Repair previously malformed marker rewrites before deciding idempotency.
    malformed_existing = re.search(
        rf"case\s+DOUBLE_SINGLE_TAP\s*:.*?/\*\s*{re.escape(DOUBLETAP_COMPAT_MARKER)}\s*\*/\S",
        body,
        flags=re.DOTALL,
    )

    if DOUBLETAP_COMPAT_MARKER in body and not malformed_existing:
        return body, False

    double_tap_case = re.search(
        r"(?P<indent>^[ \t]*)case\s+DOUBLE_TAP\s*:\s*(?P<action>.*?)\s*break\s*;\s*(?=^[ \t]*case\s+|^[ \t]*default\s*:|})",
        body,
        flags=re.MULTILINE | re.DOTALL,
    )
    if not double_tap_case:
        return body, False

    indent = double_tap_case.group("indent")
    action = double_tap_case.group("action").strip()
    if not action:
        return body, False

    replacement_case = (
        f"{indent}case DOUBLE_SINGLE_TAP: {action} break; "
        f"/* {DOUBLETAP_COMPAT_MARKER} */"
    )

    if "case DOUBLE_SINGLE_TAP:" in body:
        body_new, replaced = double_single_pat.subn(replacement_case + "\n", body, count=1)
        return body_new, replaced > 0

    insert_idx = double_tap_case.end()
    body_new = body[:insert_idx] + f"\n{replacement_case}" + body[insert_idx:]
    return body_new, True


def _normalize_tap_dance_double_tap_resolution(content: str) -> tuple[str, int]:
    """
    Treat interrupted doubles (DOUBLE_SINGLE_TAP) like DOUBLE_TAP for dances
    that do not define DOUBLE_HOLD behavior.
    """
    patched_finished = 0

    for dance_idx in range(0, 24):
        finished_name = f"dance_{dance_idx}_finished"
        finished_body, has_finished = _get_function_body(content, finished_name)
        if not has_finished:
            continue

        finished_body_new, finished_changed = _clone_double_tap_to_double_single(finished_body)
        if not finished_changed:
            continue

        content = _replace_function_body(content, finished_name, finished_body_new)
        patched_finished += 1

        reset_name = f"dance_{dance_idx}_reset"
        reset_body, has_reset = _get_function_body(content, reset_name)
        if not has_reset:
            continue

        reset_body_new, reset_changed = _clone_double_tap_to_double_single(reset_body)
        if reset_changed:
            content = _replace_function_body(content, reset_name, reset_body_new)

    return content, patched_finished


def _relax_aggressive_tapping_terms(content: str) -> tuple[str, int]:
    """
    Oryx can emit very small per-key tapping terms (e.g. TAPPING_TERM-120/-134),
    which makes single taps behave like missed/hold events for normal typing speed.
    Clamp per-key reductions in get_tapping_term to a safer ceiling.
    """
    body, has_fn = _get_function_body(content, "get_tapping_term")
    if not has_fn:
        return content, 0

    changes = 0

    def _clamp(match: re.Match[str]) -> str:
        nonlocal changes
        original_subtract = int(match.group(1))
        clamped_subtract = min(original_subtract, MAX_TAPPING_TERM_SUBTRACT)
        if clamped_subtract != original_subtract:
            changes += 1
            return f"return TAPPING_TERM - {clamped_subtract};"
        return match.group(0)

    body_new = re.sub(r"return\s+TAPPING_TERM\s*-\s*(\d+)\s*;", _clamp, body)
    if changes == 0:
        return content, 0

    return _replace_function_body(content, "get_tapping_term", body_new), changes


def patch_keymap(layout_dir: str, custom_code_path: str) -> None:
    keymap_path = os.path.join(layout_dir, "keymap.c")
    if not os.path.exists(keymap_path):
        print(f"Error: {keymap_path} not found")
        print(f"Contents of {layout_dir}:")
        for f in os.listdir(layout_dir):
            print(f)
        sys.exit(1)

    with open(keymap_path, "r", encoding="utf-8") as f:
        content = f.read()

    print("Found keymap.c, length:", len(content))

    # 1) Add forward declarations for custom language hooks.
    content, _ = _inject_custom_language_prototypes(content)

    # 2) Replace FN24 behavior only in the corresponding tap-dance function.
    content, replaced = _replace_fn24_in_space_tap_dance(content)
    if replaced:
        print("Replaced KC_F24 tap-dance behavior with KP_DOT+SPACE on double tap")
    elif "KC_F24" in content:
        print("Warning: KC_F24 found, but no matching dance_<n>_finished/reset patch target was found.")
    else:
        print("KC_F24 not present in keymap.c; no FN24 tap-dance replacement needed.")

    # 3) Patch language switch/resync hooks in dance_1 if present.
    content, lang_toggle_patched, lang_resync_patched = _patch_language_switch_tap_dance(content)
    if lang_toggle_patched:
        print("Patched dance_1 Alt+Shift path with custom_language_toggled hooks")
    else:
        print("Warning: Did not patch dance_1 Alt+Shift path; language toggle counter may be incomplete.")

    if lang_resync_patched:
        print("Patched dance_1 resync hooks (double-hold and/or single-hold)")
    else:
        print("Warning: Did not patch dance_1 resync hooks.")

    # 4) Patch RGB indicator hook.
    content, rgb_patched = _patch_rgb_indicator_hook(content)
    if rgb_patched:
        print("Patched rgb_matrix_indicators_user with custom language indicator hook")
    else:
        print("Warning: rgb_matrix_indicators_user not found; language RGB indicator hook not applied.")

    # 5) For the SPACE/SHIFT dance, prefer hold when interrupted by another key.
    content, spaceshift_hold_pref_patched = _prefer_hold_for_space_shift_dance(content)
    if spaceshift_hold_pref_patched:
        print("Patched SPACE/SHIFT dance to prefer hold on interrupt")
    else:
        print("Warning: Could not patch SPACE/SHIFT hold-preference behavior.")

    # 6) For dances without explicit hold behavior, treat SINGLE_HOLD as SINGLE_TAP.
    content, hold_fallback_count = _normalize_tap_dance_hold_resolution(content)
    if hold_fallback_count > 0:
        print(f"Added SINGLE_HOLD->SINGLE_TAP fallback to {hold_fallback_count} tap-dance handlers")
    else:
        print("No tap-dance SINGLE_HOLD fallback patching required.")

    # 7) For dances without explicit double-hold behavior, treat DOUBLE_SINGLE_TAP
    # as DOUBLE_TAP so interrupted doubles still trigger the double function.
    content, doubletap_fallback_count = _normalize_tap_dance_double_tap_resolution(content)
    if doubletap_fallback_count > 0:
        print(f"Added DOUBLE_SINGLE_TAP->DOUBLE_TAP fallback to {doubletap_fallback_count} tap-dance handlers")
    else:
        print("No tap-dance DOUBLE_SINGLE_TAP fallback patching required.")

    # 8) Increase dot+space dance tapping term by ~20%.
    content, space_dot_term_patched = _increase_space_dot_tapping_term(content)
    if space_dot_term_patched:
        print("Raised dot+space dance tapping term by ~20%")
    else:
        print("Warning: Could not raise dot+space dance tapping term.")

    # 9) Set a long tapping term for the language switch tap-dance key.
    content, language_term_patched = _set_language_switch_tapping_term(content)
    if language_term_patched:
        print(f"Set language switch tapping term to {LANGUAGE_SWITCH_TAPPING_TERM_MS}ms")
    else:
        print("Warning: Could not set language switch tapping term.")

    # 10) Optionally relax aggressive per-key tapping-term reductions.
    if RELAX_AGGRESSIVE_TAPPING_TERMS:
        content, tapping_term_changes = _relax_aggressive_tapping_terms(content)
        if tapping_term_changes > 0:
            print(
                f"Relaxed {tapping_term_changes} aggressive per-key tapping-term reductions "
                f"(max subtract: {MAX_TAPPING_TERM_SUBTRACT})"
            )
        else:
            print("No aggressive per-key tapping-term reductions required patching.")
    else:
        print("Keeping Oryx per-key tapping terms unchanged.")

    # 11) Hook process_record_user
    pattern = r"bool\s+process_record_user\s*\("
    if not re.search(pattern, content):
        print("Error: Could not find process_record_user in keymap.c")
        print("File start:", content[:500])
        sys.exit(1)

    content = re.sub(pattern, "bool process_record_user_oryx(", content, count=1)

    wrapper_code = (
        "\n\n// ============================================================\n"
        "// INJECTED BY ORYX-CUSTOM-MOONLANDER WORKFLOW\n"
        "// ============================================================\n"
        "bool process_record_user_oryx(uint16_t keycode, keyrecord_t *record);\n"
        '#include "custom_code.c"\n'
        + "\n"
        "bool process_record_user(uint16_t keycode, keyrecord_t *record) {\n"
        "    if (!process_record_user_custom(keycode, record)) {\n"
        "        return false;\n"
        "    }\n"
        "    return process_record_user_oryx(keycode, record);\n"
        "}\n"
    )

    content += wrapper_code

    with open(keymap_path, "w", encoding="utf-8") as f:
        f.write(content)

    print("Successfully patched keymap.c")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: patch_keymap.py <layout_dir> <custom_code_path>")
        sys.exit(1)
    patch_keymap(sys.argv[1], sys.argv[2])

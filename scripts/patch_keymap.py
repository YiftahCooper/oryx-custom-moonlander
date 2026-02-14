import os
import re
import sys


def _replace_in_keymaps_only(content: str) -> tuple[str, bool]:
    """
    Replace KC_F24 -> TD(TD_SPACE) only inside the keymaps array.
    Uses permissive keymaps header matching + brace matching for robustness.
    """
    header_pat = re.compile(r"keymaps\s*\[\]\s*\[[^\]]+\]\s*\[[^\]]+\]", re.DOTALL)
    m = header_pat.search(content)
    if not m:
        return content, False

    eq_idx = content.find("=", m.end())
    if eq_idx == -1:
        return content, False

    open_brace_idx = content.find("{", eq_idx)
    if open_brace_idx == -1:
        return content, False

    close_brace_idx = _find_matching_brace(content, open_brace_idx)
    if close_brace_idx == -1:
        return content, False

    body = content[open_brace_idx + 1 : close_brace_idx]
    if "KC_F24" not in body:
        return content, False

    body2 = body.replace("KC_F24", "TD(TD_SPACE)")
    content2 = content[: open_brace_idx + 1] + body2 + content[close_brace_idx:]
    return content2, True


def _replace_kc_f24_fallback(content: str) -> tuple[str, bool]:
    """Fallback replacement when keymaps block parsing fails due upstream format drift."""
    replaced, n = re.subn(r"\bKC_F24\b", "TD(TD_SPACE)", content)
    return replaced, n > 0



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

    # 1) Inject TD_SPACE into enum
    enum_pattern = r"(enum\s+tap_dance_codes\s*\{)([^}]*)(\};)"
    m = re.search(enum_pattern, content, re.DOTALL)
    if m:
        print("Found tap_dance_codes enum")
        if "TD_SPACE" not in m.group(2):
            if "TAP_DANCE_COUNT" in m.group(2):
                new_body = m.group(2).replace("TAP_DANCE_COUNT", "TD_SPACE,\n    TAP_DANCE_COUNT")
            else:
                new_body = m.group(2) + "\n    TD_SPACE,"
            content = content.replace(m.group(0), m.group(1) + new_body + m.group(3))
            print("Injected TD_SPACE into enum")
    else:
        print("Warning: Could not find tap_dance_codes enum (TD_SPACE enum not injected).")

    # 1b) Ensure prototypes exist before tap_dance_actions[]
    td_prototypes = (
        "\n// --- TD_SPACE prototypes (injected) ---\n"
        "void td_space_finished(tap_dance_state_t *state, void *user_data);\n"
        "void td_space_reset(tap_dance_state_t *state, void *user_data);\n"
        "// -------------------------------------\n"
    )

    # 1c) Inject action entry
    action_pattern = r"(tap_dance_action_t\s+tap_dance_actions\[\]\s*=\s*\{)([^}]*)(\};)"
    m = re.search(action_pattern, content, re.DOTALL)
    if m:
        print("Found tap_dance_actions array")
        if "td_space_finished" not in content[: m.start()]:
            content = content[: m.start()] + td_prototypes + content[m.start() :]
            m = re.search(action_pattern, content, re.DOTALL)

        if m and "TD_SPACE" not in m.group(2):
            new_action = "\n    [TD_SPACE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_space_finished, td_space_reset),"
            content = content.replace(m.group(0), m.group(1) + m.group(2) + new_action + m.group(3))
            print("Injected TD_SPACE action")
    else:
        print("Warning: Could not find tap_dance_actions array (TD_SPACE action not injected).")

    td_logic_code = r"""
// --- TD_SPACE Injection ---
void td_space_finished(tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {
        tap_code(KC_SPACE);
    } else if (state->count == 2) {
        tap_code(KC_KP_DOT);
        tap_code(KC_SPACE);
    }
}

void td_space_reset(tap_dance_state_t *state, void *user_data) {
    // No cleanup needed
}
// --------------------------
"""

        # 2) Replace KC_F24 safely (keymaps[] only), fallback globally if parsing drifted.
    content, replaced = _replace_in_keymaps_only(content)
    if replaced:
        print("Replaced KC_F24 with TD(TD_SPACE) inside keymaps[]")
    elif "KC_F24" in content:
        content, replaced_fallback = _replace_kc_f24_fallback(content)
        if replaced_fallback:
            print("Replaced KC_F24 with TD(TD_SPACE) via fallback token replacement")
        else:
            print("Warning: KC_F24 replacement fallback did not change content")
    else:
        print("Warning: KC_F24 not replaced (keymaps[] block not found or KC_F24 absent).")

    # 3) Hook process_record_user
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
        + td_logic_code
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

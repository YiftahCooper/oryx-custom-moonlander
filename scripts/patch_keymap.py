import os
import sys
import re

def patch_keymap(layout_dir, custom_code_path):
    keymap_path = os.path.join(layout_dir, 'keymap.c')
    
    if not os.path.exists(keymap_path):
        print(f"Error: {keymap_path} not found")
        # List dir to debug
        print(f"Contents of {layout_dir}:")
        for f in os.listdir(layout_dir):
            print(f)
        sys.exit(1)
        
    with open(keymap_path, 'r') as f:
        content = f.read()

    print("Found keymap.c, length:", len(content))

    # =========================================================================
    # 1. Inject TD_SPACE Logic
    # =========================================================================
    
    # A. Inject Enum Value
    # Find "enum tap_dance_codes {" and insert TD_SPACE before the closing brace or specific marker
    # Oryx usually ends with "};"
    enum_pattern = r'(enum\s+tap_dance_codes\s*\{)([^}]*)(\};)'
    match = re.search(enum_pattern, content, re.DOTALL)
    if match:
        print("Found tap_dance_codes enum")
        # Check if TD_SPACE is already there (unlikely unless user added it)
        if "TD_SPACE" not in match.group(2):
            # Insert TD_SPACE before the end
            # If TAP_DANCE_COUNT exists, put it before that.
            if "TAP_DANCE_COUNT" in match.group(2):
                new_enum_body = match.group(2).replace("TAP_DANCE_COUNT", "TD_SPACE,\n  TAP_DANCE_COUNT")
            else:
                new_enum_body = match.group(2) + "\n  TD_SPACE,"
            
            content = content.replace(match.group(0), match.group(1) + new_enum_body + match.group(3))
            print("Injected TD_SPACE into enum")
    else:
        print("Warning: Could not find tap_dance_codes enum. Creating one?")
        # If no tap dance exists yet, we might need to create the whole block.
        # But Oryx usually has it if any tap dance is used.
        # If not, we might fail to compile if we assume it exists.
        pass

    # B. Inject Tap Dance Action
    # Find "tap_dance_action_t tap_dance_actions[] = {"
    action_pattern = r'(tap_dance_action_t\s+tap_dance_actions\[\]\s*=\s*\{)([^}]*)(\};)'
    match = re.search(action_pattern, content, re.DOTALL)
    if match:
        print("Found tap_dance_actions array")
        if "TD_SPACE" not in match.group(2):
            # Add our action
            new_action = "\n  [TD_SPACE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_space_finished, td_space_reset),"
            content = content.replace(match.group(0), match.group(1) + match.group(2) + new_action + match.group(3))
            print("Injected TD_SPACE action")

    # C. Inject Helper Functions
    # We'll append these to the end or put them in the wrapper
    td_logic_code = """
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
    
    # =========================================================================
    # 2. Keymap Replacement
    # =========================================================================
    
    # Replace KC_F24 with TD(TD_SPACE)
    # This allows the user to map a specific key in Oryx to F24, and we hijack it.
    if "KC_F24" in content:
        content = content.replace("KC_F24", "TD(TD_SPACE)")
        print("Replaced KC_F24 with TD(TD_SPACE)")
    else:
        print("Warning: KC_F24 not found. Make sure you mapped the key to F24 in Oryx!")

    # =========================================================================
    # 3. Hook process_record_user
    # =========================================================================

    pattern = r'bool\s+process_record_user\s*\('
    if not re.search(pattern, content):
        print("Error: Could not find process_record_user in keymap.c")
        # Print first 500 chars to help debug
        print("File start:", content[:500])
        sys.exit(1)
        
    content = re.sub(pattern, 'bool process_record_user_oryx(', content)

    wrapper_code = """
// ============================================================
// INJECTED BY ORYX-CUSTOM-MOONLANDER WORKFLOW
// ============================================================

// Forward declaration of the Oryx function we renamed
bool process_record_user_oryx(uint16_t keycode, keyrecord_t *record);

// Include user custom code
#include "custom_code.c"

// TD_SPACE Logic
""" + td_logic_code + """

// Main entry point called by QMK
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  // 1. Run custom logic first
  if (!process_record_user_custom(keycode, record)) {
    return false; // Custom code handled it and requested stop
  }
  
  // 2. Fallback to Oryx logic
  return process_record_user_oryx(keycode, record);
}
"""
    content += wrapper_code

    with open(keymap_path, 'w') as f:
        f.write(content)
    
    print("Successfully patched keymap.c")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: patch_keymap.py <layout_dir> <custom_code_path>")
        sys.exit(1)
        
    patch_keymap(sys.argv[1], sys.argv[2])

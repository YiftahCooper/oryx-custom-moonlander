import os
import sys
import re

def patch_keymap(layout_dir, custom_code_path):
    keymap_path = os.path.join(layout_dir, 'keymap.c')
    
    if not os.path.exists(keymap_path):
        print(f"Error: {keymap_path} not found")
        sys.exit(1)
        
    with open(keymap_path, 'r') as f:
        content = f.read()

    # 1. Rename Oryx's process_record_user
    # Look for "bool process_record_user(" and replace with "bool process_record_user_oryx("
    # Note: We rely on the exact signature found in Oryx files
    pattern = r'bool\s+process_record_user\s*\('
    if not re.search(pattern, content):
        print("Error: Could not find process_record_user in keymap.c")
        sys.exit(1)
        
    new_content = re.sub(pattern, 'bool process_record_user_oryx(', content)

    # 2. Prepare the Wrapper Code
    # We need to declare the custom function and the Oryx function
    wrapper_code = """
// ============================================================
// INJECTED BY ORYX-CUSTOM-MOONLANDER WORKFLOW
// ============================================================

// Forward declaration of the Oryx function we renamed
bool process_record_user_oryx(uint16_t keycode, keyrecord_t *record);

// Include user custom code directly
#include "custom_code.c"

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

    # 3. Append Wrapper to the end
    new_content += wrapper_code

    with open(keymap_path, 'w') as f:
        f.write(new_content)
    
    print("Successfully patched keymap.c")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: patch_keymap.py <layout_dir> <custom_code_path>")
        sys.exit(1)
        
    patch_keymap(sys.argv[1], sys.argv[2])

# Custom QMK Build for ZSA Moonlander (Clever Injection Method)

This repository allows you to sync your Oryx layout with custom QMK code **without merge conflicts**.

## How it works (The Clever Bit)

Instead of maintaining a modified `keymap.c` and fighting git merge conflicts every time Oryx updates:
1.  We maintain our custom code in `custom_qmk/custom_code.c`.
2.  The workflow downloads the *fresh* Oryx source code.
3.  It **injects** our custom code into the Oryx keymap on-the-fly during the build.
    *   It renames Oryx's `process_record_user` to `process_record_user_oryx`.
    *   It adds a wrapper `process_record_user` that calls YOUR code first.

## How to use

1.  **Add Custom Logic**:
    *   Edit `custom_qmk/custom_code.c` in this repository.
    *   Add your macros, key overrides, or LED logic there.
2.  **Run the Workflow**:
    *   Go to **Actions** -> **Fetch and build layout**.
    *   Run on branch **`main`**.
3.  **Download Firmware**:
    *   Get the `.bin` file from the run artifacts.
4.  **Optional: Windows Language Sync (Windhawk)**:
    *   Follow `docs/windhawk-language-sync.md` to sync OS language state to keyboard RGB.

## File Structure

*   `custom_qmk/custom_code.c`: **EDIT THIS FILE.** This is where your code lives.
*   `scripts/patch_keymap.py`: The script that performs the injection.
*   `keymap.c` (in artifacts): Generated automatically. Do not edit.

## Why this is better
*   **No Conflicts**: You never edit the generated Oryx file directly.
*   **Clean History**: Your repo only tracks your custom code, not the thousands of lines of Oryx generated code.
*   **Safe**: If Oryx updates their code structure, the patch script might fail (safe fail), but you won't silently lose logic in a bad merge.

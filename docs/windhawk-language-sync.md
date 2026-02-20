# Windhawk Language Sync Setup (Moonlander + Oryx/QMK)

This setup makes the keyboard RGB language indicator follow the actual Windows input language state.

## What This Adds

1. Optional `F18` -> language switch shortcut on Windows (via Windhawk).
2. Windows language-state sync -> keyboard over RAW HID:
   - English = blue indicator
   - Hebrew = red indicator

## Files Added

- `host_tools/windhawk/moonlander_language_sync.wh.cpp`
- `custom_qmk/custom_code.c` updated with RAW HID language-state receiver
- `scripts/patch_keymap.py` updated to keep language key behavior Oryx-managed while injecting only the RGB indicator hook

## 1) Oryx Configuration

1. In Oryx, set your language key to:
   - Tap: `KC_F18`
   - Hold: `KC_LEFT_CTRL`
2. Keep your normal global tapping-term and hold settings as you prefer.
3. Export/apply changes in Oryx.

## 2) Build and Flash

1. Run GitHub Action: `Fetch and build layout`.
2. Flash the newly built `.bin`.

## 3) Install the Windhawk Mod

1. Open Windhawk.
2. Create a new mod and paste `host_tools/windhawk/moonlander_language_sync.wh.cpp`.
3. Build and enable the mod.
4. Keep default settings first:
   - `enableF18Hotkey = true`
   - `shortcutMode = 1` (`Win+Space`)
   - `pollIntervalMs = 120`
   - `onlyMoonlander = true`

## 4) Verify

1. Tap your language key:
   - Windows should switch language.
2. RGB indicator key should reflect current language.
3. Hold the same key with another key:
   - Should still behave as `Ctrl` (from Oryx dual-role mapping).

## Troubleshooting

1. If language switches but RGB does not update:
   - Set `debugLogging = true` in mod settings and check Windhawk log output.
   - Temporarily set `onlyMoonlander = false` to test broader HID matching.
2. If `F18` should not be remapped by Windhawk:
   - Set `enableF18Hotkey = false`.
   - Keep only OS->keyboard language sync active.
3. If Windows shortcut preference is different:
   - Set `shortcutMode`:
     - `1`: `Win+Space` (recommended)
     - `2`: `Alt+Shift`
     - `3`: `Ctrl+Shift`

## Protocol Reference

Windhawk sends an Oryx-native RAW HID command:

- Command: `ORYX_STATUS_LED_CONTROL` (`0x0A`)
- Payload: `param[0] = 0x00` English, `0x01` Hebrew

Firmware reads mirrored state from `rawhid_state.status_led_control` in `custom_qmk/custom_code.c`.

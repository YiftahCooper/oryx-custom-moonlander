// ==WindhawkMod==
// @id              moonlander-language-sync
// @name            Moonlander Language Sync
// @description     Maps F18 to language switching and syncs EN/HE state to QMK RGB via RAW HID.
// @version         1.0.1
// @author          Yiftah + Codex
// @include         explorer.exe
// @compilerOptions -lsetupapi -lhid
// ==/WindhawkMod==

// ==WindhawkModReadme==
// This mod does two things:
// 1) Optional: maps F18 to a language-switch shortcut (Win+Space by default).
// 2) Sends current Windows input language state to QMK over RAW HID:
//    - English -> 0
//    - Hebrew  -> 1
//
// Transport:
// Uses Oryx's ORYX_STATUS_LED_CONTROL command (0x0A), where param[0]
// carries the language state (0 English, 1 Hebrew). Firmware reads the
// mirrored bool via rawhid_state.status_led_control.
//
// Recommended keyboard mapping:
// - Tap: KC_F18
// - Hold: KC_LEFT_CTRL
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enableF18Hotkey: true
  $name: Enable F18 language hotkey
  $description: If enabled, F18 sends the configured language shortcut.
- shortcutMode: 1
  $name: F18 shortcut mode
  $description: 0=None, 1=Win+Space, 2=Alt+Shift, 3=Ctrl+Shift.
- pollIntervalMs: 120
  $name: Poll interval (ms)
  $description: How often to read current Windows input language.
- onlyMoonlander: true
  $name: Restrict to Moonlander
  $description: If enabled, only devices with product string containing "Moonlander" are targeted.
- debugLogging: false
  $name: Debug logging
  $description: Enable verbose Windhawk log messages.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <setupapi.h>
#include <hidsdi.h>
#include <hidpi.h>

#include <cstdint>
#include <cwctype>
#include <string>
#include <vector>

namespace {

constexpr UINT kHotkeyId = 0xA701;
constexpr uint16_t kRawHidUsagePage = 0xFF60;
constexpr uint16_t kRawHidUsage = 0x0061;
constexpr uint8_t kOryxStatusLedControlCommand = 0x0A;
constexpr uint8_t kLanguageSyncEnglish = 0x00;
constexpr uint8_t kLanguageSyncHebrew = 0x01;
constexpr DWORD kRetryWhenNotSentMs = 5000;

enum ShortcutMode : int {
    ShortcutNone = 0,
    ShortcutWinSpace = 1,
    ShortcutAltShift = 2,
    ShortcutCtrlShift = 3,
};

struct Settings {
    bool enable_f18_hotkey = true;
    int shortcut_mode = ShortcutWinSpace;
    int poll_interval_ms = 120;
    bool only_moonlander = true;
    bool debug_logging = false;
};

Settings g_settings;
HANDLE g_stop_event = nullptr;
HANDLE g_worker_thread = nullptr;

std::wstring to_lower_copy(const wchar_t *text) {
    if (!text) {
        return L"";
    }
    std::wstring lowered(text);
    for (wchar_t &ch : lowered) {
        ch = static_cast<wchar_t>(towlower(ch));
    }
    return lowered;
}

bool contains_case_insensitive(const wchar_t *haystack, const wchar_t *needle) {
    if (!needle || needle[0] == L'\0') {
        return true;
    }
    std::wstring hay = to_lower_copy(haystack);
    std::wstring ndl = to_lower_copy(needle);
    return hay.find(ndl) != std::wstring::npos;
}

void load_settings() {
    g_settings.enable_f18_hotkey = Wh_GetIntSetting(L"enableF18Hotkey") != 0;

    int shortcut_mode = Wh_GetIntSetting(L"shortcutMode");
    if (shortcut_mode < ShortcutNone || shortcut_mode > ShortcutCtrlShift) {
        shortcut_mode = ShortcutWinSpace;
    }
    g_settings.shortcut_mode = shortcut_mode;

    int poll_interval_ms = Wh_GetIntSetting(L"pollIntervalMs");
    if (poll_interval_ms < 20 || poll_interval_ms > 2000) {
        poll_interval_ms = 120;
    }
    g_settings.poll_interval_ms = poll_interval_ms;

    g_settings.only_moonlander = Wh_GetIntSetting(L"onlyMoonlander") != 0;
    g_settings.debug_logging = Wh_GetIntSetting(L"debugLogging") != 0;
}

void send_key_event(WORD virtual_key, bool pressed) {
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = virtual_key;
    if (!pressed) {
        input.ki.dwFlags = KEYEVENTF_KEYUP;
    }
    SendInput(1, &input, sizeof(INPUT));
}

void send_win_space() {
    send_key_event(VK_LWIN, true);
    send_key_event(VK_SPACE, true);
    send_key_event(VK_SPACE, false);
    send_key_event(VK_LWIN, false);
}

void send_modifier_chord(WORD mod1, WORD mod2) {
    send_key_event(mod1, true);
    send_key_event(mod2, true);
    send_key_event(mod2, false);
    send_key_event(mod1, false);
}

void trigger_language_shortcut() {
    switch (g_settings.shortcut_mode) {
        case ShortcutWinSpace:
            send_win_space();
            break;
        case ShortcutAltShift:
            send_modifier_chord(VK_LMENU, VK_LSHIFT);
            break;
        case ShortcutCtrlShift:
            send_modifier_chord(VK_LCONTROL, VK_LSHIFT);
            break;
        case ShortcutNone:
        default:
            break;
    }
}

bool get_active_language_is_hebrew(bool *is_hebrew) {
    if (!is_hebrew) {
        return false;
    }

    HWND foreground_window = GetForegroundWindow();
    DWORD thread_id = foreground_window ? GetWindowThreadProcessId(foreground_window, nullptr) : 0;
    HKL keyboard_layout = GetKeyboardLayout(thread_id);
    LANGID lang_id = LOWORD((UINT_PTR)keyboard_layout);
    *is_hebrew = (PRIMARYLANGID(lang_id) == LANG_HEBREW);
    return true;
}

bool device_matches_filters(HANDLE device_handle) {
    wchar_t manufacturer[128] = {};
    wchar_t product[128] = {};
    HidD_GetManufacturerString(device_handle, manufacturer, sizeof(manufacturer));
    HidD_GetProductString(device_handle, product, sizeof(product));

    if (!contains_case_insensitive(manufacturer, L"zsa")) {
        return false;
    }

    if (g_settings.only_moonlander && !contains_case_insensitive(product, L"moonlander")) {
        return false;
    }

    return true;
}

bool send_language_report_to_device(HANDLE device_handle, bool is_hebrew) {
    PHIDP_PREPARSED_DATA preparsed_data = nullptr;
    if (!HidD_GetPreparsedData(device_handle, &preparsed_data) || !preparsed_data) {
        return false;
    }

    HIDP_CAPS caps = {};
    NTSTATUS caps_status = HidP_GetCaps(preparsed_data, &caps);
    HidD_FreePreparsedData(preparsed_data);

    if (caps_status != HIDP_STATUS_SUCCESS) {
        return false;
    }

    if (caps.UsagePage != kRawHidUsagePage || caps.Usage != kRawHidUsage) {
        return false;
    }

    const ULONG report_len = caps.OutputReportByteLength;
    if (report_len < 4) {
        return false;
    }

    std::vector<uint8_t> report(report_len, 0);
    report[0] = 0x00;  // Report ID
    report[1] = kOryxStatusLedControlCommand;
    report[2] = is_hebrew ? kLanguageSyncHebrew : kLanguageSyncEnglish;

    DWORD written = 0;
    BOOL write_ok = WriteFile(
        device_handle,
        report.data(),
        static_cast<DWORD>(report.size()),
        &written,
        nullptr
    );

    if (write_ok && written == report.size()) {
        return true;
    }

    return HidD_SetOutputReport(
        device_handle,
        report.data(),
        static_cast<ULONG>(report.size())
    ) == TRUE;
}

bool send_language_state_to_keyboards(bool is_hebrew) {
    GUID hid_guid = {};
    HidD_GetHidGuid(&hid_guid);

    HDEVINFO device_info = SetupDiGetClassDevsW(
        &hid_guid,
        nullptr,
        nullptr,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
    );
    if (device_info == INVALID_HANDLE_VALUE) {
        return false;
    }

    bool any_sent = false;
    SP_DEVICE_INTERFACE_DATA interface_data = {};
    interface_data.cbSize = sizeof(interface_data);

    for (DWORD index = 0;; ++index) {
        if (!SetupDiEnumDeviceInterfaces(device_info, nullptr, &hid_guid, index, &interface_data)) {
            break;
        }

        DWORD required_size = 0;
        SetupDiGetDeviceInterfaceDetailW(
            device_info,
            &interface_data,
            nullptr,
            0,
            &required_size,
            nullptr
        );
        if (required_size == 0) {
            continue;
        }

        std::vector<uint8_t> detail_buffer(required_size, 0);
        PSP_DEVICE_INTERFACE_DETAIL_DATA_W detail_data =
            reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA_W>(detail_buffer.data());
        detail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

        if (!SetupDiGetDeviceInterfaceDetailW(
                device_info,
                &interface_data,
                detail_data,
                required_size,
                nullptr,
                nullptr)) {
            continue;
        }

        HANDLE device_handle = CreateFileW(
            detail_data->DevicePath,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
        );
        if (device_handle == INVALID_HANDLE_VALUE) {
            continue;
        }

        if (device_matches_filters(device_handle) &&
            send_language_report_to_device(device_handle, is_hebrew)) {
            any_sent = true;
        }

        CloseHandle(device_handle);
    }

    SetupDiDestroyDeviceInfoList(device_info);
    return any_sent;
}

DWORD WINAPI worker_thread_proc(void *) {
    MSG msg = {};
    // Create message queue for WM_HOTKEY.
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    bool hotkey_registered = false;
    if (g_settings.enable_f18_hotkey && g_settings.shortcut_mode != ShortcutNone) {
        hotkey_registered = RegisterHotKey(nullptr, kHotkeyId, MOD_NOREPEAT, VK_F18) == TRUE;
        if (!hotkey_registered && g_settings.debug_logging) {
            Wh_Log(L"RegisterHotKey(VK_F18) failed: %lu", GetLastError());
        }
    }

    bool have_last_state = false;
    bool last_is_hebrew = false;
    bool last_send_succeeded = false;
    DWORD next_unsent_retry_tick = 0;
    DWORD last_poll_tick = 0;

    while (WaitForSingleObject(g_stop_event, 0) == WAIT_TIMEOUT) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_HOTKEY && msg.wParam == kHotkeyId) {
                trigger_language_shortcut();
                have_last_state = false;  // force immediate state refresh
                last_send_succeeded = false;
                next_unsent_retry_tick = 0;
                last_poll_tick = 0;
            }
        }

        DWORD now = GetTickCount();
        if (last_poll_tick == 0 || now - last_poll_tick >= static_cast<DWORD>(g_settings.poll_interval_ms)) {
            last_poll_tick = now;

            bool is_hebrew = false;
            if (get_active_language_is_hebrew(&is_hebrew)) {
                bool state_changed = !have_last_state || is_hebrew != last_is_hebrew;
                bool should_retry_unsent = !state_changed && !last_send_succeeded &&
                                           (next_unsent_retry_tick == 0 || now >= next_unsent_retry_tick);
                if (state_changed || should_retry_unsent) {
                    bool sent = send_language_state_to_keyboards(is_hebrew);
                    last_is_hebrew = is_hebrew;
                    have_last_state = true;
                    last_send_succeeded = sent;
                    next_unsent_retry_tick = sent ? 0 : (now + kRetryWhenNotSentMs);

                    if (g_settings.debug_logging) {
                        if (sent) {
                            Wh_Log(L"Language sync sent: %ls", is_hebrew ? L"Hebrew" : L"English");
                        } else {
                            Wh_Log(L"Language sync not sent; retry scheduled in %lu ms", kRetryWhenNotSentMs);
                        }
                    }
                }
            }
        }

        Sleep(10);
    }

    if (hotkey_registered) {
        UnregisterHotKey(nullptr, kHotkeyId);
    }

    return 0;
}

bool start_worker() {
    g_stop_event = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stop_event) {
        return false;
    }

    g_worker_thread = CreateThread(nullptr, 0, worker_thread_proc, nullptr, 0, nullptr);
    if (!g_worker_thread) {
        CloseHandle(g_stop_event);
        g_stop_event = nullptr;
        return false;
    }

    return true;
}

void stop_worker() {
    if (g_stop_event) {
        SetEvent(g_stop_event);
    }

    if (g_worker_thread) {
        WaitForSingleObject(g_worker_thread, 5000);
        CloseHandle(g_worker_thread);
        g_worker_thread = nullptr;
    }

    if (g_stop_event) {
        CloseHandle(g_stop_event);
        g_stop_event = nullptr;
    }
}

}  // namespace

BOOL Wh_ModInit() {
    load_settings();
    if (!start_worker()) {
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit() {
    stop_worker();
}

void Wh_ModSettingsChanged() {
    stop_worker();
    load_settings();
    start_worker();
}

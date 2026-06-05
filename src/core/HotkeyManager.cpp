#include "HotkeyManager.h"
#include <QApplication>
#include <QSettings>
#include <QDebug>

HotkeyManager& HotkeyManager::instance()
{
    static HotkeyManager inst;
    return inst;
}

HotkeyManager::HotkeyManager(QObject *parent) : QObject(parent)
{
    qApp->installNativeEventFilter(this);

    QSettings s("EShot", "EShot");
    UINT modifiers = static_cast<UINT>(s.value("hotkeyModifiers", 0).toUInt());
    UINT vkey      = static_cast<UINT>(s.value("hotkeyVKey", VK_SNAPSHOT).toUInt());

    if (registerHotkey(HOTKEY_CAPTURE, modifiers, vkey)) {
        m_captureModifiers = modifiers;
        m_captureVirtualKey = vkey;
        qDebug() << "[HotkeyManager] Capture hotkey registered (mod=" << modifiers << " vk=" << vkey << ")";
    } else {
        qWarning() << "[HotkeyManager] Failed to register hotkey (mod=" << modifiers << " vk=" << vkey << "). Trying default...";
        // Try default PrtSc as well
        if (!registerHotkey(HOTKEY_CAPTURE, 0, VK_SNAPSHOT)) {
            qWarning() << "[HotkeyManager] Default hotkey also failed. User must change hotkey in settings.";
        } else {
            m_captureModifiers = 0;
            m_captureVirtualKey = VK_SNAPSHOT;
        }
    }

    m_recordingPauseModifiers = static_cast<UINT>(s.value("recordingPauseHotkeyModifiers", MOD_CONTROL | MOD_ALT).toUInt());
    m_recordingPauseVirtualKey = static_cast<UINT>(s.value("recordingPauseHotkeyVKey", 'P').toUInt());
    m_recordingStopModifiers = static_cast<UINT>(s.value("recordingStopHotkeyModifiers", MOD_CONTROL | MOD_ALT).toUInt());
    m_recordingStopVirtualKey = static_cast<UINT>(s.value("recordingStopHotkeyVKey", 'S').toUInt());
    m_recordingCancelModifiers = static_cast<UINT>(s.value("recordingCancelHotkeyModifiers", MOD_CONTROL | MOD_ALT).toUInt());
    m_recordingCancelVirtualKey = static_cast<UINT>(s.value("recordingCancelHotkeyVKey", 'X').toUInt());
    registerHotkey(HOTKEY_RECORDING_PAUSE, m_recordingPauseModifiers, m_recordingPauseVirtualKey);
    registerHotkey(HOTKEY_RECORDING_STOP, m_recordingStopModifiers, m_recordingStopVirtualKey);
    registerHotkey(HOTKEY_RECORDING_CANCEL, m_recordingCancelModifiers, m_recordingCancelVirtualKey);
}

HotkeyManager::~HotkeyManager()
{
    unregisterAllHotkeys();
    qApp->removeNativeEventFilter(this);
}

bool HotkeyManager::registerHotkey(int id, UINT modifiers, UINT virtualKey)
{
    if (RegisterHotKey(nullptr, id, modifiers, virtualKey)) {
        m_registeredHotkeys.append(id);
        return true;
    }
    return false;
}

bool HotkeyManager::isPlainPrintScreen(UINT modifiers, UINT virtualKey)
{
    return modifiers == 0 && virtualKey == VK_SNAPSHOT;
}

bool HotkeyManager::isWindowsPrintScreenSnippingEnabled()
{
#ifdef Q_OS_WIN
    QSettings reg(QStringLiteral("HKEY_CURRENT_USER\\Control Panel\\Keyboard"),
                  QSettings::NativeFormat);
    return reg.value(QStringLiteral("PrintScreenKeyForSnippingEnabled"), 0).toInt() != 0;
#else
    return false;
#endif
}

bool HotkeyManager::setWindowsPrintScreenSnippingEnabled(bool enabled)
{
#ifdef Q_OS_WIN
    QSettings reg(QStringLiteral("HKEY_CURRENT_USER\\Control Panel\\Keyboard"),
                  QSettings::NativeFormat);
    reg.setValue(QStringLiteral("PrintScreenKeyForSnippingEnabled"), enabled ? 1 : 0);
    reg.sync();
    return reg.status() == QSettings::NoError;
#else
    Q_UNUSED(enabled);
    return false;
#endif
}

void HotkeyManager::unregisterHotkey(int id)
{
    UnregisterHotKey(nullptr, id);
    m_registeredHotkeys.removeAll(id);
}

void HotkeyManager::unregisterAllHotkeys()
{
    for (int id : m_registeredHotkeys) UnregisterHotKey(nullptr, id);
    m_registeredHotkeys.clear();
}

bool HotkeyManager::reRegisterCaptureHotkey(UINT modifiers, UINT virtualKey)
{
    if (modifiers == m_captureModifiers && virtualKey == m_captureVirtualKey && m_registeredHotkeys.contains(HOTKEY_CAPTURE))
        return true;

    unregisterHotkey(HOTKEY_CAPTURE);
    bool ok = registerHotkey(HOTKEY_CAPTURE, modifiers, virtualKey);
    if (ok) {
        m_captureModifiers = modifiers;
        m_captureVirtualKey = virtualKey;
        return true;
    }

    bool restored = registerHotkey(HOTKEY_CAPTURE, m_captureModifiers, m_captureVirtualKey);
    if (!restored) {
        qWarning() << "[HotkeyManager] Failed to restore previous hotkey. Trying default...";
        if (registerHotkey(HOTKEY_CAPTURE, 0, VK_SNAPSHOT)) {
            m_captureModifiers = 0;
            m_captureVirtualKey = VK_SNAPSHOT;
        }
    }
    return false;
}

bool HotkeyManager::reRegisterRecordingHotkeys(UINT pauseModifiers, UINT pauseVirtualKey,
                                               UINT stopModifiers, UINT stopVirtualKey,
                                               UINT cancelModifiers, UINT cancelVirtualKey)
{
    const UINT oldPauseModifiers = m_recordingPauseModifiers;
    const UINT oldPauseVirtualKey = m_recordingPauseVirtualKey;
    const UINT oldStopModifiers = m_recordingStopModifiers;
    const UINT oldStopVirtualKey = m_recordingStopVirtualKey;
    const UINT oldCancelModifiers = m_recordingCancelModifiers;
    const UINT oldCancelVirtualKey = m_recordingCancelVirtualKey;

    unregisterHotkey(HOTKEY_RECORDING_PAUSE);
    unregisterHotkey(HOTKEY_RECORDING_STOP);
    unregisterHotkey(HOTKEY_RECORDING_CANCEL);

    bool ok = true;
    ok = registerHotkey(HOTKEY_RECORDING_PAUSE, pauseModifiers, pauseVirtualKey) && ok;
    ok = registerHotkey(HOTKEY_RECORDING_STOP, stopModifiers, stopVirtualKey) && ok;
    ok = registerHotkey(HOTKEY_RECORDING_CANCEL, cancelModifiers, cancelVirtualKey) && ok;

    if (ok) {
        m_recordingPauseModifiers = pauseModifiers;
        m_recordingPauseVirtualKey = pauseVirtualKey;
        m_recordingStopModifiers = stopModifiers;
        m_recordingStopVirtualKey = stopVirtualKey;
        m_recordingCancelModifiers = cancelModifiers;
        m_recordingCancelVirtualKey = cancelVirtualKey;
    } else {
        unregisterHotkey(HOTKEY_RECORDING_PAUSE);
        unregisterHotkey(HOTKEY_RECORDING_STOP);
        unregisterHotkey(HOTKEY_RECORDING_CANCEL);
        registerHotkey(HOTKEY_RECORDING_PAUSE, oldPauseModifiers, oldPauseVirtualKey);
        registerHotkey(HOTKEY_RECORDING_STOP, oldStopModifiers, oldStopVirtualKey);
        registerHotkey(HOTKEY_RECORDING_CANCEL, oldCancelModifiers, oldCancelVirtualKey);
    }
    return ok;
}

bool HotkeyManager::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
    Q_UNUSED(result);
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG") {
        MSG *msg = static_cast<MSG*>(message);
        if (msg->message == WM_HOTKEY) {
            int id = static_cast<int>(msg->wParam);
            emit hotkeyTriggered(id);
            if (id == HOTKEY_CAPTURE) emit captureRequested();
            else if (id == HOTKEY_RECORDING_PAUSE) emit recordingPauseRequested();
            else if (id == HOTKEY_RECORDING_STOP) emit recordingStopRequested();
            else if (id == HOTKEY_RECORDING_CANCEL) emit recordingCancelRequested();
            return true;
        }
    }
    return false;
}

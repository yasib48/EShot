#ifndef HOTKEYMANAGER_H
#define HOTKEYMANAGER_H

#include <QObject>
#include <QAbstractNativeEventFilter>
#include <QList>

#if defined(Q_OS_WIN) || defined(_WIN32)
#include <windows.h>
#endif

class HotkeyManager : public QObject, public QAbstractNativeEventFilter {
    Q_OBJECT

public:
    static HotkeyManager& instance();
    bool registerHotkey(int id, UINT modifiers, UINT virtualKey);
    void unregisterHotkey(int id);
    void unregisterAllHotkeys();
    bool reRegisterCaptureHotkey(UINT modifiers, UINT virtualKey);
    bool reRegisterRecordingHotkeys(UINT pauseModifiers, UINT pauseVirtualKey,
                                    UINT stopModifiers, UINT stopVirtualKey,
                                    UINT cancelModifiers, UINT cancelVirtualKey);
    UINT captureModifiers() const { return m_captureModifiers; }
    UINT captureVirtualKey() const { return m_captureVirtualKey; }
    static bool isPlainPrintScreen(UINT modifiers, UINT virtualKey);
    static bool isWindowsPrintScreenSnippingEnabled();
    static bool setWindowsPrintScreenSnippingEnabled(bool enabled);
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

signals:
    void hotkeyTriggered(int id);
    void captureRequested();
    void recordingPauseRequested();
    void recordingStopRequested();
    void recordingCancelRequested();

private:
    explicit HotkeyManager(QObject *parent = nullptr);
    ~HotkeyManager();
    HotkeyManager(const HotkeyManager&) = delete;
    HotkeyManager& operator=(const HotkeyManager&) = delete;

    QList<int> m_registeredHotkeys;
    UINT m_captureModifiers = 0;
    UINT m_captureVirtualKey = VK_SNAPSHOT;
    UINT m_recordingPauseModifiers = MOD_CONTROL | MOD_ALT;
    UINT m_recordingPauseVirtualKey = 'P';
    UINT m_recordingStopModifiers = MOD_CONTROL | MOD_ALT;
    UINT m_recordingStopVirtualKey = 'S';
    UINT m_recordingCancelModifiers = MOD_CONTROL | MOD_ALT;
    UINT m_recordingCancelVirtualKey = 'X';

public:
    static constexpr int HOTKEY_CAPTURE = 1;
    static constexpr int HOTKEY_RECORDING_PAUSE = 2;
    static constexpr int HOTKEY_RECORDING_STOP = 3;
    static constexpr int HOTKEY_RECORDING_CANCEL = 4;
};

#endif

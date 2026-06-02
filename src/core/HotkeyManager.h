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
    UINT captureModifiers() const { return m_captureModifiers; }
    UINT captureVirtualKey() const { return m_captureVirtualKey; }
    static bool isPlainPrintScreen(UINT modifiers, UINT virtualKey);
    static bool isWindowsPrintScreenSnippingEnabled();
    static bool setWindowsPrintScreenSnippingEnabled(bool enabled);
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

signals:
    void hotkeyTriggered(int id);
    void captureRequested();

private:
    explicit HotkeyManager(QObject *parent = nullptr);
    ~HotkeyManager();
    HotkeyManager(const HotkeyManager&) = delete;
    HotkeyManager& operator=(const HotkeyManager&) = delete;

    QList<int> m_registeredHotkeys;
    UINT m_captureModifiers = 0;
    UINT m_captureVirtualKey = VK_SNAPSHOT;

public:
    static constexpr int HOTKEY_CAPTURE = 1;
};

#endif

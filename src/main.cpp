#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QPixmap>
#include <QDebug>
#include <QSettings>
#include <QPalette>

#include "core/HotkeyManager.h"
#include "core/TranslationManager.h"
#include "capture/CaptureOverlay.h"
#include "ui/SettingsDialog.h"
#include "ui/AboutDialog.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

class EShotApp : public QObject {
    Q_OBJECT

public:
    EShotApp(QObject *parent = nullptr) : QObject(parent)
    {
        TranslationManager::init();
        loadSettings();
        setupTrayIcon();
        setupOverlay();
        setupHotkey();
    }

    ~EShotApp()
    {
        if (m_trayIcon) m_trayIcon->hide();
        if (m_overlay) { delete m_overlay; m_overlay = nullptr; }
    }

private slots:
    void onCaptureRequested()
    {
        if (m_overlay && m_overlay->isVisible()) return;
        if (m_overlay) m_overlay->startCapture();
    }

    void onWindowCaptureRequested()
    {
        if (m_overlay && m_overlay->isVisible()) return;
        if (m_overlay) m_overlay->startWindowCapture();
    }

    void onCaptureCompleted(const QPixmap &pixmap)
    {
        Q_UNUSED(pixmap);
        if (m_trayIcon && m_showNotifications) {
            m_trayIcon->showMessage(
                TranslationManager::notifCaptureTitle(),
                TranslationManager::notifCaptureMsg(pixmap.width(), pixmap.height()),
                QSystemTrayIcon::Information, 2000);
        }
    }

    void onCaptureCancelled() {}

    void onTrayActivated(QSystemTrayIcon::ActivationReason reason)
    {
        if (reason == QSystemTrayIcon::DoubleClick) onCaptureRequested();
    }

    void onQuitAction() { qApp->quit(); }

    void onSettingsRequested()
    {
        SettingsDialog dlg;
        if (dlg.exec() == QDialog::Accepted) {
            loadSettings();
            rebuildTrayMenu();
        }
    }

    void onAboutRequested()
    {
        AboutDialog dlg;
        dlg.exec();
    }

private:
    void loadSettings()
    {
        QSettings s("EShot", "EShot");
        m_showNotifications = s.value("showNotifications", true).toBool();
    }

    void rebuildTrayMenu()
    {
        if (!m_trayMenu) return;
        m_trayMenu->clear();

        QAction *captureAction = m_trayMenu->addAction(QIcon(":/icons/copy.svg"), TranslationManager::trayCapture());
        connect(captureAction, &QAction::triggered, this, &EShotApp::onCaptureRequested);
        QAction *windowCaptureAction = m_trayMenu->addAction(QIcon(":/icons/rectangle.svg"), TranslationManager::trayCaptureWindow());
        connect(windowCaptureAction, &QAction::triggered, this, &EShotApp::onWindowCaptureRequested);
        QAction *settingsAction = m_trayMenu->addAction(QIcon(":/icons/gear.svg"), TranslationManager::traySettings());
        connect(settingsAction, &QAction::triggered, this, &EShotApp::onSettingsRequested);
        QAction *aboutAction = m_trayMenu->addAction(QIcon(":/icons/pen.svg"), TranslationManager::trayAbout());
        connect(aboutAction, &QAction::triggered, this, &EShotApp::onAboutRequested);
        m_trayMenu->addSeparator();
        QAction *quitAction = m_trayMenu->addAction(QIcon(":/icons/close.svg"), TranslationManager::trayQuit());
        connect(quitAction, &QAction::triggered, this, &EShotApp::onQuitAction);

        m_trayIcon->setToolTip(TranslationManager::appTitle());
    }

    void setupTrayIcon()
    {
        m_trayIcon = new QSystemTrayIcon(this);
        QIcon trayIcon(":/icons/pen.svg");
        if (trayIcon.isNull()) {
            QPixmap pix(32, 32); pix.fill(Qt::blue);
            trayIcon = QIcon(pix);
        }
        m_trayIcon->setIcon(trayIcon);
        m_trayIcon->setToolTip(TranslationManager::appTitle());

        m_trayMenu = new QMenu();
        rebuildTrayMenu();

        m_trayIcon->setContextMenu(m_trayMenu);
        connect(m_trayIcon, &QSystemTrayIcon::activated, this, &EShotApp::onTrayActivated);
        m_trayIcon->show();
    }

    void setupOverlay()
    {
        m_overlay = new CaptureOverlay();
        connect(m_overlay, &CaptureOverlay::captureCompleted, this, &EShotApp::onCaptureCompleted);
        connect(m_overlay, &CaptureOverlay::captureCancelled, this, &EShotApp::onCaptureCancelled);
    }

    void setupHotkey()
    {
        connect(&HotkeyManager::instance(), &HotkeyManager::captureRequested,
                this, &EShotApp::onCaptureRequested);
    }

    QSystemTrayIcon *m_trayIcon = nullptr;
    QMenu *m_trayMenu = nullptr;
    CaptureOverlay *m_overlay = nullptr;
    bool m_showNotifications = true;
};

#include "main.moc"

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
#endif
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QApplication app(argc, argv);
    app.setApplicationName("EShot");
    app.setApplicationVersion("2.0.0");
    app.setOrganizationName("EShot");
    app.setQuitOnLastWindowClosed(false);
    app.setStyle("Fusion");
    app.setWindowIcon(QIcon(":/icons/pen.svg"));

    QSettings settings("EShot", "EShot");
    if (settings.value("darkMode", true).toBool()) {
        QPalette p;
        p.setColor(QPalette::Window, QColor(53,53,53));
        p.setColor(QPalette::WindowText, Qt::white);
        p.setColor(QPalette::Base, QColor(42,42,42));
        p.setColor(QPalette::AlternateBase, QColor(66,66,66));
        p.setColor(QPalette::ToolTipBase, QColor(53,53,53));
        p.setColor(QPalette::ToolTipText, Qt::white);
        p.setColor(QPalette::Text, Qt::white);
        p.setColor(QPalette::Button, QColor(53,53,53));
        p.setColor(QPalette::ButtonText, Qt::white);
        p.setColor(QPalette::BrightText, Qt::red);
        p.setColor(QPalette::Link, QColor(42,130,218));
        p.setColor(QPalette::Highlight, QColor(42,130,218));
        p.setColor(QPalette::HighlightedText, Qt::black);
        app.setPalette(p);
    }

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qWarning() << "[EShot] System tray is not available!";
        return 1;
    }

    EShotApp eshotApp;
    return app.exec();
}

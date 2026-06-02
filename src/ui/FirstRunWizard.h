#ifndef FIRSTRUNWIZARD_H
#define FIRSTRUNWIZARD_H

#include <QDialog>
#include <QLabel>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

class QComboBox;
class QKeySequenceEdit;
class QLineEdit;
class QPushButton;

class FirstRunWizard : public QDialog {
    Q_OBJECT

public:
    explicit FirstRunWizard(QWidget *parent = nullptr);
    ~FirstRunWizard();

    static bool shouldShow();

private slots:
    void onBrowse();
    void onFinish();
    void onHotkeyChanged();
    void onDisableWindowsPrintScreenSnipping();

private:
    void setupUi();
    void loadDefaults();
    void updatePrintScreenConflictUi();
    static bool keySequenceToWin32(const QKeySequence &seq, UINT &modifiers, UINT &vkey);

    QComboBox *m_langCombo = nullptr;
    QKeySequenceEdit *m_hotkeyEdit = nullptr;
    QLabel *m_hotkeyStatusLabel = nullptr;
    QLabel *m_printScreenConflictLabel = nullptr;
    QPushButton *m_printScreenFixButton = nullptr;
    QLineEdit *m_savePathEdit = nullptr;
};

#endif

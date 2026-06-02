#ifndef FIRSTRUNWIZARD_H
#define FIRSTRUNWIZARD_H

#include <QDialog>
#include <QStackedWidget>

class QComboBox;
class QKeySequenceEdit;
class QLineEdit;

class FirstRunWizard : public QDialog {
    Q_OBJECT

public:
    explicit FirstRunWizard(QWidget *parent = nullptr);
    ~FirstRunWizard();

    static bool shouldShow();

private slots:
    void onNext();
    void onBack();
    void onFinish();

private:
    void setupPages();
    void showPage(int index);

    int m_currentPage;
    QStackedWidget *m_stack;

    // Page 1: Language
    QComboBox *m_langCombo;

    // Page 2: Shortcut
    QKeySequenceEdit *m_hotkeyEdit;

    // Page 3: Save path
    QLineEdit *m_savePathEdit;
};

#endif

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

    // Sayfa 1: Dil
    QComboBox *m_langCombo;

    // Sayfa 2: Kısayol
    QKeySequenceEdit *m_hotkeyEdit;

    // Sayfa 3: Kayıt yolu
    QLineEdit *m_savePathEdit;
};

#endif

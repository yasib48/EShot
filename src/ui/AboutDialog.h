#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

class QLabel;
class QPushButton;
class QNetworkAccessManager;
class QNetworkReply;

class AboutDialog : public QDialog {
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

private slots:
    void onCheckForUpdates();
    void onUpdateReplyFinished(QNetworkReply *reply);

private:
    void setupUI();
    void setUpdateStatus(const QString &text, const QString &color);

    QLabel *m_updateStatusLabel = nullptr;
    QPushButton *m_checkUpdateBtn = nullptr;
    QNetworkAccessManager *m_updateManager = nullptr;
};

#endif // ABOUTDIALOG_H

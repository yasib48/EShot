#ifndef IMGURUPLOADER_H
#define IMGURUPLOADER_H

#include <QObject>
#include <QPixmap>
#include <QString>

class QNetworkAccessManager;

class ImgurUploader : public QObject {
    Q_OBJECT

public:
    explicit ImgurUploader(QObject *parent = nullptr);
    ~ImgurUploader();

    void upload(const QPixmap &pixmap);
    bool isUploading() const { return m_uploading; }

signals:
    void uploadFinished(const QString &url);
    void uploadError(const QString &error);

private slots:
    void onReplyFinished();

private:
    QNetworkAccessManager *m_networkManager;
    bool m_uploading;
};

#endif

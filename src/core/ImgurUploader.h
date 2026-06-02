#ifndef IMGURUPLOADER_H
#define IMGURUPLOADER_H

#include "ImageUploader.h"

class QNetworkReply;
class QHttpMultiPart;

class ImgurUploader : public ImageUploader {
    Q_OBJECT

public:
    explicit ImgurUploader(QObject *parent = nullptr);
    ~ImgurUploader() override;

    Provider provider() const override { return Provider::Imgur; }
    QString providerDisplayName() const override;
    bool needsAuth() const override { return true; }

    void setClientId(const QString &id);
    QString clientId() const { return m_clientId; }

    void upload() override;
    void cancel() override;

private:
    QString m_clientId;
    QNetworkReply *m_reply = nullptr;
    QHttpMultiPart *m_multipart = nullptr;
};

ImageUploader *createImgurUploader(QObject *parent = nullptr);

#endif

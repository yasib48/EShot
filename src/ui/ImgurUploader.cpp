#include "ImgurUploader.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QBuffer>
#include <QDebug>

ImgurUploader::ImgurUploader(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_uploading(false)
{
}

ImgurUploader::~ImgurUploader() {}

void ImgurUploader::upload(const QPixmap &pixmap)
{
    if (m_uploading) return;
    m_uploading = true;

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG");
    buffer.close();

    QJsonObject payload;
    payload["image"] = QString(ba.toBase64());
    payload["type"] = "base64";

    QUrl url(QStringLiteral("https://api.imgur.com/3/image"));
    QNetworkRequest req(url);
    req.setRawHeader("Authorization", "Client-ID 546c25a59c58ad7");
    req.setRawHeader("Content-Type", "application/json");

    QByteArray body = QJsonDocument(payload).toJson(QJsonDocument::Compact);
    QNetworkReply *reply = m_networkManager->post(req, body);
    connect(reply, &QNetworkReply::finished, this, &ImgurUploader::onReplyFinished);
}

void ImgurUploader::onReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) { m_uploading = false; return; }

    reply->deleteLater();
    m_uploading = false;

    if (reply->error() != QNetworkReply::NoError) {
        emit uploadError(reply->errorString());
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject root = doc.object();

    if (root["success"].toBool()) {
        QJsonObject data = root["data"].toObject();
        QString link = data["link"].toString();
        if (!link.isEmpty()) {
            emit uploadFinished(link);
        } else {
            emit uploadError("No link in response");
        }
    } else {
        emit uploadError(root["data"].toObject()["error"].toString());
    }
}

#ifndef OCRENGINE_H
#define OCRENGINE_H

#include <QObject>
#include <QString>
#include <QPixmap>
#include <QSet>

class QProcess;

class OcrEngine : public QObject {
    Q_OBJECT

public:
    explicit OcrEngine(QObject *parent = nullptr);
    ~OcrEngine() override;

    void recognize(const QPixmap &pixmap, const QString &languageTag = "en-US");

    static QString tesseractPath();
    static QString tessdataDir();
    static QString mapLanguageTag(const QString &bcp47);

signals:
    void textReady(const QString &text);
    void failed(const QString &reason);

private:
    QProcess *m_proc = nullptr;
    QSet<QString> m_pendingFiles;
};

#endif

#ifndef OCRENGINE_H
#define OCRENGINE_H

#include <QObject>
#include <QPixmap>
#include <QString>

class OCREngine : public QObject {
    Q_OBJECT

public:
    explicit OCREngine(QObject *parent = nullptr);
    ~OCREngine();

    void extractText(const QPixmap &pixmap);
    bool isProcessing() const { return m_processing; }

signals:
    void textExtracted(const QString &text);
    void ocrError(const QString &error);

private:
    bool m_processing;
};

#endif

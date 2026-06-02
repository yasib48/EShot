#ifndef GIFENCODER_H
#define GIFENCODER_H

#include <QObject>
#include <QString>
#include <QImage>
#include <QFile>
#include <QByteArray>
#include <QVector>

class GifEncoder : public QObject {
public:
    explicit GifEncoder(QObject *parent = nullptr);
    ~GifEncoder();

    bool open(const QString &path, int width, int height, int loopCount = 0);
    bool addFrame(const QImage &image, int delayCs = 10);
    bool close();

    bool isOpen() const { return m_fileOpen; }
    QString errorString() const { return m_lastError; }

private:
    struct Color { quint8 r, g, b; };

    bool writeHeader();
    bool writeLogicalScreenDescriptor(int width, int height);
    bool writeNetscapeLoopExtension(int loopCount);
    bool writeGraphicControlExtension(int delayCs);
    bool writeImageDescriptor(int left, int top, int width, int height);
    bool writeIndexedImageData(const QByteArray &indices);
    bool writeDataBlock(QByteArray &block);

    bool writeByte(quint8 b);
    bool writeBytes(const char *data, qsizetype size);
    bool writeShort(quint16 v);

    QByteArray quantizeToPalette(const QImage &image, QVector<Color> &palette) const;
    bool writeColorTable(const QVector<Color> &palette);

    QFile m_file;
    QString m_path;
    int m_width = 0;
    int m_height = 0;
    bool m_fileOpen = false;
    QString m_lastError;
};

#endif

#ifndef RECORDINGINDICATOR_H
#define RECORDINGINDICATOR_H

#include <QWidget>
#include <QRect>
#include <QTimer>

class RecordingIndicator : public QWidget {
    Q_OBJECT

public:
    explicit RecordingIndicator(const QRect &captureRect, QWidget *parent = nullptr);
    ~RecordingIndicator() override;

    void setFrameCount(int count);
    void setRemainingSeconds(int seconds);
    void stop();

signals:
    void stopRequested();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QString badgeText() const;
    void updateInteractiveRegion();

    QRect m_captureRect;
    QRect m_borderRect;
    QRect m_badgeRect;
    QRect m_stopRect;
    bool m_badgeAbove = true;
    QTimer *m_blinkTimer;
    bool m_visible = true;
    int m_frameCount = 0;
    int m_remainingSeconds = -1;
    bool m_running = true;
};

#endif

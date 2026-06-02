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

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QRect m_captureRect;
    QRect m_borderRect;
    bool m_badgeAbove = true;
    QTimer *m_blinkTimer;
    bool m_visible = true;
    int m_frameCount = 0;
    int m_remainingSeconds = -1;
    bool m_running = true;
};

#endif

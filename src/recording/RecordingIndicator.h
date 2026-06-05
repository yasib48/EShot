#ifndef RECORDINGINDICATOR_H
#define RECORDINGINDICATOR_H

#include <QWidget>
#include <QRect>
#include <QTimer>

class RecordingIndicator : public QWidget {
    Q_OBJECT

public:
    explicit RecordingIndicator(const QRect &captureRect, QWidget *parent = nullptr,
                                int borderWidth = 4, bool supportsPause = false);
    ~RecordingIndicator() override;

    void setFrameCount(int count);
    void setRemainingSeconds(int seconds);
    void setElapsedSeconds(int seconds);
    void setPaused(bool paused);
    void stop();

signals:
    void stopRequested();
    void pauseRequested();
    void resumeRequested();
    void cancelRequested();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QString badgeText() const;
    void updateInteractiveRegion();

    QRect m_captureRect;
    QRect m_borderRect;
    QRect m_badgeRect;
    QRect m_pauseRect;
    QRect m_stopRect;
    QRect m_cancelRect;
    bool m_badgeAbove = true;
    bool m_controlsInside = false;
    QTimer *m_blinkTimer;
    bool m_visible = true;
    int m_frameCount = 0;
    int m_remainingSeconds = -1;
    int m_elapsedSeconds = 0;
    bool m_running = true;
    bool m_supportsPause = false;
    bool m_paused = false;
    int m_borderWidth = 4;
    bool m_dragging = false;
    QPoint m_dragOffset;
};

#endif

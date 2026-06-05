#include "RecordingIndicator.h"
#include "../core/TranslationManager.h"
#include <QGuiApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QRegion>
#include <QScreen>

#ifdef Q_OS_WIN
#include <windows.h>
#ifndef WDA_EXCLUDEFROMCAPTURE
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#endif
#endif

RecordingIndicator::RecordingIndicator(const QRect &captureRect, QWidget *parent, int borderWidth, bool supportsPause)
    : QWidget(parent), m_captureRect(captureRect), m_supportsPause(supportsPause), m_borderWidth(borderWidth)
{
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    constexpr int pad = 6;
    constexpr int badgeHeight = 28;
    constexpr int badgeGap = 6;
    QRect screenRect = QGuiApplication::primaryScreen() ? QGuiApplication::primaryScreen()->geometry() : captureRect;
    if (QScreen *screen = QGuiApplication::screenAt(captureRect.center()))
        screenRect = screen->geometry();

    const bool fillsScreen = captureRect.left() <= screenRect.left() + 2
        && captureRect.top() <= screenRect.top() + 2
        && captureRect.right() >= screenRect.right() - 2
        && captureRect.bottom() >= screenRect.bottom() - 2;
    m_controlsInside = fillsScreen;
    m_badgeAbove = !m_controlsInside && captureRect.top() - badgeHeight - badgeGap - pad >= screenRect.top();
    QRect windowRect = m_controlsInside
        ? captureRect.adjusted(-pad, -pad, pad, pad)
        : captureRect.adjusted(-pad, m_badgeAbove ? -(badgeHeight + badgeGap + pad) : -pad,
                               pad, m_badgeAbove ? pad : badgeHeight + badgeGap + pad);
    setGeometry(windowRect);
    m_borderRect = QRect(captureRect.topLeft() - windowRect.topLeft(), captureRect.size()).adjusted(-2, -2, 2, 2);

    m_blinkTimer = nullptr;
    updateInteractiveRegion();

    show();
    raise();
#ifdef Q_OS_WIN
    SetWindowDisplayAffinity(reinterpret_cast<HWND>(winId()), WDA_EXCLUDEFROMCAPTURE);
#endif
}

RecordingIndicator::~RecordingIndicator()
{
    if (m_blinkTimer) m_blinkTimer->stop();
}

void RecordingIndicator::setFrameCount(int count)
{
    m_frameCount = count;
    updateInteractiveRegion();
    update();
}

void RecordingIndicator::setRemainingSeconds(int seconds)
{
    m_remainingSeconds = seconds;
    updateInteractiveRegion();
    update();
}

void RecordingIndicator::setElapsedSeconds(int seconds)
{
    m_elapsedSeconds = qMax(0, seconds);
    updateInteractiveRegion();
    update();
}

void RecordingIndicator::setPaused(bool paused)
{
    m_paused = paused;
    updateInteractiveRegion();
    update();
}

void RecordingIndicator::stop()
{
    m_running = false;
    if (m_blinkTimer) m_blinkTimer->stop();
    close();
}

QString RecordingIndicator::badgeText() const
{
    if (m_supportsPause) {
        int min = m_elapsedSeconds / 60;
        int sec = m_elapsedSeconds % 60;
        return QStringLiteral("\u25CF REC %1:%2")
            .arg(min, 2, 10, QChar('0'))
            .arg(sec, 2, 10, QChar('0'));
    }
    if (m_remainingSeconds >= 0) {
        int min = m_remainingSeconds / 60;
        int sec = m_remainingSeconds % 60;
        return QStringLiteral("\u25CF REC %1:%2")
            .arg(min, 2, 10, QChar('0'))
            .arg(sec, 2, 10, QChar('0'));
    }
    return QStringLiteral("\u25CF REC %1").arg(m_frameCount);
}

void RecordingIndicator::updateInteractiveRegion()
{
    QFont f = font();
    f.setBold(true);
    f.setPointSize(10);
    QFontMetrics fm(f);

    const int badgeWidth = fm.horizontalAdvance(badgeText()) + 20;
    const int pauseWidth = m_supportsPause ? fm.horizontalAdvance(m_paused ? QStringLiteral("RESUME") : QStringLiteral("PAUSE")) + 18 : 0;
    const int stopWidth = fm.horizontalAdvance(QStringLiteral("STOP")) + 18;
    const QString cancelText = TranslationManager::recordingCancel().toUpper();
    const int cancelWidth = m_supportsPause ? fm.horizontalAdvance(cancelText) + 18 : 0;
    const int bx = m_borderRect.left() + 4;
    const int by = m_controlsInside ? m_borderRect.top() + 4 : (m_badgeAbove ? 2 : m_borderRect.bottom() + 6);
    m_badgeRect = QRect(bx, by, badgeWidth, 24);
    int x = m_badgeRect.right() + 6;
    if (m_supportsPause) {
        m_pauseRect = QRect(x, by, pauseWidth, 24);
        x = m_pauseRect.right() + 6;
    } else {
        m_pauseRect = QRect();
    }
    m_stopRect = QRect(x, by, stopWidth, 24);
    if (m_supportsPause) {
        m_cancelRect = QRect(m_stopRect.right() + 6, by, cancelWidth, 24);
    } else {
        m_cancelRect = QRect();
    }

    QRegion region;
    const int border = qMax(6, m_borderWidth + 4);
    region += QRect(m_borderRect.left() - 2, m_borderRect.top() - 2, m_borderRect.width() + 4, border);
    region += QRect(m_borderRect.left() - 2, m_borderRect.bottom() - border + 3, m_borderRect.width() + 4, border);
    region += QRect(m_borderRect.left() - 2, m_borderRect.top() - 2, border, m_borderRect.height() + 4);
    region += QRect(m_borderRect.right() - border + 3, m_borderRect.top() - 2, border, m_borderRect.height() + 4);
    region += m_badgeRect.adjusted(-2, -2, 2, 2);
    if (m_supportsPause)
        region += m_pauseRect.adjusted(-2, -2, 2, 2);
    region += m_stopRect.adjusted(-2, -2, 2, 2);
    if (m_supportsPause)
        region += m_cancelRect.adjusted(-2, -2, 2, 2);
    setMask(region);
}

void RecordingIndicator::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QColor color = m_supportsPause ? QColor(45, 145, 255, 225) : QColor(220, 30, 30, 230);
    QPen pen(color, m_borderWidth);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    p.drawRect(m_borderRect);

    if (m_running) {
        p.setPen(QColor(255, 255, 255));
        QFont f = p.font();
        f.setBold(true);
        f.setPointSize(10);
        p.setFont(f);
        const QString text = badgeText();
        p.setBrush(m_paused ? QColor(80, 80, 80, 220) : color);
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(m_badgeRect, 4, 4);
        if (m_supportsPause) {
            p.setBrush(QColor(45, 45, 45, 225));
            p.drawRoundedRect(m_pauseRect, 4, 4);
        }
        p.setBrush(QColor(45, 45, 45, 225));
        p.drawRoundedRect(m_stopRect, 4, 4);
        if (m_supportsPause) {
            p.setBrush(QColor(75, 35, 35, 225));
            p.drawRoundedRect(m_cancelRect, 4, 4);
        }
        p.setPen(Qt::white);
        p.drawText(m_badgeRect, Qt::AlignCenter, text);
        if (m_supportsPause)
            p.drawText(m_pauseRect, Qt::AlignCenter, m_paused ? QStringLiteral("RESUME") : QStringLiteral("PAUSE"));
        p.drawText(m_stopRect, Qt::AlignCenter, QStringLiteral("STOP"));
        if (m_supportsPause)
            p.drawText(m_cancelRect, Qt::AlignCenter, TranslationManager::recordingCancel().toUpper());
    }
}

void RecordingIndicator::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_supportsPause && m_pauseRect.contains(event->pos())) {
        if (m_paused) emit resumeRequested();
        else emit pauseRequested();
        event->accept();
        return;
    }
    if (event->button() == Qt::LeftButton && m_stopRect.contains(event->pos())) {
        emit stopRequested();
        event->accept();
        return;
    }
    if (event->button() == Qt::LeftButton && m_supportsPause && m_cancelRect.contains(event->pos())) {
        emit cancelRequested();
        event->accept();
        return;
    }
    if (event->button() == Qt::LeftButton && m_supportsPause && m_paused) {
        m_dragging = true;
        m_dragOffset = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void RecordingIndicator::mouseMoveEvent(QMouseEvent *event)
{
    if (m_supportsPause && m_paused && m_dragging) {
        move(event->globalPosition().toPoint() - m_dragOffset);
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void RecordingIndicator::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_dragging && event->button() == Qt::LeftButton) {
        m_dragging = false;
        event->accept();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

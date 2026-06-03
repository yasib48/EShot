#include "RecordingIndicator.h"
#include <QGuiApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QRegion>
#include <QScreen>

RecordingIndicator::RecordingIndicator(const QRect &captureRect, QWidget *parent)
    : QWidget(parent), m_captureRect(captureRect)
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

    m_badgeAbove = captureRect.top() - badgeHeight - badgeGap - pad >= screenRect.top();
    QRect windowRect = captureRect.adjusted(-pad, m_badgeAbove ? -(badgeHeight + badgeGap + pad) : -pad,
                                            pad, m_badgeAbove ? pad : badgeHeight + badgeGap + pad);
    setGeometry(windowRect);
    m_borderRect = QRect(captureRect.topLeft() - windowRect.topLeft(), captureRect.size()).adjusted(-2, -2, 2, 2);

    m_blinkTimer = nullptr;
    updateInteractiveRegion();

    show();
    raise();
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

void RecordingIndicator::stop()
{
    m_running = false;
    if (m_blinkTimer) m_blinkTimer->stop();
    close();
}

QString RecordingIndicator::badgeText() const
{
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
    const int stopWidth = fm.horizontalAdvance(QStringLiteral("STOP")) + 18;
    const int bx = m_borderRect.left() + 4;
    const int by = m_badgeAbove ? 2 : m_borderRect.bottom() + 6;
    m_badgeRect = QRect(bx, by, badgeWidth, 24);
    m_stopRect = QRect(m_badgeRect.right() + 6, by, stopWidth, 24);

    QRegion region;
    const int border = 8;
    region += QRect(m_borderRect.left() - 2, m_borderRect.top() - 2, m_borderRect.width() + 4, border);
    region += QRect(m_borderRect.left() - 2, m_borderRect.bottom() - border + 3, m_borderRect.width() + 4, border);
    region += QRect(m_borderRect.left() - 2, m_borderRect.top() - 2, border, m_borderRect.height() + 4);
    region += QRect(m_borderRect.right() - border + 3, m_borderRect.top() - 2, border, m_borderRect.height() + 4);
    region += m_badgeRect.adjusted(-2, -2, 2, 2);
    region += m_stopRect.adjusted(-2, -2, 2, 2);
    setMask(region);
}

void RecordingIndicator::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QColor color(220, 30, 30, 230);
    QPen pen(color, 4);
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
        p.setBrush(QColor(220, 30, 30, 200));
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(m_badgeRect, 4, 4);
        p.setBrush(QColor(45, 45, 45, 225));
        p.drawRoundedRect(m_stopRect, 4, 4);
        p.setPen(Qt::white);
        p.drawText(m_badgeRect, Qt::AlignCenter, text);
        p.drawText(m_stopRect, Qt::AlignCenter, QStringLiteral("STOP"));
    }
}

void RecordingIndicator::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_stopRect.contains(event->pos())) {
        emit stopRequested();
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

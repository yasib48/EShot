#include "RecordingIndicator.h"
#include <QGuiApplication>
#include <QPainter>
#include <QPen>
#include <QScreen>

RecordingIndicator::RecordingIndicator(const QRect &captureRect, QWidget *parent)
    : QWidget(parent), m_captureRect(captureRect)
{
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::WindowTransparentForInput);
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
    update();
}

void RecordingIndicator::setRemainingSeconds(int seconds)
{
    m_remainingSeconds = seconds;
    update();
}

void RecordingIndicator::stop()
{
    m_running = false;
    if (m_blinkTimer) m_blinkTimer->stop();
    close();
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
        QString text;
        if (m_remainingSeconds >= 0) {
            int min = m_remainingSeconds / 60;
            int sec = m_remainingSeconds % 60;
            text = QStringLiteral("\u25CF REC %1:%2")
                .arg(min, 2, 10, QChar('0'))
                .arg(sec, 2, 10, QChar('0'));
        } else {
            text = QStringLiteral("\u25CF REC %1").arg(m_frameCount);
        }
        QFontMetrics fm(f);
        int tw = fm.horizontalAdvance(text) + 20;
        int bx = m_borderRect.left() + 4;
        int by = m_badgeAbove ? 2 : m_borderRect.bottom() + 6;
        QRect badgeRect(bx, by, tw, 24);
        p.setBrush(QColor(220, 30, 30, 200));
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(badgeRect, 4, 4);
        p.setPen(Qt::white);
        p.drawText(badgeRect, Qt::AlignCenter, text);
    }
}

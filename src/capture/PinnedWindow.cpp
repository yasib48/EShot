#include "PinnedWindow.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QGuiApplication>
#include <QDebug>
#include "../core/TranslationManager.h"

static constexpr int BAR_HEIGHT = 32;
static constexpr int CLOSE_BTN_SIZE = 20;

PinnedWindow::PinnedWindow(const QPixmap &pixmap, const QPoint &screenPos, QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool)
    , m_pixmap(pixmap)
    , m_dragging(false)
    , m_hovered(false)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMouseTracking(true);
    setCursor(Qt::SizeAllCursor);

    setFixedSize(m_pixmap.width() + 2, m_pixmap.height() + BAR_HEIGHT + 2);
    move(screenPos);
    show();

    qDebug() << "[PinnedWindow] Created at" << screenPos << "size:" << m_pixmap.size();
}

PinnedWindow::~PinnedWindow()
{
    qDebug() << "[PinnedWindow] Destroyed";
}

QRect PinnedWindow::closeButtonRect() const
{
    int x = width() - CLOSE_BTN_SIZE - 8;
    int y = (BAR_HEIGHT - CLOSE_BTN_SIZE) / 2 + 1;
    return QRect(x, y, CLOSE_BTN_SIZE, CLOSE_BTN_SIZE);
}

void PinnedWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 1. Şeffaf arka plan
    painter.fillRect(rect(), Qt::transparent);

    // 2. Pixmap (en alta)
    painter.drawPixmap(1, BAR_HEIGHT + 1, m_pixmap);

    // 3. Üst bar (her zaman görünür, yuvarlak köşeli)
    QPainterPath barPath;
    barPath.addRoundedRect(QRectF(1, 1, width() - 2, BAR_HEIGHT), 6, 6);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(20, 20, 20, 210));
    painter.drawPath(barPath);

    // 4. Hover: kapatma butonu ve pin ikonu
    if (m_hovered) {
        // Kapatma butonu
        QRect closeRect = closeButtonRect();
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(70, 70, 70, 200));
        painter.drawRoundedRect(closeRect, 3, 3);

        // X işareti
        painter.setPen(QPen(QColor(200, 200, 200), 1.5));
        int m = 5;
        painter.drawLine(closeRect.left() + m, closeRect.top() + m,
                         closeRect.right() - m, closeRect.bottom() - m);
        painter.drawLine(closeRect.right() - m, closeRect.top() + m,
                         closeRect.left() + m, closeRect.bottom() - m);

        // Pin ikonu (daire + çizgi)
        int iconX = 14;
        int iconY = BAR_HEIGHT / 2;
        painter.setPen(QPen(QColor(0, 122, 204), 1.5));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(QPoint(iconX, iconY - 2), 3, 3);
        painter.drawLine(iconX, iconY + 1, iconX, iconY + 7);

        // "Pinned" yazısı
        painter.setPen(QColor(180, 180, 180));
        QFont f("Segoe UI", 9);
        f.setBold(true);
        painter.setFont(f);
        painter.drawText(iconX + 8, 0, width() - iconX - 8 - CLOSE_BTN_SIZE - 14, BAR_HEIGHT,
                         Qt::AlignVCenter, TranslationManager::pinnedLabel());
    }

    // 5. Border (her zaman en üstte)
    QPainterPath borderPath;
    borderPath.addRoundedRect(QRectF(1, 1, width() - 2, height() - 2), 7, 7);
    painter.setPen(QPen(QColor(0, 122, 204), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(borderPath);
}

void PinnedWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_hovered && closeButtonRect().contains(event->pos())) {
            close();
            return;
        }
        m_dragging = true;
        m_dragOffset = event->pos();
    }
}

void PinnedWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging) {
        move(event->globalPosition().toPoint() - m_dragOffset);
    }
}

void PinnedWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
    }
}

void PinnedWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}

void PinnedWindow::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    m_hovered = true;
    update();
}

void PinnedWindow::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_hovered = false;
    update();
}

void PinnedWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    QAction *copyAction = menu.addAction(TranslationManager::pinnedCopy());
    connect(copyAction, &QAction::triggered, [this]() {
        QGuiApplication::clipboard()->setPixmap(m_pixmap);
    });

    menu.addSeparator();

    QAction *closeAction = menu.addAction(TranslationManager::pinnedClose());
    connect(closeAction, &QAction::triggered, this, &QWidget::close);

    menu.exec(event->globalPos());
}

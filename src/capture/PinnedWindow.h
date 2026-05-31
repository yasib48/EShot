#ifndef PINNEDWINDOW_H
#define PINNEDWINDOW_H

#include <QWidget>
#include <QPixmap>
#include <QPoint>
#include <QMenu>

class PinnedWindow : public QWidget {
    Q_OBJECT

public:
    explicit PinnedWindow(const QPixmap &pixmap, const QPoint &screenPos, QWidget *parent = nullptr);
    ~PinnedWindow();

    QPixmap pixmap() const { return m_pixmap; }
    void setOpacity(double opacity);
    double opacity() const { return m_opacity; }

signals:
    void closed(PinnedWindow *window);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    QPixmap m_pixmap;       // Orijinal görsel (asla değişmez)
    double m_scale;         // Ölçek çarpanı (1.0 = orijinal boyut)
    QPoint m_dragOffset;
    bool m_dragging;
    bool m_hovered;
    double m_opacity;

    void updateWindowSize();

    // Yeniden boyutlandırma
    enum ResizeHandle { NoHandle, TopLeft, TopRight, BottomLeft, BottomRight };
    ResizeHandle m_resizeHandle;
    QRect handleRect(ResizeHandle handle) const;
    ResizeHandle getResizeHandle(const QPoint &pos) const;
    QPoint m_resizeStartGlobal;
    QSize m_resizeStartSize;

    // Kapatma butonu alanı
    QRect closeButtonRect() const;
};

#endif

#ifndef ZOOMABLEGRAPHICSVIEW_H
#define ZOOMABLEGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

class ZoomableGraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    explicit ZoomableGraphicsView(QWidget *parent = nullptr) : QGraphicsView(parent), zoomFactor(1.0) {}

protected:
    void wheelEvent(QWheelEvent *event) override {
        if (event->modifiers() & Qt::ControlModifier) {
            // Zoom in/out si se mantiene presionado Ctrl
            double scaleFactor = 1.15;
            if (event->angleDelta().y() > 0) {
                scale(scaleFactor, scaleFactor); // Zoom in
                zoomFactor *= scaleFactor;
            } else {
                scale(1.0 / scaleFactor, 1.0 / scaleFactor); // Zoom out
                zoomFactor /= scaleFactor;
            }
        } else {
            // Si no se presiona Ctrl, scroll normal
            QGraphicsView::wheelEvent(event);
        }
    }

    void keyPressEvent(QKeyEvent *event) override {
        // Reiniciar zoom con una tecla, por ejemplo R
        if (event->key() == Qt::Key_R) {
            resetTransform();
            zoomFactor = 1.0;
        } else {
            QGraphicsView::keyPressEvent(event);
        }
    }

private:
    double zoomFactor;
};

#endif // ZOOMABLEGRAPHICSVIEW_H

#include "cgraphicsscene.h"

/*!
    \class CGraphicsScene

    CGraphicsScene is extended from QGraphicsScene, provides view port event and enter leave event
    handlers.

    You're supposed to set CGraphicsScene instances as the scenes of CGraphicsView so that
    these events can be dispatched.
*/
CGraphicsScene::CGraphicsScene(QObject *parent) : QGraphicsScene(parent) {
}

/*!
    Constructs using the scene rect specified.
*/
CGraphicsScene::CGraphicsScene(const QRectF &sceneRect, QObject *parent)
    : QGraphicsScene(sceneRect, parent) {
}

/*!
    Constructs using the rectangle specified by (x, y), and the given width and height
    for its scene rectangle.
*/
CGraphicsScene::CGraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject *parent)
    : QGraphicsScene(x, y, width, height, parent) {
}

/*!
    Destructor.
*/
CGraphicsScene::~CGraphicsScene() {
}

bool CGraphicsScene::event(QEvent *event) {
    switch (event->type()) {
        case QEvent::GraphicsSceneMove:
            viewMoveEvent(static_cast<QGraphicsSceneMoveEvent *>(event));
            return event->isAccepted();
            break;
        case QEvent::GraphicsSceneResize:
            viewResizeEvent(static_cast<QGraphicsSceneResizeEvent *>(event));
            return event->isAccepted();
            break;

        case QEvent::Enter:
            enterEvent(static_cast<QEnterEvent *>(event));
            return event->isAccepted();
            break;
        case QEvent::Leave:
            leaveEvent(event);
            return event->isAccepted();
            break;
        default:
            break;
    }
    return QGraphicsScene::event(event);
}

/*!
    A scene resize event is sent when the graphics view port moves.
*/
void CGraphicsScene::viewMoveEvent(QGraphicsSceneMoveEvent *event) {
    Q_UNUSED(event)
}

/*!
    A scene resize event is sent when the graphics view port resizes.
*/
void CGraphicsScene::viewResizeEvent(QGraphicsSceneResizeEvent *event) {
    Q_UNUSED(event)
}

/*!
    An enter event is sent when the mouse cursor enters the scene.
*/
void CGraphicsScene::enterEvent(QEnterEvent *event) {
    Q_UNUSED(event)
}

/*!
    A leave event is sent when the mouse cursor leaves the widget.
*/
void CGraphicsScene::leaveEvent(QEvent *event) {
    Q_UNUSED(event)
}

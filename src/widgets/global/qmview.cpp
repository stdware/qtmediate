#include "qmview.h"

#include <QWindow>
#include <QGuiApplication>
#include <QPainter>

/*!
    \namespace QMView
    \brief Namespace of Qt graphics utilities.
*/

namespace QMView {

    /*!
       Creates a pixmap whose resolution and pixel ratio is suitable for rendering to screen.

       \param logicalPixelSize Desired size of pixmap, in OS logical pixels
       \param window Pointer to QWindow whose DPR is taken. If it's nullptr, use qApp's DPR instead

       \return pixmap created
     */
    QPixmap createPixmap(const QSize &logicalPixelSize, QWindow *window) {
#ifndef Q_OS_MACOS
        qreal targetDPR;
        if (window)
            targetDPR = window->devicePixelRatio();
        else
            targetDPR = qApp->devicePixelRatio();
        QPixmap ret(logicalPixelSize * targetDPR);
        ret.setDevicePixelRatio(targetDPR);
        return ret;
#else
        Q_UNUSED(window);
        return QPixmap(logicalPixelSize);
#endif
    }

}
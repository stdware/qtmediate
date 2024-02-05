#ifndef QMVIEW_H
#define QMVIEW_H

#include <QWindow>
#include <QPixmap>

#include <QMWidgets/qmwidgetsglobal.h>

namespace QMView {

    QM_WIDGETS_EXPORT QPixmap createPixmap(const QSize &logicalPixelSize, QWindow *window = nullptr);

    QM_WIDGETS_EXPORT void centralizeWindow(QWidget *w, QSizeF ratio = QSizeF(-1, -1));

}

#endif // QMVIEW_H

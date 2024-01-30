#ifndef QMPAINTACCESSOR_H
#define QMPAINTACCESSOR_H

#include <QBrush>
#include <QPen>

#include <QMWidgets/qmwidgetsglobal.h>

class QMPaintAccessorPrivate;

class QM_WIDGETS_EXPORT QMPaintAccessor {
    Q_GADGET
    Q_DECLARE_PRIVATE(QMPaintAccessor)
public:
    QMPaintAccessor();
    virtual ~QMPaintAccessor();

    using PaintProc = std::function<void(QPainter *)>;

    enum PaintItem {
        PI_Rect = 0x1,
        PI_Line = 0x2,
        PI_Ellipse = 0x4,
        PI_Path = 0x8,
        PI_Point = 0x10,
        PI_Polygon = 0x20,
        PI_Text = 0x40,
    };
    Q_ENUM(PaintItem)
    Q_DECLARE_FLAGS(PaintItems, PaintItem)

public:
    virtual QPen queryPen(const PaintProc &proc, PaintItems items);
    virtual QBrush queryBrush(const PaintProc &proc, PaintItems items);

protected:
    QMPaintAccessor(QMPaintAccessorPrivate &d);

    QScopedPointer<QMPaintAccessorPrivate> d_ptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QMPaintAccessor::PaintItems);

#endif // QMPAINTACCESSOR_H

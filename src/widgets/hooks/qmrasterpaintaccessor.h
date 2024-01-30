#ifndef QMRASTERPAINTACCESSOR_H
#define QMRASTERPAINTACCESSOR_H

#include <QImage>

#include <QMWidgets/qmpaintaccessor.h>

class QMRasterPaintAccessorPrivate;

class QM_WIDGETS_EXPORT QMRasterPaintAccessor : public QMPaintAccessor {
    Q_GADGET
    Q_DECLARE_PRIVATE(QMRasterPaintAccessor)
public:
    explicit QMRasterPaintAccessor(const QImage &image);
    ~QMRasterPaintAccessor();

public:
    QPen queryPen(const PaintProc &proc, PaintItems items) override;
    QBrush queryBrush(const PaintProc &proc, PaintItems items) override;
};

#endif // QMRASTERPAINTACCESSOR_H

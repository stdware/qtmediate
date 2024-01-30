#ifndef QPENINFO_H
#define QPENINFO_H

#include <QPen>
#include <QSharedData>

#include <QMCore/qmnamespace.h>
#include <QMWidgets/qmwidgetsglobal.h>

class QPenInfoData;

class QM_WIDGETS_EXPORT QPenInfo : public QPen {
public:
    QPenInfo();
    QPenInfo(Qt::PenStyle style);
    QPenInfo(const QColor &color);
    QPenInfo(const QBrush &brush, qreal width, Qt::PenStyle s = Qt::SolidLine, Qt::PenCapStyle c = Qt::SquareCap,
             Qt::PenJoinStyle j = Qt::BevelJoin);
    ~QPenInfo();

    QPenInfo(const QPenInfo &other);
    QPenInfo(QPenInfo &&other) noexcept;

    QPenInfo &operator=(const QPenInfo &other);
    QPenInfo &operator=(QPenInfo &&other) noexcept;

    QPen toPen(QM::ButtonState state = QM::ButtonNormal) const;

    QBrush brush(QM::ButtonState state = QM::ButtonNormal) const;
    void setBrush(const QBrush &brush, QM::ButtonState state = QM::ButtonNormal);
    void setBrushes(const QList<QBrush> &brushes);

public:
    static QPenInfo fromStringList(const QStringList &stringList);

    static const char *metaFunctionName();

    QM_WIDGETS_EXPORT friend QDebug operator<<(QDebug debug, const QPenInfo &info);

private:
    QSharedDataPointer<QPenInfoData> d;
};

Q_DECLARE_METATYPE(QPenInfo)

#endif // QPENINFO_H

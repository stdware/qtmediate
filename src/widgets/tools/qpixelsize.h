#ifndef QPIXELSIZE_H
#define QPIXELSIZE_H

#include <QMetaType>
#include <QString>

#include <QMWidgets/qmwidgetsglobal.h>

class QM_WIDGETS_EXPORT QPixelSize {
public:
    QPixelSize();
    QPixelSize(int pixel);
    QPixelSize(double pixel);
    ~QPixelSize();

public:
    QString toString() const;

    int value() const;
    void setValue(int value);

    double valueF() const;
    void setValueF(double value);

    operator int() const;
    operator double() const;

public:
    static QPixelSize fromString(const QString &string);

    friend QM_WIDGETS_EXPORT QDebug operator<<(QDebug debug, const QPixelSize &pixel);

private:
    double m_value;
};

Q_DECLARE_METATYPE(QPixelSize)

#endif // QPIXELSIZE_H

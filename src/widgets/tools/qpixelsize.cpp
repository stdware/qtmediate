#include "qpixelsize.h"

#include <QDebug>

/*!
    \class QPixelSize
    \brief QPixelSize is a wrapper of a number, mainly used to receive a size value from QMetaType
           system.
*/

/*!
    Constructor, the default avalue is 0.
*/
QPixelSize::QPixelSize() : m_value(0) {
}

/*!
    Constructs from \c int value.
*/
QPixelSize::QPixelSize(int pixel) : m_value(pixel) {
}

/*!
    Constructs from \c double value.
*/
QPixelSize::QPixelSize(double pixel) : m_value(pixel) {
}

/*!
    Destructor.
*/
QPixelSize::~QPixelSize() {
}

/*!
    Returns the value as string.
*/
QString QPixelSize::toString() const {
    return QString("%1%2").arg(QString::number(m_value), "px");
}

/*!
    Returns the wrapped value.
*/
int QPixelSize::value() const {
    return m_value;
}

/*!
    Sets the wrapped value.
*/
void QPixelSize::setValue(int value) {
    m_value = value;
}

/*!
    Returns the wrapped value in floating precision.
*/
double QPixelSize::valueF() const {
    return m_value;
}

/*!
    Sets the wrapped value in floating precision.
*/
void QPixelSize::setValueF(double value) {
    m_value = value;
}

QPixelSize::operator int() const {
    return m_value;
}

QPixelSize::operator double() const {
    return m_value;
}

/*!
    Converts a QString to QPixelSize.

    The string can be suffixed with \c px unit.

    \sa QMCssType::parse()
*/
QPixelSize QPixelSize::fromString(const QString &string) {
    QString str;
    if (string.endsWith("px", Qt::CaseInsensitive)) {
        str = string.chopped(2);
    } else {
        str = string;
    }

    QPixelSize res;
    bool isNum;
    auto num = str.toDouble(&isNum);
    if (isNum) {
        res.m_value = num;
    }
    return res;
}

QDebug operator<<(QDebug debug, const QPixelSize &pixel) {
    QDebugStateSaver saver(debug);
    debug.nospace() << pixel.toString();
    return debug;
}

namespace {
    struct initializer {
        static int intConverter(const QPixelSize &size) {
            return size.value();
        }

        static double doubleConverter(const QPixelSize &size) {
            return size.valueF();
        }

        initializer() {
            QMetaType::registerConverter<QPixelSize, int>(intConverter);
            QMetaType::registerConverter<QPixelSize, double>(doubleConverter);
            QMetaType::registerConverter<QString, QPixelSize>(QPixelSize::fromString);
        }
        ~initializer() {
            QMetaType::unregisterConverterFunction(qMetaTypeId<QPixelSize>(), qMetaTypeId<int>());
            QMetaType::unregisterConverterFunction(qMetaTypeId<QPixelSize>(),
                                                   qMetaTypeId<double>());
            QMetaType::unregisterConverterFunction(qMetaTypeId<QString>(),
                                                   qMetaTypeId<QPixelSize>());
        }
    } dummy;
}
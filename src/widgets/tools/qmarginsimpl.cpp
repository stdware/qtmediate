#include "QMarginsImpl.h"

#include <QDebug>

#include "qmcss_p.h"
#include "qpixelsize.h"

/*!
    \namespace QMarginsImpl
    \brief The namespace contains functions that extend QMargins type to support QMetaType
           conversion.
*/

namespace QMarginsImpl {

    /*!
        Converts a string list to QMargins, the string list should be the form as
        <tt>["qmargins", "..."]</tt>.

        \sa QMCssType
    */
    QMargins fromStringList(const QStringList &stringList) {
        QMargins res;
        if (stringList.size() == 2 &&
            stringList.front().compare(metaFunctionName(), Qt::CaseInsensitive) == 0) {
            auto x = QMCss::parseSizeValueList(stringList.back().simplified());
            if (x.size() == 4) {
                // qmargins(left, top, right, bottom)
                res = QMargins(x.at(0), x.at(1), x.at(2), x.at(3));
            } else if (x.size() >= 2) {
                // qmargins(v, h)
                res = QMargins(x.at(1), x.at(0), x.at(1), x.at(0));
            } else if (!x.isEmpty()) {
                // qmargins(i)
                res = QMargins(x.front(), x.front(), x.front(), x.front());
            }
        }
        return res;
    }

    /*!
        Converts a string to QMargins.
    */
    QMargins fromString(const QString &s) {
        QPixelSize px = QPixelSize::fromString(s);
        int value = px.value();
        return {value, value, value, value};
    }

    /*!
        QMargins identifier when converting from a string representing as function call.

        \sa QMCssType::parse()
    */
    const char *metaFunctionName() {
        return "qmargins";
    }

    namespace {
        struct initializer {
            initializer() {
                QMetaType::registerConverter<QStringList, QMargins>(QMarginsImpl::fromStringList);
                QMetaType::registerConverter<QString, QMargins>(QMarginsImpl::fromString);

                QMCssType::registerMetaTypeName(qMetaTypeId<QMargins>(),
                                                QMarginsImpl::metaFunctionName());
            }
            ~initializer() {
                QMCssType::unregisterConverterFunction<QStringList, QMargins>();
                QMCssType::unregisterConverterFunction<QString, QMargins>();

                QMCssType::unregisterMetaTypeName(qMetaTypeId<QMargins>());
            }
        } dummy;
    }

}
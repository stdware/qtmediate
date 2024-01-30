#include "qcssvaluelist.h"

#include <QDebug>

#include "qmcss_p.h"

/*!
    \class QCssValueList
    \brief QCssValueList is a wrapper of QVariantList.
*/

/*!
    \fn QCssValueList::QCssValueList()

    Default constructor.
*/

/*!
    \fn QCssValueList::QCssValueList(std::initializer_list<QVariant> args)

    Constructs from an initializer list.
*/

/*!
    \fn QCssValueList::QCssValueList(const QVariantList &list)

    Constructs from a QVariantList.
*/

/*!
    \fn QCssValueList::QCssValueList(InputIterator first, InputIterator last)

    Constructs from a pair of iterator.
*/

/*!
    \fn QCssValueList::~QCssValueList()

    Destructor.
*/

/*!
    \fn QVariantList &QCssValueList::get()

    Returns the reference of the wrapped container.
*/

/*!
    \fn const QVariantList &QCssValueList::get() const

    Returns the const reference of the wrapped container.
*/

/*!
    Converts a string list to QCssValueList, the string list should be the form as
    <tt>["qlist", "..."]</tt>.

    \sa QMCssType::parse()
*/
QCssValueList QCssValueList::fromStringList(const QStringList &stringList) {
    if (stringList.size() != 2 ||
        stringList.front().compare(metaFunctionName(), Qt::CaseInsensitive) != 0) {
        return {};
    }
    const auto &strData = stringList.at(1);

    QCssValueList res;
    QStringList valueList = QMCss::parseStringValueList(strData);
    for (const auto &item : valueList) {
        res.get().append(QMCssType::parse(item.trimmed()));
    }

    return res;
}

/*!
    QCssValueList identifier when converting from a string representing as function call.

    \sa QMCssType::parse()
*/
const char *QCssValueList::metaFunctionName() {
    return "qlist";
}

QDebug operator<<(QDebug debug, const QCssValueList &list) {
    debug << list.get();
    return debug;
}

namespace {
    struct initializer {
        initializer() {
            QMetaType::registerConverter<QStringList, QCssValueList>(QCssValueList::fromStringList);
            QMCssType::registerMetaTypeName(qMetaTypeId<QCssValueList>(),
                                            QCssValueList::metaFunctionName());
        }
        ~initializer() {
            QMetaType::unregisterConverterFunction(qMetaTypeId<QStringList>(),
                                                   qMetaTypeId<QCssValueList>());
            QMCssType::unregisterMetaTypeName(qMetaTypeId<QCssValueList>());
        }
    } dummy;
}
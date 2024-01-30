#ifndef QMBATCH_H
#define QMBATCH_H

#include <QJsonArray>
#include <QPoint>
#include <QStringList>

#include <QMCore/qmglobal.h>

namespace QM {

    QM_CORE_EXPORT QString strUnescape(const QString &s);

    QM_CORE_EXPORT QString strRemoveSideQuote(const QString &token, bool unescape = false);

    QM_CORE_EXPORT QList<int> strListToIntList(const QStringList &list);

    QM_CORE_EXPORT QList<double> strListToDoubleList(const QStringList &list);

    QM_CORE_EXPORT QStringList jsonArrayToStrList(const QJsonArray &arr, bool considerNum = false);

    QM_CORE_EXPORT bool strIsNumber(const QString &s, bool considerDot, bool considerNeg);

    inline bool strPrefixedWith(const QString &A, const QString &B) {
        return A.startsWith(B) && (A != B);
    }

    QM_CORE_EXPORT QString adjustRepeatedName(const QSet<QString> &set, const QString &name);

    template <template <class> class Array, class T>
    void arrayMoveElements(Array<T> &arr, int index, int count, int dest) {
        count = qMin(count, arr.size() - index);
        if (count <= 0 || count > arr.size() || (dest >= index && dest <= index + count)) {
            return;
        }

        decltype(typename std::remove_reference<decltype(arr)>::type()) tmp;
        tmp.resize(count);
        std::copy(arr.begin() + index, arr.begin() + index + count, tmp.begin());

        int correctDest;
        if (dest > index) {
            correctDest = dest - count;
            auto sz = correctDest - index;
            for (int i = 0; i < sz; ++i) {
                arr[index + i] = arr[index + count + i];
            }
        } else {
            correctDest = dest;
            auto sz = index - dest;
            for (int i = sz - 1; i >= 0; --i) {
                arr[dest + count + i] = arr[dest + i];
            }
        }
        std::copy(tmp.begin(), tmp.end(), arr.begin() + correctDest);
    }

    template <template <class> class Array, class T>
    void arrayInsertSort(Array<T> &array) {
        int i, j;
        T key;

        for (j = 1; j < array.size(); ++j) {
            key = array[j];
            i = j - 1;
            while (i >= 0 && key < array[i]) {
                array[i + 1] = array[i];
                i = i - 1;
            }
            array[i + 1] = key;
        }
    }

}

#endif // QMBATCH_H

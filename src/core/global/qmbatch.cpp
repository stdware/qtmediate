#include "QMBatch.h"

#include <QDebug>
#include <QSet>
#include <QtMath>

#include <cstdio>

namespace QM {

    /*!
        Unescapes the given string.
    */
    QString strUnescape(const QString &s) {
        QString res;
        auto p = s.data();
        while (!p->isNull()) {
            if (*p == '\\') {
                auto pNext = p + 1;
                if (!pNext->isNull()) {
                    res += *pNext;
                    p += 2;
                } else {
                    break;
                }
            } else {
                res += *p;
                p++;
            }
        }
        return res;
    }

    /*!
        Removes the first quote at the beginning and end of the string.

        \param unescape Unescape the result if the string is enclosed in quotation marks.
    */
    QString strRemoveSideQuote(const QString &token, bool unescape) {
        auto str = token;
        if (str.front() == '\"' && str.back() == '\"') {
            str.remove(0, 1);
            str.remove(str.size() - 1, 1);
            if (unescape) {
                str = QM::strUnescape(str);
            }
        }
        return str;
    }

    /*!
        Converts a string list to a int list, the non-number elements will be skipped.
    */
    QList<int> strListToIntList(const QStringList &list) {
        QList<int> res;
        for (const auto &item : list) {
            bool isNum;
            int num = item.toInt(&isNum);
            if (!isNum) {
                return {};
            }
            res.append(num);
        }
        return res;
    }

    /*!
        Converts a string list to a double list, the non-number elements will be skipped.
    */
    QList<double> strListToDoubleList(const QStringList &list) {
        QList<double> res;
        for (const auto &it : list) {
            bool isNum;
            int num = it.toDouble(&isNum);
            if (!isNum) {
                return {};
            }
            res.append(num);
        }
        return res;
    }

    /*!
        Converts a json array to a double list, the non-number elements will be skipped.
    */
    QStringList jsonArrayToStrList(const QJsonArray &arr, bool considerNum) {
        QStringList res;
        for (const auto &item : arr)
            if (item.isString())
                res.append(item.toString());
            else if (item.isDouble() && considerNum)
                res.append(QString::number(item.toDouble()));
        return res;
    }

    /*!
        Returns \c true if a string can be converted to a number, otherwise returns false.

        \param considerDot If set, the dot will be considered as a number char
        \param considerNeg If set, the minus sign will be considered as a number char
    */
    bool strIsNumber(const QString &s, bool considerDot, bool considerNeg) {
        bool flag = true;

        for (int i = 0; i < s.size(); ++i) {
            QChar ch = s.at(i);
            if ((ch >= '0' && ch <= '9') || (considerDot && ch == '.') ||
                (considerNeg && ch == '-')) {
                // is Number
            } else {
                flag = false;
                break;
            }
        }

        return flag;
    }

    /*!
        \fn bool strPrefixedWith(const QString &A, const QString &B)

        Returns \c true if \c A is strictly prefied with \c B .
    */

    /*!
        Returns the next available name in the given string collection even if there's a
        duplication.
    */
    QString adjustRepeatedName(const QSet<QString> &set, const QString &name) {
        if (!set.contains(name)) {
            return name;
        }

        QString body;
        int num;

        bool failed = false;

        // Check if there's a formatted suffix
        int index = name.lastIndexOf(" (");
        if (index >= 0) {
            QString suffix = name.mid(index);

            // Get suffix index
            const char fmt[] = " (%d)%c";
            char ch;
            int n = ::sscanf(suffix.toUtf8().data(), fmt, &num, &ch);
            if (n != 1) {
                failed = true;
            } else {
                body = name.left(index);
            }
        } else {
            failed = true;
        }

        if (failed) {
            body = name;
            num = 0;
        }

        QString newName;
        while (true) {
            num++;
            newName = body + QString(" (%1)").arg(QString::number(num));
            if (!set.contains(newName)) {
                break;
            }
        };

        return newName;
    }

    /*!
        \fn void arrayMoveElements(Array<T> &arr, int index, int count, int dest)

        Moves elements in the given array, the \c dest should be the position after move.
    */

    /*!
        \fn void arrayInsertSort(Array<T> &array)

        Sorts the array using insert sort algorithm.
    */

}
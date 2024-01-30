#ifndef QMSVGX_H
#define QMSVGX_H

#include <QHash>
#include <QIcon>
#include <QStringList>

#include <QMCore/qmnamespace.h>
#include <QMWidgets/qmwidgetsglobal.h>

namespace QMSvgx {

    class QM_WIDGETS_EXPORT Icon {
    public:
        Icon();
        explicit Icon(QIcon *icon);
        ~Icon();

    public:
        bool isValid() const;

        QM::ButtonState currentState() const;
        void setCurrentState(QM::ButtonState state);

        QString color(QM::ButtonState state) const;
        void setColorHint(const QString &color);

    public:
        static QIcon create(const QHash<QM::ButtonState, QString> &fileMap,
                            const QHash<QM::ButtonState, QString> &colorMap);

        static QIcon create(const QString &file, const QString &checkedFile,
                            const QString &color = QStringLiteral("auto"));

    private:
        QIconEngine *m_engine;
    };

}

#endif // QMSVGX_H

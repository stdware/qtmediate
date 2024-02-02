#include "qmsvgx_p.h"

#include <QIconEngine>

#include <private/qicon_p.h>

#include "qmcss_p.h"

namespace QMPrivate {

    static const char *buttonStateValueToKey(QM::ButtonState state) {
        const char *key = nullptr;
        switch (state) {
            case QM::ButtonNormal:
                key = "up";
                break;
            case QM::ButtonHover:
                key = "hover";
                break;
            case QM::ButtonPressed:
                key = "down";
                break;
            case QM::ButtonDisabled:
                key = "disabled";
                break;
            case QM::ButtonNormalChecked:
                key = "up2";
                break;
            case QM::ButtonHoverChecked:
                key = "hover2";
                break;
            case QM::ButtonPressedChecked:
                key = "down2";
                break;
            case QM::ButtonDisabledChecked:
                key = "disabled2";
                break;
            default:
                Q_UNREACHABLE();
        }
        return key;
    }

    static QHash<QM::ButtonState, QString> parseStates(const QString &statesStr) {
        QString arr[8];
        if (!QMCss::parseButtonStateList(statesStr, arr, false))
            return {};

        QHash<QM::ButtonState, QString> res;
        for (int i = 0; i < 8; ++i) {
            auto state = static_cast<QM::ButtonState>(i);
            if (arr[i].isEmpty())
                continue;
            res.insert(state, arr[i]);
        }
        return res;
    };

    QString serializeSvgxArgs(const QHash<QM::ButtonState, QString> &fileMap,
                              const QHash<QM::ButtonState, QString> &colorMap) {
        QStringList fileArgs;
        for (auto it = fileMap.begin(); it != fileMap.end(); ++it) {
            fileArgs.append(
                QString("%1=\"%2\"").arg(buttonStateValueToKey(it.key())).arg(it.value()));
        }
        QStringList colorArgs;
        for (auto it = fileMap.begin(); it != fileMap.end(); ++it) {
            fileArgs.append(
                QString("%1=\"%2\"").arg(buttonStateValueToKey(it.key())).arg(it.value()));
        }
        return QString("[[(%1), (%2)]].svgx").arg(fileArgs.join(", "), colorArgs.join(", "));
    }

    bool deserializeSvgxArgs(const QString &s, QHash<QM::ButtonState, QString> *fileMap,
                             QHash<QM::ButtonState, QString> *colorMap) {
        if (!s.endsWith(".svgx", Qt::CaseInsensitive)) {
            return false;
        }

        QString content = s.mid(0, s.size() - 4);
        if (!content.startsWith("[[") || !content.endsWith("]]")) {
            return false;
        }
        content = content.mid(2, content.size() - 4);

        QStringList contentList = QMCss::parseStringValueList(content);
        if (contentList.size() > 0) {
            *fileMap = parseStates(contentList.front());
        }
        if (contentList.size() > 1) {
            *colorMap = parseStates(contentList.at(1));
        }
        return true;
    }

}

/*!
    \namespace QMCss
    \brief Namespace of QtMediate svg extension utilities.
*/

namespace QMSvgx {

    static inline QIconEngine *get_engine(QIcon &icon) {
        auto d = icon.data_ptr();
        if (!d)
            return nullptr;

        auto engine = d->engine;
        if (!engine || engine->key() != "svgx") {
            return nullptr;
        }

        return engine;
    }

    Icon::Icon() : m_engine(nullptr) {
    }

    Icon::Icon(QIcon *icon) : m_engine(icon ? get_engine(*icon) : nullptr) {
    }

    Icon::~Icon() {
    }

    bool Icon::isValid() const {
        return m_engine != nullptr;
    }

    /*!
        Gets the icon's internal state.
     */
    QM::ButtonState Icon::currentState() const {
        if (!m_engine)
            return {};

        QM::ButtonState res;
        void *a[] = {
            &res,
        };
        m_engine->virtual_hook(QMPrivate::GetState, a);
        return res;
    }

    /*!
        Updates the icon's internal state.
     */
    void Icon::setCurrentState(QM::ButtonState state) {
        if (!m_engine)
            return;

        void *a[] = {
            &state,
        };
        m_engine->virtual_hook(QMPrivate::SetState, a);
    }

    /*!
        Gets the icon's internal color of a given state.
     */
    QString Icon::color(QM::ButtonState state) const {
        if (!m_engine)
            return {};

        QString res;
        void *a[] = {
            &state,
            &res,
        };
        m_engine->virtual_hook(QMPrivate::GetColor, a);
        return res;
    }

    /*!
        Updates the icon's internal color of its current state.
     */
    void Icon::setColorHint(const QString &color) {
        if (!m_engine)
            return;

        void *a[] = {
            const_cast<QString *>(&color),
        };
        m_engine->virtual_hook(QMPrivate::SetColor, a);
    }

    /*!
        Creates a QIcon with multiple images and colors in different button states.
     */
    QIcon Icon::create(const QHash<QM::ButtonState, QString> &fileMap,
                       const QHash<QM::ButtonState, QString> &colorMap) {
        QIcon icon(".svgx");
        auto engine = get_engine(icon);
        if (!engine)
            return {};

        void *a[] = {
            const_cast<QHash<QM::ButtonState, QString> *>(&fileMap),
            const_cast<QHash<QM::ButtonState, QString> *>(&colorMap),
        };
        engine->virtual_hook(QMPrivate::Create, a);
        return icon;
    }

    /*!
        Creates a QIcon with images in normal and normal-checked states and a color.
     */
    QIcon Icon::create(const QString &file, const QString &checkedFile, const QString &color) {
        if (checkedFile.isEmpty()) {
            return create(
                {
                    {QM::ButtonNormal, file}
            },
                {{QM::ButtonNormal, color}});
        }
        return create(
            {
                {QM::ButtonNormal,        file       },
                {QM::ButtonNormalChecked, checkedFile}
        },
            {{QM::ButtonNormal, color}});
    }

}
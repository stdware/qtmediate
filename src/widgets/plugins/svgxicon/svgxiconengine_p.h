#ifndef SVGXICONENGINE_P_H
#define SVGXICONENGINE_P_H

#include <QSvgRenderer>
#include <QSharedData>
#include <QIcon>

#include <QMWidgets/private/qmbuttonstate_p.h>

class SvgxIconEnginePrivate : public QSharedData {
public:
    SvgxIconEnginePrivate() : currentState(QM::ButtonNormal) {
    }

    ~SvgxIconEnginePrivate() {
    }

    QString pmcKey(const QSize &size, QIcon::Mode mode, QIcon::State state);

    void stepSerialNum() {
        serialNum = lastSerialNum.fetchAndAddRelaxed(1);
    }

    QIcon::Mode loadDataForModeAndState(QSvgRenderer *renderer, QIcon::Mode mode,
                                        QIcon::State state);

    int serialNum;
    static QAtomicInt lastSerialNum;

    // Extensions
    void setup(const QHash<QM::ButtonState, QString> &fileMap,
               const QHash<QM::ButtonState, QString> &colorMap);
    void syncData();

    struct SvgScript {
        QString fileName;
        QString data;
        bool hasCurrentColor;

        SvgScript(const QString &fileName = {}) : fileName(fileName), hasCurrentColor(false) {
        }
    };
    QMButtonAttributes<SvgScript> svgScripts;
    QMButtonAttributes<QString> svgColors;

    QM::ButtonState currentState;
    QString colorHint;
};

#endif // SVGXICONENGINE_P_H

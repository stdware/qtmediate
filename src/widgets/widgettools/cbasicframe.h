#ifndef CBASICFRAME_H
#define CBASICFRAME_H

#include <QtWidgets/QFrame>
#include <QtWidgets/QLayout>

#include <QMWidgets/qmwidgetsmacros.h>

class QM_WIDGETS_EXPORT CBasicFrame : public QFrame {
    Q_OBJECT
    Q_LAYOUT_PROPERTY_DELCARE
public:
    explicit CBasicFrame(QWidget *parent = nullptr);
    ~CBasicFrame();
};

#endif // CBASICFRAME_H

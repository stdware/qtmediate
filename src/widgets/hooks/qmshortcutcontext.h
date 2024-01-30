#ifndef QMSHORTCUTCONTEXT_H
#define QMSHORTCUTCONTEXT_H

#include <QWidget>

#include <QMWidgets/qmwidgetsglobal.h>

class QMShortcutContextPrivate;

class QM_WIDGETS_EXPORT QMShortcutContext : public QObject {
    Q_OBJECT
public:
    explicit QMShortcutContext(QObject *parent = nullptr);
    ~QMShortcutContext();

public:
    void addWidget(QWidget *w, int priority);
    void removeWidget(QWidget *w);
    QList<QWidget *> widgets() const;
    bool containsWidget(QWidget *w) const;

private:
    QMShortcutContextPrivate *d;
};

#endif // QMSHORTCUTCONTEXT_H

#ifndef QMAPPEXTENSION_H
#define QMAPPEXTENSION_H

#include <QKeyEvent>

#include <QMCore/QMCoreAppExtension.h>
#include <QMWidgets/QMWidgetsGlobal.h>

#ifdef qAppExt
#  undef qAppExt
#endif
#define qAppExt qobject_cast<QMAppExtension *>(QMCoreAppExtension::instance())

class QMAppExtensionPrivate;

class QM_WIDGETS_EXPORT QMAppExtension : public QMCoreAppExtension {
    Q_OBJECT
    Q_DECLARE_PRIVATE(QMAppExtension)
public:
    explicit QMAppExtension(QObject *parent = nullptr);
    ~QMAppExtension();

    void showMessage(QObject *parent, MessageBoxFlag flag, const QString &title,
                     const QString &text) const override;

    static void forwardShortcut(QKeyEvent *event, QWidget *window);

protected:
    QMAppExtension(QMAppExtensionPrivate &d, QObject *parent = nullptr);
};

#endif // QMAPPEXTENSION_H

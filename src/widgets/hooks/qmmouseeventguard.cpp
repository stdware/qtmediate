#include "qmmouseeventguard.h"

#include <QDebug>
#include <QGuiApplication>
#include <QMetaObject>
#include <QMouseEvent>
#include <QWidget>

namespace {

    struct Slot {
        QByteArray name;
        std::function<void(QMouseEvent *)> func;

        Slot(const QByteArray &name) : name(name) {
        }

        Slot(const decltype(func) &func) : func(func) {
        }

        void invoke(QWidget *w, QMouseEvent *e) {
            if (!name.isEmpty()) {
                QMetaObject::invokeMethod(w, name, Qt::DirectConnection, Q_ARG(QMouseEvent *, e));
                return;
            }

            if (func) {
                func(e);
            }
        }
    };

    class ButtonHandler : public QObject {
    public:
        Qt::MouseButton b;
        QWidget *w;
        bool pressed;
        Slot slot;

        ButtonHandler(Qt::MouseButton b, const char *slot, QWidget *w)
            : b(b), slot(slot), w(w), pressed(false) {
            w->installEventFilter(this);
        }

        ButtonHandler(Qt::MouseButton b, const std::function<void(QMouseEvent *)> &slot, QWidget *w)
            : b(b), slot(slot), w(w), pressed(false) {
            w->installEventFilter(this);
        }

        inline void call(QMouseEvent *e) {
            slot.invoke(w, e);
        }

        void start(QMouseEvent *e) {
            pressed = true;
            qApp->installEventFilter(this);
            call(e);
        }

        void release(QMouseEvent *e) {
            pressed = false;
            qApp->removeEventFilter(this);
            call(e);
        }

        void abandon(QMouseEvent *e) {
            pressed = false;
            qApp->removeEventFilter(this);
            QMouseEvent e2(QEvent::MouseButtonRelease, w->mapFromGlobal(e->globalPos()), b,
                           Qt::NoButton, e->modifiers());
            call(&e2);
        }

        bool eventFilter(QObject *obj, QEvent *event) override {
            switch (event->type()) {
                case QEvent::MouseButtonPress: {
                    auto e = static_cast<QMouseEvent *>(event);
                    if (obj == w) {
                        if (pressed) {
                            // Same button pressed twice without release
                            abandon(e);
                        } else if (e->button() == b) {
                            start(e);
                        }
                    } else {
                        // Unexpected press encountered
                        abandon(e);
                    }
                    break;
                }
                case QEvent::MouseMove: {
                    auto e = static_cast<QMouseEvent *>(event);
                    if (pressed) {
                        if (e->buttons() == Qt::NoButton) {
                            // Unexpected move encountered
                            abandon(e);
                        } else if ((e->buttons() & b)) {
                            call(e);
                        }
                    }
                    break;
                }
                case QEvent::MouseButtonRelease: {
                    auto e = static_cast<QMouseEvent *>(event);
                    if (pressed) {
                        abandon(e);
                    }
                    break;
                }
                default:
                    break;
            }

            return QObject::eventFilter(obj, event);
        }
    };

}

class QMMouseEventGuardPrivate {
public:
    QMMouseEventGuardPrivate(QWidget *w) : w(w) {
    }

    ~QMMouseEventGuardPrivate() {
        qDeleteAll(buttons);
    }

    void addButton(ButtonHandler *handler) {
        const auto &button = handler->b;
        auto it = buttons.find(button);
        if (it != buttons.end()) {
            delete it.value();
            it.value() = handler;
            return;
        }
        buttons.insert(button, handler);
    }

    QWidget *w;
    QHash<Qt::MouseButton, ButtonHandler *> buttons;
};

QMMouseEventGuard::QMMouseEventGuard(QWidget *parent)
    : QObject(parent), d(new QMMouseEventGuardPrivate(parent)) {
}

QMMouseEventGuard::~QMMouseEventGuard() {
    delete d;
}

void QMMouseEventGuard::addButton(Qt::MouseButton button, const char *slot) {
    d->addButton(new ButtonHandler(button, slot, d->w));
}

void QMMouseEventGuard::addButton(Qt::MouseButton button,
                                  const std::function<void(QMouseEvent *)> &slot) {
    d->addButton(new ButtonHandler(button, slot, d->w));
}

void QMMouseEventGuard::removeButton(Qt::MouseButton button) {
    auto it = d->buttons.find(button);
    if (it == d->buttons.end())
        return;
    delete it.value();
    d->buttons.erase(it);
}
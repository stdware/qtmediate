#include "qmappextension.h"
#include "qmappextension_p.h"

#ifdef _WIN32
#  include <Windows.h>
#  define OS_MAX_PATH MAX_PATH
#else
#  include <dlfcn.h>
#  include <limits.h>
#  include <stdio.h>
#  include <string.h>

#  define OS_MAX_PATH PATH_MAX
#endif

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QFontDatabase>

#include <private/qapplication_p.h>
#include <private/qshortcutmap_p.h>

#include <QMCore/qmsystem.h>

#include "qmdecoratorv2.h"

static QString GetLibraryPath() {
#ifdef _WIN32
    wchar_t buf[OS_MAX_PATH + 1] = {0};
    HMODULE hm = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCWSTR) &GetLibraryPath, &hm) ||
        !GetModuleFileNameW(hm, buf, sizeof(buf))) {
        return {};
    }
    return QDir::fromNativeSeparators(QString::fromStdWString(buf));
#else
    Dl_info dl_info;
    dladdr((void *) GetLibraryPath, &dl_info);
    auto buf = dl_info.dli_fname;
    return QString::fromStdString(buf);
#endif
}

QMAppExtensionPrivate::QMAppExtensionPrivate() {
}

QMAppExtensionPrivate::~QMAppExtensionPrivate() {
}

void QMAppExtensionPrivate::init() {
    // This is necessary for macOS platforms, so that QIcon will return a
    // pixmap with correct devicePixelRatio when using QIcon::pixmap().
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    for (const auto &path : qAsConst(fontPaths)) {
        QDir directory(path);
        QStringList fontFiles = directory.entryList({"*.ttf", "*.otf"}, QDir::Files);
        foreach (const QString &fontFile, fontFiles) {
            QString fontFilePath = directory.absoluteFilePath(fontFile);
            int fontId = QFontDatabase::addApplicationFont(fontFilePath);
            if (fontId != -1) {
                QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
                qCDebug(qAppExtLog) << "add font families: " << fontFamilies.join(", ");
            }
        }
    }

    QFont font = QGuiApplication::font();

#ifdef Q_OS_WINDOWS
    // font.setFamily("Microsoft YaHei"); // Consider not using MSYH on non-Chinese Windows platform
#endif

    // Init font
    if (!appFont.isEmpty()) {
        QJsonValue value;
        value = appFont.value("Family");
        if (value.isString()) {
            font.setFamily(value.toString());
        }

        value = appFont.value("Size");
        if (value.isDouble()) {
            double ratio = QGuiApplication::primaryScreen()->logicalDotsPerInch() / QM::unitDpi();
            font.setPixelSize(int(value.toDouble() * ratio));
        }

        value = appFont.value("Weight");
        if (value.isDouble()) {
            font.setWeight(value.toInt());
        }

        value = appFont.value("Italic");
        if (value.isBool()) {
            font.setItalic(value.toBool());
        }
    }

    font.setStyleStrategy(QFont::PreferAntialias);
    qApp->setFont(font);

    // Add theme paths
    for (const auto &path : qAsConst(themePaths))
        qIDec->addThemePath(path);
}

QMCoreDecoratorV2 *QMAppExtensionPrivate::createDecorator(QObject *parent) {
    return new QMDecoratorV2(parent);
}

int QMAppExtensionPrivate::globalIconCacheSerialNum = 0;

/*!
    \class QMAppExtension

    The QMGuiAppExtension class is the global resources manager for \c QtMediate framework.
*/

/*
    Constructor.
*/
QMAppExtension::QMAppExtension(QObject *parent)
    : QMAppExtension(*new QMAppExtensionPrivate(), parent) {
}

/*!
    Destructor.
*/
QMAppExtension::~QMAppExtension() {
}

/*!
    Shows system message box if supported, otherwise shows Qt message box.
*/
void QMAppExtension::showMessage(QObject *parent, MessageBoxFlag flag, const QString &title,
                                 const QString &text) const {
    Q_D(const QMAppExtension);

    QWidget *w = nullptr;
    if (parent && parent->isWidgetType()) {
        w = qobject_cast<QWidget *>(parent)->window();
    }

#if defined(Q_OS_WINDOWS)
    d->osMessageBox_helper(w ? reinterpret_cast<HWND>(w->winId()) : nullptr, flag, title, text);
#elif defined(Q_OS_MAC)
    d->osMessageBox_helper(nullptr, flag, title, text);
#else
    switch (flag) {
        case Critical:
            QMessageBox::critical(w, title, text);
            break;
        case Warning:
            QMessageBox::warning(w, title, text);
            break;
        case Question:
            QMessageBox::question(w, title, text);
            break;
        case Information:
            QMessageBox::information(w, title, text);
            break;
    };
#endif
}

namespace {

    class ShortcutFilter : public QObject {
    public:
        ShortcutFilter(QWidget *org) : m_org(org), m_handled(false) {
        }

        inline bool handled() const {
            return m_handled;
        }

    protected:
        bool eventFilter(QObject *watched, QEvent *event) override {
            if (event->type() == QEvent::Shortcut) {
                QApplicationPrivate::active_window = m_org;
                m_handled = true;
            }
            return QObject::eventFilter(watched, event);
        }

    private:
        QWidget *m_org;
        bool m_handled;
    };

}

/*!
    Redirect the key event as a shortcut to the given window.
*/
void QMAppExtension::forwardShortcut(QKeyEvent *event, QWidget *window) {
    // This function hacks the QApplication data structure and simply changes
    // the `active_window` pointer temporarily to make the shortcut map transmit the
    // event to the target window.

    if (!window || !window->isWindow()) {
        return;
    }

    // Hack `active_window` temporarily
    auto org = QApplicationPrivate::active_window;
    QApplicationPrivate::active_window = window;

    // Make sure to restore `active_window` right away if shortcut matches
    ShortcutFilter filter(org);
    qApp->installEventFilter(&filter);

    // Retransmit event
    QKeyEvent keyEvent(QEvent::ShortcutOverride, event->key(), event->modifiers(),
                       event->nativeScanCode(), event->nativeVirtualKey(), event->nativeModifiers(),
                       event->text(), event->isAutoRepeat(), event->count());
    QGuiApplicationPrivate::instance()->shortcutMap.tryShortcut(&keyEvent);

    if (!filter.handled()) {
        QApplicationPrivate::active_window = org;
    }
}

/*!
    \internal
*/
QMAppExtension::QMAppExtension(QMAppExtensionPrivate &d, QObject *parent)
    : QMCoreAppExtension(d, parent) {
    d.init();
}
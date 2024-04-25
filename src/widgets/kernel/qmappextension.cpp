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

#include <qpa/qplatformfontdatabase.h>

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

    QFont font = QMAppExtension::systemDefaultFont();
    font.setPixelSize(12);

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
            font.setWeight(static_cast<QFont::Weight>(value.toInt()));
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

/*!
    Returns the system default font.
*/
QFont QMAppExtension::systemDefaultFont() {
#if defined(Q_OS_WINDOWS) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    NONCLIENTMETRICSW ncm = {0};
    ncm.cbSize = FIELD_OFFSET(NONCLIENTMETRICSW, lfMessageFont) + sizeof(LOGFONTW);
    SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
    return [](const LOGFONTW &logFont) {
        QFont qFont(QString::fromWCharArray(logFont.lfFaceName));
        qFont.setItalic(logFont.lfItalic);
        if (logFont.lfWeight != FW_DONTCARE)
            qFont.setWeight(QPlatformFontDatabase::weightFromInteger(logFont.lfWeight));
        const qreal logFontHeight = qAbs(logFont.lfHeight);
        qFont.setPixelSize(qAbs(logFont.lfHeight));
        qFont.setUnderline(logFont.lfUnderline);
        qFont.setOverline(false);
        qFont.setStrikeOut(logFont.lfStrikeOut);
        return qFont;
    }(ncm.lfMessageFont);
#else
    QFont font = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
    font.setPixelSize(font.pointSize() / 72.0 * QM::unitDpi());
    return font;
#endif
}

QFont QMAppExtension::systemDefaultFontWithDpi(double dpi) {
    QFont font = QMAppExtension::systemDefaultFont();
    double ratio =
        (dpi > 0 ? dpi : QGuiApplication::primaryScreen()->logicalDotsPerInch()) / QM::unitDpi();
    font.setPixelSize(int(12 * ratio));
    return font;
}

/*!
    \internal
*/
QMAppExtension::QMAppExtension(QMAppExtensionPrivate &d, QObject *parent)
    : QMCoreAppExtension(d, parent) {
    d.init();
}
#include "MainWindow.h"

#include <QDebug>

#include <QMWidgets/qmsvgx.h>
#include <QMWidgets/ctabbutton.h>
#include <QMWidgets/qmdecoratorv2.h>
#include <QMWidgets/qrectinfo.h>
#include <QMWidgets/qpeninfo.h>
#include <QMWidgets/qfontinfoex.h>
#include <QMWidgets/qmarginsimpl.h>

MainWindow::MainWindow() {
    auto btn = new CTabButton("ok");
    btn->setSpaceRatio(0.5);
    setCentralWidget(btn);

    btn->setStyleSheet(QMDecoratorV2::evaluateStyleSheet(R"(

CPushButton {
    --icon: svg(":/info.svg", (#FF0000, #00FF00, #0000FF));
}

    )"));

    setStyleSheet(QMDecoratorV2::evaluateStyleSheet(R"(

MainWindow {
    --styleValues: qmap(/* styleData */
    background=qrect((blue, rgba(255, 255, 255, 10%), rgba(0, 96, 192, 25%)), 3.75px), /**/
    underline=qpen(rgba(255, 255, 255, 7.5%), 1), /**/
    titleShape=qfont((#CCCCCC, white), 15px), /**/
    subShape=qfont((#909090, #CCCCCC), 15px), /**/
    descShape=qfont((#CCCCCC, white), 15px), /**/
    descHighlightShape=qfont((#3794F9, white), 15px), /**/
    descBackgroundShape=qrect(rgba(255, 255, 255, 7.5%), (2px, 2px, 2px, 0)), /**/
    titleMargins=qmargins(0, 1.25px, 10px, 1.25px), /**/
    subMargins=qmargins(0, 1.25px, 10px, 1.25px), /**/
    descMargins=qmargins(10px, 0, 0, 0), /**/
    iconMargins=qmargins(0, 0, 8px, 0), /**/
    padding=qmargins(3.75px, 12.5px), /**/
    margins=qmargins(0), /**/
    defaultIconSize=18px 18px, /**/
    );
}

    )"));

    // auto icon = QMSvgx::Icon::create(
    //     {
    //         {QM::ButtonNormal, ":/info.svg"},
    // },
    //     {
    //         {QM::ButtonNormal, "#FF0000"},
    //         {QM::ButtonHover, "#00FF00"},
    //         {QM::ButtonPressed, "#0000FF"},
    //     });
    // btn->setIcon(icon);
}

MainWindow::~MainWindow() {
}

QCssValueMap MainWindow::styleValues() const {
    return {};
}

void MainWindow::setStyleValues(const QCssValueMap &styleValues) {
    auto &map = styleValues.get();

    qDebug() << map.value("background").value<QRectInfo>();
    qDebug() << map.value("underline").value<QPenInfo>();
    qDebug() << map.value("titleShape").value<QFontInfoEx>();
    qDebug() << map.value("descMargins").value<QMargins>();
}
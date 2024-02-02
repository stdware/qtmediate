#include "MainWindow.h"

#include <QDebug>

#include <QMWidgets/qmsvgx.h>
#include <QMWidgets/cpushbutton.h>
#include <QMWidgets/qmdecoratorv2.h>

MainWindow::MainWindow() {
    auto btn = new CPushButton("ok");
    setCentralWidget(btn);

    btn->setStyleSheet(QMDecoratorV2::evaluateStyleSheet(R"(

CPushButton {
    --icon: svg(":/info.svg", (#FF0000, #00FF00, #0000FF));
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

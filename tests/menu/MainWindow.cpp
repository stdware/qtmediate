#include "MainWindow.h"

#include <QDebug>

#include <QMWidgets/qmsvgx.h>
#include <QMWidgets/cpushbutton.h>

MainWindow::MainWindow() {
    auto btn = new CPushButton("ok");
    setCentralWidget(btn);

    auto icon = QMSvgx::Icon::create(
        {
            {QM::ButtonNormal, ":/info.svg"},
    },
        {
            {QM::ButtonNormal, "#FF0000"},
            {QM::ButtonHover, "#00FF00"},
            {QM::ButtonPressed, "#0000FF"},
        });
    btn->setIcon(icon);
}

MainWindow::~MainWindow() {
}

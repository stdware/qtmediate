project(tst_menu)

set(CMAKE_AUTORCC on)
set(CMAKE_AUTOMOC on)
set(CMAKE_AUTOUIC on)

file(GLOB _src *.h *.cpp)

add_executable(${PROJECT_NAME})

qm_configure_target(${PROJECT_NAME}
    SOURCES ${_src} res.qrc
    QT_LINKS Core Gui Widgets
    LINKS ${QTMEDIATE_INSTALL_NAME}::Widgets
)
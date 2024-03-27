project(QMWidgets
    VERSION ${QTMEDIATE_VERSION}
    LANGUAGES CXX
)

# Platform libs
set(_platform_libs)

if(WIN32)
    set(_platform_libs dwmapi)
else()
    set(_platform_libs dl)
endif()

# Configure target
file(GLOB_RECURSE _src *.h *.hpp *.cpp *.cc)

qtmediate_add_library(${PROJECT_NAME} AUTOGEN
    SOURCES ${_src}
    LINKS QMCore
    LINKS_PRIVATE ${_platform_libs}
    INCLUDE_PRIVATE global hooks kernel painting tools widgets widgettools
    QT_LINKS Core Gui Widgets Svg
    QT_INCLUDE_PRIVATE Core Gui Widgets
    PREFIX QM_WIDGETS
    SYNC_INCLUDE_OPTIONS EXCLUDE "plugins/.+"
)
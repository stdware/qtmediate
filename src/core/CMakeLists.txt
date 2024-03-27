project(QMCore
    VERSION ${QTMEDIATE_VERSION}
    LANGUAGES CXX
)

# Platform libs
set(_platform_libs)

if(APPLE)
    list(APPEND _platform_libs "-framework CoreFoundation")
endif()

if(NOT WIN32)
    list(APPEND _platform_libs dl)
endif()

# Configure target
file(GLOB_RECURSE _src *.h *.hpp *.cpp *.cc)

qtmediate_add_library(${PROJECT_NAME} AUTOGEN
    SOURCES ${_src}
    LINKS_PRIVATE ${_platform_libs}
    INCLUDE_PRIVATE *
    QT_LINKS Core
    QT_INCLUDE_PRIVATE Core
    SKIP_AUTOMOC collections global text
    PREFIX QM_CORE
)

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qm_link_qt(${PROJECT_NAME} PUBLIC Core5Compat)
endif()
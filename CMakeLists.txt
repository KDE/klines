cmake_minimum_required (VERSION 3.5 FATAL_ERROR)

set (RELEASE_SERVICE_VERSION_MAJOR "20")
set (RELEASE_SERVICE_VERSION_MINOR "08")
set (RELEASE_SERVICE_VERSION_MICRO "3")
set (RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")
set (RELEASE_SERVICE_COMPACT_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}${RELEASE_SERVICE_VERSION_MINOR}${RELEASE_SERVICE_VERSION_MICRO}")

project(klines VERSION "1.6.${RELEASE_SERVICE_COMPACT_VERSION}")

set (QT_MIN_VERSION "5.9.0")
set (KF5_MIN_VERSION "5.46.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    CoreAddons
    Config
    Crash
    WidgetsAddons
    Config
    DBusAddons
    I18n
    ConfigWidgets
    XmlGui
    DocTools
)
    
find_package(KF5KDEGames 4.9.0 REQUIRED)

include(FeatureSummary)
include(ECMAddAppIcon)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(ECMSetupVersion)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(ECMQtDeclareLoggingCategory)

add_subdirectory(themes)
add_subdirectory(doc)

########### next target ###############

set(klines_SRCS 
   mwidget.cpp 
   klines.cpp 
   main.cpp
   scene.cpp
   renderer.cpp
   ballitem.cpp
   previewitem.cpp
   animator.cpp
)
ecm_setup_version(${PROJECT_VERSION} VARIABLE_PREFIX KLINES VERSION_HEADER klines_version.h)
ecm_qt_declare_logging_category(klines_SRCS HEADER klines_debug.h IDENTIFIER KLINES_LOG CATEGORY_NAME org.kde.kdegames.klines) 
qt5_add_resources(klines_SRCS klines.qrc)
kconfig_add_kcfg_files(klines_SRCS prefs.kcfgc)
file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*-apps-klines.png")
ecm_add_app_icon(klines_SRCS ICONS ${ICONS_SRCS})
add_executable(klines ${klines_SRCS})

target_link_libraries(klines
    KF5KDEGames
    KF5::Crash
    KF5::DBusAddons
    KF5::XmlGui
)

install(TARGETS klines  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install(PROGRAMS org.kde.klines.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(FILES klines.kcfg  DESTINATION  ${KDE_INSTALL_KCFGDIR})
install(FILES org.kde.klines.appdata.xml DESTINATION ${CMAKE_INSTALL_METAINFODIR})

ecm_install_icons(ICONS
    16-apps-klines.png
    22-apps-klines.png
    32-apps-klines.png
    48-apps-klines.png
    64-apps-klines.png
    128-apps-klines.png
    DESTINATION ${KDE_INSTALL_ICONDIR}
    THEME hicolor
)
install( FILES klines.categories DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR} )
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

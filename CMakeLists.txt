project(klines)

cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
set (QT_MIN_VERSION "5.7.0")
set (KF5_MIN_VERSION "5.30.0")

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
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

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

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

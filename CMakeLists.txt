project(klines)

cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
set (QT_MIN_VERSION "5.3.0")

find_package(ECM 1.3.0 REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets Qml Quick QuickWidgets Svg Test)
find_package(KF5 REQUIRED COMPONENTS 
    CoreAddons 
    Config 
    WidgetsAddons 
    Config 
    DBusAddons 
    I18n 
    GuiAddons 
    Service 
    ConfigWidgets 
    ItemViews 
    IconThemes 
    XmlGui
    KIO
    NewStuff)
    
find_package(KF5KDEGames 4.9.0 REQUIRED)

include(FeatureSummary)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
add_definitions(-DTRANSLATION_DOMAIN="klines")

add_subdirectory( themes )
add_subdirectory( doc )

########### next target ###############

set(klines_SRCS 
   mwidget.cpp 
   klines.cpp 
   main.cpp
   scene.cpp
   renderer.cpp
   ballitem.cpp
   previewitem.cpp
   animator.cpp)
   
kconfig_add_kcfg_files(klines_SRCS prefs.kcfgc )
#kde4_add_app_icon(klines_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/hi*-app-klines.png")
add_executable(klines ${klines_SRCS})

target_link_libraries(klines KF5KDEGames KF5::XmlGui KF5::DBusAddons)

install(TARGETS klines  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS org.kde.klines.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES klines.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES klinesui.rc  DESTINATION  ${KXMLGUI_INSTALL_DIR}/klines )

ecm_install_icons(ICONS
    16-apps-klines.png
    22-apps-klines.png
    32-apps-klines.png
    48-apps-klines.png
    64-apps-klines.png
    128-apps-klines.png
    DESTINATION ${ICON_INSTALL_DIR}
    THEME hicolor
)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

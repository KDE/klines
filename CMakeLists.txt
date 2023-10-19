cmake_minimum_required (VERSION 3.16 FATAL_ERROR)

set (RELEASE_SERVICE_VERSION_MAJOR "23")
set (RELEASE_SERVICE_VERSION_MINOR "11")
set (RELEASE_SERVICE_VERSION_MICRO "70")
set (RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")
set (RELEASE_SERVICE_COMPACT_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}${RELEASE_SERVICE_VERSION_MINOR}${RELEASE_SERVICE_VERSION_MICRO}")

project(klines VERSION "1.6.${RELEASE_SERVICE_COMPACT_VERSION}")

set(QT_MIN_VERSION "6.5.0")
set(KF_MIN_VERSION "5.240.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMAddAppIcon)
include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMQtDeclareLoggingCategory)
include(FeatureSummary)
include(ECMDeprecationSettings)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Widgets
)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    CoreAddons
    Config
    Crash
    WidgetsAddons
    Config
    DBusAddons
    I18n
    ConfigWidgets
    XmlGui
)
find_package(KF6DocTools ${KF_MIN_VERSION})
set_package_properties(KF6DocTools PROPERTIES DESCRIPTION
    "Tools to generate documentation"
    TYPE OPTIONAL
)

find_package(KDEGames6 6.0.0 REQUIRED)

ecm_set_disabled_deprecation_versions(
    QT 6.5
    KF 5.239
    KDEGAMES 6.0
)

add_subdirectory(themes)
add_subdirectory(icons)
add_subdirectory(src)

ki18n_install(po)
if(KF6DocTools_FOUND)
    kdoctools_install(po)
    add_subdirectory(doc)
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

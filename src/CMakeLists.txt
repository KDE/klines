ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX KLINES
    VERSION_HEADER klines_version.h
)

add_executable(klines)

target_sources(klines PRIVATE
    animator.cpp
    animator.h
    ballitem.cpp
    ballitem.h
    commondefs.h
    klines.cpp
    klines.h
    main.cpp
    mwidget.cpp
    mwidget.h
    previewitem.cpp
    previewitem.h
    renderer.cpp
    renderer.h
    scene.cpp
    scene.h


    klines.qrc
)

if(APPLE)
    set_target_properties(klines PROPERTIES
           MACOSX_BUNDLE_DISPLAY_NAME "Klines"
        MACOSX_BUNDLE_BUNDLE_NAME "Klines"
        MACOSX_BUNDLE_LONG_VERSION_STRING "Klines ${RELEASE_SERVICE_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${RELEASE_SERVICE_VERSION}"
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.kde.klines"
        MACOSX_BUNDLE_COPYRIGHT "2024 The Klines Developers")
endif()


ecm_qt_declare_logging_category(klines
    HEADER klines_debug.h
    IDENTIFIER KLINES_LOG
    CATEGORY_NAME klines
    OLD_CATEGORY_NAMES org.kde.kdegames.klines
    DESCRIPTION "KLines game"
    EXPORT KLINES
)

kconfig_add_kcfg_files(klines prefs.kcfgc)

file(GLOB ICONS_SRCS "${CMAKE_SOURCE_DIR}/icons/*-apps-klines.png")
ecm_add_app_icon(klines ICONS ${ICONS_SRCS})

target_link_libraries(klines
    KDEGames6
    KF6::CoreAddons
    KF6::Crash
    KF6::DBusAddons
    KF6::I18n
    KF6::XmlGui
    KF6::IconThemes
)

install(TARGETS klines  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(PROGRAMS org.kde.klines.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(FILES klines.kcfg  DESTINATION  ${KDE_INSTALL_KCFGDIR})
install(FILES org.kde.klines.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

ecm_qt_install_logging_categories(
    EXPORT KLINES
    FILE klines.categories
    DESTINATION "${KDE_INSTALL_LOGGINGCATEGORIESDIR}"
)

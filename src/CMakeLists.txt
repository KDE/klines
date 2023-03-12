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

ecm_qt_declare_logging_category(klines
    HEADER klines_debug.h
    IDENTIFIER KLINES_LOG
    CATEGORY_NAME org.kde.kdegames.klines
    DESCRIPTION "KLines game"
    EXPORT KLINES
)

kconfig_add_kcfg_files(klines prefs.kcfgc)

file(GLOB ICONS_SRCS "${CMAKE_SOURCE_DIR}/icons/*-apps-klines.png")
ecm_add_app_icon(klines ICONS ${ICONS_SRCS})

if (QT_MAJOR_VERSION STREQUAL "6")
    target_link_libraries(klines KDEGames6)
else()
    target_link_libraries(klines KF5KDEGames)
endif()

target_link_libraries(klines
    KF${KF_MAJOR_VERSION}::CoreAddons
    KF${KF_MAJOR_VERSION}::Crash
    KF${KF_MAJOR_VERSION}::DBusAddons
    KF${KF_MAJOR_VERSION}::XmlGui
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

project(klines)

if(NOT COMPILING_KDEGAMES)
	find_package(KDE4 REQUIRED)
	include(KDE4Defaults)
	include(MacroLibrary)
	find_package(KDEGames REQUIRED)

	add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
	add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
	include_directories(${QDBUS_INCLUDE_DIRS} ${KDE4_INCLUDES} ${KDEGAMES_INCLUDE_DIRS})
endif(NOT COMPILING_KDEGAMES)

include_directories( ${CMAKE_SOURCE_DIR}/libkdegames/highscore  )

add_subdirectory( themes )

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
   

kde4_add_kcfg_files(klines_SRCS prefs.kcfgc )

kde4_add_app_icon(klines_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/hi*-app-klines.png")
kde4_add_executable(klines ${klines_SRCS})

target_link_libraries(klines  ${KDE4_KDEUI_LIBS} kdegames )

install(TARGETS klines  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS klines.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES klines.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES klinesui.rc  DESTINATION  ${DATA_INSTALL_DIR}/klines )

kde4_install_icons( ${ICON_INSTALL_DIR}   )

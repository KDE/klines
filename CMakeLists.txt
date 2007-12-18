project(klines)
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

set(klines_icons
	hi16-app-klines.png
	hi32-app-klines.png
	hi48-app-klines.png
	hi64-app-klines.png
	hi128-app-klines.png
)
kde4_add_win32_app_icon(klines_SRCS ${klines_icons})
kde4_add_executable(klines ${klines_SRCS})

target_link_libraries(klines  ${KDE4_KDEUI_LIBS} kdegames )

install(TARGETS klines  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES klines.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES klines.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES klinesui.rc  DESTINATION  ${DATA_INSTALL_DIR}/klines )

kde4_install_icons( ${ICON_INSTALL_DIR}   )

project(klines)
include_directories( ${CMAKE_SOURCE_DIR}/libkdegames ${CMAKE_SOURCE_DIR}/libkdegames/highscore  )


########### next target ###############

set(klines_SRCS 
   prompt.cpp 
   mwidget.cpp 
   linesboard.cpp 
   field.cpp 
   cell.cpp 
   ballpainter.cpp 
   klines.cpp 
   main.cpp )

kde4_automoc(${klines_SRCS})

kde4_add_kcfg_files(klines_SRCS prefs.kcfgc )

kde4_add_executable(klines ${klines_SRCS})

target_link_libraries(klines  ${KDE4_KDEUI_LIBS} kdegames )

install(TARGETS klines  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES klines.desktop  DESTINATION  ${XDG_APPS_DIR} )
install( FILES klines.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES balls.jpg field.jpg fire.jpg  DESTINATION  ${DATA_INSTALL_DIR}/klines/ )
install( FILES klinesui.rc  DESTINATION  ${DATA_INSTALL_DIR}/klines )

kde4_install_icons( ${ICON_INSTALL_DIR}   )




#original Makefile.am contents follow:

#
#INCLUDES= -I$(top_srcdir)/libkdegames -I$(top_srcdir)/libkdegames/highscore $(all_includes)
#METASOURCES = AUTO
#
#bin_PROGRAMS = klines
#klines_SOURCES = prompt.cpp mwidget.cpp linesboard.cpp field.cpp cell.cpp \
#                 ballpainter.cpp klines.cpp main.cpp prefs.kcfgc
#klines_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#klines_LDADD = $(LIB_KDEGAMES)
#klines_DEPENDENCIES = $(LIB_KDEGAMES_DEP)
#
#picsdir = $(kde_datadir)/klines/
#pics_DATA = balls.jpg field.jpg fire.jpg
#
#xdg_apps_DATA = klines.desktop
#kde_kcfg_DATA = klines.kcfg
#
#EXTRA_DIST = $(pics_DATA)
#
#KDE_ICON = klines
#
#rcdir = $(kde_datadir)/klines
#rc_DATA = klinesui.rc
#
#messages: rc.cpp
#	$(XGETTEXT) *.cpp -o $(podir)/klines.pot;
#

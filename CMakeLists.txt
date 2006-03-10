kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/libkdegames ${CMAKE_SOURCE_DIR}/libkdegames/highscore ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


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

target_link_libraries(klines  ${LIB_KDEUI} kdegames )

install_targets(/bin klines )


########### install files ###############

install_files( ${XDG_APPS_DIR} FILES klines.desktop )
install_files( ${KCFG_INSTALL_DIR} FILES klines.kcfg )
install_files( ${DATA_INSTALL_DIR}/klines/ FILES balls.jpg field.jpg fire.jpg )
install_files( ${DATA_INSTALL_DIR}/klines FILES klinesui.rc )

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )

kde4_footer()



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

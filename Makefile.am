
INCLUDES= -I$(top_srcdir)/libkdegames -I$(top_srcdir)/libkdegames/highscore $(all_includes)
METASOURCES = AUTO

bin_PROGRAMS = klines
klines_SOURCES = prompt.cpp mwidget.cpp linesboard.cpp field.cpp cell.cpp \
                 ballpainter.cpp klines.cpp main.cpp prefs.kcfgc
klines_LDFLAGS = $(all_libraries) $(KDE_RPATH)
klines_LDADD = $(LIB_KDEGAMES)
klines_DEPENDENCIES = $(LIB_KDEGAMES_DEP)

picsdir = $(kde_datadir)/klines/
pics_DATA = balls.jpg field.jpg fire.jpg

xdg_apps_DATA = klines.desktop
kde_kcfg_DATA = klines.kcfg

EXTRA_DIST = $(pics_DATA)

KDE_ICON = klines

rcdir = $(kde_datadir)/klines
rc_DATA = klinesui.rc

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/klines.pot;


bin_PROGRAMS = klines
klines_SOURCES = prompt.cpp mwidget.cpp linesboard.cpp highscore.cpp field.cpp cell.cpp ballpainter.cpp klines.cpp main.cpp 
klines_LDADD   = -lkdeui

KDE_ICON = klines

klinesdir = $(kde_datadir)/klines/
klines_DATA = balls.jpg field.jpg fire.jpg

desktopdir = $(kde_appsdir)/Games/TacticStrategy
desktop_DATA = klines.desktop

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

METASOURCES = AUTO

# the library search path. 
klines_LDFLAGS = $(all_libraries) $(KDE_RPATH)

messages: 
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/klines.pot; \
	fi


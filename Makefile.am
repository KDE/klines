####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = klines
klines_SOURCES = prompt.cpp mwidget.cpp linesboard.cpp highscore.cpp field.cpp cell.cpp ballpainter.cpp klines.cpp main.cpp 
klines_LDADD   = -lkdeui -lkdecore $(LIB_QT) $(LIBSOCKET)

SUBDIRS = povray 

EXTRA_DIST = main.cpp klines.cpp klines.h klines.desktop hi32-app-klines.png hi16-app-klines.png ballpainter.cpp ballpainter.h balls.jpg cell.h cell.cpp cfg.h field.cpp field.h field.jpg fire.jpg highscore.cpp highscore.h linesboard.cpp linesboard.h mwidget.cpp mwidget.h prompt.cpp prompt.h 

install-data-local:
	$(mkinstalldirs) $(kde_appsdir)/Applications/
	$(INSTALL_DATA) $(srcdir)/klines.desktop $(kde_appsdir)/Applications/klines.desktop
	$(mkinstalldirs) $(kde_icondir)/hicolor/32x32/apps/
	$(INSTALL_DATA) $(srcdir)/hi32-app-klines.png $(kde_icondir)/hicolor/32x32/apps/klines.png
	$(mkinstalldirs) $(kde_icondir)/hicolor/16x16/apps/
	$(INSTALL_DATA) $(srcdir)/hi16-app-klines.png $(kde_icondir)/hicolor/16x16/apps/klines.png
	$(mkinstalldirs) $(kde_datadir)/klines/
	$(INSTALL_DATA) $(srcdir)/balls.jpg $(kde_datadir)/klines/ball.jpg
	$(mkinstalldirs) $(kde_datadir)/klines/
	$(INSTALL_DATA) $(srcdir)/field.jpg $(kde_datadir)/klines/field.jpg
	$(mkinstalldirs) $(kde_datadir)/klines/
	$(INSTALL_DATA) $(srcdir)/fire.jpg $(kde_datadir)/klines/fire.jpg

uninstall-local:
	-rm -f $(kde_appsdir)/Applications/klines.desktop
	-rm -f $(kde_icondir)/hicolor/32x32/apps/klines.png
	-rm -f $(kde_icondir)/hicolor/16x16/apps/klines.png
	-rm -f $(kde_datadir)/klines/ball.jpg
	-rm -f $(kde_datadir)/klines/field.jpg
	-rm -f $(kde_datadir)/klines/fire.jpg

####### kdevelop will overwrite this part!!! (end)############
# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to. 
# kde_icondir       Where your icon should go to.
# kde_minidir       Where your mini icon should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

METASOURCES = AUTO

# the library search path. 
klines_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# Uncomment the following two lines if you add a ui.rc file for your application to make use of
# KDE
#rcdir = $(kde_datadir)/klines
#rc_DATA = klinesui.rc

#WARNING: if you use a ui.rc file above, use:

# messages: rc.cpp

# instead of 

# messages:

messages: 
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/klines.pot; \
	fi


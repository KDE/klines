#! /usr/bin/env bash
$EXTRACTATTR --attr=Action,text --attr=Action,toolTip *.rc >> rc.cpp || exit 11
$XGETTEXT *.cpp -o $podir/klines.pot;

#!/bin/sh

#FIXME: install this launcher (debug launcher) at install time so it can do
#relative.

#export QML_IMPORT_TRACE=1
export QML_CONSOLE_EXTENDED=1

#this only works for qtquick controls though, use Text { renderType: Text.NativeRendering }

export DESKTOPCOMPONENTS_USE_NATIVE_TEXT=1
#export QML_COMPILER_STATS=1

cd ./install/bin

./ore-infinium $*


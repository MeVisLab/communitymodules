#!/bin/sh
#
# Run this script to create a Makefile for your project
#

# Check for Mac OS X
if [ "`uname -s`" = "Darwin" ]; then

echo "Please use 'open -b de.mevis.MeVisLab.MacProfileTranslator MLCMCurveList.pro' or 'open MLCMCurveList.pro'"

# Linux case
else

# Print usage-info
function usage () {
  echo "MLCMCurveList [-d|-r]"
  echo "Creates Makefile.MLCMCurveList - run \"make -f Makefile.MLCMCurveList\" to build your module"
  echo "  -r : create Makefile for release-version (libMLCMCurveList.so)"
  echo "  -d : create Makefile for debug-version (libMLCMCurveList_d.so)"
  exit 1
}

# Check arguments
if test "(" "$#" -gt "1" ")" -o "(" "(" "$#" = "1" ")" -a "(" "$1" != "-r" ")" -a "(" "$1" != "-d" ")" ")"
then
  usage
fi

# Switch debug/release/default
if [ "$1" = "-d" ];then
  DEBUG=1
elif [ "$1" = "-r" ];then
  DEBUG=0
else
  DEBUG=$(cat /proc/$PPID/cmdline|grep -c MeVisLab_d)
fi

if [ "$DEBUG" = "1" ];then
  VERSION="Debug-Version"
  FLAG="debug"
  EXT="_d"
else
  VERSION="Release-Version"
  FLAG="release"
  EXT=""
fi

echo "Creating Makefile.MLCMCurveList for $VERSION (libMLCMCurveList$EXT.so)"
qmake -t lib -o Makefile.MLCMCurveList "CONFIG+=$FLAG" "CONFIG+=MeVisLab" MLCMCurveList.pro

fi


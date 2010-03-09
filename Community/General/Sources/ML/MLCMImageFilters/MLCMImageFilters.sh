#!/bin/sh
#
# Run this script to create a Makefile for your project
#

# Check for Mac OS X
if [ "`uname -s`" = "Darwin" ]; then

echo "Please use 'open -b de.mevis.MeVisLab.MacProfileTranslator MLCMImageFilters.pro' or 'open MLCMImageFilters.pro'"

# Linux case
else

# Print usage-info
function usage () {
  echo "MLCMImageFilters [-d|-r]"
  echo "Creates Makefile.MLCMImageFilters - run \"make -f Makefile.MLCMImageFilters\" to build your module"
  echo "  -r : create Makefile for release-version (libMLCMImageFilters.so)"
  echo "  -d : create Makefile for debug-version (libMLCMImageFilters_d.so)"
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

echo "Creating Makefile.MLCMImageFilters for $VERSION (libMLCMImageFilters$EXT.so)"
qmake -t lib -o Makefile.MLCMImageFilters "CONFIG+=$FLAG" "CONFIG+=MeVisLab" MLCMImageFilters.pro

fi


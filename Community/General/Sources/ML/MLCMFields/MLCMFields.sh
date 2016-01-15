#!/bin/bash
#
# Run this script to create a Makefile for your project
#

# Check for Mac OS X
if [ "`uname -s`" = "Darwin" ]; then

echo "Please use 'open -b de.mevis.MeVisLab.MacProfileTranslator MLCMFields.pro' or 'open MLCMFields.pro'"

# Linux case
else

# Print usage-info
function usage () {
  echo "MLCMFields [-d|-r]"
  echo "Creates Makefile.MLCMFields - run \"make -f Makefile.MLCMFields\" to build your module"
  echo "  -r : create Makefile for release-version (libMLCMFields.so)"
  echo "  -d : create Makefile for debug-version (libMLCMFields_d.so)"
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
  MODE_FLAG="CONFIG-=release CONFIG+=debug"
  EXT="_d"
else
  VERSION="Release-Version"
  MODE_FLAG="CONFIG-=debug CONFIG+=release"
  EXT=""
fi

echo "Creating Makefile.MLCMFields for $VERSION (libMLCMFields$EXT.so)"
qmake -t lib -o Makefile.MLCMFields $MODE_FLAG "CONFIG+=MeVisLab" MLCMFields.pro

fi


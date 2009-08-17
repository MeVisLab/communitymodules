#! /bin/bash
if [ "`uname -s`" = "Darwin" && "$MLAB_ROOT" = "" ]; then
  echo "Generate Xcode projects by double-clicking the file 'MLOsiriXImporter.pro' in Finder or use the MeVisLabProjectGenerator.app from the command line to customize the project creation."
else
  python $MLAB_ROOT/MeVis/Foundation/BuildTools/Scripts/createProject.py MLOsiriXImporter
fi

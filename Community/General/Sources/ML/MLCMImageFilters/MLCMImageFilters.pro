# -----------------------------------------------------------------------------
# MLCMImageFilters project profile
#
# \file    MLCMImageFilters.pro
# \author  Theo van Walsum
# \date    2010-03-09
# -----------------------------------------------------------------------------

TEMPLATE = lib

TARGET  = MLCMImageFilters

DESTDIR = ../../../lib
DLLDESTDIR = ../../../lib

# add dependencies of this project here

CONFIG += dll ML

# set high warn level (warn 4 on MSCV)
WARN = HIGH


MLAB_PACKAGES += MeVisLab_Standard

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += MLCMIMAGEFILTERS_EXPORTS

HEADERS += \
    MLCMImageFiltersInit.h \
    MLCMImageFiltersSystem.h \
	  ExpressionParser/parser.h \
	  ExpressionParser/parseImage.h \
	  ExpressionParser/mlExpressionParser1.h \
	  ExpressionParser/mlExpressionParser2.h \
    ExpressionParser/mlNewImageFromExpression.h

SOURCES += \
    MLCMImageFiltersInit.cpp \
	  ExpressionParser/parser.cpp \
	  ExpressionParser/mlExpressionParser1.cpp \
	  ExpressionParser/mlExpressionParser2.cpp \
    ExpressionParser/mlNewImageFromExpression.cpp

# additional files that are NOT compiled
RELATEDFILES += \
    ../../../Modules/ML/MLCMImageFilters/MLCMImageFilters.def



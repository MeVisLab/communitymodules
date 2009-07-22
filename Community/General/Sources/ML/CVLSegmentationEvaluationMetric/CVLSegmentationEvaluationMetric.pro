# -----------------------------------------------------------------------------
# CVLSegmentationEvaluationMetric project profile
#
# \file    CVLSegmentationEvaluationMetric.pro
# \author  Stefan Saur
# \date    2009-07-21
# -----------------------------------------------------------------------------

TEMPLATE   = lib

TARGET     = CVLSegmentationEvaluationMetric

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# set high warn level (warn 4 on MSCV)
WARN = HIGH

# add used projects here (see included pri files below for available projects)
CONFIG += dll ML

MLAB_PACKAGES += MeVisLab_Standard

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += CVLSEGMENTATIONEVALUATIONMETRIC_EXPORTS

HEADERS += \
    CVLSegmentationEvaluationMetricInit.h \
    CVLSegmentationEvaluationMetricSystem.h \
    mlSegmentationEvaluationMetric.h

SOURCES += \
    CVLSegmentationEvaluationMetricInit.cpp \
    mlSegmentationEvaluationMetric.cpp

# additional files that are NOT compiled
RELATEDFILES += \
    ../../../Modules/ML/CVLSegmentationEvaluationMetric/CVLSegmentationEvaluationMetric.def


# -----------------------------------------------------------------------------
# MLDicomSurfaceSegmentation project profile
#
# \file
# \author  Axel Newe
# \date    2015-09-25
# -----------------------------------------------------------------------------


TEMPLATE   = lib

TARGET     = MLDicomSurfaceSegmentation

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# Set high warn level (warn 4 on MSVC)
WARN = HIGH

# Add used projects here (see included pri files below for available projects)
CONFIG += dll ML MLBase MLWEM dcmtk boost MLBaseListExtensions MLDicomTree MLMultiFileVolume
#CONFIG += MLDicomTagInterfaces

MLAB_PACKAGES += MeVis_Foundation MeVisLab_Standard Community_General
#MLAB_PACKAGES += FMEWork_ReleaseMevis

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += MLDICOMSURFACESEGMENTATION_EXPORTS

# Enable ML deprecated API warnings. To completely disable the deprecated API, change WARN to DISABLE.
DEFINES += ML_WARN_DEPRECATED

HEADERS += \
    MLDicomSurfaceSegmentationInit.h \
    MLDicomSurfaceSegmentationSystem.h \
    DICOMFileFormat/DCM_SharedTypes.h \
    DICOMFileFormat/DCM_PatientIE.h \
    DICOMFileFormat/DCM_StudyIE.h \
    DICOMFileFormat/DCM_SeriesIE.h \
    DICOMFileFormat/DCM_FrameOfReferenceIE.h \
    DICOMFileFormat/DCM_EquipmentIE.h \
    DICOMFileFormat/DCM_SurfaceIE.h \
    DicomSurfaceSegmentationLoad/DicomSurfaceSegmentationLoad.h \
    DicomSurfaceSegmentationSave/DicomSurfaceSegmentationSave.h \
	  tools/dicom/coordinates3D.h \
	  tools/dicom/dicomDatasetParser.h \
	  tools/dicom/dicomSSODatasetParser.h \
	  tools/dicom/element3D.h \



SOURCES += \
    MLDicomSurfaceSegmentationInit.cpp \
    DICOMFileFormat/DCM_PatientIE.cpp \
    DICOMFileFormat/DCM_StudyIE.cpp \
    DICOMFileFormat/DCM_SeriesIE.cpp \
    DICOMFileFormat/DCM_FrameOfReferenceIE.cpp \
    DICOMFileFormat/DCM_EquipmentIE.cpp \
    DICOMFileFormat/DCM_SurfaceIE.cpp \
    DicomSurfaceSegmentationLoad/DicomSurfaceSegmentationLoad.cpp \
    DicomSurfaceSegmentationSave/DicomSurfaceSegmentationSave.cpp \
	  tools/dicom/coordinates3D.cpp \
	  tools/dicom/dicomDatasetParser.cpp \
	  tools/dicom/dicomSSODatasetParser.cpp \
	  tools/dicom/element3D.cpp \


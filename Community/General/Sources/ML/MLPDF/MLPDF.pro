# -----------------------------------------------------------------------------
# MLPDF project profile
#
# \file    MLPDF.pro
# \author  Axel Newe
# \date    2014-02-06
# -----------------------------------------------------------------------------


TEMPLATE   = lib

TARGET     = MLPDF

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# Set high warn level (warn 4 on MSVC)
WARN = HIGH

# Add used projects here (see included pri files below for available projects)
CONFIG += dll ML MLBase MLWEM zlib png MLBaseListExtensions

MLAB_PACKAGES += MeVisLab_Standard MeVis_ThirdParty Community_General

INCLUDEPATH += ./shared/
INCLUDEPATH += ./thirdparty/libHaru/RELEASE/include/
INCLUDEPATH += ./thirdparty/libHaru/RELEASE/win32/include/

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += MLPDF_EXPORTS

# Enable ML deprecated API warnings. To completely disable the deprecated API, change WARN to DISABLE.
DEFINES += ML_WARN_DEPRECATED

HEADERS += \
  MLPDFInit.h \
  MLPDFSystem.h \
  thirdparty/libHaru/RELEASE/include/hpdf.h \
  thirdparty/libHaru/RELEASE/include/hpdf_3dmeasure.h \
  thirdparty/libHaru/RELEASE/include/hpdf_annotation.h \
  thirdparty/libHaru/RELEASE/include/hpdf_catalog.h \
  thirdparty/libHaru/RELEASE/include/hpdf_conf.h \  
  thirdparty/libHaru/RELEASE/include/hpdf_consts.h \
  thirdparty/libHaru/RELEASE/include/hpdf_destination.h \
  thirdparty/libHaru/RELEASE/include/hpdf_doc.h \
  thirdparty/libHaru/RELEASE/include/hpdf_encoder.h \
  thirdparty/libHaru/RELEASE/include/hpdf_encrypt.h \
  thirdparty/libHaru/RELEASE/include/hpdf_encryptdict.h \
  thirdparty/libHaru/RELEASE/include/hpdf_error.h \
  thirdparty/libHaru/RELEASE/include/hpdf_exdata.h \
  thirdparty/libHaru/RELEASE/include/hpdf_ext_gstate.h \
  thirdparty/libHaru/RELEASE/include/hpdf_font.h \
  thirdparty/libHaru/RELEASE/include/hpdf_fontdef.h \
  thirdparty/libHaru/RELEASE/include/hpdf_gstate.h \
  thirdparty/libHaru/RELEASE/include/hpdf_image.h \
  thirdparty/libHaru/RELEASE/include/hpdf_info.h \
  thirdparty/libHaru/RELEASE/include/hpdf_list.h \
  thirdparty/libHaru/RELEASE/include/hpdf_mmgr.h \
  thirdparty/libHaru/RELEASE/include/hpdf_namedict.h \
  thirdparty/libHaru/RELEASE/include/hpdf_objects.h \
  thirdparty/libHaru/RELEASE/include/hpdf_outline.h \
  thirdparty/libHaru/RELEASE/include/hpdf_page_label.h \
  thirdparty/libHaru/RELEASE/include/hpdf_pages.h \
  thirdparty/libHaru/RELEASE/include/hpdf_pdfa.h \
  thirdparty/libHaru/RELEASE/include/hpdf_streams.h \
  thirdparty/libHaru/RELEASE/include/hpdf_types.h \
  thirdparty/libHaru/RELEASE/include/hpdf_u3d.h \
  thirdparty/libHaru/RELEASE/include/hpdf_utils.h \
  thirdparty/libHaru/RELEASE/include/hpdf_version.h \
#  thirdparty/libHaru/RELEASE/include/hpdf_config.h \
  thirdparty/libHaru/RELEASE/win32/include/hpdf_config.h \
  shared/MLPDF_DataTypes.h \
  shared/MLPDF_Tools.h \
  SpecificationGenerator/MLPDF_SpecificationGenerator.h \
  SavePDF/SavePDF.h \
  SaveU3D/SaveU3D.h \
  SaveU3D/U3DFileFormat/U3D_BitStream.h \
  SaveU3D/U3DFileFormat/U3D_Constants.h \
  SaveU3D/U3DFileFormat/U3D_ContextManager.h \
  SaveU3D/U3DFileFormat/U3D_DataBlock.h \
  SaveU3D/U3DFileFormat/U3D_DataBlockWriter.h \
  SaveU3D/U3DFileFormat/U3D_DataTypes.h \
  SaveU3D/U3DFileFormat/U3D_FileWriter.h \
  SaveU3D/U3DFileFormat/U3D_Tools.h \
  SavePRC/SavePRC.h \
  SavePRC/PRCFileFormat/PRC_Macros.h \
  SavePRC/PRCFileFormat/PRC_Defines.h \
  SavePRC/PRCFileFormat/PRC_DataTypes.h \
  SavePRC/PRCFileFormat/PRC_BitStream.h \
  SavePRC/PRCFileFormat/PRC_Write.h \
  SavePRC/PRCFileFormat/PRC_File.h \
  SavePRC/PRCFileFormat/PRC_Tools.h \
    
SOURCES += \
  MLPDFInit.cpp \
  thirdparty/libHaru/RELEASE/src/hpdf_3dmeasure.c \
  thirdparty/libHaru/RELEASE/src/hpdf_annotation.c \
  thirdparty/libHaru/RELEASE/src/hpdf_array.c \
  thirdparty/libHaru/RELEASE/src/hpdf_binary.c \
  thirdparty/libHaru/RELEASE/src/hpdf_boolean.c \
  thirdparty/libHaru/RELEASE/src/hpdf_catalog.c \
  thirdparty/libHaru/RELEASE/src/hpdf_destination.c \
  thirdparty/libHaru/RELEASE/src/hpdf_dict.c \  
  thirdparty/libHaru/RELEASE/src/hpdf_doc.c \
  thirdparty/libHaru/RELEASE/src/hpdf_doc_png.c \
  thirdparty/libHaru/RELEASE/src/hpdf_encoder.c \
  thirdparty/libHaru/RELEASE/src/hpdf_encoder_cns.c \
  thirdparty/libHaru/RELEASE/src/hpdf_encoder_cnt.c \
  thirdparty/libHaru/RELEASE/src/hpdf_encoder_jp.c \
  thirdparty/libHaru/RELEASE/src/hpdf_encoder_kr.c \
  thirdparty/libHaru/RELEASE/src/hpdf_encoder_utf.c \
  thirdparty/libHaru/RELEASE/src/hpdf_encrypt.c \
  thirdparty/libHaru/RELEASE/src/hpdf_encryptdict.c \
  thirdparty/libHaru/RELEASE/src/hpdf_error.c \
  thirdparty/libHaru/RELEASE/src/hpdf_exdata.c \
  thirdparty/libHaru/RELEASE/src/hpdf_ext_gstate.c \
  thirdparty/libHaru/RELEASE/src/hpdf_font.c \
  thirdparty/libHaru/RELEASE/src/hpdf_font_cid.c \
  thirdparty/libHaru/RELEASE/src/hpdf_font_tt.c \
  thirdparty/libHaru/RELEASE/src/hpdf_font_type1.c \
  thirdparty/libHaru/RELEASE/src/hpdf_fontdef.c \
  thirdparty/libHaru/RELEASE/src/hpdf_fontdef_base14.c \
  thirdparty/libHaru/RELEASE/src/hpdf_fontdef_cid.c \
  thirdparty/libHaru/RELEASE/src/hpdf_fontdef_cns.c \
  thirdparty/libHaru/RELEASE/src/hpdf_fontdef_cnt.c \
  thirdparty/libHaru/RELEASE/src/hpdf_fontdef_jp.c \
  thirdparty/libHaru/RELEASE/src/hpdf_fontdef_kr.c \
  thirdparty/libHaru/RELEASE/src/hpdf_fontdef_tt.c \
  thirdparty/libHaru/RELEASE/src/hpdf_fontdef_type1.c \
  thirdparty/libHaru/RELEASE/src/hpdf_gstate.c \
  thirdparty/libHaru/RELEASE/src/hpdf_image.c \
  thirdparty/libHaru/RELEASE/src/hpdf_image_ccitt.c \
  thirdparty/libHaru/RELEASE/src/hpdf_image_png.c \
  thirdparty/libHaru/RELEASE/src/hpdf_info.c \
  thirdparty/libHaru/RELEASE/src/hpdf_list.c \
  thirdparty/libHaru/RELEASE/src/hpdf_mmgr.c \
  thirdparty/libHaru/RELEASE/src/hpdf_name.c \
  thirdparty/libHaru/RELEASE/src/hpdf_namedict.c \
  thirdparty/libHaru/RELEASE/src/hpdf_null.c \
  thirdparty/libHaru/RELEASE/src/hpdf_number.c \
  thirdparty/libHaru/RELEASE/src/hpdf_objects.c \
  thirdparty/libHaru/RELEASE/src/hpdf_outline.c \
  thirdparty/libHaru/RELEASE/src/hpdf_page_label.c \
  thirdparty/libHaru/RELEASE/src/hpdf_page_operator.c \
  thirdparty/libHaru/RELEASE/src/hpdf_pages.c \
  thirdparty/libHaru/RELEASE/src/hpdf_pdfa.c \
  thirdparty/libHaru/RELEASE/src/hpdf_real.c \
  thirdparty/libHaru/RELEASE/src/hpdf_streams.c \
  thirdparty/libHaru/RELEASE/src/hpdf_string.c \
  thirdparty/libHaru/RELEASE/src/hpdf_u3d.c \
  thirdparty/libHaru/RELEASE/src/hpdf_utils.c \
  thirdparty/libHaru/RELEASE/src/hpdf_xref.c \
  shared/MLPDF_Tools.cpp \
  SpecificationGenerator/MLPDF_SpecificationGenerator.cpp \
  SavePDF/SavePDF.cpp \
  SaveU3D/SaveU3D.cpp \
  SaveU3D/SaveU3D_Tools.cpp \
  SaveU3D/SaveU3D_ProcessPointSets.cpp \
  SaveU3D/SaveU3D_ProcessLineSets.cpp \
  SaveU3D/SaveU3D_ProcessMeshes.cpp \
  SaveU3D/U3DFileFormat/U3D_BitStream.cpp \
  SaveU3D/U3DFileFormat/U3D_ContextManager.cpp \
  SaveU3D/U3DFileFormat/U3D_DataBlock.cpp \
  SaveU3D/U3DFileFormat/U3D_DataBlockWriter.cpp \
  SaveU3D/U3DFileFormat/U3D_FileWriter.cpp \
  SaveU3D/U3DFileFormat/U3D_Tools.cpp \
  SavePRC/SavePRC.cpp \
  SavePRC/SavePRC_Tools.cpp \
  SavePRC/SavePRC_ProcessPointSets.cpp \
  SavePRC/SavePRC_ProcessLineSets.cpp \
  SavePRC/SavePRC_ProcessMeshes.cpp \
  SavePRC/PRCFileFormat/PRC_DataTypes.cpp \
  SavePRC/PRCFileFormat/PRC_BitStream.cpp \
  SavePRC/PRCFileFormat/PRC_Write.cpp \
  SavePRC/PRCFileFormat/PRC_File.cpp \
  SavePRC/PRCFileFormat/PRC_Tools.cpp \

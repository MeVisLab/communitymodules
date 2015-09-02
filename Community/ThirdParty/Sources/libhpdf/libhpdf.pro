# -----------------------------------------------------------------------------
# libhpdf project profile
#
# \file    libhpdf.pro
# \author  Axel Newe
# \date    2015-09-01
# -----------------------------------------------------------------------------

MEVIS_MAINTAINER = Axel Newe

TEMPLATE   = lib

TARGET     = libhpdf

DESTDIR    = ../../lib
DLLDESTDIR = ../../lib

# Set high warn level (warn 4 on MSVC)
#WARN = HIGH
# Switch of all warnings... it's a third party library and we don't care about warnings...
CONFIG += warn_off

# Add used projects here (see included pri files below for available projects)
CONFIG += dll zlib png

#MLAB_PACKAGES += MeVisLab_Standard

INCLUDEPATH += include/
INCLUDEPATH += src/

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

# Needed by hpdf.h
DEFINES += HPDF_DLL_MAKE

HEADERS += \
  hpdf_config.h \               # This is the only file that does not come from the original  library
  include/hpdf.h \
  include/hpdf_3dmeasure.h \
  include/hpdf_annotation.h \
  include/hpdf_catalog.h \
  include/hpdf_conf.h \  
  include/hpdf_consts.h \
  include/hpdf_destination.h \
  include/hpdf_doc.h \
  include/hpdf_encoder.h \
  include/hpdf_encrypt.h \
  include/hpdf_encryptdict.h \
  include/hpdf_error.h \
  include/hpdf_exdata.h \
  include/hpdf_ext_gstate.h \
  include/hpdf_font.h \
  include/hpdf_fontdef.h \
  include/hpdf_gstate.h \
  include/hpdf_image.h \
  include/hpdf_info.h \
  include/hpdf_list.h \
  include/hpdf_mmgr.h \
  include/hpdf_namedict.h \
  include/hpdf_objects.h \
  include/hpdf_outline.h \
  include/hpdf_page_label.h \
  include/hpdf_pages.h \
  include/hpdf_pdfa.h \
  include/hpdf_streams.h \
  include/hpdf_types.h \
  include/hpdf_u3d.h \
  include/hpdf_utils.h \
  include/hpdf_version.h \
  src/t4.h \

SOURCES += \
  src/hpdf_3dmeasure.c \
  src/hpdf_annotation.c \
  src/hpdf_array.c \
  src/hpdf_binary.c \
  src/hpdf_boolean.c \
  src/hpdf_catalog.c \
  src/hpdf_destination.c \
  src/hpdf_dict.c \  
  src/hpdf_doc.c \
  src/hpdf_doc_png.c \
  src/hpdf_encoder.c \
  src/hpdf_encoder_cns.c \
  src/hpdf_encoder_cnt.c \
  src/hpdf_encoder_jp.c \
  src/hpdf_encoder_kr.c \
  src/hpdf_encoder_utf.c \
  src/hpdf_encrypt.c \
  src/hpdf_encryptdict.c \
  src/hpdf_error.c \
  src/hpdf_exdata.c \
  src/hpdf_ext_gstate.c \
  src/hpdf_font.c \
  src/hpdf_font_cid.c \
  src/hpdf_font_tt.c \
  src/hpdf_font_type1.c \
  src/hpdf_fontdef.c \
  src/hpdf_fontdef_base14.c \
  src/hpdf_fontdef_cid.c \
  src/hpdf_fontdef_cns.c \
  src/hpdf_fontdef_cnt.c \
  src/hpdf_fontdef_jp.c \
  src/hpdf_fontdef_kr.c \
  src/hpdf_fontdef_tt.c \
  src/hpdf_fontdef_type1.c \
  src/hpdf_gstate.c \
  src/hpdf_image.c \
  src/hpdf_image_ccitt.c \
  src/hpdf_image_png.c \
  src/hpdf_info.c \
  src/hpdf_list.c \
  src/hpdf_mmgr.c \
  src/hpdf_name.c \
  src/hpdf_namedict.c \
  src/hpdf_null.c \
  src/hpdf_number.c \
  src/hpdf_objects.c \
  src/hpdf_outline.c \
  src/hpdf_page_label.c \
  src/hpdf_page_operator.c \
  src/hpdf_pages.c \
  src/hpdf_pdfa.c \
  src/hpdf_real.c \
  src/hpdf_streams.c \
  src/hpdf_string.c \
  src/hpdf_u3d.c \
  src/hpdf_utils.c \
  src/hpdf_xref.c \

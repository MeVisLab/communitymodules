# -----------------------------------------------------------------------------
# ${vars.cppProject.fullProject} project profile
#
# \file    ${__TARGETFILE__}
# \author  ${vars.author}
# \date    ${__DATE__}
# -----------------------------------------------------------------------------


TEMPLATE   = lib

TARGET     = ${vars.cppProject.fullProject}

DESTDIR    = ${vars.cppProject.relRootDir}/lib
DLLDESTDIR = ${vars.cppProject.relRootDir}/lib

# Set high warn level (warn 4 on MSVC)
WARN = HIGH

# Add used projects here (see included pri files below for available projects)
CONFIG += dll ML MLPDF

MLAB_PACKAGES += MeVisLab_Standard Community_General

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += ${vars.cppProject.fullProjectUpper}_EXPORTS

# Enable ML deprecated API warnings. To completely disable the deprecated API, change WARN to DISABLE.
DEFINES += ML_WARN_DEPRECATED

HEADERS += ${"\\"}
    ${vars.cppProject.fullProject}Init.h ${"\\"}
    ${vars.cppProject.fullProject}System.h ${"\\"}
    ${vars.moduleName}.h ${"\\"}


SOURCES += ${"\\"}
    ${vars.cppProject.fullProject}Init.cpp ${"\\"}
    ${vars.moduleName}.cpp ${"\\"}


# Additional files that are NOT compiled
RELATEDFILES += ${"\\"}
    ../Modules/${vars.moduleName}.def ${"\\"}
    ../Modules/${vars.moduleName}.script ${"\\"}


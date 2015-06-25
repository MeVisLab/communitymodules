# **InsertLicense** code
# -----------------------------------------------------------------------------
# Community_General Package Setup, defining default variables and Package references.
#
# \file    DefaultProjectSetup.pri
# \author  Generated by MLABRepoTools/Package.py
# \date    06/2015
# -----------------------------------------------------------------------------

isEmpty(CONFIG) {
  warning("CONFIG is empty.")
}

!isEmpty(PACKAGE_SETUP_INCLUDED) {
  warning("Package setup already included, skipping!")
}

isEmpty(PACKAGE_SETUP_INCLUDED) {
  # Define the include guard
  PACKAGE_SETUP_INCLUDED = 1
  
  message("Loading DefaultProjectSetup.pri")

  message("**************************************************************")
  message("Setting up default project configuration")
  message("--------------------------------------------------------------")
  message("Project Path: $$OUT_PWD")
  message("Package Path: $$(MLAB_Community_General)")
  
  # probe a couple of directory hierarchies to get the relative path to the package
  dirs = .. ../.. ../../.. ../../../.. ../../../../.. ../../../../../.. ../../../../../../..
  for(dir, dirs) {
    exists($${OUT_PWD}/$${dir}/Configuration/DefaultProjectSetup.pri) {
      RelativePackagePath = $$dir
    }
  }

  APPLICATIONTEMPLATES = app vcapp
  contains(APPLICATIONTEMPLATES, $$TEMPLATE) {
    isEmpty(DESTDIR) {
      isEmpty(RelativePackagePath) {
        warning("Failed to generate default DESTDIR")
      }
      else {
        DESTDIR = $${RelativePackagePath}/bin
      }
      message("DESTDIR missing, setting default: $$DESTDIR")
    }
  }
  else {
    isEmpty(DLLDESTDIR) {
      isEmpty(RelativePackagePath) {
        warning("DLLDESTDIR: missing, relative package path not available")
      }
      else {
        DLLDESTDIR = $${RelativePackagePath}/lib
      }
      message("DLLDESTDIR missing, setting default: $$DLLDESTDIR")
    }
    isEmpty(DESTDIR) {
      DESTDIR = $$DLLDESTDIR
      message("DESTDIR missing, setting default: $$DESTDIR")
    }
  }
  
  isEmpty(WARN) {
    WARN = MAXIMUM64
    message("WARN missing, setting default: $$WARN")
  }

  win32:isEmpty(OBJECTS_DIR) {
    # avoid mix-up of temporary files with other MLImageIO projects
    debug {
      OBJECTS_DIR = tmp/debug/obj/$$TARGET
    } else {
      OBJECTS_DIR = tmp/release/obj/$$TARGET
    }
    message("OBJECTS_DIR missing, setting default: $$OBJECTS_DIR")
  }

  message("--------------------------------------------------------------")
  
  DEFAULT_PACKAGES_Community_General = Community_General \
                                           FMEwork_VTK \
                                           FMEwork_ITK \
                                           FMEwork_ThirdParty \
                                           FMEwork_ReleaseMeVis \
                                           FMEstable_ReleaseMeVis \
                                           MeVisLab_Standard \
                                           MeVisLab_IDE
  for(PACKAGE,DEFAULT_PACKAGES_Community_General) {
    PACKAGEPATH = $(MLAB_$${PACKAGE})
    FILE = $$PACKAGEPATH/Configuration/$${PACKAGE}.pri
    exists($$FILE) {
      MLAB_PACKAGES -= $$PACKAGE
      MLAB_PACKAGES += $$PACKAGE
    }
    else {
      message(WARNING: Package $$PACKAGE not found - ommitting dependencies)
    }
  }
  
  include($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

}

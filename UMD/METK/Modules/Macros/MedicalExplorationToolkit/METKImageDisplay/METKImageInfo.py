# Python module import
from ObjMgr import *
from VisDefinitions import *
from linear import *

IMAGE_INFO_VERSION = 1


# called from METKImageDisplay.py
def closeImage(image = ""):
   ctx.field("ready").value = False
   pass


# called from METKImageDisplay.py
def getImage(parent):
   if ctx.field("image").isValid():
      ctx.field("ready").value = False
      ctx.log("Computing image infos for " + parent)
      childIDs = _cls_info.get(parent, LAY_GLOBAL, INF_CHILDS).split(";")
      for imageID in childIDs:
         if imageID != "" and (not _cls_info.existInfo(imageID, LAY_IMAGE, INF_IMAGEINFOVERSION) or _cls_info.getInt(imageID, LAY_IMAGE, INF_IMAGEINFOVERSION) != IMAGE_INFO_VERSION):
            ctx.field("METKLoadSegMask.objID").value = imageID
            ctx.field("METKLoadSegMask.load").touch()
            MLAB.processEvents()
            ctx.field("ImageStat.update").touch()
            MLAB.processEvents()
            min = ctx.field("ImageStat.totalMinVal").value
            max = ctx.field("ImageStat.totalMaxVal").value
            _cls_info.typedSet(imageID, LAY_IMAGE, INF_WINDOWCENTER,     min + (max - min) / 2, INFOTYPE_INT32)
            _cls_info.typedSet(imageID, LAY_IMAGE, INF_WINDOWWIDTH,      max - min,             INFOTYPE_INT32)
            _cls_info.typedSet(imageID, LAY_IMAGE, INF_IMAGEINFOVERSION, IMAGE_INFO_VERSION,    INFOTYPE_INT32)            
      _cls_info.notify()
      ctx.field("ready").value = True
      ctx.log("Image infos computed")
   return


def imageSet(field = 0):
   ctx.field("ready").value = False
   if ctx.field("roiSelect").value != "":
      getImage(ctx.field("roiSelect").value)
   pass
# Python module import
from ObjMgr import *
from VisDefinitions import *
from NeckSurgery import *

# Global class instances
_cls_input= None
_cls_output= None


def init():
   global _cls_input, _cls_output
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_input  = ObjInfo(ctx.module("input"))
   _cls_output = ObjInfo(ctx.module("output"))

   return
   
def handleEvents(againstTypeError = 0):
   ctx.field("ready").value = False
   ctx.field("readableFormat").value = False
   version = _cls_input.get("Global", "NeckVision", "Version")
   if version == "1.0":
      ctx.log("Converting XML data from NeckVision")
      ctx.field("readableFormat").value = True
      _cls_input.handleEvents()
      _cls_output.set(OBJ_APPLICATION, LAY_GLOBAL, INF_OBJTYPE, "Application")
      _cls_output.set(OBJ_APPLICATION, LAY_APPLICATION, INF_APPLICATIONNAME, "MedicalExplorationToolkit")
      _cls_output.set(OBJ_APPLICATION, LAY_APPLICATION, INF_APPLICATIONVERSION, "1.0")
      _cls_output.set(OBJ_APPLICATION, LAY_APPLICATION, INF_MEVISLAB, "1.4")
      
      _cls_output.set(OBJ_CASE, LAY_GLOBAL, INF_OBJTYPE, "Case")
      _cls_output.set(OBJ_PATIENT, LAY_GLOBAL, INF_OBJTYPE, "Patient")
      _cls_output.notify()
      ctx.field("ready").value = True
   return


#--------------------------------------------
# Event handling
#--------------------------------------------

def handleObjectCreatedEvent():
   pass


def handleObjectRemovedEvent():
   pass


def handleAttributeCreatedEvent():
   object = _cls_input.activeObject()
   layer  = _cls_input.activeLayer()
   info   = _cls_input.activeInfo()
   value  = _cls_input.get()
   
   #--------------------------------------------
   # global/static informations
   #--------------------------------------------
   if object == "Global":
      if layer == "Information":
         _cls_output.set(OBJ_CASE, LAY_CASE, info, value)
         
      elif layer == "Patient":
         _cls_output.set(OBJ_PATIENT, LAY_PATIENT, info, value)
   
   
   #--------------------------------------------
   # dynamic informations
   #--------------------------------------------
   
   # Image and ROI informations
   elif object == "Image":
      imageName = "Image_" + _cls_input.get("Image", "Data", "Basename")
      if layer == "Data":
         if info == "Basename": 
            _cls_output.set(imageName, LAY_IMAGE, INF_FILENAME,  value + ".dcm")
            _cls_output.set(imageName, LAY_IMAGE, INF_IMAGETYPE, "Original")
            _cls_output.set(imageName, LAY_IMAGE, INF_OBJVALUE,  "-1")
            _cls_output.set(imageName, LAY_GLOBAL, INF_OBJTYPE,  "Source")
            _cls_output.set(imageName, LAY_GLOBAL, INF_CHILDS,   "")
            _cls_output.typedSet(imageName, LAY_GLOBAL, INF_VALID, True, INFOTYPE_BOOL)
      if layer == "Modality":
         if info == "Equipment": 
            _cls_output.set(imageName, LAY_IMAGE, INF_MODALITY, value)
         if info == "Date": 
            _cls_output.set(imageName, LAY_IMAGE, INF_STUDYDATE, value)
         if info == "Institition": 
            _cls_output.set(imageName, LAY_IMAGE, INF_INSTITUTION, value)
      if layer == "ROI":
         ROIName = "ROI_" + value
         _cls_output.set(ROIName, LAY_IMAGE,  INF_FILENAME,  value + ".dcm")
         _cls_output.set(ROIName, LAY_IMAGE,  INF_IMAGETYPE, "Original")
         _cls_output.set(ROIName, LAY_GLOBAL, INF_OBJTYPE,   "Result")
         _cls_output.set(ROIName, LAY_IMAGE,  INF_OBJVALUE,  "-1")
         _cls_output.typedSet(ROIName, LAY_GLOBAL, INF_VALID, True, INFOTYPE_BOOL)
         # ChildID des Image updaten
         _cls_output.set(imageName, LAY_GLOBAL, INF_CHILDS, _cls_output.get(imageName, LAY_GLOBAL, INF_CHILDS) + ROIName + ";")
         # ParentID setzen
         _cls_output.set(ROIName, LAY_GLOBAL, INF_PARENT,  imageName)

   # Segmentationresults
   elif object[:3] == "ROI" and layer[:6] == "Result":
      if info == INF_STRUCTURE:
         structureName = (_cls_input.get(object, layer, INF_STRUCTURE)).replace(" ", "")
         structureName = "Structure_" + structureName.replace("-", "")
         ROIName = "ROI_" + _cls_input.get("Image", "ROI", object)
         # ChildID der ROI updaten
         _cls_output.set(ROIName, LAY_GLOBAL, INF_CHILDS, _cls_output.get(ROIName, LAY_GLOBAL, INF_CHILDS) + structureName + ";")
         # ParentID setzen
         _cls_output.set(structureName, LAY_GLOBAL, INF_PARENT,  ROIName)
         _cls_output.set(structureName, LAY_GLOBAL, INF_OBJTYPE, "Result")
         _cls_output.set(structureName, LAY_IMAGE,  INF_IMAGETYPE, "Segmentation")
         _cls_output.typedSet(structureName, LAY_GLOBAL, INF_VALID, True, INFOTYPE_BOOL)
         
         _cls_output.set(structureName, LAY_DESCRIPTION, INF_NAME, _cls_input.get(object, layer, INF_STRUCTURE))
         
         # Structure specific
         structure = _cls_input.get(object, layer, INF_STRUCTURE)
         infoInfo = _cls_input.get(object, layer, "Info")
         
         # Seite angeben
         if infoInfo.find("right") != -1:
	    _cls_output.set(structureName, LAY_DESCRIPTION, INF_SIDE, "Right")
         if infoInfo.find("left") != -1:
	    _cls_output.set(structureName, LAY_DESCRIPTION, INF_SIDE, "Left")

         # bei Nerven keinen IMAGE-Eintrag setzen
         if structure.find("N. ") == -1:
            _cls_output.set(structureName, LAY_IMAGE, INF_FILENAME, _cls_input.get(object, layer, INF_FILENAME) + ".dcm")
         _cls_output.set(structureName, LAY_FILES, INF_IVFILE, _cls_input.get(object, layer, INF_FILENAME) + ".iv")
         
         # all structures are unpickable, except lymph nodes and tumors
         _cls_output.set(structureName, LAY_APPEARANCE,  INF_PICKSTYLE, "UNPICKABLE")

         # is Lymphnode
         if structure.find("Lymph") != -1:
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTURE, LYMPHNODE)
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTUREGROUP, LYMPHNODE)
            _cls_output.set(structureName, LAY_APPEARANCE,  INF_PICKSTYLE, "SHAPE")
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_LEVEL, infoInfo[6:infoInfo.find(",")])
         # is Muscle
         elif structure.find("M. ") != -1:
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTURE, "Muscle")
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTUREGROUP, "Muscle")
         # is Jugularis
         elif structure.find("jugularis") != -1:
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTURE, "Vein")
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTUREGROUP, "Vessel")
         # is Carotis
         elif structure.find("carotis") != -1:
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTURE, "Artery")
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTUREGROUP, "Vessel")
         # is Bone
         elif structure.find("Bones") != -1:
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTURE, "Bone")
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTUREGROUP, "Bone")
         # is Nerv
         elif structure.find("N. ") != -1:
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTURE, "Nerve")
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTUREGROUP, "Nerve")
         # is Gland
         elif structure.find("Gl. ") != -1:
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTURE, "Salivary Gland")
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTUREGROUP, "Gland")
         # is Larynx
         elif structure.find("Larynx") != -1 or structure.find("C. ") != -1:
            if structure.find("C. ") != -1:
               _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTURE, "Cartilago")
            else:
               _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTURE, "Larynx")
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTUREGROUP, "Larynx")
         # is Tumor
         elif structure.find("Tumor") != -1:
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTURE, "Tumor")
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTUREGROUP, "Tumor")
            _cls_output.set(structureName, LAY_APPEARANCE,  INF_PICKSTYLE, "SHAPE")
         # is Trachea
         elif structure.find("Trachea") != -1:
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTURE, "Trachea")
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTUREGROUP, "Respiratory Tract")
         # is Pharynx
         elif structure.find("Pharynx") != -1:
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTURE, "Pharynx")
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTUREGROUP, "Respiratory Tract")
         # is Lung
         elif structure.find("Lung") != -1:
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTURE, "Lung")
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTUREGROUP, "Respiratory Tract")

         # is something else
         else:
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTURE, "UserDefined")
            _cls_output.set(structureName, LAY_DESCRIPTION, INF_STRUCTUREGROUP, "UserDefined")
            ctx.log(structureName + " was interpreted as UserDefined!")
         _cls_output.typedSet(structureName, LAY_APPEARANCE, INF_COLOR,        getStandardColor(_cls_output.get(structureName, LAY_DESCRIPTION, INF_STRUCTURE)), INFOTYPE_VEC3) 
	 _cls_output.typedSet(structureName, LAY_APPEARANCE, INF_TRANSPARENCY, getStandardTransparency(_cls_output.get(structureName, LAY_DESCRIPTION, INF_STRUCTURE)), INFOTYPE_DOUBLE) 
	 _cls_output.typedSet(structureName, LAY_DESCRIPTION, INF_IMPORTANCE, getStandardImportance(_cls_output.get(structureName, LAY_DESCRIPTION, INF_STRUCTURE)), INFOTYPE_DOUBLE) 
	 _cls_output.typedSet(structureName, LAY_APPEARANCE, INF_BB,           False, INFOTYPE_BOOL) 

   return


def handleAttributeRemovedEvent():
   pass


def handleAttributeModifiedEvent():
   pass

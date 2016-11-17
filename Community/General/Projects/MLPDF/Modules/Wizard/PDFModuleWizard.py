#----------------------------------------------------------------------------------
# Wizard for creating PDF modules
#
# \file    PDFModuleWizard.py
# \author  Axel Newe
# \date    2015-12-01
#----------------------------------------------------------------------------------

_titles = ["Module Properties","Demo Code","Module Field Interface"]
_infos =  ["Enter the general properties of the module.","Select if demo code shall be generated.","Add fields to the interface of the module."]

# --- Initialization

# Initialize module
def InitModule():
  InitFields()
  UpdateFields()

# Initialize field values
def InitFields():
  ctx.field("exampleNetwork").value = True
  ctx.field("cppProject.directoryStructure").value = "Self-contained"
  pass

# Update field values
def UpdateFields():
  UpdateFieldTypeFlags()
  gListViewEdit.fieldEdit(ctx.field("fieldName"))

# Initialize list view control
def InitWindow():
  global _step
  fieldNames    = ["fieldName", "fieldType", "fieldComment", "fieldValue", "fieldValues"]
  fieldDefaults = ["", "String", "", "", ""] 
  gListViewEdit.setListView("fieldList")
  gListViewEdit.setItemFields(fieldNames, fieldDefaults, 0, "fields")
  gListViewEdit.setSelectedField("fieldSelected")
  gListViewEdit.setListSupplementFunction(SupplementField)
  _step = 0
  UpdateTab()

# --- Field updates

# Update info box
def UpdateInfoBox():
  ctx.control("info").setTitle(moduleTypeInfo[ctx.field("type").value])

# --- Code creation

# Create code from template list
def CreateCode():
  # Deselect in field list to make sure that changes to fieldName are
  # processed as field selections, not as editing
  gListViewEdit.deselect()
  
  if not CheckIfModuleMayBeCreated(ctx.field("moduleName").value):
    return
  
  templateListPath = ctx.field("templateListPath").stringValue()
  CreateCodeFromTemplateList(templateListPath)
  
  MLAB.setVariable("ModuleWizardMenuReloadModulesOnClose", "1")

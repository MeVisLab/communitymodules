# Python module import
from os import *
from os.path import *
from urlparse import *
from string import *
from ObjMgr import *
from VisDefinitions import *
import _winreg
from kButton import *

def openCase(field = 0):
   caseFile = MLABFileDialog.getOpenFileName("","XML files (*.xml)", "Open Case File");
   if caseFile != "":
      ctx.field("METKManager.fileName").value = caseFile
      ctx.field("METKManager.load").touch()
      ctx.window().setTitle("METKDemonstrator | " + caseFile)
   return


def saveAs(fieldP =0):
   ctx.field("METKManager.saveAs").touch()
   pass


def save(field=0):
   ctx.field("METKManager.save").touch()
   pass
   	
   	
def exit(field=0):
   ctx.field("METKManager.cleanup").touch()   
   if field:
      ctx.window().close()
   return
   
   

def loadDemo(fieldP=0):
   ctx.log("try to load demo case")
   try:
      METKDemoSettings = _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, r'Software\MeVis\METKDemonstrator')
      InstallFolder = _winreg.QueryValueEx(METKDemoSettings,'Install_Dir')[0]
      ctx.log("HKCU_Software\MeVis\METKDemonstrator  exists")
   except EnvironmentError: # so it isn't NT/2000/XP
      try:
         METKDemoSettings = _winreg.OpenKey(_winreg.HKEY_LOCAL_MACHINE, r'Software\MeVis\METKDemonstrator')
         InstallFolder = _winreg.QueryValueEx(METKDemoSettings,'Install_Dir')[0]
         ctx.log("HKLM_Software\MeVis\METKDemonstrator  exists")
      except EnvironmentError: # so it isn't NT/2000/XP
         InstallFolder = relPath(MLABFileManager.getExecutableDir()+"/","../../")
         ctx.log("No install folder found. Setting install folder to "+str(InstallFolder))
      
   caseFile = InstallFolder + "/DemoCases/Hals12/Hals12Demo.xml"
   ctx.log("case file = "+caseFile)
   ctx.field("METKManager.fileName").value = caseFile
   ctx.field("METKManager.load").touch()
   ctx.window().setTitle("METKDemonstrator | " + caseFile)   
   return
   
   

def viewCSO(field = 0):
   cso = ctx.field("METKCSO.cso").value
   if cso!="":
      ctx.log("cso="+cso)
      script = "[INI]\nLENGTHTIMEUNIT=1\nLENGTHSECONDS=2\nREALTIME=YES\n\n"
      script = script + "[SCRIPT]\n[0,1] '"+cso+"' VIEW front 1"
      ctx.field("UMDAnimation2.ScriptArea").value=script
      ctx.field("UMDAnimation2.ScriptAreaExecute").touch()
   return
   
   
   
def initWindow(fieldP=0):
   global _buttonEventFilter1
   _buttonEventFilter1 = ctx.control("ButtonEventFilter")
   
   #Buttons zum Laden
   global bt_demo, bt_browse, bt_exit
   bt_demo = kButton(ctx,ctx.control("bt_demo"),"$(LOCAL)/img/bt_demo-n.png","$(LOCAL)/img/bt_demo-p.png","$(LOCAL)/img/bt_demo-o.png","$(LOCAL)/img/bt_demo-d.png")
   _buttonEventFilter1.addControl(ctx.control("bt_demo"), True)
   bt_browse = kButton(ctx,ctx.control("bt_browse"),"$(LOCAL)/img/bt_browse-n.png","$(LOCAL)/img/bt_browse-p.png","$(LOCAL)/img/bt_browse-o.png","$(LOCAL)/img/bt_browse-d.png")
   _buttonEventFilter1.addControl(ctx.control("bt_browse"), True)
   bt_exit = kButton(ctx,ctx.control("bt_exit"),"$(LOCAL)/img/bt_exit-n.png","$(LOCAL)/img/bt_exit-p.png","$(LOCAL)/img/bt_exit-o.png","$(LOCAL)/img/bt_exit-d.png")
   _buttonEventFilter1.addControl(ctx.control("bt_exit"), True)
   
   #Buttons für Browser
   global bt_sgb, bt_hierarchy
   bt_sgb = kButton(ctx,ctx.control("bt_sgb"),"$(LOCAL)/img/bt_sgb-n.png","$(LOCAL)/img/bt_sgb-p.png","$(LOCAL)/img/bt_sgb-o.png","$(LOCAL)/img/bt_sgb-d.png")
   _buttonEventFilter1.addControl(ctx.control("bt_sgb"), True)
   bt_sgb.setType(BT_BOOL)   
   bt_hierarchy = kButton(ctx,ctx.control("bt_hierarchy"),"$(LOCAL)/img/bt_hierarchy-n.png","$(LOCAL)/img/bt_hierarchy-p.png","$(LOCAL)/img/bt_hierarchy-o.png","$(LOCAL)/img/bt_hierarchy-d.png")
   _buttonEventFilter1.addControl(ctx.control("bt_hierarchy"), True)
   bt_hierarchy.setType(BT_BOOL)
   bt_hierarchy.setState(STATE_DOWN)
   
   #Buttons Collections, CSO
   global bt_cso,bt_collections,bt_addcol
   bt_collections = kButton(ctx,ctx.control("bt_collections"),"$(LOCAL)/img/bt_collections-n.png","$(LOCAL)/img/bt_collections-p.png","$(LOCAL)/img/bt_collections-o.png","$(LOCAL)/img/bt_collections-d.png")
   _buttonEventFilter1.addControl(ctx.control("bt_collections"), True)
   bt_collections.setType(BT_BOOL)   
   bt_cso = kButton(ctx,ctx.control("bt_cso"),"$(LOCAL)/img/bt_cso-n.png","$(LOCAL)/img/bt_cso-p.png","$(LOCAL)/img/bt_cso-o.png","$(LOCAL)/img/bt_cso-d.png")
   _buttonEventFilter1.addControl(ctx.control("bt_cso"), True)
   bt_cso.setType(BT_BOOL)
   bt_cso.setState(STATE_DOWN)
   bt_addcol = kButton(ctx,ctx.control("bt_addcol"),"$(LOCAL)/img/bt_addcol-n.png","$(LOCAL)/img/bt_addcol-p.png","$(LOCAL)/img/bt_addcol-o.png","$(LOCAL)/img/bt_addcol-d.png")
   _buttonEventFilter1.addControl(ctx.control("bt_addcol"), True)
   
   
   global analyzer1
   analyzer1 = kButtonAnalyzer(ctx,_buttonEventFilter1)
   
   
   
   global _tw_browser
   _tw_browser = ctx.control("browser")
   global _tw_bottom
   _tw_bottom = ctx.control("tabbottom")

   
   return
   
def analyzeButtonEventWindow(event):
   analyzer1.analyzeButtonEvent(event)
   return
   
   
def bt_demoClicked(field=0):
   loadDemo()
   return
   
   
def bt_browseClicked(field=0):
   openCase()
   return
   
def bt_exitClicked(field=0):
   exit()
   return

def bt_sgbClicked(fieldP=0):
   _tw_browser.selectTabAtIndex(1)
   bt_sgb.setState(STATE_DOWN)
   bt_hierarchy.setState(STATE_UP)
   return
     
def bt_hierarchyClicked(fieldP=0):
   _tw_browser.selectTabAtIndex(0)
   bt_sgb.setState(STATE_UP)
   bt_hierarchy.setState(STATE_DOWN)
   return
   
   
def bt_csoClicked(fieldP=0):
   _tw_bottom.selectTabAtIndex(0)
   bt_cso.setState(STATE_DOWN)
   bt_collections.setState(STATE_UP)
   return
     
def bt_collectionsClicked(fieldP=0):
   _tw_bottom.selectTabAtIndex(1)
   bt_cso.setState(STATE_UP)
   bt_collections.setState(STATE_DOWN)
   return
   
   
def bt_addcolClicked(fieldP=0):
   ctx.field("METKCollections.create").touch()
   return
   
   
# frag mich mal keiner was dieser mist hier mit der url soll ... 
# aber das war die einzige möglichkeit, aus einem absoluten pfad (c:/aaa/bbb/ccc/) und einer relativen Angabe wie "../" etwas zu zaubern   
def relPath(orgpath, relative):
   (scheme, netloc, path, query, fragment) = urlsplit(urljoin("http://netloc/"+orgpath, relative),"http://",0)   
   return lstrip(path,"/")      
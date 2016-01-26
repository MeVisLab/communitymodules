#######################################################################################
#
#This is a button clas from Konrad Mühler - muehler@isg.cs.uni-magdeburg.de
#
# With the help of this class, images in MeVisLab can be extended to clickable buttons
#
#
#######################################################################################


BT_NORMAL = "ButtonTypeNormal"
BT_BOOL   = "ButtonTypeBool"

STATE_UP = "StateUp"
STATE_DOWN = "StateDown"


class kButtonAnalyzer():
   def __init__(self,parent,filter):
      self.mouseIsIn = False
      self.leftButtonPressed = False
      
      self.buttonEventFilter = filter
      self.parent = parent
      return
   
   
   def analyzeButtonEvent(self,event):
      if (self.parent ==None):
         raise RuntimeError, "parent not set"
         return
      
      if (self.buttonEventFilter==None):
         raise RuntimeError, "buttonEventFilter not set"
         return
      
      buttonName = self.buttonEventFilter.eventControl().getName()
      if (buttonName == ""):
         raise RuntimeError, "Button has no name"
         return
      
      #get state
      btenabled = self.parent.call(buttonName+".getEnabled")
      if (not btenabled):
         return
      
      eventType = event["type"]
      
      #get state
      btstate = self.parent.call(buttonName+".getState")
      #get type
      bttype = self.parent.call(buttonName+".getType")
      
      if (eventType=="MouseButtonRelease" and self.mouseIsIn):
         if (event["button"]=="left"):
            self.leftButtonPressed = False
            if (btstate==STATE_UP and bttype==BT_BOOL):
               self.parent.call(buttonName+".setState",[STATE_DOWN])
            else:
               self.parent.call(buttonName+".setState",[STATE_UP])
               if (self.mouseIsIn):
                  try: self.parent.call(buttonName+".setOver")
                  except: raise RuntimeError, "Can't call setOver for "+buttonName
               else:
                  try: self.parent.call(buttonName+".setNormal")
                  except: raise RuntimeError, "Can't call setNormal for "+buttonName
            
            try: self.parent.call(buttonName+"Clicked")
            except: raise RuntimeError, "Can't call "+buttonName+"Clicked"
      
      
      elif (eventType=="MouseButtonPress"):
         if (event["button"]=="left"):
            self.leftButtonPressed = True
            try: self.parent.call(buttonName+".setDown")
            except: raise RuntimeError, "Can't call "+buttonName+"setDown"
      
      elif (eventType=="Enter"):
         self.mouseIsIn = True
         if (btstate==STATE_UP):
            try: self.parent.call(buttonName+".setOver")
            except: raise RuntimeError, "Can't call "+buttonName+"setOver"
      
      elif (eventType=="Leave"):
         self.mouseIsIn = False
         if (not self.leftButtonPressed and btstate==STATE_UP):
            try: self.parent.call(buttonName+".setNormal")
            except: raise RuntimeError, "Can't call "+buttonName+"setNormal"
      
      return




class kButton():
   
   def __init__(self,parent,imageControl,imageNormal,imageDown,imageOver,imageDisabled):
      self.imageNormal = imageNormal
      self.imageDown = imageDown
      self.imageOver = imageOver
      self.imageDisabled = imageDisabled
      self.control = imageControl
      self.parent = parent
      
      self.state = STATE_UP
      self.type = BT_NORMAL
      self.isEnabled = True
      return
   
   
   def setDown(self):
      if (self.parent==None):
         raise RuntimeError, "parent not set"
         return
      self.control.setImageFile(self.parent.expandFilename(self.imageDown))
      return
   
   def setNormal(self):
      if (self.parent==None):
         raise RuntimeError, "parent not set"
         return
      self.control.setImageFile(self.parent.expandFilename(self.imageNormal))
      return
   
   
   def setDisabled(self):
      if (self.parent==None):
         raise RuntimeError, "parent not set"
         return
      self.control.setImageFile(self.parent.expandFilename(self.imageDisabled))
      return
   
   
   def setOver(self):
      if (self.parent==None):
         raise RuntimeError, "parent not set"
         return
      self.control.setImageFile(self.parent.expandFilename(self.imageOver))
      return
   
   def getState(self):
      return self.state
   
   
   def setState(self,value):
      self.state = value
      if (value==STATE_DOWN):
         self.setDown()
      else:
         self.setNormal()
      return
   
   
   
   def setType(self,value):
      self.type = value
      return
   
   
   def getType(self):
      return self.type
   
   
   def getEnabled(self):
      return self.isEnabled
   
   
   def setEnabled(self,value):
      if (value==True):
         self.setNormal()
      else:
         self.setDisabled()
      self.isEnabled = value
      return
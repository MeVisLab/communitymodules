
from mevis import MLABFileManager
import datetime

class Excel:
    
  _excel = ""
  _stringMode = 0
  _options = {'commas':1,'tabs':0}
    
  #---------------------------------------------------------------------------------------------------------------------------------------
  def __init__(self,options={}):
    self._excel = ""
    for option, value in options.iteritems():
      self._options[option] = value
    
  #---------------------------------------------------------------------------------------------------------------------------------------
  def enableStringMode(self):    
    self._stringMode = 1
    
  #---------------------------------------------------------------------------------------------------------------------------------------
  def writeHeader(self,header):    
    if self._options['tabs']:
      delim = "\t"
    else:
      delim = ";"
      
    for field in header:
      self._excel += field + delim
    self._excel += "\n"
    
  #---------------------------------------------------------------------------------------------------------------------------------------
  def writeValues(self,values): 
    if self._options['tabs']:
      delim = "\t"
    else:
      delim = ";"
      
    for value in values:      
      if not type(value) is unicode and not type(value) is str:
        if self._stringMode:
          self._excel += `value`
        else:
          self._excel += '%.5f' % value
      else:
        self._excel += value  
      self._excel += delim 
    self._excel += "\n"    
  
  #---------------------------------------------------------------------------------------------------------------------------------------
  def save(self,filename=""):
    if not self._excel:
      return
    
    if self._options['commas']:
      self._excel = self._excel.replace(".",",")
  
    if not filename:
      filename = ctx.type() + "-" + datetime.date.today().strftime("%Y-%m-%d")
    file = MLABFileDialog.getSaveFileName(MLABFileManager.getHomeDir() + "/" + filename + ".csv","Excel files (*.csv)","Save file")
    if file:
      MLABFileManager.writeStringToFile(file,self._excel)
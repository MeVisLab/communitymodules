# Author Kerstin Kellermann
import pickle, pprint

#--- global notations -------------------------------------------------------------
_STRUCTURE         = "Structure"
_CASE              = "Case"
_CORREL_NEIGHBOURS = "Neighbours" 
INF_CHARACTER      = "Character"
_TYPE_MALIGNANT    = "Malignant"  # bösartig
_TYPE_BENIGN       = "Benign"     # gutartig
_TYPE_DOUBTFUL     = "Doubtful"   # verdächtig

#----------------------------------------------------------------------------------
#
#   CorrelNetwork class
#
#
class METKObjCorrelation(object):
   """computes correlations of Objects' structures"""   
      
   def __init__(self, module, ctx = None):
      """Initialize object"""      

      if not module: return
      self.module              = module
      if not ctx: self.ctx     = module.parent()
        
      self.fld_success         = self.module.field("success")
      self.fld_DistFile        = self.module.field("fileNameOfDistances")
      self.fld_maxNeighbDist   = self.module.field("maxNeighbourDist") 
      self.fld_applyMaxDist    = self.module.field("applyMaxDist")
      self.fld_structure       = self.module.field("structure")
      self.fld_allStructures   = self.module.field("allStructures")
      self.fld_struct_caseSel  = self.module.field("correl_selection")
      self.fld_correlation     = self.module.field("correlation") 
      self.fld_allCorrelations = self.module.field("allCorrelations") 
  
      self.fld_correlType      = self.module.field("correlationType") 
      self.fld_allCorrelTypes  = self.module.field("allCorrelationType")  
      self.fld_relatedStructs  = self.module.field("relatedStructures") 
      self.fld_firstCorrel     = self.module.field("firstCorrelation") 
      self.fld_nextCorrel      = self.module.field("nextCorrelation") 
      self.fld_firstType       = self.module.field("firstCorrelType") 
      self.fld_nextType        = self.module.field("nextCorrelType") 
      
      self.fld_enableColor     = self.module.field("enableColering")
      return

   def success(self):
      """Check if last operation has been successful. Returns 'True' on success, 'False' otherwise.
         All operations will set an success/error state."""
      return self.fld_success.value
    
   def getDistDict(self):
     distFile = open(self.fld_DistFile.value, 'rb')
     distDict = pickle.load(distFile)
     distFile.close()
     return distDict
   
   def removeDistDictFile(self):
     MLABFileManager.remove(self.fld_DistFile.value)
     return
        
   def setNeighbourMaxDistance(self, distance):
     """setup maximal distance for Neighbourhood-Correlation"""
     self.fld_maxNeighbDist.value = distance
     self.fld_applyMaxDist.touch()
   
   def setDemandStructure(self, structID):
     """setup demand-structure"""
     self.fld_structure.value = structID
#     MLAB.processEvents()
   
   def setCorrelation(self, correlID):
     """setup demand-correlation"""
     self.fld_correlation.value = correlID
     
   def setCorrelationType(self, correlTypeID):
     """setup demand-correlation"""
     self.fld_correlType.value = correlTypeID 
   
   def setCorrelMode(self, selection = _STRUCTURE):
     """setup Selection-Mode.
       0 or 1 Args: ("structure" (default) for structure related and "case" for case related correlation rendition)"""
     if selection == _STRUCTURE or selection == _CASE:
       self.fld_struct_caseSel.value = selection
     else: ctx.log("setCorrelSelection() incorrect argument! possible ()=('_STRUCTURE') or ('_CASE')")
   
   def getAllStructures(self):
     """get a list of all structures in the database resp. in the correlationnetwork"""
     return self.fld_allStructures.value[2:].split(', ')
   
   def getCurrentStruct(self):
     """get ID of currently selected demand-structure"""
     return self.fld_structure.value
   
   def getCorrelations(self, *args):
     """Returns CorrelSelection dependent a list of given demand-structure's avaiable correlations 
        or a list of all listed Correlations of current case. 
        Takes 0 or 1 Arguments: getCorrelations([],[demand-structure])
        INFO: determination of case- or structure-dependency with setCorrelSelection(["structure"],["case"])"""
     if len(args) > 1: 
       ctx.log("getCorrelations() supports 0 or 1 argument")
       return 
     elif len(args) == 1:
       if self.fld_struct_caseSel.value == False:
         ctx.log("getCorrelations() supports only 0 argument with active case-mode-selection.")
         return
       else:
         self.fld_structure.value = args[0]     
     return self.fld_allCorrelations.value[2:].split(', ')

   def getCurrentCorrel(self):
     """returns the currently selected correlation"""
     return self.fld_correlation.value
 
   def getFirstCorrel(self):
     """Get Id of 'first' listed correlation of an demand-structure resp. the case.
        If the CorrelationBase is empty success() will return 'False'.
        INFO: determination of case- or structure-dependency with setCorrelSelection(["structure"],["case"])"""
     self.fld_firstCorrel.touch()
     return self.fld_correlation.value
    
   def getNextCorrel(self):
     """get Id of 'next' listed correlation of an demand-structure resp. the case. 
       If there are no more correlations success() will return 'False'.
       NOTE: The firstObject() function must be called before nextObject().
       INFO: determination of case- or structure-dependency with setCorrelSelection(["structure"],["case"])"""
     self.fld_nextCorrel.touch()
     return self.fld_correlation.value 
   
   def getCorrelTypes(self, *args):
     """Returns a list of the current cases resp. given demand-structure's avaiable correlationsTypes for given correlation. 
        Takes 0, 1 or 2 Arguments: getCorrelType([],[correlation],[demand-structure, correlation])
        INFO: determination of case- or structure-dependency with setCorrelSelection(["structure"],["case"])"""     
     numArgs = len(args)
     if numArgs > 2:
       ctx.log("getCorrelType() supports 0, 1 or 2 arguments")
       return 
     elif numArgs == 1:
       self.fld_correlation.value = args[0]          
     elif numArgs == 2:
       if self.fld_struct_caseSel.value == False:
         ctx.log("getCorrelations() supports only 0 or 1 argument with active case-mode-selection.")
         return
       else:
         self.fld_structure.value   = args[0]
         self.fld_correlation.value = args[1]
     return self.fld_allCorrelTypes.value.split(', ')  
   
   def getCurrentType(self):
     """returns the currently selected correlationType"""
     return self.fld_correlType.value   
   
   def getFirstType(self):
     """Get Id of 'first' listed correlation of an demand-structure resp. the case.
        If the CorrelationBase is empty success() will return 'False'.
        INFO: determination of case- or structure-dependency with setCorrelSelection(["structure"],["case"])"""
     self.fld_firstType.touch()
     return self.fld_correlType.value 
    
   def getNextType(self):
     """get Id of 'next' listed correlation of an demand-structure resp. the case. 
       If there are no more correlations success() will return 'False'.
       NOTE: The firstObject() function must be called before nextObject().
       INFO: determination of case- or structure-dependency with setCorrelSelection(["structure"],["case"])"""
     self.fld_nextType.touch()
     return self.fld_correlType.value  
   
   def getRelatedStructs(self, *args):
     """Returns a list of related Structure of an correlation with an specific correlType.
       Takes 0, 1, 2 or 3 Arguments: 
       getRelatedStruct([], [correlType][correlation, correlType],[demand-structure, correlation, correlType])
       INFO: in a structure dependent correlation rendition, the list is ordered by minimal-distance to demand-structure """
     numArgs = len(args)
     if numArgs > 3:
       ctx.log("getCorrelType() supports 0, 1, 2 or 3 arguments")
       return 
     elif numArgs == 1:
       self.fld_correlType.value = args[0]
     elif numArgs == 2:
       if self.fld_struct_caseSel == _STRUCTURE:
         self.fld_structure.value    = args[0] 
         self.fld_correlation.value  = args[1]
       else:
         self.fld_correlation.value = args[0] 
         self.fld_correlType.value  = args[1]
     elif numArgs == 3:
         self.fld_structure.value   = args[0]
         self.fld_correlation.value = args[1]
         self.fld_correlType.value  = args[2]
     
     if self.fld_relatedStructs.value != "":
       return eval(self.fld_relatedStructs.value)
     else: return False 
      
   def getRelatedStructs1(self):
     """Returns a list of related Structure of an correlation with an specific correlType.
     No argument are necessary but a correlation and CorrelationType has to be seted before this funct
     otherwise use getRelatedStruct()"""
     if self.fld_relatedStructs.value != "":
       return eval(self.fld_relatedStructs.value)
     else: return False 
       
   def enableColering(self):
    self.fld_enableColor.value = True    
    return 
  
    def disableColoring(self):
      self.fld_enableColor.value = True    
      return 
   pass 
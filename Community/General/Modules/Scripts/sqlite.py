
import sqlite3
import datetime
import re

#---------------------------------------------------------------------------------------------------------------------------------------
def sqliteregexp(expr,item):
  r = re.compile('.*' + expr + '.*',re.IGNORECASE)
  return r.match(item) is not None

#---------------------------------------------------------------------------------------------------------------------------------------
def sqliterowfactory(cursor,row):
  d = {}
  for id,col in enumerate(cursor.description):
    d[col[0]] = row[id]
  return d

#---------------------------------------------------------------------------------------------------------------------------------------
class SQLite:

  _pacsTable = {'id':'INTEGER PRIMARY KEY AUTOINCREMENT',
                'patient_name':'TEXT',
                'patient_id':'TEXT',
                'study_id':'TEXT',
                'study_description':'TEXT',
                'study_date':'TEXT',
                'acquisition_date':'TEXT',
                'acquisition_number':'TEXT',
                'protocol':'TEXT',
                'modality':'TEXT',
                'series_number':'TEXT',
                'series_description':'TEXT',
                'submit_date':'TEXT',
                'filename':'TEXT'}                
                 
  _sqlite = None
  
  _highlightColor = ""
  _highlightRe = None

  #---------------------------------------------------------------------------------------------------------------------------------------
  def __init__(self,filename):
    self.openConnection(filename)

  #---------------------------------------------------------------------------------------------------------------------------------------
  def openConnection(self,filename):  
    self._sqlite = sqlite3.connect(filename)
    self._sqlite.isolation_level = None
    self._sqlite.row_factory = sqliterowfactory
    self._sqlite.create_function("REGEXP",2,sqliteregexp)
  
  #---------------------------------------------------------------------------------------------------------------------------------------
  def closeConnection(self):
    if not self._sqlite:
      return
      
    self._sqlite.close()
    self._sqlite = None

  #---------------------------------------------------------------------------------------------------------------------------------------
  def getType(self):
    return "SQLite"
    
  #---------------------------------------------------------------------------------------------------------------------------------------
  def createPacsTable(self):
    if not self._sqlite:
      return
      
    tables = self._sqlite.execute("SELECT * FROM `sqlite_master` WHERE `type`='table'").fetchall()
    for table in tables:
      if table['name'] == 'pacs':
        return

    fields = []
    for field, description in self._pacsTable.iteritems():
      fields.append("`" + field + "` " + description)
    self._sqlite.execute("CREATE TABLE `patients` (" + ", ".join(fields) + ")")
  
  #---------------------------------------------------------------------------------------------------------------------------------------
  def createTable(self,name,table):
    if not self._sqlite:
      return
      
    sqtables = self._sqlite.execute("SELECT * FROM `sqlite_master` WHERE `type`='table'").fetchall()
    for sqtable in sqtables:
      if sqtable['name'] == name:
        return
        
    fields = []
    for field, description in table.iteritems():
      fields.append("`" + field + "` " + description)
    self._sqlite.execute("CREATE TABLE `" + name + "` (" + ", ".join(fields) + ")")

  #---------------------------------------------------------------------------------------------------------------------------------------
  def doesPatientIDExistFile(self,patientID):
    if not self._sqlite:
      return 0
      
    result = self._sqlite.execute("SELECT * FROM `patients` WHERE `id`='" + `patientID` + "'").fetchall()
    return not len(result) == 0

  #---------------------------------------------------------------------------------------------------------------------------------------
  def fetchPatient(self,patient_id,study_id,study_datetime,acquisition_datetime,
                      acquisition_number,series_number):
    if not self._sqlite:
      return []
      
    result = self._sqlite.execute("SELECT * FROM `patients` WHERE `patient_id`='" + patient_id + 
                       "' AND `study_id`='" + study_id + "' AND `study_date`='" +
                       study_datetime + "' AND `acquisition_date`='" + acquisition_datetime + 
                       "' AND `acquisition_number`='" + acquisition_number + 
                       "' AND `series_number`='" + series_number + "'").fetchall()
                       
    if result:
      return result[0]
    else:
      return []
    
  #---------------------------------------------------------------------------------------------------------------------------------------
  def storePatient(self,filename,patient_name,patient_id,study_id,study_description,study_datetime,
                   acquisition_datetime,acquisition_number,protocol,modality,series_number,
                   series_description):
    
    if not self._sqlite:
      return
      
    stored = 0
    if not self.fetchPatient(patient_id,study_id,study_datetime,acquisition_datetime,
                             acquisition_number,series_number):
                                
      now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
      values = [patient_name,patient_id,study_id,study_description,study_datetime,
                acquisition_datetime,acquisition_number,protocol,modality,
                series_number,series_description,now,filename]
      
      self._sqlite.execute("INSERT INTO `patients` " + 
                          "(`patient_name`,`patient_id`,`study_id`,`study_description`,`study_date`," + 
                          "`acquisition_date`,`acquisition_number`,`protocol`,`modality`,`series_number`," + 
                          "`series_description`,`submit_date`,`filename`) " + 
                          "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?)",values)
      stored = 1
      
    return stored
  
  #---------------------------------------------------------------------------------------------------------------------------------------
  def queryStore(self,table,query,fields,values):
    where = []
    for field,value in query.iteritems():
      where.append("`" + field + "`='" + value + "'")
    where = " AND ".join(where)
    
    ID = 0
      
    result = self.query("SELECT `id` FROM `" + table + "` WHERE " + where)
    if len(result):
      ID = result[0]['id']
    else:
      self.store(table,fields,values)
      result = self.query("SELECT `id` FROM `" + table + "` WHERE " + where)
      if len(result):
        ID = result[0]['id']
      else: 
        print("Error in queryStore")
        
    return ID
    
  #---------------------------------------------------------------------------------------------------------------------------------------
  def store(self,table,fields,values):
    query = "INSERT INTO `" + table + "` ("
    
    tableFields = []
    questions = []
    for field in fields:
      tableFields.append("`" + field + "`")
      questions.append("?")
    
    query += ",".join(tableFields) + ") VALUES (" + ",".join(questions) + ")"
                  
    self._sqlite.execute(query,values)

  #---------------------------------------------------------------------------------------------------------------------------------------
  def fetchLatest(self,table,limit=50):
    if not self._sqlite:
      return
      
    results = self._sqlite.execute("SELECT * FROM `" + table + "` ORDER BY `submit_date` DESC LIMIT 0," + `limit`).fetchall()
    return results
  
  #---------------------------------------------------------------------------------------------------------------------------------------
  def query(self,query):
    if not self._sqlite:
      return
      
    results = self._sqlite.execute(query).fetchall()
    return results
    
  #---------------------------------------------------------------------------------------------------------------------------------------
  def execute(self,query):
    if not self._sqlite:
      return
      
    self._sqlite.execute(query)
    
  #---------------------------------------------------------------------------------------------------------------------------------------
  def search(self,searchString,table,searchFields,color=""):
    if not self._sqlite:
      return []
      
    searchWords = searchString.strip().split(" ")
    
    where = []  
    for searchWord in searchWords:
      subwhere = []
      
      for field in searchFields:
        subwhere.append("`" + field + "` REGEXP '" + searchWord + "'")
          
      where.append("(" + " OR ".join(subwhere) + ")")
    
    query = "SELECT * FROM `" + table + "` WHERE " + " AND ".join(where)
    
    results = self._sqlite.execute(query).fetchall()
        
    if color:      
      if not color[0] == "#":
        colors = color.split(" ")
        color = "#"
        for c in colors:
          c = float(c)*255
          color += '%02x' % c
      
      self._highlightColor = color
        
      self._highlightRe = re.compile('(' + "|".join(searchWords) + ')',re.IGNORECASE)
      for i in range(len(results)):
        result = results[i]
        highlights = {}
        for field,value in result.iteritems():
          if type(value) is str or type(value) is unicode:
            tvalue = self._highlightRe.sub(r'<font color=__COLOR__>\1</font>',value)
            highlights['highlight_' + field] = re.sub('__COLOR__',self._highlightColor,tvalue)
        for field,value in highlights.iteritems():
          result[field] = value
    else:
      self._highlightRe = None
      self._highlightColor = ""

    return results
  
  #---------------------------------------------------------------------------------------------------------------------------------------
  def highlight(self,string,color=""):
    if not self._sqlite:
      return string

    hstring = string

    if color:      
      if not color[0] == "#":
        colors = color.split(" ")
        color = "#"
        for c in colors:
          c = float(c)*255
          color += '%02x' % c
      
      self._highlightColor = color
        
    if self._highlightRe and self._highlightColor:
      hstring = self._highlightRe.sub(r'<font color=__COLOR__>\1</font>',string)
      hstring = re.sub('__COLOR__',self._highlightColor,hstring)
      
    return hstring
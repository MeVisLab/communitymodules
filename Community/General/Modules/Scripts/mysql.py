
from mevis import MLAB
import datetime
import socket
import re

#---------------------------------------------------------------------------------------------------------------------------------------
class MySQL:  
  _mysql = None
  _db = None
  _cursor = None
  
  _highlightColor = ""
  _highlightRe = None

  #---------------------------------------------------------------------------------------------------------------------------------------
  def __init__(self,host,user,password,db):
    self.openConnection(host,user,password,db)

  #---------------------------------------------------------------------------------------------------------------------------------------
  def openConnection(self,host,user,password,db):      
    if self._mysql and self._mysql.isOpen():
      self.closeConnection()
      
    self._mysql = MLAB.addSqlDatabase('Mysql')
    self._mysql.setHostName(host)
    self._mysql.setUserName(user)
    self._mysql.setPassword(password)
    self._mysql.setDatabaseName(db)
    self._mysql.setPort(3306)
    self._mysql.open()
    
    valid = self._mysql.isOpen()
    if not valid:
      self.closeConnection()
  
  #---------------------------------------------------------------------------------------------------------------------------------------
  def closeConnection(self):
    if not self._mysql:
      return
      
    self._mysql.close()
    MLAB.removeSqlDatabase(self._mysql)
    self._mysql = None

  #---------------------------------------------------------------------------------------------------------------------------------------
  def getType(self):
    return "MySQL"
    
  #---------------------------------------------------------------------------------------------------------------------------------------
  def store(self,table,fields,values):
    query = self._mysql.createQuery()
    
    tableFields = []
    questions = []
    for field in fields:
      tableFields.append("`" + field + "`")
      questions.append("?")
    
    query.prepare("INSERT INTO `" + table + "` (" + ",".join(tableFields) + 
                  ") VALUES (" + ",".join(questions) + ")")
                  
    for i in range(len(values)):
      query.bindValue(i,values[i])
      
    stored = query.execQuery()    
    ID = query.lastInsertId()    
    self._mysql.removeQuery(query)
    
    return ID    

  #---------------------------------------------------------------------------------------------------------------------------------------
  def queryStore(self,table,query,fields,values):
    where = []
    for field,value in query.iteritems():
      where.append("`" + field + "`='" + value + "'")
    where = " AND ".join(where)
    
    ID = 0
      
    result = self.query("SELECT `ID` FROM `" + table + "` WHERE " + where)
    if len(results):
      ID = int(results[0]['ID'])
    else:
      ID = self.store(table,fields,values)
        
    return ID
  
  #---------------------------------------------------------------------------------------------------------------------------------------
  def execute(self,query):
    self._mysql.query(query)

  #---------------------------------------------------------------------------------------------------------------------------------------
  def fetchLatest(self,table,field,limit=50):
    results = self._mysql.query("SELECT * FROM `" + table + "` ORDER BY `" + field + 
                                "` DESC LIMIT 0," + `limit`)
    if results == -1:
      results = []
    else:
      for i in range(len(results)):
        result = results[i]
        for field, value in result.iteritems():
          if field[-5:] == "_date":
            if value:
              results[i][field] = value.toString("yyyy-MM-dd hh:mm:ss")
            else:
              results[i][field] = "0000-00-00 00:00:00"
    
    return results  
      
  #---------------------------------------------------------------------------------------------------------------------------------------
  def query(self,query):
    results = self._mysql.query(query)
    if results == 1:
      results = []
    elif results == -1:
      results = []
    else:
      for i in range(len(results)):
        result = results[i]
        for field, value in result.iteritems():
          if field[-5:] == "_date":
            if value:
              results[i][field] = value.toString("yyyy-MM-dd hh:mm:ss")
            else:
              results[i][field] = "0000-00-00 00:00:00"
            
    return results
      
  #---------------------------------------------------------------------------------------------------------------------------------------
  def searchMultiple(self,searchString,tables,condition,searchFields,color=""):
    if not self._mysql:
      return []
      
    searchWords = searchString.strip().split(" ")
    
    where = []  
    for searchWord in searchWords:
      subwhere = []
      
      for field in searchFields:
        subwhere.append(field + " REGEXP '" + searchWord + "'")
          
      where.append("(" + " OR ".join(subwhere) + ")")
          
    query = "SELECT * FROM " + tables + " WHERE " + condition + " AND " + " AND ".join(where)
    
    return self._search(query,searchWords,color)
    
  #---------------------------------------------------------------------------------------------------------------------------------------
  def search(self,searchString,table,searchFields,color=""):
    if not self._mysql:
      return []
      
    searchWords = searchString.strip().split(" ")
    
    where = []  
    for searchWord in searchWords:
      subwhere = []
      
      for field in searchFields:
        subwhere.append("`" + field + "` REGEXP '" + searchWord + "'")
          
      where.append("(" + " OR ".join(subwhere) + ")")
          
    query = "SELECT * FROM `" + table + "` WHERE " + " AND ".join(where)
    
    return self._search(query,searchWords,color)
    
  #---------------------------------------------------------------------------------------------------------------------------------------
  def _search(self,query,searchWords,color=""):
    results = self._mysql.query(query)    
    if results == -1:
      results = []
    else:
      for i in range(len(results)):
        result = results[i]
        for field, value in result.iteritems():
          if field[-5:] == "_date":
            if value:
              results[i][field] = value.toString("yyyy-MM-dd hh:mm:ss")
            else:
              results[i][field] = "0000-00-00 00:00:00"
        
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
    if not self._mysql:
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
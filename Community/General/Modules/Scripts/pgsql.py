
from mevis import MLAB
import datetime
import socket
import re

#---------------------------------------------------------------------------------------------------------------------------------------
class PostgreSQL:  
  _pgsql = None
  _db = None
  _cursor = None
  
  _highlightColor = ""
  _highlightRe = None

  #---------------------------------------------------------------------------------------------------------------------------------------
  def __init__(self,host,user,password,db):
    self.openConnection(host,user,password,db)

  #---------------------------------------------------------------------------------------------------------------------------------------
  def openConnection(self,host,user,password,db):      
    if self._pgsql and self._pgsql.isOpen():
      self.closeConnection()
      
    self._pgsql = MLAB.addSqlDatabase('Postgres')
    self._pgsql.setHostName(host)
    self._pgsql.setUserName(user)
    self._pgsql.setPassword(password)
    self._pgsql.setDatabaseName(db)
    self._pgsql.setPort(5432)
    self._pgsql.open()
    
    valid = self._pgsql.isOpen()
    if not valid:
      print("could not open")
      self.closeConnection()
  
  #---------------------------------------------------------------------------------------------------------------------------------------
  def closeConnection(self):
    if not self._pgsql:
      return
      
    self._pgsql.close()
    MLAB.removeSqlDatabase(self._pgsql)
    self._pgsql = None

  #---------------------------------------------------------------------------------------------------------------------------------------
  def getType(self):
    return "PostgreSQL"
    
  #---------------------------------------------------------------------------------------------------------------------------------------
  def store(self,table,fields,values):
    query = self._pgsql.createQuery()
    
    questions = []
    for field in fields:
      questions.append("?")
    
    query.prepare("INSERT INTO " + table + " (" + ",".join(fields) + 
                  ") VALUES (" + ",".join(questions) + ")")
                  
    for i in range(len(values)):
      query.bindValue(i,values[i])
      
    query.execQuery()  
    currval = self._pgsql.query("SELECT currval('" + table + "_id_seq')")
    ID = int(currval[0]['currval'])
    self._pgsql.removeQuery(query)
    
    return ID    

  #---------------------------------------------------------------------------------------------------------------------------------------
  def queryStore(self,table,query,fields,values):
    where = []
    for field,value in query.iteritems():
      where.append(field + "='" + value + "'")
    where = " AND ".join(where)
    
    ID = 0
      
    result = self.query("SELECT id FROM " + table + " WHERE " + where)
    if len(result):
      ID = int(result[0]['id'])
    else:
      ID = self.store(table,fields,values)
        
    return ID
  
  #---------------------------------------------------------------------------------------------------------------------------------------
  def execute(self,query):
    self._pgsql.query(query)

  #---------------------------------------------------------------------------------------------------------------------------------------
  def commit(self):
    self._pgsql.commit()

  #---------------------------------------------------------------------------------------------------------------------------------------
  def fetchLatest(self,table,field,limit=50):
    results = self._pgsql.query("SELECT * FROM " + table + " ORDER BY " + field + 
                                " DESC LIMIT 0," + `limit`)
    if results == -1:
      results = []
    else:
      for i in range(len(results)):
        result = results[i]
        for field, value in result.iteritems():
          if field[-5:] == "_date":
            if value == "":
              results[i][field] = "0000-00-00 00:00:00"
    
    return results  
      
  #---------------------------------------------------------------------------------------------------------------------------------------
  def query(self,query):
    results = self._pgsql.query(query)
    if results == 1:
      results = []
    elif results == -1:
      results = []
    else:
      for i in range(len(results)):
        result = results[i]
        for field, value in result.iteritems():
          if field[-5:] == "_date":
            if value == "":
              results[i][field] = "0000-00-00 00:00:00"
            
    return results
      
  #---------------------------------------------------------------------------------------------------------------------------------------
  def searchMultiple(self,searchString,tables,condition,searchFields,color="",aliases=""):
    if not self._pgsql:
      return []
      
    searchWords = searchString.strip().split(" ")
    
    where = []  
    for searchWord in searchWords:
      subwhere = []
      
      for field in searchFields:
        subwhere.append(field + " ~* '" + searchWord + "'")
          
      where.append("(" + " OR ".join(subwhere) + ")")
          
    query = "SELECT *" + aliases + " FROM " + tables + " WHERE "
    if condition:
      query += condition + " AND "
    query += " AND ".join(where)
    
    return self._search(query,searchWords,color)
    
  #---------------------------------------------------------------------------------------------------------------------------------------
  def search(self,searchString,table,searchFields,color=""):
    if not self._pgsql:
      return []
      
    searchWords = searchString.strip().split(" ")
    
    where = []  
    for searchWord in searchWords:
      subwhere = []
      
      for field in searchFields:
        subwhere.append(field + " ~* '" + searchWord + "'")
          
      where.append("(" + " OR ".join(subwhere) + ")")
          
    query = "SELECT * FROM " + table + " WHERE " + " AND ".join(where)
    
    return self._search(query,searchWords,color)
    
  #---------------------------------------------------------------------------------------------------------------------------------------
  def _search(self,query,searchWords,color=""):
    results = self._pgsql.query(query)    
    if results == -1:
      results = []
    else:
      for i in range(len(results)):
        result = results[i]
        for field, value in result.iteritems():
          if field[-5:] == "_date":
            if value == "":
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
    if not self._pgsql:
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

import smtplib
import MimeWriter
import mimetools
import cStringIO
import re

#---------------------------------------------------------------------------------------------------------------------------------------
class HTMLMail:
  _smtpserver = ""
  _source = ""
  _destination = ""
  _subject = ""
  _html = ""
  _ascii = ""

  #---------------------------------------------------------------------------------------------------------------------------------------
  def __init__(self,smtpserver="",source="",destination="",subject="",html="",ascii=""):
    self.setSTMPServer(smtpserver)
    self.setSource(source)
    self.setDestination(destination)
    self.setSubject(subject)
    self.setMessage(html, ascii)

  #---------------------------------------------------------------------------------------------------------------------------------------
  def setSMTPServer(self,smtpserver):
    self._smtpserver = smtpserver

    #---------------------------------------------------------------------------------------------------------------------------------------
  def setSource(self,source):
    self._source = source
    
  #---------------------------------------------------------------------------------------------------------------------------------------
  def setDestination(self,destination):
    self._destination = destination
    
  #---------------------------------------------------------------------------------------------------------------------------------------
  def setSubject(self,subject):
    self._subject = subject
    
  #---------------------------------------------------------------------------------------------------------------------------------------
  def setMessage(self,html,ascii=""):
    self._html = html
    self._ascii = ascii
    
  #---------------------------------------------------------------------------------------------------------------------------------------
  def send(self):  
    if not self._smptserver or not self._source or not self._destination or not self._subject or not self._html:
      return
      
    msg = self._createHTMLMail(self._source,self._destination,self._subject,self._html,self._ascii)
    server = smtplib.SMTP(self._smtpserver)
    _from = re.findall('\<(.*?)\>',self._source)
    if _from:
      server.sendmail(_from[0],self._destination,msg)
    else:
      server.sendmail(self._source,self._destination,msg)
    server.quit()

  #---------------------------------------------------------------------------------------------------------------------------------------
  def _createHTMLMail(self,sender,recipient,subject,html,text):
    out = cStringIO.StringIO()
    
    writer = MimeWriter.MimeWriter(out)
    
    writer.addheader("From",sender)
    writer.addheader("To",recipient)
    writer.addheader("Subject",subject)
    writer.addheader("MIME-Version","1.0")
    writer.startmultipartbody("alternative")
    writer.flushheaders()
  
    if text:
      textin = cStringIO.StringIO(text)
      subpart = writer.nextpart()
      subpart.addheader("Content-Transfer-Encoding","quoted-printable")
      pout = subpart.startbody("text/plain",[("charset", "us-ascii")])
      mimetools.encode(textin, pout,"quoted-printable")
      textin.close()
    
    htmlin = cStringIO.StringIO(html)
    subpart = writer.nextpart()
    subpart.addheader("Content-Transfer-Encoding","quoted-printable")
    pout = subpart.startbody("text/html",[("charset", "us-ascii")])
    mimetools.encode(htmlin, pout,"quoted-printable")
    htmlin.close()
    
    writer.lastpart()
    msg = out.getvalue()
    out.close()
    
    return msg
#----------------------------------------------------------------------------------
#
# Copyright (c) 2011, Image Sciences Institute, UMC Utrecht.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of ISI nor the names of its contributors 
#       may be used to endorse or promote products derived from this software 
#       without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL ISI BE LIABLE FOR ANY DIRECT, INDIRECT, 
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#----------------------------------------------------------------------------------

from mevis import *
import smtplib
import mimetypes

# Here are the email package modules we'll need

from optparse import OptionParser
from email import encoders
from email.message import Message
from email.mime.audio import MIMEAudio
from email.mime.base import MIMEBase
from email.mime.image import MIMEImage
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText

def sendEmail():
  # Credentials (if needed)  
  username = ctx.field("serverUsername").value;
  password = ctx.field("serverPassword").value;
  
  server = ctx.field("serverAddress").value
  serverPort = ctx.field("serverPort").value;
  serverString = server + ":" + str(serverPort);
  
  ctx.field("status").value = "Creating message...";
  
  msg = createMessage();
  
  
  # The actual mail send  
  try:
    ctx.field("status").value = "Connecting...";
  
    if (ctx.field("serverAuthentication").value == 'SSL'):
      server = smtplib.SMTP_SSL(serverString);
    else :
      server = smtplib.SMTP(serverString);
    
    if (ctx.field("serverAuthentication").value == 'TLS'):
      server.starttls();
    ctx.field("status").value = "Connecting...";  
    if (ctx.field("serverAuthentication").value != "None"):
      server.login(username,password);
    ctx.field("status").value = "Connection finished";  
    
    ctx.field("status").value = "Sending mail...";
    server.sendmail(ctx.field("fromAddress").value, ctx.field("toAddress").value, msg.as_string());
    ctx.field("status").value = "Sending mail...finished";
    server.quit()  
  except smtplib.SMTPConnectError:
    ctx.field("status").value = "Connection failed (SMTPConnectError)";
  except smtplib.SMTPAuthenticationError:
    ctx.field("status").value = "Connection failed (SMTPAuthenticationError)";
  except smtplib.SMTPRecipientsRefused:
    ctx.field("status").value = "Sending failed (SMTPRecipientsRefused)";
  except smtplib.SMTPHeloError:
    ctx.field("status").value = "Sending failed (SMTPHeloError)";
  except smtplib.SMTPSenderRefused:
    ctx.field("status").value = "Sending failed (SMTPSenderRefused)";
  except smtplib.SMTPDataError:
    ctx.field("status").value = "Sending failed (SMTPDataError)";
    
def createMessage():
  # Create the container (outer) email message.
  msg = MIMEMultipart()
  msg['Subject'] = ctx.field("subject").value;
  msg['From'] = ctx.field("fromAddress").value;
  msg['To'] = ctx.field("toAddress").value;
  msg.preamble = ctx.field("subject").value;
  
  if len(ctx.field("attachment").value) >0:
    attachFile(msg, ctx.field("attachment").value);
  
  bodyTxt = ctx.field("body").value;
  bodyTxt += "\n\nThis message was sent by MeVisLab"
  
  body = MIMEText(bodyTxt, 'plain')
  
  
  msg.attach(body)

  return msg;

def attachFile(body, fn):
  #print "trying to attach " + fn;
  ctype, encoding = mimetypes.guess_type(fn);
  if ctype is None or encoding is not None:
      # No guess could be made, or the file is encoded (compressed), so
      # use a generic bag-of-bits type.
      ctype = 'application/octet-stream';
  maintype, subtype = ctype.split('/', 1);
  if maintype == 'text':
      fp = open(fn);
      # Note: we should handle calculating the charset
      msg = MIMEText(fp.read(), _subtype=subtype);
      fp.close();
  elif maintype == 'image':
      fp = open(fn, 'rb');
      msg = MIMEImage(fp.read(), _subtype=subtype);
      fp.close();
  elif maintype == 'audio':
      fp = open(fn, 'rb');
      msg = MIMEAudio(fp.read(), _subtype=subtype);
      fp.close();
  else:
      fp = open(fn, 'rb');
      msg = MIMEBase(maintype, subtype);
      msg.set_payload(fp.read());
      fp.close();
      # Encode the payload using Base64
      encoders.encode_base64(msg);
  # Set the filename parameter
  name = fn.split("/")[-1].split("\\")[-1];
  msg.add_header('Content-Disposition', 'attachment', filename=name);
  body.attach(msg);
  
def serverSettingsChanged():
  if ctx.field("serverSettings").value == 'UMCUtrecht':
    ctx.field("serverAddress").value = 'mail.umcutrecht.nl';
    ctx.field("serverPort").value = 25;
    ctx.field("serverAuthentication").value = 'None';  
  elif ctx.field("serverSettings").value == 'GMail':
    ctx.field("serverAddress").value = 'smtp.gmail.com';
    ctx.field("serverPort").value = 587;
    ctx.field("serverAuthentication").value = 'TLS';
  elif ctx.field("serverSettings").value == 'Other':
    ctx.field("serverAddress").value = 'smtp.exampledomain.com';
    ctx.field("serverPort").value = 25;
    ctx.field("serverAuthentication").value = 'None';
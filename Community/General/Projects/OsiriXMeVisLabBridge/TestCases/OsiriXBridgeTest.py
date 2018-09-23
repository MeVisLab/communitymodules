# -*- coding: utf-8 -*-

# **InsertLicense** code author="Felix Ritter"

from mevis import *
from TestSupport import Base, Fields, Logging, ScreenShot
from TestSupport.Macros import *
from threading import Thread
import subprocess, time, os, sys, tempfile
import xmlrpc.client, socket


kOsiriXBundleId = 'com.rossetantoine.osirix'
kOsiriXDBDirName = 'OsiriX Data'
kOsiriXSupportDirName = 'OsiriX'

kHorosBundleId = 'org.horosproject.horos'
kHorosDBDirName = 'Horos Data'
kHorosSupportDirName = 'Horos'

if False:
  kAppBundleId = kOsiriXBundleId
  kAppDBDirName = kOsiriXDBDirName
  kAppSupportDirName = kOsiriXSupportDirName
else:
  kAppBundleId = kHorosBundleId
  kAppDBDirName = kHorosDBDirName
  kAppSupportDirName = kHorosSupportDirName


myctx = None
myTestDirPath = None

def mayRunTest ():
  return (sys.platform == 'darwin') and MLAB.hasVariable('TESTCENTER_EXTERNAL_DATA_FME') and MLABFileManager.macGetApplicationForId(kAppBundleId)

def execute (command, wait=True):
  returnCode = None
  Logging.info('Calling "' + command + '"')
  process = subprocess.Popen(command, shell=True, env={})
  if wait == True:
    returnCode = process.wait()
  return returnCode


class OsiriXXMLRPCThread (Thread):
  def __init__ (self, func):
    Thread.__init__(self)
    self.OsiriX = xmlrpc.client.ServerProxy('http://localhost:8080/')
    self.func = func
    self.result = None
  def run (self):
    self.result = eval (self.func)
  def waitForFinished (self):
    while Thread.isAlive(self):
      MLAB.processEvents()
      time.sleep(0.050)
     

def setUpTestCase ():
  global myctx, myTestDirPath
  
  if not mayRunTest():
    Logging.info('Test is not allowed to run')
    return
  
  if not socket.gethostname().startswith(('fritter-pc.', 'fritter-nb.')):
    Logging.info('Installing OsiriX plugin...')
    # create destination folder
    execute('/bin/mkdir -p \'%s/Library/Application Support/%s/Plugins\'' % (os.path.expanduser('~'), kAppSupportDirName))
    result = execute('/usr/bin/rsync -av --delete \'%s/Applications/MeVisLab.app/Contents/Packages/FMEwork/ReleaseMeVis/bin/PlugIns/OsiriXMeVisLabBridge.osirixplugin\' \'%s/Library/Application Support/%s/Plugins/\'' % (os.path.expanduser('~'), os.path.expanduser('~'), kAppSupportDirName))
    Logging.info('done.')
    
    ASSERT_EQ(result, 0, 'Return code of rsync')
  else:
    Logging.info('Using already installed OsiriX plugin...')
    
  Logging.info('Creating OsiriX database parent folder...')
  # make sure we have exotic characters and spaces in the path
  myTestDirPath = tempfile.mkdtemp(prefix=u'\u4500 ')
  Logging.info(myTestDirPath)
  Logging.info('done.')
  
  Logging.info('Starting OsiriX...')
  result = execute('/usr/bin/osascript -e \'tell application id "' + kAppBundleId + '" to activate\'')
  Logging.info('done.')
  
  ASSERT_EQ(result, 0, 'Return code of osascript')
  
  Logging.info('Initializing OsiriX database: OsiriX.OpenDB( {"path": "%s"} )' % myTestDirPath)
  t = OsiriXXMLRPCThread('''self.OsiriX.OpenDB( {"path": "''' + myTestDirPath + '''"} )''')
  t.start()
  t.waitForFinished()
  Logging.info('Result: ' + str(t.result))

  ASSERT_EQ(int(t.result['error']), 0, 'Result of OpenDB')

  Logging.info('Loading test network...')
  myctx = MLAB.priv().loadNetworkFromFile(ctx.localPath() + '/OsiriXBridgeTest_manual.mlab').lookupModule('macro')
  Logging.info('done.')
  
  ASSERT_NE(myctx, 0, 'MeVisLab network context')
  
  return

def tearDownTestCase ():
  if not mayRunTest():
    return
  
  Logging.info('Closing OsiriX database: OsiriX.CloseDB( {"path": "%s"} )' % myTestDirPath)
  t = OsiriXXMLRPCThread('''self.OsiriX.CloseDB( {"path": "''' + myTestDirPath + '''"} )''')
  t.start()
  t.waitForFinished()
  Logging.info('Result: ' + str(t.result))

  ASSERT_EQ(int(t.result['error']), 0, 'Result of CloseDB')
  
  Logging.info('Quitting OsiriX...')
  execute('/usr/bin/osascript -e \'tell application id "' + kAppBundleId + '" to quit\'')
  Logging.info('done.')
  
  if myTestDirPath:
    Logging.info('Removing OsiriX database parent folder...')
    execute('/bin/rm -fvr \'%s\'' % myTestDirPath)
    Logging.info('done.')
  
  return

def TEST001_FilterChain ():
  if not mayRunTest():
    return
  
  try:

    # Check client id
    Logging.info('Checking reported client application bundle id...')
    appId = myctx.field('OsiriXBridge.clientAppBundleId').value
    Logging.info('Connected to %s' % (appId))
    Logging.info('done.')
    
    ASSERT_EQ(appId, kAppBundleId, 'Check for reported client id')
    
    # Import test series
    localTestSeriesPath = os.path.join(MLAB.variable('TESTCENTER_EXTERNAL_DATA_FME'), 'MeVisLabModules/Release/OsiriXBridge/Test_Patient/')
    result = execute('/usr/bin/ditto \'%s\' \'%s/%s/INCOMING.noindex/\'' % (localTestSeriesPath, myTestDirPath, kAppDBDirName))

    ASSERT_EQ(result, 0, 'Return code of ditto')

    # Grant some time for receiving images
    for _ in range(300):
      MLAB.processEvents()
      time.sleep(0.050)
    
    # Select test series
    Logging.info('OsiriX.DBWindowFind( {"request": "(name LIKE \'*Test*\')", "table": "Study", "execute": "Select"} )')
    t = OsiriXXMLRPCThread('''self.OsiriX.DBWindowFind( {"request": "(name LIKE '*Test*')", "table": "Study", "execute": "Select"} )''')
    t.start()
    t.waitForFinished()
    Logging.info('Result: ' + str(t.result))
    
    ASSERT_EQ(int(t.result['error']), 0, 'Result of DBWindowFind')
    ASSERT_EQ(int(t.result['elements'][0]['numberOfImages']), 64, 'Number of images in study')

    # Send selected series to MeVisLab
    Logging.info('OsiriX.SendSelectedSeriesToMeVisLab( {"target": "TestTarget"} )')
    t = OsiriXXMLRPCThread('''self.OsiriX.SendSelectedSeriesToMeVisLab( {"target": "TestTarget"} )''')
    t.start()
    t.waitForFinished()
    Logging.info('Result: ' + str(t.result))
    
    ASSERT_EQ(int(t.result['error']), 0, 'Result of SendSelectedSeriesToMeVisLab')
    
    # Grant some time for receiving images
    for _ in range(300):
      MLAB.processEvents()
      time.sleep(0.050)
      
    # Select complete series
    Logging.info('OsiriX.DBWindowFind( {"request": "(name LIKE \'*Test*\')", "table": "Study", "execute": "Select"} )')
    t = OsiriXXMLRPCThread('''self.OsiriX.DBWindowFind( {"request": "(name LIKE '*Test*')", "table": "Study", "execute": "Select"} )''')
    t.start()
    t.waitForFinished()
    Logging.info('Result: ' + str(t.result))
    
    ASSERT_EQ(int(t.result['error']), 0, 'Result of DBWindowFind')
    ASSERT_EQ(int(t.result['elements'][0]['numberOfImages']), 128, 'Number of images in study')

    # Delete test series
    Logging.info('OsiriX.DBWindowFind( {"request": "(name LIKE \'*Test*\')", "table": "Study", "execute": "Delete"} )')
    t = OsiriXXMLRPCThread('''self.OsiriX.DBWindowFind( {"request": "(name LIKE '*Test*')", "table": "Study", "execute": "Delete"} )''')
    t.start()
    t.waitForFinished()
    Logging.info('Result: ' + str(t.result))
    
    ASSERT_EQ(int(t.result['error']), 0, 'Result of DBWindowFind')

  except (xmlrpc.client.Error, socket.error) as err:
    print ("ERROR", err)
  return

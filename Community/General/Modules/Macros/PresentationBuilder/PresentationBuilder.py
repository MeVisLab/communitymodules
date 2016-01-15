
from mevis import MLAB
from mevis import MLABFileManager
import re

_presentations = {}
_slideNames = []
_slideBullets = {}
_slideModules = {}
_styles = {}
_styleTags = {}

#------------------------------------------------------------------------------------
def initialize(field=None):  
  global _styles
  _styles = {}
  
  cb = ctx.control("styles")
  cb.clearItems()
  cb.insertItem("")
  
  stylePath = ctx.localPath() + "/Styles/"
  
  contents = MLABFileManager.contentOfDirectory(stylePath)
  for file in contents:
    if MLABFileManager.isDir(stylePath + file):
      if MLABFileManager.exists(stylePath + file + "/style") and \
         MLABFileManager.exists(stylePath + file + "/tags") and \
         MLABFileManager.exists(stylePath + file + "/template") and \
         MLABFileManager.exists(stylePath + file + "/style.png"):
         
        cb.insertItem(file)
        _styles[file] = stylePath + file + "/"
        
  if _styles:
    cb.setCurrentItem(0)
    ctx.field("style").setStringValue(cb.currentText())
    selectStyle()
    
  global _presentations
  _presentations = {}
  
  cb = ctx.control("presentations")
  cb.clearItems()
  cb.insertItem("")
  
  presentationsPath = ctx.localPath() + "/Presentations/"
  contents = MLABFileManager.contentOfDirectory(presentationsPath)
  for file in contents:
    if MLABFileManager.isDir(presentationsPath + file):
      if MLABFileManager.exists(presentationsPath + file + "/config") and \
         MLABFileManager.exists(presentationsPath + file + "/slides"):
         
        cb.insertItem(file)        
        _presentations[file] = presentationsPath + file + "/"
  
#------------------------------------------------------------------------------------
def selectStyle(field=None):
  style = ctx.field("style").stringValue()
  string = ""
  
  if not style == "":
    string = "Image { image = " + ctx.localPath() + "/Styles/" + style + "/style.png }"

  ctx.control("styleImages").setContentString(string)

#------------------------------------------------------------------------------------
def selectPresentation(field=None):
  presentation = ctx.field("presentation").stringValue()
  header = ""
  footer = ""
  module = ""
  style = ""
  
  if presentation:
    config = MLABFileManager.readStringFromFile(_presentations[presentation] + "config").split("\n")
    for line in config:
      parts = line.split("=")
      if not len(parts) == 2: continue
        
      tag = parts[0].strip()
      value = parts[1].strip()
      
      if tag == "HEADER":
        header = value
      elif tag == "FOOTER":
        footer = value
      elif tag == "STYLE":
        if _styles.has_key(value):
          style = value
      elif tag == "MODULE":
        module = value    
  
  ctx.field("header").setStringValue(header)
  ctx.field("footer").setStringValue(footer)
  ctx.field("module").setStringValue(module)
  ctx.field("style").setStringValue(style)
  
  selectStyle()

#------------------------------------------------------------------------------------
def buildPresentation():

  #set style
  style = ctx.field("style").stringValue()
  
  # check/delete module path
  localPath = ctx.localPath() + "/"
  stylePath = localPath + "Styles/" + style
  contentPath = localPath + "Presentations/" + ctx.field("presentation").stringValue()
  moduleName = ctx.field("module").stringValue()
  modulePath = localPath + "/Generated/" + moduleName
  
  # create directories
  MLABFileManager.mkdir(modulePath)
  MLABFileManager.mkdir(modulePath + "/style")
  MLABFileManager.mkdir(modulePath + "/data")
  MLABFileManager.mkdir(modulePath + "/macros")
  MLABFileManager.mkdir(modulePath + "/scripts")
  
  slashRe = re.compile('(.*?)[\/]{3,}',re.DOTALL)
    
  #parse style.script
  global _styleTags
  _styleTags = {}
  
  if MLABFileManager.exists(stylePath + "/tags"):
    tagRe = re.compile('\[(.*?)\]\s*=\s*(.*)',re.DOTALL)
    content = MLABFileManager.readStringFromFile(stylePath + "/tags")
    matches = slashRe.findall(content)
    for styleTag in matches:
      if styleTag == "": continue
      tagMatches = tagRe.findall(styleTag)      
      _styleTags[tagMatches[0][0]] = tagMatches[0][1]
      
  # generate content: slides
  global _slideBullets
  _slideBullets = {}
  global _slideNames
  _slideNames = []
  global _slideModules
  _slideModules = {}  
  
  slides = ""
  if MLABFileManager.exists(contentPath + "/slides"):
    content = MLABFileManager.readStringFromFile(contentPath + "/slides")
    matches = slashRe.findall(content)
    count = 0
    for slide in matches:
      if slide == "": continue
      result = parseSlide(slide,count+1)
      if not result == "":
        count += 1
        slides += result
    
  # generate content: menu
  menu = ""
  if MLABFileManager.exists(contentPath + "/menu"):
    content = MLABFileManager.readStringFromFile(contentPath + "/menu")
    menuRe = re.compile('(.*?)[\/]+',re.DOTALL)
    matches = menuRe.findall(content)
    for menuItem in matches:
      if menuItem == "": continue
      menu += parseMenuItem(menuItem)
    
  # generate def file
  defFile = "MacroModule " + moduleName + " { externalDefinition = $(LOCAL)/" + moduleName + ".script }" 
  MLABFileManager.writeStringToFile(modulePath + "/" + moduleName + ".def",defFile)
  
  # copy py file
  MLABFileManager.copy(localPath + "template_py",modulePath + "/" + moduleName + ".py")
  
  # load styles
  styleFile = MLABFileManager.readStringFromFile(stylePath + "/style")
  styleRe = re.compile('DefineStyle\s*(.*?)\s*\{(.*?)[\/]{3,}',re.DOTALL)  
  styles = styleRe.findall(styleFile)
  
  # process slideBullets
  for i in range(len(styles)):
    styleName = styles[i][0].strip()
    styleDef = styles[i][1].strip()    
    for k,v in _slideBullets.iteritems():
      for i in range(len(v)):
        _slideBullets[k][i] = _slideBullets[k][i].replace("style = " + styleName,"style { " + styleDef)
        _slideBullets[k][i] = _slideBullets[k][i].replace("\n"," ")

  sources = ""
  # write slideBullets file
  if len(_slideBullets):
    bulletFile = "_bullets = {\n"
    num = 0
    for k,v in _slideBullets.iteritems():
      if num > 0:
        bulletFile += ","
      bulletFile += "'" + `k` + "':['" + "','".join(v) + "']\n"
      num = num + 1
    bulletFile += "}"
    sources += "source = $(LOCAL)/scripts/bullets.py\n"
    MLABFileManager.writeStringToFile(modulePath + "/scripts/bullets.py",bulletFile)
  
  # process script file -> replace content tags
  scriptFile = MLABFileManager.readStringFromFile(stylePath + "/template")
  scriptFile = scriptFile.replace("template.py",moduleName + ".py")  
  files = MLABFileManager.recursiveContentOfDirectory(contentPath + "/scripts")
  for file in files:
    if not file == "":
      sources += "source = $(LOCAL)/scripts/" + file.strip() + "\n"
      
  # print sources
  if not sources == "":
    scriptFile = scriptFile.replace("Commands {","Commands {\n" + sources)       
  scriptFile = scriptFile.replace("[HEADER]","\"" + ctx.field("header").stringValue() + "\"")
  scriptFile = scriptFile.replace("[FOOTER]","\"" + ctx.field("footer").stringValue() + "\"")
  scriptFile = scriptFile.replace("[MENU]",menu)
  scriptFile = scriptFile.replace("[SLIDES]",slides)
  
  # process script file -> replace style tags
  for i in range(len(styles)):
    styleName = styles[i][0].strip()
    style = styles[i][1].strip()    
    scriptFile = scriptFile.replace("style = " + styleName,"style { " + style)
  
  # write script file  
  MLABFileManager.writeStringToFile(modulePath + "/" + moduleName + ".script",scriptFile)
  
  # copy style data from stylePath
  styleDataPath = stylePath + "/data/"
  if MLABFileManager.exists(styleDataPath):
    
    files = MLABFileManager.recursiveContentOfDirectory(styleDataPath)
    for file in files:
      if MLABFileManager.isDir(styleDataPath + file):
        MLABFileManager.mkdir(modulePath + "/style/" + file)
      else:
        MLABFileManager.copy(styleDataPath + file,modulePath + "/style/" + file)
  else:
    print(styleDataPath)
    
  # write .mlab file for module
  # write .def and .script files for used modules
  mlabFile = "//MDL v1 utf8\n network {\n watchlist = \"\"\n}\n"
  for k,v in _slideModules.iteritems():
    macro = k.strip()
    mlabFile += "module " + macro + " {\n fields {\n  instanceName = " + macro + "\n }\n internalFields = \"\"\n}\n"
    
    defFile = "MacroModule " + macro + " { externalDefinition = $(LOCAL)/" + macro + ".script }"
    scriptFile = "Interface { Inputs {} Outputs {} Parameters {} } Commands { }"

    MLABFileManager.writeStringToFile(modulePath + "/macros/" + macro + ".def",defFile)
    MLABFileManager.writeStringToFile(modulePath + "/macros/" + macro + ".script",scriptFile)  
    
  mlabFile += "connections = \"\"\n"
  
  MLABFileManager.writeStringToFile(modulePath + "/" + moduleName + ".mlab",mlabFile)
      
  # copy data, macros, networks, scripts, etc from contentPath
  files = MLABFileManager.recursiveContentOfDirectory(contentPath)
  extRE = re.compile('(\.dcm|\.tiff|\.tif|\.avi)')
  for file in files:
    if file == "slides": continue
    if file == "menu": continue
    if file == "config": continue
    if MLABFileManager.isDir(contentPath + "/" + file):
      if not MLABFileManager.exists(modulePath + "/" + file):
        MLABFileManager.mkdir(modulePath + "/" + file)
    else:
      if not MLABFileManager.exists(modulePath + "/" + file):
        MLABFileManager.copy(contentPath + "/" + file,modulePath + "/" + file)
      else:
        if not len(extRE.findall(file)):
          MLABFileManager.copy(contentPath + "/" + file,modulePath + "/" + file)
        #else:
        #  print("Not copying existing file " + file)
    
#------------------------------------------------------------------------------------
def parseSlide(slide,num):  
  # sanity check
  contentRE = re.compile('(CONTENT)\s*=\s*(.*?)(?:(TITLE|NAME)|$)',re.DOTALL)
  matches = contentRE.findall(slide)  
  if len(matches) == 0:
    return ""
  
  string = ""
  gstring = "TabViewItem  { \
  expandX = YES \
  expandY = YES "
  
  # process NAME tag
  nameRe = re.compile('(NAME)\s*=\s*(.*?)(?:(CONTENT|TITLE)|$)',re.DOTALL)
  matches = nameRe.findall(slide)
  global _slideNames
  if len(matches):
    _slideNames.append(matches[0][1].strip())
  else:
    _slideNames.append("")
        
  # process TITLE tag
  titleRE = re.compile('(TITLE)\s*=\s*(.*?)(?:(CONTENT|NAME)|$)',re.DOTALL)
  matches = titleRE.findall(slide)    
  if len(matches) and _styleTags.has_key('SLIDETITLE'):
    string += _styleTags['SLIDETITLE'].replace("[TEXT]","\"" + matches[0][1].strip() + "\"")
    
  # process CONTENT tag
  global _slideBullets
  global _slideModules
  matches = contentRE.findall(slide)  
  if len(matches):
    currentSlide = matches[0][1]
    prepends = []
    
    # NETWORK
    networkRE = re.compile('NETWORK\s+=\s+(.*?)\n')
    networkMatches = networkRE.findall(currentSlide)
    if len(networkMatches):
      prepends.append("tabSelectedCommand = centerNetworkView")
      for i in range(len(networkMatches)):
        networkMatch = networkMatches[i]
        networkMatchRE = re.compile('NETWORK\s+=\s+' + networkMatch + '\n')
        currentSlide = networkMatchRE.sub('NetworkView { name = slide' + `num` + '_' + `i` + ' module = ' + networkMatch + ' }',currentSlide)
        _slideModules[networkMatch] = networkMatch
        
    # NETWORKHIDDEN
    networkRE = re.compile('NETWORKHIDDEN\s+=\s+(.*?)\n')
    networkMatches = networkRE.findall(currentSlide)
    if len(networkMatches):
      for i in range(len(networkMatches)):
        networkMatch = networkMatches[i]
        networkMatchRE = re.compile('NETWORKHIDDEN\s+=\s+' + networkMatch + '\n')
        currentSlide = networkMatchRE.sub('',currentSlide)
        _slideModules[networkMatch] = networkMatch
        
    # PANEL
    panelRE = re.compile('PANEL\s+=\s+(.*?)\n')
    panelMatches = panelRE.findall(currentSlide)
    if len(panelMatches):
      for i in range(len(panelMatches)):
        panelMatch = panelMatches[i]
        panelMatchRE = re.compile('PANEL\s+=\s+' + panelMatch + '\n')
        currentSlide = panelMatchRE.sub('Panel { module = ' + panelMatch + ' } ',currentSlide)
      
    # VIEWER
    viewerRE = re.compile('VIEWER\s+=\s+(.*?)\n')
    viewerMatches = viewerRE.findall(currentSlide)
    if len(viewerMatches):
      for i in range(len(viewerMatches)):
        viewerMatch = viewerMatches[i]
        viewerMatchRE = re.compile('VIEWER\s+=\s+' + viewerMatch + '\n')
        currentSlide = viewerMatchRE.sub('Viewer ' + viewerMatch + ' { type = SoExaminerViewer viewing = YES } ',currentSlide)
        
    # RENDERAREA
    viewerRE = re.compile('RENDERAREA\s+=\s+(.*?)\n')
    viewerMatches = viewerRE.findall(currentSlide)
    if len(viewerMatches):
      for i in range(len(viewerMatches)):
        viewerMatch = viewerMatches[i]
        viewerMatchRE = re.compile('RENDERAREA\s+=\s+' + viewerMatch + '\n')
        currentSlide = viewerMatchRE.sub('Viewer ' + viewerMatch + ' { type = SoRenderArea viewing = NO } ',currentSlide)

    # MoviePlayer
    mPRE = re.compile('MoviePlayer\s+\{',re.DOTALL)
    if len(mPRE.findall(currentSlide)):
      prepends.append("tabSelectedCommand = setupMoviePlayer")
      currentSlide = mPRE.sub('MoviePlayer { name = movie' + `num` + ' ',currentSlide)      
      
    # SEPARATOR
    separatorRE = re.compile('SEPARATOR\s+=\s+(.*?)\n')
    separatorMatches = separatorRE.findall(currentSlide)
    for separatorMatch in separatorMatches:
      separtorMatchRE = re.compile('SEPARATOR\s+=\s+' + separatorMatch + '\n')
      if _styleTags.has_key('SEPARATOR'):
        separtorString = _styleTags['SEPARATOR'].replace("[TEXT]","\"" + separatorMatch + "\"");
        currentSlide = separtorMatchRE.sub(separtorString,currentSlide)
      else:
        currentSlide = separtorMatchRE.sub('Label { title = "Unknown tag SEPARATOR (' + separatorMatch + ')"}',currentSlide,1)
      
    # BULLET
    numbullets = 0
    specialRE = re.compile('(\?|\.|\+|\(|\)|\[|\]|\{|\})')
    bulletRE = re.compile('BULLET\s+=(.*?)\n')
    bulletMatches = bulletRE.findall(currentSlide)
    if len(bulletMatches):
      for bulletMatch in bulletMatches:
        bulletMatchRegExp = specialRE.sub(r'\\\1',bulletMatch)
        bulletMatchRE = re.compile('BULLET\s+=' + bulletMatchRegExp + '\n')
        if _styleTags.has_key('BULLET'):
          currentSlide = bulletMatchRE.sub('DynamicFrame { name = bullet_' + `num` + '_' + `numbullets` + ' alignX = Left } ',currentSlide)
          bulletString = _styleTags['BULLET'].replace("[TEXT]","\\\"" + bulletMatch + "\\\"")
          if _slideBullets.has_key(num):
            _slideBullets[num].append(bulletString)
          else:
            _slideBullets[num] = []
            _slideBullets[num].append(bulletString)
          numbullets = numbullets + 1     
        else:
          currentSlide = bulletMatchRE.sub('Label { title = "Unknown tag BULLET (' + bulletMatch + ')"}',currentSlide,1) 
            
    # BULLETSUB
    bulletRE = re.compile('BULLETSUB\s+=(.*?)\n')
    bulletMatches = bulletRE.findall(currentSlide)
    if len(bulletMatches):
      for bulletMatch in bulletMatches:
        bulletMatchRegExp = specialRE.sub(r'\\\1',bulletMatch)
        bulletMatchRE = re.compile('BULLETSUB\s+=' + bulletMatchRegExp + '\n')
        if _styleTags.has_key('BULLETSUB'):
          currentSlide = bulletMatchRE.sub('DynamicFrame { name = bullet_' + `num` + '_' + `numbullets` + ' alignX = Left } ',currentSlide,1)
          bulletString = _styleTags['BULLETSUB'].replace("[TEXT]","\\\"" + bulletMatch + "\\\"")
          if _slideBullets.has_key(num):
            _slideBullets[num].append(bulletString)
          else:
            _slideBullets[num] = []
            _slideBullets[num].append(bulletString)
          numbullets = numbullets + 1
        else:
          currentSlide = bulletMatchRE.sub('Label { title = "Unknown tag BULLETSUB (' + bulletMatch + ')"}',currentSlide,1)
          
    # BULLETEMPTY
    bulletRE = re.compile('BULLETEMPTY\n')
    bulletMatches = bulletRE.findall(currentSlide)
    if len(bulletMatches):
      for i in range(len(bulletMatches)):
        if _styleTags.has_key('BULLETEMPTY'):
          currentSlide = bulletRE.sub('DynamicFrame { name = bullet_' + `num` + '_' + `numbullets` + ' alignX = Left } ',currentSlide,1)
          bulletString = _styleTags['BULLETEMPTY'].replace("[TEXT]","\\\"\\\"")
          if _slideBullets.has_key(num):
            _slideBullets[num].append(bulletString)
          else:
            _slideBullets[num] = []
            _slideBullets[num].append(bulletString)
          numbullets = numbullets + 1
        else:
          currentSlide = bulletRE.sub('Label { title = "Unknown tag BULLETEMPTY"}',currentSlide,1)
          
    # BULLETSUBEMPTY
    bulletRE = re.compile('BULLETSUBEMPTY\n')
    bulletMatches = bulletRE.findall(currentSlide)
    if len(bulletMatches):
      for i in range(len(bulletMatches)):
        if _styleTags.has_key('BULLETSUBEMPTY'):
          currentSlide = bulletRE.sub('DynamicFrame { name = bullet_' + `num` + '_' + `numbullets` + ' alignX = Left } ',currentSlide,1)
          bulletString = _styleTags['BULLETSUBEMPTY'].replace("[TEXT]","\\\"\\\"")
          if _slideBullets.has_key(num):
            _slideBullets[num].append(bulletString)
          else:
            _slideBullets[num] = []
            _slideBullets[num].append(bulletString)
          numbullets = numbullets + 1
        else:
          currentSlide = bulletRE.sub('Label { title = "Unknown tag BULLETSUBEMPTY"}',currentSlide,1)
        
  # SEPARATORSUB
    bulletRE = re.compile('SEPARATORSUB\s+=(.*?)\n')
    bulletMatches = bulletRE.findall(currentSlide)
    if len(bulletMatches):
      for bulletMatch in bulletMatches:
        bulletMatchRegExp = specialRE.sub(r'\\\1',bulletMatch)
        bulletMatchRE = re.compile('SEPARATORSUB\s+=' + bulletMatchRegExp + '\n')
        if _styleTags.has_key('SEPARATORSUB'):
          currentSlide = bulletMatchRE.sub('DynamicFrame { name = bullet_' + `num` + '_' + `numbullets` + ' alignX = Right } ',currentSlide,1)
          bulletString = _styleTags['SEPARATORSUB'].replace("[TEXT]","\\\"" + bulletMatch + "\\\"")
          if _slideBullets.has_key(num):
            _slideBullets[num].append(bulletString)
          else:
            _slideBullets[num] = []
            _slideBullets[num].append(bulletString)
          numbullets = numbullets + 1
        else:
          currentSlide = bulletMatchRE.sub('Label { title = "Unknown tag SEPARATORSUB (' + bulletMatch + ')"}',currentSlide,1)
          
    # BULLETSMALL
    bulletRE = re.compile('BULLETSMALL\s+=(.*?)\n')
    bulletMatches = bulletRE.findall(currentSlide)
    if len(bulletMatches):
      for bulletMatch in bulletMatches:
        bulletMatchRegExp = specialRE.sub(r'\\\1',bulletMatch)
        bulletMatchRE = re.compile('BULLETSMALL\s+=' + bulletMatchRegExp + '\n')
        if _styleTags.has_key('BULLETSMALL'):
          currentSlide = bulletMatchRE.sub('DynamicFrame { name = bullet_' + `num` + '_' + `numbullets` + ' alignX = Right } ',currentSlide,1)
          bulletString = _styleTags['BULLETSMALL'].replace("[TEXT]","\\\"" + bulletMatch + "\\\"")
          if _slideBullets.has_key(num):
            _slideBullets[num].append(bulletString)
          else:
            _slideBullets[num] = []
            _slideBullets[num].append(bulletString)
          numbullets = numbullets + 1
        else:
          currentSlide = bulletMatchRE.sub('Label { title = "Unknown tag BULLETSMALL (' + bulletMatch + ')"}',currentSlide,1)
          
    # PUSHUP
    pushupRE = re.compile('PUSHUP')
    currentSlide = pushupRE.sub('Empty { expandY = YES }',currentSlide)
      
    string += "\n".join(prepends) + "\n"
    string += currentSlide

  if not string == "":
    gstring += string + "}\n\n"
  else:
    gstring = ""
  
  return gstring 
  
#------------------------------------------------------------------------------------
def parseMenuItem(menuItem):
  string = ""
  
  # process TITLE tag
  titleRe = re.compile('(TITLE)\s*=\s*(.*?)(?:(SLIDE|EMPTY)|$)',re.DOTALL)
  titleMatches = titleRe.findall(menuItem)
  if len(titleMatches):
    # process SLIDE tag
    num = -1
    slideRe = re.compile('(SLIDE)\s*=\s*(.*?)(?:(TITLE|EMPTY)|$)',re.DOTALL)
    slideMatches = slideRe.findall(menuItem)
    if len(slideMatches):
      for i in range(len(_slideNames)):
        if _slideNames[i] == slideMatches[0][1].strip():
          num = i
          break;

    itemName = ""
    if not num == -1:
      itemName = "name = menuItem" + `num`
      
    if _styleTags.has_key('MENUITEM'):
      menuItem = _styleTags['MENUITEM']
      menuItem = menuItem.replace("[TEXT]","\"" + titleMatches[0][1].strip() + "\"")
      menuItem = menuItem.replace("[NAME]",itemName)
      string += menuItem
  
  # process EMPTY tag
  emptyRe = re.compile('(EMPTY)\s*=\s*(.*?)(?:(TITLE|SLIDE)|$)',re.DOTALL)
  matches = emptyRe.findall(menuItem)
  if len(matches):
  
    if _styleTags.has_key('MENUEMPTY'):
      string += _styleTags['MENUEMPTY'].replace('[SIZE]',matches[0][1].strip())
    
  # return  
  return string 
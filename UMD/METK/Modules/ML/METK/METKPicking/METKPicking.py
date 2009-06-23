
_listView = None
_win = None
def init():
   return
   
   
def initPopupWindow():
   print "METKPicking: initPopupWindow"
   global _win
   #_win = ctx.window("MultiSelection")
   #_win.setHeight(300)
   #print _win
   #_win = ctx
   #print _win   
   return

def initListView():
   global _listView
   _listView = ctx.control("objListView")
   updateListView()
   return


def closeListView():
   global _listView
   _listView = None
   return


def updateListView():
   if _listView:
      _listView.clearItems()
      
      values = ctx.field("objList").value.split(",")
            
      for singleValue in values:
         singleValue=singleValue.replace("Structure_","")
         item = _listView.insertItem()
         item.setPixmapFile(0, ctx.expandFilename("$(LOCAL)/img1.png"))
         item.setText(1, singleValue)
   
   return


def clicked(item, column):
   #
   return
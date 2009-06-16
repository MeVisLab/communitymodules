from kButton import *

button1 = None
button2 = None
eventFilter1 = None
analyzer1 = None


def initWindow(fieldP = 0):
   global button1,button2
   button1 = kButton(ctx,ctx.control("button1"),"$(LOCAL)/Button1-normal.png","$(LOCAL)/Button1-down.png","$(LOCAL)/Button1-over.png","$(LOCAL)/Button1-disabled.png")
   button2 = kButton(ctx,ctx.control("button2"),"$(LOCAL)/Button2-normal.png","$(LOCAL)/Button2-down.png","$(LOCAL)/Button2-over.png","$(LOCAL)/Button2-disabled.png")
   button2.setType(BT_BOOL)
   
   global eventFilter1
   eventFilter1 = ctx.control("eventFilter1")
   eventFilter1.addControl(ctx.control("button1"), True)
   eventFilter1.addControl(ctx.control("button2"), True)
   
   global analyzer1
   analyzer1 = kButtonAnalyzer(ctx,eventFilter1)
   
   return
   
def button1Clicked(args=0):
   print "First Button clicked"
   return

def button2Clicked(args=0):
   print "Second Button clicked"
   return   
      
def disableButton1(args=0):
   button1.setEnabled(not ctx.field("disableButton1").value)
   return
   
def disableButton2(args=0):
   button2.setEnabled(not ctx.field("disableButton2").value)
   return
   
   
def analyzeButtonEventWindow1(event):
   analyzer1.analyzeButtonEvent(event)
   return
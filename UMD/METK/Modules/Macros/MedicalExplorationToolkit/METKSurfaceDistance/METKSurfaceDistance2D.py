# Python module import
from mevis import *


def init(field = 0):
   ctx.field("display").touch()
   return


def display(field = 0):
   ctx.field("SoView2DContour.triggerRemoveAll").touch()
   ctx.field("SoView2DContour.interactiveDensity").value = False
   
   if ctx.field("display").value == True:
      ctx.field("SoView2DContour.triggerAddContour").touch()
      ctx.field("SoView2DContour.drawLines").value = True
      ctx.field("SoView2DContour.density").value = ctx.field("innerMarginValue").value
      ctx.field("SoView2DContour.contour_color").value = ctx.field("innerMarginColor").value
      ctx.field("SoView2DContour.interactiveDensity").value = False

      ctx.field("SoView2DContour.triggerAddContour").touch()
      ctx.field("SoView2DContour.drawLines").value = True
      ctx.field("SoView2DContour.density").value = ctx.field("outerMarginValue").value
      ctx.field("SoView2DContour.contour_color").value = ctx.field("outerMarginColor").value
      ctx.field("SoView2DContour.interactiveDensity").value = False
   return
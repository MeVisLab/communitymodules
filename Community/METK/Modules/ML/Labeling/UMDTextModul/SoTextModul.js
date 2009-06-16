
function getFontfile( filename ){
     ctx.field("font_1").value = ctx.unexpandFilename(filename);
	
}

function fileDialog()
{
  exp = ctx.expandFilename(ctx.field("font_1").stringValue());
  var file = MLABFileDialog.getOpenFileName(exp,"","Open file");
  if ( file )
  {
    ctx.field("font_1").value = ctx.unexpandFilename(file);
  }
}

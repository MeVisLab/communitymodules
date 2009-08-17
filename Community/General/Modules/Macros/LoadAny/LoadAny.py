# **InsertLicense** code
#----------------------------------------------------------------------------------
#! Python functionality for the LoadAny module
#! \file    LoadAny.py
#! \author  Wolf Spindler
#! \date    08/09
#----------------------------------------------------------------------------------

from mevis import MLAB
import os

# Loads first 4Kb from file, replaces null chars by "." and
# sends it into the fileHeader text field. Also a short header 
# version with at most 48 chars is written into shortFileHeader 
# where null characters are removed.
def updateFileHeaderPy():
  if os.path.exists(ctx.field("trueName").value):
    fileIn = file(ctx.field("trueName").value, 'rb')
    header1K = fileIn.read(4096)

    # Replace  null characters with "."
    result = header1K.replace('\0', ".")
    ctx.field("fileHeader").value = result

    # Remove null chars and take first 48 chars.
    subHeader = header1K.strip('\0')[0:48]
    subHeader = subHeader.replace('\0', ".")

    # Append "..." if header is shorter than 48.
    if len(subHeader) > 48:
      subHeader = subHeader + "..."
    ctx.field("shortFileHeader").value = subHeader
    fileIn.close()


// **InsertLicense** code
//----------------------------------------------------------------------------------
//! Java Script functionality for the LoadAny module
/*!
// \file    LoadAny.py
// \author  Wolf Spindler
// \date    08/09
*/
//----------------------------------------------------------------------------------

var loaderModName;
var fileLoader   ;

SuffixList = [
  // File ending | Loader Module                        | Name of Outfield                       | File namefield  | Format/Loader Description                                               | Automatic? |                                          | Field to touch/set|   Check    | CheckVal
  [".tiff"        ,  "ImageLoad"                         , "output0"                              , "filename"      , "MeVisLab ImageLoad for the Tagged Image File Format"                     ,  "Yes",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
  [".tif"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "MeVisLab ImageLoad for the Tagged Image File Format"                     ,  "Yes",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
  [".dcm"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "MeVisLab ImageLoad"                                                      ,  "Yes",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
  [".raw"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Try to load with ImageLoad module manually"                              ,   "No",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
  [".pgm"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "MeVisLab ImageLoad for Portable Grey Map"                                ,  "Yes",  "ImageSave vtkPNMWriter",                               ""    ,      ""    ,     ""    ],
  [".pnm"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "MeVisLab ImageLoad"                                                      ,  "Yes",  "ImageSave vtkPNMWriter",                               ""    ,      ""    ,     ""    ],
  [".ppm"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "MeVisLab ImageLoad"                                                      ,  "Yes",  "ImageSave vtkPNMWriter",                               ""    ,      ""    ,     ""    ],
  [".png"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "MeVisLab ImageLoad"                                                      ,  "Yes",  "ImageSave vtkPNGWriter",                               ""    ,      ""    ,     ""    ],
  [".bmp"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "MeVisLab ImageLoad"                                                      ,  "Yes",  "ImageSave vtkBMPWriter",                               ""    ,      ""    ,     ""    ],
  [".dicom"       ,  "ImageLoad"                         , "output0"                              , "filename"      , "MeVisLab ImageLoad"                                                      ,  "Yes",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
  [".jpe"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "MeVisLab ImageLoad"                                                      ,  "Yes",  "ImageSave vtkJPEGWriter",                              ""    ,      ""    ,     ""    ],
  [".jpg"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "MeVisLab ImageLoad"                                                      ,  "Yes",  "ImageSave vtkJPEGWriter",                              ""    ,      ""    ,     ""    ],
  [".jpeg"        ,  "ImageLoad"                         , "output0"                              , "filename"      , "MeVisLab ImageLoad"                                                      ,  "Yes",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
  [".analyze"     ,  "ImageLoad"                         , "output0"                              , "filename"      , "MeVisLab ImageLoad"                                                      ,  "Yes",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
  [".quoc"        ,  "ImageLoad"                         , "output0"                              , "filename"      , "MeVisLab ImageLoad"                                                      ,  "Yes",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
                                                                                                                                                                                                                                                                                         
  // Additional file formats supported by ImageLoad on Mac.                                                                                                                                                                                                                              
  [".3fr"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for .3fr files"                                  ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".arw"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for .arw Sony RAW Image files"                   ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".ai"          ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for Adobe Illustrator Vector Graphics"           ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".cr2"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for Canon Digital Camera Raw images"             ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".crw"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for Canon Digital Camera Raw images"             ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".dcr"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for Kodak Digital Camera Raw images"             ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".dng"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for Digital Negative Format images"              ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".exr"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for OpenEXR Bitmaps"                             ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".fff"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for .fff  files"                                 ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".fpx"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for .fpx FlashPix Bitmaps"                       ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".gif"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for Graphic Interchange Format images"           ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".hdr"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for .hdr files"                                  ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".icns"        ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for .hdr files"                                  ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".ico"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for Windows .ico icon files"                     ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".j2k"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for JPEG-2000 images"                            ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".jp2"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for JPEG-2000 images"                            ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".jpe"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for .jpe JPEG images"                            ,  "Yes",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
  [".jpf"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for .jpf files"                                  ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".jpx"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for JPEG-2000 images"                            ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".mos"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for .mos files"                                  ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".mrw"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for Sony (Minolta) Raw Image File"               ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".nef"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for .nef files"                                  ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".orf"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for Olympus Digital Camera Raw Images"           ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".pct"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for .pct files"                                  ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".pdf"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for Adobe Acrobat .pdf files"                    ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".pef"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for Pentax RAW Bitmap Graphics"                  ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".pic"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for .pic files"                                  ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".pict"        ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for Macintosh Quickdraw/PICT Drawings"           ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".pntg"        ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for MacPaint Bitmap Graphics"                    ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".psd"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for Photoshop files"                             ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".qif"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for Quicktime images"                            ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".qti"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for Quicktime images"                            ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".qtif"        ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for Quicktime images"                            ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".raf"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for Fuji CCD-RAW Graphic files"                  ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".sgi"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for Silicon Graphics Images"                     ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".srf"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for .srf files"                                  ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".tga"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for Truevision Targa Graphics"                   ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".xbm"         ,  "ImageLoad"                         , "output0"                              , "filename"      , "Mac ImageLoad extension for X Bitmaps"                                   ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  ["*"            ,  "ImageLoad"                         , "output0"                              , "filename"      , "Try MeVisLab ImageLoad for files with no or unknown extension"           ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
                                                                                                                                                                                                                                                                                         
  [".base"        ,  "LoadBase"                          , "outObject"                            , "filename"      , "MeVisLab loader for Base Objects"                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".xml"         ,  "LoadBase"                          , "outObject"                            , "filename"      , "MeVisLab loader for Base Objects"                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  ["*"            ,  "LoadBase"                          , "outObject"                            , "filename"      , "Try MeVisLab Base loader for files with no or unknown extension"         ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
                                                                                                                                                                                                                                                                                         
  [".gvr"         ,  "GVRVolumeLoad"                     , "outVolume"                            , "filename"      , "MeVisLab Giga Voxel Renderer octtree files"                              ,  "Yes",  "GVRVolumeSave",                                        ""    ,      ""    ,     ""    ],
  [".mlimage"     ,  "MLImageFormatLoad"                 , "output0"                              , "fileName"      , "MeVisLab ML Image Format"                                                ,  "Yes",  "MLImageFormatSave",                                    ""    ,      ""    ,     ""    ],
  [".mif"         ,  "MLImageFormatLoad"                 , "output0"                              , "fileName"      , "MeVisLab ML Image Format"                                                ,  "Yes",  "MLImageFormatSave",                                    ""    ,      ""    ,     ""    ],
                                                                                                                                                                                                                                                                                         
  [".cso"         ,  "CSOLoad"                           , "outCSOList"                           , "fileName"      , "MeVisLab Contour Segmentation Object (CSO) File Format"                  ,  "Yes",  "CSOSave",                                              ""    , "loadSuccess",   "TRUE"],
  [".pwn"         ,  "PointCloudLoad"                    , "outPointCloud"                        , "fileName"      , "MeVisLab Point Cloud (Points With normals) file format"                  ,  "Yes",  "PointCloudSave",                                       "load",      ""    ,     ""    ],
                                                                                                                                                                                                  
  [".txt"         ,  "LoadTreeData"                      , "outputGraph"                          , "filename"      , "MeVisLab Tree Data Loader"                                               ,  "Yes",  "SaveTreeData",                                         "load", "isValidOutputGraph", "TRUE"],
  [".mltrdata"    ,  "LoadTreeData"                      , "outputGraph"                          , "filename"      , "MeVisLab Tree Data Loader"                                               ,  "Yes",  "SaveTreeData",                                         "load", "isValidOutputGraph", "TRUE"],

  [".lfa"         ,  "LoadLinFunc"                       , "outputArray"                          , "fileName"      , "LinFunc Array"                                                           ,  "Yes",  "SaveLinFunc",                                          "load",      ""    ,     ""    ],
  [".lf"          ,  "LoadLinFunc"                       , "outputArray"                          , "fileName"      , "LinFunc"                                                                 ,  "Yes",  "SaveLinFunc",                           "singleFunction=TRUE",      ""    ,     ""    ],

  // itkImageFileReader                                                                                                                                                                                                                                                                  
  [".analyze"     ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Analyze 7.5"                                                             ,  "Yes",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
  [".hdr"         ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Analyze 7.5"                                                             ,  "Yes",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
  [".img"         ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Analyze 7.5"                                                             ,  "Yes",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
  [".bmp"         ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "BMP ? using itk BMPImageIO"                                               ,  "Yes",  "ImageSave vtkBMPWriter",                               ""    ,      ""    ,     ""    ],
  [".dcm"         ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "DICOM"                                                                   ,  "Yes",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
  [".gdcm"        ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "DICOM"                                                                   ,  "Yes",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
  [".dicom"       ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "DICOM"                                                                   ,  "Yes",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
  [".4x"          ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "GE 4x"                                                                   ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".5x"          ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "GE 5x"                                                                   ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".ge"          ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "GE Advantage Windows"                                                    ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".ge4x"        ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "GE Advantage Windows"                                                    ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".ge5x"        ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "GE Advantage Windows"                                                    ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".gipl"        ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Gipl (.gipl)"                                                            ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".ipl"         ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "IPLCommon"                                                               ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".jpg"         ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "JPEG ? using itk JPEGImageIO"                                             ,  "Yes",  "ImageSave vtkJPEGWriter",                              ""    ,      ""    ,     ""    ],
  [".jpeg"        ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "JPEG ? using itk JPEGImageIO"                                             ,  "Yes",  "ImageSave vtkJPEGWriter",                              ""    ,      ""    ,     ""    ],
  [".mha"         ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "MetaImage (.mha/.mhd)"                                                   ,  "Yes",  "vtkMetaImageWriter",                                   ""    ,      ""    ,     ""    ],
  [".mhd"         ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "MetaImage (.mha/.mhd)"                                                   ,  "Yes",  "vtkMetaImageWriter",                                   ""    ,      ""    ,     ""    ],
  [".png"         ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "PNG ? "                                                                   ,  "Yes",  "ImageSave vtkPNGWriter",                               ""    ,      ""    ,     ""    ],
  [".raw"         ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Raw ? "                                                                   ,  "Yes",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
  [".vision"      ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Siemens Vision"                                                          ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".siemens"     ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Siemens Vision"                                                          ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".spr"         ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Stimulate (spr/sdt)"                                                     ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".sdt"         ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Stimulate (spr/sdt)"                                                     ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".stimulate"   ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Stimulate (spr/sdt)"                                                     ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".tif"         ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Tagged Image File Format"                                                ,  "Yes",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
  [".tiff"        ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Tagged Image File Format"                                                ,  "Yes",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
  [".bio"         ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Bio-Rad confocal microscope (MRC)"                                       ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".biorad"      ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Bio-Rad confocal microscope (MRC)"                                       ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".brains"      ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Brains2Mask file format"                                                 ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".brains2"     ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Brains2Mask file format"                                                 ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".brains2mask" ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Brains2Mask file format"                                                 ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".bruker"      ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Bruker2DSEQ"                                                             ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".bruker2d"    ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Bruker2DSEQ"                                                             ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".bruker2dseq" ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Bruker2DSEQ"                                                             ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],

  // Hmm, MINC obviously cannot be read although I believed so.
  //[".mnc"         ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "MINC2 file format"                                                       ,  "Yes",  "vtkMINCImageWriter",                                   ""    ,      ""    ,     ""    ],
  //[".mnc2"        ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "MINC2 file format"                                                       ,  "Yes",  "vtkMINCImageWriter",                                   ""    ,      ""    ,     ""    ],
  [".minc"        ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "MINC2 file format"                                                       ,  "Yes",  "vtkMINCImageWriter",                                   ""    ,      ""    ,     ""    ],
  [".minc2"       ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "MINC2 file format"                                                       ,  "Yes",  "vtkMINCImageWriter",                                   ""    ,      ""    ,     ""    ],
  [".nii"         ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Neuroimaging Informatics Technology Initiative files"                    ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".nifti"       ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Neuroimaging Informatics Technology Initiative files"                    ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".nhdr"        ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "'Nearly Raw Raster Data' images"                                         ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".nrrd"        ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "'Nearly Raw Raster Data' images"                                         ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".philips"     ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "PhilipsREC images"                                                       ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".philipsreq"  ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "PhilipsREC images"                                                       ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".rec"         ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "PhilipsREC images"                                                       ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".par"         ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "PhilipsREC images"                                                       ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".recpar"      ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "PhilipsREC images"                                                       ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".vox"         ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "VoxBoCUB images"                                                         ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".voxbo"       ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "VoxBoCUB images"                                                         ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".voxbocub"    ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "VoxBoCUB images"                                                         ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  ["*"            ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "Try itkImageFileReader for files with no or unknown extension"           ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
                                                                                                                                                                                                                                                                                          
  // vtk image, mesh or polygon formats                                                                                                                                                                                                                                                   
  [".3ds"         ,  "vtk3DSImporter"                    ,  "output3DSImporter"                   , "fileName"      , "VTK 3D Studio file reader"                                               ,   "No",  "",                                                     ""    ,      ""    ,     ""    ],
  [".av"          ,  "vtkAVSucdReader"                   ,  "outputAVSucdReader"                  , "fileName"      , "VTK AVS 'UCD' format file reader"                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".avsucd"      ,  "vtkAVSucdReader"                   ,  "outputAVSucdReader"                  , "fileName"      , "VTK AVS 'UCD' format file reader"                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".sucd"        ,  "vtkAVSucdReader"                   ,  "outputAVSucdReader"                  , "fileName"      , "VTK AVS 'UCD' format file reader"                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".inp"         ,  "vtkAVSucdReader"                   ,  "outputAVSucdReader"                  , "fileName"      , "VTK AVS 'UCD' format file reader"                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".bmp"         ,  "vtkBMPReader"                      ,  "outputBMPReader"                     , "fileName"      , "Bitmap"                                                                  ,  "Yes",  "ImageSave vtkBMPWriter",                               ""    ,      ""    ,     ""    ],
  [".byu"         ,  "vtkBYUReader"                      ,  "outputBYUReader"                     , "fileName"      , "VTK Movie BYU file reader"                                               ,  "Yes",  "vtkBYUWriter",                                         ""    ,      ""    ,     ""    ],
  [".g"           ,  "vtkBYUReader"                      ,  "outputBYUReader"                     , "fileName"      , "VTK Movie BYU file reader"                                               ,  "Yes",  "vtkBYUWriter",                                         ""    ,      ""    ,     ""    ],
  [".s"           ,  "vtkBYUReader"                      ,  "outputBYUReader"                     , "fileName"      , "VTK Movie BYU file reader"                                               ,  "Yes",  "vtkBYUWriter",                                         ""    ,      ""    ,     ""    ],
  [".d"           ,  "vtkBYUReader"                      ,  "outputBYUReader"                     , "fileName"      , "VTK Movie BYU file reader"                                               ,  "Yes",  "vtkBYUWriter",                                         ""    ,      ""    ,     ""    ],
  [".t"           ,  "vtkBYUReader"                      ,  "outputBYUReader"                     , "fileName"      , "VTK Movie BYU file reader"                                               ,  "Yes",  "vtkBYUWriter",                                         ""    ,      ""    ,     ""    ],
  [".graph"       ,  "vtkChacoGraphReader"               ,  "outputChacoGraphReader"              , "fileName"      , "Reads Chaco files into a vtkGraph"                                       ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".graph"       ,  "vtkChacoReader"                    ,  "outputChacoReader"                   , "baseName"      , "Reads Chaco files into a vtkUnstructuredGrid"                            ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".coords"      ,  "vtkChacoGraphReader"               ,  "outputChacoGraphReader"              , "fileName"      , "Reads binary cosmology file into a vtkUnstructuredGrid"                  ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".coords"      ,  "vtkChacoReader"                    ,  "outputChacoReader"                   , "fileName"      , "Reads Chaco files into a vtkUnstructuredGrid"                            ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [""             ,  "vtkCosmoReader"                    ,  "outputCosmoReader"                   , "fileName"      , "Reads a binary cosmology file into a vtkUnstructuredGrid"                ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  //Instable? ["*"            ,  "vtkDelimitedTextReader"            ,  "outputDelimitedTextReader"           , "fileName"      , "Pulls in data from a flat, delimited text file"              ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".dem"         ,  "vtkDEMReader"                      ,  "outputDEMReader"                     , "fileName"      , "Digital Elevation Model (DEM)"                                           ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".dcm"         ,  "vtkDICOMImageReader"               ,  "outputDICOMImageReader"              , "fileName"      , "DICOM"                                                                   ,  "Yes",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
  [".dicom"       ,  "vtkDICOMImageReader"               ,  "outputDICOMImageReader"              , "fileName"      , "DICOM"                                                                   ,  "Yes",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
  [".case"        ,  "vtkEnSightGoldReader"              ,  "outputEnSightGoldReader"             , "caseFileName"  , "VTK Ensight ? Gold file reader"                                          ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".geo"         ,  "vtkEnSightGoldReader"              ,  "outputEnSightGoldReader"             , "caseFileName"  , "VTK Ensight ? Gold file reader"                                          ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".case"        ,  "vtkEnSightGoldBinaryReader"        ,  "outputEnSightGoldBinaryReader"       , "caseFileName"  , "VTK Ensight ? Gold file reader"                                          ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".geo"         ,  "vtkEnSightGoldBinaryReader"        ,  "outputEnSightGoldBinaryReader"       , "caseFileName"  , "VTK Ensight ? Gold file reader"                                          ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".case"        ,  "vtkEnSightReader"                  ,  "outputEnSightReader"                 , "caseFileName"  , "VTK Ensight ? file reader"                                               ,  "Yes",  "vtkEnSightWriter",                                     ""    ,      ""    ,     ""    ],
  [".geo"         ,  "vtkEnSightReader"                  ,  "outputEnSightReader"                 , "caseFileName"  , "VTK Ensight ? file reader"                                               ,  "Yes",  "vtkEnSightWriter",                                     ""    ,      ""    ,     ""    ],
  [".case"        ,  "vtkEnSight6Reader"                 ,  "outputEnSight6Reader"                , "caseFileName"  , "VTK Ensight6 ? file reader"                                              ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".geo"         ,  "vtkEnSight6Reader"                 ,  "outputEnSight6Reader"                , "caseFileName"  , "VTK Ensight6 ? file reader"                                              ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".case"        ,  "vtkEnSight6BinaryReader"           ,  "outputEnSight6BinaryReader"          , "caseFileName"  , "VTK Ensight6 binary ? file reader"                                       ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".geo"         ,  "vtkEnSight6BinaryReader"           ,  "outputEnSight6BinaryReader"          , "caseFileName"  , "VTK Ensight6 binary ? file reader"                                       ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".ex2"         ,  "vtkExodusIIReader"                 ,  "outputExodusIIReader"                , "fileName"      , "VTK ExodusII unstructured grid reader"                                   ,  "Yes",  "vtkExodusIIWriter",                                    ""    ,      ""    ,     ""    ],
  [".exii"        ,  "vtkExodusIIReader"                 ,  "outputExodusIIReader"                , "fileName"      , "VTK ExodusII unstructured grid reader"                                   ,  "Yes",  "vtkExodusIIWriter",                                    ""    ,      ""    ,     ""    ],
  [".ex2"         ,  "vtkExodusReader"                   ,  "outputExodusReader"                  , "fileName"      , "VTK ExodusII unstructured grid reader"                                   ,  "Yes",  "vtkExodusIIWriter",                                    ""    ,      ""    ,     ""    ],
  [".exii"        ,  "vtkExodusReader"                   ,  "outputExodusReader"                  , "fileName"      , "VTK ExodusII unstructured grid reader"                                   ,  "Yes",  "vtkExodusIIWriter",                                    ""    ,      ""    ,     ""    ],
  [".facet"       ,  "vtkFacetReader"                    ,  "outputFacetReader"                   , "fileName"      , "VTK Facet file reader"                                                   ,  "Yes",  "vtkFacetWriter",                                       ""    ,      ""    ,     ""    ],
  [""             ,  "vtkFixedWidthTextReader"           ,  "outputFixedWidthTextReader"          , "fileName"      , ""                                                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [""             ,  "vtkFLUENTReader"                   ,  "outputFLUENTReader"                  , "fileName"      , ""                                                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  ["*"            ,  "vtkGenericDataObjectReader"        ,  "outputGenericDataObjectReader"       , "fileName"      , ""                                                                        ,  "Yes",  "vtkGenericDataObjectWriter",                           ""    ,      ""    ,     ""    ],
  [".mr"          ,  "vtkGESignaReader"                  ,  "outputGESignaReader"                 , "fileName"      , "GE Signal"                                                               ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".ct"          ,  "vtkGESignaReader"                  ,  "outputGESignaReader"                 , "fileName"      , "GE Signal"                                                               ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".gesigna"     ,  "vtkGESignaReader"                  ,  "outputGESignaReader"                 , "fileName"      , "GE Signal"                                                               ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".ximg"        ,  "vtkGESignaReader"                  ,  "outputGESignaReader"                 , "fileName"      , "GE Signal"                                                               ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".graph"       ,  "vtkGraphReader"                    ,  "outputGraphReader"                   , "fileName"      , "ASCII and binary reader for vtkGraphs"                                   ,  "Yes",  "vtkGraphWriter",                                       ""    ,      ""    ,     ""    ],
  [".cube"        ,  "vtkGaussianCubeReader"             ,  "outputGaussianCubeReader"            , "fileName"      , "VTK Gaussian file reader"                                                ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".case"        ,  "vtkGenericEnSightReader"           ,  "outputGenericEnSightReader"          , "caseFileName"  , "VTK Ensight ? file reader"                                               ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".geo"         ,  "vtkGenericEnSightReader"           ,  "outputGenericEnSightReader"          , "caseFileName"  , "VTK Ensight ? file reader"                                               ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".neu"         ,  "vtkGAMBITReader"                   ,  "outputGAMBITReader"                  , "fileName"      , "VTK Fluent GAMBIT ASCII file reader"                                     ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".raw"         ,  "vtkImageReader"                    ,  "outputImageReader"                   , "fileName"      , "RAW files ?"                                                             ,  "Yes",  "ImageSave vtkImageWriter",                             ""    ,      ""    ,     ""    ],
  [".img"         ,  "vtkImageReader"                    ,  "outputImageReader"                   , "fileName"      , "RAW files ?"                                                             ,  "Yes",  "vtkImageWriter",                                       ""    ,      ""    ,     ""    ],
  [".imag"        ,  "vtkImageReader"                    ,  "outputImageReader"                   , "fileName"      , "RAW files ?"                                                             ,  "Yes",  "vtkImageWriter",                                       ""    ,      ""    ,     ""    ],
  [".image"       ,  "vtkImageReader"                    ,  "outputImageReader"                   , "fileName"      , "RAW files ?"                                                             ,  "Yes",  "vtkImageWriter",                                       ""    ,      ""    ,     ""    ],
  [".isi"         ,  "vtkISIReader"                      ,  "outputISIReader"                     , "fileName"      , "VTK bibliographic citations reader"                                      ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".jpg"         ,  "vtkJPEGReader"                     ,  "outputJPEGReader"                    , "fileName"      , "JPEG"                                                                    ,  "Yes",  "ImageSave vtkJPEGWriter",                              ""    ,      ""    ,     ""    ],
  [".jpeg"        ,  "vtkJPEGReader"                     ,  "outputJPEGReader"                    , "fileName"      , "JPEG"                                                                    ,  "Yes",  "ImageSave vtkJPEGWriter",                              ""    ,      ""    ,     ""    ],
  [".lsdyna"      ,  "vtkLSDynaReader"                   ,  "outputLSDynaReader"                  , "fileName"      , "VTK reader for LS-Dyna databases (d3plot)"                               ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".3dplot"      ,  "vtkLSDynaReader"                   ,  "outputLSDynaReader"                  , "fileName"      , "VTK reader for LS-Dyna databases (d3plot)"                               ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".tri"         ,  "vtkMCubesReader"                   ,  "outputMCubesReader"                  , "fileName"      , "VTK Marching Cubes file reader"                                          ,  "Yes",  "vtkMCubesWriter",                                      ""    ,      ""    ,     ""    ],
  [".lim"         ,  "vtkMCubesReader"                   ,  "outputMCubesReader"                  , "fileName"      , "VTK Marching Cubes file reader"                                          ,  "Yes",  "vtkMCubesWriter",                                      ""    ,      ""    ,     ""    ],
  [".mnc"         ,  "vtkMINCImageReader"                ,  "outputMINCImageReader"               , "fileName"      , "MINC (1.1) image data reader"                                            ,  "Yes",  "vtkMINCImageWriter",                                   ""    ,      ""    ,     ""    ],
  [".minc"        ,  "vtkMINCImageReader"                ,  "outputMINCImageReader"               , "fileName"      , "MINC (1.1) image data reader"                                            ,  "Yes",  "vtkMINCImageWriter",                                   ""    ,      ""    ,     ""    ],
  [".mha"         ,  "vtkMetaImageReader"                ,  "outputMetaImageReader"               , "fileName"      , "Binary UNC meta image data"                                              ,  "Yes",  "vtkMetaImageWriter",                                   ""    ,      ""    ,     ""    ],
  [".mhd"         ,  "vtkMetaImageReader"                ,  "outputMetaImageReader"               , "fileName"      , "Binary UNC meta image data"                                              ,  "Yes",  "vtkMetaImageWriter",                                   ""    ,      ""    ,     ""    ],
  [".mfix"        ,  "vtkMFIXReader"                     ,  "outputMFIXReader"                    , "fileName"      , "Unstructured grid MFIX file reader"                                      ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [""             ,  "vtkMultiBlockPLOT3DReader"         ,  "outputMultiBlockPLOT3DReader"        , "fileName"      , ""                                                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [""             ,  "vtkNetCDFCOARDSReader"             ,  "outputNetCDFCOARDSReader"            , "fileName"      , ""                                                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [""             ,  "vtkNetCDFReader"                   ,  "outputNetCDFReader"                  , "fileName"      , ""                                                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".obj"         ,  "vtkOBJReader"                      ,  "outputOBJReader"                     , "fileName"      , "Wavefront OBJ file format"                                               ,  "Yes",  "WEMSave vtkOBJExporter",                               ""    ,      ""    ,     ""    ],
  [""             ,  "vtkOpenFOAMReader"                 ,  "outputOpenFOAMReader"                , "fileName"      , ""                                                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [""             ,  "vtkParticleReader"                 ,  "outputParticleReader"                , "fileName"      , ""                                                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [""             ,  "vtkPDBReader"                      ,  "outputPDBReader"                     , "fileName"      , ""                                                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [""             ,  "vtkPExodusIIReader"                ,  "outputPExodusIIReader"               , "fileName"      , ""                                                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".bin"         ,  "vtkPLOT3DReader"                   ,  "outputPLOT3DReader"                  , "fileName"      , "VTK PLOT3D file reader"                                                  ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".ply"         ,  "vtkPLYReader"                      ,  "outputPLYReader"                     , "fileName"      , "Polygon Model Format (PLY)"                                              ,  "Yes",  "WEMSave vtkPLYWriter",                                 ""    ,      ""    ,     ""    ],
  [".png"         ,  "vtkPNGReader"                      ,  "outputPNGReader"                     , "fileName"      , "PNG"                                                                     ,  "Yes",  "ImageSave vtkPNGWriter",                               ""    ,      ""    ,     ""    ],
  [".pnm"         ,  "vtkPNMReader"                      ,  "outputPNMReader"                     , "fileName"      , "PNM"                                                                     ,  "Yes",  "ImageSave vtkPNMWriter",                               ""    ,      ""    ,     ""    ],
  [".ppm"         ,  "vtkPNMReader"                      ,  "outputPNMReader"                     , "fileName"      , "PNM"                                                                     ,  "Yes",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
  [".vtk"         ,  "vtkPolyDataReader"                 ,  "outputPolyDataReader"                , "fileName"      , "VTK Poly Data ? file reader"                                             ,  "Yes",  "vtkPolyDataWriter",                                    ""    ,      ""    ,     ""    ],
  ["*"            ,  "vtkPolyDataReader"                 ,  "outputPolyDataReader"                , "fileName"      , "VTK Poly Data ? file reader"                                             ,  "Yes",  "vtkPolyDataWriter",                                    ""    ,      ""    ,     ""    ],
  [""             ,  "vtkPOPReader"                      ,  "outputPOPReader"                     , "fileName"      , ""                                                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [""             ,  "vtkPSLACReader"                    ,  "outputPSLACReader"                   , "fileName"      , ""                                                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [""             ,  "vtkParticleReader"                 ,  "outputParticleReader"                , "fileName"      , "Try vtkXYZMolReader for files with no or unknown extension"              ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".particle"    ,  "vtkParticleReader"                 ,  "outputParticleReader"                , "fileName"      , "VTK Particle file reader"                                                ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".raw"         ,  "vtkParticleReader"                 ,  "outputParticleReader"                , "fileName"      , "VTK Particle file reader"                                                ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".pdb"         ,  "vtkPDBReader"                      ,  "outputPDBReader"                     , "fileName"      , "VTK PDB Molecule file reader"                                            ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [""             ,  "vtkRectilinearGridReader"          ,  "outputRectilinearGridReader"         , "fileName"      , ""                                                                        ,  "Yes",  "vtkRectilinearGridWriter",                             ""    ,      ""    ,     ""    ],
  [".ris"         ,  "vtkRISReader"                      ,  "outputRISReader"                     , "fileName"      , "VTK Renderman file reader"                                               ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [""             ,  "vtkRTXMLPolyDataReader"            ,  "outputRTXMLPolyDataReader"           , "fileName"      , ""                                                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".stl"         ,  "vtkSTLReader"                      ,  "outputSTLReader"                     , "fileName"      , "Stereo Lithography File"                                                 ,  "Yes",  "WEMSave vtkSTLWriter",                                 ""    ,      ""    ,     ""    ],
  [".stlb"        ,  "vtkSTLReader"                      ,  "outputSTLReader"                     , "fileName"      , "Stereo Lithography File"                                                 ,  "Yes",  "WEMSave vtkSTLWriter",                                 ""    ,      ""    ,     ""    ],
  [".vtk"         ,  "vtkStructuredGridReader"           ,  "outputStructuredGridReader"          , "fileName"      , "VTK Structured Grid ? file reader"                                       ,  "Yes",  "vtkStructuredGridWriter",                              ""    ,      ""    ,     ""    ],
  [""             ,  "vtkSESAMEReader"                   ,  "outputSESAMEReader"                  , "fileName"      , ""                                                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [""             ,  "vtkSimplePointsReader"             ,  "outputSimplePointsReader"            , "fileName"      , ""                                                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [""             ,  "vtkSLACParticleReader"             ,  "outputSLACParticleReader"            , "fileName"      , ""                                                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [""             ,  "vtkSLACReader"                     ,  "outputSLACReader"                    , "fileName"      , ""                                                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".slc"         ,  "vtkSLCReader"                      ,  "outputSLCReader"                     , "fileName"      , "SLC"                                                                     ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [""             ,  "vtkSQLGraphReader"                 ,  "outputSQLGraphReader"                , "fileName"      , ""                                                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  ["*"            ,  "vtkStructuredGridReader"           ,  "outputStructuredGridReader"          , "fileName"      , ""                                                                        ,  "Yes",  "vtkStructuredGridWriter",                              ""    ,      ""    ,     ""    ],
  ["*"            ,  "vtkStructuredPointsReader"         ,  "outputStructuredPointsReader"        , "fileName"      , ""                                                                        ,  "Yes",  "vtkStructuredPointsWriter",                            ""    ,      ""    ,     ""    ],
  [""             ,  "vtkTableReader"                    ,  "outputTableReader"                   , "fileName"      , ""                                                                        ,  "Yes",  "vtkTableWriter",                                       ""    ,      ""    ,     ""    ],
  [".tif"         ,  "vtkTIFFReader"                     ,  "outputTIFFReader"                    , "fileName"      , "Tagged Image File Format loaded with vtkTIFFReader"                      ,  "Yes",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
  [".tiff"        ,  "vtkTIFFReader"                     ,  "outputTIFFReader"                    , "fileName"      , "Tagged Image File Format loaded with vtkTIFFReader"                      ,  "Yes",  "ImageSave",                                            ""    ,      ""    ,     ""    ],
  [""             ,  "vtkTreeReader"                     ,  "outputTreeReader"                    , "fileName"      , "VTK loader for vtk TreeData"                                             ,  "Yes",  "vtkTreeWriter",                                        ""    ,      ""    ,     ""    ],
  [".tlp"         ,  "vtkTulipReader"                    ,  "outputTulipReader"                   , "fileName"      , "VTK loader for tulip files"                                              ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".facet"       ,  "vtkUGFacetReader"                  ,  "outputUGFacetReader"                 , "fileName"      , "VTK Unigraphics Facet Files file reader"                                 ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".vtk"         ,  "vtkUnstructuredGridReader"         ,  "outputUnstructuredGridReader"        , "fileName"      , "VTK reader for vtk unstructured grids"                                   ,  "Yes",  "vtkUnstructuredGridWriter",                            ""    ,      ""    ,     ""    ],
  [".raw"         ,  "vtkVolume16Reader"                 ,  "outputVolume16Reader"                , "fileName"      , "VTK loader for 16 bit image data file(s)"                                ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [""             ,  "vtkVolume16Reader"                 ,  "outputVolume16Reader"                , "fileName"      , "VTK loader for 16 bit image data file(s)"                                ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".vrml"        ,  "vtkVRMLImporter"                   ,  "outputVRMLImporter"                  , "fileName"      , "VTK importer for the Virtual Reality Modeling Language"                  ,   "No",  "WEMSave vtkVRMLExporter SoSceneWriter VRMLWriter",     ""    ,      ""    ,     ""    ],
  [".wrl"         ,  "vtkVRMLImporter"                   ,  "outputVRMLImporter"                  , "fileName"      , "VTK importer for the Virtual Reality Modeling Language"                  ,   "No",  "WEMSave vtkVRMLExporter SoSceneWriter VRMLWriter",     ""    ,      ""    ,     ""    ],
  [".gml"         ,  "vtkXGMLReader"                     ,  "outputXGMLReader"                    , "fileName"      , "VTK Reader for XML graph files based losely on the GML notation"         ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".xgml"        ,  "vtkXGMLReader"                     ,  "outputXGMLReader"                    , "fileName"      , "VTK Reader for XML graph files based losely on the GML notation"         ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".xml"         ,  "vtkXMLHierarchicalBoxDataReader"   ,  "outputXMLHierarchicalBoxDataReader"  , "fileName"      , "VTK XML reader for vtk hierarchical box data"                            ,  "Yes",  "vtkXMLHierarchicalBoxDataWriter",                      ""    ,      ""    ,     ""    ],
  [".xml"         ,  "vtkXMLHierarchicalDataReader"      ,  "outputXMLHierarchicalDataReader"     , "fileName"      , "VTK XML reader for vtk hierarchical data"                                ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".xml"         ,  "vtkXMLHyperOctreeReader"           ,  "outputXMLHyperOctreeReader"          , "fileName"      , "VTK XML reader for vtk hyper octtrees"                                   ,  "Yes",  "vtkXMLHyperOctreeWriter",                              ""    ,      ""    ,     ""    ],
  [".xml"         ,  "vtkXMLImageDataReader"             ,  "outputXMLImageDataReader"            , "fileName"      , "VTK XML reader for vtk image data"                                       ,  "Yes",  "vtkXMLImageDataWriter",                                ""    ,      ""    ,     ""    ],
  [".vti"         ,  "vtkXMLImageDataReader"             ,  "outputXMLImageDataReader"            , "fileName"      , "VTK XML reader for vtk image data"                                       ,  "Yes",  "vtkXMLImageDataWriter",                                ""    ,      ""    ,     ""    ],
  [".xml"         ,  "vtkXMLMaterialReader"              ,  "outputXMLMaterialReader"             , "fileName"      , "VTK XML reader for vtk materials"                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".xml"         ,  "vtkXMLMultiBlockDataReader"        ,  "outputXMLMultiBlockDataReader"       , "fileName"      , "VTK XML reader for vtk multiblock data"                                  ,  "Yes",  "vtkXMLMultiBlockDataWriter",                           ""    ,      ""    ,     ""    ],
  [".xml"         ,  "vtkXMLMultiGroupDataReader"        ,  "outputXMLMultiGroupDataReader"       , "fileName"      , "VTK XML reader for vtk multigroup data"                                  ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".xml"         ,  "vtkXMLPImageDataReader"            ,  "outputXMLPImageDataReader"           , "fileName"      , ""                                                                        ,  "Yes",  "vtkXMLPImageDataWriter",                               ""    ,      ""    ,     ""    ],
  [".xml"         ,  "vtkXMLPolyDataReader"              ,  "outputXMLPolyDataReader"             , "fileName"      , ""                                                                        ,  "Yes",  "vtkXMLPolyDataWriter",                                 ""    ,      ""    ,     ""    ],
  [".vtp"         ,  "vtkXMLPolyDataReader"              ,  "outputXMLPolyDataReader"             , "fileName"      , ""                                                                        ,  "Yes",  "vtkXMLPolyDataWriter",                                 ""    ,      ""    ,     ""    ],
  [".xml"         ,  "vtkXMLPPolyDataReader"             ,  "outputXMLPPolyDataReader"            , "fileName"      , ""                                                                        ,  "Yes",  "vtkXMLPPolyDataWriter",                                ""    ,      ""    ,     ""    ],
  [".xml"         ,  "vtkXMLPRectilinearGridReader"      ,  "outputXMLPRectilinearGridReader"     , "fileName"      , ""                                                                        ,  "Yes",  "vtkXMLPRectilinearGridWriter",                         ""    ,      ""    ,     ""    ],
  [".xml"         ,  "vtkXMLPStructuredGridReader"       ,  "outputXMLPStructuredGridReader"      , "fileName"      , ""                                                                        ,  "Yes",  "vtkXMLPStructuredGridWriter",                          ""    ,      ""    ,     ""    ],
  [".xml"         ,  "vtkXMLPUnstructuredGridReader"     ,  "outputXMLPUnstructuredGridReader"    , "fileName"      , ""                                                                        ,  "Yes",  "vtkXMLPUnstructuredGridWriter",                        ""    ,      ""    ,     ""    ],
  [".xml"         ,  "vtkXMLRectilinearGridReader"       ,  "outputXMLRectilinearGridReader"      , "fileName"      , "VTK XML reader for vtk rectilinear grids"                                ,  "Yes",  "vtkXMLRectilinearGridWriter",                          ""    ,      ""    ,     ""    ],
  [".vtr"         ,  "vtkXMLRectilinearGridReader"       ,  "outputXMLRectilinearGridReader"      , "fileName"      , "VTK XML reader for vtk rectilinear grids"                                ,  "Yes",  "vtkXMLRectilinearGridWriter",                          ""    ,      ""    ,     ""    ],
  [".xml"         ,  "vtkXMLStructuredGridReader"        ,  "outputXMLStructuredGridReader"       , "fileName"      , "VTK XML reader for vtk structured grids"                                 ,  "Yes",  "vtkXMLStructuredGridWriter",                           ""    ,      ""    ,     ""    ],
  [".vts"         ,  "vtkXMLStructuredGridReader"        ,  "outputXMLStructuredGridReader"       , "fileName"      , "VTK XML reader for vtk structured grids"                                 ,  "Yes",  "vtkXMLStructuredGridWriter",                           ""    ,      ""    ,     ""    ],
  [".xml"         ,  "vtkXMLTreeReader"                  ,  "outputXMLTreeReader"                 , "fileName"      , "VTK XML reader for vtk tree data"                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".xml"         ,  "vtkXMLUnstructuredGridReader"      ,  "outputXMLUnstructuredGridReader"     , "fileName"      , "VTK XML reader for unstructured grids"                                   ,  "Yes",  "vtkXMLUnstructuredGridWriter",                         ""    ,      ""    ,     ""    ],
  [".vtu"         ,  "vtkXMLUnstructuredGridReader"      ,  "outputXMLUnstructuredGridReader"     , "fileName"      , "VTK XML reader for unstructured grids"                                   ,  "Yes",  "vtkXMLUnstructuredGridWriter",                         ""    ,      ""    ,     ""    ],
  [".bin"         ,  "vtkXYZMolReader"                   ,  "outputXYZMolReader"                  , "fileName"      , "VTK XYZ file reader"                                                     ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".xyz"         ,  "vtkXYZMolReader"                   ,  "outputXYZMolReader"                  , "fileName"      , "VTK XYZ file reader"                                                     ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],

  // Try some  generic readers last, because they often cannot handle some date less good than specialized readers.                                                       
  [".vtk"         ,  "vtkDataSetReader"                  ,  "outputDataSetReader"                 , "fileName"      , "Reads ASCII or binary field data files into vtk format"                  ,  "Yes",  "vtkDataSetWriter",                                     ""    ,      ""    ,     ""    ],
  ["*"            ,  "vtkDataSetReader"                  ,  "outputDataSetReader"                 , "fileName"      , "Reads ASCII or binary field data files into vtk format"                  ,  "Yes",  "vtkDataSetWriter",                                     ""    ,      ""    ,     ""    ],
  [".vtk"         ,  "vtkDataObjectReader"               ,  "outputDataObjectReader"              , "fileName"      , "Reads ASCII or binary field data files into vtk format"                  ,  "Yes",  "vtkDataObjectWriter",                                  ""    ,      ""    ,     ""    ],
  ["*"            ,  "vtkDataObjectReader"               ,  "outputDataObjectReader"              , "fileName"      , "Reads ASCII or binary field data files into vtk format"                  ,  "Yes",  "vtkDataObjectWriter",                                  ""    ,      ""    ,     ""    ],
  [".vtk"         ,  "vtkDataReader"                     ,  "outputDataReader"                    , "fileName"      , "Reads attributes from a vtk data file"                                   ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  ["*"            ,  "vtkDataReader"                     ,  "outputDataReader"                    , "fileName"      , "Reads attributes from a vtk data file"                                   ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],

  // Try more specific readers last, because they sometimes accept non appropriate files partially (e.g. the only read points from a grid).                                                       
  ["*"            ,  "vtkGraphReader"                    ,  "outputGraphReader"                   , "fileName"      , "ASCII and binary reader for vtkGraphs"                                   ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  ["*"            ,  "vtkAVSucdReader"                   ,  "outputAVSucdReader"                  , "fileName"      , "Try vtkAVSucdReader for files with no or unknown extension"              ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  ["*"            ,  "vtkEnSightMasterServerReader"      ,  "outputEnSightMasterServerReader"     , "caseFileName"  , ""                                                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  ["*"            ,  "vtkGenericEnSightReader"           ,  "outputGenericEnSightReader"          , "caseFileName"  , ""                                                                        ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  ["*"            ,  "vtkXYZMolReader"                   ,  "outputXYZMolReader"                  , "fileName"      , "Try vtkXYZMolReader for files with no or unknown extension"              ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  ["*"            ,  "vtkImageReader"                    ,  "outputImageReader"                   , "fileName"      , "Try vtkImageReader for files with no or unknown extension"               ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  [".vtr"         ,  "vtkRectilinearGridReader"          ,  "outputRectilinearGridReader"         , "fileName"      , ""                                                                        ,  "Yes",  "vtkRectilinearGridWriter",                             ""    ,      ""    ,     ""    ],

  // Always try real vtk loaders first.
  [".vtk"         ,  "itkImageFileReader"                , "output0"                              , "unresolvedFileName"      , "VTK Structured Points"                                                   ,  "Yes",  "",                                                     ""    ,      ""    ,     ""    ],
  
                                                                                                                                                                                                                                                                                         
  [".wem"         ,  "WEMLoad"                           , "outWEM"                               , "filename"      , "MeVisLab Winged Edge File Format (WEM)"                                  ,  "Yes",  "WEMSave",                                              ""    , "loadingOK",     "TRUE"],
  [".off"         ,  "WEMLoad"                           , "outWEM"                               , "filename"      , "3D Mesh Object File Format"                                              ,  "Yes",  "WEMSave",                                              ""    , "loadingOK",     "TRUE"],
  [".geom"        ,  "WEMLoad"                           , "outWEM"                               , "filename"      , "MeVisLab Winged Edge File Format (WEM)"                                  ,  "Yes",  "WEMSave",                                              ""    , "loadingOK",     "TRUE"],
//[".iv"          ,  "WEMLoad"                           , "outWEM"                               , "filename"      , "MeVisLab Winged Edge File Format (WEM)"                                  ,  "Yes",  "WEMSave vtkIVWriter vtkIVExporter SoSceneWriter",      ""    , "loadingOK",     "TRUE"],
  [".obj"         ,  "WEMLoad"                           , "outWEM"                               , "filename"      , "Wavefront OBJ file format"                                               ,  "Yes",  "WEMSave vtkOBJExporter",                               ""    , "loadingOK",     "TRUE"],
  [".ply"         ,  "WEMLoad"                           , "outWEM"                               , "filename"      , "Polygon Model Format (PLY)"                                              ,  "Yes",  "WEMSave vtkPLYReader",                                 ""    , "loadingOK",     "TRUE"],
  [".stl"         ,  "WEMLoad"                           , "outWEM"                               , "filename"      , "Stereo Lithography File"                                                 ,  "Yes",  "WEMSave vtkSTLWriter",                                 ""    , "loadingOK",     "TRUE"],
  [".stlb"        ,  "WEMLoad"                           , "outWEM"                               , "filename"      , "Binary Stereo Lithography File"                                          ,  "Yes",  "WEMSave vtkSTLWriter",                                 ""    , "loadingOK",     "TRUE"],
  ["*"            ,  "WEMLoad"                           , "outWEM"                               , "filename"      , "MeVisLab Winged Edge File Format (WEM)"                                  ,  "Yes",  "WEMSave vtkSTLWriter",                                 ""    , "loadingOK",     "TRUE"],
  
  // The following loaders are not available everywhere.                                                                                                                                                                                                                                 
  // BioFormatsLoad                                                                                                                                                                                                                                                                      
  [".psd"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Adobe Photoshop PSD"                                                     ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".al3d"        ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Alicona 3D"                                                              ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".gel"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Amersham Biosciences GEL"                                                ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tif"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Andor Bio-Imaging Division (ABD) variant of the Tagged Image File Format",  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tiff"        ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Andor Bio-Imaging Division (ABD) variant of the Tagged Image File Format",  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".png"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Animated PNG"                                                            ,  "Yes",  "ImageSave vtkPNGWriter",                               "open",      ""    ,     ""    ],
  [".svs"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Aperio SVS variant of the Tagged Image File Format"                      ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".avi"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "AVI (Audio Video Interleave)"                                            ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".sdt"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Becker & Hickl SPCImage"                                                 ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".pic"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Bio-Rad PIC"                                                             ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".ims"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Bitplane Imaris"                                                         ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".bmp"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "BMP (Windows Bitmap)"                                                    ,  "Yes",  "ImageSave vtkBMPWriter",                               "open",      ""    ,     ""    ],
  [".dv"          ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "DeltaVision"                                                             ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".r3d"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "DeltaVision"                                                             ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".dcm"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "DICOM"                                                                   ,  "Yes",  "ImageSave",                                            "open",      ""    ,     ""    ],
  [".dicom"       ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "DICOM"                                                                   ,  "Yes",  "ImageSave",                                            "open",      ""    ,     ""    ],
  [".eps"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "EPS (Encapsulated PostScript)"                                           ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".flex"        ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Evotec Flex"                                                             ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".img"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "FEI"                                                                     ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".fits"        ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "FITS (Flexible Image Transport System)"                                  ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".dm3"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Gatan Digital Micrograph"                                                ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".gif"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "GIF (Graphics Interchange Format)"                                       ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".naf"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Hamamatsu Aquacosmos NAF"                                                ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".ics"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "ICS (Image Cytometry Standard)"                                          ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".seq"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Image-Pro Sequence"                                                      ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".ipw"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Image-Pro Workspace"                                                     ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".liff"        ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Improvision Openlab LIFF"                                                ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tif"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Improvision variant of the Tagged Image File Format"                     ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".raw"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Improvision Openlab Raw"                                                 ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tif"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "InCell 1000 variant of the Tagged Image File Format"                     ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tiff"        ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "InCell 1000 variant of the Tagged Image File Format"                     ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".xdce"        ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "InCell 1000"                                                             ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".ipl"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "IPLab"                                                                   ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".jpg"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "JPEG"                                                                    ,  "Yes",  "ImageSave vtkJPEGWriter",                              "open",      ""    ,     ""    ],
  [".jp2"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "JPEG 2000"                                                               ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".xv"          ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Khoros VIFF (Visualization Image File Format) Bitmap"                    ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".lif"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Leica LAS AF LIF (Leica Image File Format)"                              ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tif"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Leica LCS LEI variant of the Tagged Image File Format"                   ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tiff"        ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Leica LCS LEI variant of the Tagged Image File Format"                   ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".lei"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Leica LCS LEI"                                                           ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".l2d"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Li-Cor L2D"                                                              ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".scn"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Li-Cor L2D"                                                              ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tif"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Li-Cor L2D variant of the Tagged Image File Format"                      ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tiff"        ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Li-Cor L2D variant of the Tagged Image File Format"                      ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".lim"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "LIM (Laboratory Imaging/Nikon)"                                          ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tif"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "MetaMorph 7.5 variant of the Tagged Image File Format"                   ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tiff"        ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "MetaMorph 7.5 variant of the Tagged Image File Format"                   ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".stk"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "MetaMorph Stack (STK)"                                                   ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".mnc"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "MINC MRI"                                                                ,  "Yes",  "vtkMINCImageWriter",                                   "open",      ""    ,     ""    ],
  [".minc"        ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "MINC MRI"                                                                ,  "Yes",  "vtkMINCImageWriter",                                   "open",      ""    ,     ""    ],
  [".txt"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "?Manager"                                                             ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tif"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "?Manager variant of the Tagged Image File Format"                       ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tiff"        ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "?Manager variant of the Tagged Image File Format"                       ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".mng"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "MNG (Multiple-image Network Graphics)"                                   ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".mrc"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "MRC (Medical Research Council)"                                          ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".nef"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "NEF (Nikon Electronic Format)"                                           ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tif"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "NEF (Nikon Electronic Format) variant of the Tagged Image File Format"   ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tiff"        ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "NEF (Nikon Electronic Format) variant of the Tagged Image File Format"   ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".nd2"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Nikon NIS-Elements ND2"                                                  ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".nrrd"        ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "nrrd (Nearly Raw Raster Data)"                                           ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".sld"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Olympus 3i SlideBook"                                                    ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".apl"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Olympus CellR/APL"                                                       ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".mtb"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Olympus CellR/APL"                                                       ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tnb"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Olympus CellR/APL"                                                       ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tif"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Olympus CellR/APL variant of the Tagged Image File Format"               ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tiff"        ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Olympus CellR/APL variant of the Tagged Image File Format"               ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tif"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Olympus FluoView TIFF"                                                   ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tiff"        ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Olympus FluoView TIFF"                                                   ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".oib"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Olympus FluoView FV1000"                                                 ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".oif"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Olympus FluoView FV1000"                                                 ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".ome.tif"     ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "OME variant of the Tagged Image File Format"                             ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".ome"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "OME-XML"                                                                 ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".2"           ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "PerkinElmer UltraView"                                                   ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".3"           ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "PerkinElmer UltraView"                                                   ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".4"           ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "PerkinElmer UltraView"                                                   ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".5"           ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "PerkinElmer UltraView"                                                   ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".6"           ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "PerkinElmer UltraView"                                                   ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tif"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "PerkinElmer UltraView variant of the Tagged Image File Format"           ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tiff"        ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "PerkinElmer UltraView variant of the Tagged Image File Format"           ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".pgm"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "PGM (Portable Gray Map)"                                                 ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".pict"        ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "PICT (Macintosh Picture)"                                                ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".png"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "PNG (Portable Network Graphics)"                                         ,  "Yes",  "ImageSave vtkPNGWriter",                               "open",      ""    ,     ""    ],
  [".tif"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Prairie Technologies variant of the Tagged Image File Format"            ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".xml"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Prairie Technologies variant of the Tagged Image File Format"            ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".mov"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "QuickTime Movie"                                                         ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".cxd"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "SimplePCI"                                                               ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".tif"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "TIFF (Tagged Image File Format)"                                         ,  "Yes",  "ImageSave",                                            "open",      ""    ,     ""    ],
  [".xys"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "VisiTech XYS"                                                            ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".zvi"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Zeiss AxioVision ZVI (Zeiss Vision Image)"                               ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  [".lsm"         ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Zeiss LSM (Laser Scanning Microscope) 510"                               ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
  ["*"            ,  "BioFormatsLoad"                    , "output0"                              , "imageFilePath" , "Try BioFormatsLoad for files with no or unknown extension"               ,  "Yes",  "",                                                     "open",      ""    ,     ""    ],
                                                                                                                                                                                                                                                                                          
  // Files not automatically loaded by LoadAny.                                                                                                                                                                                                                                           
//[".txt"         ,  "LoadCAT08Data"                     , ""                                     , ""              , "MICCAI Coronary Artery Tracking 2008 competition reference points"       ,   "No",  "",                                                     ""    ,      ""    ,     ""    ],
  [".mlhelp"      ,  "ModuleDocEditor"                   , ""                                     , ""              , "MeVisLab ModuleDocEditor documentation file"                             ,   "No",  "ModuleDocEditor",                                      ""    ,      ""    ,     ""    ],
  [".pro"         ,  ""                                  , ""                                     , ""              , "Qt project file used by MeVisLab developer projects"                     ,   "No",  "",                                                     ""    ,      ""    ,     ""    ],
  [".pri"         ,  ""                                  , ""                                     , ""              , "Qt and MeVisLab package include for .pro files"                          ,   "No",  "",                                                     ""    ,      ""    ,     ""    ],
  [".mlinstall"   ,  ""                                  , ""                                     , ""              , "MeVisLab installer description. Use ToolRunner for it."                  ,   "No",  "",                                                     ""    ,      ""    ,     ""    ],
  [".mldoc"       ,  ""                                  , ""                                     , ""              , "MeVisLab documentation description. Use ToolRunner for it."              ,   "No",  "",                                                     ""    ,      ""    ,     ""    ],
  [".script"      ,  ""                                  , ""                                     , ""              , "MeVisLab definition file for modules and applications"                   ,   "No",  "",                                                     ""    ,      ""    ,     ""    ],
  [".mlab"        ,  ""                                  , ""                                     , ""              , "MeVisLab network file, use MeVisLab to open it"                          ,   "No",  "",                                                     ""    ,      ""    ,     ""    ],
  [".py"          ,  ""                                  , ""                                     , ""              , "(MeVisLab) Python script file."                                          ,   "No",  "",                                                     ""    ,      ""    ,     ""    ],
  [".js"          ,  ""                                  , ""                                     , ""              , "(MeVisLab) Java Script files."                                           ,   "No",  "",                                                     ""    ,      ""    ,     ""    ],
  [".mat"         ,  ""                                  , ""                                     , ""              , "MatLab file. Try MatLab itself or MatLabScriptWrapper."                  ,   "No",  "MatLab",                                               ""    ,      ""    ,     ""    ],
  [".raw"         ,  ""                                  , ""                                     , ""              , "Image-J Raw"                                                             ,   "No",  "",                                                     ""    ,      ""    ,     ""    ],
                                                                                                                                                                                                  
                                                                                                                                                                                                                                                                                         
  // Test Open Inventor last, because in case of bad files it scroll amounts of errors...                                                                                                         
  [".oiv"         ,  "SoFile"                            , "self"                                 , "name"          , "Open Inventor File Format"                                               ,  "Yes",  "WEMSave vtkIVWriter vtkIVExporter SoSceneWriter",      ""    ,      ""    ,     ""    ],
  [".iv"          ,  "SoFile"                            , "self"                                 , "name"          , "Open Inventor File Format"                                               ,  "Yes",  "WEMSave vtkIVWriter vtkIVExporter SoSceneWriter",      ""    ,      ""    ,     ""    ],

  [".srspd"       ,  "LoadRawSiemensSpectData"           , "output0"                              , "fileName"      , "Siemens Spectroscopy Data file"                                          ,  "Yes",  "",                                                   "update",      ""    ,     ""    ]
]                                                                                                                                                                                                                                                                                                  


//----------------------------------------------------------------
//! Resets global variables.
//----------------------------------------------------------------
function init()
{
  clear();
}

//----------------------------------------------------------------
//! Called when the module is created. Clear a previously created
//! loader, status, enum and information fields etc. and touch the 
//! abbreviated name field to start the update process.
//----------------------------------------------------------------
function wakeUpInit()
{
  ctx.field("trueName"  ).setStringValue("");
  //ctx.field("prefLoader").setStringValue("");
  ctx.field("status"    ).setStringValue("");
  //setUpEnumFields();
  if (ctx.field("name").value!=""){
    ctx.field("name").touch();
  }
}

//----------------------------------------------------------------
//! Removes a potentially existing loader and resets all
//! information fields.
//----------------------------------------------------------------
function clear()
{
  if (fileLoader){
    ctx.network().removeModule (fileLoader);
    //logConsole("Loader module deleted.", false, true);
  }
  loaderModName = "";
  fileLoader    = 0;
  ctx.field("textOut")        .setStringValue("");
  ctx.field("loaderName")     .setStringValue("");
  ctx.field("writerNames")    .setStringValue("");
  ctx.field("formatInfo")     .setStringValue("");
  ctx.field("shortFileHeader").setStringValue("");
  ctx.field("fileHeader")     .setStringValue("");
}

//----------------------------------------------------------------
//! Updates the table with the available table of file 
//! extensions loaders etc.
//----------------------------------------------------------------
function setUpEnumFields()
{
  const numSuffixes = SuffixList.length;
  var strList = "Extension@Loader@Writer@Description\n";
  for (fn=0; fn < numSuffixes; fn++){
    const suffixStr = SuffixList[fn][0]; 
    constAdded = ((suffixStr == "") ? "<No Extension>" : suffixStr) + "@" +
                  SuffixList[fn][1] + "@" +
                  SuffixList[fn][6] + "@" +
                  SuffixList[fn][4];
    strList += constAdded + "\n";
  }
  ctx.field("prefLoader").setValue(strList);
}

//----------------------------------------------------------------
//! Called if a file is dropped onto the module - it simply 
//! updates the abbreviated file name then.
//----------------------------------------------------------------
function fileDropped(filename)
{
  ctx.field("name").value = ctx.unexpandFilename(filename);
}

//----------------------------------------------------------------
//! Called if the abbreviated file name is changed. 
//----------------------------------------------------------------
function nameChanged(checkTypical, checkAll)
{
  clear();
  var exp = ctx.expandFilename(ctx.field("name").stringValue());
  if (MLABFileManager.exists(exp)) {
    if (MLABFileManager.isDir(exp)) {
      logConsole("LocalImage.name - Selection is a directory (" + exp + ")", true, false);
      clear();
    }
    else {
      ctx.field("trueName").setStringValue(exp);
      const loadRet = searchAndSetUpLoader(ctx.field("trueName").value, checkTypical, checkAll);
      const ok = (1==loadRet) ||     // Non automatic loader found.
                 (2==loadRet);       // Full load success.
    }
  } 
  else {
    if (exp && (exp!="")){
      clear();
      logConsole("LoadAny.js - File does not exist ("+exp+")", true, false);
    }
  }
}

//----------------------------------------------------------------
//! Opens a file dialog to select a file to open.
//----------------------------------------------------------------
function fileDialog()
{
  var exp = ctx.expandFilename(ctx.field("name").stringValue());
  var file = MLABFileDialog.getOpenFileName(exp,"","Open file");
  if (file){
    ctx.field("name").value = ctx.unexpandFilename(file);
  }
}
  
//----------------------------------------------------------------
//! Return the case corrected name if a module with name name 
//! (with potentially different upper/lower case usage) exists,
//! otherwise "" is returned.
//----------------------------------------------------------------
function moduleNameExists(modName)
{ 
  var allModules = MLAB.allModules();
  for (var m=0; m < allModules.length; m++){
    if (modName.toLowerCase() == allModules[m].toLowerCase()){ 
      return allModules[m];
    }
  }
  // Not found. Return "";
  return "";
}

//----------------------------------------------------------------
//! Appends strVal to the current status field or even posts
//! it to the MeVisLab console if isWarning is enabled.
//! If onlyOnVerbose is enabled then the message is shown only 
//! if the verbose field is enabled.
//! The console output is limited to the size of 4k to reduce
//! performance loss due to too large field contents.
//----------------------------------------------------------------
function logConsole(strVal, isWarning, onlyOnVerbose)
{
  if (isWarning){
    MLAB.logWarning(strVal);
  }
  if (!onlyOnVerbose || (ctx.field("verbose").value && onlyOnVerbose)){
    var newConsoleVal = ctx.field("status").value;
    newConsoleVal += ((newConsoleVal != "") ? "\n" : "") + strVal;
    if (newConsoleVal.length > 8192){
      // String too long, which becomes slow. Reduce to at most 4095 chars.
      newConsoleVal = newConsoleVal.substring(newConsoleVal.length-8192);
    }
    ctx.field("status").value = newConsoleVal;
  }
}

//----------------------------------------------------------------
//! Helpr function to test whether suffix is the same value
//! as it is found at positions [start, end[ in str 
//! if lower/uppercase is ignored.
//----------------------------------------------------------------
function checkSubString(str, start, end, suffix)
{
  var subStr = str.substring(start, end).toLowerCase();
  var res = (subStr==suffix);
  return res;
}


//----------------------------------------------------------------
// Heuristical approach to find out an appropriate file extension
// which describes the loader to be used.
// The is either done by a header analysis of the file where
// signatures of file formats are used or just by guessing 
// the format from the file name.
//----------------------------------------------------------------
function examineHeader()
{
  var fExt = "";
  header1K = ctx.field("fileHeader").value;
  logConsole("File header analysis started, looking into not more than " + header1K.length + " bytes.", false, true);
  if (header1K.length > 0){
    if      (checkSubString(header1K,  0,   2, "p5"         )){ fExt = ".pgm";     }
    else if (checkSubString(header1K,  0,   2, "p6"         )){ fExt = ".pnm";     }
    else if (checkSubString(header1K,  1,   4, "png"        )){ fExt = ".png";     }
    else if (checkSubString(header1K,  0,   3, "bm6"        )){ fExt = ".bmp";     }
    else if (checkSubString(header1K,  0,   7, "mlimage"    )){ fExt = ".mlimage"; }
    else if (checkSubString(header1K,  0,   7, "cerius2"    )){ fExt = ".cube";    }

    else if ((header1K.substring(0,4096).toLowerCase().indexOf("siemens") >= 0) &&
             ((header1K.substring(0,4096).toLowerCase().indexOf("spec") >= 0) ) ||
              (header1K.substring(0,4096).toLowerCase().indexOf("triotim") >= 0) ||
              (header1K.substring(0,4096).toLowerCase().indexOf("sgradspec") >= 0) ||
              (header1K.substring(0,4096).toLowerCase().indexOf("lvectorsize")>=0) )   { fExt = ".srspd"; } // Siemens spectroscopy data
    
    else if (checkSubString(header1K,128, 132, "dicm"       )){ fExt = ".dcm";     }
    else if (checkSubString(header1K,  0,   4, "lymb"       )){ fExt = ".stl";     }
    else if (checkSubString(header1K,  0,   3, "ii*"        )){ fExt = ".tiff";    }
    else if (checkSubString(header1K,  0,   2, "mm"         )){ fExt = ".tiff";    }
    else if (checkSubString(header1K,  0,   8, "#ensight"   )){ fExt = ".case";    }
    else if (checkSubString(header1K,  0,   3, "cdf"        )){ fExt = ".mnc";     }
    else if (checkSubString(header1K,  0,   5, "# vtk"      )){ fExt = ".vtk";     }
    else if (checkSubString(header1K,  0,   2, "ga"         )){ fExt = ".facet";   }
    else if (checkSubString(header1K,  0,   5, "facet"      )){ fExt = ".facet";   }
    else if (checkSubString(header1K,  0,   8, "#vrml v1"   )){ fExt = ".iv";      }
    else if (checkSubString(header1K,  0,  11, "#inventor v")){ fExt = ".iv";      }
    else if (checkSubString(header1K,  0,   9, "# avs ucd"  )){ fExt = ".avsucd";  }
    else if (checkSubString(header1K,  6,  10, "jfif"       )){ fExt = ".jpg";     }
    else if (checkSubString(header1K, 31,  37, "gambit"     )){ fExt = ".neu";     }
    else if (checkSubString(header1K,  0,   3, "off"        )){ fExt = ".off";     }
    else if (checkSubString(header1K,  0,   4, "nrrd"       )){ fExt = ".nrrd";    }
    else if (checkSubString(header1K,  0,  17, "treedata version ")){ fExt = ".mltrdata"; }
    else if ((header1K.length > 4) && (((header1K.charCodeAt(0)==92) && (header1K.charCodeAt(1)==1 )) ||
                                       ((header1K.charCodeAt(2)== 1) && (header1K.charCodeAt(3)==92)))  ) { fExt = ".nii"; }
    else if (header1K.substring(0,100).toLowerCase().indexOf("vtkfile type=\"imagedata\""       ) >= 0)   { fExt = ".vti"; }
    else if (header1K.substring(0,100).toLowerCase().indexOf("vtkfile type=\"structuredgrid\""  ) >= 0)   { fExt = ".vts"; }
    else if (header1K.substring(0,100).toLowerCase().indexOf("vtkfile type=\"rectilineargrid\"" ) >= 0)   { fExt = ".vtr"; }
    else if (header1K.substring(0,300).toLowerCase().indexOf("ima") >= 0                              )   { fExt = ".dcm"; }   // guessing .ima files
    
    else if (checkSubString(header1K,  0,   4, "atom"       )){ fExt = ".pdb";     } // only a guess, not a real signature.

    //else if (checkSubString(header1K,  0,   4, "imgf"       )){ fExt = ".";    } // Still no idea which format this is.
    //else if (header1K.substring(0,100).indexOf("master_server") >= 0){ fExt = ".emsr"; }

    //else if (checkSubString(header1K,  0,   3, ""           )){ fExt = ".";      }
    //else if (checkSubString(header1K,  0,   3, ""           )){ fExt = ".";      }
    //else if (checkSubString(header1K,  0,   3, ""           )){ fExt = ".";      }
    //else if (checkSubString(header1K,  0,   3, ""           )){ fExt = ".";      }
    //else if (checkSubString(header1K,  0,   3, ""           )){ fExt = ".";      }
    //else if (checkSubString(header1K,  0,   3, ""           )){ fExt = ".";      }
    //else if (checkSubString(header1K,  0,   3, ""           )){ fExt = ".";      }
    //else if (checkSubString(header1K,  0,   3, ""           )){ fExt = ".";      }
    
    // Files which do not have a signature:
    // .pwn, 
    //logConsole(fExt + "CharCodes=" + header1K.charCodeAt(0) + ", " + 
    //                                 header1K.charCodeAt(1) + ", " + 
    //                                 header1K.charCodeAt(2) + ", " + 
    //                                 header1K.charCodeAt(3), 
    //                                 false, true );
  }
  if (fExt != ""){
    logConsole("File header analysis produced extension proposal '" + fExt + "'.", false, true);
  }
  else{
    logConsole("File header analysis did not produce a usable extension.", false, true);
  }
  return fExt;
}


//----------------------------------------------------------------
//! Heuristical function to find an appropriate file extension 
//! to select a loader; it removes numbers at the end of the file 
//! name and looks into the file header to identify the file 
//! format.
//! It returns a file extension if the format hopefully was
//! recognized or "" otherwise.
//----------------------------------------------------------------
function findOutFileExtension(fName)
{
  var pathComps = MLABFileManager.splitPath(fName);
  var lSuffix = "";
  if (pathComps && pathComps.ext){
    lSuffix = pathComps.ext.toLowerCase();
    // Ignore numbers as suffixes, they do not help to identify the file format.
    var isInt = true;
    const sufStr = lSuffix.substring(1);
    for (var s=0; s < sufStr.length; s++){
      const chCode = sufStr.charCodeAt(s);
      isInt &= (chCode >= 48) && (chCode <= 57);
    }
    if (isInt){
      lSuffix = "";
      logConsole("Ignoring number extension", false, true);
    }
  }
  if ((lSuffix == "") && ctx.field("analyzeHeader").value){
    lSuffix = examineHeader();
    if (lSuffix != ""){
      logConsole("Assuming file extension " + lSuffix, false, false);
    }
  }
  return lSuffix;
}

//----------------------------------------------------------------
//! If a loader of the same type as given by foundIdx still does 
//! not exist it is created newly (an old one is removed 
//! automatically).
//! The global fileLoader variable is also updated/reset and 
//! can be used to check validity/success of loader creation.
//----------------------------------------------------------------
function instantiateLoader(foundIdx)
{
  // Try to create the loader, or - if there is already an existing 
  // one of the same type - then leave the existing one unchanged.
  if (!fileLoader || (SuffixList[foundIdx][1] != loaderModName)){

    // If there is already a differing loader then remove it.
    if (fileLoader){
      ctx.network().removeModule (fileLoader);
      logConsole("Loader module deleted because a newer one is required", false, true);
      loaderModName = "";
      fileLoader    = 0;
    }
    
    // Only create a new loader if it differs from the previous one.
    fileLoader = ctx.network().createModule(SuffixList[foundIdx][1]);
    if (fileLoader){
      loaderModName = SuffixList[foundIdx][1];
      //logConsole("Loader instantiated for suffix '" + SuffixList[foundIdx][0] + "'", false, true);
    }
    else{
      logConsole("Loader instantiation failed!", true, false);
    }
  }
  else{
    // That's okay.
    //logConsole("Loader already exists!", false, true);
  }
}

//----------------------------------------------------------------
//! Sets information fields about the loader / file extension 
//! specified by the foundIdx.
//----------------------------------------------------------------
function setFormatInfos(foundIdx)
{     
  ctx.field("loaderName" ).setValue(SuffixList[foundIdx][1]);
  ctx.field("formatInfo" ).setValue(SuffixList[foundIdx][4]);
  ctx.field("writerNames").setValue(SuffixList[foundIdx][6]);
}

//----------------------------------------------------------------
//! Connect the loader output to all necessary other modules
//! in the internal network.
//----------------------------------------------------------------
function connectLoaderOutput(outputField, foundIdx)
{
  // Output found?
  if (!outputField || !fileLoader){
    logConsole("Internal error: Loader or its output field are not valid, cannot establish connections!", true, false);
  }
  else{
    // Yes, we have an output field. Connect it to the correct switch/bypass input.
    if (outputField.getType() == "SoNode"){ 
      // Note: Do not check for isInventorField, because SoNode outputs 
      // in Inventor are no inventor fields.
      ctx.field("SoSwitch.child").connectFrom(outputField);
      logConsole("Inventor connector SoSwitch.child identified and connected.", false, true);
    }
    else if (outputField.getType() == "Image"){
      ctx.field("Bypass.input0").connectFrom(outputField);
      logConsole("ML Image connector Bypass.input0 identified and connected.", false, true);
    }
    else if (outputField.getType()=="MLBase"){
      ctx.field("BaseBypassOp.inBaseField").connectFrom(outputField);
      logConsole("Base connector BaseBypassOp.inBaseField identified and connected.", false, true);
      if (SuffixList[foundIdx][1].substring(0,3) == "vtk"){
        // Connect vtk loaders also to the VTKInputInfos module.
        ctx.field("VTKInputInfos.inputvtkObject").connectFrom(outputField);
      }
    }
    else{
      ctx.field("textOut").connectFrom(outputField);
      logConsole("Atypical (field) result output identified and connected to text output field.", false, true);
    }
  }
}

//----------------------------------------------------------------
//! Returns a non empty string with output descriptions on valid 
//! loader, valid loader index, and if if the loader output is 
//! considered valid. Otherwiese an empty string is returned.
//----------------------------------------------------------------
function checkOutputValidity(outputField, foundIdx)
{
  if (!fileLoader || (foundIdx<0)){ return ""; }

  var validOutputs  = "";
  var vtkBaseOutput = false;
  var outputFound   = false;

  if  (SuffixList[foundIdx][1].substring(0,3) == "vtk"){
    // A vtk module. vtkLoaders have the problem that they always pass themselves at the 
    // base object. We have to look at the vtk loaders output with the vtkInputInfos module
    // to see whether load was successful. First select the object itself and check number 
    // of outputs.
    ctx.field("VTKInputInfos.idx").setIntValue(-1);
    // Does the loader has at least one output?
    if (ctx.field("VTKInputInfos.numberOfOutputs").intValue() > 0){

      // Yes, there is at least one. Select it.
      ctx.field("VTKInputInfos.idx").setIntValue(0);

      // Check some property configurations, which should be become valid on normally loaded files.
      var poa = (ctx.field("VTKInputInfos.ext"              ).value != "0 0 0 0 0 0");
      var pob = (ctx.field("VTKInputInfos.numberOfPoints"   ).value != "0"          );
      var poc = (ctx.field("VTKInputInfos.dataDimension"    ).value != "0"          );
      
      // For all non grid types such as tables, graphs etc. allow them to be loaded even if 
      // all other properties are invalid.
      var pod = (ctx.field("VTKInputInfos.dataObjectType"   ).value >= 10           );

      // Negativ indicators for valid loads.      
      var noa = ((ctx.field("VTKInputInfos.bounds"          ).value == "0 0 0 0 0 0" ) &&
                 (ctx.field("VTKInputInfos.ext"             ).value == "0 0 0 0 0 0" ) &&
                 (ctx.field("VTKInputInfos.dataObjectType"  ).value == "6"         ));
      //logConsole("poa=" + poa + ", pob=" + pob + ", poc=" + poc + ", noa=" + noa, false, true);
      validOutputs = (((poa || pob || poc) && !noa) || pod) ? "ML Base (with vtk object)" : "";
      if (validOutputs != ""){
        vtkBaseOutput = true;

        // Yes we have found an output or something which is considered as so. 
        outputFound   = true;
      }
    }
    else{
      logConsole("Validity check failed because of too little number of outputs.", false, true);
    }
    if (validOutputs == ""){
      logConsole("Validity check field in loader '" + SuffixList[foundIdx][1] + "' is invalid.", false, true);
    }
  }
  else if (SuffixList[foundIdx][8] != ""){
    // In the loader table a special field is specified whose value has to be checked for validity.
    var validityFieldValid = fileLoader.hasField(SuffixList[foundIdx][8]);
    if (!validityFieldValid){
      outFieldValid("Check of validity check field in loader '" + SuffixList[foundIdx][1] + "' is invalid.", true, false);
    }
    else{
      // Yes we have found an output or something which is considered as so. 
      outputFound = true;

      // Determine output validity.
      // Compare field value with given compare value.
      validOutputs = (fileLoader.field(SuffixList[foundIdx][8]).stringValue().toLowerCase() == 
                      SuffixList[foundIdx][9].toLowerCase()) ? "Special field" : "";
      if (validOutputs != ""){ 
        logConsole("Specific loader field of loader '" + SuffixList[foundIdx][1] + "' indicates validity.", false, true);
      }
      else{
        logConsole("Specific loader field of loader '" + SuffixList[foundIdx][1] + "' indicates invalidity. ('" + 
                   fileLoader.field(SuffixList[foundIdx][8]).stringValue().toLowerCase() + "' != '" +
                   SuffixList[foundIdx][9].toLowerCase() + "')", false, true);
      }
    }
  }
  else if (outputField.getType() == "SoNode"){
    // Yes we have found an output or something which is considered as so. 
    outputFound  = true;

    // Determine output validity.
    validOutputs = (outputField.nodeType() != "") ? "SoNode" : "";
    logConsole("Inventor output of loader could be validated", false, true);
  }
  else if (outputField.getType() == "Image"){ 
    // Yes we have found an output or something which is considered as so. 
    outputFound  = true;

    // Determine output validity.
    validOutputs = outputField.isValid() ? "ML Image" : "";
    logConsole("Image output of loader could " + ((validOutputs != "") ? "": "not ") + "be validated.", false, true);
  }
  else if (outputField.getType() == "MLBase"){
    // Yes we have found an output or something which is considered as so. 
    outputFound  = true;

    // Determine output validity.
    var outFieldVal = outputField.value();
    validOutputs = ((outFieldVal != undefined) && (outFieldVal != "NULL")) ? "MLBase" : "";
    if (validOutputs != ""){
      logConsole("Output field of loader could be validated with value '" + outFieldVal + "'", false, true);
    }
  }

  // Check whether we can also convert and connect a vtkImage to the ML image output.
  if (vtkBaseOutput && (validOutputs != "")){
    // Check whether the vtkToMLImage output is also valid. If yes then
    // connect it to the image output.
    if ((validOutputs != "") && ctx.field("VTKToMLImage.output0").isValid()){
      ctx.field("Bypass.input0").connectFrom(ctx.field("VTKToMLImage.output0"));
      logConsole("Image output of Bypass.output0 could be validated from VTKToMLImage.output0", false, true);
      validOutputs += " and ML Image";
    }
  }
  
  if (!outputFound){
    // No valid output connection found.
    logConsole("No valid output connection found in loader '" + SuffixList[foundIdx][1] + "'", false, true);
  }
  return validOutputs;
}


//----------------------------------------------------------------
//! Touches or sets a value of a certain field in the loader
//! module given by foundIdx. If the touchFieldName string contains
//! a "=" then the value right of "=" is assigned to the field, 
//! otherwise the field is only touched.
//! If SuffixList[foundIdx][7] is empty nothing is done.
//----------------------------------------------------------------
function setNonAutoFields(foundIdx)
{
  //--- Touch/set load fields/buttons of non automatic loaders.
  var touchFieldName = SuffixList[foundIdx][7];
  if (touchFieldName != ""){
    // Use an arbitrary assignment value.
    var assignValue = "";
    
    // Check whether the touchFieldName also contains an "=" which means 
    // that we have to assign a value.
    var equalIdx = touchFieldName.indexOf('=');
    if (equalIdx >= 0){
      // Yes we have to assign a specific value and need to shorten the field value itself.
      assignValue    = touchFieldName.substring(equalIdx+1, touchFieldName.length);
      touchFieldName = touchFieldName.substring(0, equalIdx);
    }
    
    // Explicit name of output field specified.
    if (fileLoader.hasField(touchFieldName)){
      logConsole("Touching field '" + touchFieldName + 
                 "' of module '" + fileLoader.fullName() + 
                 ((equalIdx < 0) ? "'." : ("' by assigning value '" + assignValue + "'.")), false, true);
      if (equalIdx >=0){
        // Assign the value.
        fileLoader.field(touchFieldName).value = assignValue;
      }
      else{
        // only touch the field, do not change the value.
        fileLoader.field(touchFieldName).touch();
      }
      logConsole("Touching finished.", false, true);
    }
    else{
      logConsole("Warning: Specified load field name is not part of the loader", true, false);
    }
  }

}


//----------------------------------------------------------------
//! If a created loader module exists the connect it inside of the
//! internal network with appropriate other modules to get its
//! results at the modules' outputs.
//! Returns a non empty string with output descriptions if 
//! everything worked fine, otherwise an empty string is returned.
//----------------------------------------------------------------
function updateAndConnectLoader(foundIdx)
{     
  // If we have a file loader then set format info field, get its output field,
  // and pass result to output conectors.
  var validOutputs = "";
  if (!fileLoader){
    logConsole("Invalid loader passed to updateAndConnectLoader", true, false);
  }
  else{
    logConsole("Initializing the loader...", false, true);
    
    //--- Determine output field.
    const outputFieldName = SuffixList[foundIdx][2];
    var outputField = 0;
    if (outputFieldName != ""){
      // Explicit name of output field specified.
      if (fileLoader.hasField(outputFieldName)){
        outputField = fileLoader.field(outputFieldName);
      }
    }
    else{
      // No output field name specified, take the first one if there is one.
      if (fileLoader.outputs().length > 0){
        outputField = fileLoader.outputs()[0];
      }
    }
    
    if (!outputField){
      logConsole("No output field found, stopping loader setup.", false, true);
    }
    else{

      // Connect loader output to appropriate other modules in the internal network.
      connectLoaderOutput(outputField, foundIdx);
      
      // Set up file name.
      const fNameFieldName = SuffixList[foundIdx][3];
      if (fNameFieldName != ""){
        // Explicit name of output field specified.
        if (fileLoader.hasField(fNameFieldName)){
          // Set file name.
          var fileNameField = fileLoader.field(fNameFieldName);
          fileNameField.value = ctx.field("trueName").value;
          logConsole("fileNameField was set to " + fileNameField.value, false, true);
                     
          //--- Touch/set load fields/buttons of non automatic loaders.
          setNonAutoFields(foundIdx);
          
          //-- Try to find out whether the module output is valid.
          validOutputs = checkOutputValidity(outputField, foundIdx);
          
          if (validOutputs != ""){
            logConsole(validOutputs + " output validated with loader '" + SuffixList[foundIdx][1] + "'.", false, false);
            setFormatInfos(foundIdx);
          }
          else{
            logConsole("Check with loader '" + SuffixList[foundIdx][1] + "' failed.", false, false);
          }

        }
        else{
          logConsole("Invalid file name field  '" + fNameFieldName + "' specified for loader module!" +
                     "(1) Not trying to load suffix '" + SuffixList[foundIdx][0] + "' with loader '" + SuffixList[foundIdx][1] + "'.", true, false);
        }
      }
      else{
        logConsole("Filename field '" + SuffixList[foundIdx][3] + "' for loader '" + SuffixList[foundIdx][1] + "' is not set up correctly!" +
                   "Not trying to load suffix '" + SuffixList[foundIdx][0] + "' with loader '" + SuffixList[foundIdx][1] + "'.", true, false);
      }

      //--- Touch load field of non automatic loaders.
      // If a field has to be touched to load the data then do it here.
      if (validOutputs == ""){
        logConsole("Output not validated, stopping loader setup.", false, true);
      }

    } // if (outputField)

  } // if else (fileLoader)
  return validOutputs;
}

//----------------------------------------------------------------
//! Checks module module name, tries to instantiate and to connect
//! it with the module outouts.
//! If this failed then it returns 
//! * 0 on failures,
//! * 1 on valid but not automatically usable loaders, and
//! * 2 on success.
//----------------------------------------------------------------
function tryToLoadWith(foundIdx)
{
  var retVal = 0;

  // Check the loader at foundIdx.
  const loaderName      = SuffixList[foundIdx][1];
  const validModuleName = moduleNameExists(loaderName);
  if ("" == validModuleName){
    // Avoid script errors on invalid modules.
    logConsole("Could not check loader '" + loaderName + "', because it is not available!", false, true);
    retVal = 0;
  }
  else{
    // Yes, we have a valid loader name.
    
    // Check whether the module can be configured automatically.
    if (SuffixList[foundIdx][5]=="No"){
      // Loader is available, but cannot be configured automatically.
      // Finish search if loader is found, but cannot be used.
      // Continue only if all loaders shall be checked.
      setFormatInfos(foundIdx);
      logConsole("Loader '" + loaderName + "' found, but you must use and configure it manually.", false, false);
      retVal = 1;
    }
    else{
      // Create or update the loader specified at foundIdx.
      instantiateLoader(foundIdx);
      
      // If we have a loader then update its file name and on a valid result 
      // connect its result to it the output connectors.
      if (fileLoader){
        //logConsole("Starting loader connections to outputs...", false, true);
        retVal = updateAndConnectLoader(foundIdx) != "" ? 2 : 0; // 2 = success, 0 on failure.
      }
    }
      
  }
  return retVal;
}

//----------------------------------------------------------------
//! Scans table and tries to set up an appropriate loader to
//! get valid module outputs.
//! * 1 on valid but not automatically usable loaders, and
//! * 2 on success.
//! * general failures on all other values.
//----------------------------------------------------------------
function searchAndSetUpLoader(fName, checkTypical, checkAll)
{
  // Check for a valid file name.
  var retVal = 4;
  if (fName==""){
    logConsole("File name is empty", true, false);
    clear();
    return 0;
  }

  // In checkAll mode remove previous loader because only differeng loaders are checked.
  if (checkAll){
    clear();
  }

  // Additional info about file, loader or format.
  var matchButNotWorkList = "";
  
  // Index to be set to the finally found loader.
  var foundIdx = -1;
    
  // If we do not have a checktypical or checkAll mode which do not evaluate file extensions then look 
  // for a file extension or try to analyze the header (if allowed) or to guess.
  var lSuffix = "";
  if (!checkTypical && !checkAll){
    lSuffix = findOutFileExtension(fName);
  }

  // For dicom file always examine the header first, if necessary replace it to use 
  // internal loader, because both internal and normal loaders crash on wrong files.
  if ((lSuffix.toLowerCase() == ".dcm") && (ctx.field("analyzeHeader").value)){
    lSuffix = examineHeader(fName);
  }
  
  // Check for failure.
  if ((lSuffix == "") && !checkTypical && !checkAll){
    // Hmm, we coud not find an appropriate suffix.
    clear();
    var addVal = "";
    if (!ctx.field("analyzeHeader").value){
      addVal = " Perhaps the activation of 'Use header Analysis' helps.";
    }
    logConsole("Could not determine a useful extension." + addVal, true, false);
    retVal = 3;
  }
  else{
    // We have either a valid file extension or we are in a checkTypical or checkAll mode.
    if (lSuffix == ""){
      if (checkTypical){ logConsole("Trying to find loader (without using the extension) by checking typical loaders.", false, true); }
      if (checkAll    ){ logConsole("Trying to find loader (without using the extension) by checking all loaders."    , false, true); }
    }
    else{
      logConsole("Trying to find loader for extension '" + lSuffix + "'.", false, true);
    }
    
    // Traverse entire suffix/loader table and look for a matching loader.
    // Do this twice in case of a non found loader whereby the second trial 
    // is done with a suffix determined from a heuristic analysis.
    // If this is disabled then run only once.
    const allowHA       = ctx.field("analyzeHeader").value;
    const numTrials     = allowHA ? 2 : 1;
    const numSuffixes   = SuffixList.length;
    const hasBioFormats = moduleNameExists("BioFormatsLoad") != "";
    
    // Run twice if necessary and in first loop is nothing found.
    for (var trials = 0; (foundIdx<0) && (trials < numTrials); trials++){
      
      // First loop finished and no loader found?
      if (1 == trials){
        // Reset  foundIdx in second loop.
        foundIdx = -1;

        // If we already have a valid loader then do not scan a second time.
        if ((2 == retVal) || (1 == retVal)){
          break;
        }
        else{
          // Search with another file extension determined from the header analysis, if allowed.
          logConsole("Trying again with automatically detected file extension.", false, true);
          lSuffix = examineHeader();
          if (lSuffix == ""){ 
            // Bad luck, even header analysis did not succedd. Do not run again.
            logConsole("Even header analysis did not succeed; the file could not openened.", false, false);
            break;
          }
          else{
            logConsole("Assuming file extension " + lSuffix, false, false);
          }
        }
      }
      
      // Check suffix against all table entries.
      for (var fn=0; (foundIdx<0) && (fn < numSuffixes); fn++){
        
        // Get suffix of loader to check.
        const currSuffix = SuffixList[fn][0];
        var doCheck = false;

        //logConsole("Checking extension '" + currSuffix + "'", false, true);
        if (checkAll ||                                                       // Check all Loaders, ignoring the suffix.
            (checkTypical && (currSuffix=="*")) ||                            // Check most common loaders, ignoring the suffix.
            (!checkTypical && (currSuffix != "") && (currSuffix == lSuffix))  // Check loaders with matching suffix.
            ){
          doCheck  = true;
          logConsole("Checking index '" + fn + 
                     "' with extension '" + SuffixList[fn][0] + 
                     "' and loader '" + SuffixList[fn][1] + "'.", false, true);
        }
        
        // No index found? Then check next table entry.
        if (!doCheck){
          continue;
        }
        
        // In checkAll mode avoid that one loader is checked more than once.
        if (checkAll && (SuffixList[fn][1] == loaderModName)){
          continue;
        }
        
        // If we make use of the BioFormats module then allow it only if the corresponding flag is enabled.
        if (hasBioFormats && (SuffixList[fn][1] == "BioFormatsLoad") && !ctx.field("allowBioFormats").value){
          logConsole("Not checking 'BioFormatsLoad' (although it is available!), because its use is disabled.", false, true);
          continue;
        }
        
        //logConsole("fi=" + foundIdx + ", fn=" + fn + ", checkAll=" + checkAll + ", retVal = " + retVal + ", doCheck=" + doCheck + ".");

        // Try to find, create and use a loader to get valid module outputs. returns
        //! * 0 on failures,
        //! * 1 on valid but not automatically usable loaders, and
        //! * 2 on success.
        retVal = tryToLoadWith(fn);
        if ((1==retVal) || (2==retVal)){
          // Yes, we found a module with success or manual use.
          foundIdx = fn;
        }
        else{
          if (SuffixList[fn][1] != ""){
            matchButNotWorkList += "Check also: Loader '" + SuffixList[fn][1] + "' matches file extension, but cannot load it.\n"
          }
          if (SuffixList[fn][4] != ""){
            matchButNotWorkList += "Check also: " + SuffixList[fn][4] + "'.\n";
          }
        }
      } // for (var fn=0; fn < numSuffixes; fn++)
      
    } // for (trials)
    
  } // else of if ((lSuffix == "") && !checkTypical && !checkAll)
  
  // Nothing matched fine. 
  if (2 == retVal){
    // Success!
    logConsole("File could be opened.", false, true);
  }
  else if (1 == retVal){
    //! * 1 on valid but not automatically usable loaders
    logConsole("Loader for manual use found.", false, true);
  }
  else if (3 == retVal){
    // No useful extension found. Touch trueName field to call header 
    // load to have header information in short and header info fields 
    // for further analysis.
    ctx.field("trueName").touch();
    logConsole("No hint or extension found to find an appropriate loader; the file could not openened.", false, false);
  }
  else if (4 == retVal){
    // No matching extension found at all.
    ctx.field("trueName").touch();
    logConsole("No matching extension found to find an appropriate loader; the file could not openened.", false, false);
    if (!ctx.field("analyzeHeader").value){
      logConsole("Perhaps the activation of 'Use header Analysis' helps.", false, false);
    }
  }
  else{
    // General failure, normally 0.
    logConsole("Could not find any appropriate loader to open the file.", false, false);
  }

  // Print additional information if available.
  if (matchButNotWorkList != ""){
    if (2==retVal){
      // On success print only in verbose mode.
      logConsole(matchButNotWorkList, false, true);
    }
    else{
      // On suboptimal loader find or failure always print additional information.
      logConsole(matchButNotWorkList, false, false);
    }
  }
  
  if ((2==retVal) || (1==retVal)){
    if (SuffixList[foundIdx][1] == "BioFormatsLoad"){
      // Be sure to print license information for the BioFormats loader, because this is a requirement for its use in our house.
      logConsole("The used BioFormatsLoad module is based on Bio-Formats (c) 2008 University of Wisconsin and Glencoe Software, Inc", false, false);
    }
  }
  return retVal;
}

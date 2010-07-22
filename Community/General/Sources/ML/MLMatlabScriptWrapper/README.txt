The MatlabScriptWrapper module makes it possible to execute Matlab scripts within
the MeVisLab environment, and it provides an interface to transfer data between
MeVisLab and Matlab. Matlab must be installed for this module to work. Furthermore,
as the MatlabScriptWrapper must be linked against the installed Matlab version,
there is no binary version of this module. This means that you must compile the
MatlabScriptWrapper yourself. The basic steps for doing this are explained below.

1. The following components must be installed on your system:
  - An SDK version of MeVisLab (available at www.mevislab.de)
  - A C++ compiler, e.g., Visual Studio for Windows or gcc for Linux.
  - Python 2.5 (Python 3.0 or higher will not work).
  - A local installation of Matlab. 

  - Linux only: the C Shell csh
  
  If you use Windows, the free Visual Studio Express version may be used
  as compiler.
  
Steps 2 and 3 describe the compile process under Windows. Similar steps are
required for Mac OS X or Linux.
  
2. In the MLMatlabScriptWrapper.pro file, the path to the Matlab libraries must be set.
   For this, you will need to locate the required Matlab *.lib files. For example, 
   for a 64-bit Windows version of Matlab 2007b installed under "C:/Program Files/MATLAB",
   the following path must be set:
    
   MATLAB_LIBDIR = "C:/Program Files/MATLAB/R2007b/extern/lib/win64/microsoft"

   In addition, the path to the engine.h file must be added. For the above example, this is done with
   the following line
   
   INCLUDEPATH += "C:/Program Files/MATLAB/R2007b/extern/include"

3. Save the updated MLMatlabScriptWrapper.pro file and execute the MLMatlabScriptWrapper.bat file. This
   will create a C++ project for your system. Open the project file in Visual Studio and compile.

4. Start MeVisLab and make sure that the Community Sources package is in the search path of MeVisLab. This
   is set under Edit->Preferences->Packages in MeVisLab. Make sure that the Community/General package is
   scanned by inspecting the Debug Output window.

5. When the MatlabScriptWrapper module is dragged onto the MeVisLab workspace, a Matlab session should start
   and a Matlab console window appear. If the module is red, the correct DLL files are not found or loaded.
   Check the following:
   
   - That you started the Release mode of MeVisLab if you compiled in release mode,
     or in Debug mode if you compiled in debug mode.
   - That the path to the Matlab DLL-files is not known to the system. Under Windows, you may have to add the path
     to the Matlab DLL-files to the PATH environment variable manually. The Matlab
     DLL files are found in a subdirectory in the $MATLABROOT$/bin/ directory, e.g., in
     C:\Program Files\MATLAB\R2007b\bin\win32
     Make sure that this directory is in the PATH environment variable and restart MeVisLab after the PATH variable
     has been changed.
     
   If the module is still red, the DependencyWalker program can be used to find out which DLL-files that are missing.
   If you have DependencyWalker installed, it can be executed by right-clicking on the red MatlabScriptWrapper module
   in MeVisLab and then choosing "Debugging"->"Show DLL Dependency"
   On Linux this can be done with /usr/bin/ldd
   
If you experience troubles, please search the MeVisLab forum http://www.mevis-research.de/cgi-bin/discus/discus.cgi


General hints:

- Under Windows, you may have to execute the command "matlab \regserver" from a Windows command prompt (not the Matlab prompt!).
- Under Linux you need to set your LD_LIBRARY_PATH to include the matlab directories before you run MeVisLab:
    setenv LD_LIBRARY_PATH ${MATLAB_ROOT}/bin/glnxa64/:$LD_LIBRARY_PATH
  Replace ${MATLAB_ROOT} with the directory where matlab is installed. Use /bin/glnx86/ for the 32 bit version.
  Should the module still be red also add ${MATLAB_ROOT}/sys/os/glnxa64/ to your LD_LIBRARY_PATH.
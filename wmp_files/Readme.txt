!!!BEFORE TRYING TO BUILD THE SwiftMiX PROJECT!!!

I've pre-built the bpl "My Controls" library (Windows Media Player) library which you can install in C++ Builder with:

Component->Install Packages->Add then navigate to MyControls.bpl (located in WMP_BplLibrary.zip) and install.

NOTE: After building project MyControls.cbproj, the output library, MyControls.bpl file is located in folder:

 \Users\Public\Public Documents\Embarcadero\Studio\22.0\BPL
--------------------------

Alternatively, you don't need any of these files, you can import the Windows Media Player Active-X control yourself if you are using Windows 10/11:

In the Embarcadero RAD Studio C++ Builder IDE:

Component->Import Component
choose Import Active-X Control
Press Next
Scroll near the bottom of the list and choose Windows Media Player

--------------------------

Or... unzip MyControlsProject.zip into your C++ Builder projects directory.

Open C++ Builder and Open->Open Project and choose MyControls.cbproj.

In the rightmost window, click right on MyControls.bpl and click Install.

The project "should" :-) build and install the Windows Media Player.

Install directories for C++ Builder on Windows 10/11 are - a pain. It seems to install the BPL in
C:\Users\Public\Documents\Embarcadero\Studio\22.0\BPL. But on Windows I have a "Public Documents" folder...
And here's another folder - does Windows move files here???
C:\Users\(you)\Documents\Embarcadero\Studio\22.0\Imports

--------------------------

NOW you can open and build the SwiftMiX project!
I've pre-built the bpl "My Controls" library (Windows Media Player) library which you can install in C++ Builder with:

Component->Install Packages->Add then navigate to MyControls.bpl and install.

--------------------------

Alternatively, you don't need any of these files, you can import the Windows Media Player Active-X control yourself if you are using Windows 10:

Component->Import Component
choose Import Active-X Control
Press Next
Scroll near the bottom of the list and choose Windows Media Player

--------------------------

Or... put MyControlsProject into your C++ Builder projects directory.

Open C++ Builder and Open->Open Project and choose MyControls.

In the rightmost window, click right on MyControls.bpl and click Install.

The project "should" :-) build and install the Windows Media Player.

Install directories for C++ Builder on Windows 10 are - a pain. It seems to install in
C:\Users\Public\Documents\Embarcadero\Studio\21.0\BPL. But on Windows I have a "Public Documents" folder...
And here's another folder - does Windows move files here???
C:\Users\(you)\Documents\Embarcadero\Studio\21.0\Imports

--------------------------

NOW you can open and build the SwiftMiX project!
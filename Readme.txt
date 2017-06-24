Software by Scott Swift 2017 This program is distributed under the
terms of the GNU General Public License. Distribute freely.

# swiftmix-borland-cpp
SwiftMiX is a DJ-like music-mixer that uses Windows-Media Player Active-X components. It can import/export/convert most playlists and handles Unicode file-paths.

This project builds with Embarcadero RAD Studio 10.2 (Tokyo)

Free compiler: [Embarcadero C++ Builder 10.2 Tokyo Starter Edition](https://www.embarcadero.com/products/cbuilder/starter/promotional-download)

Before loading the project, you need to install the Windows Media Player Control as follows:
Click Component->Import Component->Import Active-X Control
Click Next and scroll down the list and choose Windows Media Player

Now you can click File->Open Project and choose SwiftMiXeR.cbproj

The project also needs access to MFC header-files for its custom, subclassed open-file dialog.
Add $(BDSINCLUDE)\windows\sdk; to the resource-compiler include path:
Project->Options->Resource Compiler->Directories and Conditionals->Include File Search Path

Project web-site:

http://swiftmix.yahcolorize.com

Contact: dxzl@live.com

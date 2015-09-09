Copyright 2015 Scott Swift - This program is distributed under the
terms of the GNU General Public License.

UNLESS YOU HAVE C++ Builder 4 or 5 I don't recommend trying to build this
project - I've tried without the original IDE and system files and it's a royal
pain and probably a waste of your valuable time.

Use Borland C++ Builder 4 to compile. The project file has the .bpr
extension and can be edited with a normal text editor. The free compiler
can probably compile it but you will need to open SwiftMiX.bpr
and use it to derive compiler and linker command-lines for your own
makefile. I have included all referenced library and .obj files (I hope!)
in lib.zip.

If you ARE using the Borland (now called Embarcadero) IDE, you need to
install the Windows Media Player Active-X control. Choose
Component->Import Active-X Control then select Windows Media Player (down
at the bottom of the list).

Below is a snippet from DEPLOY.TXT in my licensed version of
C++ Builder 4 that says I can distribute the libraries:

2.4 Runtime Packages: Professional Edition
------------------------------------------
If you are a licensed user of C++Builder 4 Professional,
redistributable runtime packages include the following:

  IBEVNT40.BPL
  IBSMP40.BPL
  NMFAST40.BPL
  QRPT40.BPL
  TEE40.BPL
  TEEDB40.BPL
  TEEUI40.BPL
  VCL40.BPL
  VCLDB40.BPL
  VCLDBX40.BPL
  VCLJPG40.BPL
  VCLSMP40.BPL
  VCLX40.BPL

e-mail: dxzl@live.com
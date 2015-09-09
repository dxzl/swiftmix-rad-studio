//---------------------------------------------------------------------------
#include <vcl.h>
#include "Main.h"
#pragma hdrstop
USERES("SwiftMiX.res");
USEFORM("Main.cpp", MainForm);
USEFORM("About.cpp", AboutForm);
USEFORM("SMList.cpp", PlaylistForm);
USEFORM("Progress.cpp", ProgressForm);
USEFORM("AutoSize.cpp", AutoSizeForm);
USEFORM("FormExportMode.cpp", ExportModeForm);
USEUNIT("DefaultStrings.cpp");
USEUNIT("MyCheckLst.pas");
USEUNIT("XmlTable.cpp");
USEFORM("FormImportMode.cpp", ImportModeForm);
USERC("SwiftMiX.rc");
USEUNIT("RegHelper.cpp");
USEFORM("FormSFDlg.cpp", SFDlgForm);
USEFORM("FormDirDlg.cpp", DirDlgForm);
USEFORM("FormImport.cpp", ImportForm);
USEFORM("FormExport.cpp", ExportForm);
USEUNIT("FormOFMSDlg.cpp");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  try
  {
    Application->Initialize();
    Application->Title = "SwiftMiX";
    Application->CreateForm(__classid(TMainForm), &MainForm);
     Application->Run();
  }
  catch (Exception &exception)
  {
     Application->ShowException(&exception);
  }
  return 0;
}
//---------------------------------------------------------------------------

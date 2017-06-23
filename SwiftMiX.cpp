//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
#include <Vcl.Styles.hpp>
#include <Vcl.Themes.hpp>
USEFORM("Progress.cpp", ProgressForm);
USEFORM("FormSFDlg.cpp", SFDlgForm);
USEFORM("Main.cpp", MainForm);
USEFORM("SMList.cpp", PlaylistForm);
USEFORM("FormDirDlg.cpp", DirDlgForm);
USEFORM("About.cpp", AboutForm);
USEFORM("AutoSize.cpp", AutoSizeForm);
USEFORM("FormImportMode.cpp", ImportModeForm);
USEFORM("FormOFMSDlg.cpp", OFMSDlgForm);
USEFORM("FormImport.cpp", ImportForm);
USEFORM("FormExport.cpp", ExportForm);
USEFORM("FormExportMode.cpp", ExportModeForm);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    try
    {
         Application->Initialize();
         Application->MainFormOnTaskBar = true;
         TStyleManager::TrySetStyle("Golden Graphite");
     Application->CreateForm(__classid(TMainForm), &MainForm);
     Application->Run();
    }
    catch (Exception &exception)
    {
         Application->ShowException(&exception);
    }
    catch (...)
    {
         try
         {
             throw Exception("");
         }
         catch (Exception &exception)
         {
             Application->ShowException(&exception);
         }
    }
    return 0;
}
//---------------------------------------------------------------------------

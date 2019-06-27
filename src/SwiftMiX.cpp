//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
#include <Vcl.Styles.hpp>
#include <Vcl.Themes.hpp>
USEFORM("Progress.cpp", ProgressForm);
USEFORM("Main.cpp", MainForm);
USEFORM("SMList.cpp", PlaylistForm);
USEFORM("FormSFDlg.cpp", SFDlgForm);
USEFORM("FormDirDlg.cpp", DirDlgForm);
USEFORM("AutoSize.cpp", AutoSizeForm);
USEFORM("About.cpp", AboutForm);
USEFORM("FormExport.cpp", ExportForm);
USEFORM("FormOFMSDlg.cpp", OFMSDlgForm);
USEFORM("FormImportMode.cpp", ImportModeForm);
USEFORM("FormImport.cpp", ImportForm);
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
     Application->CreateForm(__classid(TProgressForm), &ProgressForm);
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

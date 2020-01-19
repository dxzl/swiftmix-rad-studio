//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
#include <Vcl.Styles.hpp>
#include <Vcl.Themes.hpp>
USEFORM("Progress.cpp", ProgressForm);
USEFORM("Main.cpp", MainForm);
USEFORM("FormSFDlg.cpp", SFDlgForm);
USEFORM("FormSearchUncheck.cpp", SearchUncheckForm);
USEFORM("SMList.cpp", PlaylistForm);
USEFORM("AutoSize.cpp", AutoSizeForm);
USEFORM("About.cpp", AboutForm);
USEFORM("FormDirDlg.cpp", DirDlgForm);
USEFORM("FormOFMSDlg.cpp", OFMSDlgForm);
USEFORM("FormImportMode.cpp", ImportModeForm);
USEFORM("FormImport.cpp", ImportForm);
USEFORM("FormExportMode.cpp", ExportModeForm);
USEFORM("FormExport.cpp", ExportForm);
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

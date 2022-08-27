//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
#include <Vcl.Styles.hpp>
#include <Vcl.Themes.hpp>
USEFORM("FormExportMode.cpp", ExportModeForm);
USEFORM("FormImport.cpp", ImportForm);
USEFORM("FormImportMode.cpp", ImportModeForm);
USEFORM("FormOFMSDlg.cpp", OFMSDlgForm);
USEFORM("FormSearchUncheck.cpp", SearchUncheckForm);
USEFORM("FormExport.cpp", ExportForm);
USEFORM("About.cpp", AboutForm);
USEFORM("AutoSize.cpp", AutoSizeForm);
USEFORM("SMList.cpp", PlaylistForm);
USEFORM("TagEditForm.cpp", FormTags);
USEFORM("Main.cpp", MainForm);
USEFORM("Progress.cpp", ProgressForm);
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

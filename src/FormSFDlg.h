//---------------------------------------------------------------------------
#ifndef FormSFDlgH
#define FormSFDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>

// For SwiftMiX use:
#define SFDbg MainForm
#define SFUtil MainForm
// For YahCoLoRiZe use:
//#define SFDbg DTSColor
//#define SFUtil utils

//---------------------------------------------------------------------------
// Control id's for common dialog box controls
// see http://msdn.microsoft.com/en-us/library/ms646960.aspx#_win32_Explorer_Style_Control_Identifiers
// for a complete list.
// S.S. these are in dlgs.h
//chx1 The read-only check box
//cmb1 Drop-down combo box that displays the list of file type filters
//stc2 Label for the cmb1 combo box
//cmb2 Drop-down combo box that displays the current drive or folder, and that allows the user to select a drive or folder to open
//stc4 Label for the cmb2 combo box
//cmb13 Drop-down combo box that displays the name of the current file, allows the user to type the name of a file to open, and select a file that has been opened or saved recently. This is for earlier Explorer-compatible applications without hook or dialog template. Compare with edt1.
//edt1 Edit control that displays the name of the current file, or allows the user to type the name of the file to open. Compare with cmb13.
//stc3 Label for the cmb13 combo box and the edt1 edit control
//lst1 List box that displays the contents of the current drive or folder
//stc1 Label for the lst1 list box
//IDOK The OK command button (push button)
//IDCANCEL The Cancel command button (push button)
//pshHelp The Help command button (push button)
#define stc2 0x0441
#define cmb1 0x0470
#define stc3 0x0442
#define edt1 0x0480
#define cmb13 0x047c
#define lst1 0x0460
#define lst2 0x0461

// Control aliases....
#define ID_FilterCombo cmb1
#define ID_FilterLabel stc2
#define ID_FileNameLabel stc3
// cmb13 is now used for edt1 and you must use
// HWND hWndEdit = (HWND)SendMessage(hFileNameCombo, CBEM_GETEDITCONTROL, 0, 0);
// to get a handle to the inner edit-control in it.
#define ID_FileName edt1
#define ID_FileList lst1
#define ID_FileNameCombo cmb13

#define ID_MyFileName ID_FileNameCombo

#define SF_BUFSIZE (MAX_PATH*10)
//---------------------------------------------------------------------------
class TSFDlgForm : public TForm
{
__published:  // IDE-managed Components
  void __fastcall FormDestroy(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormActivate(TObject *Sender);

private:
  String __fastcall ExtractFileExtW(String sIn);
  WideChar* __fastcall SetFilter(void);
  wchar_t* __fastcall GetFilter(wchar_t* pFilterBuf, int iFilter, int iMax);
  int __fastcall FindFilter(wchar_t* pFilterBuf, wchar_t* pFilterToFind, int iMax);
  String __fastcall GetFolderPath(HWND hDlg);

  WideChar* p_szFileName;
  WideChar* p_szTitleName;

  OPENFILENAMEW m_sfn;

  int FFilterCount, FFilterIndex;

  WideChar* p_filterBuf;
  String FFilters, FFileNameLabel;
  String FDefFile, FDefExt;
  String FCurrentFilter, FCurrentFolder;
  String FFileName, FInitialDir, FDlgTitle;

  HWND FDlgHandle;

protected:

  String __fastcall GetTitle(void);

  static UINT CALLBACK SFNHookProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

public:    // User declarations
  bool __fastcall Execute(String uDefFile, String uInitialDir,
                                                            String uDlgTitle);

  __property HWND DlgHandle = {read = FDlgHandle};
  __property int FilterCount = {read = FFilterCount};
  __property int FilterIndex = {read = FFilterIndex};
  __property String Title = {read = GetTitle};
  __property String FileName = {read = FFileName};
  __property String Filters = {read = FFilters, write = FFilters};
  __property String FileNameLabel = {read = FFileNameLabel, write = FFileNameLabel};
  __property String DefFile = {read = FDefFile};
  __property String DefExt = {read = FDefExt};
  __property String CurrentFolder = {read = FCurrentFolder};
  __property String CurrentFilter = {read = FCurrentFilter};
  __property String InitialDir = {read = FInitialDir};
  __property String DlTitle = {read = FDlgTitle};
};
//---------------------------------------------------------------------------
extern PACKAGE TSFDlgForm *SFDlgForm;
//---------------------------------------------------------------------------
#endif

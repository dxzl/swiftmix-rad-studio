//---------------------------------------------------------------------------
#ifndef FormSFDlgH
#define FormSFDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

// For SwiftMiX use:
#define SFDbg MainForm
#define SFUtil MainForm
// For YahCoLoRiZe use:
//#define SFDbg DTSColor
//#define SFUtil utils

// Control aliases....
#define ID_FilterCombo cmb1
#define ID_FilterLabel stc2
#define ID_FileNameLabel stc3
#define ID_FileName edt1
#define ID_FileList lst1
#define ID_FileNameCombo cmb13

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
  String __fastcall GetTextFromCommonDialog(HWND hDlg, UINT msg);

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

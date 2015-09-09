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
__published:	// IDE-managed Components
  void __fastcall FormDestroy(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormActivate(TObject *Sender);

private:
  WideString __fastcall ExtractFileExtW(WideString sIn);
  WideChar* __fastcall SetFilter(void);
  wchar_t* __fastcall GetFilter(wchar_t* pFilterBuf, int iFilter, int iMax);
  int __fastcall FindFilter(wchar_t* pFilterBuf, wchar_t* pFilterToFind, int iMax);
  WideString __fastcall GetTextFromCommonDialog(HWND hDlg, UINT msg);

  WideChar* p_szFileName;
  WideChar* p_szTitleName;

  OPENFILENAMEW m_sfn;

  int FFilterCount, FFilterIndex;

  WideChar* p_filterBuf;
  WideString FFilters, FFileNameLabel;
  WideString FDefFile, FDefExt;
  WideString FCurrentFilter, FCurrentFolder;
  WideString FFileName, FInitialDir, FDlgTitle;

  HWND FDlgHandle;

protected:

  String __fastcall GetFileNameUtf8(void);
  String __fastcall GetTitleUtf8(void);

  static UINT CALLBACK SFNHookProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

public:		// User declarations
  bool __fastcall ExecuteW(WideString wDefFile, WideString wInitialDir,
                                                            String uDlgTitle);
  bool __fastcall ExecuteU(String uDefFile, String uInitialDir,
                                                            String uDlgTitle);

  __property HWND DlgHandle = {read = FDlgHandle};
	__property int FilterCount = { read = FFilterCount };
	__property int FilterIndex = { read = FFilterIndex };
  __property String TitleUtf8 = {read = GetTitleUtf8 };
  __property String FileNameUtf8 = {read = GetFileNameUtf8 };
  __property WideString FileName = {read = FFileName};
  __property WideString Filters = {read = FFilters, write = FFilters};
  __property WideString FileNameLabel = {read = FFileNameLabel, write = FFileNameLabel};
  __property WideString DefFile = {read = FDefFile};
  __property WideString DefExt = {read = FDefExt};
  __property WideString CurrentFolder = {read = FCurrentFolder};
  __property WideString CurrentFilter = {read = FCurrentFilter};
  __property WideString InitialDir = {read = FInitialDir};
  __property WideString DlTitle = {read = FDlgTitle};
};
//---------------------------------------------------------------------------
extern PACKAGE TSFDlgForm *SFDlgForm;
//---------------------------------------------------------------------------
#endif

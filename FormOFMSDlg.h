/*------------------------------------------------------------------
// Author: Mr. Swift 2015
// Thanks to Scott Wisniewski for his 2010 C# subclassed file-dialog
// and to Denis Ponomarenko for his 2007 COSFDlg class implementation.
// I could not have done this without their excellent source-code
// to "show me how".
//
// https://github.com/scottwis/OpenFileOrFolderDialog
// http://www.codeproject.com/Articles/35529/CBrowseFolderDialog-Class
// http://www.codeproject.com/Articles/17916/Simple-MFC-independent-Open-Save-File-Dialog-class
// http://www.codeproject.com/Articles/3235/Multiple-Selection-in-a-File-Dialog
//
  ----------------------------------------------------------------*/
//---------------------------------------------------------------------------
#ifndef FormOFMSDlgH
#define FormOFMSDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <CommCtrl.h>
#include <ExtCtrls.hpp>
#include <SysUtils.hpp>
#include <Forms.hpp>
#include <Windows.h>
#include <vcl\olectnrs.hpp>

// For SwiftMiX use:
#define OFDbg MainForm
#define OFUtil MainForm
// For YahCoLoRiZe use:
//#define OFDbg DTSColor
//#define OFUtil utils

#define WM_ITEMS_SELECTED (WM_USER+1)
//---------------------------------------------------------------------------
class TWideItem
{
public:
  WideString s;
  bool IsDirectory;
};
//---------------------------------------------------------------------------
// Control id's for common dialog box controls
// see http://msdn.microsoft.com/en-us/library/ms646960.aspx#_win32_Explorer_Style_Control_Identifiers
// for a complete list.
// S.S. these are in dlgs.h
//chx1	The read-only check box
//cmb1	Drop-down combo box that displays the list of file type filters
//stc2	Label for the cmb1 combo box
//cmb2	Drop-down combo box that displays the current drive or folder, and that allows the user to select a drive or folder to open
//stc4	Label for the cmb2 combo box
//cmb13	Drop-down combo box that displays the name of the current file, allows the user to type the name of a file to open, and select a file that has been opened or saved recently. This is for earlier Explorer-compatible applications without hook or dialog template. Compare with edt1.
//edt1	Edit control that displays the name of the current file, or allows the user to type the name of the file to open. Compare with cmb13.
//stc3	Label for the cmb13 combo box and the edt1 edit control
//lst1	List box that displays the contents of the current drive or folder
//stc1	Label for the lst1 list box
//IDOK	The OK command button (push button)
//IDCANCEL	The Cancel command button (push button)
//pshHelp	The Help command button (push button)
//#define stc2 0x0441
//#define cmb1 0x0470
//#define stc3 0x0442
//#define edt1 0x0480
//#define cmb13 0x047c
//#define lst1 0x0460
// #define lst2 0x0461
//#define IDOK 1
//#define IDCANCEL 2
// Control aliases that actually make sense....
#define ID_FilterCombo cmb1
#define ID_FilterLabel stc2
#define ID_FileNameLabel stc3
#define ID_FileName edt1
#define ID_FileList lst1
#define ID_FileNameCombo cmb13

// NOTE: These constants are defined in resource.h
//#define IDD_CustomOpenDialog 101
//#define ID_SELECT 1001
//#define ID_CUSTOM_CANCEL 1002

#define ID_FILE_SUBCLASS_PROC 0
#define ID_LISTVIEW_SUBCLASS_PROC 1
#define ID_FILEEDIT_SUBCLASS_PROC 2

#define OF_BUFSIZE (MAX_PATH*10)

#define BUTTON_GAP 4

#define INVALID_FILE_ATTRIBUTES 0xFFFFFFFF

typedef void __fastcall (__closure *TOFMSDlgFolderChangeEvent)(TObject* Sender, TWMNotify& Message);

//
// subclassing stuff
//
typedef LRESULT (CALLBACK* SUBCLASSPROC)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT* uIdSubclass, DWORD* dwRefData);

//---------------------------------------------------------------------------
// This is for the new version IFileDialog
/*
typedef enum _FILEOPENDIALOGOPTIONS {
  FOS_OVERWRITEPROMPT         = 0x00000002,
  FOS_STRICTFILETYPES         = 0x00000004,
  FOS_NOCHANGEDIR             = 0x00000008,
  FOS_PICKFOLDERS             = 0x00000020,
  FOS_FORCEFILESYSTEM         = 0x00000040,
  FOS_ALLNONSTORAGEITEMS      = 0x00000080,
  FOS_NOVALIDATE              = 0x00000100,
  FOS_ALLOWMULTISELECT        = 0x00000200,
  FOS_PATHMUSTEXIST           = 0x00000800,
  FOS_FILEMUSTEXIST           = 0x00001000,
  FOS_CREATEPROMPT            = 0x00002000,
  FOS_SHAREAWARE              = 0x00004000,
  FOS_NOREADONLYRETURN        = 0x00008000,
  FOS_NOTESTFILECREATE        = 0x00010000,
  FOS_HIDEMRUPLACES           = 0x00020000,
  FOS_HIDEPINNEDPLACES        = 0x00040000,
  FOS_NODEREFERENCELINKS      = 0x00100000,
  FOS_DONTADDTORECENT         = 0x02000000,
  FOS_FORCESHOWHIDDEN         = 0x10000000,
  FOS_DEFAULTNOMINIMODE       = 0x20000000,
  FOS_FORCEPREVIEWPANEON      = 0x40000000,
  FOS_SUPPORTSTREAMABLEITEMS  = 0x80000000
} ; typedef DWORD FILEOPENDIALOGOPTIONS;
*/

//---------------------------------------------------------------------------
class TOFMSDlgForm : public TForm
{
__published:	// IDE-managed Components
  void __fastcall FormDestroy(TObject *Sender);
  void __fastcall FormActivate(TObject *Sender);

private:
  WideChar* __fastcall SetFilter(void);
  wchar_t* __fastcall GetFilter(wchar_t* pFilterBuf, int iFilter, int iMax);
  int __fastcall FindFilter(wchar_t* pFilterBuf, wchar_t* pFilterToFind, int iMax);
  bool __fastcall ResizeCustomControl(HWND hDlg, RECT rect);
  bool __fastcall ResizeCustomControl(HWND hDlg);
  bool __fastcall PositionButton(HWND hDlg, int top, int left, int right);
  bool __fastcall LoadFontFrom(HWND hDlgDest, HWND hDlgSrc);
  WideString __fastcall GetShortcutTarget(WideString file);
  bool __fastcall GetShortcut(WideString &sPath, bool &bIsDirectory);
  bool __fastcall AddWideItem(WideString sPath, bool bIsDirectory);
  bool __fastcall GetSelectedItems(void);
  WideString __fastcall GetTextFromCommonDialog(HWND hWnd, UINT msg);
  bool __fastcall InitDialog(HWND hDlg);
  WideString __fastcall GetNextFileName(void);
  bool __fastcall DeleteFileNameObjects();
  int __fastcall ProcessNotifyMessage(HWND hDlg, LPOFNOTIFY p_notify);
  bool __fastcall MyDefSubclassProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
  bool __fastcall MyRemoveWindowSubclass(HWND hWnd, SUBCLASSPROC pfnSubclass, int uIdSubclass);
  bool __fastcall MySetWindowSubclass(HWND hWnd, SUBCLASSPROC pfnSubclass, int uIdSubclass);

  WideChar* p_szFileName;
  WideChar* p_szTitleName;

  OPENFILENAMEW m_ofn;
  HWND m_hListView;
  HWND m_hFileNameCombo;
  HWND m_hNewWindow;
  HWND m_hCustomOpenButton;

  bool FFolderIsSelected, FSingleSelect;

  int FFilterCount, FFilterIndex;

  static const WideChar szUntitled[10];

  WideString FCurrentFolder, FCurrentFilter;
  WideString FInitialDir, FDlgTitle;

  WideString FFilters, FFileNameLabel;
  WideChar* p_filterBuf;

  TList* p_fno;

  UINT FResult;

  HWND FDlgHandle;

protected:
  WideString __fastcall GetFileName(void);
  String __fastcall GetFileNameUtf8(void);
  String __fastcall GetTitleUtf8(void);
  TList* __fastcall GetFileNameObjects(void);

  static UINT CALLBACK OFNHookProc(HWND hDlg, UINT msg, WPARAM wParam,
                                                              LPARAM lParam);

  static LRESULT CALLBACK OpenFileSubClass(HWND hDlg, UINT uMsg, WPARAM wParam,
                     LPARAM lParam, UINT* uIdSubclass, DWORD* dwRefData);

  static LRESULT CALLBACK DefViewSubClass(HWND hDlg, UINT uMsg, WPARAM wParam,
                           LPARAM lParam, UINT* uIdSubclass, DWORD* dwRefData);

public:		// User declarations
  __fastcall TOFMSDlgForm(TComponent* Owner);
  virtual __fastcall ~TOFMSDlgForm();

  // Call this show the dialog.
  bool __fastcall ExecuteW(int iFilter, WideString wInitialDir,
                                                          String sDlgTitle);
  bool __fastcall ExecuteU(String uFilter, String uInitialDir,
                                                            String uDlgTitle);
  bool __fastcall ExecuteU(int iFilter, String uInitialDir,
                                                           String uDlgTitle);

  // You can set these three before calling Execute
  __property WideString Filters = {read = FFilters, write = FFilters};
  __property WideString FileNameLabel = {read = FFileNameLabel,
                                                write = FFileNameLabel};
	__property bool SingleSelect = {read = FSingleSelect, write = FSingleSelect};

	__property bool FolderIsSelected = {read = FFolderIsSelected};

  __property TList* FileNameObjects = {read = GetFileNameObjects};

  __property HWND DlgHandle = {read = FDlgHandle};
  __property UINT Result = {read = FResult};
	__property int FilterCount = {read = FFilterCount};
	__property int FilterIndex = {read = FFilterIndex}; // 1-based index
  __property String TitleUtf8 = {read = GetTitleUtf8 };
  __property String FileNameUtf8 = {read = GetFileNameUtf8};
  __property WideString FileName = {read = GetFileName};
  __property WideString InitialDir = {read = FInitialDir};
  __property WideString DlgTitle = {read = FDlgTitle};
  __property WideString CurrentFolder = {read = FCurrentFolder};
  __property WideString CurrentFilter = {read = FCurrentFilter};
};
//---------------------------------------------------------------------------
extern PACKAGE TOFMSDlgForm *OFMSDlgForm;
//---------------------------------------------------------------------------
#endif

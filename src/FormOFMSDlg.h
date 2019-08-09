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
#include <dlgs.h>
//#include <vcl\olectnrs.hpp>

// For SwiftMiX use:
#define OFDbg MainForm
#define OFUtil MainForm
// For YahCoLoRiZe use:
//#define OFDbg DTSColor
//#define OFUtil utils

#define WM_ITEMS_SELECTED (WM_USER+1)

// without a MINWIDTH, the custom buttons sometimes won't show!
#define MINWIDTH 192 // it's actually 188 but add a few pixels to be safe
//---------------------------------------------------------------------------
class TWideItem
{
public:
  String s;
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
// (dlgs.h)
//#define stc2 0x0441
//#define cmb1 0x0470
//#define stc3 0x0442
//#define edt1 0x0480
//#define cmb13 0x047c
//#define lst1 0x0460
//#define lst2 0x0461
//#define IDOK 1
//#define IDCANCEL 2
// Control aliases that actually make sense....
#define ID_FilterCombo cmb1
#define ID_FilterLabel stc2
#define ID_FileNameLabel stc3

// https://www.codeproject.com/questions/40907/cfiledialog-hidecontrol-broken
// Using Spy++ on a sample application I just built, it would seem that
// instead of edt1, you now want to use cmb13 (0x47c). I guess they decided
// that as that field is (now) actually a combo-box, that would be a better
// ID to use.
//#define ID_FileName edt1

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

// 6/6/2019 - problem with custom buttons not appearing when program is
// first run... until you stretch out the dialog width a few pixels.
// Trying a simple fix to decrease the cumulative width of the buttons and
// gaps.
//#define BUTTON_GAP 3
#define BUTTON_GAP 2

// this is the MAKEINTRESOURCEW macro with "::" before ULONG_PTR
// (I was having namespace conflicts...)
#define MYMIR(i) ((LPWSTR)((::ULONG_PTR)((WORD)(i))))

// name of property we set to pass parameters...
#define PROP_OFN L"OFMS"

typedef void __fastcall (__closure *TOFMSDlgFolderChangeEvent)(TObject* Sender, TWMNotify& Message);

//
// subclassing stuff
//
typedef LRESULT (CALLBACK* MYSUBCLASSPROC)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

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
  void __fastcall FormCreate(TObject *Sender);

private:
  bool __fastcall UriIsDirectory(String sUri);
  WideChar* __fastcall SetFilter(void);
  wchar_t* __fastcall GetFilter(wchar_t* pFilterBuf, int iFilter, int iMax);
  int __fastcall FindFilter(wchar_t* pFilterBuf, wchar_t* pFilterToFind, int iMax);
  bool __fastcall ResizeCustomControl(HWND hDlg, RECT rect);
  bool __fastcall ResizeCustomControl(HWND hDlg);
  bool __fastcall PositionButton(HWND hDlg, int top, int left, int right);
  bool __fastcall LoadFontFrom(HWND hDlgDest, HWND hDlgSrc);
  bool __fastcall AddWideItem(String sPath, bool bIsDirectory);
  bool __fastcall GetSelectedItems(bool bRaw=false);
  String __fastcall GetTextFromCommonDialog(HWND hWnd, UINT msg);
  bool __fastcall InitDialog(HWND hDlg);
  String __fastcall GetNextFileName(void);
  bool __fastcall DeleteFileNameObjects();
  int __fastcall ProcessNotifyMessage(HWND hDlg, LPOFNOTIFY p_notify);
//  bool __fastcall MyDefSubclassProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
  bool __fastcall MyRemoveWindowSubclass(HWND hWnd, MYSUBCLASSPROC pfnSubclass, int uIdSubclass);
  bool __fastcall MySetWindowSubclass(HWND hWnd, MYSUBCLASSPROC pfnSubclass, int uIdSubclass);

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

  String FCurrentFolder, FCurrentFilter;
  String FInitialDir, FDlgTitle;

  String FFilters, FFileNameLabel;
  WideChar* p_filterBuf;

  TList* p_fno;

  UINT FResult;

  HWND FDlgHandle;

protected:
  String __fastcall GetTitle(void);
  String __fastcall GetFileName(void);
  TList* __fastcall GetFileNameObjects(void);

  static UINT CALLBACK OFNHookProc(HWND hDlg, UINT msg, WPARAM wParam,
                                                              LPARAM lParam);

  static LRESULT CALLBACK OpenFileSubClass(HWND hDlg, UINT uMsg, WPARAM wParam,
                     LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

  static LRESULT CALLBACK DefViewSubClass(HWND hDlg, UINT uMsg, WPARAM wParam,
                           LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

public: // User declarations
//  __fastcall TOFMSDlgForm(TComponent* Owner);
//  virtual __fastcall ~TOFMSDlgForm();

  // Call this show the dialog.
  bool __fastcall Execute(String uFilter, String uInitialDir, String uDlgTitle);
  bool __fastcall Execute(int iFilter, String uInitialDir, String uDlgTitle);

  // You can set these three before calling Execute
  __property String Filters = {read = FFilters, write = FFilters};
  __property String FileNameLabel = {read = FFileNameLabel,
                                                write = FFileNameLabel};
  __property bool SingleSelect = {read = FSingleSelect, write = FSingleSelect};

  __property bool FolderIsSelected = {read = FFolderIsSelected};

  __property TList* FileNameObjects = {read = GetFileNameObjects};

  __property HWND DlgHandle = {read = FDlgHandle};
  __property UINT Result = {read = FResult};
  __property int FilterCount = {read = FFilterCount};
  __property int FilterIndex = {read = FFilterIndex}; // 1-based index
  __property String Title = {read = GetTitle};
  __property String FileName = {read = GetFileName};
  __property String InitialDir = {read = FInitialDir};
  __property String DlgTitle = {read = FDlgTitle};
  __property String CurrentFolder = {read = FCurrentFolder};
  __property String CurrentFilter = {read = FCurrentFilter};
};
//---------------------------------------------------------------------------
extern PACKAGE TOFMSDlgForm *OFMSDlgForm;
//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Registry.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)


#include "Main.h"
#pragma hdrstop

#include "OSFDlg.h"

#include "ShlObj.h"
#include "ObjIdl.h"
#include "Resource.h"

static LRESULT CALLBACK OpenFileSubClass(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT* uIdSubclass, DWORD* dwRefData);
static LRESULT CALLBACK DefViewSubClass(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT* uIdSubclass, DWORD* dwRefData);
static UINT CALLBACK OFNHookProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

typedef BOOL __stdcall (*tSetWindowSubclass)(HWND hDlg, SUBCLASSPROC pfnSubclass, UINT* uIdSubclass, DWORD* dwRefData);
typedef BOOL __stdcall (*tRemoveWindowSubclass)(HWND hDlg, SUBCLASSPROC pfnSubclass, UINT* uIdSubclass);
typedef BOOL __stdcall (*tDefSubclassProc)(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

tSetWindowSubclass pSetWindowSubclass;
tRemoveWindowSubclass pRemoveWindowSubclass;
tDefSubclassProc pDefSubclassProc;

UINT m_buttonWidth = 0;
UINT m_buttonHeight = 0;
HMODULE m_hComCtl32 = NULL;

//---------------------------------------------------------------------------
//  constructor - OPENFILENAMEW structure initialization
__fastcall TOSFDialog::TOSFDialog()
{
  m_hComCtl32 = LoadLibrary("Comctl32.dll");

  p_szFileName = new WideChar[BUFSIZE];
  p_szTitleName = new WideChar[BUFSIZE];
  p_fno = new TList();

  if (m_hComCtl32 == NULL || p_szFileName == NULL || p_szTitleName == NULL || p_fno == NULL)
  {
#if DEBUG_ON
    MainForm->CWrite( "\r\nUnable to load Comctl32.dll!\r\n");
#endif
    return;
  }

  pSetWindowSubclass = (tSetWindowSubclass)GetProcAddress(m_hComCtl32, "SetWindowSubclass");
  pRemoveWindowSubclass = (tRemoveWindowSubclass)GetProcAddress(m_hComCtl32, "RemoveWindowSubclass");
  pDefSubclassProc = (tDefSubclassProc)GetProcAddress(m_hComCtl32, "DefSubclassProc");

  m_ofn.lStructSize       = sizeof(OPENFILENAMEW);
  m_ofn.hwndOwner         = NULL;
  m_ofn.hInstance         = (void*)GetWindowLong(Application->Handle, GWL_HINSTANCE);
  m_ofn.lpstrFilter       = NULL;            // Set in Open and Save functions
  m_ofn.lpstrCustomFilter = NULL;
  m_ofn.nMaxCustFilter    = 0;
  m_ofn.nFilterIndex      = 4; // *.*
  m_ofn.lpstrFile         = p_szFileName;   // Set to member variable address
  m_ofn.nMaxFile          = BUFSIZE;
  m_ofn.lpstrFileTitle    = p_szTitleName;   // Set to member variable address
  m_ofn.nMaxFileTitle     = BUFSIZE;
  m_ofn.lpstrInitialDir   = NULL;
  m_ofn.lpstrTitle        = NULL;           // Set in Open and Save functions
  m_ofn.Flags             = NULL;
  m_ofn.nFileOffset       = 0;
  m_ofn.nFileExtension    = 0;
  m_ofn.lpstrDefExt       = NULL;          // Set in Open and Save functions
  m_ofn.lCustData         = (unsigned long)this;          // lets us access our class data
  m_ofn.lpfnHook          = OFNHookProc;
  m_ofn.lpTemplateName    = MAKEINTRESOURCEW(IDD_CustomOpenDialog);

  FFilter = "";
  p_filterBuf = NULL;
  m_hListViewControl = NULL;
  m_hFileNameCombo = NULL;
  m_result = IDCANCEL;

  m_folderIsSelected = false;
}
//------------------- destructor -------------------------
__fastcall TOSFDialog::~TOSFDialog()
{
  try { if (p_filterBuf != NULL) delete [] p_filterBuf; } catch(...) {}
  try { if (p_szTitleName != NULL) delete [] p_szTitleName; } catch(...) {}
  try { if (p_szFileName != NULL) delete [] p_szFileName; } catch(...) {}

  try
  {
    if (p_fno != NULL)
    {
      DeleteFileNameObjects();
      delete p_fno;
    }
  }
  catch(...) {}

  try { if (m_hComCtl32 != NULL) FreeLibrary(m_hComCtl32); } catch(...) {}
}
//------------ FileOpenDlg function ----------------------
bool __fastcall TOSFDialog::FileOpenDlg(String sDefExt, String sDlgTitle, String sInitialDir, bool bMultSel)
// Strings are passed in as UTF-8 and are converted to WideString
{
  p_szFileName[0] = p_szTitleName[0] = (WideChar)'\0';

  WideString wDefExt = MainForm->Utf8ToWide(sDefExt);
  WideString wInitialDir = MainForm->Utf8ToWide(sInitialDir);
  WideString wDlgTitle = MainForm->Utf8ToWide(sDlgTitle);

  m_ofn.lpstrFilter = this->SetFilter();
  m_ofn.lpstrDefExt = wDefExt.c_bstr();
  m_ofn.lpstrTitle  = wDlgTitle.c_bstr();
  m_ofn.lpstrInitialDir = wInitialDir.c_bstr();

  m_ofn.Flags = OFN_ENABLEHOOK | OFN_ENABLESIZING |
          OFN_NOTESTFILECREATE | OFN_EXPLORER | OFN_ALLOWMULTISELECT |
          OFN_NODEREFERENCELINKS | OFN_ENABLETEMPLATE | OFN_HIDEREADONLY;
          //OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

  bool bRet = GetOpenFileNameW(&m_ofn);

  return bRet;
}

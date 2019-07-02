//---------------------------------------------------------------------------
#include <vcl.h>
#include "Main.h"
#pragma hdrstop

#include "FormDirDlg.h"
#include "ShlObj.h"
#include "ObjIdl.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
void __fastcall TDirDlgForm::FormCreate(TObject *Sender)
{
  FDlgHandle = NULL;
  FAutoScroll = false;
  OleInitialize(NULL); // This does CoInitialize(NULL) too and allows drag/drop
}
//---------------------------------------------------------------------------
void __fastcall TDirDlgForm::FormDestroy(TObject *Sender)
{
  OleUninitialize();
}
//---------------------------------------------------------------------------
String __fastcall TDirDlgForm::Execute(int nCSIDL)
{
  String sOut;
  wchar_t pszPath[MAX_PATH+1];
  pszPath[0] = L'\0';

  LPITEMIDLIST pidlRoot = NULL;

  BROWSEINFOW bi = {0};

  if(nCSIDL > 0)
  {
    SHGetSpecialFolderLocation(Application->Handle, nCSIDL, &pidlRoot);
//    SHGetFolderPathW(Application->Handle, pszPath, nCSIDL, FALSE);
  }
  else
    pidlRoot = NULL;

//  bi.hwndOwner = Application->Handle;
  bi.lParam = (unsigned)this; // pointer to us to pass to the callbacks
  bi.pidlRoot = pidlRoot;
  bi.lpfn = FAutoScroll ? BrowseCallbackProc : NULL;
  bi.lpszTitle = pszPath;
  bi.ulFlags = BIF_USENEWUI | BIF_SHAREABLE;
  LPITEMIDLIST stId = SHBrowseForFolderW(&bi);

  if (stId != 0)
  {
//    SHGetKnownFolderPath(FOLDERID_Music, 0, NULL, pszPath);

    if (SHGetPathFromIDListW(stId, pszPath))
      sOut = String(pszPath);

    CoTaskMemFree(stId);
  }

  return sOut;
}
//---------------------------------------------------------------------------
BOOL CALLBACK TDirDlgForm::FindTreeViewCallback(HWND hwnd, LPARAM lParam)
{
  wchar_t szClassName[MAX_PATH+1];
  szClassName[0] = L'\0';
  ::GetClassNameW(hwnd, szClassName, sizeof(szClassName));
  szClassName[MAX_PATH] = L'\0';
  //TDirDlgForm* pDirDlg = (TDirDlgForm*)lParam; // available for future use...

  if(lstrcmpiW(szClassName, L"SysTreeView32") == 0)
  {
    HWND* phWnd = (HWND*)lParam;

    if(phWnd)
      *phWnd = hwnd;

    return FALSE;
  }

  return TRUE;
}
//---------------------------------------------------------------------------
INT CALLBACK TDirDlgForm::BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM pData)
{
/*
  static BOOL bProcessEnsureVisible = FALSE;

  switch(uMsg)
  {
    case BFFM_INITIALIZED:
    {
      // save the parent handle for later use by the destructor to send a WM_CLOSE
      if (pData != NULL)
        ((TDirDlgForm*)pData)->DlgHandle = GetParent(hwnd);

      bProcessEnsureVisible = FALSE;

      wchar_t buff[MAX_PATH+1];
      buff[0] = L'\0';
      ::GetCurrentDirectoryW(MAX_PATH, buff);
      buff[MAX_PATH] = L'\0';

      //In case of Windows 7 and later
      //INFO: Correction for the Microsoft bug that doesn't
      //      scroll the tree-view to the selected item...
      OSVERSIONINFOW osi;
      osi.dwOSVersionInfoSize = sizeof(osi);
      if(GetVersionExW(&osi) && osi.dwMajorVersion >= 6 && osi.dwMinorVersion >= 1)
        bProcessEnsureVisible = TRUE;

      ::SendMessageW(hwnd, BFFM_SETSELECTIONW, TRUE, (LPARAM)buff);
    }
    break;

    case BFFM_SELCHANGED:
    {
      if(bProcessEnsureVisible)
      {
        bProcessEnsureVisible = FALSE;

        HWND hWndTree = NULL;
        EnumChildWindows(hwnd, (WNDENUMPROC)FindTreeViewCallback, (LPARAM)&hWndTree);

        if(hWndTree)
        {
          HTREEITEM hItm = TreeView_GetSelection(hWndTree);

          if(hItm != NULL)
            TreeView_EnsureVisible(hWndTree, hItm);
        }
      }
    }
    break;
  }
*/
  return 0;

}
//---------------------------------------------------------------------------
void __fastcall TDirDlgForm::FormActivate(TObject *Sender)
{
  if (this->FDlgHandle)
    BringWindowToTop(this->FDlgHandle);
}
//---------------------------------------------------------------------------


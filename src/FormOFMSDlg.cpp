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
// NOTE: The original file-open dialog's code for Borland C++ Builder is in the source-code files:
// OPENSAVE.CPP and DLGFILE.CPP
//
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms646839%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
// https://msdn.microsoft.com/en-us/library/windows/desktop/bb776913(v=vs.85).aspx#ok_button_options
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms644995%28v=vs.85%29.aspx
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms646960(v=vs.85).aspx#_win32_Explorer_Style_Custom_Templates
//
// Control Identifiers for the default Explorer-Style box
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
//
//DM_GETDEFID You can send this message to a dialog box. The dialog box returns the control identifier of the default push button, if the dialog box has one; otherwise, it returns zero.
//DM_REPOSITION You can send this message to a top-level dialog box. The dialog box repositions itself so it fits within the desktop area.
//DM_SETDEFID You can send this message to a dialog box. The dialog box sets the default push button to the control specified by the control identifier in the wParam parameter.
//WM_ACTIVATE Restores the input focus to the control identified by the previously saved handle if the dialog box is activated. Otherwise, the procedure saves the handle to the control having the input focus.
//WM_CHARTOITEM Returns zero.
//WM_CLOSE Posts the BN_CLICKED notification message to the dialog box, specifying IDCANCEL as the control identifier. If the dialog box has an IDCANCEL control identifier and the control is currently disabled, the procedure sounds a warning and does not post the message.
//WM_COMPAREITEM Returns zero.
//WM_ERASEBKGND Fills the dialog box client area by using either the brush returned from the WM_CTLCOLORDLG message or with the default window color.
//WM_GETFONT Returns a handle to the application-defined dialog box font.
//WM_INITDIALOG Returns zero.
//WM_LBUTTONDOWN Sends a CB_SHOWDROPDOWN message to the combo box having the input focus, directing the control to hide its drop-down list box. The procedure calls DefWindowProc to complete the default action.
//WM_NCDESTROY Releases global memory allocated for edit controls in the dialog box (applies to dialog boxes that specify the DS_LOCALEDIT style) and frees any application-defined font (applies to dialog boxes that specify the DS_SETFONT or DS_SHELLFONT style). The procedure calls the DefWindowProc function to complete the default action.
//WM_NCLBUTTONDOWN Sends a CB_SHOWDROPDOWN message to the combo box having the input focus, directing the control to hide its drop-down list box. The procedure calls DefWindowProc to complete the default action.
//WM_NEXTDLGCTL Sets the input focus to the next or previous control in the dialog box, to the control identified by the handle in the wParam parameter, or to the first control in the dialog box that is visible, not disabled, and has the WS_TABSTOP style. The procedure ignores this message if the current window with the input focus is not a control.
//WM_SETFOCUS Sets the input focus to the control identified by a previously saved control window handle. If no such handle exists, the procedure sets the input focus to the first control in the dialog box template that is visible, not disabled, and has the WS_TABSTOP style. If no such control exists, the procedure sets the input focus to the first control in the template.
//WM_SHOWWINDOW Saves a handle to the control having the input focus if the dialog box is being hidden, then calls DefWindowProc to complete the default action.
//WM_SYSCOMMAND Saves a handle to the control having the input focus if the dialog box is being minimized, then calls DefWindowProc to complete the default action.
//WM_VKEYTOITEM Returns zero.
#include <vcl.h>
#include "Main.h"
#pragma hdrstop

#include "FormOFMSDlg.h"

#include "ShlObj.h"
#include "ObjIdl.h"
#include "Resource.h"

typedef BOOL __stdcall (*tSetWindowSubclass)(HWND hDlg, MYSUBCLASSPROC pfnSubclass, LPUINT uIdSubclass, LPDWORD dwRefData);
typedef BOOL __stdcall (*tRemoveWindowSubclass)(HWND hDlg, MYSUBCLASSPROC pfnSubclass, LPUINT uIdSubclass);
typedef BOOL __stdcall (*tDefSubclassProc)(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

tSetWindowSubclass pSetWindowSubclass;
tRemoveWindowSubclass pRemoveWindowSubclass;
tDefSubclassProc pDefSubclassProc;

UINT m_buttonWidth = 0;
UINT m_buttonHeight = 0;
HMODULE m_hComCtl32 = NULL;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TOFMSDlgForm *OFMSDlgForm;
//---------------------------------------------------------------------------
__fastcall TOFMSDlgForm::TOFMSDlgForm(TComponent* Owner)
  : TForm(Owner)
{
  FDlgHandle = NULL;

  FSingleSelect = false;
  FFolderIsSelected = false;
  FDisplayFullPath = DEFAULT_FULL_FILEPATH;

  p_szFileName = new WideChar[OF_BUFSIZE];
  p_szTitleName = new WideChar[OF_BUFSIZE];

  m_hComCtl32 = LoadLibraryW(L"Comctl32.dll");
  p_fno = new TList();

  if (m_hComCtl32 == NULL || p_szFileName == NULL || p_szTitleName == NULL || p_fno == NULL)
  {
#if DEBUG_ON
    OFDbg->CWrite( "\r\nUnable to load Comctl32.dll!\r\n");
#endif
    return;
  }

  pSetWindowSubclass = (tSetWindowSubclass)GetProcAddress(m_hComCtl32, "SetWindowSubclass");
  pRemoveWindowSubclass = (tRemoveWindowSubclass)GetProcAddress(m_hComCtl32, "RemoveWindowSubclass");
#if IS_OLD_BORLAND_CPP_BUILDER
  pDefSubclassProc = (tDefSubclassProc)GetProcAddress(m_hComCtl32, "DefSubclassProc");
#else
  pDefSubclassProc = NULL; // for RAD studio just call DefSubclassProc() directly
#endif

  ZeroMemory(&m_ofn, sizeof(OPENFILENAMEW));

  m_ofn.lStructSize       = sizeof(OPENFILENAMEW);
#if IS_OLD_BORLAND_CPP_BUILDER
  m_ofn.hInstance         = (void*)GetWindowLong(Application->Handle, GWL_HINSTANCE);
#else
  m_ofn.hInstance         = (HINSTANCE__*)GetWindowLong(Application->Handle, GWL_HINSTANCE);
#endif
  m_ofn.lpstrFile         = p_szFileName;   // Set to member variable address
  m_ofn.nMaxFile          = OF_BUFSIZE;
  m_ofn.lpstrFileTitle    = p_szTitleName;   // Set to member variable address
  m_ofn.nMaxFileTitle     = OF_BUFSIZE;
  m_ofn.lCustData         = (unsigned long)this; // lets us access our class data
  m_ofn.lpfnHook          = OFNHookProc;
#if IS_OLD_BORLAND_CPP_BUILDER
  m_ofn.lpTemplateName    = MAKEINTRESOURCEW(IDD_CustomOpenDialog);
#else
  m_ofn.lpTemplateName    = MYMIR(IDD_CustomOpenDialog);
#endif

  p_filterBuf = NULL;

  FFilterCount = 0;
  FFilterIndex = 0;
  FResult = IDCANCEL;

  m_hListView = NULL;
  m_hChildWindow = NULL;
  m_hMyFileName = NULL;
}
//------------------- destructor -------------------------
__fastcall TOFMSDlgForm::~TOFMSDlgForm()
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
//---------------------------------------------------------------------------
void __fastcall TOFMSDlgForm::FormDestroy(TObject *Sender)
{
#if DEBUG_ON
  OFDbg->CWrite("\r\nFormDestroy() in TOFMSDlgForm()!\r\n");
#endif
}
//---------------------------------------------------------------------------
bool __fastcall TOFMSDlgForm::ExecuteU(String uFilter, String uInitialDir,
                                                            String uDlgTitle)
{
  m_ofn.lpstrFilter = this->SetFilter(); // sets FFilterCount property var!

  // Try to locate the 1-based filter-index of the extension on our sDefFile
  // in the null-separated list of filters in the lpstrFilter filters-string
  WideString wExt = WideString("*.") + OFUtil->Utf8ToWide(uFilter);
  int iFilter = FindFilter(this->p_filterBuf, wExt.c_bstr(), FFilterCount);
  return ExecuteU(iFilter, uInitialDir, uDlgTitle);
}

bool __fastcall TOFMSDlgForm::ExecuteU(int iFilter, String uInitialDir,
                                                            String uDlgTitle)
{
  return ExecuteW(iFilter, OFUtil->Utf8ToWide(uInitialDir), uDlgTitle);
}

bool __fastcall TOFMSDlgForm::ExecuteW(int iFilter, WideString wInitialDir,
                                                            String uDlgTitle)
// Strings are passed in as UTF-8 and are converted to WideString
{
  p_szFileName[0] = p_szTitleName[0] = (WideChar)'\0';

  FFilterIndex = iFilter;
  FInitialDir = wInitialDir;
  FDlgTitle = OFUtil->Utf8ToWide(uDlgTitle);

  m_ofn.lpstrFilter = this->SetFilter(); // sets FFilterCount property var!

  // set to the first filter if the extension passed in is not there
  // (You would usually set the first filter to *.*)
  if (FFilterIndex <= 0 && FFilterCount > 0)
    FFilterIndex = 1;

  FCurrentFilter = WideString(GetFilter((wchar_t*)m_ofn.lpstrFilter,
                                        FFilterIndex, FFilterCount));
  m_ofn.nFilterIndex = FFilterIndex;

  m_ofn.lpstrDefExt = FCurrentFilter.c_bstr();
  m_ofn.lpstrTitle  = FDlgTitle.c_bstr();
  m_ofn.lpstrInitialDir = FInitialDir.c_bstr();

  m_ofn.Flags = OFN_NOTESTFILECREATE|OFN_HIDEREADONLY|OFN_EXPLORER|
    OFN_ENABLEHOOK|OFN_ENABLESIZING|OFN_ENABLETEMPLATE;

  // Additional options for our custom multi-select version
  // (setting OFN_NODEREFERENCELINKS will allow .lnk files to be selected
  // and passed to the caller... clearing it lets you double-click a
  // shortcut and go to its folder...)
  if (!FSingleSelect)
    m_ofn.Flags |= OFN_ALLOWMULTISELECT|OFN_NODEREFERENCELINKS;

  //OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

  // If the user specifies a file name and clicks the OK button, the return
  // value is nonzero. The buffer pointed to by the lpstrFile member of the
  // OPENFILENAME structure contains the full path and file name specified
  // by the user.
  bool bRet = GetOpenFileNameW(&m_ofn); // Execute the system's built-in open dialog...

  // Override bRet if FSingleSelect
  if (FSingleSelect)
    bRet = (this->FResult == IDCANCEL) ? false : true;

  return bRet;
}
//---------------------------------------------------------------------------
WideChar* __fastcall TOFMSDlgForm::SetFilter(void)
{
  try
  {
    if (FFilters.IsEmpty())
      FFilters = "All files (*.*)|*.*";

    int lenStr = this->FFilters.Length();
    int lenBuf = (lenStr+2)*sizeof(WideChar); // two null-terminators at end!

    // Reallocate filter-string buffer if needed
    try
    {
      if (this->p_filterBuf != NULL)
        delete [] this->p_filterBuf;
    }
    catch(...) {}

    this->p_filterBuf = new WideChar[lenBuf];

    if (this->p_filterBuf == NULL)
      return NULL;

    WideChar c;

    int ii = 0;
    int count = 0;

    // Replace | with '\0' and pad out to end of buffer
    for (; ii < lenStr; ii++)
    {
      c = FFilters[ii+1];

      if (c == '|')
      {
        c = '\0';
        count++;
      }

      this->p_filterBuf[ii] = c;
    }

    this->p_filterBuf[ii] = '\0';

    // odd # of vertical-bar separators then add 1 to make number divisable by 2
    // and we need an extra null to make two at the end...
    if ((count & 1) != 0)
    {
      count++;
      this->p_filterBuf[ii+1] = '\0';
    }

    count /= 2; // two vertical bars for each filter

    FFilterCount = count; // set internal property var

    return this->p_filterBuf;
  }
  catch(...) { return NULL; }
}
//---------------------------------------------------------------------------
wchar_t* __fastcall TOFMSDlgForm::GetFilter(wchar_t* pFilterBuf, int iFilter, int iMax)
// given a 1-based index, returns a pointer to the filter-string. iMax is the
// number of filters in the string.
{
  if (pFilterBuf == NULL || pFilterBuf[0] == '\0')
    return pFilterBuf;

  wchar_t* p_start;
  int idx = 0;

  // limit check
  if (iFilter < 1 || iFilter > iMax)
    iFilter = iMax;

  do
  {
    // first part of the filter (used to display in the box)
    while (*pFilterBuf != '\0')
      pFilterBuf++;

    pFilterBuf++;

    p_start = pFilterBuf; // start on the part we want!

    // second part of the filter (the actual ".wma" filter)
    while (*pFilterBuf != '\0')
      pFilterBuf++;

    idx++;
    pFilterBuf++;

  } while (idx < iFilter && idx < iMax);

  return p_start;
}
//---------------------------------------------------------------------------
int __fastcall TOFMSDlgForm::FindFilter(wchar_t* pFilterBuf, wchar_t* pFilterToFind, int iMax)
// given a pointer to a filter like "*.wpl", returns its 0-based index in pFilterBuf
// to use as nFilterIndex in the OPENFILENAMEW struct on initialization. iMax
// should be set to this->FilterCount.
//
// Return -1 if not found
{
  int idx = 0;
  bool bEqual;
  int ii;

  do
  {
    // first part of the filter (used to display in the box)
    while (*pFilterBuf != '\0')
      pFilterBuf++;

    pFilterBuf++;

    bEqual = true;
    ii = 0;

    // second part of the filter (the actual ".wma" filter)
    for(;;)
    {
      if (*pFilterBuf == '\0')
        break;

      if (*pFilterBuf++ != pFilterToFind[ii++])
        if (bEqual)
          bEqual = false;
    }

    idx++;

    // bEqual still set? We found our filter!
    if (bEqual)
      return idx;

    pFilterBuf++;

  } while (idx < iMax);

  return 0;
}
/*---------------------------------------------------------------------------
        GetNextFileName - function that returns results of
              multiple selection one by one
  ---------------------------------------------------------------------------*/
// If the user selects more than one file, the lpstrFile buffer returns the path to the current directory
// followed by the file names of the selected files. The nFileOffset member is the offset, in bytes or characters,
// to the first file name, and the nFileExtension member is not used. For Explorer-style dialog boxes, the directory
// and file name strings are NULL separated, with an extra NULL character after the last file name.
/*
WideString __fastcall TOFMSDlgForm::GetNextFileName(void)
{
  static WideChar szResult[MAX_PATH];
  static WideChar szDirectory[MAX_PATH];
  static int iPosition = 0;
  static BOOL bFirstTime = TRUE;
  static BOOL bLastName  = FALSE;
  WideChar szFileName[MAX_PATH];

  if(bFirstTime)
  {
    iPosition = 0;
    for(unsigned short i = 0; i < m_ofn.nFileOffset; i++, iPosition++)
    {
      if((szDirectory[i] = p_szFileName[iPosition]) == WideChar('\0'))
      {
        if(szDirectory[iPosition - 1] != WideChar('\\'))
          szDirectory[iPosition] = WideChar('\\');
        break;
      }
    }
  }
  if(bLastName)
  {
    bLastName = FALSE;
    bFirstTime = TRUE;
    return "";
  }
  if(p_szFileName[iPosition] == WideChar('\0'))
    iPosition++;

  szFileName[0] = WideChar('\0');

  for(unsigned int j = 0; j < sizeof(szFileName)/sizeof(szFileName[0]); j++, iPosition++)
  {
    if((szFileName[j] = p_szFileName[iPosition]) == WideChar('\0'))
    {
      if(p_szFileName[iPosition + 1] == WideChar('\0'))
        bLastName = TRUE;

      //-----------------making full file path -------------
      unsigned short k = 0;
      for(;k < m_ofn.nFileOffset; k++)
      {
        if((szResult[k] = szDirectory[k]) == WideChar('\0'))
          break;
      }
      for(unsigned int u = 0; u < sizeof(szFileName) / sizeof(szFileName[0]); u++, k++)
      {
        if((szResult[k] = szFileName[u]) == WideChar('\0'))
        {
          bFirstTime = FALSE;
          return WideString(szResult);
        }
      }
    }
  }
  return "";
}
*/
//---------------------------------------------------------------------------
bool __fastcall TOFMSDlgForm::GetMultiSelect(void)
{
  return m_ofn.Flags & OFN_ALLOWMULTISELECT;
}
//---------------------------------------------------------------------------
// Property getter for this->FileNameUtf8
String __fastcall TOFMSDlgForm::GetFileNameUtf8(void)
{
  return OFUtil->WideToUtf8Ansi(this->FileName);
}
//---------------------------------------------------------------------------
// Property getter for this->FileName
WideString __fastcall TOFMSDlgForm::GetFileName(void)
{
  return WideString(p_szFileName);
}
//---------------------------------------------------------------------------
// Property getter
String __fastcall TOFMSDlgForm::GetTitleUtf8(void)
{
  return OFUtil->WideToUtf8Ansi(WideString(p_szTitleName));
}
//---------------------------------------------------------------------------
TList* __fastcall TOFMSDlgForm::GetFileNameObjects(void)
{
  return this->p_fno;
}
//---------------------------------------------------------------------------
bool __fastcall TOFMSDlgForm::InitDialog(HWND hDlg)
{
  if (hDlg == NULL) return false;

  try
  {
    HWND hParent = GetParent(hDlg);

    if (!this->MySetWindowSubclass(hParent, OpenFileSubClass, ID_FILE_SUBCLASS_PROC))
    {
#if DEBUG_ON
      OFDbg->CWrite( "\r\nSubclassing file-dialog failed!\r\n");
#endif
    }

    //update the file name label
    HWND hFileNameLabel = GetDlgItem(hParent, ID_FileNameLabel);

    if (!this->FileNameLabel.IsEmpty())
      SendMessageW(hFileNameLabel, WM_SETTEXT, 0, (LPARAM)this->FileNameLabel.c_bstr());

    //find the button controls in the parent
    HWND hCancelButton = GetDlgItem(hParent, IDCANCEL);
    HWND hOkButton = GetDlgItem(hParent, IDOK);

    //We don't want the accelerator keys for the ok and cancel buttons to work, because
    //they are not shown on the dialog. However, we still want the buttons enabled
    //so that "esc" and "enter" have the behavior they used to. So, we just
    //clear out their text instead.
    SetWindowTextW(hOkButton, L"");
    SetWindowTextW(hCancelButton, L"");

    //find our button controls
    HWND hCustomSelectButton = GetDlgItem(hDlg, ID_CUSTOM_SELECT);
    HWND hCustomCancelButton = GetDlgItem(hDlg, ID_CUSTOM_CANCEL);
    this->m_hCustomOpenButton = GetDlgItem(hDlg, ID_CUSTOM_OPEN);

    // disable the select button for single-select mode
    if (FSingleSelect)
    {
      ShowWindow(hCustomSelectButton, SW_HIDE);
//      LONG style = GetWindowLong(hCustomSelectButton, GWL_STYLE);
//      style &= ~WS_VISIBLE; // Clear the visible flag
//      style |= WS_DISABLED; // set the disabled flag
//      SetWindowLong(hCustomSelectButton, GWL_STYLE, style);
#if DEBUG_ON
      OFDbg->CWrite( "\r\nHave single-select\r\n");
#endif
    }
    else
      //copy the font from the parent's button
      LoadFontFrom(hCustomSelectButton, hOkButton);

    //copy the font from the parent's button
    LoadFontFrom(this->m_hCustomOpenButton, hOkButton);
    LoadFontFrom(hCustomCancelButton, hCancelButton);

    //hide the ok and cancel buttons
    SendMessageW(hParent, CDM_HIDECONTROL, IDOK, 0);
    SendMessageW(hParent, CDM_HIDECONTROL, IDCANCEL, 0);

    // Get the old dialog's cancel button location
    WINDOWPLACEMENT cancelLoc;
    cancelLoc.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(hCancelButton, &cancelLoc);

    // We will be setting all three of our "overlayed" buttons to the same
    // dimensions as the original Cancel button
    m_buttonWidth = cancelLoc.rcNormalPosition.right - cancelLoc.rcNormalPosition.left;
    m_buttonHeight = cancelLoc.rcNormalPosition.bottom - cancelLoc.rcNormalPosition.top;

    // Subclassing makes room for new buttons at the bottom but we don't need
    // that because we are overlaying the new buttons over the old - exactly where they
    // are... so subtract m_buttonHeight back off
    WINDOWPLACEMENT wndLoc;
    wndLoc.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(hDlg, &wndLoc);
    wndLoc.rcNormalPosition.bottom -= m_buttonHeight;
    SetWindowPlacement(hDlg, &wndLoc);

    //ResizeCustomControl(hDlg, fileNameLoc.Right, hCustomCancelButton, hCustomSelectButton);
    this->ResizeCustomControl(hDlg);

    return true;
  }
  catch(...) { return false; }
}
//---------------------------------------------------------------------------
bool __fastcall TOFMSDlgForm::ResizeCustomControl(HWND hDlg)
{
  if (hDlg == NULL) return FALSE;

  try
  {
    HWND hParent = GetParent(hDlg);

    //find the button controls in the parent
    HWND hCancelButton = GetDlgItem(hParent, IDCANCEL);

    WINDOWPLACEMENT cancelLoc;
    cancelLoc.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(hCancelButton, &cancelLoc);

    // clipping region encompassing only the new buttons (to fix bug when
    // running on XP)
    RECT rect;
    rect.top = cancelLoc.rcNormalPosition.top;
    rect.bottom = cancelLoc.rcNormalPosition.bottom;
    rect.right = cancelLoc.rcNormalPosition.right;

    if (FSingleSelect)
      rect.left = rect.right - 2*m_buttonWidth - 1*BUTTON_GAP;
    else
      rect.left = rect.right - 3*m_buttonWidth - 2*BUTTON_GAP;

    // extend the filename edit box to the rightmost edge of the rightmost
    // button (Cancel)
    HWND hFileName = GetDlgItem(hParent, ID_MyFileName);
    WINDOWPLACEMENT fileNameLoc;
    fileNameLoc.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(hFileName, &fileNameLoc);
    fileNameLoc.rcNormalPosition.right = cancelLoc.rcNormalPosition.right;
    SetWindowPlacement(hFileName, &fileNameLoc);

    // position the rightmost edge of the filter box "two button-gaps"
    // left of the leftmost edge of the leftmost button
    HWND hFilterCombo = GetDlgItem(hParent, ID_FilterCombo);
    WINDOWPLACEMENT filterComboLoc;
    filterComboLoc.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(hFilterCombo, &filterComboLoc);
    filterComboLoc.rcNormalPosition.right = rect.left - (2*BUTTON_GAP);
    SetWindowPlacement(hFilterCombo, &filterComboLoc);

    // rect is a rectangle around the (either two or three) buttons
    return ResizeCustomControl(hDlg, rect);
  }
  catch (...)
  {
#if DEBUG_ON
    OFDbg->CWrite( "\r\nException in ResizeCustomControl() A\r\n");
#endif
  }

  return FALSE;
}
//---------------------------------------------------------------------------
bool __fastcall TOFMSDlgForm::ResizeCustomControl(HWND hDlg, RECT rect)
{
  try
  {
    HWND hCustomSelectButton = GetDlgItem(hDlg, ID_CUSTOM_SELECT);
    HWND hCustomOpenButton = GetDlgItem(hDlg, ID_CUSTOM_OPEN);
    HWND hCustomCancelButton = GetDlgItem(hDlg, ID_CUSTOM_CANCEL);

    if (hCustomCancelButton == NULL || hCustomOpenButton == NULL ||
                                            hCustomSelectButton == NULL)
      return FALSE;

    int r = rect.right;
    int l = rect.right - m_buttonWidth;
    PositionButton(hCustomCancelButton, rect.top, l, r);
    r -= m_buttonWidth + BUTTON_GAP;
    l -= m_buttonWidth + BUTTON_GAP;
    PositionButton(hCustomOpenButton, rect.top, l, r);

    // Only need a select button for the multi-select dialog
    if (!FSingleSelect)
    {
      r -= m_buttonWidth + BUTTON_GAP;
      l -= m_buttonWidth + BUTTON_GAP;
      PositionButton(hCustomSelectButton, rect.top, l, r);
    }

    //see bug # 844
    //We clip hDlg to only draw in the rectangle around our custom buttons.
    //When we supply a custom dialog template to GetOpenFileName(), it adds
    //an extra HWND to the open file dialog, and then sticks all the controls
    //in the dialog //template inside the HWND. It then resizes the control
    //to stretch from the top of the open file dialog to the bottom of the
    //window, extending the bottom of the window large enough to include the
    //additional height of the dialog template. This ends up sticking our custom
    //buttons at the bottom of the window, which is what we want.
    //
    //However, the fact that the parent window extends from the top of the open
    //file dialog was causing some painting problems on Windows XP SP 3 systems.
    //Basically, because the window was covering the predefined controls on the
    //open file dialog, they were not getting painted. This results in a blank
    //window. I tried setting an extended WS_EX_TRANSPARENT style on the dialog,
    //but that didn't help.
    //
    //So, to fix the problem I setup a window region for the synthetic HWND.
    //This clips the drawing of the window to only within the region containing
    //the custom buttons, and thus avoids the problem.
    //
    //I'm not sure why this wasn't an issue on Vista.

    HRGN hRgn = NULL;

    try
    {
      hRgn = CreateRectRgnIndirect(&rect); // (Uses "logical units")
      if (hRgn != NULL)
        SetWindowRgn(hDlg, hRgn, TRUE);
    }
    __finally
    {
      if (hRgn != NULL) try { DeleteObject(hRgn); } catch(...) {}
    }

    // 8/23/2019 - fix problem with buttons initially not appearing
    WINDOWPLACEMENT wndLoc;
    wndLoc.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(hDlg, &wndLoc);
    wndLoc.rcNormalPosition.right = rect.right + 4;
    SetWindowPlacement(hDlg, &wndLoc);

    return TRUE;
  }
  catch (...)
  {
#if DEBUG_ON
    OFDbg->CWrite( "\r\nException in ResizeCustomControl() B\r\n");
#endif
    return FALSE;
  }
}
//---------------------------------------------------------------------------
bool __fastcall TOFMSDlgForm::PositionButton(HWND hDlg, int top, int left, int right)
{
  if (hDlg == NULL) return FALSE;

  try
  {
    //int id = GetDlgCtrlID(hDlg);

    //BringWindowToTop(hDlg);

    WINDOWPLACEMENT buttonLoc;
    buttonLoc.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(hDlg, &buttonLoc);

    buttonLoc.rcNormalPosition.left = left;
    buttonLoc.rcNormalPosition.right = right;
    buttonLoc.rcNormalPosition.top = top;
    buttonLoc.rcNormalPosition.bottom = top + m_buttonHeight;

    SetWindowPlacement(hDlg, &buttonLoc);
    InvalidateRect(hDlg, NULL, TRUE);

    return TRUE;
  }
  catch (...)
  {
#if DEBUG_ON
    OFDbg->CWrite("\r\nException in PositionButton()\r\n");
#endif
  }

  return FALSE;
}
//---------------------------------------------------------------------------
bool __fastcall TOFMSDlgForm::GetSelectedItems(void)
{
  HWND hListView = this->m_hListView;

  if (this->FileNameObjects == NULL || hListView == NULL) return false;

  bool bRet = false;

  try
  {
      WideChar* nativeBuffer = NULL;

      try
      {
        nativeBuffer = new WideChar[OF_BUFSIZE];

        // delete all created TWideItem objects
        this->DeleteFileNameObjects();

        if (nativeBuffer != NULL)
        {
          LVITEMW lvitem;
          lvitem.mask = LVIF_TEXT;
          lvitem.iSubItem = 0; // get the item-text not sub-item
          lvitem.pszText = nativeBuffer;
          lvitem.cchTextMax = OF_BUFSIZE;

          WideString wName;
          WideString wCurrDir = this->CurrentFolder;

//ShowMessage("A:" + FCurrentFolder);
//ShowMessage("B:" + currDir);

          // Get the first selected item
          int iFirst = SendMessageW(hListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
          int iPos = iFirst;
#if DEBUG_ON
          OFDbg->CWrite("\r\niPos (first): " + String(iPos) + "\r\n");
#endif

          while (iPos != -1)
          {
            int length = SendMessageW(hListView, LVM_GETITEMTEXTW, iPos, (LPARAM)&lvitem);
            wName = WideString(lvitem.pszText, length);

#if DEBUG_ON
            OFDbg->CWrite("\r\nLength: " + String(length) +
                  ", iPos: " + String(iPos) + " Name: " + wName + "\r\n");
#endif

            if (!wName.IsEmpty())
            {
              int len = wCurrDir.Length();

              WideString wPath;

              if (len > 0)
              {
                WideChar c = wCurrDir[len];

                if (c != WideChar('\\'))
                  wPath = wCurrDir + WideString("\\") + wName;
                else
                  wPath = wCurrDir + wName;
              }
              else
                wPath = wName;

              bool bIsDirectory = false;

              // wPath, bIsDirectory are by reference...
              if (GetShortcut(wPath, bIsDirectory))
                this->AddWideItem(wPath, bIsDirectory);
            }
#if DEBUG_ON
            else
              OFDbg->CWrite("\r\nwName is empty for: " + String(iPos) +"\r\n");
#endif

            // Get the next selected item
            iPos = SendMessageW(hListView, LVM_GETNEXTITEM, iPos, LVNI_SELECTED);

            if (iPos == iFirst)
              break;
          }
        }
#if DEBUG_ON
        else
        {
          if (nativeBuffer == NULL)
            OFDbg->CWrite("\r\nnativeBuffer == NULL in GetSelectedItems()!\r\n");
          else
            OFDbg->CWrite("\r\nhListView == NULL in GetSelectedItems()!\r\n");
        }
#endif

        bRet = true;
      }
      __finally
      {
        try { if (nativeBuffer != NULL) delete [] nativeBuffer; }
        catch(...)
        {
#if DEBUG_ON
          OFDbg->CWrite("\r\nException thrown ideletein nativeBuffer in GetSelectedItems()!\r\n");
#endif
        };
      }
  }
  catch(...)
  {
#if DEBUG_ON
    OFDbg->CWrite( "\r\nException thrown in GetSelectedItems()!\r\n");
#endif
  }

  return bRet;
}
//---------------------------------------------------------------------------
/*
 * returns true if there is a valid shortcut (that we had permission to open)
 */
bool __fastcall TOFMSDlgForm::GetShortcut(WideString &wPath, bool &bIsDirectory)
{
  bIsDirectory = false;

  if (OFUtil->IsUriW(wPath)) return true; // just

  try
  {
#if DEBUG_ON
    OFDbg->CWrite("\r\nGetShortcut() wPath: " + wPath +"\r\n");
#endif

    // Do this first because we might have a .lnk file with no extension in our list-view control.
    if (OFUtil->DirectoryExistsW(wPath))
    {
      bIsDirectory = true;
      return true;
    }

    if (OFUtil->FileExistsW(wPath))
    {
      if (ExtractFileExt(wPath).LowerCase() == ".lnk")
      {
        wPath = GetShortcutTarget(wPath);

        if (OFUtil->FileExistsW(wPath))
          return true;

        if (OFUtil->DirectoryExistsW(wPath))
        {
          bIsDirectory = true;
          return true;
        }
      }
      else
        return true;
    }
    else
    {
      wPath += ".lnk"; // case where the .lnk extension is not displayed in the list-view control

#if DEBUG_ON
      OFDbg->CWrite("\r\nCheck file: " + wPath + "\r\n");
#endif
      if (OFUtil->FileExistsW(wPath))
      {
        wPath = GetShortcutTarget(wPath);
#if DEBUG_ON
        OFDbg->CWrite("\r\nShortcut target: " + wPath + "\r\n");
#endif

        if (OFUtil->FileExistsW(wPath))
        {
#if DEBUG_ON
          OFDbg->CWrite("\r\nShortcut exists: " + wPath + "\r\n");
#endif
          return true;
        }

        if (OFUtil->DirectoryExistsW(wPath))
        {
#if DEBUG_ON
          OFDbg->CWrite("\r\nDirectory exists: " + wPath + "\r\n");
#endif
          bIsDirectory = true;
          return true;
        }
      }
    }
  }
  catch (...)
  {
#if DEBUG_ON
    OFDbg->CWrite( "\r\nException in GetShortcut()\r\n");
#endif
  }

  return false;
}
//---------------------------------------------------------------------------
WideString __fastcall TOFMSDlgForm::GetShortcutTarget(WideString wPath)
{
  if (ExtractFileExt(wPath).LowerCase() != ".lnk")
    return "";

  WideString wOut = "";

  CoInitialize(NULL);

  try
  {
    IShellLinkW* psl = NULL;
    IPersistFile* ppf = NULL;
    TWin32FindDataW* wfs = NULL;

    try
    {
      WideChar Info[MAX_PATH+1];

      CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void**)&psl);
      if (psl != NULL)
      {
        if (psl->QueryInterface(IID_IPersistFile, (void**)&ppf) == 0)
        {
          if (ppf != NULL)
          {
            ppf->Load(wPath.c_bstr(), STGM_READ);
            psl->GetPath(Info, MAX_PATH, wfs, SLGP_UNCPRIORITY);
            wOut = WideString(Info);
          }
        }
      }
    }
    __finally
    {
      if (ppf) ppf->Release();
      if (psl) psl->Release();
      CoUninitialize();
    }
  }
  catch(...) { }

  return wOut;
}
//---------------------------------------------------------------------------
WideString __fastcall TOFMSDlgForm::GetTextFromCommonDialog(HWND hDlg, UINT msg)
{
  try
  {
    WideChar* buf = NULL;
    WideString str;

    try
    {
      buf = new WideChar[OF_BUFSIZE];
      buf[0] = L'\0';

      // NOTE: you are "supposed" to be able to get the required buffer size
      // - I tried 0, -1 - null-pointer, a buffer of 40 bytes... function always
      // just returns "1"
//      DWORD ret = SendMessageW(hDlg, msg, 0, NULL);
//#if DEBUG_ON
//      OFDbg->CWrite("\r\nBufsize required: " + String(ret) + "\r\n");
//#endif

      SendMessageW(hDlg, msg, OF_BUFSIZE, (LPARAM)buf);

      str = WideString(buf);
    }
    __finally
    {
      if (buf != NULL)
        try { delete [] buf; } catch (...) {}
    }

    return str;
  }
  catch(...) { return ""; }
}
//---------------------------------------------------------------------------
bool __fastcall TOFMSDlgForm::LoadFontFrom(HWND hDlgDest, HWND hDlgSrc)
{
  if (hDlgDest == NULL || hDlgSrc == NULL) return FALSE;

  try
  {
    HFONT hFont = (HFONT)SendMessageW(hDlgSrc, WM_GETFONT, 0, 0);
    if (hFont == NULL) return FALSE;
    SendMessageW(hDlgDest, WM_SETFONT, (WPARAM)hFont, 0);
    return TRUE;
  }
  catch(...) { return FALSE; }
}
//---------------------------------------------------------------------------
bool __fastcall TOFMSDlgForm::AddWideItem(WideString wPath, bool bIsDirectory)
{
  try
  {
    TWideItem* pWI = new TWideItem();
    pWI->s = wPath;
    pWI->IsDirectory = bIsDirectory;
    p_fno->Add(pWI);
    this->FResult = IDOK;
    return true;
  }
  catch(...)
  {
#if DEBUG_ON
    OFDbg->CWrite("\r\nAddWideItem() failed!\r\n");
#endif
    return false;
  }
}
//---------------------------------------------------------------------------
bool __fastcall TOFMSDlgForm::DeleteFileNameObjects()
{
  if (p_fno == NULL) return false;

  try
  {
    for (int ii = 0; ii < p_fno->Count; ii++)
    {
      if (p_fno->Items[ii] != NULL)
        delete p_fno->Items[ii];
    }

    p_fno->Clear();
    return true;
  }
  catch(...)
  {
#if DEBUG_ON
    OFDbg->CWrite("\r\nDeleteObjectsSL() failed!\r\n");
#endif
    return false;
  }
}
//---------------------------------------------------------------------------
bool __fastcall TOFMSDlgForm::MySetWindowSubclass(HWND hDlg, MYSUBCLASSPROC pfnSubclass, int uIdSubclass)
{
  if (m_hComCtl32 == NULL) return false;

  BOOL bRet = false;

  try
  {
    if (pSetWindowSubclass != NULL)
      bRet = (*pSetWindowSubclass)(hDlg, pfnSubclass, (UINT*)uIdSubclass, (DWORD*)this);
  }
  catch(...) {}

  return bRet;
}
//---------------------------------------------------------------------------
bool __fastcall TOFMSDlgForm::MyRemoveWindowSubclass(HWND hDlg, MYSUBCLASSPROC pfnSubclass, int uIdSubclass)
{
  if (m_hComCtl32 == NULL) return false;

  bool bRet = false;

  try
  {
    if (pRemoveWindowSubclass != NULL)
    {
      unsigned id = (unsigned)uIdSubclass;
      bRet = (*pRemoveWindowSubclass)(hDlg, pfnSubclass, &id);
    }
  }
  catch(...) {}

  return bRet;
}
//---------------------------------------------------------------------------
bool __fastcall TOFMSDlgForm::MyDefSubclassProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  if (m_hComCtl32 == NULL) return false;

  bool bRet = false;

  try
  {
    if (pDefSubclassProc != NULL)
      bRet = (*pDefSubclassProc)(hDlg, msg, wParam, lParam);
#if DEBUG_ON
    else
      OFDbg->CWrite("\r\nNULL MyDef Subclsaa!!!!!\r\n");
#endif
  }
  catch(...) {}

  return bRet;
}
//---------------------------------------------------------------------------
LRESULT CALLBACK TOFMSDlgForm::OpenFileSubClass(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam, LPUINT uIdSubclass, LPDWORD dwRefData)
{
  // WM_PARENTNOTIFY When the child window is being created, the system sends WM_PARENTNOTIFY just before the CreateWindow
  // or CreateWindowEx function that creates the window returns. When the child window is being destroyed, the system sends
  // the message before any processing to destroy the window takes place.
  //
  // LOWORD(wParam) == WM_CREATE
  // The child window is being created. HIWORD(wParam) is the identifier of the child window. lParam is a handle to the child window.
  TOFMSDlgForm* p = (TOFMSDlgForm*)dwRefData;
  if (p == NULL) return NULL;

  switch (uMsg)
  {
    case WM_ITEMS_SELECTED: // Custom message!
    {
      if (p->m_hListView == NULL) return NULL;

      // Get the selected-count
      int count = SendMessageW(p->m_hListView, LVM_GETSELECTEDCOUNT, 0, 0);
#if DEBUG_ON
      OFDbg->CWrite("\r\nWM_ITEMS_SELECTED, Count: " + String(count) + "\r\n");
#endif
      if (count)
      {
//          // Get the first selected item
//          int iFirst = SendMessageW(p->m_hListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
//          if (iFirst >= 0)
//          {
//            WideString wName = p->GetListViewItemText(p->m_hListView, iFirst);
//#if DEBUG_ON
//            // we get a hex 201 when we click in the file-edit combobox
//            OFDbg->CWrite("\r\nWwName: " + String(wName) + "\r\n");
//#endif
//
//            if (!wName.IsEmpty())
//            {
//              //  EM_CANUNDO
//              //  EM_CHARFROMPOS
//              //  EM_EMPTYUNDOBUFFER
//              //  EM_FMTLINES
//              //  EM_GETCUEBANNER
//              //  EM_GETFIRSTVISIBLELINE
//              //  EM_GETHANDLE
//              //  EM_GETHILITE
//              //  EM_GETIMESTATUS
//              //  EM_GETLIMITTEXT
//              //  EM_GETLINE
//              //  EM_GETLINECOUNT
//              //  EM_GETMARGINS
//              //  EM_GETMODIFY
//              //  EM_GETPASSWORDCHAR
//              //  EM_GETRECT
//              //  EM_GETSEL
//              //  EM_GETTHUMB
//              //  EM_GETWORDBREAKPROC
//              //  EM_HIDEBALLOONTIP
//              //  EM_LIMITTEXT
//              //  EM_LINEFROMCHAR
//              //  EM_LINEINDEX
//              //  EM_LINELENGTH
//              //  EM_LINESCROLL
//              //  EM_NOSETFOCUS
//              //  EM_POSFROMCHAR
//              //  EM_REPLACESEL
//              //  EM_SCROLL
//              //  EM_SCROLLCARET
//              //  EM_SETCUEBANNER
//              //  EM_SETHANDLE
//              //  EM_SETHILITE
//              //  EM_SETIMESTATUS
//              //  EM_SETLIMITTEXT
//              //  EM_SETMARGINS
//              //  EM_SETMODIFY
//              //  EM_SETPASSWORDCHAR
//              //  EM_SETREADONLY
//              //  EM_SETRECT
//              //  EM_SETRECTNP
//              //  EM_SETSEL
//              //  EM_SETTABSTOPS
//              //  EM_SETWORDBREAKPROC
//              //  EM_SHOWBALLOONTIP
//              //  EM_TAKEFOCUS
//              //  EM_UNDO
//              //  WM_UNDO
//              if (p->m_hMyFileName)
//                p->SetFileName(p->m_hMyFileName, wName);
//            }
//          }

        LONG style = GetWindowLongW(p->m_hCustomOpenButton, GWL_STYLE);
        bool bDisabled = (style & WS_DISABLED);

        if (count == 1)
        {
          // enable the open button
          if (bDisabled)
          {
            SetWindowLongW(p->m_hCustomOpenButton, GWL_STYLE, style & ~WS_DISABLED);
            InvalidateRect(p->m_hCustomOpenButton, NULL, TRUE);
          }
        }
        else
        {
          // disable the open button (setting this in the .rc file now!)
          if (!bDisabled)
          {
            SetWindowLongW(p->m_hCustomOpenButton, GWL_STYLE, style | WS_DISABLED);
            InvalidateRect(p->m_hCustomOpenButton, NULL, TRUE);
          }
        }
      }
    }
    break;

    case WM_PARENTNOTIFY:
    {
      if (LOWORD(wParam) == WM_CREATE)
      {
        // If the subclass has already been installed, the reference data is just updated...
        // NOTE: ListView's handle changes when you open a folder!
        int id = HIWORD(wParam);

        if ((id == ID_LISTVIEW_SUBCLASS_PROC || id == ID_FILEEDIT_SUBCLASS_PROC || id == ID_FILE_SUBCLASS_PROC))
        {
          if (!p->MySetWindowSubclass((HWND)lParam, DefViewSubClass, ID_LISTVIEW_SUBCLASS_PROC))
          {
#if DEBUG_ON
            OFDbg->CWrite("\r\nFailed to set list-view subclass...\r\n");
#endif
          }
          else
          {
            ShowWindow(GetParent(hDlg), SW_HIDE); // Hide the old window so it wont briefly show!

            p->m_hChildWindow = (HWND)lParam;
            p->m_hListView = GetDlgItem(hDlg, ID_FileList); // this works here... but the handle changes!

            // get the file-edit-box handle
            p->m_hMyFileName = GetDlgItem(hDlg, ID_MyFileName);
#if !IS_OLD_BORLAND_CPP_BUILDER
            p->m_hMyFileName = (HWND)SendMessageW(p->m_hMyFileName, CBEM_GETEDITCONTROL, 0, 0);
#endif

          }
        }
      }
    }
    break;
  }

#if IS_OLD_BORLAND_CPP_BUILDER
  return DefDlgProcW(hDlg, uMsg, wParam, lParam);
#else
  return DefSubclassProc(hDlg, uMsg, wParam, lParam);
#endif
}
//---------------------------------------------------------------------------
LRESULT CALLBACK TOFMSDlgForm::DefViewSubClass(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam, LPUINT uIdSubclass, LPDWORD dwRefData)
{
  try
  {
    TOFMSDlgForm* p = (TOFMSDlgForm*)dwRefData;
    if (p == NULL) return NULL;

    switch (uMsg)
    {
      case WM_NOTIFY:
      {
        LPOFNOTIFY ofNotify = (LPOFNOTIFY)lParam;
        NMHDR notifyHdr = ofNotify->hdr;

        if (notifyHdr.hwndFrom != NULL && notifyHdr.idFrom == ID_LISTVIEW_SUBCLASS_PROC)
        {
          if (notifyHdr.code == LVN_ODSTATECHANGED || notifyHdr.code == LVN_ITEMCHANGED)
          {
            // NOTE: If a list-view control has the LVS_OWNERDATA style, and
            // the user selects a range of items by holding down the SHIFT
            // key and clicking the mouse, LVN_ITEMCHANGED notification codes
            // are not sent for each selected or deselected item.Instead,
            // you will receive a single LVN_ODSTATECHANGED notification code,
            // indicating that a range of items has changed state!!!

#if DEBUG_ON
            OFDbg->CWrite("\r\nLVN_ODSTATECHANGED or LVN_ITEMCHANGED\r\n");
#endif
            LPNMLISTVIEW nmListView = (LPNMLISTVIEW)lParam;

            if (nmListView != NULL)
            {
              NMHDR listViewHdr = nmListView->hdr;
              if (listViewHdr.hwndFrom != NULL)
              {
                // Refresh the private member handle for the ListView control
                // when user clicks a new item or selects a range of items...
                p->m_hListView = listViewHdr.hwndFrom;

                PostMessageW(GetParent(hDlg), WM_ITEMS_SELECTED, 0, 0);
              }
            }
          }
        }
      }
      break;
    }
#if IS_OLD_BORLAND_CPP_BUILDER
    return p->MyDefSubclassProc(hDlg, uMsg, wParam, lParam);
#else
    return DefSubclassProc(hDlg, uMsg, wParam, lParam);
#endif
  }
  catch(...)
  {
#if DEBUG_ON
    OFDbg->CWrite( "\r\nException in DefViewSubClass()\r\n");
#endif
    return 0;
  }
}
//---------------------------------------------------------------------------
// **************** Hook function
// callback function (parameter of EnumChildWindows) to find controls
// WM_NOTIFY
//
// wParam (not used)
// lParam points to:
//
// typedef struct _OFNOTIFY {
//   NMHDR          hdr;
//   LPOPENFILENAMEW lpOFN; // OPENFILENAMEW struct
//   LPTSTR         pszFile;
// } OFNOTIFY, *LPOFNOTIFY;
//
// hdr points to:
//
// typedef struct tagNMHDR {
//   HWND     hwndFrom;
//   UINT_PTR idFrom;
//   UINT     code;
// } NMHDR;
//
// code is:
//   CDN_FILEOK
//   CDN_FOLDERCHANGE
//   CDN_HELP
//   CDN_INCLUDEITEM
//     The dialog box sends a CDN_INCLUDEITEM notification for each item in the folder. The dialog box sends this
//     notification only if the OFN_ENABLEINCLUDENOTIFY flag was set when the dialog box was created.
//   CDN_INITDONE
//   CDN_SELCHANGE
//   CDN_SHAREVIOLATION
//   CDN_TYPECHANGE
//
// If the hook procedure returns zero, the dialog box accepts the specified file name and closes.
//
// To reject the specified file name and force the dialog box to remain open, return a nonzero value
//  from the hook procedure and call the SetWindowLong function to set a nonzero DWL_MSGRESULT value.
//
// messages:
// CDM_GETFILEPATH
// CDM_GETFOLDERIDLIST
// CDM_GETFOLDERPATH
// CDM_GETSPEC           // Retrieves the file name (not including the path) of the currently selected file
//                       // Macro: int CommDlg_OpenSave_GetSpec(hwnd, lparam, wparam);
// CDM_HIDECONTROL
// CDM_SETCONTROLTEXT
// CDM_SETDEFEXT
// SETRGBSTRING
//
// The following sample code gets the SETRGBSTRING message identifier and then sets the color selection to blue.
// UINT uiSetRGB;
// uiSetRGB = RegisterWindowMessage(SETRGBSTRING);
// SendMessage(hdlg, uiSetRGB, 0, (LPARAM) RGB(0, 0, 255));
//
// The hook procedure receives notification messages sent from the dialog box. The hook procedure also receives
// messages for any additional controls that you defined by specifying a child dialog template. The hook procedure
// does not receive messages intended for the standard controls of the default dialog box.
//
// If the hook procedure returns zero, the default dialog box procedure processes the message.
// If the hook procedure returns a nonzero value, the default dialog box procedure ignores the message.
// For the CDN_SHAREVIOLATION and CDN_FILEOK notification messages, the hook procedure should return a nonzero
// value to indicate that it has used the SetWindowLong function to set a nonzero DWL_MSGRESULT value.
UINT CALLBACK TOFMSDlgForm::OFNHookProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
    case WM_CLOSE:
    {
      // Call SendMessage(SFDlgForm->DlgHandle, WM_CLOSE, 0, 0); to invoke
      // this hook to destroy the open-file dialog...
      INT r = 0;
      EndDialog(hDlg, (INT)&r);
    }
    break;

    case WM_INITDIALOG:
    {
      // The dialog box procedure should return TRUE to direct the system to set the keyboard focus to the
      // control specified by wParam. Otherwise, it should return FALSE to prevent the system from setting the
      // default keyboard focus.
      // The dialog box procedure should return the value directly. The DWL_MSGRESULT value set by the
      // SetWindowLong function is ignored.
      // The control to receive the default keyboard focus is always the first control in the dialog box that is
      // visible, not disabled, and that has the WS_TABSTOP style. When the dialog box procedure returns TRUE,
      // the system checks the control to ensure that the procedure has not disabled it. If it has been disabled,
      // the system sets the keyboard focus to the next control that is visible, not disabled, and has the WS_TABSTOP.
      // An application can return FALSE only if it has set the keyboard focus to one of the controls of the dialog box.

      // lParam points to OPENFILENAMEW struct with lCustData as our main class-object
      OPENFILENAMEW* p_ofn = (OPENFILENAMEW*)lParam;

      if (p_ofn == NULL)
        break;

      TOFMSDlgForm* p = (TOFMSDlgForm*)p_ofn->lCustData;

      if (p == NULL)
        break;

      // Save the OPENFILENAMEW pointer...
      if (!SetProp(GetParent(hDlg), OFNPROP, (void*)lParam))
      {
#if DEBUG_ON
        OFDbg->CWrite("\r\nSET Prop Failed in FormOFMSDlg()\r\n");
#endif
      }

      if (!p->InitDialog(hDlg)) // Hide original buttons...
      {
#if DEBUG_ON
        OFDbg->CWrite("\r\nInitDialog failed!\r\n");
#endif
      }
      else
      {
        HWND hCustomSelectButton = GetDlgItem(hDlg, ID_CUSTOM_SELECT);
        if (hCustomSelectButton != NULL)
          ::SetFocus(hCustomSelectButton);
      }
    }
    return TRUE;

    case WM_DESTROY:
    {
      // Get the OPENFILENAMEW pointer...
      OPENFILENAMEW* p_ofn = (OPENFILENAMEW*)GetProp(GetParent(hDlg), OFNPROP);
      if (p_ofn == NULL) break;

      TOFMSDlgForm* p = (TOFMSDlgForm*)p_ofn->lCustData;
      if (p == NULL) break;

      try { p->MyRemoveWindowSubclass(hDlg, OpenFileSubClass, ID_FILE_SUBCLASS_PROC); } catch(...) {}
      try { p->MyRemoveWindowSubclass(hDlg, DefViewSubClass, ID_LISTVIEW_SUBCLASS_PROC); } catch(...) {}
      RemoveProp(GetParent(hDlg), OFNPROP);
#if DEBUG_ON
      OFDbg->CWrite("\r\nWM_DESTROY in OFNHookProc()\r\n");
#endif
    }
    break;

    case WM_NOTIFY:
    {
      // Get the OPENFILENAMEW pointer...
      OPENFILENAMEW* p_ofn = (OPENFILENAMEW*)GetProp(GetParent(hDlg), OFNPROP);
      if (p_ofn == NULL) break;

      TOFMSDlgForm* p_osfDlg = (TOFMSDlgForm*)p_ofn->lCustData;

      LPOFNOTIFY p_notify = (LPOFNOTIFY)lParam;

      LRESULT r = p_osfDlg->ProcessNotifyMessage(hDlg, p_notify);

      if (r != 0)
      {
        SetWindowLongW(hDlg, DWL_MSGRESULT, r);
        return r;
      }
    }
    break;

    case WM_SIZE:
    {
      // Get the OPENFILENAMEW pointer...
      OPENFILENAMEW* p_ofn = (OPENFILENAMEW*)GetProp(GetParent(hDlg), OFNPROP);
      if (p_ofn == NULL) break;
      TOFMSDlgForm* p_osfDlg = (TOFMSDlgForm*)p_ofn->lCustData;
      if (p_osfDlg == NULL) break;
      p_osfDlg->ResizeCustomControl(hDlg);
    }
    return TRUE;

    case WM_COMMAND: // If an application processes this message, it should return zero.
    {
      HWND hParent = GetParent(hDlg);
      UINT code = HIWORD(wParam);
      UINT id = LOWORD(wParam);

      switch (code)
      {
        case BN_CLICKED:
        {
          // Get the OPENFILENAMEW pointer...
          OPENFILENAMEW* p_ofn = (OPENFILENAMEW*)GetProp(GetParent(hDlg), OFNPROP);

          if (p_ofn == NULL) break;

          TOFMSDlgForm* p = (TOFMSDlgForm*)p_ofn->lCustData;

          if (p == NULL) break;

          // Do not call the EndDialog function from the hook procedure. Instead, the hook procedure
          // can call the PostMessage function to post a WM_COMMAND message with the IDCANCEL value to the
          // dialog box procedure. Posting IDCANCEL closes the dialog box and causes the dialog box function
          // to return FALSE. If you need to know why the hook procedure closed the dialog box, you must provide
          // your own communication mechanism between the hook procedure and your application.
          switch (id)
          {
            case ID_CUSTOM_OPEN:
            {
              // Translate a click of our "Open" button into the OK button and forward the request to the
              // open file dialog.

              PostMessageW(hParent, WM_COMMAND, MakeWParam(IDOK, BN_CLICKED), (LPARAM)GetDlgItem(hParent, IDOK));
#if DEBUG_ON
              OFDbg->CWrite("\r\nsent BN_CLICKED IDOK!\r\n");
#endif
            }
            return TRUE;

            case ID_CUSTOM_CANCEL:
            {
              p->FResult = IDCANCEL;
              // Note: Calling SendMessage( hParent, WM_CLOSE, 0, 0); seemed to work ok too
              PostMessageW(hParent, WM_COMMAND, MakeWParam(IDCANCEL, BN_CLICKED), (LPARAM)GetDlgItem(hParent, IDCANCEL));
            }
            return TRUE;

            case ID_CUSTOM_SELECT:
            {
              // get the file-edit-box modify flag
              BOOL bModified = (p->m_hMyFileName == NULL) ? FALSE : SendMessage(p->m_hMyFileName, EM_GETMODIFY, 0, 0);

              if (bModified)
              {
                p->FResult = IDCANCEL; // assume bad path...

#if DEBUG_ON
                OFDbg->CWrite("\r\nFile edit control was modified, getting path...\r\n");
#endif

                p->DeleteFileNameObjects();

                // if the user typed their own path - use that...
                int bufLen = GetWindowTextLengthW(p->m_hMyFileName) + 1;
                WideChar* buf = new WideChar[bufLen];

                int strLen = GetWindowTextW(p->m_hMyFileName, buf, bufLen);

                if (strLen > 0)
                {
                  WideString wPath = WideString(buf, strLen);

#if DEBUG_ON
                  OFDbg->CWrite("\r\nEdit Control Path: " + String(wPath) + "\r\n");
#endif
                  if (!wPath.IsEmpty())
                  {
                    TStringList* sl = NULL;

                    // path can have multiple quote and space delineated song-files in it...
                    try
                    {
                      sl = new TStringList();

                      if (p->ParseComplexPath(wPath, sl))
                      {
                        if (sl->Count)
                        {
                          for (int ii = 0; ii < sl->Count; ii++)
                          {
                            bool bIsDirectory = false;

                            // sPath, bIsDirectory are by reference...
                            WideString wS;

#if IS_OLD_BORLAND_CPP_BUILDER
                            wS = OFUtil->Utf8ToWide(sl->Strings[ii]);
#else
                            wS = WideString(sl->Strings[ii]);
#endif

                            if (p->GetShortcut(wS, bIsDirectory))
                              p->AddWideItem(wS, bIsDirectory);
                          }
                        }
                      }
                    }
                    __finally
                    {
                      if (sl) delete sl;
                    }
                  }
                }
                delete [] buf;
              }
              else
              {
                // If the hook procedure returns zero, the dialog box accepts the specified file name and closes.
#if DEBUG_ON
                OFDbg->CWrite("\r\nGetting selected items\r\n");
#endif
                if (!p->GetSelectedItems())
                {
                  p->FResult = IDCANCEL;
#if DEBUG_ON
                  OFDbg->CWrite("\r\nError in GetSelectedItems(), returning IDCANCEL...\r\n");
#endif
                }
                else
                {
                  p->FResult = IDOK;
#if DEBUG_ON
                  OFDbg->CWrite("\r\nGot selected items, returning IDOK...\r\n");
#endif
                }
              }

              // Note: Calling SendMessage( hParent, WM_CLOSE, 0, 0);
              // seemed to work ok too...
              PostMessageW(hParent, WM_COMMAND,
                    MakeWParam(IDCANCEL, BN_CLICKED),
                         (LPARAM)GetDlgItem(hParent, IDCANCEL));

            } // end case ID_CUSTOM_SELECT
            return TRUE;
#if DEBUG_ON
            default:
            {
              OFDbg->CWrite("\r\nUnknown BN_CLICKED id: " + IntToHex((int)id, 8) + "\r\n");
            }
            break;
#endif
          } // end switch "id"
        } // end case BN_CLICKED
        break;
#if DEBUG_ON
        default:
        {
          OFDbg->CWrite("\r\nUnknown BN_CLICKED: " + IntToHex((int)code, 8) + "\r\n");
        }
        break;
#endif
      } // end switch "code"
    } // end case WM_COMMAND
    break;

//#if DEBUG_ON
//    default:
//    {
//      OFDbg->CWrite("\r\nUnknown msg: " + IntToHex((int)msg, 8) + "\r\n");
//    }
//    break;
//#endif
  } // end switch msg
  return FALSE; // Calls default if you return 0
}
//---------------------------------------------------------------------------
bool __fastcall TOFMSDlgForm::ParseComplexPath(WideString wPath, TStringList* sl)
{
  // NOTE: found another quirk in the dialog... if you select multiple files
  // and the length is longer than the box's width, the file-path part is
  // omitted! The string might just begin with a quote or a space-quote - then,
  // we need to add the path ourselves!

  int iLen = wPath.Length();
  if (iLen == 0) return false;

  int Q1 = 0;
  WideString wAcc, wMainPath;
  bool bHaveMainPath = false;

  int ii;
  for (ii = 1; ii <= iLen; ii++)
  {
    WideChar c = wPath[ii];

    if (c == '\"')
    {
      if (!bHaveMainPath)
      {
        // set the main path
        wMainPath = WTrim(wAcc); // trim any spaces before the leading quote;
        wAcc = "";

        if (wMainPath.IsEmpty())
        {
          wMainPath = OFUtil->GetCurrentDirW();
          
          // has to be terminated!
          if (wMainPath.Length() > 0 && wMainPath[wMainPath.Length()] != '\\')
            wMainPath += "\\";
        }

        bHaveMainPath = true;
        Q1 = ii;
      }
      else if (Q1)
      {
        wAcc = WTrim(wMainPath) + WTrim(wAcc);
        if (wAcc.Length())
        {
#if IS_OLD_BORLAND_CPP_BUILDER
          sl->Add(OFUtil->WideToUtf8Ansi(wAcc));
#else
          sl->Add(String(wAcc));
#endif
#if DEBUG_ON
          OFDbg->CWrite("\r\nParseComplexPath: \"" + String(wAcc) + "\"\r\n");
#endif
        }
        Q1 = 0;
        wAcc = "";
      }
      else
        Q1 = ii;
    }
    else
      wAcc += WideString(c);
  }
  if (!bHaveMainPath)
  {
    // set the main path
    wAcc = WTrim(wAcc); // trim any spaces before the leading quote;

    if (wAcc.IsEmpty())
    {
      wAcc = OFUtil->GetCurrentDirW();

      // has to be terminated!
      if (wAcc.Length() > 0 && wAcc[wAcc.Length()] != '\\')
        wAcc += "\\";
    }

    if (wAcc.Length())
    {
#if IS_OLD_BORLAND_CPP_BUILDER
      sl->Add(OFUtil->WideToUtf8Ansi(wAcc));
#else
      sl->Add(String(wAcc));
#endif
#if DEBUG_ON
      OFDbg->CWrite("\r\nParseComplexPath: \"" + String(wAcc) + "\"\r\n");
#endif
    }
  }
  return true;
}
//---------------------------------------------------------------------------
WideString __fastcall TOFMSDlgForm::WTrim(WideString wIn)
{
#if IS_OLD_BORLAND_CPP_BUILDER
  while (wIn.Length() > 1 && wIn[wIn.Length()] == ' ')
    wIn = wIn.SubString(1, wIn.Length()-1);
  while (wIn.Length() > 1 && wIn[1] == ' ')
    wIn = wIn.SubString(2, wIn.Length()-1);
  if (wIn.Length() == 1 && wIn[1] == ' ')
    return "";
  return wIn;
#else
  return wIn.Trim();
#endif
}
//---------------------------------------------------------------------------
int __fastcall TOFMSDlgForm::ProcessNotifyMessage(HWND hDlg, LPOFNOTIFY p_notify)
{
  if (p_notify == NULL)
    return 0;

  // notifyData.hdr.code can be: CDN_FILEOK, CDN_FOLDERCHANGE, CDN_HELP,
  // CDN_INITDONE, CDN_SELCHANGE, CDN_SHAREVIOLATION, CDN_TYPECHANGE.
  switch (p_notify->hdr.code)
  {
    case CDN_SELCHANGE:
    {
      // when a new file is selected, this sets the file-name box and window
      // title to the new file
#if DEBUG_ON
      OFDbg->CWrite("\r\nCDN_SELCHANGE in FormOFMSDlg()\r\n");
#endif
      if (NewFileSelected(hDlg))
        return TRUE;
    }
    break;

    case CDN_TYPECHANGE:
    {
      if (p_notify->lpOFN == NULL || p_notify->lpOFN->lCustData == NULL)
        break;

      LPOPENFILENAME pOFN = p_notify->lpOFN;
      TOFMSDlgForm* pThis = (TOFMSDlgForm*)pOFN->lCustData;

      int idx = pOFN->nFilterIndex;

      // typechange: user changed filters - we want to append the new
      // filter to FDefFile and set the file edit box to it
      wchar_t* pFilter = pThis->GetFilter((wchar_t*)pOFN->lpstrFilter,
                                                idx, pThis->FilterCount);

      if (pFilter == NULL)
        break;

      if (pFilter[0] == '*') // skip leading *
        pFilter++;

      if (pFilter[0] == '\0')
        break;

      pThis->FCurrentFilter = WideString(pFilter); // still has the . on it
      pThis->FFilterIndex = idx;

#if DEBUG_ON
      OFDbg->CWrite("\r\nFormOFMSDlg CDN_TYPECHANGE: " +
                              String(pThis->FCurrentFilter) + "\r\n");
#endif
    }
    break;

#if DEBUG_ON
    case CDN_INCLUDEITEM:
    {
      OFDbg->CWrite("\r\nCDN_INCLUDEITEM in FormOFMSDlg()\r\n");
    }
    break;
#endif

    case CDN_FOLDERCHANGE:
    {
      // when a new folder is opened, this sets the file-name box
      // and window title to the new path
#if DEBUG_ON
      OFDbg->CWrite("\r\nCDN_FOLDERCHANGE in FormOFMSDlg()\r\n");
#endif
      HWND hParent = GetParent(hDlg);

      // Show current folder path in window-title-bar and in the file edit-box
      WideString newFolder =
                this->GetTextFromCommonDialog(hParent, CDM_GETFOLDERPATH);

      if (!newFolder.IsEmpty() && this->FCurrentFolder != newFolder)
      {
        this->FCurrentFolder = newFolder;

        // Set TOFMSDlgForm's window-title to the current folder
        WideString wTemp = this->FDlgTitle + L" - " + newFolder;
        SetWindowTextW(hParent, wTemp.c_bstr());

        HWND hFileName = GetDlgItem(hParent, ID_MyFileName);

        // clear the file-name edit-box when a new folder is opened
        if (hFileName != NULL)
          SetWindowTextW(hFileName, L"");

        return TRUE;
      }
    }
    break;

    case CDN_FILEOK: // press of the old dialog's Open button (we do a remote press of it someplace...)
    {
#if DEBUG_ON
      OFDbg->CWrite("\r\nCDN_FILEOK in FormOFMSDlg()\r\n");
#endif
      // If the hook procedure returns zero, the dialog box accepts the specified file name and closes.
      // To reject the specified file name and force the dialog box to remain open, return a nonzero value from
      // the hook procedure and call the SetWindowLong function to set a nonzero DWL_MSGRESULT value.
      //
      // the hook procedure receives the CDN_FILEOK message when the user chooses a file name
      // and clicks the OK button. In response to this message, the hook procedure can use the
      // SetWindowLong function to reject the selected name and force the dialog box to remain open.
      //
      // For the CDN_SHAREVIOLATION and CDN_FILEOK notification messages, the hook procedure should return a nonzero
      // value to indicate that it has used the SetWindowLong function to set a nonzero DWL_MSGRESULT value.
      //
      // If you use SetWindowLong with the DWL_MSGRESULT index to set the return value for a message
      // processed by a dialog procedure, you should return TRUE directly afterward. Otherwise, if you call any
      // function that results in your dialog procedure receiving a window message, the nested window message could
      // overwrite the return value you set using DWL_MSGRESULT.

      // This works - does a press of our new Select button when the user presses Enter from the file-name edit-box...
      // It will add an individual son-file to a player-list but won't add a directory... never comes here if the
      // path is not a valid file...
      PostMessageW(hDlg, WM_COMMAND, MakeWParam(ID_CUSTOM_SELECT, BN_CLICKED),
                                 (LPARAM)GetDlgItem(hDlg, ID_CUSTOM_SELECT));
    }
    return TRUE;

    case CDN_INITDONE:
    {
      // save the parent handle for later use by the destructor to send a WM_CLOSE
      this->FDlgHandle = GetParent(hDlg);

#if DEBUG_ON
      OFDbg->CWrite("\r\nTOFMSDlgForm() CDN_INITDONE\r\n");
#endif
      HWND hFileName = GetDlgItem(this->FDlgHandle, ID_MyFileName);
      if (hFileName != NULL)
        ::SetFocus(hFileName);
    }
    break;
  }

  return 0;
}
//---------------------------------------------------------------------------
bool __fastcall TOFMSDlgForm::SetFileName(HWND hEdit, WideString wName)
{
  try
  {
    if (FDisplayFullPath)
    {
      WideString wDir = OFUtil->GetCurrentDirW();

        // Fyi: ExpandFileName() also neat function
        // try { sPath = ExtractRelativePath(ExtractFilePath(sListFullPath), sSongFullPath); }
        // catch(...) {}

        WideString wPath = "";

        int len = wDir.Length();
        if (len > 0)
        {
          WideChar c = wDir[wDir.Length()];
          if (c != L'\\')
            wPath = wDir + WideString("\\") + wName;
          else
            wPath = wDir + wName;
        }
        else
          wPath = wName;

        SetWindowTextW(hEdit, wPath.c_bstr());

        this->FCurrentFolder = wDir;
    }
    else
      SetWindowTextW(hEdit, wName.c_bstr());

    return true;
  }
  catch(...){ return false; }
}
//---------------------------------------------------------------------------
bool __fastcall TOFMSDlgForm::NewFileSelected(HWND hDlg)
{
  HWND hParent = GetParent(hDlg);

  if (hParent == NULL)
    return false;

  bool bRet = false;

  // Show current folder path in window-title-bar and in the file edit-box
  WideString newFile = this->GetTextFromCommonDialog(hParent, CDM_GETFILEPATH);

#if DEBUG_ON
  OFDbg->CWrite("\r\n" + String(newFile) + "\r\n");
#endif

  if (!newFile.IsEmpty())
  {
    this->FFolderIsSelected = OFUtil->DirectoryExistsW(newFile) ? true : false;

    HWND hFileName = GetDlgItem(hParent, ID_MyFileName);

#if IS_OLD_BORLAND_CPP_BUILDER
    HWND hWndEdit = hFileName;
#else
    HWND hWndEdit = (HWND)SendMessageW(hFileName, CBEM_GETEDITCONTROL, 0, 0);
#endif

    // clear the file-name edit-box when a new folder is opened
    if (hWndEdit != NULL)
    {
      if (!FDisplayFullPath)
        newFile = ExtractFileName(newFile);

      SetWindowTextW(hWndEdit, newFile.c_bstr());
      bRet = true;
    }
  }
  else
    this->FFolderIsSelected = false;

  return bRet;
}
//---------------------------------------------------------------------------
WideString __fastcall TOFMSDlgForm::GetListViewItemText(HWND hListView, int selectedIndex)
{
  LVITEMW lvitem = {0};
  lvitem.mask = LVIF_TEXT;
  WideChar* nativeBuffer = NULL;

  WideString wName;

  try
  {
    nativeBuffer = new WideChar[OF_BUFSIZE];
    if (nativeBuffer != NULL)
    {
      lvitem.pszText = nativeBuffer;
      lvitem.cchTextMax = OF_BUFSIZE;
      int length = SendMessageW(hListView, LVM_GETITEMTEXTW, selectedIndex, (LPARAM)&lvitem);
      wName = WideString(lvitem.pszText, length);
    }
  }
  __finally
  {
    if (nativeBuffer != NULL)
      try { delete [] nativeBuffer; } catch(...) {}
  }

  return wName;
}
//-------------------- FileSaveDlg function -------------------
/*
//---------------------------------------------------------------------------
bool __fastcall TOFMSDlgForm::FileSaveDlg(WideString szDefExt, WideString szDlgTitle)
{
  p_szFileName[0] = p_szTitleName[0] = (WideChar)'\0';

  for(unsigned int i = 0; i <= 10; i++)
    p_szFileName[i] = p_szTitleName[i] = szUntitled[i];

  m_ofn.lpstrFilter =  this->SetFilter();
  m_ofn.lpstrDefExt = szDefExt.c_bstr();
  m_ofn.lpstrTitle = szDlgTitle.c_bstr();
  m_ofn.Flags = OFN_OVERWRITEPROMPT;

  return GetSaveFileNameW(&m_ofn);
}
//---------------------------------------------------------------------------
*/
//---------------------------------------------------------------------------
void __fastcall TOFMSDlgForm::FormActivate(TObject *Sender)
{
  if (this->FDlgHandle)
    BringWindowToTop(this->FDlgHandle);
}
//---------------------------------------------------------------------------


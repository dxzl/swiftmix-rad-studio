//---------------------------------------------------------------------------
// YahCoLoRiZe - Edit, process and send colorized text into IRC chat-rooms
// via various chat-clients such as mIRC, IceCHat and LeafCHat
//
// Author: Scott Swift
//
// Released to GitHub under GPL v3 October, 2016
//
//---------------------------------------------------------------------------
#include <vcl.h>
#include "Main.h"
#pragma hdrstop

#include "FormSFDlg.h"
#include "ShlObj.h"
#include "ObjIdl.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSFDlgForm *SFDlgForm;
//---------------------------------------------------------------------------
void __fastcall TSFDlgForm::FormCreate(TObject *Sender)
{
  TForm* f = static_cast<TForm*>(Sender);
  HWND h = f ? f->Handle : Application->Handle;

  OleInitialize(NULL);

  FDlgHandle = NULL;

  p_szFileName = new WideChar[SF_BUFSIZE];
  p_szTitleName = new WideChar[SF_BUFSIZE];

  if (p_szFileName == NULL || p_szTitleName == NULL)
  {
#if DEBUG_ON
    SFDbg->CWrite( "\r\nUnable to load Comctl32.dll!\r\n");
#endif
    return;
  }

  ZeroMemory(&m_sfn, sizeof(OPENFILENAMEW));

  m_sfn.lStructSize       = sizeof(OPENFILENAMEW);
  m_sfn.hwndOwner         = this->Handle; // set NULL or you can't drag/drop!
  m_sfn.hInstance         = (HINSTANCE__*)GetWindowLong(h, GWL_HINSTANCE);
  m_sfn.lpstrFile         = p_szFileName;   // Set to member variable address
  m_sfn.nMaxFile          = SF_BUFSIZE;
  m_sfn.lpstrFileTitle    = p_szTitleName;   // Set to member variable address
  m_sfn.nMaxFileTitle     = SF_BUFSIZE;
  m_sfn.lpfnHook          = SFNHookProc;
  m_sfn.lCustData         = (unsigned long)this; // pointer to us to pass to the callbacks

  p_filterBuf = NULL;

  FFilterCount = 0;
  FFilterIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TSFDlgForm::FormDestroy(TObject *Sender)
{
// this never is called!!!!!!!!!!!!!

  try { if (p_filterBuf != NULL) delete [] p_filterBuf; } catch(...) {}
  try { if (p_szTitleName != NULL) delete [] p_szTitleName; } catch(...) {}
  try { if (p_szFileName != NULL) delete [] p_szFileName; } catch(...) {}

#if DEBUG_ON
  SFDbg->CWrite("\r\nFormDestroy() in TSFDlgForm()!\r\n");
#endif

  OleUninitialize();
}
//---------------------------------------------------------------------------
bool __fastcall TSFDlgForm::Execute(String uDefFile,
                                    String uInitialDir, String uDlgTitle)
// Strings are passed in as UTF-8 and are converted to String
{
  p_szFileName[0] = p_szTitleName[0] = C_NULL;

  FDefFile = uDefFile;
  FDefExt = ExtractFileExt(FDefFile);

  FInitialDir = uInitialDir;
  FDlgTitle = uDlgTitle;

  m_sfn.lpstrFilter = this->SetFilter(); // sets m_filterCount property var!

  // Try to locate the 1-based filter-index of the extension on our sDefFile
  // in the null-separated list of filters in the lpstrFilter filters-string
  FFilterIndex = FindFilter((wchar_t*)m_sfn.lpstrFilter,
        String((String("*") + this->FDefExt)).w_str(),
                                                FFilterCount);

  // set to the first filter if the extension passed in is not there
  // (You would usually set the first filter to *.*)
  if (FFilterIndex <= 0 && FFilterCount > 0)
    FFilterIndex = 1;

  FCurrentFilter = String(GetFilter((wchar_t*)m_sfn.lpstrFilter,
                                        FFilterIndex, FFilterCount));
  m_sfn.nFilterIndex = FFilterIndex;
  //--------------------------------------------

  // point lpstrFile to a buffer
  wchar_t fileBuf[MAX_PATH+1];
  fileBuf[0] = C_NULL;

  // Copy the file and extension to the buffer
  if (!FDefFile.IsEmpty())
    wcscpy(fileBuf, FDefFile.w_str());

  // Set the rest of the OPENFILENAMEW struct members...
  m_sfn.lpstrFile = fileBuf;
  m_sfn.lpstrTitle = FDlgTitle.w_str();
  m_sfn.lpstrInitialDir = FInitialDir.w_str();

  m_sfn.Flags = OFN_NOTESTFILECREATE|OFN_HIDEREADONLY|OFN_EXPLORER|
      OFN_NODEREFERENCELINKS|OFN_PATHMUSTEXIST|OFN_ENABLEHOOK|OFN_ENABLESIZING|OFN_PATHMUSTEXIST;
  //OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

  // If the user specifies a file name and clicks the OK button and the function is successful,
  // the return value is nonzero. The buffer pointed to by the lpstrFile member of the OPENFILENAME
  // structure contains the full path and file name specified by the user.
  return GetSaveFileNameW(&m_sfn); // this runs the system's save-file dialog!
}
//---------------------------------------------------------------------------
WideChar* __fastcall TSFDlgForm::SetFilter(void)
// Scans the FFilters String (property "Filters") to replace vertical bar '|'
// separators with '\0' and also count the number of filters for the FilterCount
// property. this->p_filterBuf[] will have the output that the file-open dialog
// can use.
//
// This function allocates/re-allocates the output buffer!
{
  try
  {
    if (FFilters.IsEmpty())
      FFilters = "All files (*.*)|*.*";

    int lenStr = FFilters.Length();
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

    // Replace | with L'\0' and pad out to end of buffer
    for (; ii < lenStr; ii++)
    {
      c = FFilters[ii+1];

      if (c == '|')
      {
        c = C_NULL;
        count++;
      }

      this->p_filterBuf[ii] = c;
    }

    this->p_filterBuf[ii] = C_NULL;

    // odd # of vertical-bar separators then add 1 to make number divisable by 2
    // and we need an extra null to make two at the end...
    if ((count & 1) != 0)
    {
      count++;
      this->p_filterBuf[ii+1] = C_NULL;
    }

    count /= 2; // two vertical bars for each filter

    FFilterCount = count; // set internal property var

    return this->p_filterBuf;
  }
  catch(...) { return NULL; }
}
//---------------------------------------------------------------------------
wchar_t* __fastcall TSFDlgForm::GetFilter(wchar_t* pFilterBuf, int iFilter, int iMax)
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
    while (*pFilterBuf != C_NULL)
      pFilterBuf++;

    pFilterBuf++;

    p_start = pFilterBuf; // start on the part we want!

    // second part of the filter (the actual ".wma" filter)
    while (*pFilterBuf != C_NULL)
      pFilterBuf++;

    idx++;
    pFilterBuf++;

  } while (idx < iFilter && idx < iMax);

  return p_start;
}
//---------------------------------------------------------------------------
int __fastcall TSFDlgForm::FindFilter(wchar_t* pFilterBuf, wchar_t* pFilterToFind, int iMax)
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
    while (*pFilterBuf != C_NULL)
      pFilterBuf++;

    pFilterBuf++;

    bEqual = true;
    ii = 0;

    // second part of the filter (the actual ".wma" filter)
    for(;;)
    {
      if (*pFilterBuf == C_NULL)
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
String __fastcall TSFDlgForm::GetNextFileName(void)
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
      if((szDirectory[i] = p_szFileName[iPosition]) == WideChar(C_NULL))
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
  if(p_szFileName[iPosition] == WideChar(C_NULL))
    iPosition++;

  szFileName[0] = WideChar(C_NULL);

  for(unsigned int j = 0; j < sizeof(szFileName)/sizeof(szFileName[0]); j++, iPosition++)
  {
    if((szFileName[j] = p_szFileName[iPosition]) == WideChar(C_NULL))
    {
      if(p_szFileName[iPosition + 1] == WideChar(C_NULL))
        bLastName = TRUE;

      //-----------------making full file path -------------
      unsigned short k = 0;
      for(;k < m_ofn.nFileOffset; k++)
      {
        if((szResult[k] = szDirectory[k]) == WideChar(C_NULL))
          break;
      }
      for(unsigned int u = 0; u < sizeof(szFileName) / sizeof(szFileName[0]); u++, k++)
      {
        if((szResult[k] = szFileName[u]) == WideChar(C_NULL))
        {
          bFirstTime = FALSE;
          return String(szResult);
        }
      }
    }
  }
  return "";
}
*/
//---------------------------------------------------------------------------
//bool __fastcall TSFDlgForm::GetMultiSelect(void)
//{
//  return m_ofn.Flags & OFN_ALLOWMULTISELECT;
//}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Property getter
String __fastcall TSFDlgForm::GetTitle(void)
{
  return String(p_szTitleName);
}
//---------------------------------------------------------------------------
String __fastcall TSFDlgForm::GetTextFromCommonDialog(HWND hDlg, UINT msg)
{
  try
  {
    WideChar* buf = NULL;
    String str;

    try
    {
      buf = new WideChar[SF_BUFSIZE];
      buf[0] = C_NULL;

      // NOTE: you are "supposed" to be able to get the required buffer size
      // - I tried 0, -1 - null-pointer, a buffer of 40 bytes... function always
      // just returns "1"
//      DWORD ret = SendMessageW(hDlg, msg, 0, NULL);
//#if DEBUG_ON
//      SFDbg->CWrite("\r\nBufsize required: " + String(ret) + "\r\n");
//#endif

      SendMessageW(hDlg, msg, SF_BUFSIZE, (LPARAM)buf);

      str = String(buf);
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
UINT CALLBACK TSFDlgForm::SFNHookProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
// hDlg is a handle to the child control... use GetParent(hDlg) to get the main dialog's handle
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

// doing the same thing in CDN_INITDONE
#if DEBUG_ON
    case WM_INITDIALOG:
    {
      // Save the OPENFILENAMEW pointer...
//      if (!SetProp(GetParent(hDlg), "SFN", (void*)lParam))
//      {
//#if DEBUG_ON
//        SFDbg->CWrite("\r\nSET Prop Failed in FormSFDlg()\r\n");
//#endif
//      }

//      OPENFILENAMEW* p_sfn = (OPENFILENAMEW*)lParam;
//      if (p_sfn == NULL) break;
//      TSFDlgForm* p = (TSFDlgForm*)p_sfn->lCustData;
//      if (p == NULL) break;
//
//      // Save the main dialog handle as a property
//      p->DlgHandle = GetParent(hDlg);

      SFDbg->CWrite("\r\nWM_INITDIALOG in SFNHookProc()\r\n");
    }
    break;
#endif

#if DEBUG_ON
    case WM_DESTROY:
    {
      // Take out tour callback hook before the function goes away!
//      OPENFILENAMEW* p_sfn = (OPENFILENAMEW*)GetProp(GetParent(hDlg), "SFN");
//      if (p_sfn != NULL)
//        p_sfn->lpfnHook = NULL;
//
//      RemoveProp(GetParent(hDlg), "SFN");

      SFDbg->CWrite("\r\nWM_DESTROY in SFNHookProc()\r\n");
    }
    break;
#endif

    case WM_NOTIFY:
    {
      LPOFNOTIFY p_notify = (LPOFNOTIFY)lParam;

      if (p_notify == NULL || p_notify->lpOFN == NULL ||
                                      p_notify->lpOFN->lCustData == NULL)
        break;

      LPOPENFILENAME pOFN = p_notify->lpOFN;
      TSFDlgForm* pThis = (TSFDlgForm*)pOFN->lCustData;

      switch (p_notify->hdr.code)
      {
        //CDN_FILEOK
        //CDN_FOLDERCHANGE
        //CDN_HELP
        //CDN_INCLUDEITEM
        //CDN_INITDONE
        //CDN_SELCHANGE
        //CDN_SHAREVIOLATION
        //-----------------------------------------------
        case CDN_FOLDERCHANGE:
        {
#if DEBUG_ON
          SFDbg->CWrite("\r\nCDN_FOLDERCHANGE in FormSFDlg()\r\n");
#endif
          HWND hParent = GetParent(hDlg);

          String newFolder =
                  pThis->GetTextFromCommonDialog(hParent, CDM_GETFOLDERPATH);

          if (!newFolder.IsEmpty() && pThis->FCurrentFolder != newFolder)
          {
            pThis->FCurrentFolder = newFolder;

            // Set TSFDlgForm's window-title to the current folder
            SetWindowTextW(hParent, String(pThis->FDlgTitle + ": " + newFolder).w_str());
          }
        }
        return TRUE;

        case CDN_FILEOK: // press of the old dialog's Save button
        {
          if (pOFN->lpstrFile != NULL)
          {
            pThis->FFileName = String((wchar_t*)pOFN->lpstrFile);
#if DEBUG_ON
            SFDbg->CWrite("\r\nCDN_FILEOK in FormSFDlg(): " + pThis->FFileName + "\r\n");
#endif
          }
        }
        break;

        case CDN_TYPECHANGE:
        {
          int idx = pOFN->nFilterIndex;

          // typechange: user changed filters - we want to append the new
          // filter to FDefFile and set the file edit box to it
          wchar_t* pFilter = pThis->GetFilter((wchar_t*)pOFN->lpstrFilter,
                                     idx, pThis->FilterCount);

          if (pFilter == NULL)
            break;

          if (pFilter[0] == '*') // skip leading *
            pFilter++;

          if (pFilter[0] == C_NULL)
            break;

          pThis->FCurrentFilter = String(pFilter); // still has the . on it
          pThis->FFilterIndex = idx;

#if DEBUG_ON
          SFDbg->CWrite("\r\nFormSFDlg CDN_TYPECHANGE: " +
                                   String(pThis->FCurrentFilter) + "\r\n");
#endif

          // Get the file-box text and put a new filter on it...

          HWND hFileName = GetDlgItem(GetParent(hDlg), ID_FileName);

          if (hFileName == NULL)
            break;

          // Get the file-name box text-length
          int len = GetWindowTextLengthW(hFileName);

          String sText;

          if (len > 0)
          {
            // Get the text in the file-name box
            wchar_t* buf = new wchar_t[len+1];
            GetWindowTextW(hFileName, buf, len+1);
            sText = String(buf);
            delete [] buf;
          }

          len = sText.Length();

          if (len == 0)
            sText = pThis->DefFile;
          else
          {
            do // scan string backward for a dot
            {
              if (sText[len] == '.')
                break;
              len--;
            } while (len > 0);

            if (len != 0) // found extension's dot!
              sText = sText.SubString(1, len-1); // strip off old extension
          }

          if (pThis->CurrentFilter == String(".*"))
            sText += pThis->DefExt;
          else
            sText += pThis->CurrentFilter;

          // write the old text (or default file-name) with the new filter to the file-name box
          SetWindowText(hFileName, sText.w_str());

#if DEBUG_ON
          SFDbg->CWrite("\r\nCDN_TYPECHANGE text: " + sText + "\r\n");
#endif
        }
        break;

        case CDN_INITDONE:
        {
          // save the handle for later use by the destructor to send a WM_CLOSE
          pThis->FDlgHandle = GetParent(hDlg);

#if DEBUG_ON
          SFDbg->CWrite("\r\nTSFDlgForm() CDN_INITDONE\r\n");
#endif
        }
        break;

#if DEBUG_ON
        default:
          SFDbg->CWrite("\r\nFormSFDlg() p_notify unhandled code: " + String(p_notify->hdr.code) + "\r\n");
        break;
#endif
      }
      break;
    }
  } // end switch msg
  return FALSE; // Calls default if you return 0
}
//---------------------------------------------------------------------------
void __fastcall TSFDlgForm::FormActivate(TObject *Sender)
{
  if (this->FDlgHandle)
    BringWindowToTop(this->FDlgHandle);
}
//---------------------------------------------------------------------------


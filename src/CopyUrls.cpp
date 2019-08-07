//---------------------------------------------------------------------------
#include <vcl.h>
#include "Main.h"
#pragma hdrstop

#include "CopyUrls.h"
#include "SMList.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// Pass in a TPlaylistForm* as Owner!
__fastcall TMyUrlCopy::TMyUrlCopy(TComponent* Owner, String sSource, String sDest, int idx)
//__fastcall TMyUrlCopy::TMyUrlCopy(TPlaylistForm* f, String sSource, String sDest, int idx)
{
  if (!Owner || !MainForm->MyFileCopyList)
    return;

  try
  {
      m_hDll = LoadLibrary(L"wininet.dll"); // "wininet.dll"

      if (m_hDll == NULL)
      {
#if DEBUG_ON
        MainForm->CWrite( "\r\nTMyUrlCopy::~TMyUrlCopy(): Can't load wininet.dll\r\n");
#endif
        return;
      }

      // Get custom dll's entry point
      pInternetOpen = (tInternetOpen)MainForm->LoadProcAddr(m_hDll, L"InternetOpenW"); // need a leading underscore????
      pInternetOpenUrl = (tInternetOpenUrl)MainForm->LoadProcAddr(m_hDll, L"InternetOpenUrlW"); // need a leading underscore????
      pInternetCloseHandle = (tInternetCloseHandle)MainForm->LoadProcAddr(m_hDll, L"InternetCloseHandle"); // need a leading underscore????
      pInternetReadFile = (tInternetReadFile)MainForm->LoadProcAddr(m_hDll, L"InternetReadFile"); // need a leading underscore????

      if (!pInternetOpen || !pInternetOpenUrl || !pInternetCloseHandle || !pInternetReadFile)
      {
#if DEBUG_ON
        DWORD err = GetLastError();
        MainForm->CWrite( "\r\nTMyUrlCopy::~TMyUrlCopy(): Bad proc-address:LastError(): " + String(err) + "\r\n");
#endif
        return;
      }
  }
  catch(...)
  {
    return;
  }

  m_plForm = (TPlaylistForm*)Owner;
  m_list = (m_plForm == ListA) ? 0 : 1;

  m_sSource = sSource;
  m_sDest = sDest;
  m_idx = idx;

  // file handles
  m_fw = NULL;
  m_hWebConnect = NULL;
  m_hWebAddress = NULL;
  m_hDll = NULL;

  m_lTotalWritten = 0;
  m_bCancel = false;
  m_bTimeout = false;

  m_timerInterval = TIME_URLCOPY_INITIAL;
  pTimer = new TTimer(Owner);
  pTimer->Enabled = false;
  pTimer->Interval = m_timerInterval;
  pTimer->OnTimer = this->TimerEvent;

  // add ourself's object-pointer to TList in the parent form (TProgress)
  MainForm->MyFileCopyList->Add(this);

  pTimer->Enabled = true; // 50ms and we start copy
}
//---------------------------------------------------------------------------
__fastcall TMyUrlCopy::~TMyUrlCopy(void)
{
  // close files if open
  try
  {
    // try to pump through a clean cancel...
    m_bCancel = true;
    Application->ProcessMessages();

    if (pTimer)
      delete pTimer;

    if (MainForm->MyFileCopyList)
    {
      // remove our pointer from the parent list
      int idx = MainForm->MyFileCopyList->IndexOf(this);
      if (idx >= 0)
        MainForm->MyFileCopyList->Delete(idx);
    }

    if (m_fw)
    {
      FileClose(m_fw);

      // delete write-file if we are aborting uncleanly...
      if (FileExists(m_sDest))
        DeleteFile(m_sDest);
    }

    if (m_hWebAddress)
      pInternetCloseHandle(m_hWebAddress);

    if (m_hWebConnect)
      pInternetCloseHandle(m_hWebConnect);

    if (m_hDll)
      FreeLibrary(m_hDll);
  }
  catch(...)
  {
#if DEBUG_ON
    MainForm->CWrite( "\r\nTMyUrlCopy::~TMyUrlCopy(): Exception in TMyUrlCopy destructor!: \"" + m_sSource +"\"\r\n");
#endif
  }
}
//---------------------------------------------------------------------------
long __fastcall TMyUrlCopy::MyUrlCopy(void)
{
  if (m_hDll || !pInternetOpen || !pInternetOpenUrl ||
               !pInternetCloseHandle || !pInternetReadFile)
    return -2;

  Byte* pBuf = NULL;

  try
  {
    try
    {
      // this timeout will keep resetting in the file read/write loop...
      pTimer->Enabled = false;
      m_timerInterval = TIME_URLCOPY_TIMEOUT;

      m_lTotalWritten = 0;
      m_bCancel = false;
      m_bTimeout = false;

      if (m_hWebAddress)
        pInternetCloseHandle(m_hWebAddress);
      if (m_hWebConnect)
        pInternetCloseHandle(m_hWebConnect);
      if (m_fw)
        FileClose(m_fw);

      m_fw = FileCreate(m_sDest);

      if (!m_fw)
        return -3;

      //  LPCWSTR lpszAgent,
      //  DWORD   dwAccessType,
      //  LPCWSTR lpszProxy,
      //  LPCWSTR lpszProxyBypass,
      //  DWORD   dwFlags
      m_hWebConnect = pInternetOpen((LPCWSTR)L"Default_User_Agent",
       (DWORD)INTERNET_OPEN_TYPE_PRECONFIG, (LPCWSTR)NULL,
        (LPCWSTR)NULL, (DWORD)0);
      if (!m_hWebConnect)
        return -4;

      //INTERNETAPI_(HINTERNET) InternetOpenUrlW(
      //    _In_ HINTERNET hInternet,
      //    _In_ LPCWSTR lpszUrl,
      //    _In_reads_opt_(dwHeadersLength) LPCWSTR lpszHeaders,
      //    _In_ DWORD dwHeadersLength,
      //    _In_ DWORD dwFlags,
      //    _In_opt_ DWORD_PTR dwContext
      //    );
      m_hWebAddress = pInternetOpenUrl(m_hWebConnect,
       (LPCWSTR)m_sSource.c_str(), (LPCWSTR)NULL, (DWORD)0,
        (DWORD)INTERNET_FLAG_KEEP_CONNECTION, (DWORD_PTR)0);
      if (!m_hWebAddress)
      {
         pInternetCloseHandle(m_hWebConnect);
         return -5;
      }

      DWORD DataSize = URLMOVE_BUFSIZE;
      pBuf = new Byte[DataSize];

      if (!pBuf)
        return -6;

      for(;;)
      {
        // stop failsafe timeout
        pTimer->Enabled = false;

        Application->ProcessMessages();

        if (this->m_bCancel)
        {
#if DEBUG_ON
          MainForm->CWrite( "\r\nTMyUrlCopy::MyUrlCopy(): exiting loop due to m_bCancel set: \"" + m_sSource +"\"\r\n");
#endif
          return -1;
        }

        if (this->m_bTimeout)
        {
#if DEBUG_ON
          MainForm->CWrite( "\r\nTMyUrlCopy::MyUrlCopy(): exiting loop due to m_bTimeout set: \"" + m_sSource +"\"\r\n");
#endif
          return -7;
        }

        // restart the main timeout timer
        pTimer->Interval = m_timerInterval;
        pTimer->Enabled = true;

        DWORD dwTemp;

        //https://docs.microsoft.com/en-us/windows/win32/api/wininet/nf-wininet-internetreadfile

        // When running asynchronously, if a call to InternetReadFile does not result in a
        // completed transaction, it will return FALSE and a subsequent call to GetLastError
        // will return ERROR_IO_PENDING. When the transaction is completed the InternetStatusCallback
        // specified in a previous call to InternetSetStatusCallback will be called with
        // INTERNET_STATUS_REQUEST_COMPLETE.
        if (pInternetReadFile(m_hWebAddress, pBuf, DataSize, &dwTemp))
        {
          long br = (long)dwTemp;

          if (br == 0)
            break;

          //ShowMessage("Read " + String(br) + " from URL: \"" + String(sSource) + "\"");

          if (br)
          {
            long bw = FileWrite(m_fw, pBuf, br);

            if (br != bw)
              return -8;

            m_lTotalWritten += br;
          }
        }
        else
        {
          int iErr =  GetLastError();

          if (iErr != ERROR_INSUFFICIENT_BUFFER)
          {
            // ERROR_IO_PENDING
    #if DEBUG_ON
            MainForm->CWrite( "\r\nInternet error: " + String(iErr) + "\r\n");
    #endif
            return -9;
          }

          delete [] pBuf;
          DataSize += URLMOVE_BUFSIZE;
          pBuf = new Byte[DataSize];
        }
      }

      return m_lTotalWritten;
    }
    catch(...)
    {
      return -10;
    }
  }
  __finally
  {
    pTimer->Enabled = false;

    if (m_hWebAddress)
    {
      pInternetCloseHandle(m_hWebAddress);
      m_hWebAddress = NULL;
    }

    if (m_hWebConnect)
    {
      pInternetCloseHandle(m_hWebConnect);
      m_hWebConnect = NULL;
    }

    if (m_fw)
    {
      FileClose(m_fw);
      m_fw = NULL;
    }

    if (pBuf)
      delete [] pBuf;
  }

  return 0;
}
//---------------------------------------------------------------------------
// the constructor fires a timer event to start the file-copy
void __fastcall TMyUrlCopy::TimerEvent(TObject* Sender)
{
  pTimer->Enabled = false;

  Application->ProcessMessages();

  if (m_timerInterval == TIME_URLCOPY_INITIAL)
  {
    // MyUrlCopy, sets a timeout also - which will re-enter TimerEvent()
    // and take the TIME_URLCOPY_TIMEOUT path
    m_retCode = this->MyUrlCopy();

    // do a retry retry if error and not user-cancel
    if (m_retCode != -1 && m_retCode != 0)
    {
      if (MainForm->FailedToCopyList)
        MainForm->AddFailure(this->m_sSource, this->m_sDest, this->m_idx, this->m_list);

#if DEBUG_ON
      String sErr = "TMyUrlCopy::TimerEvent():retCode:" + String(m_retCode) + ":";
      switch(m_retCode)
      {
        case -2:
          sErr += "Bad LoadLibrary() procedure address!";
        break;

        case -3:
          sErr +=  "Can't open destination file: \"" + String(m_sDest) + "\"";
        break;

        case -4:
          sErr +=  "Connection Failed or Syntax error";
        break;

        case -5:
          sErr +=  "Unable to open internet URL: \"" + String(m_sSource) + "\"";
        break;

        case -6:
          sErr += "Can't allocate buffer memory!";
        break;

        case -7:
          sErr += "Timeout trying to read internet file!";
        break;

        case -8:
          sErr += "File-write error!";
        break;

        case -9:
          sErr += "Unknown internet error!";
        break;

        case -10:
          sErr +=  "Exception thrown opening internet URL";
        break;

        default:
          sErr += "Unknoen error!";
        break;
      }

      MainForm->CWrite( "\r\n" + sErr + "\r\n");
#endif
    }
    else // we're done! - set up to delete ourself!
    {
      // handle a cache-file write's special considerations...
      if (m_idx >= 0 && m_idx < m_plForm->Count)
      {
        TPlayerURL* p = (TPlayerURL*)m_plForm->CheckBox->Items->Objects[m_idx];
        if (p)
        {
          // was this a cache-write?
          if (p->cacheNumber == -1 && m_plForm)
          {
          // set the calling TPlaylistForm's
            p->cachePath = m_sDest;
            p->cacheNumber = m_plForm->CacheCount+1; // all cache #s must be non-zero!

            // delete the oldest file in the cache...
            MainForm->DeleteCacheFile(m_plForm);

            m_plForm->CacheCount++;

            // if count is 0 (unlikely) - we've exceeded the # files in a 32 bit int
            // of continuous playback... if so, just turn off caching and the files
            // (up to MAX_CACHE_FILES) will be deleted on exit along with the directory
            // containing them.
            if (m_plForm->CacheCount == 0)
               MainForm->CacheEnabled = false;
    #if DEBUG_ON
            MainForm->CWrite( "\r\nCopyUrls cache-write success! \"" + m_sDest + "\"\r\n");
    #endif
          }
          else
            p->cacheNumber = 0; // make sure we clear this from -1 (cache write in-progress)
        }
      }
    }

    // we're done! - set up to go away (poof!!!)
    m_timerInterval = TIME_URLCOPY_DESTROY;
    pTimer->Interval = m_timerInterval;
    pTimer->Enabled = true;
  }
  else if (m_timerInterval == TIME_URLCOPY_TIMEOUT)
  {
#if DEBUG_ON
    MainForm->CWrite( "\r\nTMyUrlCopy::TimerEvent(): 10-sec timeout, setting m_bTimeout (retry using CopyFile): \"" + m_sSource +"\"\r\n");
#endif
    // this will cleanly jog us out of a stuck read/write loop
    // thanks to Application->ProcessMessages() - so the __finally block will
    // execute in MyUrlCopy() and
    m_bTimeout = true;
  }
  else if (m_timerInterval == TIME_URLCOPY_DESTROY)
  {
    delete this; // delete ourself (TMyUrlCopy)
  }
#if DEBUG_ON
  else
  {
    MainForm->CWrite( "\r\nTMyUrlCopy::TimerEvent(): UNKNOWN TIMER EVENT!!!!!!\r\n");
  }
#endif

  Application->ProcessMessages();
}
//---------------------------------------------------------------------------

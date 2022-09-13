//---------------------------------------------------------------------------
#include <vcl.h>
#include "Main.h"
#pragma hdrstop

#include "CopyFiles.h"
#include "SMList.h"
#include "PlaylistHelper.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// Pass in a TPlaylistForm* as Owner!
__fastcall TMyFileCopy::TMyFileCopy(TComponent* Owner, String sSource, String sDest, int idx)
//__fastcall TMyFileCopy::TMyFileCopy(TPlaylistForm* f, String sSource, String sDest, int idx)
{
  if (!Owner || !MainForm->MyFileCopyList)
    return;

  m_plForm = (TPlaylistForm*)Owner;
  m_listId = (m_plForm == ListA) ? PLAYER_A_ID : PLAYER_B_ID;

  m_sSource = sSource;
  m_sDest = sDest;
  m_idx = idx;

  // file handles
  m_fr = NULL;
  m_fw = NULL;

  m_lTotalWritten = 0;
  m_bCancel = false;
  m_bTimeout = false;

  m_timerInterval = TIME_FILECOPY_INITIAL;
  pTimer = new TTimer(Owner);
  pTimer->Enabled = false;
  pTimer->Interval = m_timerInterval;
  pTimer->OnTimer = this->TimerEvent;

  // add this instance of TMyFileCopy to MainForm->MyFileCopyList
  MainForm->MyFileCopyList->Add(this);

  pTimer->Enabled = true; // 50ms and we start copy
}
//---------------------------------------------------------------------------
__fastcall TMyFileCopy::~TMyFileCopy(void)
{
  // close files if open
  try
  {
    // try to pump through a clean cancel...
    m_bCancel = true;
    Application->ProcessMessages();

    if (pTimer)
      delete pTimer;

    if (m_fr)
      FileClose(m_fr);

    if (m_fw)
    {
      FileClose(m_fw);

      // delete write-file if we are aborting uncleanly...
      if (FileExists(m_sDest))
        DeleteFile(m_sDest);
    }

    if (MainForm->MyFileCopyList)
    {
      // remove our pointer from the parent list
      int idx = MainForm->MyFileCopyList->IndexOf(this);
      if (idx >= 0)
      {
        MainForm->MyFileCopyList->Delete(idx);
#if DEBUG_ON
        MainForm->CWrite( "\r\nTMyFileCopy::~TMyFileCopy: Delete MainForm->MyFileCopyList idx=" + String(idx));
#endif
      }
    }
  }
  catch(...)
  {
#if DEBUG_ON
    MainForm->CWrite( "\r\nTMyFileCopy::~TMyFileCopy(): Exception in TMyFileCopy destructor!: \"" + m_sSource +"\"\r\n");
#endif
  }
}
//---------------------------------------------------------------------------
int __fastcall TMyFileCopy::MyFileCopy(void)
{
//  String sSaveAUrl, sSaveBUrl;
  Byte* pBuf = NULL;

  try
  {
    try
    {
      // this timeout will keep resetting in the file read/write loop...
      pTimer->Enabled = false;
      m_timerInterval = TIME_FILECOPY_TIMEOUT;

      long lFileLength = MainForm->MyGetFileSize(m_sSource);
      m_lTotalWritten = 0;

      m_bCancel = false;
      m_bTimeout = false;

// NOTE: don't need this since I discovered the problem was actually the file-sharing mode
// being opened for MyGetFileSize() in FormMain.cpp!
      // save Wmp URLs - we can't copy the files if Windows Media Player has them locked!!!
//      if (ListA->Wmp->URL == sSource)
//      {
//        sSaveAUrl = ListA->Wmp->URL;
//        ListA->Wmp->URL = "dummy.mp3";
//      }
//      if (ListB->Wmp->URL == sSource)
//      {
//        sSaveBUrl = ListB->Wmp->URL;
//        ListB->Wmp->URL = "dummy.mp3";
//      }

      if (m_fr)
        FileClose(m_fr);
      if (m_fw)
        FileClose(m_fw);

      // Open in binary/read-only mode
      m_fr = FileOpen(m_sSource.c_str(), fmOpenRead | fmShareDenyWrite);
      m_fw = FileCreate(m_sDest.c_str());

      if (m_fr == NULL || m_fw == NULL)
        return -2;

      int iBufSize = FILE_BUF_SIZE;
      pBuf = new Byte[iBufSize];

      if (!pBuf)
        return -3;

      for(;;)
      {
        // stop failsafe timeout
        pTimer->Enabled = false;

        Application->ProcessMessages();

        if (this->m_bCancel)
        {
#if DEBUG_ON
          MainForm->CWrite( "\r\nTMyFileCopy::MyFileCopy(): exiting loop due to m_bCancel set: \"" + m_sSource +"\"\r\n");
#endif
          return -1;
        }

        if (this->m_bTimeout)
        {
#if DEBUG_ON
          MainForm->CWrite( "\r\nTMyFileCopy::MyFileCopy(): exiting loop due to m_bTimeout set: \"" + m_sSource +"\"\r\n");
#endif
          return -4;
        }

        // restart the main timeout timer
        pTimer->Interval = m_timerInterval;
        pTimer->Enabled = true;

        long br = FileRead(m_fr, pBuf, iBufSize);

        if (br <= 0)
          break;

        long bw = FileWrite(m_fw, pBuf, br);

        if (br != bw)
          return -5; // write fail

        m_lTotalWritten += bw;

        if (lFileLength && m_lTotalWritten >= lFileLength)
          break;
      };
    }
    catch(...)
    {
      return -6;
    }
  }
  __finally
  {
    pTimer->Enabled = false;

    if (m_fr)
    {
      FileClose(m_fr);
      m_fr = NULL;
    }
    if (m_fw)
    {
      FileClose(m_fw);
      m_fw = NULL;
    }

    if (pBuf)
      delete [] pBuf;

    // restore saved Wmp URLs
//    if (sSaveAUrl.Length() > 0)
//      ListA->Wmp->URL = sSaveAUrl;
//    if (sSaveBUrl.Length() > 0)
//      ListB->Wmp->URL = sSaveBUrl;

    // clean up failure
    if (this->m_bCancel || this->m_bTimeout)
    {
      if (FileExists(m_sDest))
        DeleteFile(m_sDest);
    }
  }

  return 0;
}
//---------------------------------------------------------------------------
// the constructor fires a timer event to start the file-copy
void __fastcall TMyFileCopy::TimerEvent(TObject* Sender)
{
  pTimer->Enabled = false;

  Application->ProcessMessages();

  if (m_timerInterval == TIME_FILECOPY_INITIAL)
  {
    // MyFileCopy, sets a timeout also - which will re-enter TimerEvent()
    // and take the TIME_FILECOPY_TIMEOUT path
    m_retCode = this->MyFileCopy();

    // do a retry retry if error and not user-cancel
    if (m_retCode != -1 && m_retCode != 0)
    {
      // copy and overwrite (failed should have been deleted)
      // this will delete successfully copied files from the fail-list - then,
      // you can call ShowFailures and it should show nothing!
      if (!CopyFile(this->m_sSource.c_str(), this->m_sDest.c_str(), false))
        if (MainForm->FailedToCopyList)
          MainForm->AddFailure(this->m_sSource, this->m_sDest, this->m_idx, this->m_listId);

#if DEBUG_ON
      MainForm->CWrite( "\r\nTMyFileCopy::TimerEvent():retCode: " + String(m_retCode) +"\r\n");
#endif
    }
    else // we're done! - set up to delete ourself!
    {
      // need to add entry to a playlist's CacheList if the
      // cacheNumber is -1 (a flag indicating this is a cache-file write)
      if (m_plForm && m_idx >= 0 && m_idx < m_plForm->Count)
      {
        TPlayerURL* p = (TPlayerURL*)m_plForm->CheckBox->Items->Objects[m_idx];
        // check cachNumber for flag of "-1" which indicates a cache-
        // write is in-progress...
        if (p && p->cacheNumber == -1)
        {
          if (pFC->AddCacheEntry(m_plForm, m_sDest, m_idx))
            p->cacheNumber = m_plForm->CacheList->Count;
          else
            p->cacheNumber = 0;
        }
      }
    }

    // we're done! - set up to go away (poof!!!)
    m_timerInterval = TIME_FILECOPY_DESTROY;
    pTimer->Interval = m_timerInterval;
    pTimer->Enabled = true;
  }
  else if (m_timerInterval == TIME_FILECOPY_TIMEOUT)
  {
#if DEBUG_ON
    MainForm->CWrite( "\r\nTMyFileCopy::TimerEvent(): 10-sec timeout, setting m_bTimeout (retry using CopyFile): \"" + m_sSource +"\"\r\n");
#endif
    // this will cleanly jog us out of a stuck read/write loop
    // thanks to Application->ProcessMessages() - so the __finally block will
    // execute in MyFileCopy() and
    m_bTimeout = true;
  }
  else if (m_timerInterval == TIME_FILECOPY_DESTROY)
  {
    delete this; // delete ourself (TMyFileCopy)
  }
#if DEBUG_ON
  else
  {
    MainForm->CWrite( "\r\nTMyFileCopy::TimerEvent(): UNKNOWN TIMER EVENT!!!!!!\r\n");
  }
#endif

  Application->ProcessMessages();
}
//---------------------------------------------------------------------------
//long __fastcall TMyFileCopy::MyGetFileSize(long h)
//{
//  long len = FileSeek(h,0,2);
//  FileSeek(h,0,0);
//  return len;
//}
//---------------------------------------------------------------------------

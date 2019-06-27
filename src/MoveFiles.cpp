//---------------------------------------------------------------------------
#include <vcl.h>
#include "Main.h"
#pragma hdrstop

#include "MoveFiles.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// NOTE: TProgressForm creates and manages TMyFileCopy objects!
__fastcall TMyFileCopy::TMyFileCopy(TComponent* Owner, String sSource, String sDest, int idx, int list)
{
  if (!MainForm->MyFileCopyList)
    return;

  // add ourself's object-pointer to TList in the parent form (TProgress)
  MainForm->MyFileCopyList->Add(this);

  m_sSource = sSource;
  m_sDest = sDest;
  m_idx = idx;
  m_list = idx;

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
        MainForm->MyFileCopyList->Delete(idx);
    }
  }
  catch(...)
  {
    ShowMessage("Exception in TMyFileCopy destructor!");
  }
}
//---------------------------------------------------------------------------
int __fastcall TMyFileCopy::MyFileCopy(String sSource, String sDest, int idx, int list)
{
  String sSaveAUrl, sSaveBUrl;
  Byte* pBuf = NULL;

  try
  {
    try
    {
      pTimer->Enabled = false; // don't wand any timeouts while opening files

      long lFileLength = MainForm->MyGetFileSize(sSource);
      m_lTotalWritten = 0;

      m_timerInterval = TIME_FILECOPY_TIMEOUT;

      m_bCancel = false;
      m_bTimeout = false;

      // save Wmp URLs - we can't copy the files if Windows Media Player has them locked!!!
      if (ListA->Wmp->URL == sSource)
      {
        sSaveAUrl = ListA->Wmp->URL;
        ListA->Wmp->URL = "dummy.mp3";
      }
      if (ListB->Wmp->URL == sSource)
      {
        sSaveBUrl = ListB->Wmp->URL;
        ListB->Wmp->URL = "dummy.mp3";
      }

      if (m_fr)
        FileClose(m_fr);
      if (m_fw)
        FileClose(m_fw);

      // Open in binary/read-only mode
      m_fr = FileOpen(sSource.c_str(), fmOpenRead | fmShareDenyWrite);
//      m_fr = FileOpen(sSource.c_str(), fmOpenRead | fmShareCompat);
      m_fw = FileCreate(sDest.c_str());

      if (m_fr == NULL || m_fw == NULL)
        return -2;

      int iBufSize = FILE_BUF_SIZE;
      pBuf = new Byte[iBufSize];

      if (!pBuf)
        return -3;

      for(;;)
      {
        Application->ProcessMessages();

        // start a failsafe timeout between 64K buffer-writes
        pTimer->Enabled = false;
        pTimer->Interval = m_timerInterval;
        pTimer->Enabled = true;

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
      return -5;
    }
  }
  __finally
  {
    pTimer->Enabled = false; // cancel timeout

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
    if (sSaveAUrl.Length() > 0)
      ListA->Wmp->URL = sSaveAUrl;
    if (sSaveBUrl.Length() > 0)
      ListB->Wmp->URL = sSaveBUrl;

    // clean up failure
    if (this->m_bCancel || this->m_bTimeout)
    {
      if (FileExists(m_sDest))
        DeleteFile(m_sDest);

      // this is kind of ballsy... daring... restarting the file-copy
      // after it timed out !!!!!!!!!!!!!!!!!!!
//      if (this->m_bTimeout && MainForm && FileExists(m_sSource))
//      {
//#if DEBUG_ON
//    MainForm->CWrite( "\r\nTMyFileCopy::MyFileCopy(): RETRY AFTER TIMEOUT OF: \"" +
//                    m_sSource + "\"\r\nTO:\"" + m_sDest + "\"\r\n");
//#endif
//        MainForm->MyFileCopy(m_sSource, m_sDest);
//      }
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

  // m_timerInterval
  if (pTimer->Interval == TIME_FILECOPY_TIMEOUT)
  {
#if DEBUG_ON
    MainForm->CWrite( "\r\nTMyFileCopy::TimerEvent(): setting m_bTimeout 10-sec timeout: \"" + m_sSource +"\"\r\n");
#endif

    if (MainForm->FailedToCopyList)
      MainForm->AddFailure(this->m_sSource, this->m_sDest, this->m_idx, this->m_list);

    m_bTimeout = true;
    Application->ProcessMessages();
  }
  else
  {
    m_retCode = this->MyFileCopy(this->m_sSource, this->m_sDest, this->m_idx, this->m_list);

#if DEBUG_ON
    if (m_retCode != 0)
      MainForm->CWrite( "\r\nTMyFileCopy::TimerEvent():retCode: " + String(m_retCode) +"\r\n");
#endif

    delete this; // delete ourself (TMyFileCopy)
  }
}
//---------------------------------------------------------------------------
//long __fastcall TMyFileCopy::MyGetFileSize(long h)
//{
//  long len = FileSeek(h,0,2);
//  FileSeek(h,0,0);
//  return len;
//}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef MoveFilesH
#define MoveFilesH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>

#define FILE_BUF_SIZE 65536

// 50ms after instantiating a TMyFileCopy, a TTimer fires and calls MyFileCopy()
#define TIME_FILECOPY_INITIAL 50
#define TIME_FILECOPY_TIMEOUT 10000 // timeout
#define MAX_TMyFileCopy_OBJECTS 10
//---------------------------------------------------------------------------
class TMyFileCopy
{
  private:

    //long __fastcall MyGetFileSize(long h);

    long m_lTotalWritten;
    long m_fr, m_fw; // file handles
    String m_sSource, m_sDest;
    int m_retCode, m_idx, m_list;
    int m_timerInterval;
    bool m_bCancel, m_bTimeout;

    TTimer* pTimer;

  protected:
    void __fastcall TimerEvent(TObject* Sender);

  public:

    __fastcall TMyFileCopy(TComponent* Owner, String sSource, String sDest, int idx, int list);
    __fastcall ~TMyFileCopy(void);

    int __fastcall MyFileCopy(String sSource, String sDest, int idx, int list);

  __property int RetCode = {read = m_retCode};
};
//---------------------------------------------------------------------------
#endif


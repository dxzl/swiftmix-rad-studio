//---------------------------------------------------------------------------
#ifndef CopyUrlsH
#define CopyUrlsH
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>

#define URLMOVE_BUFSIZE 65536

// 50ms after instantiating a TMyUrlCopy, a TTimer fires and calls MyUrlCopy()
// (TODO: need timer handler "mode" constants rather than checking for "different m_timerInterval values!)
#define TIME_URLCOPY_INITIAL 50
#define TIME_URLCOPY_DESTROY 300
#define TIME_URLCOPY_TIMEOUT 10000 // timeout
//---------------------------------------------------------------------------
class TPlaylistForm; // forward reference

typedef HINTERNET WINAPI(*tInternetOpen)(LPCWSTR, DWORD, LPCWSTR, LPCWSTR, DWORD);
typedef HINTERNET WINAPI(*tInternetOpenUrl)(HINTERNET, LPCWSTR, LPCWSTR, DWORD, DWORD, DWORD_PTR);
typedef BOOL WINAPI(*tInternetCloseHandle)(HINTERNET);
typedef BOOL WINAPI(*tInternetReadFile)(HINTERNET, LPVOID, DWORD, LPDWORD);

class TMyUrlCopy
{
  private:

    //long __fastcall MyGetFileSize(long h);

    long m_lTotalWritten;
    long m_fw; // file handles
    HINTERNET m_hWebConnect, m_hWebAddress;
    HINSTANCE m_hDll;
    String m_sSource, m_sDest;
    int m_retCode, m_idx, m_list;
    int m_timerInterval;
    bool m_bCancel, m_bTimeout;
    TPlaylistForm* m_plForm;

    TTimer* pTimer;

    tInternetOpen pInternetOpen; // InternetOpenW 0x007A8F0
    tInternetOpenUrl pInternetOpenUrl; // InternetOpenUrlW 0x0013CA40
    tInternetCloseHandle pInternetCloseHandle; // InternetCloseHandle 0x0005ECB0
    tInternetReadFile pInternetReadFile; // InternetReadFile 0x0007AE80

  protected:
    void __fastcall TimerEvent(TObject* Sender);

  public:

    __fastcall TMyUrlCopy(TComponent* Owner, String sSource, String sDest, int idx);
    __fastcall ~TMyUrlCopy(void);

    long __fastcall MyUrlCopy(void);

  __property int RetCode = {read = m_retCode};
  __property bool Cancel = {read = m_bCancel, write = m_bCancel};
};
//---------------------------------------------------------------------------
#endif


//---------------------------------------------------------------------------
#ifndef FormDirDlgH
#define FormDirDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------

#define BIF_NEWDIALOGSTYLE (0x00000040)
#define BIF_SHAREABLE (0x00008000)

#define CSIDL_MYMUSIC 13
//---------------------------------------------------------------------------
class TDirDlgForm : public TForm
{
__published:	// IDE-managed Components
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
  bool FAutoScroll;
  HWND FDlgHandle;
protected:
  static INT CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM pData);
  static BOOL CALLBACK FindTreeViewCallback(HWND hwnd, LPARAM lParam);
public:
  WideString __fastcall Execute(int nCSIDL);

  __property HWND DlgHandle = {read = FDlgHandle, write = FDlgHandle};
  __property bool AutoScroll = {read = FAutoScroll, write = FAutoScroll};
};
//---------------------------------------------------------------------------
extern PACKAGE TDirDlgForm *DirDlgForm;
//---------------------------------------------------------------------------
#endif


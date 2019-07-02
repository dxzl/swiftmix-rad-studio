//---------------------------------------------------------------------------
#ifndef FormDirDlgH
#define FormDirDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
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
  String __fastcall Execute(int nCSIDL);

  __property HWND DlgHandle = {read = FDlgHandle, write = FDlgHandle};
  __property bool AutoScroll = {read = FAutoScroll, write = FAutoScroll};
};
//---------------------------------------------------------------------------
#endif


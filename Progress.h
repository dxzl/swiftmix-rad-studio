//---------------------------------------------------------------------------
#ifndef ProgressH
#define ProgressH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TProgressForm : public TForm
{
__published:	// IDE-managed Components
  TProgressBar *ProgressBar;
  TLabel *Label;
protected:
  int __fastcall GetProgressPosition(void);
  void __fastcall SetProgressPosition(int Value);

private:	// User declarations

  int FMaxVal;

public:		// User declarations
  __fastcall TProgressForm(TComponent* Owner);

  static void __fastcall Init(int maxVal);
  static void __fastcall Init(int maxVal, int minLimit);
  static void __fastcall Move(int newVal);
  static void __fastcall UnInit(void);

  __property int MaxVal = {read = FMaxVal, write = FMaxVal};
  __property int Position = {read = GetProgressPosition, write = SetProgressPosition};
};
//---------------------------------------------------------------------------
extern PACKAGE TProgressForm *ProgressForm;
//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
#ifndef ProgressH
#define ProgressH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>

#define DEFAULT_MIN 25 // progress bar shows up only if >= 500 items to process

#define PROGRESS_MODE_NORMAL     0
#define PROGRESS_MODE_CUMULATIVE 1
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class TProgressVars
{
  public:
    int Position;
    int ExpectedIterations;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class TProgressForm : public TForm
{
__published:	// IDE-managed Components
  TProgressBar *ProgressBar;
  TLabel *Label;
  TButton *ButtonCancel;
  void __fastcall FormDestroy(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall ButtonCancelClick(TObject *Sender);
protected:
  int __fastcall GetDepth(void);
  int __fastcall GetProgressPosition(void);
  int __fastcall GetExpectedIterations(void);
  void __fastcall SetProgressPosition(int Value);
  bool __fastcall GetShowCancelButton(void);
  void __fastcall SetShowCancelButton(int Value);

private:	// User declarations

  int FTotalExpectedIterations, FCumulativeIterations, FProgressMode;
  bool FCanceled;
  TList* pTProgressVarsList;

public:		// User declarations
  __fastcall TProgressForm(TComponent* Owner);

  int __fastcall Init(int maxIterations);
  int __fastcall Init(int maxIterations, int minLimit);
  bool __fastcall Move(int newVal);
  void __fastcall UnInit(bool bReset=false);

  __property int ProgressMode = {read = FProgressMode, write = FProgressMode};
  __property int Position = {read = GetProgressPosition, write = SetProgressPosition};
  __property int ExpectedIterations = {read = GetExpectedIterations};
  __property int Depth = {read = GetDepth};
  __property bool ShowCancelButton = {read = GetShowCancelButton, write = SetShowCancelButton};
  __property bool Canceled = {read = FCanceled};

  // have to set this before a sequence of Init() if PROGRESS_MODE_CUMULATIVE!
  __property int TotalExpectedIterations = {read = FTotalExpectedIterations, write = FTotalExpectedIterations};
};
//---------------------------------------------------------------------------
#endif

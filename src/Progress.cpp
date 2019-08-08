//---------------------------------------------------------------------------
#include <vcl.h>
#include "Main.h"
#pragma hdrstop

#include "Progress.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
// TProgressForm class methods - TMyFileCopy class methods are at the end...
// We create a TProgressForm for each playlist, ListA and ListB!
//---------------------------------------------------------------------------
__fastcall TProgressForm::TProgressForm(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::FormCreate(TObject *Sender)
{
  this->Color = TColor(0xF5CFB8);
  FTotalExpectedIterations = 1;
  FCumulativeIterations = 0;
  FProgressMode = PROGRESS_MODE_NORMAL; // other option is PROGRESS_MODE_CUMULATIVE

  // TProgressForm::Init() will creates a new copy of TProgressVars and stores its
  // object pointer TProgressVarsList
  pTProgressVarsList = new TList();

  ProgressBar->Min = 0;
  ProgressBar->Max = 100; // 100 percent complete
  ShowCancelButton = true;
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::FormDestroy(TObject *Sender)
{
  if (pTProgressVarsList)
  {
    if (pTProgressVarsList->Count)
    {
      // have to go backward...
      while(pTProgressVarsList->Count)
      {
        TProgressVars* p = (TProgressVars*)pTProgressVarsList->Items[0];

        if (p)
          delete p;

        pTProgressVarsList->Delete(0);
      }
    }
    delete pTProgressVarsList;
  }
}
//---------------------------------------------------------------------------
// property getter
int __fastcall TProgressForm::GetDepth(void)
{
  if (pTProgressVarsList)
    return pTProgressVarsList->Count;
  return 0;
}
//---------------------------------------------------------------------------
// property getter
int __fastcall TProgressForm::GetProgressPosition(void)
{
  return this->ProgressBar->Position;
}
//---------------------------------------------------------------------------
// property setter
void __fastcall TProgressForm::SetProgressPosition(int Value)
{
  this->ProgressBar->Position = Value;
}
//---------------------------------------------------------------------------
// property getter
bool __fastcall TProgressForm::GetShowCancelButton(void)
{
  return this->ButtonCancel->Visible;
}
//---------------------------------------------------------------------------
// property setter
void __fastcall TProgressForm::SetShowCancelButton(int Value)
{
  FCanceled = false;
  this->ButtonCancel->Enabled = Value;
  this->ButtonCancel->Visible = Value;
}
//---------------------------------------------------------------------------
// return 1 id error to avoid divide by zero error in Move
int __fastcall TProgressForm::GetExpectedIterations(void)
{
  if (!pTProgressVarsList || !pTProgressVarsList->Count)
    return 1;

  TProgressVars* p = (TProgressVars*)pTProgressVarsList->Items[pTProgressVarsList->Count-1];

  int mi = p->ExpectedIterations;

  if (mi == 0)
    return 1;

  return mi;
}
//---------------------------------------------------------------------------
// returns the Canceled flag
bool __fastcall TProgressForm::Move(int Value)
{
  // percent-complete
  // have to get most recently added CurrentMaxIterations!
  int mi, num;
  if (FProgressMode == PROGRESS_MODE_CUMULATIVE)
  {
    num = FCumulativeIterations+Value;
    mi = this->TotalExpectedIterations;
  }
  else
  {
    num = Value;
    mi = this->ExpectedIterations;
  }

  if (mi == 0)
    return false; // avoid divide by zero

  int iNewPos = 100*num/mi;

  // MUST check position or GUI processing is very S L O W!
  if (this->ProgressBar->Position != iNewPos)
    this->ProgressBar->Position = iNewPos;

  Application->ProcessMessages();

  if (Application->Terminated || (int)GetAsyncKeyState(VK_ESCAPE) < 0)
    this->FCanceled = true;

  return this->FCanceled;
}
//---------------------------------------------------------------------------
// overloaded...

// maxVal is the total # of iterations in a loop
int __fastcall TProgressForm::Init(int maxIterations)
{
  return Init(maxIterations, DEFAULT_MIN);
}

// maxVal is the total # of iterations in a loop
// maxVal must be >= minLimit for the progress bar to be shown...
int __fastcall TProgressForm::Init(int maxIterations, int minLimit)
{
  if (maxIterations > 0)
  {
    // save previous values in list
    TProgressVars* p = new TProgressVars();
    p->Position = ProgressBar->Position;
    p->ExpectedIterations = maxIterations;
    FCumulativeIterations = 0;
    pTProgressVarsList->Add((void*)p);

    // init new progress-bar values
    this->ProgressBar->Position = 0;

    // don't show the bar unless we have a worthwhile number of iterations
    if ((maxIterations >= minLimit || FProgressMode == PROGRESS_MODE_CUMULATIVE) && !this->Visible)
      this->Visible = true;

    FCanceled = false;
  }

  return pTProgressVarsList->Count;
}
//---------------------------------------------------------------------------
// bReset defaults false
void __fastcall TProgressForm::UnInit(bool bReset)
{
  if (pTProgressVarsList)
  {
    if (pTProgressVarsList->Count)
    {
      // restore position from previous level
      TProgressVars* p = (TProgressVars*)pTProgressVarsList->Items[pTProgressVarsList->Count-1];

      if (p)
      {
        if (FProgressMode == PROGRESS_MODE_CUMULATIVE)
          FCumulativeIterations += p->ExpectedIterations;
        else
          FCumulativeIterations = 0;

        ProgressBar->Position = p->Position;

        // delete entire list if bReset; otherwise, only delete
        // the most recent (last) entry
        int iMin = bReset ? 0 : pTProgressVarsList->Count-1;
        for(int ii = pTProgressVarsList->Count-1; ii >= iMin; ii--)
        {
          TProgressVars* p = (TProgressVars*)pTProgressVarsList->Items[ii];
          if (p)
            delete p;
          pTProgressVarsList->Delete(ii);
        }
      }
    }

    this->Visible = pTProgressVarsList->Count ? true : false;
  }
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::ButtonCancelClick(TObject *Sender)
{
  // Cancel
  FCanceled = true;
  ButtonCancel->Enabled = false;
  ShowMessage("Cancel in-progress...");
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#include <vcl.h>
#include "Main.h"
#pragma hdrstop

#include "Progress.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TProgressForm *ProgressForm = NULL;

//---------------------------------------------------------------------------
// TProgressForm class methods - TMyFileCopy class methods are at the end...
//---------------------------------------------------------------------------
__fastcall TProgressForm::TProgressForm(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::FormCreate(TObject *Sender)
{
  this->Color = TColor(0xF5CFB8);
  FCanceled = false;
  FCurrentMaxIterations = 0;
  FCumulativeIterations = 0;
  FProgressMode = PROGRESS_MODE_NORMAL; // other option is PROGRESS_MODE_CUMULATIVE

  // TProgressForm::Init() will creates a new copy of TProgressVars and stores its
  // object pointer TProgressVarsList
  pTProgressVarsList = new TList();

  ProgressBar->Min = 0;
  ProgressBar->Max = 100; // 100 percent complete

// this is'nt finished as yet... may never be - hard to predict the future!
//  FProgressMode = PROGRESS_MODE_CUMULATIVE; // other option is PROGRESS_MODE_NORMAL
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::FormDestroy(TObject *Sender)
{
  if (pTProgressVarsList)
  {
    if (pTProgressVarsList->Count)
    {
      // have to go backward...
      for(int ii = pTProgressVarsList->Count-1; ii >= 0 ; ii--)
      {
        TProgressVars* p = (TProgressVars*)pTProgressVarsList->Items[ii];
        if (p)
          delete p;
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
void __fastcall TProgressForm::Move(int Value)
{
  int iNewPos;

  // percent-complete
  iNewPos = 100*Value/FCurrentMaxIterations;

  // MUST check position or GUI processing is very S L O W!
  if (this->ProgressBar->Position != iNewPos)
  {
    this->ProgressBar->Position = iNewPos;
    Application->ProcessMessages();
  }
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
  // save previous values in list
  TProgressVars* p = new TProgressVars();
  p->mOldPosition = ProgressBar->Position;
  p->mOldMaxIterations = FCurrentMaxIterations;
  pTProgressVarsList->Add((void*)p);

  // init new progress-bar values
  ProgressBar->Position = 0;
  FCurrentMaxIterations = maxIterations;

  // don't show the bar unless we have a worthwhile number of iterations
  if (maxIterations >= minLimit)
    this->Visible = true;

  Application->ProcessMessages();
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
      // delete entire list if bReset; otherwise, only delete
      // the most recent (last) entry
      int iMin = bReset ? 0 : pTProgressVarsList->Count-1;
      for(int ii = pTProgressVarsList->Count-1; ii >= iMin; ii--)
      {
        TProgressVars* p = (TProgressVars*)pTProgressVarsList->Items[ii];
        if (p)
        {
          FCumulativeIterations += p->mOldMaxIterations; // for PROGRESS_MODE_CUMULATIVE
          delete p;
        }
        pTProgressVarsList->Delete(ii);
      }

      // restore position from previous level
      if (pTProgressVarsList->Count)
      {
        int idx = pTProgressVarsList->Count-1;
        TProgressVars* p = (TProgressVars*)pTProgressVarsList->Items[idx];
        if (p)
        {
          ProgressBar->Position = p->mOldPosition;
          FCurrentMaxIterations = p->mOldMaxIterations;
        }
      }
      else // finished
      {
        FCumulativeIterations = 0;
        this->Visible = false;
      }
    }
    else
      this->Visible = false;
  }
  else
    this->Visible = false;
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


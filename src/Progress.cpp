//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Progress.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TProgressForm *ProgressForm = NULL;
//---------------------------------------------------------------------------
__fastcall TProgressForm::TProgressForm(TComponent* Owner)
  : TForm(Owner)
{
  Color = TColor(0xF5CFB8);
  FMaxVal = 100;
}
//---------------------------------------------------------------------------
int __fastcall TProgressForm::GetProgressPosition(void)
{
  if (ProgressForm == NULL) return 0;
  return ProgressForm->ProgressBar->Position;
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::SetProgressPosition(int Value)
{
  if (ProgressForm != NULL)
    ProgressForm->ProgressBar->Position = 100*Value/FMaxVal;
}
//---------------------------------------------------------------------------
// static
void __fastcall TProgressForm::Init(int maxVal, int minLimit)
{
  if (maxVal == 0 || maxVal < minLimit)
    return; // don't bother if < minLimit

  if (ProgressForm == NULL)
    Application->CreateForm(__classid(TProgressForm), &ProgressForm);

  ProgressForm->Position = 0;

  ProgressForm->MaxVal = maxVal;
}

void __fastcall TProgressForm::Init(int maxVal)
{
  Init(maxVal, 100);
}
//---------------------------------------------------------------------------
// static
void __fastcall TProgressForm::UnInit(void)
{
  if (ProgressForm != NULL)
  {
    ProgressForm->Release();
    ProgressForm = NULL;
  }
}
//---------------------------------------------------------------------------
// static
void __fastcall TProgressForm::Move(int newVal)
{
  if (ProgressForm != NULL)
    ProgressForm->Position = newVal;

  Application->ProcessMessages();
}
//---------------------------------------------------------------------------


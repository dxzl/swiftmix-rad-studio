//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormSearchUncheck.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSearchUncheckForm *SearchUncheckForm;
//---------------------------------------------------------------------------
__fastcall TSearchUncheckForm::TSearchUncheckForm(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TSearchUncheckForm::FormCreate(TObject *Sender)
{
  Edit1->Text = FText;
}
//---------------------------------------------------------------------------
void __fastcall TSearchUncheckForm::ButtonOkClick(TObject *Sender)
{
  if (Edit1->Text.IsEmpty())
  {
    ShowMessage("Please enter text to search for or press Cancel!");
    return;
  }
  FText = Edit1->Text;
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TSearchUncheckForm::ButtonCancelClick(TObject *Sender)
{
  FText = "";
  Close();
}
//---------------------------------------------------------------------------

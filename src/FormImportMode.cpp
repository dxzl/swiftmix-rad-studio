//---------------------------------------------------------------------------
#include <vcl.h>
#include "Main.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TImportModeForm *ImportModeForm;
//---------------------------------------------------------------------------
__fastcall TImportModeForm::TImportModeForm(TComponent* Owner)
  : TForm(Owner)
{
  FMode = IMPORT_MODE_NONE; // undefined
}
//---------------------------------------------------------------------------
void __fastcall TImportModeForm::FormDestroy(TObject *Sender)
{
#if DEBUG_ON
  MainForm->CWrite( "\r\nFormDestroy() in TImportModeForm()!\r\n");
#endif
}
//---------------------------------------------------------------------------
void __fastcall TImportModeForm::FormCreate(TObject *Sender)
{
  Left = MainForm->Left + MainForm->Width/2 - Width/2;
  Top = MainForm->Top + MainForm->Height/2 - Height/2;
  OurTitle = "Import Mode";
}
//---------------------------------------------------------------------------
void __fastcall TImportModeForm::FormShow(TObject *Sender)
{
  if (FMode == IMPORT_MODE_NONE)
    FMode = IMPORT_MODE_AUTO;

  RadioGroup->ItemIndex = FMode;
}
//---------------------------------------------------------------------------
void __fastcall TImportModeForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
  FMode = RadioGroup->ItemIndex;
}
//---------------------------------------------------------------------------
void __fastcall TImportModeForm::ButtonCancelClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TImportModeForm::SetTitle(String sTitle)
{
  FTitle = sTitle;
  this->Caption = FTitle;
}
//---------------------------------------------------------------------------


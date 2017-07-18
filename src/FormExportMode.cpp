//---------------------------------------------------------------------------
#include <vcl.h>
#include "Main.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TExportModeForm *ExportModeForm;
//---------------------------------------------------------------------------
__fastcall TExportModeForm::TExportModeForm(TComponent* Owner)
  : TForm(Owner)
{
  FMode = EXPORT_PATH_NONE; // undefined
  FEncoding = EXPORT_MODE_NONE; // undefined
  FWriteBOM = false;
  FUncPathFmt = false;
  FFile = "";
}
//---------------------------------------------------------------------------
void __fastcall TExportModeForm::FormCreate(TObject *Sender)
{
  Left = MainForm->Left + MainForm->Width/2 - Width/2;
  Top = MainForm->Top + MainForm->Height/2 - Height/2;
}
//---------------------------------------------------------------------------
void __fastcall TExportModeForm::FormDestroy(TObject *Sender)
{
#if DEBUG_ON
  MainForm->CWrite( "\r\nFormDestroy() in TIExportModeForm()!\r\n");
#endif
}
//---------------------------------------------------------------------------
void __fastcall TExportModeForm::FormShow(TObject *Sender)
{
  if (FMode == EXPORT_PATH_NONE)
    FMode = EXPORT_PATH_ABSOLUTE;

  RGPathType->ItemIndex = FMode;

  this->Caption = FFile; // this will handle UTF-8 (with special handler WMSetText())
  FileLabel->Caption = FTitle; // ANSI only!

  Ext = ExtractFileExt(FFile).LowerCase();

  //  "All Files (*.*)|*.*|" + // 1
  //  "Windows Media (wpl)|*.wpl|" +
  //  "MPEG UTF-8 (m3u8)|*.m3u8|" +
  //  "MPEG ANSI (m3u)|*.m3u|" +
  //  "Adv Stream XML (asx)|*.asx|" +
  //  "XML Shareable (xspf)|*.xspf|" +
  //  "Win Audio XML (wax)|*.wax|" +
  //  "Windows XML (wmx)|*.wmx|" +
  //  "Winamp (pls)|*.pls|" +
  //  "Text (txt)|*.txt"; // 11
  FEncoding = (Ext == ".m3u" || Ext == ".pls" || Ext == ".txt") ? EXPORT_MODE_ANSI : EXPORT_MODE_UTF8;

  RGEncoding->ItemIndex = FEncoding;

  UncPathCheckBox->Enabled = (Ext == ".xspf") ? false : true; // xspf no choice allowed
  UncPathCheckBox->Checked = (Ext == ".xspf") ? true : false;

  WriteBOMCheckBox->Enabled = true;
  WriteBOMCheckBox->Checked = (Ext == ".wpl" || Ext == ".m3u" ||
    Ext == ".m3u8" || Ext == ".pls" || Ext == ".txt") ? false : true;
}
//---------------------------------------------------------------------------
void __fastcall TExportModeForm::FormClose(TObject *Sender, TCloseAction &Action)
{
  FMode = RGPathType->ItemIndex;
  FEncoding = RGEncoding->ItemIndex;
  FUncPathFmt = UncPathCheckBox->Checked;
  FWriteBOM = WriteBOMCheckBox->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TExportModeForm::CancelClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------


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
  FMode = -1; // undefined
  FSaveAsUtf8 = false;
  FUncPathFormat = false;
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
  if (FMode == -1)
    FMode = EXPORT_PATH_ABSOLUTE;

  RadioGroup->ItemIndex = FMode;

  this->Caption = FFile; // this will handle UTF-8 (with special handler WMSetText())
  FileLabel->Caption = FTitle; // ANSI only!

  Ext = ExtractFileExt(FFile).LowerCase();
  // "Windows Media Playlist (*.wpl)|*.wpl|"
  // "M3U8 (*.m3u8)|*.m3u8|"
  // "M3U (*.m3u)|*.m3u|"
  // "ASX (*.asx)|*.asx|"
  // "XSPF (*.xspf)|*.xspf|"
  // "Windows (*.wax)|*.wax|"
  // "Winamp (*.pls)|*.pls|"
  // "Text (*.txt)|*.txt|"
  // "All Files (*.*)|*.*";
  SaveAsUtf8CheckBox->Enabled = (Ext == ".m3u8" || Ext == ".m3u") ? false : true;
  SaveAsUtf8CheckBox->Checked = (Ext == ".m3u8" || Ext == ".asx" ||
         Ext == ".wpl" || Ext == ".wax" || Ext == ".xspf") ? true : false;

  UncPathCheckBox->Enabled = (Ext == ".xspf" || Ext == ".wpl") ? false : true;
  UncPathCheckBox->Checked = false;
}
//---------------------------------------------------------------------------
void __fastcall TExportModeForm::FormClose(TObject *Sender, TCloseAction &Action)
{
  FMode = RadioGroup->ItemIndex;
  FSaveAsUtf8 = SaveAsUtf8CheckBox->Checked;
  FUncPathFormat = UncPathCheckBox->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TExportModeForm::CancelClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TExportModeForm::WMSetText(TWMSetText &Msg)
// Allow Unicode Window Caption (the Caption property is ANSI-only)
{
  if (Msg.Text != NULL)
  {
    WideString w = MainForm->Utf8ToWide(String(Msg.Text));
    DefWindowProcW(this->Handle, Msg.Msg, 0, (LPARAM)w.c_bstr());
    Msg.Result = TRUE;
  }
}
//---------------------------------------------------------------------------

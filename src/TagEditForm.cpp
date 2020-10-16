//---------------------------------------------------------------------------
#include <vcl.h>
#include "Main.h"
#pragma hdrstop

#include "TagEditForm.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TFormTags::TFormTags(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormTags::FormCreate(TObject *Sender)
{
  f = NULL;
  ReadOnly = false;
  TagPath = "";

  LabelCredit->Caption = "(Thanks to Daniel Gaussmann for his tag-library "
                 "\"Audio Werkzeuge Bibliothek\" (AWB)";
}
//---------------------------------------------------------------------------
// How to use:
// TFormTags* pMyTags = new TFormTags();
// pMyTags->ShowTags(TPlayerURL* p);
// pMyTags->Show();
void __fastcall TFormTags::ShowTags(TPlayerURL* p)
{
  if (f) delete f;
  if (!p) return;

  try
  {
    f = new TGeneralAudioFile(p->path);
    if (f)
    {
      LabelPath->Caption = p->path;
      EdtTitle->Text  = f->Title;
      EdtArtist->Text = f->Artist;
      EdtAlbum->Text  = f->Album;
      EdtGenre->Text  = f->Genre;
      EdtYear->Text   = f->Year;
      EdtTrack->Text  = f->Track;
      Memo1->Clear();
      Memo1->Lines->Add(Format("Type:      %s", OPENARRAY(TVarRec, (f->FileTypeName))));
      Memo1->Lines->Add(Format("FileSize   %d Bytes", OPENARRAY(TVarRec, (f->FileSize))));
      Memo1->Lines->Add(Format("Duration   %d sec", OPENARRAY(TVarRec, (f->Duration))));
      Memo1->Lines->Add(Format("Btrate     %d kBit/s", OPENARRAY(TVarRec, (f->Bitrate/1000))));
      Memo1->Lines->Add(Format("Samplerate %d Hz", OPENARRAY(TVarRec, (f->Samplerate))));
      Memo1->Lines->Add(Format("Channels:  %d", OPENARRAY(TVarRec, (f->Channels))));
      this->TagPath = p->path;
      String sExt = ExtractFileExt(p->path).LowerCase();
      this->ReadOnly = sExt == ".wma" || sExt == ".wav";
      BtnSave->Enabled = !this->ReadOnly;
    }
  }
  __finally
  {
   if (f)
   {
     Show();
   }
   else
   {
     Release();
   }
  }
}
//---------------------------------------------------------------------------
//String __fastcall TUtils::GoFormat(String FormatStr, int Parm1)
//{
//  return(Format(FormatStr, ARRAYOFCONST((Parm1))));
//}
//---------------------------------------------------------------------------
void __fastcall TFormTags::FormClose(TObject *Sender, TCloseAction &Action)
{
  if (f) delete (f);
  f = NULL;
  Release();
}
//---------------------------------------------------------------------------
void __fastcall TFormTags::BtnSaveClick(TObject *Sender)
{
  TGeneralAudioFile* f = NULL;

  try
  {
    f = new TGeneralAudioFile(this->TagPath);
    if (f)
    {
      f->Title = EdtTitle->Text;
      f->Artist = EdtArtist->Text;
      f->Album = EdtAlbum->Text;
      f->Genre = EdtGenre->Text;
      f->Year = EdtYear->Text;
      f->Track = EdtTrack->Text;
      f->UpdateFile();
    }
  }
  __finally
  {
    if (f) delete f;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormTags::ButtonPasteTagsClick(TObject *Sender)
{
  try
  {
    Clipboard()->Open();
    String sTags = Clipboard()->AsText;
    int retCode = SetTagsFromString(sTags);
    if (retCode == -1)
      ShowMessage("Select \"Copy Tags\" before you can \"Paste Tags\"!");
    else if (retCode < 0)
      ShowMessage("Unable to retreive tags from clipboard!");
    Clipboard()->Close();
  }
  catch(...)
  {
    ShowMessage("Unable to retreive tags from clipboard!");
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormTags::ButtonCopyTagsClick(TObject *Sender)
{
  try
  {
    Clipboard()->Open();
    Clipboard()->Clear();
    String sTags = GetTagsAsString();
    if ((sTags.Length() > 0))
      Clipboard()->AsText = sTags;
    else
      ShowMessage("Unable to copy tags to clipboard!");
    Clipboard()->Close();
  }
  catch(...)
  {
    ShowMessage("Unable to copy tags to clipboard!");
  }
}
//---------------------------------------------------------------------------
int __fastcall TFormTags::SetTagsFromString(String sIn)
{
  try
  {
    String sl[TOKENCOUNT];

    int ii = 0; // this acts as a failsafe exit from loop...
    for(; ii < TOKENCOUNT; ii++)
    {
      int len = sIn.Length();
      if (len == 0) break;
      int pos = sIn.LastDelimiter(","); // get "ENDSMTAGS" terminator index
      String sTok;
      if (pos >= 0)
      {
        sTok = sIn.SubString(pos+1, len-pos);
        sIn = sIn.SubString(1, pos-1);
        sl[ii] = sTok;
      }
      else
        break;
      if (pos == 0) break; // last token added
    }
    if (ii != TOKENCOUNT-1)
      return -1;

    String sIdentifier = StripTab(sl[0]);
    if (sIdentifier != "ENDSMTAGS")
      return -2;
    EdtTitle->Text = StripTab(sl[6]);
    EdtArtist->Text = StripTab(sl[5]);
    EdtAlbum->Text = StripTab(sl[4]);
    EdtGenre->Text = StripTab(sl[3]);
    EdtYear->Text = StripTab(sl[2]);
    EdtTrack->Text = StripTab(sl[1]);
  }
  catch(...)
  {
    return -3;
  }
  return 0;
}
//---------------------------------------------------------------------------
// strips quotes from quoted string
// (NOTE: allow for string having extraneous leading/trailing blanks)
String __fastcall TFormTags::StripTab(String sIn)
{
  TReplaceFlags rf;
  rf << rfReplaceAll;
  return StringReplace(sIn.Trim(), "\t", "", rf);
}
//---------------------------------------------------------------------------
//http://docwiki.embarcadero.com/RADStudio/Sydney/en/UTF-8_Conversion_Routines
String __fastcall TFormTags::GetTagsAsString(void)
{
  String sOut;

  try
  {
    sOut += "\t" + EdtTitle->Text + "\t," +
            "\t" + EdtArtist->Text + "\t," +
            "\t" + EdtAlbum->Text + "\t," +
            "\t" + EdtGenre->Text + "\t," +
            "\t" + EdtYear->Text + "\t," +
            "\t" + EdtTrack->Text + "\t," +
            "\tENDSMTAGS\t"; // use this to validate the clipboard data
  }
  catch(...) {}
  return sOut;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#include <vcl.h>
#include "Main.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TExportForm *ExportForm;
//---------------------------------------------------------------------------
// See also: _wsplitpath, _wfnsplit, _wfnmerge, _wchdir, _wgetcwd, _wgetcurdir, _wmakepath, _wfullpath
//---------------------------------------------------------------------------
__fastcall TExportForm::TExportForm(TComponent* Owner)
  : TForm(Owner)
{
  pSFDlg = NULL;
  pExpModeDlg = NULL;
}
//---------------------------------------------------------------------------
__fastcall TExportForm::~TExportForm()
{
}
//---------------------------------------------------------------------------
void __fastcall TExportForm::FormCreate(TObject *Sender)
{
  Application->CreateForm(__classid(TSFDlgForm), &pSFDlg);
  Application->CreateForm(__classid(TExportModeForm), &pExpModeDlg);
}
//---------------------------------------------------------------------------
void __fastcall TExportForm::FormDestroy(TObject *Sender)
{
#if DEBUG_ON
  MainForm->CWrite( "\r\nFormDestroy() in TExportForm()!\r\n");
#endif
}
//---------------------------------------------------------------------------
void __fastcall TExportForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
  if (pSFDlg != NULL)
  {
    pSFDlg->Close();
    pSFDlg->Release();
  }

  if (pExpModeDlg != NULL)
  {
    pExpModeDlg->Close();
    pExpModeDlg->Release();
  }

#if DEBUG_ON
  MainForm->CWrite( "\r\nFormClose() in TExportForm()!\r\n");
#endif
}
//---------------------------------------------------------------------------
int __fastcall TExportForm::Dialog(TPlaylistForm* f, String d, String t)
// Title (t) and Initial directory (d) are UTF-8
{
#if (FREEWARE_EDITION == false)
  if (PK->ComputeDaysRemaining() <= 0)
  {
    ShowMessage("Trial Expired, visit:\n" + String(WEBSITE));
    return -1;
  }
#endif

  if (f == NULL || pExpModeDlg == NULL || pSFDlg == NULL)
    return 0;

  if (f->CheckBox == NULL || f->CheckBox->Items->Count == 0 || f->CheckBox->Tag < 0)
    return 0;

  int Count = -1;

  try
  {
    // NOTE: It's critical to use WideString() to wrap the following string!
    pSFDlg->Filters = WideString("All Files (*.*)|*.*|"
                  "Windows Media (wpl)|*.wpl|"
                  "MPEG UTF-8 (m3u8)|*.m3u8|"
                  "MPEG ANSI (m3u)|*.m3u|"
                  "Adv Stream XML (asx)|*.asx|"
                  "XML Shareable (xspf)|*.xspf|"
                  "Win Audio XML (wax)|*.wax|"
                  "Windows XML (wmx)|*.wmx|"
                  "Windows Video (wvx)|*.wvx|"
                  "Winamp (pls)|*.pls|"
                  "Text (txt)|*.txt");

    // Run the TSaveDialog and get a file name...
    String uDefFile = MainForm->WideToUtf8(WideString(EXPORT_FILE) +
                                                  WideString(EXPORT_EXT));

    if (pSFDlg->ExecuteU(uDefFile, d, t) == FALSE)
      return -1; // -1 will suppress an error-message

    String uName = pSFDlg->FileNameUtf8; // Get UTF-8 filepath

    if (uName.IsEmpty())
      return -1;

    String sPlayer = f == ListA ? "A" : "B";
    pExpModeDlg->Title = "Export Player " + sPlayer + " List"; // ANSI only!
    pExpModeDlg->FileName = uName;
    pExpModeDlg->Mode = EXPORT_PATH_ABSOLUTE;

    if (pExpModeDlg->ShowModal() == mrCancel)
      return -1;

    WideString wName = pSFDlg->FileName;
    bool bFileExists = MainForm->FileExistsW(wName);

    if (bFileExists)
    {
      WideString sMsg = WideString("File Already Exists:\n\n\"") +  wName +
                                            WideString("\"\n\nOverwrite it?");

      int button = MessageBoxW(MainForm->Handle, sMsg.c_bstr(),
              L"File Exists", MB_YESNO + MB_ICONQUESTION + MB_DEFBUTTON1);

      if (button == IDNO)
        return -1; // Don't print error message
    }

    // Gets the count of items exported
    Count = NoDialogW(f, wName, pExpModeDlg->Mode,
        pExpModeDlg->SaveAsUtf8, pExpModeDlg->UncPathFormat);
  }
  catch(...) {}

  return Count;
}
//---------------------------------------------------------------------------
int __fastcall TExportForm::NoDialogW(TPlaylistForm* f, WideString wListFullPath,
                              int Mode, bool bSaveAsUtf8, bool bUncPathFormat)
{
  return NoDialogU(f, MainForm->WideToUtf8(wListFullPath),
                             Mode, bSaveAsUtf8, bUncPathFormat);
}

int __fastcall TExportForm::NoDialogU(TPlaylistForm* f, String uListFullPath,
                              int Mode, bool bSaveAsUtf8, bool bUncPathFormat)
//
// uListFullPath must be UTF-8 (we use MainForm->WriteStringToFileW() to save the UTF-8 TStringList()...
// (we use the vcl's ansi string-parsing functions on uListFullPath - they work on a utf-8 string
// but not on a WideString)
//
// The bSaveAsUtf8 and bUncPathFormat flags pertain to the file-paths in the play-list
//
// Mode
// EXPORT_PATH_RELATIVE       0
// EXPORT_PATH_ROOTED         1
// EXPORT_PATH_ABSOLUTE       2
// EXPORT_PATH_NONE           3
{
  if (f == NULL)
    return 0;

  TMyCheckListBox* lb = f->CheckBox;

  if (lb == NULL || lb->Items->Count == 0 || lb->Tag < 0)
    return 0;

  int Count = 0;

  TStringList* sl = new TStringList();

  if (sl == NULL)
    return 0;

  try
  {
    int len = lb->Items->Count;

    String Ext = ExtractFileExt(uListFullPath).LowerCase();

    if (Ext.Length() > 0 && Ext.Pos(".") == 1)
      Ext.Delete(1, 1);

    MainForm->ExportExt = Ext; // Save as UTF-8

    String sTemp, sSavePrefix;

    TProgressForm::Init(len);

    if (Ext == "wpl")
    {
      sl->Add("<?wpl version=\"1.0\"?>");
      sl->Add("<smil>");
      sl->Add(" <head>");
      // From a file generated by Windows Media Player:
      // <meta name="Generator" content="Microsoft Windows Media Player -- 12.0.9600.17415"/>
      // <meta name="ItemCount" content="85"/>
      sl->Add("   <meta name=\"Generator\" content=\"SwiftMiX Player -- " + String(VERSION) + "\"/>");
      sl->Add("   <title>" + ExtractFileName(uListFullPath) + "</title>");
      sl->Add(" </head>");
      sl->Add(" <body>");
      sl->Add("   <seq>");

      for (int ii = 0; ii < len; ii++)
      {
        try
        {
          sTemp = ProcessFileName(lb->Items->Strings[ii], uListFullPath, Mode, bSaveAsUtf8, bUncPathFormat);

          if (!sTemp.IsEmpty())
          {
#if DEBUG_ON
    MainForm->CWrite( "\r\nR:" + sTemp + "\r\n");
#endif
            sTemp = InsertXMLSpecialCodes(sTemp); // replace "&" with "&amp;"
#if DEBUG_ON
    MainForm->CWrite( "\r\nS:" + sTemp + "\r\n");
#endif

            sl->Add("     <media src=\"" + sTemp + "\"/>");
            Count++;
          }
        }
        catch(...) { }

        TProgressForm::Move(ii);
      }
      sl->Add("   </seq>");
      sl->Add(" </body>");
      sl->Add("</smil>");
    }
    else if (Ext == "xspf") // Save as Windows-Media-Player XML file
    {
      String sEnc = bSaveAsUtf8 ? "\"UTF-8\"" : "\"ANSI\"";
      sl->Add("<?xml version=\"1.0\" encoding=" + sEnc + "?>");
      sl->Add("<playlist version=\"1\" xmlns=\"http://xspf.org/ns/0/\">");
      sl->Add(" <tracklist>");

      for (int ii = 0 ; ii < len ; ii++)
      {
        try
        {
          String sName = lb->Items->Strings[ii];

          // Note: sName is returned as a ref with the song-title (filename)
          sTemp = ProcessFileName(sName, uListFullPath, Mode, bSaveAsUtf8, bUncPathFormat);

          if (!sTemp.IsEmpty())
          {
            sTemp = MainForm->PercentEncode(sTemp, PERCENTCHARS, true); // also encode chars above 127

            sl->Add("   <track>");
            sl->Add("     <title>" + sName + "</title>");
            sl->Add("     <location>" + sTemp + "</location>");
            sl->Add("   </track>");
            Count++;
          }
        }
        catch(...) { }

        TProgressForm::Move(ii);
      }

      sl->Add(" </tracklist>");
      sl->Add("</playlist>");
    }
    else if (Ext == "asx" || Ext == "wax") // Save as Windows-Media-Player XML file
    {
      sl->Add("<ASX version = \"3.0\">");
      String sEnc = bSaveAsUtf8 ? "\"UTF-8\"" : "\"ANSI\"";
      sl->Add("   <PARAM name = \"encoding\" value = " + sEnc + " />");
      sl->Add("   <TITLE>" + ExtractFileName(uListFullPath) + "</TITLE>");

      for (int ii = 0 ; ii < len ; ii++)
      {
        try
        {
          String sName = lb->Items->Strings[ii];

          // sName is returned by reference with the Title (filename)
          sTemp = ProcessFileName(sName, uListFullPath, Mode,
                                        bSaveAsUtf8, bUncPathFormat);

          if (!sTemp.IsEmpty())
          {
// Unclear if this is needed - it IS needed for .wpl files for WMP 12 to read the list...
//            sTemp = InsertXMLSpecialCodes(sTemp); // replace "&" with "&amp;"

            sl->Add("   <ENTRY>");
            sl->Add("     <TITLE>" + sName + "</TITLE>");
            sl->Add("     <REF HREF = \"" + sTemp + "\" />");
            sl->Add("   </ENTRY>");
            Count++;
          }
        }
        catch(...) { }

        TProgressForm::Move(ii);
      }

      sl->Add("</ASX>");
    }
    else if (Ext == "pls") // Save as PLSv2 (Winamp)
    {
      sl->Add("[playlist]");

      for (int ii = 0 ; ii < len ; ii++)
      {
        try
        {
          sTemp = ProcessFileName(lb->Items->Strings[ii], uListFullPath,
                                         Mode, bSaveAsUtf8, bUncPathFormat);

          if (!sTemp.IsEmpty())
          {
            String sCount = String(Count+1) + "=";
            sl->Add("File" + sCount + sTemp);
            sl->Add("Title" + sCount + UniversalExtractFileName(sTemp));
            sl->Add("Length" + sCount + "-1"); // ignore length (usually for streaming)
            Count++;
          }
        }
        catch(...) { }

        TProgressForm::Move(ii);
      }

      if (sl->Count > 0)
      {
        sl->Add("NumberOfEntries=" + String(Count));
        sl->Add("Version=2");
      }
    }
    else // handle m3u and m3u8
    {
      // Save as plain-text file
      for (int ii = 0 ; ii < len ; ii++)
      {
        try
        {
          sTemp = ProcessFileName(lb->Items->Strings[ii], uListFullPath,
                                       Mode, bSaveAsUtf8, bUncPathFormat);

          if (!sTemp.IsEmpty())
          {
            sl->Add(sTemp);
            Count++;
          }
        }
        catch(...) { }

        TProgressForm::Move(ii);
      }
    }

    if (sl->Count > 0)
      MainForm->WriteStringToFileW(MainForm->Utf8ToWide(uListFullPath), sl->Text);

    TProgressForm::UnInit();
  }
  catch(...) { ShowMessage("Error In NoDialog()"); }

  delete sl;
  return Count;
}
//---------------------------------------------------------------------------
String __fastcall TExportForm::ProcessFileName(String &uName,
        String uListFullPath, int Mode, bool bSaveAsUtf8, bool bUncPathFormat)
// uName is the full utf-8 file path from the list-box. It can be in
// "file:/localhost/drive/path/file.ext" format
// or like "C:\path\song.wma", "relative-path\song.wma",
// "\rooted-relative-path\song.wma"
// ".\path\song.wma", "..\path\song.wma", "./path/song.mp3"
// uName contains the File name only on return
// We return the path we want to write into to the playlist
{
  try
  {
    String sTemp = uName;

    if (!bSaveAsUtf8)
      sTemp = MainForm->Utf8ToAnsi(sTemp);

    // Return the title (filename) in uName
    uName = UniversalExtractFileName(sTemp);

    // If it's a non-file URL like HTTP://, just return it as-is...
    if (sTemp.Pos(":/") > 2 && sTemp.LowerCase().Pos("file:") != 1)
      return sTemp;

    // Convert to "normal" file-path we can work with
    String sSavePrefix = StripFileUriPrefixIfAny(sTemp);

    // Ok, it's a file: URI. Convert to normal Windows path if needed and apply user path options...
    sTemp = GetFileString(uListFullPath, sTemp, Mode); // add user-selected path options

    if (bUncPathFormat)
    {
      sTemp = MainForm->ReplaceAll(sTemp, '\\', '/');

      // FYI: Looks like Microsoft metafiles that have relative links HAVE to
      // be on the local server... then you access those playlists via an ASX
      // metafile at the client which has a <ENTRYREF HREF = "" />
      // that points to the remote playlist...
      if (Mode != EXPORT_PATH_RELATIVE)
        sTemp = sTemp.Insert(sSavePrefix, 1); // put back the "file:/localhost/" part...
      else if (sTemp.Pos("./") != 1) // don't keep adding more!
        sTemp = sTemp.Insert("./", 1); // this is required for UNIX paths (ok for Windows too!)
    }

    return sTemp;
  }
  catch(...)
  {
    ShowMessage("Error In ProcessFileName()");
    return "";
  }
}
//---------------------------------------------------------------------------
String __fastcall TExportForm::GetFileString(String uListFullPath,
                                            String sSongFullPath, int Mode)
// uListFullPath is the path/filename of the list we are writing to.
// sSongFullPath is the path/filename of the song.
//
// Mode:
// EXPORT_PATH_RELATIVE       0
// EXPORT_PATH_ROOTED         1
// EXPORT_PATH_ABSOLUTE       2
// EXPORT_PATH_NONE           3
//
// Only call this with a path of the form "file:/localhost/drive/path/file.ext"
//
// This function converts the input path to a "standard" file path and applies
// the user's chosen Mode to it. The return string is a "standard" path without
// the "file:" prefix and with "/" changed to "\"
//
{
  // ANSI ReplaceAll should work ok on a UTF-8 path...
  sSongFullPath = MainForm->ReplaceAll(sSongFullPath, '/', '\\');

  try
  {
    String sTemp; // default returned string...

    if (Mode == EXPORT_PATH_RELATIVE)
    {
      // This function returns FileNameAndPath if the song is on a different
      // drive than the list's RootPath
      // The relative path returned has no leading backslash. "..\..\" are
      // inserted automatically to go up.
      try { sTemp = ExtractRelativePath(ExtractFilePath(uListFullPath), sSongFullPath); }
      catch(...) { ShowMessage("Error 1 In GetFileString()"); }
    }
    else if (Mode == EXPORT_PATH_NONE)
      sTemp = ExtractFileName(sSongFullPath);
    else if (Mode == EXPORT_PATH_ROOTED)
    {
      String sDrive = ExtractFileDrive(sSongFullPath);

      if (!sDrive.IsEmpty())
      {
        int pos = uListFullPath.Pos(sDrive);
        if (pos > 0)
        {
          TReplaceFlags rFlags = (TReplaceFlags() << rfIgnoreCase); // set StringReplace behavior
          sTemp = StringReplace(sSongFullPath, sDrive, "", rFlags); // strip drive
        }
      }
    }
    else // EXPORT_PATH_ABSOLUTE
      sTemp = sSongFullPath;

    return sTemp;
  }
  catch(...) { ShowMessage("Error 2 In GetFileString()"); return ""; }
}
//---------------------------------------------------------------------------
String __fastcall TExportForm::InsertXMLSpecialCodes(String sIn)
{
  String sOut;

  // Special XML replacements...
  try
  {
    int len = sIn.Length();
    for(int ii = 1 ; ii <= len; ii++)
      sOut += XmlSpecialCharEncode(sIn[ii]);
  }
  catch(...) { }

  return sOut;
}
//---------------------------------------------------------------------------
String __fastcall TExportForm::XmlSpecialCharEncode(char c)
{
  for (int ii = 0; ii < XMLCODESLEN; ii++)
    if (XMLCHARS[ii] == c)
      return XMLCODES[ii];

  return c;
}
//---------------------------------------------------------------------------
String __fastcall TExportForm::StripFileUriPrefixIfAny(String &sIn)
// Returns sIn by reference "as-is" or with the "file:/localhost/" part stripped.
// The "normal" return string is empty if no changes were made to sIn or
// it contains the "file:/localhost/" part so it can be restored if needed...
{
  try
  {
    if (sIn.Length() < 8) return "";
    String sTemp = sIn.SubString(1, 6).LowerCase();
    if (sTemp != "file:/") return "";
    int len = sIn.Length();
    int ii = 7; // start looking for the next / after file:/
    for (; ii <= len; ii++)
      if (sIn[ii] == '/') break;
    if (ii == len) return ""; // did not find second '/'!
    sTemp = sIn.SubString(1, ii);
    sIn = sIn.SubString(ii+1, len-(ii+1)+1);
    return sTemp;
  }
  catch(...) { return ""; }
}
//---------------------------------------------------------------------------
bool __fastcall TExportForm::IsUri(String sIn)
{
  // Could have "file:/laptop/D:/path/file.wma" so the key to telling a URL from
  // a drive letter is that url preambles are more than one char!
  //
  // sIn should be trimmed but does not need to be lower-case...
  return sIn.Pos(":/") > 2; // > 2 means you must have more than 1 char before the : (like "file:/")
}
//---------------------------------------------------------------------------
String __fastcall TExportForm::UniversalExtractFileName(String sIn)
{
  int len = sIn.Length();
  int idx = len;
  char c;

  for(;;)
  {
    if (idx == 0) break;
    c = sIn[idx];
    if (c == '\\' || c == '/') break;
    idx--;
  }

  if (idx != 0) return sIn.SubString(idx+1, len-(idx+1)+1);
  return sIn;
}
//---------------------------------------------------------------------------


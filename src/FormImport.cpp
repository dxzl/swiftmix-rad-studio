//---------------------------------------------------------------------------
#include <vcl.h>
#include "Main.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TImportForm *ImportForm;
//---------------------------------------------------------------------------
// See also: _wsplitpath, _wfnsplit, _wfnmerge, _wchdir, _wgetcwd, _wgetcurdir, _wmakepath, _wfullpath
//---------------------------------------------------------------------------
__fastcall TImportForm::TImportForm(TComponent* Owner)
  : TForm(Owner)
{
  XMLCHARSLEN = StrLen(XMLCHARS); // do this once for speed!
}
//---------------------------------------------------------------------------
__fastcall TImportForm::~TImportForm()
{
}
//---------------------------------------------------------------------------
void __fastcall TImportForm::FormDestroy(TObject *Sender)
{
#if DEBUG_ON
  MainForm->CWrite( "\r\nFormDestroy() in TImportForm()!\r\n");
#endif
}
//---------------------------------------------------------------------------
void __fastcall TImportForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
#if DEBUG_ON
  MainForm->CWrite( "\r\nFormClose() in TImportForm()!\r\n");
#endif
}
//---------------------------------------------------------------------------
int __fastcall TImportForm::Dialog(TPlaylistForm* f, String d, String t)
// Title (t) and Initial directory (d) are UTF-8
// returns the count of items added to a player's list
{
#if (FREEWARE_EDITION == false)
  if (PK->ComputeDaysRemaining() <= 0)
  {
    ShowMessage("Trial Expired, visit:\n" + String(WEBSITE));
    return -1;
  }
#endif

  if (f == NULL)
    return 0;

  TOFMSDlgForm* pOFMSDlg = NULL;
  TImportModeForm* pImpModeDlg = NULL;

  try
  {
    try
    {
      Application->CreateForm(__classid(TOFMSDlgForm), &pOFMSDlg);
      Application->CreateForm(__classid(TImportModeForm), &pImpModeDlg);

      if (pOFMSDlg == NULL || pImpModeDlg == NULL)
        return -2;

      pOFMSDlg->SingleSelect = true;

      // NOTE: It's critical to use WideString() to wrap the following string!
      pOFMSDlg->Filters = WideString("All Files (*.*)|*.*|"
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

      if (pOFMSDlg->ExecuteU(IMPORT_EXT, d, t))
      {
        String Ext = ExtractFileExt(pOFMSDlg->FileName).LowerCase();

        int iTempMode = GetMode(Ext);

        //  IMPORT_MODE_NONE     (-1)
        //  IMPORT_MODE_AUTO      0
        //  IMPORT_MODE_UTF8      1
        //  IMPORT_MODE_ANSI      2
        //  IMPORT_MODE_UTF16     3
        //  IMPORT_MODE_UTF16BE   4
        String sMode;
        switch(iTempMode)
        {
          case IMPORT_MODE_UTF8:
            sMode = "UTF-8";
          break;
          case IMPORT_MODE_ANSI:
            sMode = "ANSI";
          break;
          case IMPORT_MODE_UTF16:
            sMode = "UTF-16";
          break;
          case IMPORT_MODE_UTF16BE:
            sMode = "UTF-16BE";
          break;
          default:
          break;
        }

        pImpModeDlg->OurTitle = "File-type: \"" + Ext + "\" " + sMode;

        if (pImpModeDlg->ShowModal() == mrCancel)
          return -1; // -1 is cancel

        return NoDialog(f, pOFMSDlg->FileName, pImpModeDlg->Mode);
      }
    }
    catch(...)
    {
      return -3; // error
    }
  }
  __finally
  {
    MainForm->ReleaseFormNoClose(pOFMSDlg);
    MainForm->ReleaseForm(pImpModeDlg);
  }

  return -1; // canceled
}
//---------------------------------------------------------------------------
int __fastcall TImportForm::GetMode(String Ext)
// Return Mode a mode based on sExt. sExt is lowercase and has the "." on entry...
//
// Modes:
// IMPORT_MODE_NONE     (-1)
// IMPORT_MODE_AUTO      0
// IMPORT_MODE_UTF8      1
// IMPORT_MODE_ANSI      2
// IMPORT_MODE_UTF16     3
// IMPORT_MODE_UTF16BE   4
{
//  if (Ext == ".m3u8" || Ext == ".wpl" || Ext == ".asx" || Ext == ".wax" ||
//                        Ext == ".wmx" || Ext == ".wvx" || Ext == ".xspf")
//    return IMPORT_MODE_UTF8;

  if (Ext == ".m3u" || Ext == ".pls")
    return IMPORT_MODE_ANSI;

  return IMPORT_MODE_UTF8;
}
//---------------------------------------------------------------------------
// returns the # of songs added
int __fastcall TImportForm::NoDialog(TPlaylistForm* f, String sPath, int Mode)
//
// sPath is utf-8!
//
// Pass GetMode() in the Mode field if you call this without invoking it from Dialog()
// (for example, if a playlist file was drag-dropped and needs expedient processing)
//
// Returns the count of items added to a player's list
//
// WPL
//<smil>
//    <head><title>This Is A Test</title></head>
//    <body>
//        <seq>
//            <media src="F:\Music\Steely Dan\Countdown &amp; Ecstasy\05 Show Biz Kids.wma"/>
//            <media src="F:\Music\Steely Dan\Countdown &amp; Ecstasy\06 My Old School.wma"/>
//        </seq>
//    </body>
//</smil>
//
// XSPF
//<?xml version="1.0" encoding="UTF-8"?>
//<playlist version="1" xmlns="http://xspf.org/ns/0/">
//  <trackList>
//    <track>
//      <title>Windows Path</title>
//      <location>file:///C:/music/foo.mp3</location>
//    </track>
//    <track>
//      <title>Linux Path</title>
//      <location>file:///media/music/foo.mp3</location>
//    </track>
//    <track>
//      <title>Relative Path</title>
//      <location>music/foo.mp3</location>
//    </track>
//    <track>
//      <title>External Example</title>
//      <location>http://www.example.com/music/bar.ogg</location>
//    </track>
//  </trackList>
//</playlist>
{
  if (f == NULL || sPath.IsEmpty())
    return 0;

  int originalListCount = f->Count;

  TStringList* sl = NULL;

  try
  {
    // Press and hold Shift to bypass the file-extention filtering
    // (Checked in MainForm->AddFileToListBox())
    MainForm->GBypassFilters = (GetKeyState(VK_SHIFT) & 0x8000);

    try
    {
      if (!FileExists(sPath))
        return 0;

      String Ext = ExtractFileExt(sPath).LowerCase();

      sl = new TStringList();

      if (sl == NULL) return 0;

      if (Ext.Length() > 0 && Ext.Pos(".") == 1)
        Ext = Ext.Delete(1, 1);

      MainForm->ImportExt = Ext; // save as utf-8

      // Check for Winamp INI file
      if (Ext == "pls")
        ReadIniFile(sPath, sl);
      else
      {
        TEncoding* enc;

        // auto-set mode
        if (Mode == IMPORT_MODE_AUTO)
          Mode = this->GetMode(Ext);

        if (Mode == IMPORT_MODE_ANSI)
          enc = TEncoding::ANSI;
        else if (Mode == IMPORT_MODE_UTF16)
          enc = TEncoding::Unicode;
        else if (Mode == IMPORT_MODE_UTF16BE)
          enc = TEncoding::BigEndianUnicode;
        else
          enc = TEncoding::UTF8;

        sl->LoadFromFile(sPath, enc);
      }

      if (sl->Count == 0)
        return 0;

      if (Ext == "pls")
      {
        f->Progress->Init(sl->Count);

        // Read as plain-text, one file or URL per line
        for (int ii = 0; ii < sl->Count; ii++)
        {
          try
          {
            String sFile = sl->Strings[ii];

            if (sFile.Length() > 0)
              sFile = sFile.Trim();

            if (sFile.Length() == 0) continue;

            String Ext = ExtractFileExt(sFile).LowerCase();

            if (Ext.IsEmpty() && DirectoryExists(sFile))
              MainForm->AddDirToListBox(f, sFile); // recurse add folder and sub-folder's songs to list
            else if (ReplaceRelativePath(sFile, sPath)) // returns sFile as an absolute path...
              MainForm->AddFileToListBox(f, sFile);

            if (f->Progress->Move(ii))
              break;
          }
          catch(...) { ShowMessage("Exception thrown in TImportForm::NoDialog()! (1)"); }
        }
      }
      else
      {
        // Returns iCount == 0 if not an XML file-extension
        int iCount = XmlParser(f, Ext, sl->Text, sPath);

        if (iCount == 0)
        {
          f->Progress->Init(sl->Count);

          // Read as plain-text, one file or URL per line
          for (int ii = 0; ii < sl->Count; ii++)
          {
            try
            {
              String sFile = sl->Strings[ii];

              if (sFile.Length() > 0)
                sFile = sFile.Trim();

              if (sFile.Length() == 0 || sFile[1] == '#') continue; // Filter out .m3u info tags...

              String Ext = ExtractFileExt(sFile).LowerCase();

              if (Ext.IsEmpty() && DirectoryExists(sFile))
                MainForm->AddDirToListBox(f, sFile); // recurse add folder and sub-folder's songs to list
              else if (ReplaceRelativePath(sFile, sPath)) // returns sFile as an absolute path...
                MainForm->AddFileToListBox(f, sFile);

              if (f->Progress->Move(ii))
                break;
            }
            catch(...) { ShowMessage("Exception thrown in TImportForm::NoDialog()! (2)"); }
          }
        }
      }
    }
    catch(...) { ShowMessage("Exception thrown in TImportForm::NoDialog()!"); }
  }
  // NOTE: Unlike C#, __finally does NOT get called if we execute a return!!!
  __finally
  {
    if (sl)
      delete sl;

    f->Progress->UnInit(true);
  }

  return f->Count-originalListCount;
}
//---------------------------------------------------------------------------
int __fastcall TImportForm::XmlParser(TPlaylistForm* f, String sExt, String sIn, String sPath)
// sType has "href" "location" or "source", sIn has the raw file data...
// f has the form of the playlist we add files to.
// Returns Count of files added, 0 if error or no files
{
  String sType;

  sExt = sExt.LowerCase();
  if (sExt == "wpl")
    sType = "src";
  else if (sExt == "xspf")
    sType = "location";
  else if (sExt == "asx" || sExt == "wax" || sExt == "wmx" || sExt == "wvx")
    sType = "href";

  // Decode percent codes for xspf files. Decode XML codes for windows playlists
  bool bDecodePercentCodes = sExt == "xspf" ? true : false;
  bool bDecodeXmlCodes = sType == "href" || sExt == "wpl" ? true : false;

  TStringList* xsl = NULL;
  int originalListCount = f->Count;

  try
  {
    try
    {
      // parse out control chars and line terminators (result will be in s)
      String s;
      for (int ii = 1; ii <= sIn.Length(); ii++)
        if ((unsigned int)sIn[ii] >= SPACE)
          s += sIn[ii];

      int OriginalLength = s.Length();

      String sTag, sUrl;

      bool bTagParse = false;
      bool bUrlParse = false;

      // Use sIn as a temp string to see if we are UTF-8.... if we are, don't need to convert
      int iTemp = s.Length();
      iTemp = iTemp >= 500 ? 500 : iTemp;
      sIn = s.SubString(1, iTemp).LowerCase();

      xsl = new TStringList();

      f->Progress->Init(OriginalLength);

      for (int ii = 1; ii <= OriginalLength; ii++)
      {
        if (bTagParse)
        {
          if (s[ii] == '>')
          {
            // we have some sort of complete tag...

            sTag = sTag.Trim();
            int len = sTag.Length();

            if (len > 0)
            {
              String sTagLower = sTag.LowerCase();

              // tag is <xxx href> or <xxx location> or <xxx source>
              if (sTagLower.Pos(sType) > 0)
              {
                // come here for both an opening or a closing tag!
                bool bTagHasUrl = (!bUrlParse && sTag[len] == '/');
                bool bClosingTag = sTag[1] == '/'; // S.S. 7/21/16 add bClosingTag

                if (bTagHasUrl || (bUrlParse && bClosingTag)) // S.S. 7/21/16 change to !bClosingTag
                {
                  if (bTagHasUrl)
                    sUrl = sTag;

                  // can be "<media source = "this &amp; file" />"
                  // or "<ref href = "this file" />"
                  // or "<location = 'this file'/>"
                  // or "<location>this file</location>"
                  // or "<source> "this file" </source>"
                  // or "<ref HREF> 'this file' </href>"
                  if (ParseFileLine(sUrl, bTagHasUrl)) // returns sUrl by-reference, cleaned-up...
                  {
                    sUrl = StripCrLfAndTrim(sUrl);

                    if (bDecodePercentCodes)
                      sUrl = ReplacePercentCodes(sUrl);
                    if (bDecodeXmlCodes)
                      sUrl = ReplaceXmlCodes(sUrl);

                    xsl->Add(sUrl); // add to string-list
                  }

                  bUrlParse = false;
                }
                else if (!bUrlParse && !bClosingTag) // S.S. 7/21/16 add !bClosingTag
                {
                  sUrl = "";
                  bUrlParse = true; // start of url string next char
                }
              }
            }

            bTagParse = false;
          }
          else
            sTag += s[ii];
        }
        else if (s[ii] == '<')
        {
          sTag = "";
          bTagParse = true;
        }
        else if (bUrlParse)
          sUrl += s[ii]; // accumulate Url in Tag string

        if (f->Progress->Move(ii-1))
          break;
      }

      if (xsl->Count)
      {
        f->Progress->Init(xsl->Count);

        for (int ii = 0; ii < xsl->Count; ii++)
        {
          sUrl = xsl->Strings[ii];
          String Ext = ExtractFileExt(sUrl).LowerCase();

          if (Ext.IsEmpty() && DirectoryExists(sUrl))
            MainForm->AddDirToListBox(f, sUrl); // recurse add folder and sub-folder's songs to list
          else if (ReplaceRelativePath(sUrl, sPath)) // returns sUrl as an absolute path...
            MainForm->AddFileToListBox(f, sUrl);

          if (f->Progress->Move(ii))
            break;
        }
      }
    }
    catch(...){ ShowMessage("Exception thrown in TImportForm::XmlParser()!"); }
  }
  __finally
  {
    if (xsl)
      delete xsl;

    f->Progress->UnInit();
  }

  return f->Count-originalListCount;
}
//---------------------------------------------------------------------------
bool __fastcall TImportForm::ParseFileLine(String &sRef, bool bTagHasUrl)
// if sTagHasUrl is set we expect an equals sign... (<href = "name.ext" />)
// sRef is both the input and output string. Returns true if no errors.
{
  if (sRef.Length() == 0) return false;

  String sFile;

  try
  {
    int pos;
    String sQuote;

    if (bTagHasUrl)
    {
      pos = sRef.Pos("=");
      if (pos > 0)
        sRef = sRef.Delete(1, pos+1-1); // delete up through "="
    }

    pos = sRef.Pos("\"");
    if (pos > 0)
    {
      sRef = sRef.Delete(1, pos+1-1); // delete "
      sQuote = "\"";
    }
    else
    {
      pos = sRef.Pos("\'");
      if (pos > 0)
      {
        sRef = sRef.Delete(1, pos+1-1); // delete '
        sQuote = "\'";
      }
    }

    pos = 0;
    if (!sQuote.IsEmpty())
      pos = sRef.Pos(sQuote); // is there a trailing quote?

    if (pos > 0)
      sFile = sRef.SubString(1, pos-1);
    else
    {
      sFile = sRef;
      // erase leftover / from the "/>" ending
      if (bTagHasUrl)
        if (sFile[sFile.Length()] == '/')
          sFile = sFile.SetLength(sFile.Length()-1);
    }

    sRef = sFile;

    return true;
  }
  catch(...)
  {
    ShowMessage("Exception thrown in TImportForm::ParseFileLine()!");
    return false;
  }
}
//---------------------------------------------------------------------------
bool __fastcall TImportForm::ReplaceRelativePath(String &sFile, String sPath)
// Takes sUrl, from an imported play-list - which could be prefixed with "file://"
// and have forward or back-slashes... and could be a relative path or rooted-relative
// path - and converts it to a file-path with no "file://" prefix and delimited with
// windows-style backslashes and using sPath (the path of the song-list file)
// to return a full file-path with name and extension.
{
  try
  {
    bool bIsFileUri = MainForm->IsFileUri(sFile);

    // If it's a non-file URL like HTTP://, just return it as-is...
    if (MainForm->IsUri(sFile) && !bIsFileUri)
      return true;

    // Save old path
    WideString sTemp = GetCurrentDir();

    // Set current directory to that of our play-list file
    SetCurrentDir(ExtractFilePath(sPath));

    if (bIsFileUri)
    {
      sFile = sFile.Delete(1, 5);
      sFile = ReplaceStr(sFile, "\\", "/"); // make slashes consistent

      // delete leading //
      if (sFile.Pos("///.") == 1)
        sFile = sFile.Delete(1, 3);
      else if (sFile.Pos("//.") == 1)
        sFile = sFile.Delete(1, 2);
      else if (sFile.Pos("/.") == 1)
        sFile = sFile.Delete(1, 1);
      else if (sFile.Pos("///") == 1)
        sFile = sFile.Delete(1, 3);
      else if (sFile.Pos("//") == 1)
        sFile = sFile.Delete(1, 2);

      sFile = ExpandUNCFileName(sFile);
    }
    else
    {
      sFile = ReplaceStr(sFile, "/", "\\"); // make slashes consistent
      sFile = ExpandFileName(sFile);
    }

    SetCurrentDir(sTemp);
    return true;
  }
  catch(...)
  {
    ShowMessage("Exception thrown in TImportForm::ReplaceRelativePath()!");
    return false;
  }
}
//---------------------------------------------------------------------------
String __fastcall TImportForm::StripCrLfAndTrim(String sIn)
// Trim trailing spaces and remove cr/lf chars
{
  String sOut;

  for (int ii = 1; ii <= sIn.Length(); ii++)
    if (sIn[ii] != '\r' && sIn[ii] != '\n')
      sOut += sIn[ii];

  return sOut.Trim();
}
//---------------------------------------------------------------------------
String __fastcall TImportForm::ReplaceXmlCodes(String sIn)
// Replace &amp; etc...
{
  for (int ii = 0; ii < XMLCODESLEN; ii++)
    sIn = ReplaceStr(sIn, String(XMLCODES[ii]), String(XMLCHARS[ii]));
  return sIn;
}
//---------------------------------------------------------------------------
String __fastcall TImportForm::ReplacePercentCodes(String sIn)
// Replace %20 etc...
{
  int len = sIn.Length();
  if (len == 0) return "";

  if (len >= 3)
  {
    for (int ii = 1; ii <= len-3; ii++)
    {
      if (sIn[ii] == '%' && mishex(sIn[ii+1]) && mishex(sIn[ii+2])) // have a %hh hex code?
      {
        int val = String("0x0" + String(sIn[ii+1]) + String(sIn[ii+2])).ToIntDef(-1);

        if (val >= 0)
        {
          // Replace %XX with an ANSI char (might be part of a UTF-8 sequence...)
          sIn[ii] = (unsigned char)val;
          sIn = sIn.Delete(ii+1, 2);
          len -= 2;
        }
      }
    }
  }
  return sIn;
}
//----------------------------------------------------------------------------
bool __fastcall TImportForm::mishex(char c)
{
  if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) return true;
  return false;
}
//---------------------------------------------------------------------------
bool __fastcall TImportForm::ReadIniFile(String sIniPath, TStringList* sl)
// Reads the .pls file
{
  if (sl == NULL)
    return false;

  TIniFile *pIni = NULL;

  bool bRet = false;

  try
  {
    try
    {
      pIni = new TIniFile(sIniPath);

      if (pIni != NULL)
      {
        int count = pIni->ReadInteger("playlist", "NumberOfEntries", 0);

        if (count > 0)
        {
          String sPath, sTemp;

          for (int ii = 0; ii < count; ii++)
          {
            sTemp = "File" + String(ii+1);
            sPath = pIni->ReadString("playlist", sTemp, "");

            if (!sPath.IsEmpty())
              sl->Add(sPath);
          }
        }

        bRet = true;
      }
    }
    catch(const std::exception& e)
    {
#if DEBUG_ON
      MainForm->CWrite("\r\nException thrown in TImportForm::ReadIniFile():\r\n" + String(e.what()) + "\r\n");
#endif
    }
  }
  // NOTE: Unlike C#, __finally does NOT get called if we execute a return!!!
  __finally
  {
    try {if (pIni != NULL) delete pIni; }
    catch(...) { bRet = false; }
  }

  return bRet;
}
//---------------------------------------------------------------------------


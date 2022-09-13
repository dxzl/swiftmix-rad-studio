//---------------------------------------------------------------------------
#include <vcl.h>
#include "Main.h"
#pragma hdrstop

#include "PlaylistHelper.h"

#pragma package(smart_init)
//---------------------------------------------------------------------------
//fyi:
//HKEY subKey = nullptr;
//LONG result = RegOpenKeyEx(key, subPath.c_str(), 0, KEY_READ, &subKey);
//if (result == ERROR_SUCCESS) - key exists!
//---------------------------------------------------------------------------
//  constructor
__fastcall TPlaylistHelper::TPlaylistHelper(void)
{
}
//------------------- destructor -------------------------
__fastcall TPlaylistHelper::~TPlaylistHelper()
{
}
//---------------------------------------------------------------------------
String __fastcall TPlaylistHelper::GetURL(TPlaylistForm *f, int listIndex)
{
  if (listIndex < 0 || listIndex >= f->CheckBox->Count)
  {
#if DEBUG_ON
    MainForm->CWrite("\r\nTPlaylistHelper::GetURL(): bad index or null list-pointer!\r\n");
#endif
    return "";
  }

  TPlayerURL *p = (TPlayerURL*)f->CheckBox->Items->Objects[listIndex];
  if (!p) return "";

  String sPath = p->path;

  try
  {
    if (MainForm->CacheEnabled && p->cacheNumber > 0)
    {
      String sCachePath = pFC->GetCachePath(f, p);
      if (FileExists(sCachePath)){
#if DEBUG_ON
        MainForm->CWrite("\r\nTPlaylistHelper::GetURL(): found cached file!: \"" + sCachePath + "\"\r\n");
#endif
        sPath = sCachePath;
      }
      else{
#if DEBUG_ON
        MainForm->CWrite("\r\nTPlaylistHelper::GetURL(): cach-file " +
           String(p->cacheNumber) + " not found!: \"" + sCachePath + "\"\r\n");
#endif
        pFC->DeleteCacheFile(f, p->cacheNumber);
      }
    }
  }
  catch(...) {};

  return sPath;
}
//---------------------------------------------------------------------------
// this will instantiate an object of TMyFileCopy() which copies a
// file or times out - and automatically deletes itself
//
// returns a full path to the destination file by reference in sDestDir
// returns 0 if success - negative if error
int __fastcall TPlaylistHelper::MyFileCopy(TPlaylistForm *f, String &sDestDir, int idx, bool bIsCacheFile)
{
  if (!f || !MainForm->MyFileCopyList || idx < 0 || idx >= f->CheckBox->Count || !sDestDir.Length())
    return -2;

  TPlayerURL *p = (TPlayerURL*)f->CheckBox->Items->Objects[idx];
  if (!p) return -3;
  String sSourcePath = p->path;
  if (!sSourcePath.Length()) return -4;

  p->bDownloaded = false;

  String sDestPath = sDestDir + "\\" ;

  // check sSourcePath's first characters for ftp:\\, http:\\ or https:\\
  // see if this is a URL that MyCopyUrl can handle...
  String lsSourcePath = sSourcePath.LowerCase();
  int iPos1 = lsSourcePath.Pos("https://");
  int iPos2 = lsSourcePath.Pos("ftp://");
  int iPos3 = lsSourcePath.Pos("http://");
  int iPos4 = lsSourcePath.Pos("ftps://");

  bool bIsWebFile = (iPos1 == 1 || iPos2 == 1 || iPos3 == 1 || iPos4 == 1) ? true : false;

  String sFileName = MainForm->MyExtractFileName(sSourcePath);

  // every cache file has a unique numerical prefix to differentiate
  // different song files that have the same name...
  if (bIsCacheFile)
  {
#if DEBUG_ON
    MainForm->CWrite("\r\nTPlaylistHelper::MyFileCopy(): (before unique) sFileName=\"" + sFileName + "\"\r\n");
#endif

    sFileName = String(f->UniqueNumber++) + " " + sFileName;

#if DEBUG_ON
    MainForm->CWrite("\r\nTPlaylistHelper::MyFileCopy(): (after unique) sFileName=\"" + sFileName + "\"\r\n");
#endif
  }

  sDestPath = IncludeTrailingPathDelimiter(sDestPath) + sFileName;

  sDestDir = sDestPath; // return full-path by-reference

  if (!FileExists(sDestPath))
  {
    // non-blocking local or web file-copy

    // this bit of code limits the max # of simultaneous TMyFileCopy objects
    // NOTE: TMyUrlCopy and TMyFileCopy both add themselves to MyFileCopyList!
    if (MainForm->MyFileCopyList->Count > MAX_TMyFileCopy_OBJECTS)
    {
      do
      {
        Sleep(20);
        Application->ProcessMessages();
      } while (MainForm->MyFileCopyList->Count > MAX_TMyFileCopy_OBJECTS);
    }

    if (bIsWebFile)
    {
      new TMyUrlCopy(f, sSourcePath, sDestPath, idx);
      p->bDownloaded = true;
    }
    else if (FileExists(sSourcePath))
    {
      new TMyFileCopy(f, sSourcePath, sDestPath, idx);
      p->bDownloaded = false;
    }
    else
      return -4; // file should exist
  }


  return 0;
}
//---------------------------------------------------------------------------
int __fastcall TPlaylistHelper::AddDirToListBox(TPlaylistForm *f, String sPath)
// Strings are in UTF-8 format!
{
  TStringList *sl = NULL;
  int originalListCount = f->Count;
  int filesAdded;

  String sSaveCurrentDir = GetCurrentDir(); // save

  try
  {
    sl = new TStringList();

    if (!sl)
      return 0;

    // set current directory so we can recurse through subdirectories
    if (!SetCurrentDir(sPath))
      return 0;

#if DEBUG_ON
    MainForm->CWrite("\r\nTPlaylistHelper::AddDirToListBox(): sPath=\"" + sPath + "\"\r\n");
#endif

    RecurseFileAdd(f, sl);

    int slCount = sl->Count;

    if (slCount)
    {
      f->Progress->Init(slCount);

      for (int ii = 0 ; ii < slCount ; ii++)
      {
        AddFileToListBox(f, sl->Strings[ii]);

        if (f->Progress->Move(ii))
          break;
      }

      f->Progress->UnInit();
    }
  }
  __finally
  {
    if (!sSaveCurrentDir.IsEmpty())
      SetCurrentDir(sSaveCurrentDir); // restore

    if (sl)
      delete sl;

    filesAdded = f->Count-originalListCount;

    if (filesAdded)
      ShowPlaylist(f);
  }

  return filesAdded; // return # songs added to list
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistHelper::AddFileToListBox(TPlaylistForm *f, String sSourcePath)
{
  if (f == NULL || sSourcePath.Length() == 0)
    return false;

  try
  {
    // NOTE: the following creates a new TPlayerURL() class object!
    // (its pointer is stored in the list-item's Tag property)
    if (MainForm->IsAudioFile(sSourcePath))
      f->AddListItem(sSourcePath);

    // move the first file to the cache
    if (f->Count == 1)
      pFC->CopyFileToCache(f, -1);

    return true;
  }
  catch(...) { return false; }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistHelper::RecurseFileAdd(TPlaylistForm *f, TStringList *slFiles)
// Uses the String versions of the Win32 API FindFirstFile and FindNextFile directly
// and converts the resulting paths to UTF-8 for storage in an ordinary TStringList
//
// Use SetCurrentDir() to set our root directory or TOpenDialog sets it also...
{
  if (slFiles == NULL)
    return;

  TStringList *slSubDirs = NULL;
  HANDLE hFind = NULL;

  Application->ProcessMessages();

  try
  {
    slSubDirs = new TStringList();

    if (!slSubDirs)
      return;

    // get list of subdirectories and files
    MainForm->FindFirstNextToStringLists(slFiles, slSubDirs);

    // Get songs in all subdirectories
    // NOTE: best not to use progress bar here because we just stack up Init() objects - messy!
    for (int ii = 0; ii < slSubDirs->Count; ii++)
    {
      if (SetCurrentDir(slSubDirs->Strings[ii]))
      {
        RecurseFileAdd(f, slFiles);

        // NOTE: An attempt to open a search with a trailing backslash always fails.
        SetCurrentDir("..");
      }
    }
  }
  __finally
  {
    try { if (hFind != NULL) FindClose(hFind); } catch(...) {}

    if (slSubDirs)
      delete slSubDirs;
  }
}
//---------------------------------------------------------------------------
int __fastcall TPlaylistHelper::CopyMusicFiles(TPlaylistForm *f, String sUserDir)
{
//  String sWmpUrl;

  try
  {
    int count = f->Count;

    f->Progress->Init(count, 5);

    try
    {
      String sSourcePath;

      // Copy ListA or ListB files
      for (int ii = 0; ii < count; ii++)
      {
// NOTE: don't need this since I discovered the problem was actually the file-sharing mode
// being opened for MyGetFileSize() in FormMain.cpp!
        // Found that we can't copy a file that is open in Wmp->URL!!!!!!!!!!!!!!!!!!
//        sWmpUrl = f->Wmp->URL; // release the file from the player
//        if (sWmpUrl.Length() > 0)
//        {
//          String sWmpFile = MainForm->MyExtractFileName(sWmpUrl);
//          if (sWmpFile == uFileName)
//            f->Wmp->URL = "dummy.mp3";
//        }

        // pass in sUserDir - returns sDestPath by reference!
        String sDestPath = sUserDir;
        int retCode = MyFileCopy(f, sDestPath, ii);
        if (retCode < 0)
        {
#if DEBUG_ON
          MainForm->CWrite( "\r\nTPlaylistHelper::MyFileCopy() error: " + String (retCode) + "\r\n");
#endif
          return retCode;
        }

        if (f->Progress->Move(ii))
          return -1;
      }
    }
    catch(...) {}
  }
  __finally
  {
    // restore song file had to disconnect from the player to copy it...
//    if (sWmpUrl.Length() && f->Wmp->URL == "dummy.mp3")
//      f->Wmp->URL = sWmpUrl;

    f->Progress->UnInit();
  }

  return 0;
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistHelper::FileDialog(TPlaylistForm *f, String &d, String t)
// d is the default directory (in UTF-8)
// t is the title (in UTF-8)
{
  if (f == NULL || f->Wmp == NULL)
    return false;

#if !FREEWARE_EDITION
  if (PK->ComputeDaysRemaining() <= 0)
  {
    ShowMessage("Trial Expired, visit: \n" + String(WEBSITE));
    return false;
  }
#endif

  bool bRet = false; // presume failure...

  try
  {
    try
    {
      TOFMSDlgForm *fd = f->CreateFileDialog();

      if (fd != NULL)
      {
        // Filters
        fd->Filters = "All Files (*.*)|*.*|"
                    "Windows Media (*.wma)|*.wma|"
                    "MP3 (*.mp3)|*.mp3|"
                    "WAV (*.wav)|*.wav|"
                    "OGG (*.ogg)|*.ogg"; // need codecs from xiph.org

        MainForm->FilesAddedCount = 0;

        fd->ExecuteU(0, d, t); // no def utf-8 extension?

        if(fd->Result == IDOK)
        {
          // Press and hold Shift to bypass the file-extention filtering
          // (Checked in AddFileToListBox())
          MainForm->GBypassFilters = (GetKeyState(VK_SHIFT) & 0x8000);

#if DEBUG_ON
          // Display for diagnostics
          if (fd->FileNameObjects && fd->FileNameObjects->Count > 0)
          {
            MainForm->CWrite( "\r\nPrinting selected-files list!!!!!!!\r\n");

            String s1 = "";

            int iCount = fd->FileNameObjects->Count;

            for (int ii = 0; ii < iCount; ii++)
            {
              TWideItem *pWI = (TWideItem*)fd->FileNameObjects->Items[ii];

              if (pWI)
              {
                String s = String(pWI->s);
                String sDir = pWI->IsDirectory ? "true" : "false";
                s1 += s + " (IsDirectory: " +  sDir + ")\n";
              }
            }

            if (!s1.IsEmpty())
              MainForm->CWrite( "\r\n" + s1 + "\r\n");

            bRet = true;
          }
          else
            MainForm->CWrite( "\r\nsl TStringList is NULL or Empty!\r\n");
#endif

          if (fd->FileNameObjects && fd->FileNameObjects->Count > 0)
          {
            if (f == ListA)
              MainForm->SaveDirA = fd->CurrentFolder;
            else
              MainForm->SaveDirB = fd->CurrentFolder;

            int iCount = fd->FileNameObjects->Count;

            f->Progress->Init(iCount);

            for (int ii = 0; ii < iCount ; ii++)
            {
              TWideItem *pWI = (TWideItem*)fd->FileNameObjects->Items[ii];

              if (pWI)
                ProcessFileItem(f, pWI->s);

              if (f->Progress->Move(ii))
                return false;
            }
          }

          // Show the listbox
          if (f->Count)
            ShowPlaylist(f);

          bRet = true;
        }
        // User canceled... only a problem if they added files via our custom mechanism...
        else if (MainForm->FilesAddedCount > 0)
        {
#if DEBUG_ON
          MainForm->CWrite("\r\nGFilesAdded is > 0...\r\n");
#endif
          bRet = true;
        }
        else
        {
#if DEBUG_ON
          MainForm->CWrite("\r\nUser canceled!!!!!!\r\n");
#endif
        }
      }
    }
    catch(...) { ShowMessage("FileDialog() threw an exception"); }
  }
  __finally
  {
    f->DestroyFileDialog();
    f->Progress->UnInit(true);
  }

  if (bRet)
    MainForm->SetVolumes();

  return bRet;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistHelper::LoadListWithDroppedFiles(TPlaylistForm *f, TWMDropFiles &Msg)
{
  if (f == NULL)
    return;

  TCursor Save_Cursor;
  wchar_t *pBuf = NULL;
  TStringList *sl = NULL;
  ImportForm = NULL;

  MainForm->FilesAddedCount = 0;

  try
  {
    //get dropped files count
    int DroppedCount = ::DragQueryFileW((HDROP)Msg.Drop, -1, NULL, 0);

    if (DroppedCount == 0)
      return;

    Save_Cursor = Screen->Cursor;
    Screen->Cursor = crHourGlass;    // Show hourglass cursor

    if (ImportForm == NULL)
      Application->CreateForm(__classid(TImportForm), &ImportForm);

    pBuf = new wchar_t[MAX_PATH];
    sl = new TStringList();
//    sl->Sorted = true; // don't want to mess up a user-chosen order...

    f->Progress->Init(DroppedCount);

    for (int ii = 0; ii < DroppedCount; ii++)
    {
      ::DragQueryFileW((HDROP)Msg.Drop, ii, pBuf, MAX_PATH);

      if (*pBuf != '\0')
        sl->Add(pBuf);

      if (f->Progress->Move(ii))
        break;
    }

    // Repair a quirky list-system... (not needed after the first item has been added to a listbox!)
    if (sl->Count > 1 && f->Count == 0)
      sl->Exchange(0, sl->Count-1);

    // Press and hold Shift to bypass the file-extention filtering
    MainForm->GBypassFilters = (GetKeyState(VK_SHIFT) & 0x8000);

    String SaveDir = GetCurrentDir(); // Save

    f->Progress->Init(sl->Count);

    for (int ii = 0; ii < sl->Count; ii++)
    {
      ProcessFileItem(f, sl->Strings[ii]);

      if (f->Progress->Move(ii))
        break;
    }

    SetCurrentDir(SaveDir); // Restore

    if (MainForm->FilesAddedCount > 0)
      ShowPlaylist(f);
    else
      ShowMessage("Song(s) may not have been added because either they did not\n"
             "appear to be sound files or the file was not found.\n\n"
             "Perhaps your music drive USB cable is unplugged?\n\n"
             "(To bypass the filter-list, press and hold SHIFT until you release\n"
             "the mouse button and drag/drop the file(s) again.)");
  }
  __finally
  {
    if (MainForm->ReleaseForm((TForm*)ImportForm))
      ImportForm = NULL;
    try { if (pBuf != NULL) delete [] pBuf; } catch(...) {}
    try { if (sl != NULL) delete sl; } catch(...) {}

    f->Progress->UnInit(true);

    // Restore the previous cursor.
    Screen->Cursor = Save_Cursor;
  }
}
//---------------------------------------------------------------------------
// expand any .lnk shortcut, if any and validate file
void __fastcall TPlaylistHelper::ProcessFileItem(TPlaylistForm *f, String s)
{
//#if DEBUG_ON
//      MainForm->CWrite("\r\nTMainForm::ProcessFileItem: Original sSourcePath: \"" + s + "\"\r\n");
//#endif

      // expand any .lnk shortcut, if any and validate file
      bool bIsDirectory;
      if (MainForm->GetShortcut(s, bIsDirectory))
      {
//#if DEBUG_ON
//        MainForm->CWrite("\r\nTMainForm::ProcessFileItem: GetShortcut() sSourcePath: \"" + s + "\"\r\n");
//#endif
        if (bIsDirectory)
        {
//#if DEBUG_ON
//          MainForm->CWrite("\r\nTMainForm::ProcessFileItem: bIsDirectory is set! Calling AddDirToListBox()\r\n");
//#endif
          MainForm->FilesAddedCount += AddDirToListBox(f, s); // recurse add folder and sub-folder's songs to list
        }
        else if (MainForm->IsPlaylistPath(s))
        {
//#if DEBUG_ON
//          MainForm->CWrite("\r\nTMainForm::ProcessFileItem: IsPlaylistPath() returned true! Calling ImportForm->NoDialog()\r\n");
//#endif
          MainForm->FilesAddedCount += ImportForm->NoDialog(f, s, IMPORT_MODE_AUTO); // Load the playlist
        }
        else if (AddFileToListBox(f, s))
        {
//#if DEBUG_ON
//          MainForm->CWrite("\r\nTMainForm::ProcessFileItem: AddFileToListBox() returned true!\r\n");
//#endif
          MainForm->FilesAddedCount++;
        }
//#if DEBUG_ON
//        else
//        {
//          MainForm->CWrite("\r\nTMainForm::ProcessFileItem: ITEM NOT PROCESSED!\r\n");
//        }
//#endif
      }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistHelper::ShowPlaylist(TPlaylistForm *f)
{
  if (f == NULL)
  {
    ShowMessage("null pointer in ShowPlaylist()");
    return;
  }

  try
  {
    if (f->Count && f->PlayIdx == -1)
      f->QueueFirst();

    f->SetTitle();

    if (f->WindowState == wsMinimized)
      f->WindowState = wsNormal;

    // already showing? skip...
    if (f->Visible)
      return;

    f->Height = MainForm->Height/2 + 3;
    f->Width = 3*MainForm->Width/2;

    int borderWidth;
    int borderHeight;

    if (MainForm->VistaOrHigher)
    {
      borderWidth = GetSystemMetrics(SM_CXDLGFRAME);
      borderHeight = GetSystemMetrics(SM_CYDLGFRAME);
    }
    else // xp
    {
      borderWidth = 0;
      borderHeight = 0;
    }

    if (f == ListA)
    {
      // Player 1
      f->Left = MainForm->Left - borderWidth;
      f->Top = MainForm->Top - f->Height - borderHeight;
    }
    else
    {
      // Player 2
      f->Left = (MainForm->Left + MainForm->Width) - f->Width + borderWidth;
      f->Top = MainForm->Top + MainForm->Height + borderHeight;
    }

    if (f->Left < 1)
      f->Left = 1;

    int diff = (f->Left+f->Width)-Screen->Width;
    if (diff > 0)
      f->Left -= diff;

    if (f->Top < 1)
      f->Top = 1;

    diff = (f->Top+f->Height)-Screen->Height;
    if (diff > 0)
      f->Top -= diff;

    f->Show();
// 6/2022 added PlayPreview - release of ctrl key requires playlist form's focus!
//    MainForm->SetFocus();

//    GDock->WindowMoved(f->Handle);
  }
  catch(...) { ShowMessage("ShowPlaylist() threw an exception"); }
}
//---------------------------------------------------------------------------


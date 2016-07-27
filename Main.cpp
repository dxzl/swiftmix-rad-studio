// EDIT UNIT8.h TO CHANGE TRIAL-KEY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//---------------------------------------------------------------------------
#include <vcl.h>
#include "Main.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "WMPLib_OCX"
#pragma resource "*.dfm"

TMainForm* MainForm;
//CWindowDock* gDock;
#if !FREEWARE_EDITION
KeyClass* PK;
#endif
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner) : TForm(Owner)
{
  ListA = NULL;
  ListB = NULL;

  m_filesAddedCount = 0;

#if !FREEWARE_EDITION
  // Init License-Key Properties
  PK = new KeyClass();

  bool bRet;
  TLicenseKey* lk = new TLicenseKey();
  bRet = lk->ValidateLicenseKey(false);
  delete lk;

  if (!bRet)
  {
    ShowMessage("Critical error in class: TLicenseKey!");
    Release();
    Application->Terminate();
    return;
  }
#endif

#if DEBUG_ON
  CInit();
#endif

  // Create Window Docking Manager
//  gDock = new CWindowDock();

  ProgressForm = NULL; // Program tests this in RecurseFileAdd

  Application->HintColor = TColor(0xF5CFB8);
  Application->HintPause = 500;
  Application->HintHidePause = 4000;
  Application->ShowHint = true;

  FadeAt = 10;

  GPlaylistForm = NULL;

  DeskDir = GetSpecialFolder(CSIDL_DESKTOPDIRECTORY);

  // Try to register the SwiftMix messages
  RWM_SwiftMixTime = RegisterWindowMessage("WM_SwiftMixTime");
  RWM_SwiftMixPlay = RegisterWindowMessage("WM_SwiftMixPlay");
  RWM_SwiftMixState = RegisterWindowMessage("WM_SwiftMixState");
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject* Sender)
{
  try
  {
    String Info = String(WindowsMediaPlayer1->versionInfo);

    int tmp = Info.Pos(".");
    String Version = Info.SubString(1,tmp-1);
    tmp = Version.ToInt();
    if (tmp < 10)
    {
      ShowMessage("Sorry, you must get the newest version of\n"
        "Microsoft's Windows Media-Player!\n"
        "Your version is: " + Info);
      Release();
      Application->Terminate();
      return;
    }
  }
  catch(...)
  {
    ShowMessage("You must get the new version of\n"
        "Microsoft's Windows Media-Player!");
    Release();
    Application->Terminate();
    return;
  }

  Caption = "SwiftMiX " + String(VERSION);
  Color = TColor(0xF5CFB8);

  // Make sure MainForm won't auto-size...
  AutoSize = false;

  // ActiveX Control needs to have its size and
  // position properties explicitly set as of
  // Version 1.38 (9/7/2010)
  WindowsMediaPlayer1->Align = alNone;
  WindowsMediaPlayer1->uiMode = "full"; // "mini", "full", "none"
  WindowsMediaPlayer1->Left = 0;
  WindowsMediaPlayer1->Top = 0;
  WindowsMediaPlayer1->Height = 45;
  WindowsMediaPlayer1->Height = 45;
// Use its default size!
//  WindowsMediaPlayer1->Width = 257;

  WindowsMediaPlayer2->Align = alNone;
  WindowsMediaPlayer2->uiMode = "full"; // "mini", "full", "none"
  WindowsMediaPlayer2->Left = WindowsMediaPlayer1->Left;
  WindowsMediaPlayer2->Top = WindowsMediaPlayer1->Height + 2;
  WindowsMediaPlayer2->Height = WindowsMediaPlayer1->Height;
  WindowsMediaPlayer2->Width = WindowsMediaPlayer1->Width;

  UpDown1->Left = 0;
  UpDown1->Height = 21;
  UpDown1->Width = 17;

  FadeRate->Left = UpDown1->Left+UpDown1->Width;
  FadeRate->Height = UpDown1->Height;
  FadeRate->Width = UpDown1->Width;

  UpDown2->Left = FadeRate->Left+FadeRate->Width;
  UpDown2->Height = UpDown1->Height;
  UpDown2->Width = UpDown1->Width;

  FadePoint->Left = UpDown2->Left+UpDown2->Width;;
  FadePoint->Height = UpDown1->Height;
  FadePoint->Width = UpDown1->Width+4;

  TrackBar1->Left = FadePoint->Left+FadePoint->Width+2;
  TrackBar1->Height = 33;
  TrackBar1->Top = WindowsMediaPlayer2->Top+WindowsMediaPlayer2->Height+2;
  TrackBar1->Width = WindowsMediaPlayer1->Width-TrackBar1->Left;

  UpDown1->Top = TrackBar1->Top+((TrackBar1->Height-UpDown1->Height)/2);
  FadeRate->Top = UpDown1->Top;
  UpDown2->Top = UpDown1->Top;
  FadePoint->Top = UpDown1->Top;

  // Now dynamically Autosize the MainForm...
  AutoSize = true;

  // Just in-case the width is too small for the status-bar's
  // time--displays...
//ShowMessage(String(WindowsMediaPlayer1->Width));
  if (ClientWidth < 240)
  {
    AutoSize = false;
    ClientWidth = 240;
  }

  // Adjust the status-bar's panels
  int Size = StatusBar1->Width/5;
  for (int ii = 0 ; ii < StatusBar1->Panels->Count ; ii++)
    StatusBar1->Panels->Items[ii]->Width = Size;

  // Create Window Docking Manager
//  FDock = new CWindowDock();
//  if (FDock == NULL)
//    ShowMessage("Unable to create docking manager!");

  //enable drag/drop files
  ::DragAcceptFiles(this->Handle, true);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormShow(TObject *Sender)
{
  // Create the song-list forms
  Application->CreateForm(__classid(TPlaylistForm), &ListA);
  Application->CreateForm(__classid(TPlaylistForm), &ListB);

  if (ListA == NULL || ListB == NULL)
  {
    ShowMessage("Problem creating player list-objects!");
    Application->Terminate();
  }

  WindowsMediaPlayer1->Tag = PLAYER_1;
  ListA->Wmp = WindowsMediaPlayer1;
  ListA->OtherWmp = WindowsMediaPlayer2;
  ListA->OtherForm = ListB;
  ListA->PlayerA = true; // this list is for the A player

  WindowsMediaPlayer2->Tag = PLAYER_2;
  ListB->Wmp = WindowsMediaPlayer2;
  ListB->OtherWmp = WindowsMediaPlayer1;
  ListB->OtherForm = ListA;
  ListB->PlayerA = false; // this list is not for the A player

	//-----------------
	// Add the windows to the docking system
	//-----------------
	//Set the parent window
//  if (gDock != NULL)
//  {
//  	gDock->SetParent(this->Handle);
//
//  	//Add the child windows
//  	gDock->AddChild(ListA->Handle); // automatically dock
//  	gDock->AddChild(ListB->Handle); // automatically dock
//  }

  TRegHelper* pReg = NULL;

  try
  {
    pReg = new TRegHelper(false);

    if (pReg != NULL)
    {
      // Initial directory is the desktop
      // The strings below are in UTF-8 format
      SaveDirA = pReg->ReadSetting(SM_REGKEY_DIR_A);
      SaveDirB = pReg->ReadSetting(SM_REGKEY_DIR_B);
      ImportExt = pReg->ReadSetting(SM_REGKEY_IMPORTEXT);
      ExportExt = pReg->ReadSetting(SM_REGKEY_EXPORTEXT);

      if (SaveDirA.IsEmpty())
        SaveDirA = WideToUtf8(DeskDir);

      if (SaveDirB.IsEmpty())
        SaveDirB = WideToUtf8(DeskDir);

      if (ImportExt.IsEmpty())
        ImportExt = WideToUtf8(IMPORT_EXT);

      if (ExportExt.IsEmpty())
        ExportExt = WideToUtf8(EXPORT_EXT);

      pReg->ReadSetting(SM_REGKEY_FADERMODE, bTypeCenterFade, false);
      pReg->ReadSetting(SM_REGKEY_FADERTYPE, bModeManualFade, false);
      pReg->ReadSetting(SM_REGKEY_SENDTIMING, bSendTiming, false);
      pReg->ReadSetting(SM_REGKEY_REPEAT_A, bRepeatModeA, false);
      pReg->ReadSetting(SM_REGKEY_REPEAT_B, bRepeatModeB, false);
      pReg->ReadSetting(SM_REGKEY_SHUFFLE_A, bShuffleModeA, false);
      pReg->ReadSetting(SM_REGKEY_SHUFFLE_B, bShuffleModeB, false);
      pReg->ReadSetting(SM_REGKEY_VOL_A, volA, 50);
      pReg->ReadSetting(SM_REGKEY_VOL_B, volB, 50);
    }
    else
    {
      ShowMessage("Unable to read settings from the registry!");

      SaveDirA = WideToUtf8(DeskDir);
      SaveDirB = WideToUtf8(DeskDir);
      ImportExt = WideToUtf8(IMPORT_EXT);
      ExportExt = WideToUtf8(EXPORT_EXT);

      bTypeCenterFade = false;
      bModeManualFade = false;
      bSendTiming = false;
      bRepeatModeA = false;
      bRepeatModeB = false;
      bShuffleModeA = false;
      bShuffleModeB = false;
    }
  }
  __finally
  {
    try { if (pReg != NULL) delete pReg; } catch(...) {}
  }

  MenuFaderTypeNormal->Checked = bTypeCenterFade ? false : true; // inverse
  MenuFaderModeAuto->Checked = bModeManualFade ? false : true; // inverse
  MenuSendTiming->Checked = bSendTiming ? true : false;
  MenuRepeatModeA->Checked = bRepeatModeA ? true : false;
  MenuRepeatModeB->Checked = bRepeatModeB ? true : false;
  MenuShuffleModeA->Checked = bShuffleModeA ? true : false;
  MenuShuffleModeB->Checked = bShuffleModeB ? true : false;

  try
  {
    if (!SetVolumes() || !SetCurrentPlayer())
    {
      ShowMessage("Re-Install Windows Media Player 10 or above!");
      Release();
      Application->Terminate();
    }

    WindowsMediaPlayer1->settings->autoStart = false;
    WindowsMediaPlayer2->settings->autoStart = false;
  }
  catch(...) { ShowMessage("There was a problem setting the Volumes!"); }

  if (!bModeManualFade)
    StatusBar1->Panels->Items[2]->Text = "Auto";
  else
    StatusBar1->Panels->Items[2]->Text = "Manual";
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
  // Don't need to release ListA and ListB - their FormDestroy() gets called!
  // (I think the VCL calls FormDestroy for all forms created with CreateForm...)

// probably we don't need this either...
//  if (ProgressForm != NULL) ProgressForm->Release();

//  if (FDock != NULL) delete FDock;

#if DEBUG_ON
  MainForm->CWrite("\r\nFormDestroy() in FormMain()\r\n");
  FreeConsole();
#endif

#if !FREEWARE_EDITION
  if (PK != NULL) delete PK;
#endif

//  Application->Terminate();
//  if (gDock != NULL) delete gDock;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormClose(TObject* Sender, TCloseAction &Action)
{
  if (WindowsMediaPlayer1)
  {
    WindowsMediaPlayer1->settings->mute = true;
    WindowsMediaPlayer1->controls->stop();
  }
  if (WindowsMediaPlayer2)
  {
    WindowsMediaPlayer2->settings->mute = true;
    WindowsMediaPlayer2->controls->stop();
  }

  // Stop Timers
  AutoFadeTimer->Enabled = false;

  if (SFDlgForm != NULL)
  {
    SFDlgForm->Close();
    SFDlgForm->Release();
  }
  if (OFMSDlgForm != NULL)
  {
    OFMSDlgForm->Close();
    OFMSDlgForm->Release();
  }
  if (DirDlgForm != NULL)
  {
    DirDlgForm->Close();
    DirDlgForm->Release();
  }
  if (ListA != NULL)
  {
    ListA->Close();
    ListA->Release();
  }
  if (ListB != NULL)
  {
    ListB->Close();
    ListB->Release();
  }

  TRegHelper* pReg = NULL;

  try
  {
    pReg = new TRegHelper(true);

    if (pReg != NULL)
    {
      pReg->WriteSetting(SM_REGKEY_DIR_A, SaveDirA);
      pReg->WriteSetting(SM_REGKEY_DIR_B, SaveDirB);
      pReg->WriteSetting(SM_REGKEY_FADERMODE, bTypeCenterFade);
      pReg->WriteSetting(SM_REGKEY_FADERTYPE, bModeManualFade);
      pReg->WriteSetting(SM_REGKEY_SENDTIMING, bSendTiming);
      pReg->WriteSetting(SM_REGKEY_REPEAT_A, bRepeatModeA);
      pReg->WriteSetting(SM_REGKEY_REPEAT_B, bRepeatModeB);
      pReg->WriteSetting(SM_REGKEY_SHUFFLE_A, bShuffleModeA);
      pReg->WriteSetting(SM_REGKEY_SHUFFLE_B, bShuffleModeB);
      pReg->WriteSetting(SM_REGKEY_VOL_B, volB);
      pReg->WriteSetting(SM_REGKEY_VOL_B, volB);
      pReg->WriteSetting(SM_REGKEY_IMPORTEXT, ImportExt);
      pReg->WriteSetting(SM_REGKEY_EXPORTEXT, ExportExt);
    }
  }
  __finally
  {
    try { if (pReg != NULL) delete pReg; } catch(...) {}
  }

#if DEBUG_ON
  MainForm->CWrite("\r\nFormClose() in FormMain()\r\n");
#endif
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MenuHighPriorityClick(TObject* Sender)
{
  if (!WindowsMediaPlayer1 || !WindowsMediaPlayer2)
    return;

  // HIGH_PRIORITY_CLASS
  // NORMAL_PRIORITY_CLASS
  // REALTIME_PRIORITY_CLASS
  try
  {
    DWORD dwProcessId1;
    DWORD dwProcessId2;

    GetWindowThreadProcessId(WindowsMediaPlayer1->Handle,&dwProcessId1);
    GetWindowThreadProcessId(WindowsMediaPlayer2->Handle,&dwProcessId2);

    // Get handle to Process
    HANDLE hProcess1 = OpenProcess(PROCESS_QUERY_INFORMATION| // access flag
                            PROCESS_SET_INFORMATION,
                            false, // handle inheritance flag
                            dwProcessId1); // process identifier
    // Get handle to Process
    HANDLE hProcess2 = OpenProcess(PROCESS_QUERY_INFORMATION| // access flag
                            PROCESS_SET_INFORMATION,
                            false, // handle inheritance flag
                            dwProcessId2); // process identifier

    if (!hProcess1)
      return;

    if (!hProcess2)
    {
      CloseHandle(hProcess1);
      return;
    }

    int Priority1 = (int)GetPriorityClass(hProcess1);
    int Priority2 = (int)GetPriorityClass(hProcess2);

    if (Priority1 == HIGH_PRIORITY_CLASS || Priority2 == HIGH_PRIORITY_CLASS)
    {
      SetPriorityClass(hProcess1, NORMAL_PRIORITY_CLASS);
      SetPriorityClass(hProcess2, NORMAL_PRIORITY_CLASS);
    }
    else
    {
      SetPriorityClass(hProcess1, HIGH_PRIORITY_CLASS);
      SetPriorityClass(hProcess2, HIGH_PRIORITY_CLASS);
    }

    Priority1 = (int)GetPriorityClass(hProcess1);
    Priority2 = (int)GetPriorityClass(hProcess2);

    // Menu check-mark will reflect the true priority
    if (Priority1 == HIGH_PRIORITY_CLASS && Priority2 == HIGH_PRIORITY_CLASS)
      MenuHighPriority->Checked = true;
    else
      MenuHighPriority->Checked = false;

    CloseHandle(hProcess1);
    CloseHandle(hProcess2);
  }
  catch(...)
  {
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::File1Click(TObject* Sender)
{
  if (FileDialog(ListA, SaveDirA, ADD_A_TITLE))
    ListA->QueueFirst();
//  if (FileDialog(ListA, ADD_A_FILES, SaveDirA)) ListA->QueueFirst();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::File2Click(TObject* Sender)
{
  if (FileDialog(ListB, SaveDirB, ADD_B_TITLE))
    ListB->QueueFirst();
//  if (FileDialog(ListB, ADD_B_FILES, SaveDirB)) ListB->QueueFirst();
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::FileDialog(TPlaylistForm* f, String &d, String t)
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
      TOFMSDlgForm* fd = f->CreateFileDialog();

      if (fd != NULL)
      {
        // Filters
        fd->Filters = "All Files (*.*)|*.*|"
                    "Windows Media (*.wma)|*.wma|"
                    "MP3 (*.mp3)|*.mp3|"
                    "WAV (*.wav)|*.wav";

        MainForm->FilesAddedCount = 0;

        fd->ExecuteU(0, d, t); // no def utf-8 extension?

        if(fd->Result == IDOK)
        {
          // Press and hold Shift to bypass the file-extention filtering
          // (Checked in AddFileToListBox())
          GBypassFilters = (GetKeyState(VK_SHIFT) & 0x8000);

#if DEBUG_ON
          // Display for diagnostics
          if (fd->FileNameObjects != NULL && fd->FileNameObjects->Count > 0)
          {
            MainForm->CWrite( "\r\nPrinting selected-files list!!!!!!!\r\n");

            String s1 = "";

            for (int ii = 0; ii < fd->FileNameObjects->Count; ii++)
            {
              TWideItem* pWI = (TWideItem*)fd->FileNameObjects->Items[ii];

              if (pWI != NULL)
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

          if (fd->FileNameObjects != NULL && fd->FileNameObjects->Count > 0)
          {
            if (f == ListA) SaveDirA = WideToUtf8(fd->CurrentFolder);
            else SaveDirB = WideToUtf8(fd->CurrentFolder);

            int Count = fd->FileNameObjects->Count;

            TProgressForm::Init(Count);

            for (int ii = 0; ii < Count ; ii++)
            {
              Application->ProcessMessages();
              if (Application->Terminated || (int)GetAsyncKeyState(VK_ESCAPE) < 0)
                break;

              TWideItem* pWI = (TWideItem*)fd->FileNameObjects->Items[ii];

              if (pWI != NULL)
              {
                if (pWI->IsDirectory)
                {
                  if (SetCurrentDirectoryW(pWI->s.c_bstr()))
                    AddAllSongsToListBox(f);
                }
                else
                  AddFileToListBox(f, WideToUtf8(pWI->s), false);
              }

              TProgressForm::Move(ii);
            }

            TProgressForm::UnInit();
          }

          // Show the listbox
          if (f->CheckBox->Items->Count)
            ShowPlaylist(f);

          bRet = true;
        }
        // User canceled... only a problem if they added files via our custom mechanism...
        else if (FilesAddedCount > 0)
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
  }

  if (bRet)
    SetVolumes();

  return bRet;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::AddAllSongsToListBox(TPlaylistForm* f)
// Strings are in UTF-8 format!
{
  TStringList* sl = NULL;

  try
  {
    sl = new TStringList();

    RecurseFileAdd(sl);

    int Count = sl->Count;

    if (Count > 1)
    {
      TProgressForm::Init(Count);

      for (int ii = 0 ; ii < Count ; ii++)
      {
        Application->ProcessMessages();
        if (Application->Terminated || (int)GetAsyncKeyState(VK_ESCAPE) < 0)
          break;

        AddFileToListBox(f, sl->Strings[ii], false);
        TProgressForm::Move(ii);
      }

      TProgressForm::UnInit();

      if (FilesAddedCount > 0)
        ShowPlaylist(f);
    }
  }
  __finally
  {
    try { if (sl != NULL) delete sl; } catch(...) {}
  }
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::AddFileToListBox(TPlaylistForm* f, String sFile, bool bConvertToUtf8)
// Set bConvertToUtf8 to convert to utf8 unless it already IS utf8!
{
  if (f == NULL || sFile.Length() == 0)
    return false;

  if (!IsAudioFile(sFile))
    return false;

  try
  {
    if (bConvertToUtf8)
      sFile = AnsiToUtf8(sFile);

    f->CheckBox->Items->AddObject(sFile, (TObject*)f->TextColor);
    f->CheckBox->State[f->CheckBox->Items->Count-1] = cbGrayed;
    m_filesAddedCount++;
    return true;
  }
  catch(...) { return false; }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::RecurseFileAdd(TStringList* slFiles)
// Uses the WideString versions of the Win32 API FindFirstFile and FindNextFile directly
// and converts the resulting paths to UTF-8 for storage in an ordinary TStringList
//
// Use SetCurrentDirectory() to set our root directory or TOpenDialog sets it also...
{
  Application->ProcessMessages();

  if (slFiles == NULL) return;

  TStringList* slSubDirs = new TStringList();
  if (slSubDirs == NULL) return;

  TWin32FindDataW sr;
  HANDLE hFind = NULL;
  TFindexInfoLevels l = FindExInfoStandard; // FindExInfoBasic was defined later!
  TFindexSearchOps s = FindExSearchLimitToDirectories;

  try
  {
    hFind = FindFirstFileExW(L"*", l, &sr, s, NULL, (DWORD)FIND_FIRST_EX_LARGE_FETCH);

    // Get list of subdirectories into a stringlist
    if (hFind != INVALID_HANDLE_VALUE)
    {
      do
      {
        if ((sr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {
          int len = wcslen(sr.cFileName);

          if (len == 1 && sr.cFileName[0] == L'.')
            continue;
          if (len == 2 && sr.cFileName[0] == L'.' && sr.cFileName[1] == L'.')
            continue;

//          slSubDirs->Add(WideToUtf8(WideString(sr.cFileName)));
          slSubDirs->Add(WideToUtf8(sr.cFileName));
        }
      } while (FindNextFileW(hFind, &sr) == TRUE);
    }
  }
  __finally
  {
    try { if (hFind != NULL) FindClose(hFind); } catch(...) {}
  }

  AddFilesToStringList(slFiles);

  TProgressForm::Init(slSubDirs->Count);

  // Get songs in all subdirectories
  for (int ii = 0; ii < slSubDirs->Count; ii++)
  {
    Application->ProcessMessages();
    if (Application->Terminated || (int)GetAsyncKeyState(VK_ESCAPE) < 0)
      break;

    if (SetCurrentDirectoryW(Utf8ToWide(slSubDirs->Strings[ii]).c_bstr()))
    {
      RecurseFileAdd(slFiles);
      SetCurrentDirectoryW(L"..");
    }

    // Move Progress bar if it exists
    TProgressForm::Move(ii);
  }

  delete slSubDirs;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::AddFilesToStringList(TStringList* slFiles)
// slFiles is in UTF-8!
{
  TWin32FindDataW sr;
  HANDLE hFind = NULL;
  TFindexInfoLevels l = FindExInfoStandard; // FindExInfoBasic was defined later!
  TFindexSearchOps s = FindExSearchNameMatch;

  try
  {
    // Get the current directory
    WideString wdir = GetCurrentDirW();

    hFind = FindFirstFileExW(L"*", l, &sr, s, NULL, (DWORD)FIND_FIRST_EX_LARGE_FETCH);

    // Get list of files into a stringlist
    if (hFind != INVALID_HANDLE_VALUE)
    {
      WideString ws;

      // Don't add these file-types...
      int mask = FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN;

      do
      {
        if ((sr.dwFileAttributes & mask) == 0)
        {
          ws = wdir + L"\\" + WideString(sr.cFileName);
          slFiles->Add(WideToUtf8(ws));
        }
      } while (FindNextFileW(hFind, &sr) == TRUE);
    }
  }
  __finally
  {
    try { if (hFind != NULL) ::FindClose(hFind); } catch(...) {}
  }
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::IsAudioFile(String sFile)
// should work ok on UTF-8 strings
{
  if (IsUri(sFile)) return true; // pass through http:// links

  if (GBypassFilters) return true;

  String sExt = ExtractFileExt(sFile).LowerCase();

  if (sExt.IsEmpty()) return false;

  return sExt == ".mp3" || sExt == ".wma" || sExt == ".asf" || sExt == ".wav" ||
          sExt == ".mpa" || sExt == ".mpe" || sExt == ".m3u" || sExt == ".avi" || sExt == ".aac" ||
          sExt == ".adt" || sExt == ".adts" || sExt == ".mp2" || sExt == ".cda" ||
          sExt == ".au" || sExt == ".snd" || sExt == ".aif" || sExt == ".aiff" || sExt == ".aifc" ||
          sExt == ".mid" || sExt == ".midi" || sExt == ".rmi" || sExt == ".m4a";
}
//---------------------------------------------------------------------------
// Overloaded...

// Could have "file:/laptop/D:/path/file.wma" so the key to telling a URL from
// a drive letter is that url preambles are more than one char!
//
// sIn should be trimmed but does not need to be lower-case...
bool __fastcall TMainForm::IsUri(String sIn)
{
  return sIn.Pos(":/") > 2; // > 2 means you must have more than 1 char before the : (like "file:/")
}

bool __fastcall TMainForm::IsUri(WideString wIn)
{
  return wIn.Pos(":/") > 2; // > 2 means you must have more than 1 char before the : (like "file:/")
}
//---------------------------------------------------------------------------
// Overloaded...

bool __fastcall TMainForm::IsFileUri(String sIn)
{
  return sIn.LowerCase().Pos("file:/") == 1;
}

bool __fastcall TMainForm::IsFileUri(WideString wIn)
{
  wIn = LowerCaseW(wIn);
  return wIn.Pos("file:/") == 1;
}
//---------------------------------------------------------------------------
WideString __fastcall TMainForm::LowerCaseW(WideString s)
{
  WideChar c;

  int len = s.Length();

  for (int ii = 1; ii <= len; ii++)
  {
    c = s[ii];

    if (iswupper(c))
      s[ii] = towlower(c);
  }
  return WideString(s);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::VA_10Click(TObject* Sender)
{
  // Vol 1 10%
  volA = 10;
  SetVolumes();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::VA_25Click(TObject* Sender)
{
  // Vol 1 25%
  volA = 25;
  SetVolumes();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::VA_50Click(TObject* Sender)
{
  // Vol 1 50%
  volA = 50;
  SetVolumes();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::VA_75Click(TObject* Sender)
{
  // Vol 1 75%
  volA = 75;
  SetVolumes();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::VA_100Click(TObject* Sender)
{
  // Vol 1 100%
  volA = 100;
  SetVolumes();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::VB_10Click(TObject* Sender)
{
  // Vol 2 10%
  volB = 10;
  SetVolumes();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::VB_25Click(TObject* Sender)
{
  // Vol 2 25%
  volB = 25;
  SetVolumes();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::VB_50Click(TObject* Sender)
{
  // Vol 2 50%
  volB = 50;
  SetVolumes();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::VB_75Click(TObject* Sender)
{
  // Vol 2 75%
  volB = 75;
  SetVolumes();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::VB_100Click(TObject* Sender)
{
  // Vol 2 100%
  volB = 100;
  SetVolumes();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::TrackBar1Change(TObject* Sender)
{
  // Fader Moved
  SetVolumes();
  SetCurrentPlayer(); // Set CurrentPlayer variable (used for color-coding)
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Player1Next1Click(TObject* Sender)
{
  ListA->NextIndex = ListA->TargetIndex;
  ListA->NextPlayer();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Player2Next1Click(TObject* Sender)
{
  ListB->NextIndex = ListB->TargetIndex;
  ListB->NextPlayer();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MenuForceFadeClick(TObject* Sender)
{
  ForceFade();
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::ForceFade(void)
// Manually initiate an "auto-fade"
// Also triggers next Queued song
{
  if (!WindowsMediaPlayer1 || !WindowsMediaPlayer2 || bModeManualFade)
    return false;

  try
{
  // player 1 on now?
  if (ListB->Tag >= 0 && WindowsMediaPlayer1->playState == wmppsPlaying)
  {
    // Start Player 2
    if (WindowsMediaPlayer2->playState != wmppsPlaying)
    {
      WindowsMediaPlayer2->settings->mute = true;
      WindowsMediaPlayer2->controls->play();
      WindowsMediaPlayer2->settings->mute = false;
    }

    bFadeRight = true; // Set fade-direction
    AutoFadeTimer->Enabled = true; // Start a fade
    return(true);
  }

  // player 2 on now?
  if (ListA->Tag >= 0 && WindowsMediaPlayer2->playState == wmppsPlaying)
  {
    // Start Player 1
    if (WindowsMediaPlayer1->playState != wmppsPlaying)
    {
      WindowsMediaPlayer1->settings->mute = true;
      WindowsMediaPlayer1->controls->play();
      WindowsMediaPlayer1->settings->mute = false;
    }

    bFadeRight = false; // Set fade-direction
    AutoFadeTimer->Enabled = true; // Start a fade
    return(true);
  }
}
  catch(...)
{
  ShowMessage("ForceFade() threw an exception");
}

  return false;
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::SetVolumes(void)
{
  if (!WindowsMediaPlayer1 || !WindowsMediaPlayer2)
    return(false);

  try
{
  if (bTypeCenterFade)
  {
    if (TrackBar1->Position < 50)
      WindowsMediaPlayer1->settings->volume = volA;
    else
      WindowsMediaPlayer1->settings->volume =
        volA*((100-TrackBar1->Position)*2)/100;

    if (TrackBar1->Position >= 50)
      WindowsMediaPlayer2->settings->volume = volB;
    else
      WindowsMediaPlayer2->settings->volume =
        volB*(TrackBar1->Position*2)/100;
  }
  else
  {
    WindowsMediaPlayer1->settings->volume =
      (volA*(100-TrackBar1->Position))/100;

    WindowsMediaPlayer2->settings->volume =
      (volB*TrackBar1->Position)/100;
  }

  SetVolumeCheckmarksA(volA);
  SetVolumeCheckmarksB(volB);
}
  catch(...)
{
  return(false);
}

  return(true);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SetVolumeCheckmarksA(int v)
{
  try
  {
    VA_10->Checked = false;
    VA_25->Checked = false;
    VA_50->Checked = false;
    VA_75->Checked = false;
    VA_100->Checked = false;

    switch(v)
    {
      case 10:
        VA_10->Checked = true;
      break;

      case 25:
        VA_25->Checked = true;
      break;

      case 50:
        VA_50->Checked = true;
      break;

      case 75:
        VA_75->Checked = true;
      break;

      case 100:
        VA_100->Checked = true;
      break;

      default:
        VA_50->Checked = true;
      break;
  };
  }
  catch(...)
  {
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SetVolumeCheckmarksB(int v)
{
  try
  {
    VB_10->Checked = false;
    VB_25->Checked = false;
    VB_50->Checked = false;
    VB_75->Checked = false;
    VB_100->Checked = false;

    switch(v)
    {
      case 10:
        VB_10->Checked = true;
      break;

      case 25:
        VB_25->Checked = true;
      break;

      case 50:
        VB_50->Checked = true;
      break;

      case 75:
        VB_75->Checked = true;
      break;

      case 100:
        VB_100->Checked = true;
      break;

      default:
        VB_50->Checked = true;
      break;
  };
  }
  catch(...)
  {
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Exit1Click(TObject* Sender)
{
  Release();
  Application->Terminate();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MenuAboutClick(TObject* Sender)
{
  Application->CreateForm(__classid(TAboutForm), &AboutForm);
  AboutForm->ShowModal();
  AboutForm->Release();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ClearPlaylistStop1Click(TObject* Sender)
{
  if (ListA != NULL) ListA->ClearAndStop();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ClearPlaylistStop2Click(TObject* Sender)
{
  if (ListB != NULL) ListB->ClearAndStop();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MenuFaderModeAutoClick(TObject* Sender)
{
  if (!bModeManualFade)
  {
    bModeManualFade = true;
    MenuFaderModeAuto->Checked = false;
    MenuFaderModeAuto->Caption = "Fader Mode: Manual";
    MenuForceFade->Enabled = false;
    StatusBar1->Panels->Items[2]->Text = "Manual";
  }
  else
  {
    bModeManualFade = false;
    MenuFaderModeAuto->Checked = true;
    MenuFaderModeAuto->Caption = "Fader Mode: Auto";
    MenuForceFade->Enabled = true;
    StatusBar1->Panels->Items[2]->Text = "Auto";
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MenuFaderTypeNormalClick(TObject* Sender)
{
  if (!WindowsMediaPlayer1 || !WindowsMediaPlayer2)
    return;

  if (!bTypeCenterFade)
  {
    // Fader Mode - Center-Fade
    bTypeCenterFade = true;
    MenuFaderTypeNormal->Checked = false;
    MenuFaderTypeNormal->Caption = "Fader Type: Center";
  }
  else
  {
    // Fader Mode - Normal
    bTypeCenterFade = false;
    MenuFaderTypeNormal->Checked = true;
    MenuFaderTypeNormal->Caption = "Fader Type: Normal";
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ShowPlaylist(TPlaylistForm* f)
{
  if (f == NULL)
  {
    ShowMessage("null pointer in ShowPlaylist()");
    return;
  }

  try
  {
    if (f->CheckBox->Items->Count && f->Tag == -1)
      f->QueueFirst();

    f->SetTitleW();

    if (f->WindowState == wsMinimized)
      f->WindowState = wsNormal;

    // already showing? skip...
    if (f->Visible)
      return;

    f->Color = TColor(0xF5CFB8);

    f->Height = Height/2 + 3;
    f->Width = 3*Width/2;

    int borderWidth;
    int borderHeight;

    if (IsWinVistaOrHigher())
    {
      borderWidth = 2*GetSystemMetrics(SM_CXDLGFRAME);
      borderHeight = 2*GetSystemMetrics(SM_CYDLGFRAME);
    }
    else // xp
    {
      borderWidth = 0;
      borderHeight = 0;
    }

    if (f == ListA)
    {
      // Player 1
      f->Left = Left - borderWidth;
      f->Top = Top - f->Height - borderHeight;
    }
    else
    {
      // Player 2
      f->Left = (Left + Width) - f->Width + borderWidth;
      f->Top = Top + Height + borderHeight;
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
    MainForm->SetFocus();
  }
  catch(...) { ShowMessage("ShowPlaylist() threw an exception"); }
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::IsWinVistaOrHigher(void)
{
  OSVERSIONINFO vi;
  vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&vi);

  return vi.dwPlatformId == VER_PLATFORM_WIN32_NT && vi.dwMajorVersion >= 6;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ViewPlaylist1Click(TObject* Sender)
{
  ShowPlaylist(ListA);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ViewPlaylist2Click(TObject* Sender)
{
  ShowPlaylist(ListB);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::AutoFadeTimerEvent(TObject* Sender)
{
  if (!WindowsMediaPlayer1 || !WindowsMediaPlayer2) return;

  try
  {
    if (bFadeRight)
    {
      if (TrackBar1->Position < 100)
      {
        if (TrackBar1->Position <= 100-TrackBar1->Frequency)
          TrackBar1->Position += TrackBar1->Frequency;
        else
          TrackBar1->Position = 100;
      }
      else
      {
        AutoFadeTimer->Enabled = false;

        // this is a way to periodically return focus to this
        // main form...
        RestoreFocus();

        WindowsMediaPlayer1->settings->mute = true;
        WindowsMediaPlayer1->controls->stop();
        WindowsMediaPlayer1->settings->mute = false;

        // Queue next song
        WindowsMediaPlayer1->URL = ListA->GetNext();

        if (ListA->Tag < 0)
          if (FileDialog(ListA, SaveDirA, ADD_A_TITLE))
            ListA->QueueFirst();
      }
    }
    else
    {
      if (TrackBar1->Position > 0)
      {
        if (TrackBar1->Position >= TrackBar1->Frequency)
          TrackBar1->Position -= TrackBar1->Frequency;
        else
          TrackBar1->Position = 0;
      }
      else
      {
        AutoFadeTimer->Enabled = false;

        // this is a way to periodically return focus to this
        // main form...
        RestoreFocus();

        WindowsMediaPlayer2->settings->mute = true;
        WindowsMediaPlayer2->controls->stop();
        WindowsMediaPlayer2->settings->mute = false;

        // Queue next song
        WindowsMediaPlayer2->URL = ListB->GetNext();

        if (ListB->Tag < 0)
          if (FileDialog(ListB, SaveDirB, ADD_B_TITLE))
            ListB->QueueFirst();
      }
    }
  }
  catch(...) { ShowMessage("AutoFadeTimerEvent() threw an exception..."); }
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::SetCurrentPlayer(void)
{
  if (!WindowsMediaPlayer1 || !WindowsMediaPlayer2)
    return false;

  try
  {
    if (TrackBar1->Position != 100 && WindowsMediaPlayer1->playState == wmppsPlaying)
      CurrentPlayer |= 1;
    else
      CurrentPlayer &= ~1;

    if (TrackBar1->Position != 0 && WindowsMediaPlayer2->playState == wmppsPlaying)
      CurrentPlayer |= 2;
    else
      CurrentPlayer &= ~2;

    // Set Bevels for status bar
    static OldPlayer = -1;

    if (CurrentPlayer != OldPlayer)
    {
      if (CurrentPlayer & 1)
      {
        StatusBar1->Panels->Items[0]->Bevel = (TStatusPanelBevel)1;
        StatusBar1->Panels->Items[1]->Bevel = (TStatusPanelBevel)1;
      }
      else
      {
        StatusBar1->Panels->Items[0]->Bevel = (TStatusPanelBevel)0;
        StatusBar1->Panels->Items[1]->Bevel = (TStatusPanelBevel)0;
      }

      if (CurrentPlayer & 2)
      {
        StatusBar1->Panels->Items[3]->Bevel = (TStatusPanelBevel)1;
        StatusBar1->Panels->Items[4]->Bevel = (TStatusPanelBevel)1;
      }
      else
      {
        StatusBar1->Panels->Items[3]->Bevel = (TStatusPanelBevel)0;
        StatusBar1->Panels->Items[4]->Bevel = (TStatusPanelBevel)0;
      }

      OldPlayer = CurrentPlayer;
    }
  }
  catch(...) { return false; }

  return true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Play1Click(TObject* Sender)
{
  if (!WindowsMediaPlayer1)
    return;

  WindowsMediaPlayer1->settings->mute = true;
  WindowsMediaPlayer1->controls->play();
  WindowsMediaPlayer1->settings->mute = false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Stop1Click(TObject* Sender)
{
  if (!WindowsMediaPlayer1)
    return;

  WindowsMediaPlayer1->settings->mute = true;
  WindowsMediaPlayer1->controls->stop();
  WindowsMediaPlayer1->settings->mute = false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Pause1Click(TObject* Sender)
{
  if (!WindowsMediaPlayer1)
    return;

  if (WindowsMediaPlayer1->playState == wmppsPaused) // paused?
    WindowsMediaPlayer1->controls->play();
  else
    WindowsMediaPlayer1->controls->pause();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Play2Click(TObject* Sender)
{
  if (!WindowsMediaPlayer2)
    return;

  WindowsMediaPlayer1->settings->mute = true;
  WindowsMediaPlayer2->controls->play();
  WindowsMediaPlayer1->settings->mute = false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Stop2Click(TObject* Sender)
{
  if (!WindowsMediaPlayer2)
    return;

  WindowsMediaPlayer2->settings->mute = true;
  WindowsMediaPlayer2->controls->stop();
  WindowsMediaPlayer2->settings->mute = false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Pause2Click(TObject* Sender)
{
  if (!WindowsMediaPlayer2)
    return;

  if (WindowsMediaPlayer2->playState == wmppsPaused) // paused?
    WindowsMediaPlayer2->controls->play();
  else
    WindowsMediaPlayer2->controls->pause();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::RestoreFocus(void)
{
  if (ListA->Visible && ListA->Active && ListA->WindowState == wsMaximized) MainForm->SetFocus();
  else if (ListB->Visible && ListB->Active && ListB->WindowState == wsMaximized) MainForm->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ImportPlaylist1Click(TObject* Sender)
{
  if (ListA == NULL || ListA->IsImportDlg)
    return;

  TImportForm* id = ListA->CreateImportDialog();

  int Count = id->Dialog(ListA, DeskDirUtf8, "Import PlayerA Playlist");

  if (Count > 0)
  {
    SetVolumes();
    Application->ProcessMessages();

    // Show the listbox
    ShowPlaylist(ListA);
  }
  else if (Count == 0)
    ShowMessage("Unable to load playlist (is it empty?)\nIs your music drive plugged in?");

  ListA->DestroyImportDialog();
}

void __fastcall TMainForm::ImportPlaylist2Click(TObject* Sender)
{
  if (ListB == NULL)
    return;
  Application->CreateForm(__classid(TImportForm), &ImportForm);

  int Count = ImportForm->Dialog(ListB, DeskDirUtf8, "Import PlayerB Playlist");

  if (Count > 0)
  {
    SetVolumes();
    Application->ProcessMessages();

    // Show the listbox
    ShowPlaylist(ListB);
  }
  else if (Count == 0)
    ShowMessage("Unable to load playlist (is it empty?)\nIs your music drive plugged in?");

  ImportForm->Release();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ExportPlaylist1Click(TObject* Sender)
{
  Application->CreateForm(__classid(TExportForm), &ExportForm);

  int Count = ExportForm->Dialog(ListA, DeskDirUtf8, WideToUtf8(L"Export PlayerA Playlist"));

  if (Count == 0)
    ShowMessage("Unable to export list or list empty...");

  ExportForm->Release();
}

void __fastcall TMainForm::ExportPlaylist2Click(TObject* Sender)
{
  Application->CreateForm(__classid(TExportForm), &ExportForm);

  int Count = ExportForm->Dialog(ListB, DeskDirUtf8, WideToUtf8(L"Export PlayerB Playlist"));

  if (Count == 0)
    ShowMessage("Unable to export list or list empty...");

  ExportForm->Release();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MenuHelpClick(TObject* Sender)
{
  // launch default web-browser
  //ShellExecute(Handle, "open", "iexplore.exe", HELPSITE, NULL, SW_SHOW);
  ShellExecute(NULL, "open", HELPSITE, NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MenuRepeatModeAClick(TObject* Sender)
{
  if (bRepeatModeA)
  {
    MenuRepeatModeA->Checked = false;
    bRepeatModeA = false;
  }
  else
  {
    MenuRepeatModeA->Checked = true;
    bRepeatModeA = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MenuRepeatModeBClick(TObject* Sender)
{
  if (bRepeatModeB)
  {
    MenuRepeatModeB->Checked = false;
    bRepeatModeB = false;
  }
  else
  {
    MenuRepeatModeB->Checked = true;
    bRepeatModeB = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MenuShuffleModeAClick(TObject* Sender)
{
  if (bShuffleModeA)
  {
    MenuShuffleModeA->Checked = false;
    bShuffleModeA = false;
  }
  else
  {
    MenuShuffleModeA->Checked = true;
    bShuffleModeA = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MenuShuffleModeBClick(TObject* Sender)
{
  if (bShuffleModeB)
  {
    MenuShuffleModeB->Checked = false;
    bShuffleModeB = false;
  }
  else
  {
    MenuShuffleModeB->Checked = true;
    bShuffleModeB = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MenuSendTimingClick(TObject* Sender)
{
  if (bSendTiming)
  {
    bSendTiming = false;
    MenuSendTiming->Checked = false;
  }
  else
  {
    bSendTiming = true;
    MenuSendTiming->Checked = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MenuExportSongFilesandListsClick(TObject* Sender)
{
  if (DirDlgForm != NULL)
    return;

   // Copy all song-list files to directory user selects
  if ((ListA != NULL && ListA->CheckBox->Items->Count != 0) ||
                    (ListB != NULL && ListB->CheckBox->Items->Count != 0))
  {
    MenuAutoFitToDVDCDClick(NULL);

    // Return if user needs to modify the lists due to the
    // cumulative size of the music files
    if (ComputeDiskSpace(DISKSPACE_MESSAGEBOX_YESNO) <= 0)
      return;

    Application->CreateForm(__classid(TDirDlgForm), &DirDlgForm);

    if (DirDlgForm == NULL)
      return;

    DirDlgForm->AutoScroll = false; // turn off autoscroll

    // Setting CSIDL_MYMUSIC works but the user can't go up from there!
    WideString wUserDir = DirDlgForm->Execute(CSIDL_DESKTOPDIRECTORY);

    DirDlgForm->Close();
    DirDlgForm->Release();

    if (wUserDir.IsEmpty())
      return;

    try
    {
      wUserDir += "\\SwiftMiX";

      if (DirectoryExistsW(wUserDir))
      {
        WideString wMsg = "Old directory already exists:\n\n" +
          wUserDir + "\n\nPlease delete it first...";
        ShowMessageW(wMsg);
        return;
      }
      else
        CreateDirectoryW(wUserDir.c_bstr(), NULL); // Create base directory

      // Save the playlists
      Application->CreateForm(__classid(TExportForm), &ExportForm);

      if (ExportForm == NULL)
        return;

      // EXPORT_EXT is wpl so we need to set the bSaveAsUtf8 flag... since all the files will be
      // copied into the same directory with the associated lists, we just want the file-name in the list,
      // not the path
      WideString wFile = wUserDir + "\\SwiftMiXA." + EXPORT_EXT;
      ExportForm->NoDialogW(ListA, wFile, EXPORT_PATH_NONE, true, false);
      wFile = wUserDir + "\\SwiftMiXB." + EXPORT_EXT;
      ExportForm->NoDialogW(ListB, wFile,  EXPORT_PATH_NONE, true, false);

      ExportForm->Close();
      ExportForm->Release();
      ExportForm = NULL;

      CopyMusicFiles(ListA, wUserDir);
      CopyMusicFiles(ListB, wUserDir);
      TProgressForm::UnInit();
    }
    catch (...) { }
  }
  else
    ShowMessage("Both lists are empty!");
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CopyMusicFiles(TPlaylistForm* f, WideString wUserDir)
{
  try
  {
    int count = f->CheckBox->Items->Count;

    TProgressForm::Init(count, 5);

    WideString wSourcePath, wDestPath;
    String uSourcePath, uFileName;

    // Copy ListA files
    for (int ii = 0; ii < count; ii++)
    {
      Application->ProcessMessages();
      if (Application->Terminated || (int)GetAsyncKeyState(VK_ESCAPE) < 0)
      {
        TProgressForm::UnInit();
        return;
      }

      uSourcePath = f->CheckBox->Items->Strings[ii];

      // Note that we use the old UTF-8 string "ListA->CheckBox->Items[ii]"
      // to run the Ansi ExtractFileName() on and path should be UTF-8 too...
      // THEN we convert to wide!
      uFileName = ExtractFileName(uSourcePath);

      if (uFileName.Length() > 0)
      {
        wSourcePath = Utf8ToWide(uSourcePath);

        wDestPath = wUserDir + WideString("\\") + Utf8ToWide(uFileName);

        if (CopyFileW(wSourcePath.c_bstr(), wDestPath.c_bstr(), FALSE) == 0)
          if (PromptAbort(wSourcePath))
            return;
      }

      TProgressForm::Move(ii);
    }
  }
  catch(...) {}
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::PromptAbort(WideString s)
{
  WideString sMsg = WideString("Unable to copy file:\n\n") + s +
                                                WideString("\n\nAbort?");

  if (MessageBoxW(Handle, sMsg.c_bstr(), L"File Copy Error", MB_ICONQUESTION +
                                          MB_YESNO + MB_DEFBUTTON1) == IDYES)
  {
    TProgressForm::UnInit();
    return true;
  }

  return false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MenuViewDiscSpaceRequiredClick(TObject* Sender)
{
  ComputeDiskSpace(DISKSPACE_MESSAGEBOX_OK);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MenuAutoFitToDVDCDClick(TObject* Sender)
{
  if (AutoSizeForm != NULL) return;
  Application->CreateForm(__classid(TAutoSizeForm), &AutoSizeForm);
  if (AutoSizeForm == NULL) return;

  if (AutoSizeForm->ShowModal() == mrOk)
  {
    __int64 val = ComputeDiskSpace(DISKSPACE_MESSAGEBOX_NONE);
    __int64 temp;

    if (val > AutoSizeForm->DiskSize)
    {
      temp = val-AutoSizeForm->DiskSize;

      // Call function to randomly remove at least "diff" bytes
      // alternating between lists
      __int64 newval = RandomRemove(temp);

      if (newval == temp)
        ShowMessage("User abort before operation was compleated!");
      else
      {
        String SizeStr = FormatFloat("#,##0", (double)newval);

        ShowMessage("Successfully removed songs in order to fit\n"
          "the target media...\n\n"
          "Now under media size-limit by: " + SizeStr + " Bytes");
      }
    }
//    else
//    {
//      temp = AutoSizeForm->DiskSize-val;
//      String SizeStr = FormatFloat("#,##0", (double)temp);
//      ShowMessage("Under media size-limit by: " + SizeStr + " Bytes");
//    }
}

  // Release the resources
  AutoSizeForm->Close();
  AutoSizeForm->Release();
  AutoSizeForm = NULL;
}
//---------------------------------------------------------------------------
__int64 __fastcall TMainForm::ComputeDiskSpace(int Mode)
// Add up # bytes needed
//
// Returns -1 if user wants to quit...
// otherwise returns the total # bytes required for
// both lists.
//
// Mode:
// DISKSPACE_MESSAGEBOX_NONE  0 // no box...
// DISKSPACE_MESSAGEBOX_YESNO 1
// DISKSPACE_MESSAGEBOX_OK    2
{
  WideString temp;

  unsigned __int64 total_size_A = 0;

  int cA = ListA->CheckBox->Items->Count;
  int cB = ListB->CheckBox->Items->Count;
//  int total = cA + cB;

  TProgressForm::Init(cA+cB);

  // Total size for list A
  for (int ii = 0; ii < cA; ii++)
  {
    Application->ProcessMessages();
    if (Application->Terminated || (int)GetAsyncKeyState(VK_ESCAPE) < 0)
      return -1;

    temp = Utf8ToWide(ListA->CheckBox->Items->Strings[ii]);

    if (FileExistsW(temp))
    {
      // Open file to get size
      HANDLE h = CreateFileW(temp.c_bstr(),
                         GENERIC_READ,
                         0,
                         NULL,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL,
                         NULL);

      if (h != INVALID_HANDLE_VALUE)
      {
        total_size_A += GetFileSize(h, NULL);
        CloseHandle(h);
      }
    }

    TProgressForm::Move(ii);
  }

  unsigned __int64 total_size_B = 0;

  // Total size for list B
  for (int ii = 0; ii < cB; ii++)
  {
    Application->ProcessMessages();
    if (Application->Terminated || (int)GetAsyncKeyState(VK_ESCAPE) < 0)
      return -1;

    temp = Utf8ToWide(ListB->CheckBox->Items->Strings[ii]);

    if (FileExistsW(temp))
    {
      // Open file to get size
      HANDLE h = CreateFileW(temp.c_bstr(),
                         GENERIC_READ,
                         0,
                         NULL,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL,
                         NULL);

      if (h != INVALID_HANDLE_VALUE)
      {
        total_size_B += GetFileSize(h, NULL);
        CloseHandle(h);
      }
    }

    TProgressForm::Move(ii);
  }

  TProgressForm::UnInit();

  unsigned __int64 total_size = total_size_A + total_size_B;

  // Quit now if not displaying a lot of info
  if (Mode == 0)
    return((__int64)total_size);

  String AKbMbGb, BKbMbGb, TKbMbGb;
  String ASizeStr, BSizeStr, TSizeStr;

  double KB, MB, GB;

  KB = 0.;
  MB = 0.;
  GB = 0.;

  if (total_size_A > 0.)
  {
    KB = total_size_A/1024.;
    if (KB >= 1024.) MB = KB/1024.;
    if (MB >= 1024.) GB = MB/1024.;
  }


  if (GB > 0)
  {
    ASizeStr = FormatFloat("#,##0.00", GB);
    AKbMbGb = " GiB";
  }
  else if (MB > 0)
  {
    ASizeStr = FormatFloat("#,##0.00", MB);
    AKbMbGb = " MiB";
  }
  else
  {
    ASizeStr = FormatFloat("#,##0.00", KB);
    AKbMbGb = " KiB";
  }

  KB = 0.;
  MB = 0.;
  GB = 0.;

  if (total_size_B > 0.)
  {
    KB = total_size_B/1024.;
    if (KB >= 1024.) MB = KB/1024.;
    if (MB >= 1024.) GB = MB/1024.;
  }

  if (GB > 0.)
  {
    BSizeStr = FormatFloat("#,##0.00", GB);
    BKbMbGb = " GiB";
  }
  else if (MB > 0.)
  {
    BSizeStr = FormatFloat("#,##0.00", MB);
    BKbMbGb = " MiB";
  }
  else
  {
    BSizeStr = FormatFloat("#,##0.00", KB);
    BKbMbGb = " KiB";
  }

  KB = 0.;
  MB = 0.;
  GB = 0.;

  if (total_size > 0.)
  {
    KB = total_size/1024.;

    if (KB >= 1024.)
      MB = KB/1024.;

    if (MB >= 1024.)
      GB = MB/1024.;
  }

  if (GB > 0.)
  {
    TSizeStr = FormatFloat("#,##0.00", GB);
    TKbMbGb = " GiB";
  }
  else if (MB > 0.)
  {
    TSizeStr = FormatFloat("#,##0.00", MB);
    TKbMbGb = " MiB";
  }
  else
  {
    TSizeStr = FormatFloat("#,##0.00", KB);
    TKbMbGb = " KiB";
  }

  String ABytesStr = FormatFloat("#,#0", total_size_A);
  String BBytesStr = FormatFloat("#,#0", total_size_B);
  String TBytesStr = FormatFloat("#,#0", total_size);

  String s =
            "List A Size: " + ASizeStr + AKbMbGb +
              " (" + ABytesStr + " bytes)(" + cA + " Songs)\n"
            "List B Size: " + BSizeStr + BKbMbGb +
              " (" + BBytesStr + " bytes)(" + cB + " Songs)\n\n"
            "Total Size: " + TSizeStr + TKbMbGb +
              " (" + TBytesStr + " bytes)\n\n"
            "Note: A typical DVD holds 4.38 GiB\n"
            "(A double-layer DVD holds 7.95 GiB)\n"
            "A typical CD-ROM holds 703.1 MiB";


  if (Mode == 1)
  {
    s += "\n\nOK to continue with export?";

    int button = MessageBox(Handle, s.c_str(), "Export", MB_ICONQUESTION + MB_YESNO + MB_DEFBUTTON2);

    if (button == IDNO) return (-1);
  }
  else if (Mode == 2)
    MessageBox(Handle, s.c_str(), "Disc-Space", MB_ICONINFORMATION + MB_OK);

  return((__int64)total_size);
}
//---------------------------------------------------------------------------
unsigned __int64 __fastcall TMainForm::RandomRemove(unsigned __int64 TargetBytes)
{
  unsigned __int64 acc = 0;

  TPlaylistForm* fp;

  WideString temp;

  while(ListA->CheckBox->Items->Count > 0 || ListB->CheckBox->Items->Count > 0)
  {
    // Delete from the most-populated list
    fp = (ListA->CheckBox->Items->Count > 0 && ListA->CheckBox->Items->Count > ListB->CheckBox->Items->Count) ? ListA : ListB;

    if (fp->CheckBox->Items->Count > 0)
    {
      int rand_idx = random(fp->CheckBox->Items->Count);

      temp = fp->CheckBox->Items->Strings[rand_idx];

      if (FileExistsW(temp))
      {
        // Open file to get size
        HANDLE h = CreateFileW(temp.c_bstr(),
                           GENERIC_READ,
                           0,
                           NULL,
                           OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL,
                           NULL);

        if (h != INVALID_HANDLE_VALUE)
        {
          int file_size = GetFileSize(h, NULL);

          int button = IDYES;

          if (bAutoSizePrompt)
          {
            WideString S_Player = fp == ListA ? L"A" : L"B";
            WideString s = WideString("Remove: \"") + temp + WideString("\" (") + WideString(file_size) +
                  WideString(" bytes) from Player ") + S_Player + WideString("'s list?");

            button = MessageBoxW(Handle, s.c_bstr(), L"Remove Song?", MB_ICONQUESTION + MB_YESNOCANCEL + MB_DEFBUTTON2);
          }

          if (button == IDYES)
          {
            acc += file_size;
            // Remove the item
            fp->CheckBox->Items->Delete(rand_idx);
          }

          CloseHandle(h);

          if (button == IDCANCEL) return(TargetBytes);
        }
      }

      if (acc >= TargetBytes)
        return(acc-TargetBytes);
    }
  }

  // Error
  return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::UpDown2Changing(TObject* Sender, bool &AllowChange)
{
  FadeAt = ((TUpDown*)Sender)->Position;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::UpDown1Changing(TObject* Sender, bool &AllowChange)
{
  int Pos = ((TUpDown*)Sender)->Position;

  if (Pos <= 0)
  {
    Pos = -Pos;
    AutoFadeTimer->Interval = 50*Pos;
    TrackBar1->Frequency = 1;
  }
  else
  {
    AutoFadeTimer->Interval = 50;
    TrackBar1->Frequency = Pos;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::WindowsMediaPlayerError(TObject *Sender)
{
  TWindowsMediaPlayer* wmp = dynamic_cast<TWindowsMediaPlayer*>(Sender);
  if (wmp == NULL) return;
  //TPlaylistForm* pl = wmp->Tag == PLAYER_1 ? ListA : ListB;
  //if (pl == NULL) return;

//  String sPlayer = (wmp->Tag == PLAYER_1) ? "A" : "B";
//  int errCount = wmp->Error->get_errorCount();
//
//  if (errCount)
//  {
//    int err;
//    wmp->Error->get_item(0, &err);
//    ShowMessage("Error in player " + sPlayer + ":" + String(err));
//  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::WindowsMediaPlayer1OpenStateChange(TObject* Sender, long NewState)
{
  if (ListA != NULL)
    ListA->OpenStateChange((WMPOpenState)NewState);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::WindowsMediaPlayer1PlayStateChange(TObject* Sender, long NewState)
{
  if (ListA != NULL)
    ListA->PlayStateChange((WMPPlayState)NewState);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::WindowsMediaPlayer1PositionChange(TObject* Sender, double oldPosition, double newPosition)
{
  if (ListA != NULL)
    ListA->PositionChange(oldPosition, newPosition);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::WindowsMediaPlayer1MediaChange(TObject* Sender, LPDISPATCH Item)
{
  // Populate MediaInfo struct
  if (ListA != NULL)
    ListA->GetSongInfo();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::WindowsMediaPlayer2OpenStateChange(TObject* Sender, long NewState)
{
  if (ListB != NULL)
    ListB->OpenStateChange((WMPOpenState)NewState);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::WindowsMediaPlayer2PlayStateChange(TObject* Sender, long NewState)
{
  if (ListB != NULL)
    ListB->PlayStateChange((WMPPlayState)NewState);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::WindowsMediaPlayer2PositionChange(TObject* Sender, double oldPosition, double newPosition)
{
  if (ListB != NULL)
    ListB->PositionChange(oldPosition, newPosition);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::WindowsMediaPlayer2MediaChange(TObject* Sender, LPDISPATCH Item)
{
  // Populate MediaInfo struct
  if (ListB != NULL)
    ListB->GetSongInfo();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::WMDropFile(TWMDropFiles &Msg)
{
  if (Msg.Drop == 0) return;

  try
  {
    if (ListA == NULL || ListB == NULL ||
      WindowsMediaPlayer1 == NULL || WindowsMediaPlayer2 == NULL) return;

    // Which player are we over?
    int left = WindowsMediaPlayer1->Left;
    int right = left + WindowsMediaPlayer1->Width;

    int topA = WindowsMediaPlayer1->Top;
    int bottomA = topA + WindowsMediaPlayer1->Height;

    int topB = WindowsMediaPlayer2->Top;
    int bottomB = topB + WindowsMediaPlayer2->Height;

    //get drop location
    TPoint p;
    ::DragQueryPoint((HDROP)Msg.Drop, &p);

    // Convert drop-coordinates to client
    ::ScreenToClient(NULL, &p);

    if (p.x > left && p.x < right)
    {
      if (p.y > topA && p.y < bottomA)
        LoadListWithDroppedFiles(Msg, ListA);
      else if (p.y > topB && p.y < bottomB)
        LoadListWithDroppedFiles(Msg, ListB);
    }
  }
  // NOTE: Unlike C#, __finally does NOT get called if we execute a return!!!
  __finally
  {
    try { ::DragFinish((HDROP)Msg.Drop); } catch(...) {}
    Msg.Result = 0;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LoadListWithDroppedFiles(TWMDropFiles &Msg, TPlaylistForm* f)
{
  if (f == NULL)
    return;

  HCURSOR hcurSave = NULL;
  wchar_t* pBuf = NULL;
  TStringList* sl = NULL;
  ImportForm = NULL;

  FilesAddedCount = 0;

  try
  {
    //get dropped files count
    int DroppedCount = ::DragQueryFileW((HDROP)Msg.Drop, -1, NULL, 0);

    if (DroppedCount == 0)
      return;

    // Set the cursor to the hourglass and save the previous cursor.
    hcurSave = ::SetCursor(::LoadCursor(NULL, IDC_WAIT));

    TProgressForm::Init(DroppedCount);

    if (ImportForm == NULL)
      Application->CreateForm(__classid(TImportForm), &ImportForm);

    pBuf = new wchar_t[MAX_PATH];
    sl = new TStringList();
//    sl->Sorted = true; // don't want to mess up a user-chosen order...

    for (int ii = 0; ii < DroppedCount; ii++)
    {
      ::DragQueryFileW((HDROP)Msg.Drop, ii, pBuf, MAX_PATH);

      if (*pBuf != L'\0')
        sl->Add(WideToUtf8(pBuf));

      TProgressForm::Move(ii);
    }

    // Repair a quirky list-system... (not needed after the first item has been added to a listbox!)
    if (sl->Count > 1 && f->CheckBox->Items->Count == 0)
      sl->Exchange(0, sl->Count-1);

    // Press and hold Shift to bypass the file-extention filtering
    this->GBypassFilters = (GetKeyState(VK_SHIFT) & 0x8000);

    TProgressForm::Init(sl->Count);

    WideString SaveDir = GetCurrentDirW(); // Save

    for (int ii = 0; ii < sl->Count; ii++)
    {
      Application->ProcessMessages();
      if (Application->Terminated || (int)GetAsyncKeyState(VK_ESCAPE) < 0)
        break;

      String sFile = sl->Strings[ii]; // these strings are in UTF-8!

      String Ext = ExtractFileExt(sFile).LowerCase();

      if (Ext.IsEmpty() && DirectoryExistsW(Utf8ToWide(sFile)))
      {
        SetCurrentDirW(Utf8ToWide(sFile));
        AddAllSongsToListBox(f); // recurse add folder and sub-folder's songs to list
      }
      else if (this->GBypassFilters || Ext == ".wpl" || Ext == ".m3u8" || Ext == ".m3u" ||
                            Ext == ".asx" || Ext == ".xspf" || Ext == ".wax" ||
                            Ext == ".wmx" || Ext == ".wvx" ||  Ext == ".pls" || Ext == ".txt")
        FilesAddedCount += ImportForm->NoDialog(f, sFile,
                ImportForm->GetMode(Ext, IMPORT_MODE_AUTO)); // Load the playlist
      else if (AddFileToListBox(f, sFile, false))
        FilesAddedCount++;

      TProgressForm::Move(ii);
    }

    SetCurrentDirW(SaveDir); // Restore

    if (FilesAddedCount > 0)
      ShowPlaylist(f);
    else
      ShowMessage("Song(s) may not have been added because either they did not\n"
             "appear to be sound files or the file was not found.\n\n"
             "Perhaps your music drive USB cable is unplugged?\n\n"
             "(To bypass the filter-list, press and hold SHIFT until you release\n"
             "the mouse button and drag/drop the file(s) again.)");
  }
  // NOTE: Unlike C#, __finally does NOT get called if we execute a return!!!
  __finally
  {
    try { TProgressForm::UnInit(); } catch(...) {}

    try { if (ImportForm != NULL) ImportForm->Release(); } catch(...) {}
    try { if (pBuf != NULL) delete [] pBuf; } catch(...) {}
    try { if (sl != NULL) delete sl; } catch(...) {}

    // Restore the previous cursor.
    try { if (hcurSave != NULL) ::SetCursor(hcurSave); } catch(...) {}
  }
}
//---------------------------------------------------------------------------
// UTF8 Encode/Decode
//---------------------------------------------------------------------------
String __fastcall TMainForm::AnsiToUtf8(String sIn)
// Code to convert ANSI to UTF-8 (Works!)
{
  if (sIn.IsEmpty()) return "";

  // Use the MultiByteToWideChar function to determine the size of the UTF-16 representation of the string. You use
  // this size to allocate a new buffer that can hold the UTF-16 version of the string.
  DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, sIn.c_str(), -1, NULL, 0);
  wchar_t *pwText = new wchar_t[dwNum];

  // The MultiByteToWideChar function takes the ASCII string and converts it into UTF-16, storing it in pwText.
  MultiByteToWideChar(CP_ACP, 0, sIn.c_str(), -1, pwText, dwNum);

  // The WideCharToMultiByte function tells you the size of the returned string so you can create a buffer for the UTF-8 representation.
  dwNum = WideCharToMultiByte(CP_UTF8, 0, pwText, -1, NULL, 0, NULL, NULL);
  char *psText = new char[dwNum];

  // Convert the UTF-16 string into UTF-8, storing the result into psText.
  WideCharToMultiByte(CP_UTF8, 0, pwText, -1, psText, dwNum, NULL, NULL);

  delete [] pwText;

  // Convert to VCL String.
  if (dwNum > 1) sIn = String(psText, dwNum-1);
  else sIn = "";

  delete [] psText;

  return sIn;
}
//---------------------------------------------------------------------------
String __fastcall TMainForm::Utf8ToAnsi(String sIn)
// Code to convert UTF-8 to ASCII
{
  // Use the MultiByteToWideChar function to determine the size of the UTF-16 representation of the string. You use
  // this size to allocate a new buffer that can hold the UTF-16 version of the string.
  DWORD dwNum = MultiByteToWideChar(CP_UTF8, 0, sIn.c_str(), -1, NULL, 0);
  wchar_t *pwText = new wchar_t[dwNum];

  // The MultiByteToWideChar function takes the UTF-8 string and converts it into UTF-16, storing it in pwText.
  MultiByteToWideChar(CP_UTF8, 0, sIn.c_str(), -1, pwText, dwNum);

  // The WideCharToMultiByte function tells you the size of the returned string so you can create a buffer for the ANSI representation.
  dwNum = WideCharToMultiByte(CP_ACP, 0, pwText, -1, NULL, 0, NULL, NULL);
  char *psText = new char[dwNum];

  // Convert the UTF-16 string into ANSI, storing the result into psText.
  WideCharToMultiByte(CP_ACP, 0, pwText, -1, psText, dwNum, NULL, NULL);

  delete [] pwText;

  // Convert to VCL String.
  if (dwNum > 1) sIn = String(psText, dwNum-1);
  else sIn = "";

  delete [] psText;

  return sIn;
}
//---------------------------------------------------------------------------
WideString __fastcall TMainForm::Utf8ToWide(String sIn)
// Code to convert UTF-8 to UTF-16 (wchar_t)
{
  WideString sWide = L"";

  if (sIn.Length() > 0)
  {
    wchar_t* pwText = NULL;

    try
    {
      // Use the MultiByteToWideChar function to determine the size of the UTF-16 representation of the string. You use
      // this size to allocate a new buffer that can hold the UTF-16 version of the string.
      DWORD dwNum = MultiByteToWideChar(CP_UTF8, 0, sIn.c_str(), -1, NULL, 0);
      wchar_t *pwText = new wchar_t[dwNum];

      if (pwText != NULL)
      {
        // The MultiByteToWideChar function takes the UTF-8 string and converts it into UTF-16, storing it in pwText.
        MultiByteToWideChar(CP_UTF8, 0, sIn.c_str(), -1, pwText, dwNum);

        // Convert to VCL WideString.
        if (dwNum > 1)
          sWide = WideString(pwText, dwNum-1);
      }
    }
    // NOTE: Unlike C#, __finally does NOT get called if we execute a return!!!
    __finally
    {
      try { if (pwText != NULL) delete [] pwText; } catch(...) {}
    }
  }

  return sWide;
}
//---------------------------------------------------------------------------
String __fastcall TMainForm::WideToUtf8(WideString sIn)
// Code to convert UTF-16 (WideString or WideChar wchar_t) to UTF-8
{
  if (sIn.IsEmpty()) return "";

  int nLenUtf16 = sIn.Length();

  int nLenUtf8 = WideCharToMultiByte(CP_UTF8, // UTF-8 Code Page
    0, // No special handling of unmapped chars
    sIn.c_bstr(), // wide-character string to be converted
    nLenUtf16,
    NULL, 0, // No output buffer since we are calculating length
    NULL, NULL); // Unrepresented char replacement - Use Default

  // nNameLen does NOT appear to include the NULL character!
  char* buf = new char[nLenUtf8];

  WideCharToMultiByte(CP_UTF8, // UTF-8 Code Page
    0, // No special handling of unmapped chars
    sIn.c_bstr(), // wide-character string to be converted
    nLenUtf16,
    buf,
    nLenUtf8,
    NULL, NULL); // Unrepresented char replacement - Use Default

  String sOut = String(buf, nLenUtf8); // there is no null written...
  delete [] buf;

  return sOut;
}
//---------------------------------------------------------------------------
String __fastcall TMainForm::ReplaceAll(String sIn, char c1, char c2)
{
  return ReplaceAll(sIn, String(c1), String(c2));
}

String __fastcall TMainForm::ReplaceAll(String sIn, String S1, String S2)
{
  TReplaceFlags rFlags = (TReplaceFlags() << rfReplaceAll << rfIgnoreCase);
  return StringReplace(sIn, S1, S2, rFlags);
}
//---------------------------------------------------------------------------
WideString __fastcall TMainForm::ReplaceAllW(String sIn, wchar_t c1, wchar_t c2)
{
  return ReplaceAllW(sIn, WideString(c1), WideString(c2));
}

WideString __fastcall TMainForm::ReplaceAllW(WideString sIn, WideString S1, WideString S2)
{
  TReplaceFlags rFlags = (TReplaceFlags() << rfReplaceAll << rfIgnoreCase);
  return StringReplace(sIn, S1, S2, rFlags);
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::SetCurrentDirW(WideString wDir)
// Set the current directory from a WideString
{
  return SetCurrentDirectoryW(wDir.c_bstr());
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::DirectoryExistsW(WideString wPath)
{
  // mapiwin.h and winnt.h
  // FILE_ATTRIBUTE_READONLY         0x00000001
  // FILE_ATTRIBUTE_HIDDEN           0x00000002
  // FILE_ATTRIBUTE_SYSTEM           0x00000004
  // FILE_ATTRIBUTE_DIRECTORY        0x00000010
  // FILE_ATTRIBUTE_ARCHIVE          0x00000020
  // FILE_ATTRIBUTE_NORMAL           0x00000080
  // FILE_ATTRIBUTE_TEMPORARY        0x00000100
  DWORD dwAttrib = GetFileAttributesW(wPath.c_bstr());

//#if DEBUG_ON
//    if (dwAttrib == INVALID_FILE_ATTRIBUTES)
//      MainForm->CWrite( "\r\nDirectoryExistsW No Directory: " + String(sPath) + "\r\n");
//    else
//      MainForm->CWrite( "\r\nDirectoryExistsW Attributes: " + IntToHex((int)dwAttrib, 8) + "\r\n");
//#endif

  return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
         (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::FileExistsW(WideString wPath)
{
  // mapiwin.h and winnt.h
  // FILE_ATTRIBUTE_READONLY         0x00000001
  // FILE_ATTRIBUTE_HIDDEN           0x00000002
  // FILE_ATTRIBUTE_SYSTEM           0x00000004
  // FILE_ATTRIBUTE_DIRECTORY        0x00000010
  // FILE_ATTRIBUTE_ARCHIVE          0x00000020
  // FILE_ATTRIBUTE_NORMAL           0x00000080
  // FILE_ATTRIBUTE_TEMPORARY        0x00000100
  DWORD dwAttrib = GetFileAttributesW(wPath.c_bstr());

//#if DEBUG_ON
//    if (dwAttrib == INVALID_FILE_ATTRIBUTES)
//      MainForm->CWrite( "\r\nFileExistsW No Directory: " + String(sPath) + "\r\n");
//    else
//      MainForm->CWrite( "\r\nFileExistsW Attributes: " + IntToHex((int)dwAttrib, 8) + "\r\n");
//#endif

  return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
         !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
//---------------------------------------------------------------------------
WideString __fastcall TMainForm::ExtractFileExtW(WideString sIn)
// Return ".txt" including the period, or empty-string...
{
  int len = sIn.Length();
  int idx = len;
  wchar_t c;

  for(;;)
  {
    if (idx == 0)
      break;

    c = sIn[idx];

    if (c == '.')
      break;

    idx--;
  }

  if (idx != 0)
    return sIn.SubString(idx, len-idx+1);

  return "";
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::WriteStringToFileW(WideString wPath, String sInfo)
// Writes sInfo (ANSI or UTF-8) to a UTF-16 path
{
  bool bRet = false;
  FILE* f = NULL;

  try
  {
    // open/create file for writing in text-mode
    f = _wfopen(wPath.c_bstr(), L"wb");

    unsigned length = sInfo.Length();

    if (f != NULL)
      if (fwrite(sInfo.c_str(), sizeof(char), length, f) == length)
        bRet = true;
  }
  __finally
  {
    try { if (f != NULL) fclose(f); } catch(...) {}
  }

  return bRet;
}
//---------------------------------------------------------------------------
String __fastcall TMainForm::ReadStringFromFileW(WideString wPath)
{
  FILE* f = NULL;
  char* buf = NULL;

  String sOut;

  try
  {
    // open/create file for reading in text-mode
    f = _wfopen(wPath.c_bstr(), L"rb");

    if (f != NULL)
    {
      // get file-length
      fseek(f, 0L, SEEK_END);
      int length = ftell(f);
      rewind(f);

      buf = new char[length];
      int n = fread(buf, 1, length, f);

      if (n == length)
        sOut = String(buf, length);
    }
  }
  __finally
  {
    try { if (f != NULL) fclose(f); } catch(...) {}
    try { if (buf != NULL) delete [] buf; } catch(...) {}
  }

  return sOut;
}
//---------------------------------------------------------------------------
WideString __fastcall TMainForm::GetCurrentDirW(void)
// Get the current directory as a WideString
{
  wchar_t* pCurDir = NULL;
  WideString wDir;

  try
  {
    int d_size = GetCurrentDirectoryW(0, NULL);
    pCurDir = new wchar_t[d_size];

    if (pCurDir != NULL)
    {
      GetCurrentDirectoryW(d_size, pCurDir);
      wDir = WideString(pCurDir);
    }
  }
  __finally
  {
    try { if (pCurDir != NULL) delete [] pCurDir; } catch(...) {}
  }

  return wDir;
}
//---------------------------------------------------------------------------
// Display Utf8 messages
void __fastcall TMainForm::ShowMessageU(String uStr)
{
  MessageBoxW(NULL, Utf8ToWide(uStr).c_bstr(), L"Message", MB_ICONINFORMATION | MB_OK);
}

// Display WideString messages
void __fastcall TMainForm::ShowMessageW(WideString wStr)
{
  MessageBoxW(NULL, wStr.c_bstr(), L"Message", MB_ICONINFORMATION | MB_OK);
}
//---------------------------------------------------------------------------
// Property getter to return UTF-8 version of the desktop-directory
String __fastcall TMainForm::GetDeskDir(void)
{
  return WideToUtf8(FwDeskDir);
}
//---------------------------------------------------------------------------
WideString __fastcall TMainForm::GetSpecialFolder(int csidl)
{
  HMODULE h = NULL;
  WideString sOut;
  WideChar* buf = NULL;

  try
  {
    h = LoadLibraryW(L"Shell32.dll");

    if (h != NULL)
    {
      tGetFolderPath pGetFolderPath;
      pGetFolderPath = (tGetFolderPath)GetProcAddress(h, "SHGetFolderPathW");

      if (pGetFolderPath != NULL)
      {
        buf = new WideChar[MAX_PATH];
        buf[0] = L'\0';

        if ((*pGetFolderPath)(Application->Handle, csidl, NULL, SHGFP_TYPE_CURRENT, (LPTSTR)buf) == S_OK)
          sOut = WideString(buf);
      }
    }

  }
  __finally
  {
    if (h != NULL) FreeLibrary(h);
    if (buf != NULL) delete [] buf;
  }

  return sOut;
}
//---------------------------------------------------------------------------
#if DEBUG_ON
// DEBUG CONSOLE!!!!!!!
static HANDLE m_Screen = NULL;

void __fastcall TMainForm::CInit(void)
{
  // allocate a console for this app
  // AllocConsole initializes standard input, standard output,
  // and standard error handles for the new console. The standard input
  // handle is a handle to the console's input buffer, and the standard
  // output and standard error handles are handles to the console's
  // screen buffer. To retrieve these handles, use the GetStdHandle function.
  AllocConsole();
  m_Screen = GetStdHandle (STD_OUTPUT_HANDLE);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CWrite(String S)
{
  WriteConsole(m_Screen, S.c_str(),S.Length(),0,0);
}
#endif
//---------------------------------------------------------------------------



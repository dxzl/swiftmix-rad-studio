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
#if !FREEWARE_EDITION
KeyClass* PK;
#endif

//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner) : TForm(Owner)
{
  ListA = NULL;
  ListB = NULL;

  FFilesAddedCount = 0;
  FMaxCacheFiles = MAX_CACHE_FILES;

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

  Application->HintColor = TColor(0xF5CFB8);
  Application->HintPause = 500;
  Application->HintHidePause = 4000;
  Application->ShowHint = true;

  FfadeAt = 10; // start fade 10 seconds before end

  GPlaylistForm = NULL;

  // Try to register the SwiftMix messages
  RWM_SwiftMixTime = RegisterWindowMessage(L"WM_SwiftMixTime");
  RWM_SwiftMixPlay = RegisterWindowMessage(L"WM_SwiftMixPlay");
  RWM_SwiftMixState = RegisterWindowMessage(L"WM_SwiftMixState");
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

  // do this before setting up the file-cache below...
  // to set bFileCacheEnabled
  InitRegistryVars();

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

  Panel1->Top = WindowsMediaPlayer2->Top + WindowsMediaPlayer2->Height + 2;
  Panel1->Width = WindowsMediaPlayer2->Width;

  StatusBar1->Top = Panel1->Top + Panel1->Height + 2;
  StatusBar1->Width = WindowsMediaPlayer2->Width;

//  UpDown1->Left = 0;
//  UpDown1->Height = 21;
//  UpDown1->Width = 17;
//
//  FadeRate->Left = UpDown1->Left+UpDown1->Width;
//  FadeRate->Height = UpDown1->Height;
//  FadeRate->Width = UpDown1->Width;
//
//  UpDown2->Left = FadeRate->Left+FadeRate->Width;
//  UpDown2->Height = UpDown1->Height;
//  UpDown2->Width = UpDown1->Width;
//
//  FadePoint->Left = UpDown2->Left+UpDown2->Width;;
//  FadePoint->Height = UpDown1->Height;
//  FadePoint->Width = UpDown1->Width+4;
//
//  TrackBar1->Left = FadePoint->Left+FadePoint->Width+2;
//  TrackBar1->Height = 33;
//  TrackBar1->Top = WindowsMediaPlayer2->Top+WindowsMediaPlayer2->Height+2;
//  TrackBar1->Width = WindowsMediaPlayer1->Width-TrackBar1->Left;
//
//  UpDown1->Top = TrackBar1->Top+((TrackBar1->Height-UpDown1->Height)/2);
//  FadeRate->Top = UpDown1->Top;
//  UpDown2->Top = UpDown1->Top;
//  FadePoint->Top = UpDown1->Top;

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

  pTMyFileCopyList = new TList();
  pTFailedToCopyList = new TList();

  //enable drag/drop files
  ::DragAcceptFiles(this->Handle, true);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormShow(TObject *Sender)
{
  // Create the song-list forms
  if (ListA == NULL)
    Application->CreateForm(__classid(TPlaylistForm), &ListA);
  if (ListB == NULL)
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

  // need ListA and ListB to InitFileCacheing()
  if (bFileCacheEnabled) // if it's enabled in registry...
    bFileCacheEnabled = InitFileCaching();

  //-----------------
  // Add the windows to the docking system
  //-----------------
  //Set the parent window
//  if (FDock != NULL)
//  {
//    FDock->SetParent(this->Handle);
//
//    //Add the child windows
//    FDock->AddChild(ListA->Handle); // automatically dock
//    FDock->AddChild(ListB->Handle); // automatically dock
//  }

  SetCheckmarkA(FvolA);
  SetCheckmarkB(FvolB);

  MenuFaderTypeNormal->Checked = bTypeCenterFade ? false : true; // inverse
  MenuFaderModeAuto->Checked = bModeManualFade ? false : true; // inverse
  MenuSendTiming->Checked = bSendTiming ? true : false;
  MenuRepeatModeA->Checked = bRepeatModeA ? true : false;
  MenuRepeatModeB->Checked = bRepeatModeB ? true : false;
  MenuShuffleModeA->Checked = bShuffleModeA ? true : false;
  MenuShuffleModeB->Checked = bShuffleModeB ? true : false;
  MenuCacheFiles->Checked = bFileCacheEnabled ? true : false;

  try
  {
    // SetVolumes() sets currentVolA/currentVolB and both media player volumes
    // based on the values of volA/volB and the position of TrackBar1
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
// probably we don't need this...
//  if (FDock != NULL) delete FDock;

  // delete file-cache directory and files
  if (DirectoryExists(FsCacheDir))
    DeleteDirAndFiles(FsCacheDir);

  if (pTMyFileCopyList)
    delete pTMyFileCopyList;

  ClearFailedToCopyList();

  if (pTFailedToCopyList)
    delete pTFailedToCopyList;

#if DEBUG_ON
  MainForm->CWrite("\r\nFormDestroy() in FormMain()\r\n");
  FreeConsole();
#endif

#if !FREEWARE_EDITION
  if (PK != NULL) delete PK;
#endif

//  Application->Terminate();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  if (pTMyFileCopyList->Count)
  {
    for (int ii = 0; ii < pTMyFileCopyList->Count; ii++)
    {
      TMyFileCopy* p = (TMyFileCopy*)pTMyFileCopyList->Items[ii];
      if (p)
        p->Cancel = true;
    }

    CanClose = false;
    ShowMessage("File copy in-progress, cancelling... try again in 15 seconds.");
  }
  else
  {
    CanClose = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormClose(TObject* Sender, TCloseAction &Action)
{
  if (SFDlgForm)
  {
    SFDlgForm->Close();
    SFDlgForm->Release();
  }

  if (OFMSDlgForm)
  {
    OFMSDlgForm->Close();
    OFMSDlgForm->Release();
  }

  TRegHelper* pReg = NULL;

  try
  {
    pReg = new TRegHelper(true);

    if (pReg)
    {
      pReg->WriteSetting(SM_REGKEY_DIR_A, SaveDirA);
      pReg->WriteSetting(SM_REGKEY_DIR_B, SaveDirB);
      pReg->WriteSetting(SM_REGKEY_IMPORTEXT, ImportExt);
      pReg->WriteSetting(SM_REGKEY_EXPORTEXT, ExportExt);
      pReg->WriteSetting(SM_REGKEY_FADERMODE, bTypeCenterFade);
      pReg->WriteSetting(SM_REGKEY_FADERTYPE, bModeManualFade);
      pReg->WriteSetting(SM_REGKEY_SENDTIMING, bSendTiming);
      pReg->WriteSetting(SM_REGKEY_REPEAT_A, bRepeatModeA);
      pReg->WriteSetting(SM_REGKEY_REPEAT_B, bRepeatModeB);
      pReg->WriteSetting(SM_REGKEY_SHUFFLE_A, bShuffleModeA);
      pReg->WriteSetting(SM_REGKEY_SHUFFLE_B, bShuffleModeB);
      pReg->WriteSetting(SM_REGKEY_CACHE_ENABLED, bFileCacheEnabled);
      pReg->WriteSetting(SM_REGKEY_CACHE_MAX_FILES, FMaxCacheFiles);
      pReg->WriteSetting(SM_REGKEY_VOL_A, FvolA);
      pReg->WriteSetting(SM_REGKEY_VOL_B, FvolB);
    }
  }
  __finally
  {
    try { if (pReg) delete pReg; } catch(...) {}
  }

  // Each list has a TProgressForm and other forms like TImportForm/TExportForm
  // that might be used during file-copy, etc - so delete these last.
  if (ListA)
  {
    ListA->Close();
    ListA->Release();
    ListA = NULL;
  }
  if (ListB)
  {
    ListB->Close();
    ListB->Release();
    ListB = NULL;
  }

#if DEBUG_ON
  MainForm->CWrite("\r\nFormClose() in FormMain()\r\n");
#endif
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::InitRegistryVars(void)
{
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
        SaveDirA = FsDeskDir;

      if (SaveDirB.IsEmpty())
        SaveDirB = FsDeskDir;

      if (ImportExt.IsEmpty())
        ImportExt = IMPORT_EXT;

      if (ExportExt.IsEmpty())
        ExportExt = EXPORT_EXT;

      pReg->ReadSetting(SM_REGKEY_FADERMODE, bTypeCenterFade, false);
      pReg->ReadSetting(SM_REGKEY_FADERTYPE, bModeManualFade, false);
      pReg->ReadSetting(SM_REGKEY_SENDTIMING, bSendTiming, false);
      pReg->ReadSetting(SM_REGKEY_REPEAT_A, bRepeatModeA, false);
      pReg->ReadSetting(SM_REGKEY_REPEAT_B, bRepeatModeB, false);
      pReg->ReadSetting(SM_REGKEY_SHUFFLE_A, bShuffleModeA, false);
      pReg->ReadSetting(SM_REGKEY_SHUFFLE_B, bShuffleModeB, false);
      pReg->ReadSetting(SM_REGKEY_CACHE_ENABLED, bFileCacheEnabled, true);

      pReg->ReadSetting(SM_REGKEY_CACHE_MAX_FILES, FMaxCacheFiles, MAX_CACHE_FILES);
      if (FMaxCacheFiles < MAX_CACHE_FILES)
        FMaxCacheFiles = MAX_CACHE_FILES; // need to make this the minimum!!!!

      pReg->ReadSetting(SM_REGKEY_VOL_A, FvolA, 50);
      pReg->ReadSetting(SM_REGKEY_VOL_B, FvolB, 50);

      // in case a low volume got stuck in the registry...
      if (FvolA < 10)
        FvolA = 50;
      if (FvolB < 10)
        FvolB = 50;
    }
    else
    {
      ShowMessage("Unable to read settings from the registry!");

      SaveDirA = FsDeskDir;
      SaveDirB = FsDeskDir;
      ImportExt = IMPORT_EXT;
      ExportExt = EXPORT_EXT;

      bTypeCenterFade = false;
      bModeManualFade = false;
      bSendTiming = false;
      bRepeatModeA = false;
      bRepeatModeB = false;
      bShuffleModeA = false;
      bShuffleModeB = false;
      bFileCacheEnabled = true;

      FMaxCacheFiles = MAX_CACHE_FILES;

      FvolA = 50;
      FvolB = 50;
    }
  }
  __finally
  {
    try { if (pReg != NULL) delete pReg; } catch(...) {}
  }
}
//---------------------------------------------------------------------------
// returns true if enabled
bool __fastcall TMainForm::InitFileCaching(void)
{
  FsDeskDir = GetSpecialFolder(CSIDL_DESKTOPDIRECTORY);

  // if we can find or make a Temp directory, enable custom file-cacheing
  FsCacheDir = GetSpecialFolder(CSIDL_LOCAL_APPDATA);
  if (DirectoryExists(FsCacheDir))
  {
    //FsTempDir += "\\Temp";
    //if (!DirectoryExists(FsTempDir))
    //  try { CreateDirectory(FsTempDir.c_str(), NULL); } catch(...) {}
    FsCacheDir += FILE_CACHE_PATH1; // "\\Discrete-Time Systems"
    if (!DirectoryExists(FsCacheDir))
      try { CreateDirectory(FsCacheDir.c_str(), NULL); } catch(...) {}
    FsCacheDir += FILE_CACHE_PATH2; //"\\MusicMixer"
    if (!DirectoryExists(FsCacheDir))
      try { CreateDirectory(FsCacheDir.c_str(), NULL); } catch(...) {}
    else
    {
      // directory exists so we didn't clean up... delete it and its files
      if (DeleteDirAndFiles(FsCacheDir))
        // recreate empty directory
        if (!DirectoryExists(FsCacheDir))
          try { CreateDirectory(FsCacheDir.c_str(), NULL); } catch(...) {}
    }

    if (!DirectoryExists(FsCacheDir))
      return false;

    // need to scan both song-lists, create TPlayerURL objects if missing
    // and restore the main cache-counters and count to 0;
    ListA->RestoreCache();
    ListB->RestoreCache();

    return true;
  }
  else
    return false;
}
//---------------------------------------------------------------------------
// deletes the temp "song queue" directory and files in it
bool __fastcall TMainForm::DeleteDirAndFiles(String sDir)
{
  // could use "runas" instead of "open"
  return ShellCommand(L"open", L"cmd.exe",
           L"/c cd ..\\ \& rd /s /q \"" + sDir + "\"", true);
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::ShellCommand(String sVerb, String sSourcePath,
                                  String sCmd, bool bWaitForCompletion)
{
  int bRet = true;

  try
  {
    // example (from YahCoLoRiZe old version):
    //S = String("/c echo \"Removing YahCoLoRiZe Please Wait...\"") +
    //      String(" \& ping 1.1.1.1 -n 1 -w 2000 > nul \& ") +
    //        String("cd ..\\ \& rd /s /q \"") + S + String("\" \& ") +
    //        String("cd \"") + AppDataDir + String("\" \& ") +
    //        String("rd /s /q \"") + OUR_NAME_S + String("\"");

    if (IsWinVistaOrHigher())
    {
      // Vista, Win7,8,10
      SHELLEXECUTEINFO shExecInfo = {0};

      shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);

      // wait for it to complete (NULL if no wait)
      shExecInfo.fMask = bWaitForCompletion ? SEE_MASK_NOCLOSEPROCESS : NULL;

      shExecInfo.hwnd = NULL;
//        shExecInfo.lpVerb = L"runas"; // NOTE: this works but prompts for admin password
      shExecInfo.lpVerb = sVerb.c_str(); // "open"
      shExecInfo.lpFile = sSourcePath.c_str(); // "cmd.exe"
      shExecInfo.lpParameters = sCmd.c_str(); // "/c cd ..\\ \& rd /s /q \"" + sDir + "\""
      shExecInfo.lpDirectory = NULL;
      shExecInfo.nShow = SW_HIDE;
      shExecInfo.hInstApp = NULL;

      ShellExecuteEx(&shExecInfo);

      // only use the two lines below for SEE_MASK_NOCLOSEPROCESS
      if (bWaitForCompletion)
      {
        //if (!WaitForSingleObject(shExecInfo.hProcess, INFINITE))
        if (WaitForSingleObject(shExecInfo.hProcess, 2000) != 0) // wait for 2 seconds
          bRet = false;

        CloseHandle(shExecInfo.hProcess);
      }
    }
    else // WinXP
    {
      STARTUPINFO si = {0};
      PROCESS_INFORMATION pi = {0};

      // Documentation requires NULL for app-name if cmd.exe is 16-bit
      // (which it must be because this is the only way it works!)
      sSourcePath += " "; // "cmd.exe "
      sCmd = sCmd.Insert(sSourcePath, 1);
      CreateProcess(NULL, sCmd.c_str(), NULL, NULL, FALSE,
                    CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

      CloseHandle(pi.hThread);
      CloseHandle(pi.hProcess);

      if (bWaitForCompletion)
        Sleep(1000); // wait to complete
    }
  }
  catch(...)
  {
    bRet = false;
  }

  return bRet;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::StatusBar1MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
  int j, WidthIndex;
  int iNumPanels = StatusBar1->Panels->Count;

  // for each panel in the bar...

  WidthIndex = 0;

  for (int j = 0 ; j < iNumPanels; j++)
  {
    WidthIndex += StatusBar1->Panels->Items[j]->Width;

    if (X <= WidthIndex)
    {
      if (j == 2)
        MenuFaderModeAutoClick(NULL);
      break;
    }
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
//    try
//    {
      TOFMSDlgForm* fd = f->CreateFileDialog();

      if (fd != NULL)
      {
        // Filters
        fd->Filters = "All Files (*.*)|*.*|"
                    "Windows Media (*.wma)|*.wma|"
                    "MP3 (*.mp3)|*.mp3|"
                    "WAV (*.wav)|*.wav";

        FFilesAddedCount = 0;

        fd->Execute(0, d, t); // no def utf-8 extension?

        if(fd->Result == IDOK)
        {
          // Press and hold Shift to bypass the file-extention filtering
          // (Checked in AddFileToListBox())
          GBypassFilters = (GetKeyState(VK_SHIFT) & 0x8000);

#if DEBUG_ON
          // Display for diagnostics
          if (fd->FileNameObjects && fd->FileNameObjects->Count > 0)
          {
            MainForm->CWrite( "\r\nPrinting selected-files list!!!!!!!\r\n");

            String s1 = "";

            int iCount = fd->FileNameObjects->Count;

            for (int ii = 0; ii < iCount; ii++)
            {
              TWideItem* pWI = (TWideItem*)fd->FileNameObjects->Items[ii];

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
              SaveDirA = fd->CurrentFolder;
            else
              SaveDirB = fd->CurrentFolder;

            int iCount = fd->FileNameObjects->Count;

            f->Progress->Init(iCount);

            for (int ii = 0; ii < iCount ; ii++)
            {
              TWideItem* pWI = (TWideItem*)fd->FileNameObjects->Items[ii];

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
        else if (FFilesAddedCount > 0)
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
//    }
//    catch(...) { ShowMessage("FileDialog() threw an exception"); }
  }
  __finally
  {
    f->DestroyFileDialog();
    f->Progress->UnInit(true);
  }

  if (bRet)
    SetVolumes();

  return bRet;
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
        LoadListWithDroppedFiles(ListA, Msg);
      else if (p.y > topB && p.y < bottomB)
        LoadListWithDroppedFiles(ListB, Msg);
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
void __fastcall TMainForm::LoadListWithDroppedFiles(TPlaylistForm* f, TWMDropFiles &Msg)
{
  if (f == NULL)
    return;

  TCursor Save_Cursor;
  wchar_t* pBuf = NULL;
  TStringList* sl = NULL;
  ImportForm = NULL;

  FFilesAddedCount = 0;

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
    this->GBypassFilters = (GetKeyState(VK_SHIFT) & 0x8000);

    String SaveDir = GetCurrentDir(); // Save

    f->Progress->Init(sl->Count);

    for (int ii = 0; ii < sl->Count; ii++)
    {
      ProcessFileItem(f, sl->Strings[ii]);

      if (f->Progress->Move(ii))
        break;
    }

    SetCurrentDir(SaveDir); // Restore

    if (FFilesAddedCount > 0)
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
    try { if (ImportForm != NULL) ImportForm->Release(); } catch(...) {}
    try { if (pBuf != NULL) delete [] pBuf; } catch(...) {}
    try { if (sl != NULL) delete sl; } catch(...) {}

    f->Progress->UnInit(true);

    // Restore the previous cursor.
    Screen->Cursor = Save_Cursor;
  }
}
//---------------------------------------------------------------------------
// expand any .lnk shortcut, if any and validate file
void __fastcall TMainForm::ProcessFileItem(TPlaylistForm* f, String s)
{
#if DEBUG_ON
      MainForm->CWrite("\r\nTMainForm::LoadListWithDroppedFiles: Original sSourcePath: \"" + s + "\"\r\n");
#endif

      // expand any .lnk shortcut, if any and validate file
      bool bIsDirectory;
      if (GetShortcut(s, bIsDirectory))
      {
#if DEBUG_ON
        MainForm->CWrite("\r\nTMainForm::LoadListWithDroppedFiles: GetShortcut() sSourcePath: \"" + s + "\"\r\n");
#endif
        if (bIsDirectory)
        {
#if DEBUG_ON
          MainForm->CWrite("\r\nTMainForm::LoadListWithDroppedFiles: bIsDirectory is set! Calling AddAllSongsToListBox()\r\n");
#endif
          FFilesAddedCount += AddAllSongsToListBox(f, s); // recurse add folder and sub-folder's songs to list
        }
        else if (IsPlaylistPath(s))
        {
#if DEBUG_ON
          MainForm->CWrite("\r\nTMainForm::LoadListWithDroppedFiles: IsPlaylistPath() returned true! Calling ImportForm->NoDialog()\r\n");
#endif
          FFilesAddedCount += ImportForm->NoDialog(f, s, IMPORT_MODE_AUTO); // Load the playlist
        }
        else if (AddFileToListBox(f, s))
        {
#if DEBUG_ON
          MainForm->CWrite("\r\nTMainForm::LoadListWithDroppedFiles: AddFileToListBox() returned true!\r\n");
#endif
          FFilesAddedCount++;
        }
#if DEBUG_ON
        else
        {
          MainForm->CWrite("\r\nTMainForm::LoadListWithDroppedFiles: ITEM NOT PROCESSED!\r\n");
        }
#endif
      }
}
//---------------------------------------------------------------------------
// returns true if wPath is a valid file or shortcut. Returns bIsDirectory
// by reference. If the file is a .lnk shortcut - the target it points to
// is returned in wPath.
bool __fastcall TMainForm::GetShortcut(String &wPath, bool &bIsDirectory)
{
  bIsDirectory = false;

  if (IsUri(wPath))
  {
    bIsDirectory = UriIsDirectory(wPath) ? true : false;
    return true;
  }

  try
  {
#if DEBUG_ON
    OFDbg->CWrite("\r\nGetShortcut() wPath: " + wPath +"\r\n");
#endif

    // Do this first because we might have a .lnk file with no extension in our list-view control.
    if (DirectoryExists(wPath))
    {
      bIsDirectory = true;
      return true;
    }

    if (FileExists(wPath))
    {
      if (ExtractFileExt(wPath).LowerCase() == ".lnk")
      {
        wPath = GetShortcutTarget(wPath);

        if (FileExists(wPath))
          return true;

        if (DirectoryExists(wPath))
        {
          bIsDirectory = true;
          return true;
        }
      }
      else
        return true;
    }
    else
    {
      wPath += ".lnk"; // case where the .lnk extension is not displayed in the list-view control

#if DEBUG_ON
      OFDbg->CWrite("\r\nCheck file: " + wPath + "\r\n");
#endif
      if (FileExists(wPath))
      {
        wPath = GetShortcutTarget(wPath);
#if DEBUG_ON
        OFDbg->CWrite("\r\nShortcut target: " + wPath + "\r\n");
#endif

        if (FileExists(wPath))
        {
#if DEBUG_ON
          OFDbg->CWrite("\r\nShortcut exists: " + wPath + "\r\n");
#endif
          return true;
        }

        if (DirectoryExists(wPath))
        {
#if DEBUG_ON
          OFDbg->CWrite("\r\nDirectory exists: " + wPath + "\r\n");
#endif
          bIsDirectory = true;
          return true;
        }
      }
    }
  }
  catch (...)
  {
#if DEBUG_ON
    OFDbg->CWrite( "\r\nException in GetShortcut()\r\n");
#endif
  }

  return false;
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::UriIsDirectory(String sUri)
{
  for (int ii=sUri.Length(); ii >= 1; ii--)
  {
    if (sUri[ii] == '/')
      return true;
    if (sUri[ii] == '.')
      return false;
  }
  return false;
}
//---------------------------------------------------------------------------
String __fastcall TMainForm::GetShortcutTarget(String wPath)
{
  if (ExtractFileExt(wPath).LowerCase() != ".lnk")
    return "";

  String wOut = "";

  CoInitialize(NULL);

  try
  {
    IShellLinkW* psl = NULL;
    IPersistFile* ppf = NULL;
    TWin32FindDataW* wfs = NULL;

    try
    {
      WideChar Info[MAX_PATH+1];

      CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void**)&psl);
      if (psl != NULL)
      {
        if (psl->QueryInterface(IID_IPersistFile, (void**)&ppf) == 0)
        {
          if (ppf != NULL)
          {
            ppf->Load(wPath.w_str(), STGM_READ);
            psl->GetPath(Info, MAX_PATH, wfs, SLGP_UNCPRIORITY);
            wOut = String(Info);
          }
        }
      }
    }
    __finally
    {
      if (ppf) ppf->Release();
      if (psl) psl->Release();
      CoUninitialize();
    }
  }
  catch(...) { }

  return wOut;
}
//---------------------------------------------------------------------------
int __fastcall TMainForm::AddAllSongsToListBox(TPlaylistForm* f, String sPath)
// Strings are in UTF-8 format!
{
  TStringList* sl = NULL;
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
    MainForm->CWrite("\r\nTMainForm::AddAllSongsToListBox(): sPath=\"" + sPath + "\"\r\n");
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
bool __fastcall TMainForm::AddFileToListBox(TPlaylistForm* f, String sSourcePath)
{
  if (f == NULL || sSourcePath.Length() == 0)
    return false;

  try
  {
    // NOTE: the following creates a new TPlayerURL() class object!
    // (its pointer is stored in the list-item's Tag property)
    f->AddListItem(sSourcePath);

    // move the first file to the cache
    if (f->Count == 1)
      CopyFileToCache(f, -1);

    return true;
  }
  catch(...) { return false; }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::RecurseFileAdd(TPlaylistForm* f, TStringList* slFiles)
// Uses the String versions of the Win32 API FindFirstFile and FindNextFile directly
// and converts the resulting paths to UTF-8 for storage in an ordinary TStringList
//
// Use SetCurrentDir() to set our root directory or TOpenDialog sets it also...
{
  if (slFiles == NULL)
    return;

  TStringList* slSubDirs = NULL;
  HANDLE hFind = NULL;

  Application->ProcessMessages();

  try
  {
    slSubDirs = new TStringList();

    if (!slSubDirs)
      return;

    // get list of subdirectories and files
    FindFirstNextToStringLists(slFiles, slSubDirs);

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
void __fastcall TMainForm::FindFirstNextToStringLists(TStringList* slFiles, TStringList* slDirs)
// slFiles is in UTF-8!
{
  if (!slFiles || !slDirs)
    return;

  TWin32FindDataW sr;
  HANDLE hFind = NULL;
  TFindexInfoLevels l = FindExInfoStandard; // FindExInfoBasic was defined later!
  TFindexSearchOps s = FindExSearchNameMatch; // FindExSearchLimitToDirectories;

  try
  {
    // Get the current directory
    String sCurrentDir = GetCurrentDir();

    // Add trailing backslash
    int iDirLen = sCurrentDir.Length();
    if (iDirLen && sCurrentDir[iDirLen] != '\\')
      sCurrentDir += '\\';

    // NOTE: a trailing backslash for FindFirst() is not allowed
    String sTemp = sCurrentDir + "*";

    hFind = ::FindFirstFileEx(sTemp.c_str(), l, &sr, s, NULL, (DWORD)FIND_FIRST_EX_LARGE_FETCH);

    // Get list of subdirectories into a stringlist
    if (hFind == INVALID_HANDLE_VALUE)
    {
#if DEBUG_ON
      MainForm->CWrite("\r\nTMainForm::RecurseFileAdd(): hFind INVALID_HANDLE_VALUE\r\n");
#endif
      return;
    }

    // Get list of files into a stringlist

    do
    {
      if (sr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
        int len = wcslen(sr.cFileName);

        if (len == 1 && sr.cFileName[0] == '.')
          continue;
        if (len == 2 && sr.cFileName[0] == '.' && sr.cFileName[1] == '.')
          continue;

#if DEBUG_ON
        MainForm->CWrite("\r\nTMainForm::RecurseFileAdd(): Directory=\"" + String(sr.cFileName) + "\"\r\n");
#endif
        slDirs->Add(String(sr.cFileName));
      }
      else if (!(sr.dwFileAttributes & (FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN)))
        slFiles->Add(sCurrentDir + String(sr.cFileName));

      //Application->ProcessMessages();

    } while (::FindNextFile(hFind, &sr) == TRUE);
  }
  __finally
  {
    try
    {
      if (hFind)
        ::FindClose(hFind);
    }
    catch(...) {}
  }
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::IsAudioFile(String sSourcePath)
// should work ok on UTF-8 strings
{
  if (IsUri(sSourcePath)) return true; // pass through http:// links

  if (GBypassFilters) return true;

  String sExt = ExtractFileExt(sSourcePath).LowerCase();

  if (sExt.IsEmpty()) return false;

  return sExt == ".mp3" || sExt == ".wma" || sExt == ".asf" || sExt == ".wav" ||
          sExt == ".mpa" || sExt == ".mpe" || sExt == ".m3u" || sExt == ".avi" || sExt == ".aac" ||
          sExt == ".adt" || sExt == ".adts" || sExt == ".mp2" || sExt == ".cda" ||
          sExt == ".au" || sExt == ".snd" || sExt == ".aif" || sExt == ".aiff" || sExt == ".aifc" ||
          sExt == ".mid" || sExt == ".midi" || sExt == ".rmi" || sExt == ".m4a";
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::IsPlaylistPath(String sSourcePath)
{
  if (IsUri(sSourcePath)) return true; // pass through http:// links

  if (GBypassFilters) return true;

  String sExt = ExtractFileExt(sSourcePath).LowerCase();

  return IsPlaylistExtension(sExt);
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::IsPlaylistExtension(String sExt)
{
  if (sExt.IsEmpty()) return false;

  return sExt == ".wpl" || sExt == ".m3u8" || sExt == ".m3u" ||
        sExt == ".asx" || sExt == ".xspf" || sExt == ".wax" ||
          sExt == ".wmx" || sExt == ".wvx" ||  sExt == ".pls" || sExt == ".txt";
}
//---------------------------------------------------------------------------
// Could have "file:/laptop/D:/path/file.wma" so the key to telling a URL from
// a drive letter is that url preambles are more than one char!
//
// sIn should be trimmed but does not need to be lower-case...
bool __fastcall TMainForm::IsUri(String sIn)
{
  return sIn.Pos(":/") > 2; // > 2 means you must have more than 1 char before the : (like "file:/")
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::IsFileUri(String sIn)
{
  return sIn.LowerCase().Pos("file:/") == 1;
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::IsSourcePathUri(String sIn)
{
  return sIn.LowerCase().Pos("file:/") == 1;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::VA_10Click(TObject* Sender)
{
  // Vol 1 10%
  SetVolumeAndCheckmarkA(10);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::VA_25Click(TObject* Sender)
{
  // Vol 1 25%
  SetVolumeAndCheckmarkA(25);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::VA_50Click(TObject* Sender)
{
  // Vol 1 50%
  SetVolumeAndCheckmarkA(50);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::VA_75Click(TObject* Sender)
{
  // Vol 1 75%
  SetVolumeAndCheckmarkA(75);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::VA_100Click(TObject* Sender)
{
  // Vol 1 100%
  SetVolumeAndCheckmarkA(100);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::VB_10Click(TObject* Sender)
{
  // Vol 2 10%
  SetVolumeAndCheckmarkB(10);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::VB_25Click(TObject* Sender)
{
  // Vol 2 25%
  SetVolumeAndCheckmarkB(25);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::VB_50Click(TObject* Sender)
{
  // Vol 2 50%
  SetVolumeAndCheckmarkB(50);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::VB_75Click(TObject* Sender)
{
  // Vol 2 75%
  SetVolumeAndCheckmarkB(75);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::VB_100Click(TObject* Sender)
{
  // Vol 2 100%
  SetVolumeAndCheckmarkB(100);
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
    if (ListB->Tag >= 0 && WindowsMediaPlayer1->playState == WMPPlayState::wmppsPlaying)
    {
      // Start Player 2
      if (WindowsMediaPlayer2->playState != WMPPlayState::wmppsPlaying)
      {
        WindowsMediaPlayer2->settings->mute = true;
        WindowsMediaPlayer2->controls->play();
        WindowsMediaPlayer2->settings->mute = false;
      }

      bFadeRight = true; // Set fade-direction
      AutoFadeTimer->Enabled = true; // Start a fade
      return true;
    }

    // player 2 on now?
    if (ListA->Tag >= 0 && WindowsMediaPlayer2->playState == WMPPlayState::wmppsPlaying)
    {
      // Start Player 1
      if (WindowsMediaPlayer1->playState != WMPPlayState::wmppsPlaying)
      {
        WindowsMediaPlayer1->settings->mute = true;
        WindowsMediaPlayer1->controls->play();
        WindowsMediaPlayer1->settings->mute = false;
      }

      bFadeRight = false; // Set fade-direction
      AutoFadeTimer->Enabled = true; // Start a fade
      return true;
    }
  }
  catch(...)
  {
    ShowMessage("ForceFade() threw an exception");
  }

  return false;
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::SetVolumeAndCheckmarkA(int v)
{
  bool bRet1 = SetVolumeA(v);
  bool bRet2 = SetCheckmarkA(v);
  return bRet1 || bRet2;
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::SetVolumeAndCheckmarkB(int v)
{
  bool bRet1 = SetVolumeB(v);
  bool bRet2 = SetCheckmarkB(v);
  return bRet1 || bRet2;
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::SetCheckmarkA(int v)
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

    return true;
  }
  catch(...) { return false; }
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::SetCheckmarkB(int v)
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

    return true;
  }
  catch(...) { return false; }
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::SetVolumes()
{
  bool bRetA = SetVolumeA();
  bool bRetB = SetVolumeB();
  return bRetA || bRetB;
}
//---------------------------------------------------------------------------
// overloaded...


bool __fastcall TMainForm::SetVolumeA(int v)
{
  FvolA = v;
  return SetVolumeA();
}

bool __fastcall TMainForm::SetVolumeA(void)
{
  if (!WindowsMediaPlayer1)
    return false;

  // TrackBar1 Min position is 0, Max position is 100 (0-100%)
  // Units for volA/volB global vars is in percent (0-100)
  try
  {
    if (bTypeCenterFade)
    {
      if (FaderTrackBar->Position < 50)
        FcurrentVolA = FvolA;
      else
        FcurrentVolA = FvolA*((100-FaderTrackBar->Position)*2)/100;
    }
    else
      FcurrentVolA = (FvolA*(100-FaderTrackBar->Position))/100;

//#if DEBUG_ON
//    MainForm->CWrite("\r\nSetVolumeA(): currentVolA = " + String(FcurrentVolA) + "\r\n");
//#endif

    WindowsMediaPlayer1->settings->volume = FcurrentVolA;

    return true;
  }
  catch(...)
  {
#if DEBUG_ON
    MainForm->CWrite("\r\nSetVolumeA() exception!\r\n");
#endif
    return false;
  }
}
//---------------------------------------------------------------------------
// overloaded...

bool __fastcall TMainForm::SetVolumeB(int v)
{
  FvolB = v;
  return SetVolumeB();
}

bool __fastcall TMainForm::SetVolumeB(void)
{
  if (!WindowsMediaPlayer2)
    return false;

  // TrackBar1 Min position is 0, Max position is 100 (0-100%)
  // Units for volA/volB global vars is in percent (0-100)
  try
  {
    if (bTypeCenterFade)
    {
      if (FaderTrackBar->Position >= 50)
        FcurrentVolB = FvolB;
      else
        FcurrentVolB = FvolB*(FaderTrackBar->Position*2)/100;
    }
    else
      FcurrentVolB = (FvolB*FaderTrackBar->Position)/100;

//#if DEBUG_ON
//    MainForm->CWrite("\r\nSetVolumeB(): volB = " + String(FvolB) + "\r\n");
//    MainForm->CWrite("\r\nSetVolumeB(): currentVolB = " + String(FcurrentVolB) + "\r\n");
//#endif

    WindowsMediaPlayer2->settings->volume = FcurrentVolB;

    return true;
  }
  catch(...)
  {
#if DEBUG_ON
    MainForm->CWrite("\r\nSetVolumeB() exception!\r\n");
#endif
    return false;
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
    if (f->Count && f->Tag == -1)
      f->QueueFirst();

    f->SetTitle();

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

//    GDock->WindowMoved(f->Handle);
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
// Here, we either increment or decrement the fader trackbar position by the
// trackbar frequency. The base timer unit is 50ms. The up-down Fade Rate
// can be 0 to +9 or 0 to -9. When it's positive, the timer-tick is 50ms and the
// tick-frequency of the trackbar is 0-9. There are 100 steps in the
// trackbar range. So the "normal" fade is 50ms * 100 steps = 5 seconds.
// The fastest fade is (50ms*100)/9 = .55 seconds. For negative fade-rates
// the tick-frequency is 1 and the trackbar interval is 50ms to (50*9) = 450ms.
// So the slowest fade-time is 100 steps at 50ms per step * 9 or 450 * 100 or
// 45 seconds.
void __fastcall TMainForm::AutoFadeTimerEvent(TObject* Sender)
{
  if (!WindowsMediaPlayer1 || !WindowsMediaPlayer2) return;

  try
  {
    if (bFadeRight)
    {
      if (FaderTrackBar->Position < FaderTrackBar->Max)
      {
        if (FaderTrackBar->Position <= FaderTrackBar->Max-FaderTrackBar->Frequency)
          FaderTrackBar->Position += FaderTrackBar->Frequency;
        else
          FaderTrackBar->Position = FaderTrackBar->Max;
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
        WindowsMediaPlayer1->URL = ListA->GetNextCheckCache();

        if (ListA->TargetIndex < 0)
          if (FileDialog(ListA, SaveDirA, ADD_A_TITLE))
            ListA->QueueFirst();
      }
    }
    else
    {
      if (FaderTrackBar->Position > FaderTrackBar->Min)
      {
        if (FaderTrackBar->Position >= FaderTrackBar->Frequency)
          FaderTrackBar->Position -= FaderTrackBar->Frequency;
        else
          FaderTrackBar->Position = FaderTrackBar->Min;
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
        WindowsMediaPlayer2->URL = ListB->GetNextCheckCache();

        if (ListB->TargetIndex < 0)
          if (FileDialog(ListB, SaveDirB, ADD_B_TITLE))
            ListB->QueueFirst();
      }
    }
  }
  catch(...)
  {
#if DEBUG_ON
    ShowMessage("AutoFadeTimerEvent() threw an exception...");
#endif
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FaderTrackBarChange(TObject* Sender)
{
  // Fader Moved
  SetVolumes();
  SetCurrentPlayer(); // Set CurrentPlayer variable (used for color-coding)
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::SetCurrentPlayer(void)
{
  if (!WindowsMediaPlayer1 || !WindowsMediaPlayer2)
    return false;

  try
  {
    if (FaderTrackBar->Position != FaderTrackBar->Max &&
              WindowsMediaPlayer1->playState == WMPPlayState::wmppsPlaying)
      CurrentPlayer |= 1;
    else
      CurrentPlayer &= ~1;

    if (FaderTrackBar->Position != FaderTrackBar->Min &&
          WindowsMediaPlayer2->playState == WMPPlayState::wmppsPlaying)
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

  if (WindowsMediaPlayer1->playState == WMPPlayState::wmppsPaused) // paused?
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

  if (WindowsMediaPlayer2->playState == WMPPlayState::wmppsPaused) // paused?
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

  int Count = id->Dialog(ListA, FsDeskDir, "Import PlayerA Playlist");

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

  int Count = ImportForm->Dialog(ListB, FsDeskDir, "Import PlayerB Playlist");

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
  TExportForm* pExportForm = NULL;

  try
  {
    Application->CreateForm(__classid(TExportForm), &pExportForm);

    int Count = pExportForm->Dialog(ListA, FsDeskDir, "Export PlayerA Playlist");

    if (Count == 0)
      ShowMessage("Unable to export list or list empty...");
  }
  __finally
  {
    pExportForm->Release();
  }
}

void __fastcall TMainForm::ExportPlaylist2Click(TObject* Sender)
{
  TExportForm* pExportForm = NULL;

  try
  {
    Application->CreateForm(__classid(TExportForm), &pExportForm);

    int Count = pExportForm->Dialog(ListB, FsDeskDir, "Export PlayerB Playlist");

    if (Count == 0)
      ShowMessage("Unable to export list or list empty...");
  }
  __finally
  {
    pExportForm->Release();
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MenuHelpClick(TObject* Sender)
{
  // launch default web-browser
  //ShellExecute(Handle, "open", "iexplore.exe", HELPSITE, NULL, SW_SHOW);
  ShellExecute(NULL, L"open", HELPSITE, NULL, NULL, SW_SHOWNORMAL);
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
    ::Randomize();
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
    ::Randomize();
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
void __fastcall TMainForm::MenuCacheFilesClick(TObject *Sender)
{
  if (bFileCacheEnabled)
  {
    // delete file-cache directory and files
    if (DirectoryExists(FsCacheDir))
      DeleteDirAndFiles(FsCacheDir);

    bFileCacheEnabled = false;
    MenuCacheFiles->Checked = false;
  }
  else
  {
    bFileCacheEnabled = InitFileCaching();
    MenuCacheFiles->Checked = bFileCacheEnabled;
  }
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
// Copy all song-list files to directory user selects
void __fastcall TMainForm::MenuExportSongFilesandListsClick(TObject* Sender)
{
  if (!pTFailedToCopyList)
    return;

  if (ListA == NULL || ListB == NULL || (!ListA->Count && !ListB->Count))
  {
    ShowMessage("Both lists are empty!");
    return;
  }

  TDirDlgForm* pDirDlgForm = NULL;
  TExportForm* pExportForm = NULL;

  ClearFailedToCopyList();

  try
  {
    try
    {
      // Return if user needs to modify the lists due to the
      // cumulative size of the music files
      __int64 lBytesNeeded = ComputeDiskSpace(DISKSPACE_MESSAGEBOX_YESNO);

      if (lBytesNeeded <= 0)
        return;

      AutoFitToDVD(lBytesNeeded);

      Application->CreateForm(__classid(TDirDlgForm), &pDirDlgForm);

      if (pDirDlgForm == NULL)
        return;

      pDirDlgForm->AutoScroll = false; // turn off autoscroll

      // Setting CSIDL_MYMUSIC works but the user can't go up from there!
      String sRootDir = pDirDlgForm->Execute(CSIDL_DESKTOPDIRECTORY) + "\\"; // no trailing backslash!

      pDirDlgForm->Close();

      if (sRootDir.IsEmpty())
        return;

      String sExportDir = sRootDir + String(EXPORT_DIR);

      if (DirectoryExists(sExportDir))
      {
        String wMsg = "Old directory already exists:\n\n" +
          sExportDir + "\n\nPlease delete it first...";
        ShowMessage(wMsg);
        return;
      }
      else
        CreateDirectory(sExportDir.c_str(), NULL); // Create base directory

      // user cancel? return
      if (CopyMusicFiles(ListA, sExportDir) == -1)
        return;

      if (pTFailedToCopyList->Count)
      {
        PromptRetry();
        int retCode = ShowFailures(); // show what's left after retries

        if (retCode < 0)
          return;
      }

      ClearFailedToCopyList();

      // user cancel? return
      if (CopyMusicFiles(ListB, sExportDir) == -1)
        return;

      if (pTFailedToCopyList->Count)
      {
        PromptRetry();
        int retCode = ShowFailures(); // show what's left after retries

        if (retCode < 0)
          return;
      }

      // Save the playlists
      Application->CreateForm(__classid(TExportForm), &pExportForm);

      if (pExportForm == NULL)
        return;

      // EXPORT_EXT is wpl so we need to set the bSaveAsUtf8 flag... since all the files will be
      // copied into the same directory with the associated lists, we just want the file-name in the list,
      // not the path
      // We write in utf8 without BOM

// S.S. we used to write lists into the same directory with music - better to have them one level up...
// By the way, SwiftMiX "can re-pathacize" music lists! - Just Import the list and then export
// it one level up and selecting Relative mode. Works like a charm!

//      String wFile = wUserDir + "\\SwiftMiXA." + EXPORT_EXT;
//      ExportForm->NoDialog(ListA, wFile, EXPORT_PATH_NONE, EXPORT_MODE_UTF8, false, false);
//      wFile = wUserDir + "\\SwiftMiXB." + EXPORT_EXT;
//      ExportForm->NoDialog(ListB, wFile,  EXPORT_PATH_NONE, EXPORT_MODE_UTF8, false, false);
      String sFile = sRootDir + String(EXPORT_FILE) + "A." + String(EXPORT_EXT);
      pExportForm->NoDialog(ListA, sFile, EXPORT_PATH_SWIFTMIX, EXPORT_MODE_UTF8, false, false);
      sFile = sRootDir + String(EXPORT_FILE) + "B." + String(EXPORT_EXT);
      pExportForm->NoDialog(ListB, sFile,  EXPORT_PATH_SWIFTMIX, EXPORT_MODE_UTF8, false, false);
    }
    catch(...)
    {
    }
  }
  __finally
  {
    if (pExportForm)
      pExportForm->Release();
    if (pDirDlgForm)
      pDirDlgForm->Release();
    ClearFailedToCopyList();
    ListA->Progress->UnInit(true);
    ListB->Progress->UnInit(true);
  }
}
//---------------------------------------------------------------------------
int __fastcall TMainForm::CopyMusicFiles(TPlaylistForm* f, String sUserDir)
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
//          String sWmpFile = ExtractFileName(sWmpUrl);
//          if (sWmpFile == uFileName)
//            f->Wmp->URL = "dummy.mp3";
//        }

        // pass in sUserDir - returns sDestPath by reference!
        String sDestPath = sUserDir;
        int retCode = MyFileCopy(f, sDestPath, ii);
        if (retCode < 0)
        {
#if DEBUG_ON
          MainForm->CWrite( "\r\nTMainForm::MyFileCopy() error: " + String (retCode) + "\r\n");
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
void __fastcall TMainForm::MenuViewDiscSpaceRequiredClick(TObject* Sender)
{
  ComputeDiskSpace(DISKSPACE_MESSAGEBOX_OK);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MenuAutoFitToDVDCDClick(TObject* Sender)
{
  __int64 lBytesNeeded = ComputeDiskSpace(DISKSPACE_MESSAGEBOX_NONE);

  if (lBytesNeeded >= 0)
    AutoFitToDVD(lBytesNeeded);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::AutoFitToDVD(__int64 lBytesNeeded)
{
  TAutoSizeForm* pAutoSizeForm = NULL;

  try
  {
    try
    {
      Application->CreateForm(__classid(TAutoSizeForm), &pAutoSizeForm);

      if (pAutoSizeForm == NULL) return;

      if (pAutoSizeForm->ShowModal() == mrOk)
      {
        if (lBytesNeeded > pAutoSizeForm->BytesOnMedia)
        {
          __int64 lBytesOver = lBytesNeeded-pAutoSizeForm->BytesOnMedia;

          // Call function to randomly remove at least "lBytesOver" bytes
          // alternating between lists
          __int64 lBytesUnused = RandomRemove(lBytesOver);

          if (lBytesUnused == -1)
            ShowMessage("User abort before operation was compleated!");
          else if (lBytesUnused < 0)
            ShowMessage("Error in RandomRemove()!");
          else
          {
            String SizeStr;

            try
            {
              SizeStr = FormatFloat("#,##0", (double)lBytesUnused);
            }
            catch(...)
            {
              SizeStr = "(error)";
            }

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
    }
    catch(...)
    {
    }
  }
  __finally
  {
    // Release the resources
    if (pAutoSizeForm)
      pAutoSizeForm->Release();
  }
}
//---------------------------------------------------------------------------
// Add up # bytes needed by songs on both lists
//
// Returns -1 if user wants to quit...
// otherwise returns the total # bytes required for
// both lists.
//
// Mode:
// DISKSPACE_MESSAGEBOX_NONE  0 // no box...
// DISKSPACE_MESSAGEBOX_YESNO 1
// DISKSPACE_MESSAGEBOX_OK    2
__int64 __fastcall TMainForm::ComputeDiskSpace(int Mode)
{
  __int64 total_size_B = 0;
  __int64 total_size_A = 0;
  __int64 total_size = 0;
  int cA = ListA->Count;
  int cB = ListB->Count;
//  int total = cA + cB;

  int retCode;

  try
  {
    String sPath;

    ListA->Progress->Init(cA);
    ClearFailedToCopyList();

    // Total size for list A
    for (int ii = 0; ii < cA; ii++)
    {
      sPath = ListA->CheckBox->Items->Strings[ii];

      if (FileExists(sPath))
      {
        long file_size = MyGetFileSize(sPath);

        if (file_size > 0)
          total_size_A += file_size;
        else
          AddFailure(sPath, ii);
      }

      if (ListA->Progress->Move(ii))
        return -1;
    }

    ListA->Progress->UnInit();

    ListB->Progress->Init(cB);

    // Total size for list B
    for (int ii = 0; ii < cB; ii++)
    {
      sPath = ListB->CheckBox->Items->Strings[ii];

      if (FileExists(sPath))
      {
        long file_size = MyGetFileSize(sPath);

        if (file_size > 0)
          total_size_B += file_size;
        else
          AddFailure(sPath, ii);
      }

      if (ListB->Progress->Move(ii))
        return -1;
    }

    ListB->Progress->UnInit();

    total_size = total_size_A + total_size_B;
  }
  __finally
  {
    // Show list of files that failed to be sized
    retCode = ShowFailures();
    ClearFailedToCopyList();
  }

  if (retCode < 0)
    return retCode;

  // Quit now if not displaying a lot of info
  if (Mode == 0)
    return total_size;

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

    int button = MessageBox(Handle, s.w_str(), L"Export", MB_ICONQUESTION + MB_YESNO + MB_DEFBUTTON2);

    if (button == IDNO)
      return -1;
  }
  else if (Mode == 2)
    MessageBox(Handle, s.w_str(), L"Disc-Space", MB_ICONINFORMATION + MB_OK);

  return total_size;
}
//---------------------------------------------------------------------------
// removes at least TargetBytes worth of song-files from the most populated
// list and returns -1 if user-cancel or the # excess bytes we have now.
__int64 __fastcall TMainForm::RandomRemove(__int64 TargetBytes)
{
  __int64 acc = 0;

  TPlaylistForm* fp;

  while(ListA->Count > 0 || ListB->Count > 0)
  {
    Application->ProcessMessages();

    // Delete from the most-populated list
    fp = (ListA->Count > 0 && ListA->Count > ListB->Count) ? ListA : ListB;

    if (fp->Count > 0)
    {
      int rand_idx = ::Random(fp->Count);

      String sUrl = fp->CheckBox->Items->Strings[rand_idx];

      if (FileExists(sUrl))
      {
        long file_size = MyGetFileSize(sUrl);

        if (file_size > 0)
        {
          int button = IDYES;

          if (bAutoSizePrompt)
          {
            String S_Player = fp == ListA ? L"A" : L"B";
            String s = String("Remove: \"") + sUrl + String("\" (") + String(file_size) +
                  String(" bytes) from Player ") + S_Player + String("'s list?");

            button = MessageBox(Handle, s.w_str(), L"Remove Song?",
                      MB_ICONQUESTION + MB_YESNOCANCEL + MB_DEFBUTTON2);
          }

          if (button == IDYES)
          {
            acc += file_size;
            // Remove the item
            fp->DeleteListItem(rand_idx);
          }

          if (button == IDCANCEL)
            return -1;
        }
      }

      if (acc >= TargetBytes)
        return acc-TargetBytes;
    }
  }

  // Error
  return -2;
}
//---------------------------------------------------------------------------
long __fastcall TMainForm::MyGetFileSize(String sPath)
{
  long lSize = MyGFS(sPath);

  if (lSize <= 0)
  {
    String sSaveAUrl, sSaveBUrl;

    // save Wmp URLs
    if (ListA->Wmp->URL == sPath)
    {
      sSaveAUrl = ListA->Wmp->URL;
      ListA->Wmp->URL = "dummy.mp3";
    }
    if (ListB->Wmp->URL == sPath)
    {
      sSaveBUrl = ListB->Wmp->URL;
      ListB->Wmp->URL = "dummy.mp3";
    }

    // retry
    lSize = MyGFS(sPath);

    // restore saved Wmp URLs
    if (sSaveAUrl.Length() > 0)
      ListA->Wmp->URL = sSaveAUrl;
    if (sSaveBUrl.Length() > 0)
      ListB->Wmp->URL = sSaveBUrl;
  }

  return lSize;
}
//---------------------------------------------------------------------------
long __fastcall TMainForm::MyGFS(String sPath)
{
  HANDLE h = NULL;
  long lSize = -1;

  try
  {
    // Open file to get size (allow shared read access)
    h = CreateFile(sPath.c_str(), GENERIC_READ,
//       0,
//      FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
      FILE_SHARE_READ,
     NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h && h != INVALID_HANDLE_VALUE)
      lSize = ::GetFileSize(h, NULL);
  }
  __finally
  {
    if (h)
      CloseHandle(h);
  }

  return lSize;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FPUpDownChangingEx(TObject *Sender, bool &AllowChange,
          int NewValue, TUpDownDirection Direction)
{
  FfadeAt = NewValue;
}
//---------------------------------------------------------------------------
// NOTE: If the UpDown fade-rate is 0, we force it to be 1 to avoid
// setting the timer interval to 0. 0 is allowed but turns off the
// timer-event's hook. So -9 becomes -10 and +9 becomes +10, effectively.
void __fastcall TMainForm::FRUpDownChangingEx(TObject *Sender, bool &AllowChange,
          int NewValue, TUpDownDirection Direction)
{
  if (NewValue < 0)
  {
    if (NewValue > FRUpDown->Min)
    {
      int PrevPos = ((TUpDown*)Sender)->Position;

      if (NewValue < PrevPos)
      {
        if (FPUpDown->Position < FPUpDown->Max)
          FPUpDown->Position += 3;
      }
      else
      {
        if (FPUpDown->Position > FPUpDown->Min)
          FPUpDown->Position -= 3;
      }
    }

    AutoFadeTimer->Interval = -NewValue*40;
    FaderTrackBar->Frequency = 1;
  }
  else
  {
    AutoFadeTimer->Interval = 50;
    FaderTrackBar->Frequency = NewValue;
  }
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
void __fastcall TMainForm::WindowsMediaPlayer1MediaError(TObject *Sender, LPDISPATCH Item)
{
  if (ListA != NULL)
    ListA->MediaError(Item);
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
void __fastcall TMainForm::WindowsMediaPlayer2MediaError(TObject *Sender, LPDISPATCH Item)
{
  if (ListB != NULL)
    ListB->MediaError(Item);
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::WriteStringToFile(String wPath, String sInfo)
// Writes sInfo (ANSI or UTF-8) to a UTF-16 path
{
  bool bRet = false;
  FILE* f = NULL;

  try
  {
    // open/create file for writing in text-mode
    f = _wfopen(wPath.w_str(), L"wb");

    unsigned length = sInfo.Length();

    if (f != NULL)
      if (fwrite(sInfo.w_str(), sizeof(char), length, f) == length)
        bRet = true;
  }
  __finally
  {
    try { if (f != NULL) fclose(f); } catch(...) {}
  }

  return bRet;
}
//---------------------------------------------------------------------------
String __fastcall TMainForm::GetSpecialFolder(int csidl)
{
  HMODULE h = NULL;
  String sOut;
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
          sOut = String(buf);
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
// set idx -1 to reference the last item in the list
// returns false if list empty or error
bool __fastcall TMainForm::CopyFileToCache(TPlaylistForm* f, int idx)
{
  bool bRet = false;

  if (!f || f->CheckBox->Count == 0)
    return false;

  if (idx < 0)
    idx = f->CheckBox->Count-1;

  try
  {
    // prefetch the next file into our temporary cache directory
    if (bFileCacheEnabled && idx >= 0 && idx < f->CheckBox->Count)
    {
      // add a new file to the cache

      TPlayerURL* p = (TPlayerURL*)f->CheckBox->Items->Objects[idx];
      if (!p) return false;

      // here, we avoid a headache for a large music file that's already in the
      // process of being transferred but might not have completed.
      // So just return "success".
      //
      // We might have a problem if for any reason, the xfer fails - say a web
      // xfer via URL - then we will think we have a cache file but NOT have it.
      //
      // We might need to monitor a media-load failure (see OpenStateChange()
      // in SMList.cpp) and go back to the original url, and erase the cache
      // entry.
      if (p->cacheNumber != 0) // leave this as != 0, not > 0!
        return true;

      p->cacheNumber = -1; // pending cache write

      // pass in CacheDir - returns sDestPath by reference!
      String sDestPath = CacheDir;
      int retCode = MyFileCopy(f, sDestPath, idx);
      if (retCode < 0)
      {
#if DEBUG_ON
        MainForm->CWrite( "\r\nTMainForm::MyFileCopy() error: " + String (retCode) + "\r\n");
#endif
        return false;
      }

// This has been moved into: MoveFiles.cpp TMyFileCopy::TimerEvent()
//      p->cachePath = sDestPath;
//      p->cacheNumber = f->CacheCount+1; // all cache #s must be non-zero!
//
//      // delete the oldest file in the cache...
//      DeleteCacheFile(f);
//
//      f->CacheCount++;
//
//      // if count is 0 (unlikely) - we've exceeded the # files in a 32 bit int
//      // of continuous playback... if so, just turn off caching and the files
//      // (up to MAX_CACHE_FILES) will be deleted on exit along with the directory
//      // containing them.
//      if (f->CacheCount == 0)
//      {
//         bFileCacheEnabled = false;
//         return false;
//      }
    }
  }
  catch(...) {}

  return bRet;
}
//---------------------------------------------------------------------------
// returns true if we had to delete a file from the cache due to
// being at the file limit of MAX_CACHE_FILES
// cacheNumber defaults to 0 (delete oldest cache file)
// Set cacheNumber > 0 to delete a specific cached file
//
// NOTE: f->CacheCount for each player-list just keeps increasing... it's never
// decremented - the reasoning is that we can then always find the oldest cache
// file by searching for the lowest TPlayerURL* p->cacheNumber.
bool __fastcall TMainForm::DeleteCacheFile(TPlaylistForm* f, long cacheNumber)
{
  bool bRet = false;

  try
  {
    // CacheCount will just keep going up as we add new files - which lets us
    // always know which is the oldest (and needs deleting)

// TODO: problem I'm having (I think) is - that we have f->CacheCount+1 > FMaxCacheFiles
// due to drag-dropping files between lists - and we delete files before the max is
// reached - perpetually - deleting files we need.
    if (FMaxCacheFiles > 0 && f->CacheCount+1 > FMaxCacheFiles)
    {
      // scan the list and delete oldest file
      long minCacheCount;
      int listIdx = -1;
      for (int ii = 0; ii < f->CheckBox->Count; ii++)
      {
        TPlayerURL* p = (TPlayerURL*)f->CheckBox->Items->Objects[ii];

        // NOTE: a cacheNumber of 0 means the file has not been
        // cached and the returned URL will be the original file - which
        // we don't want to delete!!!!!!
        if (!p) continue;

        if (cacheNumber == 0)
        {
          // search for oldest cache file
          if (p->cacheNumber > 0)
          {
            if (ii == 0) // initialize
            {
              minCacheCount = p->cacheNumber;
              listIdx = 0;
            }
            else if (p->cacheNumber < minCacheCount)
            {
              minCacheCount = p->cacheNumber;
              listIdx = ii;
            }
          }
        }
        else // find the list index that matches a specific cacheNumber
        {
          if (p->cacheNumber == cacheNumber)
          {
            listIdx = ii;
            break;
          }
        }
      }

      if (listIdx >= 0)
      {
        TPlayerURL* p = (TPlayerURL*)f->CheckBox->Items->Objects[listIdx];

        // be extra careful not to delete the original music file!
        String sOrigPath = f->CheckBox->Items->Strings[listIdx];

        if (p && p->cacheNumber > 0 && FileExists(p->cachePath) && p->cachePath != sOrigPath)
        {
          //ShowMessage(sDelFile);
          //String sCopy;
          //sCopy = L"/c takeown /f \"" + p->cachePath + "\"";
          //ShellCommand(L"open", L"cmd.exe", sCopy.c_str(), true);
          //if (p->bDownloaded)
          //{
            //Attrib Command Options
            //Item Explanation
            //attrib Execute the attrib command alone to see the attributes set on the files within the directory that you execute the command from.
            //+a Sets the archive file attribute to the file or directory.
            //-a Clears the archive attribute.
            //+h Sets the hidden file attribute to the file or directory.
            //-h Clears the hidden attribute.
            //+i Sets the 'not content indexed' file attribute to the file or directory.
            //-i Clears the 'not content indexed' file attribute.
            //+r Sets the read-only file attribute to the file or directory.
            //-r Clears the read-only attribute.
            //+s Sets the system file attribute to the file or directory.
            //-s Clears the system attribute.
            //+v Sets the integrity file attribute to the file or directory.
            //-v Clears the integrity attribute.
            //+x Sets the no scrub file attribute to the file or directory.
            //-x Clears the no scrub attribute.
            // drive:, path, filename This is the file (filename, optionally with drive and path), directory (path, optionally with drive), or drive that you want to view or change the attributes of. Wildcard use is allowed.
            // /s Use this switch to execute whatever file attribute display or changes you're making on the subfolders within whatever drive and/or path you've specified, or those within the folder you're executing from if you don't specify a drive or path.
            // /d This attrib option includes directories, not only files, to whatever you're executing. You can only use /d with /s.
            // /l The /l option applies whatever you're doing with the attrib command to the Symbolic Link itself instead of the target of the Symbolic Link. The /l switch only works when you're also using the /s switch.
            // /? Use the help switch with the attrib command to show details about the above options right in the Command Prompt window. Executing attrib /? is the same as using the help command to execute help attrib.
            //sCopy = L"/c attrib -s -r -h -i -x -u +a \"" + p->cachePath + "\""; // +/-RASHIXU "file" /S/D/L
            //ShellCommand(L"open", L"cmd.exe", sCopy.c_str(), true);
          //}

// try just deleting in real-time - otherwise p->cacheCount is not synced! - S.S.
          DeleteFile(p->cachePath);

//          sCopy = L"/c del /Q /F \"" + p->cachePath + "\"";
//          ShellCommand(L"open", L"cmd.exe", sCopy.c_str(), false);

          //if (!ShellCommand(L"open", L"cmd.exe", sCopy.c_str(), true))
          //  ShowMessage("failed to delete: \"" + String(sCopy) + "\"");

          // very important to restore original URL and dequeue this!!!!
          p->cachePath = sOrigPath;
          p->cacheNumber = 0;

          bRet = true; // return success
        }
      }
    }
  }
  catch(...) {}

  return bRet;
}
//---------------------------------------------------------------------------
String __fastcall TMainForm::GetURL(TCheckListBox* l, int idx)
{
  String sPath;

  try
  {
    if (bFileCacheEnabled)
    {
      TPlayerURL* p = (TPlayerURL*)l->Items->Objects[idx];

      if (p && p->cacheNumber > 0)
        sPath = p->cachePath;
      else
        sPath = l->Items->Strings[idx];
    }
    else
      sPath = l->Items->Strings[idx];
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
// bCopyNow defaults false
int __fastcall TMainForm::MyFileCopy(TPlaylistForm* f, String &sDestDir, int idx)
{
  if (!f || !pTMyFileCopyList || idx < 0 || idx >= f->CheckBox->Count || !sDestDir.Length())
    return -2;

  String sSourcePath = f->CheckBox->Items->Strings[idx];
  TPlayerURL* p = (TPlayerURL*)f->CheckBox->Items->Objects[idx];

  if (!p || !sSourcePath.Length())
    return -3;

  p->bDownloaded = false;

  String sDestPath = sDestDir + "\\" ;

  // check sSourcePath's first characters for ftp:\\, http:\\ or https:\\
  // see if this is a URL that MyCopyUrl can handle...
  String lsSourcePath = sSourcePath.LowerCase();
  int iPos1 = lsSourcePath.Pos("http://");
  int iPos2 = lsSourcePath.Pos("https://");
  int iPos3 = lsSourcePath.Pos("ftp://");

  bool bIsWebFile;

  if (iPos1 == 1 || iPos2 == 1 || iPos3 == 1)
  {
    int ii;
    int len = sSourcePath.Length();
    for (ii = len; ii > 0; ii--)
      if (sSourcePath[ii] == '/' || sSourcePath[ii] == '\\' || sSourcePath[ii] == ':')
        break;

    sDestPath += sSourcePath.SubString(ii+1, len-ii);
    bIsWebFile = true;
  }
  else
  {
    sDestPath += ExtractFileName(sSourcePath);
    bIsWebFile = false;
  }

  if (!FileExists(sDestPath))
  {
    // non-blocking local or web file-copy

    // this bit of code limits the max # of simultaneous TMyFileCopy objects
    // NOTE: TMyUrlCopy and TMyFileCopy both add themselves to MyFileCopyList!
    if (MyFileCopyList->Count > MAX_TMyFileCopy_OBJECTS)
    {
      do
      {
        Sleep(20);
        Application->ProcessMessages();
      } while (MyFileCopyList->Count > MAX_TMyFileCopy_OBJECTS);
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

  sDestDir = sDestPath; // return full-path by-reference
  return 0;
}
//---------------------------------------------------------------------------
FARPROC __fastcall TMainForm::LoadProcAddr(HINSTANCE hDll, String entry)
{
  if (entry.Length() == 0)
    return NULL;

  FARPROC addr;

  // Apparently GetProcAddress is AnsiString, not wchar_t!!!!!!!!!!
  AnsiString s = (AnsiString)entry;

  try
  {
    addr = (FARPROC)GetProcAddress(hDll, (LPCSTR)s.c_str());
  }
  catch(...)
  {
#if DEBUG_ON
    MainForm->CWrite( "\r\nTMainForm::LoadProcAddr() exception!\r\n");
#endif
    return NULL;
  }

  return addr;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::AddFailure(String sPath, int iIndex)
{
  AddFailure(sPath, "", iIndex, 0);
}

void __fastcall TMainForm::AddFailure(String sSource, String sDest, int iSource, int iList)
{
  TFailVars* p = new TFailVars();
  if (p)
  {
    p->m_sSource = sSource;
    p->m_sDest = sDest;
    p->m_iSource = iSource;
    p->m_iList = iList;
    pTFailedToCopyList->Add((void*)p);
#if DEBUG_ON
    MainForm->CWrite( "\r\nTMainForm::AddFailure(): " + String(iSource) + "dest: \"" + sDest + "\r\n");
#endif
  }
}
//---------------------------------------------------------------------------
// return -1 if user chooses to quit, 0 to continue
int __fastcall TMainForm::ShowFailures(void)
{
  int iFailCount = pTFailedToCopyList->Count;
  if (iFailCount)
  {
    String sFailed = "Failed to copy " + String(iFailCount) + " songs! Continue?\n\n";

    if (iFailCount > 10)
    {
      sFailed += "(truncating list shown to the first 10...)\n\n";
      iFailCount = 10;
    }

    for (int ii = 0; ii < iFailCount; ii++)
    {
      TFailVars* p = (TFailVars*)pTFailedToCopyList->Items[ii];
      if (p)
      {
        if (p->m_sDest.IsEmpty())
          sFailed += "Path: \"" + p->m_sSource + "\" : " + String(p->m_iSource) + "\n";
        else
        {
          sFailed += "Source: \"" + p->m_sSource + "\" index: " + String(p->m_iSource) +
                        ", " + String(p->m_iList ? "list B" : "list A") + "\n";
          sFailed += "Dest: \"" + p->m_sDest + "\"\n\n";
        }
      }
    }

    if (MessageBox(Handle, sFailed.c_str(), L"File Copy Fail-Report", MB_ICONQUESTION +
                                       MB_YESNO + MB_DEFBUTTON2) == IDNO)
      return -1;
  }
  return 0;
}
//---------------------------------------------------------------------------
// deletes TFailVars objects in the pTFailedToCopyList (TList)
void __fastcall TMainForm::ClearFailedToCopyList(void)
{
  if (pTFailedToCopyList)
  {
    while(pTFailedToCopyList->Count)
    {
      TFailVars* p = (TFailVars*)pTFailedToCopyList->Items[0];
      if (p) delete p; // delete each instantiated object of TFileCopy
      pTFailedToCopyList->Delete(0); // delete list-entry
    }
  }
}
//---------------------------------------------------------------------------
// retry file copy
void __fastcall TMainForm::PromptRetry(void)
{
  if (pTFailedToCopyList)
  {
    if (pTFailedToCopyList->Count)
    {
      // have to go backward...
      for(int ii = pTFailedToCopyList->Count-1; ii >= 0 ; ii--)
      {
        TFailVars* p = (TFailVars*)pTFailedToCopyList->Items[ii];

        if (p)
        {
          String sMsg = "Retry copy of file:\n\n\"" + p->m_sSource + "\"?";

          int button = MessageBox(Handle, sMsg.w_str(), L"File Copy Retry", MB_ICONQUESTION +
                                             MB_YESNOCANCEL + MB_DEFBUTTON1);
          if (button == IDCANCEL)
            break;

          if (button == IDNO)
            continue;

          // copy and overwrite (failed should have been deleted)
          // this will delete successfully copied files from the fail-list - then,
          // you can call ShowFailures and it should show nothing!
          if (CopyFile(p->m_sSource.c_str(), p->m_sDest.c_str(), false))
          {
            delete p;
            pTFailedToCopyList->Delete(ii);
          }
        }
      }
    }
  }
}
//---------------------------------------------------------------------------
// UTF8 Encode/Decode
//---------------------------------------------------------------------------
//AnsiString __fastcall TMainForm::AnsiToUtf8(AnsiString sIn)
//// Code to convert ANSI to UTF-8 (Works!)
//{
//  if (sIn.IsEmpty()) return "";
//
//  // Use the MultiByteToWideChar function to determine the size of the UTF-16 representation of the string. You use
//  // this size to allocate a new buffer that can hold the UTF-16 version of the string.
//  DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, sIn.c_str(), -1, NULL, 0);
//  wchar_t *pwText = new wchar_t[dwNum];
//
//  // The MultiByteToWideChar function takes the ASCII string and converts it into UTF-16, storing it in pwText.
//  MultiByteToWideChar(CP_ACP, 0, sIn.c_str(), -1, pwText, dwNum);
//
//  // The WideCharToMultiByte function tells you the size of the returned string so you can create a buffer for the UTF-8 representation.
//  dwNum = WideCharToMultiByte(CP_UTF8, 0, pwText, -1, NULL, 0, NULL, NULL);
//  char *psText = new char[dwNum];
//
//  // Convert the UTF-16 string into UTF-8, storing the result into psText.
//  WideCharToMultiByte(CP_UTF8, 0, pwText, -1, psText, dwNum, NULL, NULL);
//
//  delete [] pwText;
//
//  // Convert to VCL String.
//  if (dwNum > 1) sIn = String(psText, dwNum-1);
//  else sIn = "";
//
//  delete [] psText;
//
//  return sIn;
//}
//---------------------------------------------------------------------------
//AnsiString __fastcall TMainForm::Utf8ToAnsi(AnsiString sIn)
//// Code to convert UTF-8 to ASCII
//{
//  // Use the MultiByteToWideChar function to determine the size of the UTF-16 representation of the string. You use
//  // this size to allocate a new buffer that can hold the UTF-16 version of the string.
//  DWORD dwNum = MultiByteToWideChar(CP_UTF8, 0, sIn.c_str(), -1, NULL, 0);
//  wchar_t *pwText = new wchar_t[dwNum];
//
//  // The MultiByteToWideChar function takes the UTF-8 string and converts it into UTF-16, storing it in pwText.
//  MultiByteToWideChar(CP_UTF8, 0, sIn.c_str(), -1, pwText, dwNum);
//
//  // The WideCharToMultiByte function tells you the size of the returned string so you can create a buffer for the ANSI representation.
//  dwNum = WideCharToMultiByte(CP_ACP, 0, pwText, -1, NULL, 0, NULL, NULL);
//  char *psText = new char[dwNum];
//
//  // Convert the UTF-16 string into ANSI, storing the result into psText.
//  WideCharToMultiByte(CP_ACP, 0, pwText, -1, psText, dwNum, NULL, NULL);
//
//  delete [] pwText;
//
//  // Convert to VCL String.
//  if (dwNum > 1) sIn = String(psText, dwNum-1);
//  else sIn = "";
//
//  delete [] psText;
//
//  return sIn;
//}
//---------------------------------------------------------------------------
//WideString __fastcall TMainForm::Utf8ToWide(AnsiString sIn)
//// Code to convert UTF-8 to UTF-16 (wchar_t)
//{
//  WideString sWide = L"";
//
//  if (sIn.Length() > 0)
//  {
//    wchar_t* pwText = NULL;
//
//    try
//    {
//      // Use the MultiByteToWideChar function to determine the size of the UTF-16 representation of the string. You use
//      // this size to allocate a new buffer that can hold the UTF-16 version of the string.
//      DWORD dwNum = MultiByteToWideChar(CP_UTF8, 0, sIn.c_str(), -1, NULL, 0);
//      wchar_t *pwText = new wchar_t[dwNum];
//
//      if (pwText != NULL)
//      {
//        // The MultiByteToWideChar function takes the UTF-8 string and converts it into UTF-16, storing it in pwText.
//        MultiByteToWideChar(CP_UTF8, 0, sIn.c_str(), -1, pwText, dwNum);
//
//        // Convert to VCL WideString.
//        if (dwNum > 1)
//          sWide = WideString(pwText, dwNum-1);
//      }
//    }
//    // NOTE: Unlike C#, __finally does NOT get called if we execute a return!!!
//    __finally
//    {
//      try { if (pwText != NULL) delete [] pwText; } catch(...) {}
//    }
//  }
//
//  return sWide;
//}
//---------------------------------------------------------------------------
AnsiString __fastcall TMainForm::WideToUtf8(WideString sIn)
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

  AnsiString sOut = String(buf, nLenUtf8); // there is no null written...
  delete [] buf;

  return sOut;
}
//---------------------------------------------------------------------------
//void __fastcall TMainForm::WMMove(TWMMove &Msg)
//{
//  try
//  {
//    if (MainForm->GDock != NULL)
//        MainForm->GDock->WindowMoved(this->Handle);
//    // call the base class handler
//    TForm::Dispatch(&Msg);
//  }
//  catch(...) { }
//}
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
  WriteConsole(m_Screen, String(S).w_str(),S.Length(),0,0);
}
#endif
//---------------------------------------------------------------------------


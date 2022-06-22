//---------------------------------------------------------------------------
#include <vcl.h>
#include "Main.h"
#pragma hdrstop

#include "Urlmon.h"
#include "Progress.h"
#include <WinUser.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TPlaylistForm* ListA = NULL;
TPlaylistForm* ListB = NULL;

//---------------------------------------------------------------------------
__fastcall STRUCT_A::STRUCT_A()
{
  player = -1;
  duration = 0;

  len_path = 0;
  len_name = 0;
  len_artist = 0;
  len_album = 0;

  path[0] = NULLCHAR;
  name[0] = NULLCHAR;
  artist[0] = NULLCHAR;
  album[0] = NULLCHAR;
}
//---------------------------------------------------------------------------
__fastcall STRUCT_B::STRUCT_B()
{
  player = -1;
  state = 0;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormCreate(TObject* Sender)
{
  Color = TColor(0xF5CFB8);
  m_bInhibitFlash = false;
  m_bDoubleClick = false;
  m_bCheckClick = false;
  m_Duration = 0;
  m_PrevState = 0;
  m_failSafeCounter = 0;
  m_TimerMode = TM_NULL;
  m_bSkipFilePrompt = false;
  m_bOpening = false;
  FPlayIdx = -1;
  FTempIdx = -1;

  // properties
  FCacheCount = 0;
  FNextIndex = -1;
  FTargetIndex = -1;
  FOldMouseItemIndex = -1;
  FTextColor = clBlack;
  FWmp = NULL;
  FOtherWmp = NULL;
  FPlayerA = true; // this will remain set if this list is for player A
  FEditMode = false;
  FPlayPreview = false;
  FKeySpaceDisable = false;
  pOFMSDlg = NULL;
  Application->CreateForm(__classid(TProgressForm), &pProgress);

  CheckBox->Color = TColor(0xF5CFB8);
  CheckBox->Parent = this;
//  CheckBox->OnMouseMove = CheckBoxMouseMove;

  //enable drag&drop files
  ::DragAcceptFiles(this->Handle, true);

  // Start processing custom windows messages (for WM_SETTEXT to Unicode)
//  OldWinProc = WindowProc;
//  WindowProc = CustomMessageHandler;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  if (pProgress && pProgress->Count)
  {
    for (int ii = 0; ii < pProgress->Count; ii++)
      pProgress->Canceled = true;

    CanClose = false;
    ShowMessage("This list is busy, cancelling... try again in 15 seconds.");
  }
  else
  {
    CanClose = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormClose(TObject *Sender, TCloseAction &Action)
{
  StopPlayPreview();
  DestroyFileDialog();
  DestroyImportDialog();
  DestroyExportDialog();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormDestroy(TObject *Sender)
{
  ClearAndStop();
  DestroyProgressForm(); // have to do this here since it's created in FormCreate()!

//  if (CheckBox)
//    delete CheckBox;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y)
{
  // restart debounce timer
  MouseMoveDebounceTimer->Enabled = false;
  MouseMoveDebounceTimer->Interval = 500;
  MouseMoveDebounceTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::MouseMoveDebounceTimerEvent(TObject *Sender){
  StartPlayPreview();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::GeneralPurposeTimerEvent(TObject* Sender)
// Scroll to FPlayIdx item
{
  // timeout after user has pressed left, right up or down before
  // we exit Edit mode
  if (m_TimerMode == TM_SCROLL_KEY_PRESSED)
  {
    // this will set the item to the one the user has scrolled to
    // when SetTitle() is called
    if (!IsPlayOrPause())
    FPlayIdx = CheckBox->ItemIndex;
    FEditMode = false;
    QueueToIndex(CheckBox->ItemIndex);
    FEditMode = true;

    ExitEditModeClick(NULL);
  }

  if (FEditMode) return;

  switch(m_TimerMode)
  {
    case TM_START_PLAYER:
      GeneralPurposeTimer->Enabled = false;
      StartPlayer();
    break;

    case TM_FADE:

      GeneralPurposeTimer->Enabled = false;

      if (OtherForm->UnplayedSongsInList())
        MainForm->ForceFade();
      else
        NextSong(true);
    break;

    case TM_NEXT_SONG:
      GeneralPurposeTimer->Enabled = false;
      NextSong(true); // force start of player
    break;

    case TM_NEXT_SONG_CHECK:

      GeneralPurposeTimer->Enabled = false;

      if (!OtherForm->UnplayedSongsInList())
      {
//        FNextIndex = -1;
        NextSong(true);
      }

    break;

    case TM_STOP_PLAYER:
      GeneralPurposeTimer->Enabled = false;
      StopPlayer();
    break;

    case TM_STOP_PLAY_PREVIEW:
      GeneralPurposeTimer->Enabled = false;
      StopPlayPreview();
    break;

    case TM_CHECKBOX_CLICK:

      GeneralPurposeTimer->Enabled = false;

      if (m_bDoubleClick)
      {
        if (!Wmp)
        {
          m_bDoubleClick = false;
          m_bCheckClick = false;
          return;
        }

        // Check the item so GetNext() will queue it
        if (!IsStateGrayed(CheckBox, CheckBox->ItemIndex))
          SetGrayedState(CheckBox->ItemIndex);

        // sometimes we have a stopped player but FPlayIdx was not set to -1
        // this should not be needed if we eventually fix that...
        if (FPlayIdx >= 0 && !IsPlayOrPause())
          FPlayIdx = -1;

        int savePlayIdx = FPlayIdx;

        FNextIndex = CheckBox->ItemIndex; // tell GetNext() where to start search...
        Wmp->URL = GetNext();

        // Currently playing song's checkbox may need to be cleared...
        ClearCheckState(savePlayIdx, false);

        if (FPlayIdx >= 0)
        {
          m_bSkipFilePrompt = true; // don't ask for other-player files

          // Start this player with new song
          StartPlayer();
        }
      }
      else if (m_bCheckClick)
      {
        if (Wmp == NULL)
        {
          m_bDoubleClick = false;
          m_bCheckClick = false;
          return;
        }

        // disable highlight-scroll and flasher
        FlashTimer->Enabled = false;

        // we just clicked the song's checkbox and now it's gray (enabled,
        // but not playing), queue it up...
        if (CheckBox->State[CheckBox->ItemIndex] == cbGrayed) // don't use IsStateGrayed() here!
        {
#if DEBUG_ON
          MainForm->CWrite("\r\nTM_CHECKBOX_CLICK:m_bCheckClick: 0 (State == cbGrayed)\r\n");
#endif
          SetItemState(CheckBox->ItemIndex); // set the TPlayerURL state to match the list-box item's state
          QueueToIndex(CheckBox->ItemIndex);
        }
        else
        {
          ClearCheckState(CheckBox->ItemIndex, false);

          // nothing playing?
          if (FPlayIdx < 0 || !IsPlayOrPause())
          {
#if DEBUG_ON
            MainForm->CWrite("\r\nTM_CHECKBOX_CLICK:m_bCheckClick: 1 (State != cbGrayed): queue next item\r\n");
#endif
            // if we unchecked an item that was queued, queue the
            // next item...
            FNextIndex = CheckBox->ItemIndex; // start search here
            GetNext();
          }
          else if (FPlayIdx == CheckBox->ItemIndex)
          {
#if DEBUG_ON
            MainForm->CWrite("\r\nTM_CHECKBOX_CLICK:m_bCheckClick: 2 (State != cbGrayed): start timer fade\r\n");
#endif
            SetTimer(TM_FADE);
          }
          else if (FTargetIndex == CheckBox->ItemIndex)
          {
#if DEBUG_ON
            MainForm->CWrite("\r\nTM_CHECKBOX_CLICK:m_bCheckClick: 3 (State != cbGrayed): GetNext(true)\r\n");
#endif
            FNextIndex = CheckBox->ItemIndex; // start search here
            GetNext(true);
          }
          else
          {
#if DEBUG_ON
            MainForm->CWrite("\r\nTM_CHECKBOX_CLICK:m_bCheckClick: 4 (State != cbGrayed): no action SetTitle()\r\n");
#endif
          }
        }

        SetTitle();
      }
      else // single click an item
      {
        if (!Wmp)
        {
          m_bDoubleClick = false;
          m_bCheckClick = false;
          return;
        }

        // disable highlight-scroll and flasher
        FlashTimer->Enabled = false;

        // If we clicked on an item (not a checkbox)... queue it
        // or if we checked an unchecked box... queue it
        // FTempIdx will have the ItemIndex from the CheckBoxMouseDown()
        // event
        if (FTempIdx >= 0 && FTempIdx < CheckBox->Count)
        {
#if DEBUG_ON
          MainForm->CWrite("\r\nTM_CHECKBOX_CLICK: NOT m_bCheckClick: FTempIdx in-bounds...\r\n");
#endif
          // Check the item so GetNext() will queue it
          if (!IsStateGrayed(CheckBox, FTempIdx)){
            SetGrayedState(FTempIdx);
#if DEBUG_ON
            MainForm->CWrite("\r\nTM_CHECKBOX_CLICK: NOT m_bCheckClick: (State != cbGrayed) setting grayed state...\r\n");
#endif
          }

          // If we were playing, may need to Check the item
          if (IsPlayOrPause() && FPlayIdx >= 0 && FPlayIdx < CheckBox->Count &&
                                       !IsStateChecked(CheckBox, FPlayIdx)){
            SetCheckedState(FPlayIdx);
#if DEBUG_ON
            MainForm->CWrite("\r\nTM_CHECKBOX_CLICK: NOT m_bCheckClick: (State != cbGrayed) setting checked state...\r\n");
#endif
          }

          // Copy the clicked file to the cache - (however, we still might be
          // drag-dropping it someplace...)
          MainForm->CopyFileToCache(this, FTempIdx);

          // This will trigger a copy to cache also - but of the song
          // after this one...
          this->QueueToIndex(FTempIdx);
          FTempIdx = -1;
        }
      }

      m_bDoubleClick = false;
      m_bCheckClick = false;

    break;

    default:
      GeneralPurposeTimer->Enabled = false;
      m_TimerMode = TM_NULL;
    break;
  };
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::UnplayedSongsInList(void)
{
  GetNext(true); // set bNoSet since we're just checking
  return (TargetIndex < 0) ? false : true;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FlashTimerEvent(TObject* Sender)
{
  // 10=ready/stop
  // 1=stopped
  // 2=paused
  // 3=playing
  static bool bFlashOn = false;

  if (FEditMode || FPlayPreview) return;

  if (!Wmp || GeneralPurposeTimer->Enabled) return;

  if (CheckBox->Focused())
    CheckBox->ItemIndex = FTargetIndex;
  // if no items, or the player's index is -1, or player is in an unknown state, turn off selection
  else if (!CheckBox->Count || FPlayIdx < 0 || Wmp->playState == WMPOpenState::wmposUndefined)
    CheckBox->ItemIndex = -1;
  // if player is in pause or in play-mode
  else if (IsPlayOrPause())
  {
    // Flash faster if in pause
    if (Wmp->playState == WMPPlayState::wmppsPaused)
    {
      if (FlashTimer->Interval != 250)
        FlashTimer->Interval = 250;
    }
  else if (FlashTimer->Interval != 500)
      FlashTimer->Interval = 500;

    if (bFlashOn)
    {
      CheckBox->ItemIndex = -1;
      bFlashOn = false;
    }
    else
    {
      CheckBox->ItemIndex = FPlayIdx;
      bFlashOn = true;
    }
  }
  // if player is in stop or ready mode
  else
    CheckBox->ItemIndex = FPlayIdx;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxClickCheck(TObject* Sender)
{
  if (!FEditMode && !FPlayPreview)
  {
    m_bCheckClick = true;

    // when clicking a check-box with the mouse, CheckBoxClick() (below)
    // is also triggeres, which starts TM_CHECKBOX_CLICK - but just pressing the
    // spacebar toggles the check-state and triggers CheckBoxClickCheck() but NOT
    // CheckBoxClick()! So - SetTimer(TM_CHECKBOX_CLICK, TIME_300) here also
    // fixes the problem - otherwise, the checkbox goes unprocessed...
    SetTimer(TM_CHECKBOX_CLICK, TIME_300);

#if DEBUG_ON
    MainForm->CWrite("\r\nTPlaylistForm::CheckBoxClickCheck()\r\n");
#endif
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxClick(TObject* Sender)
{
// problem I'm having is that this event happens when mouse button is released on another
// item after a list item drag-drop. need to ignore it if the index has changed.
// the original index was saved in FTempIdx. The drop event might not have happened
// yet - we need ItemAtPos and to get the mouse X,Y coordinates.
//  if (FTempIdx != CheckBox->ItemAtPos(Mouse->CursorPos, true))
//    ShowMessage("moved");
  if (!FEditMode && !FPlayPreview)
  {
    SetTimer(TM_CHECKBOX_CLICK, TIME_300);
#if DEBUG_ON
    MainForm->CWrite("\r\nTPlaylistForm::CheckBoxClick()\r\n");
#endif
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxDblClick(TObject* Sender)
{
  if (!FEditMode && !FPlayPreview)
    m_bDoubleClick = true;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormActivate(TObject* Sender)
{
  // disable highlight-scroll and flasher
  SetTitle();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormDeactivate(TObject* Sender)
{
//  if (FEditMode)
//    ExitEditModeClick(NULL);

  StopPlayPreview();

  if (Visible)
    SetTitle();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormHide(TObject* Sender)
// This will fire first, then Deactivate fires...
{
  FlashTimer->Enabled = false;
}
//---------------------------------------------------------------------------
// We need to get listbox item index when popup menu right-click
// is performed. We save it in FTempIdx for use by various
// routines in the popup menu, such as getting the song's tag-info.
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxContextPopup(TObject *Sender, TPoint &MousePos,
          bool &Handled)
{
  FTempIdx = CheckBox->ItemAtPos(MousePos, true);
  Handled = false;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxMouseDown(TObject* Sender,
               TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (Button != mbLeft)
    return;

  try
  {
    if (!FEditMode && !FPlayPreview && CheckBox->Count)
    {
      int Index = CheckBox->ItemAtPos(Point(X,Y), true);

      // FTempIndex is used in timer-event TM_CHECKBOX_CLICK
      // and in MyMoveSelected() for non-EditMode drag-drop
      if (Index >= 0 && Index < CheckBox->Count)
      {
        FTempIdx = Index;

        // MyMoveSelected() needs a selected item!
        if (!FEditMode)
          CheckBox->Selected[FTempIdx] = true;
      }
    }
  }
  catch(...) { }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxDragDrop(TObject* Sender, TObject* Source, int X, int Y)
// User has released a dragged item(s) over the destination list...
{
  if (!Sender->ClassNameIs("TCheckListBox") || !Source->ClassNameIs("TCheckListBox"))
    return;

  TCheckListBox* DestList = (TCheckListBox*)Sender;
  TCheckListBox* SourceList = (TCheckListBox*)Source;
  MyMoveSelected(DestList, SourceList, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::MoveSelectedClick(TObject *Sender)
{
  MyMoveSelected(OtherForm->CheckBox, CheckBox);
}
//---------------------------------------------------------------------------
// x and y are default mouse coordinates (-1)
void __fastcall TPlaylistForm::MyMoveSelected(TCheckListBox* DestList, TCheckListBox* SourceList, int x, int y)
// PopupMenu2 F4
{
  // Move Selected Items To Other Playlist

  TStringList* sl = NULL;

  try
  {
    TPlaylistForm* SourceForm = (TPlaylistForm*)SourceList->Parent;
    TPlaylistForm* DestForm = (TPlaylistForm*)DestList->Parent;

    // get destination list index
    int DestIndex;
    if (x >= 0 && y >= 0)
      DestIndex = DestList->ItemAtPos(Point(x, y), true);
    else
      DestIndex = DestList->Items->Count; // insert at bottom

    // mark the DestIndex item so we can find it after deleting
    // selected items...
    TObject* SavePointer;
    if (DestIndex < DestList->Items->Count && DestIndex >= 0)
    {
      SavePointer = DestList->Items->Objects[DestIndex];
      DestList->Items->Objects[DestIndex] = NULL; // mark it with NULL
    }
    else
      SavePointer = NULL;

    sl = new TStringList();

    int iCount = SourceList->Count;
    pProgress->Init(iCount);

    // add selected items to sl and delete them
    for (int ii = iCount-1; ii >= 0; ii--)
    {
      if (SourceList->Selected[ii])
    {
        // prevent move to a selected item in same list
        if (SourceForm == DestForm)
        {
          if (DestIndex == ii)
          {
            ShowMessage("You can't move selected items onto a selected item!");
            return; // __ finally will clean up...
          }
        }
        // skip move of playing song to other player's list
        else if (IsStateChecked(SourceList, ii))
          continue;

        TPlayerURL* p = (TPlayerURL*)SourceList->Items->Objects[ii];
        if (p)
        {
          p->state = SourceList->State[ii]; // cbChecked indicates a playing song
          p->listIndex = ii;
        }

        sl->AddObject(SourceList->Items->Strings[ii], (TObject*)p);
        SourceList->Items->Delete(ii);
      }

      if (pProgress->Move(ii))
        return;
    }

    // find the new location of DestIndex from our NULL marker...
    DestIndex = DestList->Items->Count; // if NULL not found use end of list
    for (int ii = 0; ii < DestList->Items->Count; ii++)
    {
    if (DestList->Items->Objects[ii] == NULL)
      {
         DestList->Items->Objects[ii] = SavePointer; // restore pointer
         DestIndex = ii; // new DestIndex!
         break;
      }
    }

    if (DestIndex < 0)
    {
      ShowMessage("Error: DestIndex < 0!");
      return; // __ finally will clean up...
    }

    // add stringlist items into destination list
    for (int ii = 0; ii < sl->Count; ii++)
    {
      TPlayerURL* p = (TPlayerURL*)sl->Objects[ii];
      DestList->Items->InsertObject(DestIndex, sl->Strings[ii], (TObject*)p);

      if (p)
      {
        // reassign the cache-number to the other list's (ever-increasing) counter
        if (p->cacheNumber > 0 && DestList != SourceList)
        {
          DestForm->CacheCount++;
          p->cacheNumber = DestForm->CacheCount;
        }

        DestList->State[DestIndex] = p->state;
      }
      else
      {
    ShowMessage("Error: Missing TPlayerURL object! (" + String(ii) + ")");
        return; // __ finally will clean up...
      }
    }

    // scan source-list to reset the playing song's FPlayIdx and ItemIndex
    int SourceFPlayIdx = -1;
    for (int ii = 0 ; ii < SourceList->Count ; ii++)
    {
      if (IsStateChecked(SourceList, ii))
      {
        SourceFPlayIdx = ii;
        break;
      }
    }

    // TODO!!!!!!!!!!!!!!!!!
    // might want to add a field in p for the index of the next queued song and
    // be able to restore that for both source and dest lists...
    // - for now, force it to the first song
    if (SourceFPlayIdx < 0 && SourceList->Count)
      SourceFPlayIdx = 0;

    SourceForm->FPlayIdx = SourceFPlayIdx;
    SourceList->ItemIndex = SourceFPlayIdx;

    if (SourceList->Count)
    {
      // Queue a song in the source list and add the file(s) to the cache
      if (SourceForm != DestForm)
      {
        MainForm->CopyFileToCache(SourceForm, SourceForm->FPlayIdx);
        SourceForm->QueueToIndex(SourceForm->FPlayIdx);
    }
      else
      {
        MainForm->CopyFileToCache(SourceForm, DestIndex);
        SourceForm->QueueToIndex(DestIndex);
      }
    }

    // Exit edit-mode
    if (SourceForm->FEditMode)
      SourceForm->ExitEditModeClick(NULL);

    if (SourceForm != DestForm)
    {
      // scan destination-list to reset the playing song's FPlayIdx and ItemIndex
      int DestFPlayIdx = -1;
      for (int ii = 0 ; ii < DestList->Count ; ii++)
      {
        if (IsStateChecked(DestList, ii))
        {
          DestFPlayIdx = ii;
          break;
        }
      }

      DestForm->FPlayIdx = DestFPlayIdx;
      DestList->ItemIndex = DestFPlayIdx;

      // Queue a song in the destination list and add the file(s) to the cache
      if (DestIndex >= 0)
      {
        MainForm->CopyFileToCache(DestForm, DestIndex);
        DestForm->QueueToIndex(DestIndex);
    }

      // Exit edit-mode
      if (DestForm->FEditMode)
        DestForm->ExitEditModeClick(NULL);
    }
  }
  __finally
  {
    if (sl)
      delete sl;

    // set flag true if this is the "top-level" method in the overall progress-chain...
    // (it deletes all the history of prior progressbar nestings)
    pProgress->UnInit(true);

//    if (DestForm != SourceForm)
//      MainForm->ShowPlaylist(DestForm);
  }
}
//---------------------------------------------------------------------------
// bAllowDequeue defaults true
void __fastcall TPlaylistForm::DeleteListItem(int idx, bool bDeleteFromCache)
{
  try
  {
    if (idx < 0 || idx >= CheckBox->Count)
      return;

    // it's messy to remove an item that's playing... taking the easy way out!
    if (FPlayIdx == idx && IsPlayOrPause())
      return;

  // Is this song playing? If so, go to next song unless bDeleteFromCache is false.
//    if (bDeleteFromCache && idx == FPlayIdx)
//    {
//      FNextIndex = FTargetIndex;
//      NextSong();
//    }

    int SaveTargetIndex = FTargetIndex;
//    int SavePlayIndex = FPlayIdx;

    if (FTargetIndex > idx)
      FTargetIndex--;
    if (FPlayIdx > idx)
      FPlayIdx--;
    if (FNextIndex > idx)
      FNextIndex--;

    // delete TPlayerURL object
    if (MainForm->CacheEnabled)
    {
      TPlayerURL* p = (TPlayerURL*)CheckBox->Items->Objects[idx];
      if (p)
      {
        // delete a specific cached-file (if it exists)
        if (bDeleteFromCache && p->cacheNumber > 0)
          MainForm->DeleteCacheFile(this, p->cacheNumber);

        delete p;
      }
    }

    // remove item from list
    CheckBox->Items->Delete(idx);

    if (!CheckBox->Count)
    {
      FTargetIndex = -1;
      FNextIndex = -1;
      FPlayIdx = -1;
    }

    if (SaveTargetIndex == idx)
    {
      FNextIndex = idx; // start search here
      if (NextIndex >= CheckBox->Count)
        FNextIndex = 0;
      GetNext(true);
    }
  }
  catch(...) { ShowMessage("Error deleting list index: " + String(idx)); }
}
//---------------------------------------------------------------------------
// overloaded...

void __fastcall TPlaylistForm::AddListItem(String s)
{
  TPlayerURL* p = InitTPlayerURL(s);
  p->listIndex = CheckBox->Count;
  AddListItem(s, p);
}

void __fastcall TPlaylistForm::AddListItem(String s, TPlayerURL* p)
{
  String sTags = (p == NULL) ? s : GetTags(p);
  CheckBox->Items->AddObject(sTags, (TObject*)p);
  SetGrayedState(CheckBox->Count-1);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::InsertListItem(int idx, String s, TPlayerURL* p)
{
  CheckBox->Items->InsertObject(idx, s, (TObject*)p);
  SetGrayedState(idx);
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::RestoreCache(void)
{
  // reset/restore all song-file TPlayerURL structs
  for (int ii = 0; ii < CheckBox->Count; ii++)
  {
    TPlayerURL* pOld = (TPlayerURL*)CheckBox->Items->Objects[ii];
    if (pOld)
    {
      String sPath = pOld->path;

      // delete old object
      try{ delete pOld; }
      catch(...){}

      TPlayerURL* p = InitTPlayerURL(sPath);
      p->listIndex = ii;
      CheckBox->Items->Objects[ii] = (TObject*)p;
      CheckBox->Items->Strings[ii] = GetTags(p);
    }
  }
  FCacheCount = 0;
  return true;
}
//---------------------------------------------------------------------------
//EdtTitle.Text  := MainAudioFile.Title;
//EdtArtist.Text := MainAudioFile.Artist;
//EdtAlbum.Text  := MainAudioFile.Album;
//EdtGenre.Text  := MainAudioFile.Genre;
//EdtYear.Text   := MainAudioFile.Year;
//EdtTrack.Text  := MainAudioFile.Track;
//Memo1.Clear;
//Memo1.Lines.Add(Format('Type:      %s',       [MainAudioFile.FileTypeName] ));
//Memo1.Lines.Add(Format('FileSize   %d Bytes', [MainAudioFile.FileSize]     ));
//Memo1.Lines.Add(Format('Duration   %d sec',   [MainAudioFile.Duration]     ));
//Memo1.Lines.Add(Format('Btrate     %d kBit/s',[MainAudioFile.Bitrate div 1000]));
//Memo1.Lines.Add(Format('Samplerate %d Hz',    [MainAudioFile.Samplerate]   ));
//Memo1.Lines.Add(Format('Channels:  %d',       [MainAudioFile.Channels]     ));
String __fastcall TPlaylistForm::GetTags(TPlayerURL* p)
{
  if (!p) return "(TPlayerURL object missing!)";

  TGeneralAudioFile* f = NULL;
  String sTitle, sAlbum, sArtist, sDuration;
//  int iFileSize = 0;
  try
  {
    f = new TGeneralAudioFile(p->path);
    if (f)
    {
      sTitle = f->Title;
      sAlbum = f->Album;
      sArtist = f->Artist;
      sDuration = f->Duration;
//      iFileSize = f->FileSize;
    }
  }
  __finally
  {
   if (f) delete f;
  }

  String sOut;
  if (sTitle == "" || sArtist == "" ||
     (sTitle.Length() >= 6 && sTitle.SubString(1, 6) == "Track "))
    sOut = p->path;
  else
    sOut = sTitle + " (" + sArtist + ")";

  return sOut;
}
//---------------------------------------------------------------------------
TPlayerURL* __fastcall TPlaylistForm::InitTPlayerURL(String s)
{
  TPlayerURL* p = new TPlayerURL();
  p->color = this->TextColor;
  p->bDownloaded = false;
  p->bIsUri = MainForm->IsUri(s);
  p->cacheNumber = 0; // 0 = not yet cached
  // NOTE: p->URL will eventually be changed to the path of the temporary file after
  // the file is moved to the temp area in GetNext().
  p->path = s;
  p->cachePath = s;
  p->state = cbGrayed;
  return p;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxDragOver(TObject* Sender,
          TObject* Source, int X, int Y, TDragState State, bool &Accept)
{
  // Accept the item if its from a TCheckListBox
  Accept = Source->ClassNameIs("TCheckListBox");
}
//----------------------------------------------------------------------------
/*
void __fastcall TPlaylistForm::CustomMessageHandler(TMessage &msg)
// Allow Unicode Window Caption (the Caption property is ANSI-only)
{
  if (msg.Msg == WM_SETTEXT)
  {
    if (msg.LParam != NULL)
    {
      String w = String((char*)msg.LParam);
      DefWindowProcW(this->Handle, msg.Msg, 0, (LPARAM)w.c_bstr());
    }
    else
      this->OldWinProc(msg); // Call main handler
  }
  else
    this->OldWinProc(msg); // Call main handler
}
*/
// can't get scroll notifications without subclassing the list control! (sadly)
//---------------------------------------------------------------------------
//void __fastcall TPlaylistForm::WMVListScroll(TWMScroll &Msg)
// int Msg.Msg, int Msg.Result, wchar_t* Msg.Text
//{
//  HWND hSB = Msg.ScrollBar;
//#if DEBUG_ON
//    MainForm->CWrite("\r\nVScroll: ScrollCode: \"" + String(Msg.ScrollCode) +
//     "\", Result:" + String(Msg.Result) + ", Msg:" + String(Msg.Msg) + ", Pos:" + String(Msg.Pos) + "\r\n");
//#endif
//}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::WMListDropFile(TWMDropFiles &Msg)
{
  MainForm->LoadListWithDroppedFiles(this, Msg);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::WMSetText(TWMSetText &Msg)
// Allow Unicode Window Caption (the Caption property is ANSI-only)
{
  if (Msg.Text != NULL)
  {
    String w = Msg.Text;
    DefWindowProc(this->Handle, Msg.Msg, 0, (LPARAM)w.w_str());
    Msg.Result = TRUE;
  }
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::QueueFirst(void)
{
  return QueueToIndex(0);
}

bool __fastcall TPlaylistForm::QueueToIndex(int Index)
{
  if (!Wmp || !CheckBox->Count) return false;

  try
  {
    FNextIndex = Index;

    // Can't Set URL or we stop the player! If player playing or paused... don't set URL
    // because it will stop the current song
    if (!IsPlayOrPause())
      Wmp->URL = GetNext();

  FTargetIndex = Index;
    SetTitle();

    return true;
  }
  catch(...) { return false; }
}
//---------------------------------------------------------------------------
// old version - keep until new version below tests ok over time!!!
//
//void __fastcall TPlaylistForm::NextSong(bool bForceStartPlay)
//{
//  if (!Wmp) return;
//
//  try
//  {
//    bool bWasPlaying = false;
//
//    // this player on now?
//    if (IsPlayOrPause())
//      bWasPlaying = true; // playing?
//
//    // Make sure FPlayIdx is in-bounds...
//    if (FPlayIdx >= CheckBox->Count)
//      FPlayIdx = 0;
//
//    if (FNextIndex >= CheckBox->Count)
//      FNextIndex = 0;
//
//    int oldidx = FPlayIdx;
//
//    if (FNextIndex < 0)
//      FNextIndex = FPlayIdx + 1;
//
//    String sFile = GetNext(false, true); // enable random
//
//  #if DEBUG_ON
//    MainForm->CWrite("\r\nNextSong() FPlayIdx:" + String(FPlayIdx) +
//     " Target:" + String(TargetIndex) + " File: \"" + String(sFile) +
//      "\" bForceStartPlay: " + (bForceStartPlay ? "yes" : "no") + "\r\n");
//  #endif
//
//    // Old listbox checkbox needs to be cleared...
//    ClearCheckState(oldidx);
//
//    if (FPlayIdx >= 0)
//    {
//      Wmp->URL = sFile;
//
//#if DEBUG_ON
//    MainForm->CWrite("\r\nCall SetTitle\r\n");
//#endif
//      SetTitle();
//
//      if (bWasPlaying || bForceStartPlay)
//      {
//        m_bSkipFilePrompt = true;
//
//#if DEBUG_ON
//    MainForm->CWrite("\r\nCall StartPlayer\r\n");
//#endif
//        // Start player
//        StartPlayer();
//      }
//    }
//    else if (!MainForm->ForceFade())  // no more checked items
//      StopPlayer(); // Stop player
//  }
//  catch(...)
//  {
//#if DEBUG_ON
//    MainForm->CWrite("\r\nException thrown in NextSong()\r\n");
//#endif
//  }
//}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::NextSong(bool bForceStartPlay)
{
  if (!Wmp) return;

  try
  {
    bool bWasPlaying = IsPlayOrPause() ? true : false; // playing?

    int savePlayIdx = FPlayIdx;

    FNextIndex = FTargetIndex;

    if (FNextIndex >= CheckBox->Count)
      FNextIndex = 0;

    String sFile = GetNext(false, true); // enable random

  #if DEBUG_ON
    MainForm->CWrite("\r\nNextSong() FPlayIdx:" + String(FPlayIdx) +
     " Target:" + String(TargetIndex) + " File: \"" + String(sFile) +
      "\" bForceStartPlay: " + (bForceStartPlay ? "yes" : "no") + "\r\n");
  #endif

  // Old checkbox may need to be cleared...
    ClearCheckState(savePlayIdx);

    if (!sFile.IsEmpty())
    {
      Wmp->URL = sFile;

#if DEBUG_ON
      MainForm->CWrite("\r\nCall SetTitle\r\n");
#endif
      SetTitle();

      if (bWasPlaying || bForceStartPlay)
      {
        m_bSkipFilePrompt = true;

#if DEBUG_ON
        MainForm->CWrite("\r\nCall StartPlayer\r\n");
#endif
        // Start player
        StartPlayer();
      }
    }
    else if (!MainForm->ForceFade())  // no more checked items
      StopPlayer(); // Stop player
  }
  catch(...)
  {
#if DEBUG_ON
    MainForm->CWrite("\r\nException thrown in NextSong()\r\n");
#endif
  }
}
//---------------------------------------------------------------------------
// Gets a UTF-8 string from the listbox and converts it to UTF-16
String __fastcall TPlaylistForm::GetNext(bool bNoSet, bool bEnableRandom)
// Given pointer to a Form containing a listbox, returns the first file-name
// that has its check-box grayed (queued song).
//
// Set bNoSet true to cause FPlayIdx to be unaffected, instead the next grey-checked
// item is returned in FTargetIndex.
//
// BEFORE calling this function, set FNextIndex to a >= 0 value to force
// searching to begin there. Set FNextIndex -1 to begin the search at FPlayIdx+1.
//
// NOTE: this->FPlayIdx is used to hold the list-index of the currently playing song.
// FNextIndex is the start-index of the search on entry. It's set to -1 on return.
// On return, FPlayIdx set to the next available song and FTargetIndex is set to the
// next available song (grey-checked item) that follows FPlayIdx.
{
  String sFile;

#if DEBUG_ON
  MainForm->CWrite( "\r\nTPlaylistForm::GetNext() (onenter): NextIndex=" +
      String(NextIndex) +  ", FPlayIdx=" + String(FPlayIdx) +  ", TargetIndex=" +
       String(TargetIndex) + ", Player: " + (FPlayerA ? String("A") : String("B")) + "\r\n");
#endif
  try
  {
    try
    {
      int c = CheckBox->Count;

      // If Form pointer is null or no items in listbox, return ""
      if (!c)
      {
    if (!bNoSet)
          FPlayIdx = -1;

        return "";
      }

      bool bListReset = (FNextIndex >= 0) ? true : false;

      // Random (Shuffle) play?
      bool bRandom = false;

      if (bEnableRandom)
        bRandom = ((PlayerA && MainForm->ShuffleModeA) ||
          (!PlayerA && MainForm->ShuffleModeB)) ? true : false;

      int loops;

      if (bNoSet)
        loops = 1;
      else
      {
        if (bRandom)
        {
          for (int ii = 0 ; ii < c ; ii++)
          {
            FNextIndex = ::Random(c);

            if (IsStateGrayed(CheckBox, FNextIndex))
              break;
          }
        }

        loops = 2;
    }

      for (int ii = 0 ; ii < loops ; ii++)
      {
        if (FNextIndex < 0)
          FNextIndex = FPlayIdx; // start at current song if NextInxex is -1

        if (FNextIndex < 0)
        {
#if DEBUG_ON
          MainForm->CWrite("\r\nTPlaylistForm::GetNext: RETURNING EMPTY (FPlayIdx was -1)!!!!\r\n");
#endif
          return "";
        }

        int jj;
        for (jj = 0 ; jj < c ; jj++, FNextIndex++)
        {
          if (FNextIndex >= c)
            FNextIndex = 0;

          if (IsStateGrayed(CheckBox, FNextIndex))
          {
            if (ii == 0) // first loop...
            {
              if (!bNoSet && bListReset)
                // prefetch the next file into our temporary cache directory
                MainForm->CopyFileToCache(this, FNextIndex);

              sFile = MainForm->GetURL(CheckBox, FNextIndex);
            }

            break;
      }
        }

        if (jj == c)
          FNextIndex = -1; // no Play-flags set

        if (FNextIndex >= c)
        {
          ShowMessage("GetNext() FPlayIdx Index is out-of-range!");
          FPlayIdx = -1;
          FTargetIndex = -1;
        }
        else if (!bNoSet)
        {
          if (ii == 0) // first loop...
          {
            FPlayIdx = FNextIndex;
            FNextIndex = FPlayIdx+1;
          }
          else
          {
            FTargetIndex = FNextIndex;

            // prefetch the next file into our temporary cache directory
            MainForm->CopyFileToCache(this, FTargetIndex);
          }
        }
        else
          FTargetIndex = FNextIndex;
      }

      FNextIndex = -1;
      SetTitle();
  }
    catch(...) { ShowMessage("GetNext() threw an exception..."); }
  }
  __finally
  {
#if DEBUG_ON
    MainForm->CWrite( "TPlaylistForm::GetNext() (onexit): NextIndex=" +
      String(NextIndex) +  ", FPlayIdx=" + String(FPlayIdx) +  ", TargetIndex=" +
       String(TargetIndex) + "\r\n");
    MainForm->CWrite( "TPlaylistForm::GetNext() sFile: \"" + sFile +  "\"\r\n\r\n");
#endif
  }

  // URL in Windows Media Player is a String... do not percent-encode!
  // Convert UTF-8 to UTF-16!
  return sFile;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::ClearCheckState(int idx, bool bRequeueIfRepeatMode)
{
  if (idx >= 0 && idx <= CheckBox->Count)
  {
    bool bRepeatMode = PlayerA ? MainForm->RepeatModeA : MainForm->RepeatModeB;

    if (bRepeatMode && bRequeueIfRepeatMode)
      CheckBox->State[idx] = cbGrayed;
    else
      CheckBox->State[idx] = cbUnchecked;

    SetItemState(idx);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SetCheckedState(int idx)
{
  if (idx >= 0 && idx <= CheckBox->Count)
  {
    CheckBox->State[idx] = cbChecked;
    SetItemState(idx);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SetGrayedState(int idx)
{
  if (idx >= 0 && idx <= CheckBox->Count)
  {
    CheckBox->State[idx] = cbGrayed;
    SetItemState(idx);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SetItemState(int idx)
{
  TPlayerURL* p = (TPlayerURL*)CheckBox->Items->Objects[idx];
  if (p)
    p->state = CheckBox->State[idx];
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::IsStateGrayed(TCheckListBox* clb, int idx)
{
  TPlayerURL* p = (TPlayerURL*)clb->Items->Objects[idx];
  if (p)
    return (p->state == cbGrayed) ? true : false;
  return false;
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::IsStateChecked(TCheckListBox* clb, int idx)
{
  TPlayerURL* p = (TPlayerURL*)clb->Items->Objects[idx];
  if (p)
    return (p->state == cbChecked) ? true : false;
  return false;
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::IsStateUnchecked(TCheckListBox* clb, int idx)
{
  TPlayerURL* p = (TPlayerURL*)clb->Items->Objects[idx];
  if (p)
    return (p->state == cbUnchecked) ? true : false;
  return false;
}
//---------------------------------------------------------------------------
//typedef enum WMPOpenState
//{
//  wmposUndefined = 0,
//  wmposPlaylistChanging = 1,
//  wmposPlaylistLocating = 2,
//  wmposPlaylistConnecting = 3,
//  wmposPlaylistLoading = 4,
//  wmposPlaylistOpening = 5,
//  wmposPlaylistOpenNoMedia = 6,
//  wmposPlaylistChanged = 7,
//  wmposMediaChanging = 8,
//  wmposMediaLocating = 9,
//  wmposMediaConnecting = 10,
//  wmposMediaLoading = 11,
//  wmposMediaOpening = 12,
//  wmposMediaOpen = 13,
//  wmposBeginCodecAcquisition = 14,
//  wmposEndCodecAcquisition = 15,
//  wmposBeginLicenseAcquisition = 16,
//  wmposEndLicenseAcquisition = 17,
//  wmposBeginIndividualization = 18,
//  wmposEndIndividualization = 19,
//  wmposMediaWaiting = 20,
//  wmposOpeningUnknownURL = 21
//} WMPOpenState;
//
// Select a song: 1, 7, 6
// Play a song (and it's there): 21, 13
// Play a song (and it's NOT there): 21, 6
// Play a song (and it's there but wrong kind of WAV): 21, 8, 6, 12, 8, 6
// Stop while playing: 1, 8, 6, 7, 6
void __fastcall TPlaylistForm::OpenStateChange(WMPOpenState NewState)
{
  if (!Wmp) return;

  try
  {
    if (NewState == WMPOpenState::wmposMediaOpen) // Media Open
    {
#if DEBUG_ON
      MainForm->CWrite( "\r\nOpenStateChange():wmposMediaOpen: FPlayIdx:" + String(FPlayIdx) + " Target:" + String(TargetIndex) + "\r\n");
#endif

      m_Duration = (int)Wmp->currentMedia->duration;

      if (PlayerA)
        TimeDisplay(m_Duration, 1);
      else
        TimeDisplay(m_Duration, 4);

    // Green
      if (m_bOpening && FPlayIdx >= 0 && FPlayIdx < CheckBox->Count)
      {
        TPlayerURL* p = (TPlayerURL*)CheckBox->Items->Objects[FPlayIdx];
        if (p)
          p->color = clGreen;
      }

      m_bOpening = false;
    }
    else if (NewState == WMPOpenState::wmposOpeningUnknownURL)
    {
#if DEBUG_ON
      MainForm->CWrite( "\r\nOpenStateChange():wmposOpeningUnknownURL: FPlayIdx:" + String(FPlayIdx) + " Target:" + String(TargetIndex) + "\r\n");
#endif
      m_bOpening = true;
    }
    else if (NewState == WMPOpenState::wmposMediaOpening)
    {
#if DEBUG_ON
      MainForm->CWrite( "\r\nOpenStateChange():wmposMediaOpening: FPlayIdx:" + String(FPlayIdx) + " Target:" + String(TargetIndex) + "\r\n");
#endif
      m_bOpening = true;
    }
    else if (NewState == WMPOpenState::wmposPlaylistOpenNoMedia)
    {
      // Red
#if DEBUG_ON
      MainForm->CWrite( "\r\nOpenStateChange():wmposPlaylistOpenNoMedia: FPlayIdx:" + String(FPlayIdx) + " Target:" + String(TargetIndex) + "\r\n");
#endif
      m_bOpening = false;
    }
#if DEBUG_ON
  else if (NewState == WMPOpenState::wmposMediaChanging)
    {
      MainForm->CWrite( "\r\nOpenStateChange():wmposMediaChanging: FPlayIdx:" + String(FPlayIdx) + " Target:" + String(TargetIndex) + "\r\n");
    }
    else
    {
      MainForm->CWrite( "\r\nMisc OpenStateChange: " + String(NewState) + "\r\n");
    }
#endif
  }
  catch(...) {}

  MainForm->SetCurrentPlayer(); // Set CurrentPlayer variable (used for color-coding)
}
//---------------------------------------------------------------------------
// Called from WindowsMediaPlayer1MediaError and WindowsMediaPlayer2MediaError
// hooks in Main.cpp
void __fastcall TPlaylistForm::MediaError(LPDISPATCH Item)
{
  if (m_failSafeCounter < RETRY_A)
  {
    if (FPlayIdx >= 0 && FPlayIdx < CheckBox->Count)
    {
      TPlayerURL* p = (TPlayerURL*)CheckBox->Items->Objects[FPlayIdx];
      if (p)
      {
        String sPath = p->path;

        if (MainForm->CacheEnabled)
        {
          if (p->cacheNumber > 0)
          {
            if (FileExists(p->cachePath))
      {
              sPath = p->cachePath;
    #if DEBUG_ON
              MainForm->CWrite("\r\nMediaError(" + String(FPlayIdx) + "): Retrying cache-file path: \"" +
                      String(sPath) + "\"\r\n");
    #endif
            }
            else
            {
              p->cachePath = sPath;
              p->cacheNumber = 0;
    #if DEBUG_ON
              MainForm->CWrite("\r\nMediaError(" + String(FPlayIdx) + "): Restoring original path: \"" +
                      String(sPath) + "\"\r\n");
    #endif
            }
          }
        }
        else
        {
          p->cachePath = sPath;
          p->cacheNumber = 0;
        }

        Wmp->URL = sPath;
        SetTimer(TM_START_PLAYER);
      }
    }

    m_failSafeCounter++;
  }
  else if (m_failSafeCounter < RETRY_B)
  {
  // clear the check-state for the unresponsive song
    if (FPlayIdx >= 0 && FPlayIdx < CheckBox->Count)
    {
#if DEBUG_ON
      MainForm->CWrite("\r\nMediaError(" + String(FPlayIdx) + "): Gave up on this song... running MainForm->ForceFade() or NextSong(): \"" +
                  String(Wmp->URL) + "\"\r\n");
#endif
      if (!IsStateUnchecked(CheckBox, FPlayIdx))
      {
        // clear the check-state for the unresponsive song
        CheckBox->State[FPlayIdx] = cbUnchecked;
        TPlayerURL* p = (TPlayerURL*)CheckBox->Items->Objects[FPlayIdx];
        if (p)
        {
          if (MainForm->CacheEnabled && p->cacheNumber > 0 && FileExists(p->cachePath))
            MainForm->DeleteCacheFile(this, p->cacheNumber);

          p->cachePath = p->path;
          p->cacheNumber = 0;
          p->state = cbUnchecked;
        }
      }
    }

    SetTimer(TM_NEXT_SONG); // NextSong
    m_failSafeCounter++;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::PositionTimerEvent(TObject* Sender)
// Fires every second to check the remaining play-time
{
  // Return if this player is not playing...
  if (!Wmp || !OtherWmp || Wmp->playState != WMPPlayState::wmppsPlaying)
    return;

  try
  {
    if (!MainForm->ManualFade && MainForm->FadePoint != 0)
    {
      // refer also to AutoFadeTimerEvent() in Main.cpp
      int iFadePoint = MainForm->FadePoint; // FadePoint can be 0-99 seconds before the end

      // TODO: need a solution to juggle this and AutoFadeTimerEvent() in Main.cpp
      // in the case of very short music clips!!!!!!!!!!!!!!!!!!!!!!! (below does NOT work)
      //if (iFadePoint < m_Duration)
      //{
      //  iFadePoint = m_Duration;
      //  MainForm->AutoFadeTimer->Enabled = false;
      //}

      if (OtherForm->FPlayIdx >= 0 && (m_Duration-(int)Wmp->controls->currentPosition <= iFadePoint))
      {
        // Start Other Player
        if (!OtherForm->IsPlayOrPause())
          OtherForm->StartPlayer();

        if (PlayerA)
        {
          if (MainForm->FaderTrackBar->Position != MainForm->FaderTrackBar->Max)
          {
            MainForm->bFadeRight = true; // Set fade-direction
            MainForm->AutoFadeTimer->Enabled = true; // Start a fade right
          }
        }
        else if (MainForm->FaderTrackBar->Position != MainForm->FaderTrackBar->Min)
    {
          MainForm->bFadeRight = false; // Set fade-direction
          MainForm->AutoFadeTimer->Enabled = true; // Start a fade left
        }
      }
    }
  }
  catch(...) { }

  UpdatePlayerStatus();

}
//---------------------------------------------------------------------------
// Value State Description
// 0 Undefined Windows Media Player is in an undefined state.
// 1 Stopped Playback of the current media item is stopped.
// 2 Paused Playback of the current media item is paused. When
//   a media item is paused, resuming playback begins from the same location.
// 3 Playing The current media item is playing.
// 4 ScanForward The current media item is fast forwarding.
// 5 ScanReverse The current media item is fast rewinding.
// 6 Buffering The current media item is getting additional data from the server.
// 7 Waiting Connection is established, but the server is not sending data.
//   Waiting for session to begin.
// 8 MediaEnded Media item has completed playback.
// 9 Transitioning Preparing new media item.
// 10 Ready Ready to begin playing.
// 11 Reconnecting Reconnecting to stream.
//
//typedef enum WMPPlayState
//{
//  wmppsUndefined = 0,
//  wmppsStopped = 1,
//  wmppsPaused = 2,
//  wmppsPlaying = 3,
//  wmppsScanForward = 4,
//  wmppsScanReverse = 5,
//  wmppsBuffering = 6,
//  wmppsWaiting = 7,
//  wmppsMediaEnded = 8,
//  wmppsTransitioning = 9,
//  wmppsReady = 10,
//  wmppsReconnecting = 11,
//  wmppsLast = 12
//} WMPPlayState;

// sequence when media plays then stops:
// wmppsPlaying3,wmppsMediaEnded8,wmppsTransitioning9,1wmppsStopped1
void __fastcall TPlaylistForm::PlayStateChange(WMPPlayState NewState)
{
  if (Wmp == NULL || OtherWmp == NULL || FPlayPreview) return;

#if DEBUG_ON
  MainForm->CWrite( "\r\PlayStateChange: " + String(NewState) + "\r\n");
#endif

  try
  {
    if (MainForm->SendTiming) // Send Telemetry?
    {
      // 10=ready/stop
      // 1=stopped
      // 2=paused
      // 3=playing
      if (NewState == WMPPlayState::wmppsPlaying) // playing?
      {
    // "Play" state-change is received here...
        STRUCT_A sms;

        // Populate SwiftMixStruct
        GetSongInfo(sms);

        int size = sizeof(STRUCT_A);

        SendToSwiftMix(&sms, size, MainForm->RWM_SwiftMixPlay);
      }
      else
      {
        // Send data to YahCoLoRiZe
        STRUCT_B sms;

        // Populate SwiftMixStruct
        sms.player = (PlayerA == true) ? 0 : 1;
        sms.state = (int)NewState;

        int size = sizeof(STRUCT_B);

        SendToSwiftMix(&sms, size, MainForm->RWM_SwiftMixState);
      }
    }
  }
  catch(...) {}

  try
  {
    // 10=ready/stop
    // 8=media ended
    // 1=stopped
    // 2=paused
  // 3=playing
    if (NewState == WMPPlayState::wmppsReady) // song ready to play...
    {
#if DEBUG_ON
      MainForm->CWrite( "\r\nWMPPlayState():wmppsReady " + String(PlayerA ? "A" : "B") + ": FPlayIdx=" + String(FPlayIdx) + "\r\n");
      if (TargetIndex < 0)
        MainForm->CWrite( "(Good place to prompt for more music files via timer???\r\n");
#endif
    }
    else if (NewState == WMPPlayState::wmppsPaused) // pause?
    {
#if DEBUG_ON
      MainForm->CWrite( "\r\nWMPPlayState():wmppsPaused " + String(PlayerA ? "A" : "B") + ": FPlayIdx=" + String(FPlayIdx) + "\r\n");
#endif
      if (PlayerA)
      {
        MainForm->Stop1->Checked = false;
        MainForm->Play1->Checked = false;
        MainForm->Pause1->Checked = true;
      }
      else
      {
        MainForm->Stop2->Checked = false;
        MainForm->Play2->Checked = false;
        MainForm->Pause2->Checked = true;
      }
    }
    else if (NewState == WMPPlayState::wmppsPlaying) // play?
    {
#if DEBUG_ON
      MainForm->CWrite( "\r\nWMPPlayState():wmppsPlaying " + String(PlayerA ? "A" : "B") + ": FPlayIdx=" + String(FPlayIdx) + "\r\n");
#endif
      if (FPlayIdx < 0)
    {
        // illegal to start if nothing checked
        PositionTimer->Enabled = false;

        SetTimer(TM_STOP_PLAYER);
      }
      else
      {
        // display the media info in the hint of the status-bar
        MainForm->StatusBar1->Hint = GetMediaTags();

        if (PlayerA)
        {
          MainForm->Stop1->Checked = false;
          MainForm->Play1->Checked = true;
          MainForm->Pause1->Checked = false;
        }
        else
        {
          MainForm->Stop2->Checked = false;
          MainForm->Play2->Checked = true;
          MainForm->Pause2->Checked = false;
        }

        // Start this player with other player stopped and fader in wrong position
        // Autofade to this player...
        if (!MainForm->ManualFade && !MainForm->AutoFadeTimer->Enabled)
        {
          if (PlayerA)
          {
            if (MainForm->FaderTrackBar->Position != MainForm->FaderTrackBar->Min)
            {
              MainForm->bFadeRight = false;
              MainForm->AutoFadeTimer->Enabled = true;
            }
          }
          else
          {
            if (MainForm->FaderTrackBar->Position != MainForm->FaderTrackBar->Max)
            {
              MainForm->bFadeRight = true;
              MainForm->AutoFadeTimer->Enabled = true;
            }
          }
        }

        SetCheckedState(FPlayIdx);

        m_failSafeCounter = 0; // reset failsafe counter

        // Need to update target index without affecting FPlayIdx if
        // we just started a manually queued (by single-click) song.
        // (So that when clicking the list-box we see the next
        // grey-checked song in the list queued...)
        FNextIndex = FPlayIdx;
        GetNext(true, true); // allow random...

        m_bSkipFilePrompt = false;

        PositionTimer->Enabled = true;
        SetTitle();
      }

      MainForm->SetCurrentPlayer();
    }
    else if (NewState == WMPPlayState::wmppsStopped) // stop?
  {
#if DEBUG_ON
      MainForm->CWrite( "\r\nWMPPlayState():wmppsStopped " + String(PlayerA ? "A" : "B") + ": FPlayIdx=" + String(FPlayIdx) + "\r\n");
#endif
      PositionTimer->Enabled = false;
      UpdatePlayerStatus();
      ClearCheckState(FPlayIdx);

      if (PlayerA)
      {
        MainForm->Stop1->Checked = true;
        MainForm->Play1->Checked = false;
        MainForm->Pause1->Checked = false;
      }
      else
      {
        MainForm->Stop2->Checked = true;
        MainForm->Play2->Checked = false;
        MainForm->Pause2->Checked = false;
      }

      QueueToIndex(FTargetIndex);
      SetTitle();
      MainForm->SetCurrentPlayer();
    }
    else if (NewState == WMPPlayState::wmppsMediaEnded) // Song ended?
    {
#if DEBUG_ON
      MainForm->CWrite( "\r\nWMPPlayState():wmppsMediaEnded " + String(PlayerA ? "A" : "B") + ": FPlayIdx=" + String(FPlayIdx) + "\r\n");
#endif
      // if we were fading when the song ended, stop the fade-timer
      // and force it the rest of the way...
      //
    // If the top player (A) media ended - if we were fading left (toward A)
      // force the rest of the way left. If we were fading to the lower
      // player (B) - just leave it alone.
      //
      // If the bottom player (B) media ended - if we were fading right (toward B)
      // force the rest of the way right. If we were fading to the upper
      // player (A) - just leave it alone.
      if (!IsPlayOrPause())
        ClearCheckState(FPlayIdx);

      if (!MainForm->ManualFade && !MainForm->AutoFadeTimer->Enabled)
      {
//        if (MainForm->AutoFadeTimer->Enabled)
//        {
//          // do a "hurry-up" forced-fade...
//          MainForm->AutoFadeTimer->Enabled = false;
//          if (PlayerA && !MainForm->bFadeRight)
//          {
//            while (MainForm->FaderTrackBar->Position > MainForm->FaderTrackBar->Min)
//            {
//              MainForm->FaderTrackBar->Position--;
//              Application->ProcessMessages();
//              Sleep(20);
//            }
//#if DEBUG_ON
//            MainForm->CWrite( "\r\Forced fade left\r\n");
//#endif
//          }
//          else if (!PlayerA && MainForm->bFadeRight)
//          {
//            while (MainForm->FaderTrackBar->Position < MainForm->FaderTrackBar->Max)
//            {
//              MainForm->FaderTrackBar->Position++;
//              Application->ProcessMessages();
//              Sleep(20);
//            }
//#if DEBUG_ON
//            MainForm->CWrite( "\r\Forced fade right\r\n");
//#endif
//          }
//        }
        // start next player if media ended and fade-timer is not running
//        else
        {
          QueueToIndex(FTargetIndex);
          SetTimer(TM_FADE); // NextSong
        }

      }
    }
#if DEBUG_ON
    else if (NewState == WMPPlayState::wmppsTransitioning)
      MainForm->CWrite( "\r\nWMPPlayState():wmppsTransitioning " + String(PlayerA ? "A" : "B") + ": FPlayIdx=" + String(FPlayIdx) + "\r\n");
    else
      MainForm->CWrite( "\r\nMisc. WMPPlayState(): " + String(NewState) + "\r\n");
#endif

  }
  catch(...) {}

  if (NewState != WMPPlayState::wmppsTransitioning) // not transitioning? save state...
    m_PrevState = NewState;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::GetSongInfo(void)
{
  GetSongInfo(MediaInfo);
}

// This info is in utf-8 strings... the length fields are in bytes
void __fastcall TPlaylistForm::GetSongInfo(STRUCT_A &sms)
{
  sms.player = (PlayerA == true) ? 0 : 1;

  sms.duration = (int)Wmp->currentMedia->duration;

  AnsiString uPath = MainForm->WideToUtf8Ansi(Wmp->URL);
  strncpy(sms.path, uPath.c_str(), SONG_PATH_SIZE-1);
  sms.len_path = uPath.Length();
  if (sms.len_path > SONG_PATH_SIZE-1)
    sms.len_path = SONG_PATH_SIZE-1;


  AnsiString uTitle = MainForm->WideToUtf8Ansi(Wmp->currentMedia->name);
  strncpy(sms.name, uTitle.c_str(), SONG_NAME_SIZE-1);
  sms.len_name = uTitle.Length();
  if (sms.len_name > SONG_NAME_SIZE-1)
    sms.len_name = SONG_NAME_SIZE-1;

  // Note the "L" to make string-constants wide!!!!!!
  AnsiString uArtist = MainForm->WideToUtf8Ansi(Wmp->currentMedia->getItemInfo(L"WM/AlbumArtist"));
  if (uArtist == "" || uArtist.LowerCase() == "various artists" || uArtist.LowerCase() == "various")
    uArtist = MainForm->WideToUtf8Ansi(Wmp->currentMedia->getItemInfo(L"Author"));
  strncpy(sms.artist, uArtist.c_str(), SONG_NAME_SIZE-1);
  sms.len_artist = uArtist.Length();
  if (sms.len_artist > SONG_NAME_SIZE-1)
    sms.len_artist = SONG_NAME_SIZE-1;

  AnsiString uAlbum = MainForm->WideToUtf8Ansi(Wmp->currentMedia->getItemInfo(L"WM/AlbumTitle"));
  strncpy(sms.album, uAlbum.c_str(), SONG_NAME_SIZE-1);
  sms.len_album = uAlbum.Length();
  if (sms.len_album > SONG_NAME_SIZE-1)
    sms.len_album = SONG_NAME_SIZE-1;
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::SendToSwiftMix(void* sms, int size, int msg)
{
  try
  {
    bool RetVal = false;

    // Populate CopyDataStruct
    COPYDATASTRUCT cds;
    cds.dwData = msg; // Unique windows message we registered
    cds.cbData = size; // size of data
    cds.lpData = sms; // pointer to data

    // Find the target application window (if running)
    HWND hnd = FindWindow(YC_CLASS, YC_WINDOW);

    // Send the message to target
    if (hnd != 0 && msg != 0)
    {
      SendMessage(hnd, WM_COPYDATA, (WPARAM)Application->Handle, (LPARAM)&cds);
      RetVal = true;
    }

    return RetVal;
  }
  catch (...)
  {
    return false;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SetTimer(int mode, int time)
{
  GeneralPurposeTimer->Enabled = false;
  m_TimerMode = mode;
  GeneralPurposeTimer->Interval = time;
  GeneralPurposeTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::PositionChange(double oldPosition, double newPosition)
{
  // User dragged the position-bar...
  UpdatePlayerStatus();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::UpdatePlayerStatus(void)
{
  if (!Wmp)
    return;

  try
  {
    int CurrentPos = (int)Wmp->controls->currentPosition+1;

    if (PlayerA)
      TimeDisplay(CurrentPos, 0);
    else
      TimeDisplay(CurrentPos, 3);

    if (MainForm->SendTiming) // Send Telemetry?
    {
    typedef struct
      {
        __int32 player;
        __int32 duration;
        __int32 current;
        __int32 color;
        __int32 redtime;
        __int32 yellowtime;
      } SWIFTMIX_STRUCT;

      // Send data to YahCoLoRiZe
      int size = sizeof(SWIFTMIX_STRUCT);

      // Populate SwiftMixStruct
      SWIFTMIX_STRUCT sms;
      sms.player = (PlayerA == true) ? 0 : 1;
      sms.duration = m_Duration;
      sms.current = CurrentPos;

//!!!!!!!!!!!!!! not yet implimented
      sms.color = 0;
      sms.redtime = MainForm->FadePoint;
      sms.yellowtime = 0;

      SendToSwiftMix(&sms, size, MainForm->RWM_SwiftMixTime);
    }
  }
  catch(...)
  {
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::TimeDisplay(int t, int item)
{
  int hours = t / (60 * 60);
  int minutes = t / 60;
  int seconds = t % 60;

  MainForm->StatusBar1->Panels->Items[item]->Text =
      Format("%2.2d:%02.2d:%2.2d", ARRAYOFCONST((hours, minutes, seconds)));
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SetTitle(void)
{
//  if (FEditMode)
//  {
//    this->Caption = String(STR[2]);
//    return;
//  }

  try
  {
    String S1, S2;

    S2 = "(nothing queued)";

    if (FEditMode){
      if (PlayerA)
        S1 = "EditA ";
      else
        S1 = "EditB ";
    }
    else{
      if (PlayerA)
        S1 = "PlayerA ";
      else
        S1 = "PlayerB ";
    }

    // only flash if playing and not-active
    // assume no-flash
    FlashTimer->Enabled = false;
    int SelectIdx = -1;

    if (CheckBox->Count)
    {
      if (Active) // Window is focused?
      {
        if (FPlayIdx >= 0 && FPlayIdx < CheckBox->Count)
        {
          if (CheckBox->State[FPlayIdx] == cbChecked) // playing?
          {
            if (FTargetIndex >= 0 && FTargetIndex < CheckBox->Count)
            {
              S1 += "(Q) ";
              S2 = MainForm->GetURL(CheckBox, FTargetIndex);
              SelectIdx = FTargetIndex;
            }
          }
          else // not playing...
          {
            S1 += "(Q) ";
            S2 = MainForm->GetURL(CheckBox, FPlayIdx);
            SelectIdx = FPlayIdx;
          }
        }
      }
      else // Not focused...
      {
        if (FPlayIdx >= 0)
        {
          if (CheckBox->State[FPlayIdx] == cbChecked)
          {
            S1 += "(P) ";
            FlashTimer->Enabled = true;
          }
          else // not focused and player is idle...
            S1 += "(Q) ";

          if (FPlayIdx >= 0 && FPlayIdx < CheckBox->Count)
          {
            S2 = MainForm->GetURL(CheckBox, FPlayIdx);
            SelectIdx = FPlayIdx;
          }
        }
      }
    }

    // Set listbox selection to match title
    if (CheckBox->ItemIndex != SelectIdx)
      CheckBox->ItemIndex = SelectIdx;

    // Buttons and icon
    int Misc = 3*GetSystemMetrics(SM_CXSMSIZE);

    #define TITLE_PIXELS_PER_CHAR 8
    // Available # chars for song title is the
    // total form width - icon widths - S1 width - "..."
    int W = (Width-Misc)/TITLE_PIXELS_PER_CHAR - S1.Length() - 3;

    if (S2.Length() > W)
    {
      S1 += L"...";
      int len = S2.Length();
      this->Caption = S1 + S2.SubString(len-W, len-(len-W)+1);
    }
    else
      this->Caption = S1 + S2;
  }
  catch(...)
  {
    ShowMessage("SetTitle() threw an exception");
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::StartPlayPreview(void)
{
  // press and hold control key to play-preview songs in the list
  if (FEditMode || !CheckBox->Focused() || !(GetKeyState(VK_CONTROL)&0x8000)){
    return;
  }

  // don't start play if either player is already playing normally...
  if (!FPlayPreview && (IsPlayOrPause() || OtherForm->IsPlayOrPause())){
    return;
  }

  try{
    int Index = CheckBox->ItemAtPos(ScreenToClient(Mouse->CursorPos), true);

    // check for index change of song under mouse pointer
    if (Index != FOldMouseItemIndex){
      if (Index >= 0 && Index < CheckBox->Count){
        String S = MainForm->GetURL(CheckBox, Index);

//        if (S == Wmp->URL){
//          S = "";
//          if (MediaInfo.artist[0] != NULLCHAR)
//            S += "Artist: " + String(MediaInfo.artist) + String(LF);
//          if (MediaInfo.album[0] != NULLCHAR)
//            S += "Album: " + String(MediaInfo.album) + String(LF);
//          if (MediaInfo.name[0] != NULLCHAR)
//            S += "Song: " + String(MediaInfo.name);
//        }
//
//        if (!S.IsEmpty() && S != CheckBox->Hint)
//          CheckBox->Hint = S;

        // Handle "play preview" feature when Ctrl key is held down...
        if (!S.IsEmpty()){
          FPlayPreview = true;
          CheckBox->ItemIndex = -1;
          Application->ProcessMessages();
          Wmp->URL = S;
          Wmp->controls->set_currentPosition(PLAY_PREVIEW_START_TIME);
          if (PlayerA){
            MainForm->FaderTrackBar->Position = MainForm->FaderTrackBar->Min;
          }
          else{
            MainForm->FaderTrackBar->Position = MainForm->FaderTrackBar->Max;
          }
          StartPlayer();
//          SetTimer(TM_STOP_PLAY_PREVIEW, 10000);
        }
      }
      else{
        SetTimer(TM_STOP_PLAY_PREVIEW, 50);
      }

      FOldMouseItemIndex = Index;
    }
  }
  catch(...){};
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::StopPlayPreview(void)
{
  if (FPlayPreview){
    StopPlayer();
    FPlayPreview = false;
    CheckBox->ItemIndex = FTargetIndex;
    Application->ProcessMessages();
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::StartPlayer(void)
{
  Wmp->settings->mute = true;
  Wmp->controls->play();
  Wmp->settings->mute = false;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::StopPlayer(void)
{
  Wmp->settings->mute = true;
  Wmp->controls->stop();
  Wmp->settings->mute = false;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::ClearAndStop(void)
{
  // Stop and clear list...

  if (Wmp)
  {
    StopPlayPreview();
    StopPlayer();
    Wmp->URL = "";
  }

  FlashTimer->Enabled = false;
  GeneralPurposeTimer->Enabled = false;
  PositionTimer->Enabled = false;
  MainForm->AutoFadeTimer->Enabled = false;

  // Park the trackbar (seemed like a good idea - but on second thought
  // it's probably better to just freeze a fade-in-progress and let the user
  // handle the remaining fade... to avoid a sudden volume-change...)
//  if (PlayerA)
//    MainForm->TrackBar1->Position = 100;
//  else
//    MainForm->TrackBar1->Position = 0;

  String S = PlayerA ? "PlayerA " : "PlayerB ";
  this->Caption =  S + "(nothing queued)";

  ClearListItems();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::ClearListItems(void)
{
  while (MainForm->DeleteCacheFile(this) == true); // delete all cache files for this player

  int numItems = CheckBox->Count;
  for (int ii = 0; ii < numItems; ii++)
  {
    TPlayerURL* p = (TPlayerURL*)CheckBox->Items->Objects[ii];
    if (p) delete p;
  }

  CheckBox->Clear();

  FPlayIdx = -1;
  FTargetIndex = -1;
  FNextIndex = -1;
  FTempIdx = -1;
  FCacheCount = 0;

  FTextColor = clBlack;
  Color = TColor(0xF5CFB8);

  m_bInhibitFlash = false;
  m_bDoubleClick = false;
  m_bCheckClick = false;
  m_Duration = 0;
  m_PrevState = 0;
  m_failSafeCounter = 0;
  m_TimerMode = TM_NULL;
  m_bSkipFilePrompt = false;
  m_bOpening = false;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::EditModeClick(TObject* Sender)
// PopupMenu1 F3
{
  StopPlayPreview();

  // Enter Edit Mode
  FEditMode = true;

  FlashTimer->Enabled = false;
  GeneralPurposeTimer->Enabled = false;
  m_bCheckClick = false;
  m_bDoubleClick = false;

  CheckBox->PopupMenu = PopupMenu2;

  int saveSelected = CheckBox->ItemIndex;
  CheckBox->MultiSelect = true;
  CheckBox->Selected[saveSelected] = true;

  SetTitle();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::ExitEditModeClick(TObject* Sender)
// PopupMenu2 F3
{
  // Exit Edit Mode
  CheckBox->PopupMenu = PopupMenu1;
  CheckBox->MultiSelect = false;

  FEditMode = false;

  SetTitle(); // Start flashing again, etc.
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::ClearListClick(TObject* Sender)
// PopupMenu1 F4
{
  // Clear Playlist And Stop
  ClearAndStop();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckAllItems(void)
{
  for (int ii = 0 ; ii < CheckBox->Count ; ii++)
    if (ii != FPlayIdx && ii != FTargetIndex)
      SetGrayedState(ii);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckAllClick(TObject* Sender)
// PopupMenu1 F5
{
  CheckAllItems();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::UncheckAllClick(TObject* Sender)
// PopupMenu1 F6
{
  for (int ii = 0 ; ii < CheckBox->Count ; ii++)
    if (ii != FPlayIdx && ii != FTargetIndex)
      ClearCheckState(ii, false);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::DeleteSelected1Click(TObject* Sender)
// PopupMenu1 F8
{
  if (CheckBox->Count)
  {
    DeleteListItem(CheckBox->ItemIndex);

    // nothing playing?
    if (FPlayIdx < 0)
      QueueFirst();

    SetTitle();
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::DeleteSelectedClick(TObject* Sender)
// PopupMenu2 F5
{
  // Delete Selected Items
  if (CheckBox->SelCount)
  {
    // Remove from this list (must remove backward)
    for (int ii = CheckBox->Count-1 ; ii >= 0  ; ii--)
      if (CheckBox->Selected[ii])
    DeleteListItem(ii);

    // Exit edit-mode
    if (FEditMode)
      ExitEditModeClick(NULL);
  }
  else
    ShowMessage(STR[0]);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::MenuDeleteEvenIndicesClick(TObject *Sender)
{
  // Return if no items or a song is playing
  if (Wmp == NULL || CheckBox->Items->Count == 0 || IsPlayOrPause())
  {
    ShowMessage(STR[3]);
    return;
  }

  try
  {
    for (int ii = 1; ii < CheckBox->Items->Count; ii++)
      DeleteListItem(ii);
    QueueFirst();
  }
  catch(...) {}
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::MenuDeleteOddIndiciesClick(TObject *Sender)
{
 // Return if no items or a song is playing
  if (Wmp == NULL || CheckBox->Items->Count == 0 || IsPlayOrPause())
  {
    ShowMessage(STR[3]);
    return;
  }

  try
  {
    for (int ii = 0; ii < CheckBox->Items->Count; ii++)
      DeleteListItem(ii);
    QueueFirst();
  }
  catch(...) {}
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::MenuFixOrderofTrailingNumbersClick(TObject *Sender)
{
  // Return if no items or a song is playing
  if (Wmp == NULL || CheckBox->Count == 0 || Wmp->playState == WMPPlayState::wmppsPlaying)
  {
    ShowMessage(STR[3]);
    return;
  }

  int ct = CheckBox->Count;
  if (ct < 2)
    return;
  TStringList* sl = NULL;
  TStringList* slTemp = NULL;
  try
  {
    sl = new TStringList(); // used to build a new song-list
    slTemp = new TStringList(); // holds interim songs with same base name

    // iterate through songs in list, grouping songs with the same
  // filename except for trailing digits.
    for (int ii = 0; ii < ct; ii++)
    {
      TPlayerURL* p = (TPlayerURL*)CheckBox->Items->Objects[ii];
      if (!p) continue; // item already processed...
      CheckBox->Items->Objects[ii] = NULL; // mark it as processed
      String sNamePart1, sExt1; // returned by reference
      int iDigits = GetTrailingDigits(p->path, sNamePart1, sExt1);
      if (iDigits < 0)
      {
        p->listIndex = sl->Count; // return this to normal-use!
        sl->AddObject(CheckBox->Items->Strings[ii], (TObject*)p);
        continue;
      }
      // add song at ii
      if (p->cacheNumber > 0)
        MainForm->DeleteCacheFile(this, p->cacheNumber);
      p->listIndex = iDigits; // use this field to hold trailing number
      p->cachePath = p->path;
      p->state = cbGrayed;
      slTemp->AddObject(CheckBox->Items->Strings[ii], (TObject*)p);
      for (int jj = ii+1; jj < ct; jj++)
      {
        p = (TPlayerURL*)CheckBox->Items->Objects[jj];
        if (!p) continue; // item already processed...
        String sNamePart2, sExt2; // get these by reference below...
        iDigits = GetTrailingDigits(p->path, sNamePart2, sExt2);
        if (iDigits < 0 || sNamePart1 != sNamePart2 || sExt1 != sExt2)
          continue;
        // add song at jj
        if (p->cacheNumber > 0)
          MainForm->DeleteCacheFile(this, p->cacheNumber);
        p->listIndex = iDigits; // use this field to hold trailing number
        p->cachePath = p->path;
        p->state = cbGrayed;
        slTemp->AddObject(CheckBox->Items->Strings[jj], (TObject*)p);
        CheckBox->Items->Objects[jj] = NULL; // mark it as processed
      }

      // sort items and add to to sl
      if (slTemp->Count > 0)
      {
        while (slTemp->Count > 0)
        {
          int idx = IndexOfSmallestNumber(slTemp);
          if (idx >= 0)
          {
            p = (TPlayerURL*)slTemp->Objects[idx];
            if (p)
            {
              p->listIndex = sl->Count; // return this to normal-use!
              sl->AddObject(slTemp->Strings[idx], (TObject*)p);
            }
            slTemp->Delete(idx);
          }
          else
            break; // error... shouldn't happen!
        }
        slTemp->Clear();
      }
    }

    // now clear the main songlist and move new songs to it from sl
    CheckBox->Clear();
    for (int ii = 0; ii < sl->Count; ii++)
      AddListItem(sl->Strings[ii], (TPlayerURL*)sl->Objects[ii]);
  this->QueueFirst();
  }
  __finally
  {
    if (slTemp) delete slTemp;
    if (sl) delete sl;
  }
}
//---------------------------------------------------------------------------
int __fastcall TPlaylistForm::IndexOfSmallestNumber(TStringList* sl)
{
  int iSmallest = INT_MAX;
  int idx = -1;
  for (int ii=0; ii<sl->Count; ii++)
  {
    TPlayerURL* p = (TPlayerURL*)sl->Objects[ii];
    if (p && p->listIndex < iSmallest)
    {
      iSmallest = p->listIndex;
      idx = ii;
    }
  }
  return idx;
}
//---------------------------------------------------------------------------
// returns last file-extension in sExt
// returns int for trailing digits in the file name part before the first "."
// returns -1 if no digits
// sNamePart has the name part without the digits
// NOTE: if the file's name has multiple extensions, the in-between ones will
// NOT be returned, so save the original full file-path before calling!
int __fastcall TPlaylistForm::GetTrailingDigits(String s, String &sNamePart, String &sExt)
{
  String sDigits;
  int idx = s.LastDelimiter('.');
  if (idx > 1)
  {
    sExt = s.SubString(idx, s.Length()-idx+1); // return trailing file-extension by reference
    s = s.SubString(1, idx-1);
  }
  else
    sExt = "";
  int len = s.Length();
  int jj = len;
  for (; jj>0; jj--) // trim trailing spaces
    if (s[jj] != ' ')
      break;
  for (; jj>0; jj--)
  {
    Char c = s[jj];
    if (!isdigit(c))
      break;
    sDigits.Insert(c, 1);
  }
  if (jj > 0)
    sNamePart = s.SubString(1, jj);
  else
    sNamePart = "";
  return (sDigits.Length() > 0) ? sDigits.ToIntDef(-1) : -1;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::RemoveDuplicates1Click(TObject* Sender)
{
  // Return if no items or a song is playing
  if (Wmp == NULL || CheckBox->Count == 0 || Wmp->playState == WMPPlayState::wmppsPlaying)
  {
  ShowMessage(STR[3]);
    return;
  }

  for (int ii = 0; ii < CheckBox->Count; ii++)
  {
    TPlayerURL* p = (TPlayerURL*)CheckBox->Items->Objects[ii];
    if (!p) continue;
    for (int jj = CheckBox->Count-1; jj > ii; jj--)
    {
      TPlayerURL* p2 = (TPlayerURL*)CheckBox->Items->Objects[jj];
      if (!p2) continue;
      if (p->path == p2->path)
        DeleteListItem(jj);
    }
  }

// This worked and was fast but won't handle TextColor as an TObject*
/*
  TStringList* sl = new TStringList();
  sl->Sorted = true;
  sl->Duplicates = dupIgnore;

  sl->AddStrings(CheckBox->Items);

  CheckBox->Items->Clear(); // NOTE: this won't work now - need ClearListItems()
  CheckBox->Items->AddStrings(sl);

  delete sl;
*/

  FPlayIdx = -1;
  FTargetIndex = -1;

  // Probably don't want to reenable played items
  // CheckAllItems();

  this->QueueFirst();
}
//---------------------------------------------------------------------------
// randomize the songs that have not already been played
// (NOTE: quirk I've noticed - songs with a funny character seem to end up
// in the same vacinity and at the top of the list??? I don't think Move
// is working for those song-titles.)
void __fastcall TPlaylistForm::RandomizeList1Click(TObject* Sender)
{
  // Return if no items or a song is playing
  if (CheckBox->Count == 0 || Wmp->playState == WMPPlayState::wmppsPlaying)
  {
    ShowMessage(STR[3]);
    return;
  }

  TList* la = NULL;
  TList* lb = NULL;
  TList* lc = NULL;
  CheckBox->Enabled = false;

  try
  {
    // first create a list of the playlist indexes of all unplayed songs
    la = new TList();
    lb = new TList();
    lc = new TList();

    if (!la || !lb || !lc)
    return;

    // fresh random # generator seed
    ::Randomize();

    int Count = CheckBox->Count;

    // add unplayed songs to list A
    for(int ii = 0; ii < Count; ii++)
      if (IsStateGrayed(CheckBox, ii))
        la->Add((void*)ii);

    // save unplayed list in c
    lc->Assign(la);

    // select random list A indicies and add them to list B
    // and removing from list A
    Count = la->Count;
    for(int ii = 0; ii < Count; ii++)
    {
      int idx = ::Random(la->Count);
      lb->Add(la->Items[idx]);
      la->Delete(idx);
    }

    if (lc->Count != lb->Count)
    {
      ShowMessage("Mismatch in counters: TPlaylistForm::RandomizeList1Click(): ");
      return;
    }

    Count = lc->Count;
    pProgress->Init(Count);

    // now randomize only the unplayed songs
    for(int ii = 0; ii < Count; ii++)
    {
      int dstIdx = (int)lb->Items[ii];
      int srcIdx = (int)lc->Items[ii];
      CheckBox->Items->Move(srcIdx, dstIdx);

      if (pProgress->Move(ii))
        return;
    }

    pProgress->UnInit();

    FPlayIdx = -1;
    FTargetIndex = -1;

    // Probably don't want to reenable played items
    //CheckAllItems();

    this->QueueFirst();
  }
  __finally
  {
    if (la) delete la;
    if (lb) delete lb;
    if (lc) delete lc;
    CheckBox->Enabled = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SelectAllItemsClick(TObject *Sender)
{
  int Count = CheckBox->Count;
  pProgress->Init(Count);
  CheckBox->Enabled = false;

  try
  {
    for (int ii = 0; ii < Count; ii++)
    {
      CheckBox->Selected[ii] = true;
      if (pProgress->Move(ii))
        return;
    }
  }
  __finally
  {
    pProgress->UnInit();
    CheckBox->Enabled = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CopySelectedClick(TObject* Sender)
// Copies data as UTF-16 format
{
  if (CheckBox->SelCount == 0)
  {
    ShowMessage(String(STR[0]));
    return;
  }

  if (!OpenClipboard(MainForm->Handle))
    return;

  HGLOBAL hMem = NULL;;
  TStringList* sl = NULL;

  try
  {
    EmptyClipboard();

    sl = new TStringList();

    if (sl != NULL)
    {
      for (int ii = 0 ; ii < CheckBox->Count ; ii++)
        if (CheckBox->Selected[ii])
          sl->Add(MainForm->GetURL(CheckBox, ii));

      String ws = sl->Text;

      int len = ws.Length() * sizeof(WideChar);

      int termlen = 4; // Allow space for 4 bytes of 0's to terminate

      hMem = GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE, len+termlen);

      if (hMem != NULL)
      {
        // Move string into global-memory
        char* lp = (char *)GlobalLock(hMem);

        CopyMemory(lp, ws.w_str(), len);

        char term[] = "\0\0\0\0"; // Terminator

        CopyMemory(lp+len, &term, termlen);

    // Put the memory-handle on the clipboard
        // DO NOT FREE this memory!
        SetClipboardData(CF_UNICODETEXT, hMem);
      }
    }
  }
  __finally
  {
    if (hMem != NULL) GlobalUnlock(hMem);
    try { if (sl != NULL) delete sl; } catch(...) {}
  }

  CloseClipboard();
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::GetIsOpenDlg(void)
{
  return pOFMSDlg != NULL ? true : false;
}
//---------------------------------------------------------------------------
TOFMSDlgForm* __fastcall TPlaylistForm::CreateFileDialog(void)
{
  DestroyFileDialog(); // destroy the old one...
  Application->CreateForm(__classid(TOFMSDlgForm), &pOFMSDlg);
  return pOFMSDlg;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::DestroyFileDialog(void)
{
  if (MainForm->ReleaseForm((TForm*)pOFMSDlg))
    pOFMSDlg = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::DestroyImportDialog(void)
{
  if (MainForm->ReleaseForm((TForm*)pImportDlg))
    pImportDlg = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::DestroyProgressForm(void)
{
  if (MainForm->ReleaseForm((TForm*)pProgress))
    pProgress = NULL;
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::GetIsImportDlg(void)
{
  return pImportDlg != NULL ? true : false;
}
//---------------------------------------------------------------------------
TImportForm* __fastcall TPlaylistForm::CreateImportDialog(void)
{
  DestroyImportDialog(); // destroy the old one...
  Application->CreateForm(__classid(TImportForm), &pImportDlg);
  return pImportDlg;
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::GetIsExportDlg(void)
{
  return pExportDlg != NULL ? true : false;
}
//---------------------------------------------------------------------------
TExportForm* __fastcall TPlaylistForm::CreateExportDialog(void)
{
  DestroyExportDialog(); // destroy the old one...
  Application->CreateForm(__classid(TExportForm), &pExportDlg);
  return pExportDlg;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::DestroyExportDialog(void)
{
  if (MainForm->ReleaseForm((TForm*)pExportDlg))
    pExportDlg = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CopyLinkToClipboardClick(TObject *Sender)
{
  // call same function used to copy selected item links in EditMode
  CopySelectedClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SearchandUncheckClick(TObject *Sender)
{
  // get search-text
  TSearchUncheckForm* f = NULL;
  Application->CreateForm(__classid(TSearchUncheckForm), &f);

  if (f)
  {
    try
    {
      f->SearchText = "";

      f->ShowModal();

      if (!f->SearchText.IsEmpty())
      {
      for (int ii = 0; ii < CheckBox->Count; ii++)
      {
        String lcSearchText = f->SearchText.LowerCase();
        if (IsStateGrayed(CheckBox, ii))
        {
        TPlayerURL* p = (TPlayerURL*)CheckBox->Items->Objects[ii];
        if (!p) continue;
        String s = p->path;
        s = s.LowerCase();
        if (s.Pos(lcSearchText) > 0)
        {
          ClearCheckState(ii, false);

          if (FTargetIndex == ii)
          {
          FNextIndex = ii; // start search here
          GetNext();
          }
        }
        }
      }
      }
    }
    __finally
    {
      MainForm->ReleaseForm((TForm*)f);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CopyTagsToClipboardClick(TObject* Sender)
{
  if (FTempIdx < 0) return;

  try
  {
    TPlayerURL* p = (TPlayerURL*)CheckBox->Items->Objects[FTempIdx];
    if (!p || !FileExists(p->path)) return;
    TFormTags* f = new TFormTags(this); // form will delete itself on close
    if (!f) return;
    f->ShowTags(p);
  }
  catch(...) {}
//  String s = GetMediaTags();
//  if (!s.IsEmpty())
//  {
//    Clipboard()->AsText = s;
//    s += "Copied song-tags:\n";
//    ShowMessage(s);
//  }
}
//---------------------------------------------------------------------------
String __fastcall TPlaylistForm::GetMediaTags(void)
{
  String sTags;

  try
  {
    sTags = Wmp->currentMedia->getItemInfo(L"WM/AlbumArtist");

    if (sTags == "" || sTags.LowerCase() == "various artists" ||
                                          sTags.LowerCase() == "various")
      sTags = Wmp->currentMedia->getItemInfo(L"Author");

    if (!sTags.IsEmpty())
      sTags += String(", ");

    sTags += Wmp->currentMedia->getItemInfo(L"WM/AlbumTitle");

    if (!sTags.IsEmpty())
      sTags += String(", ");

    sTags += Wmp->currentMedia->name;
  }
  catch(...) {}

  return sTags;
}
//---------------------------------------------------------------------------
//void __fastcall TPlaylistForm::WMMove(TWMMove &Msg)
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
// Property getter
//---------------------------------------------------------------------------
int __fastcall TPlaylistForm::GetCount(void)
{
  return CheckBox->Count;
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::IsPlayOrPause(void)
{
  return Wmp->playState == WMPPlayState::wmppsPlaying ||
                Wmp->playState == WMPPlayState::wmppsPaused;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if (FPlayPreview && !Shift.Contains(ssCtrl)){
    StopPlayPreview();
  }

  if (FKeySpaceDisable && Key == VK_SPACE){
    FKeySpaceDisable = false;
    Key = 0;
  }
}
//---------------------------------------------------------------------------
// need this to stop selected item check-state from toggelling when you press
// (and hold) the spacebar to check/uncheck the FPlayPreview item under the mouse!
void __fastcall TPlaylistForm::CheckBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if (Key == VK_SPACE)
    Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormKeyDown(TObject* Sender, WORD &Key, TShiftState Shift)
{
  if (FEditMode){
    if (Key == VK_ESCAPE){
      ExitEditModeClick(NULL);
      Key = 0;
      return;
    }
  }
  else{
    if (FPlayPreview){
      if (!FKeySpaceDisable && Key == VK_SPACE){
        if (FOldMouseItemIndex >= 0 && FOldMouseItemIndex < CheckBox->Count){
          if (IsStateGrayed(CheckBox, FOldMouseItemIndex)){
            ClearCheckState(FOldMouseItemIndex, false);
          }
          else{
            SetGrayedState(FOldMouseItemIndex);
          }
        }
        Key = 0;
        FKeySpaceDisable = true; // key-repeat if spacebar held down is a problem!
        return;
      }
    }
    else{
      // ssShift, ssAlt, ssCtrl, ssLeft, ssRight, ssMiddle, ssDouble,
      // ssTouch, ssPen, ssCommand, ssHorizontal
      if (Shift.Contains(ssCtrl)){
        FOldMouseItemIndex = -1;
        StartPlayPreview();
        return;
      }
    }
  }

  if (Key == VK_UP || Key == VK_DOWN || Key == VK_LEFT || Key == VK_RIGHT)
  {
    // enter edit mode when keys pressed to scroll, then time out
    if (!FEditMode)
      EditModeClick(NULL);
    SetTimer(TM_SCROLL_KEY_PRESSED, TIME_2000);
    return;
  }

  if (Key == VK_DELETE)
  {
    Key = 0;

    if (FEditMode)
      DeleteSelectedClick(NULL);
    else if (CheckBox->Count)
    {
      if (!Shift.Contains(ssShift))
        DeleteListItem(CheckBox->ItemIndex);
      else // delete all unchecked songs if the shift key is held down
      {
        for (int ii = CheckBox->Count-1 ; ii >= 0  ; ii--)
        {
          if (IsStateUnchecked(CheckBox, ii))
          {
            if (FTargetIndex >= ii)
              FTargetIndex--;
            if (FPlayIdx >= ii)
              FPlayIdx--;
            if (NextIndex >= ii)
              FNextIndex--;

            DeleteListItem(ii);
          }
        }
      }

    // nothing playing?
      if (FPlayIdx < 0)
        QueueFirst();

      SetTitle();
    }
  }
}
//---------------------------------------------------------------------------
/*
void __fastcall TPlaylistForm::SettingChanged(TMessage &msg)
{
  UpdateWorkArea();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::UpdateWorkArea()
{
  SystemParametersInfo(SPI_GETWORKAREA, 0, &work_area, 0);
}
// ------------------------------------------------ ---------------------------
void __fastcall TPlaylistForm::WMWindowPosChanging(TWMWindowPosChanging &msg)
{
  RECT sr;
  snapped = false; // Set does not adsorb
  // Test window.
  snapwin = MainForm->Handle;

  // the definition of Form2 adsorbed to the on Form1, here such as be changed to other software the window
  // handle of can also be adsorbed to the on a separate software window where

  if (snapwin && IsWindowVisible(snapwin))
  // The current position of the segment window Form1, Form2 mobile Form2 Form1 of the function to determine
  // whether in the vertical and horizontal directions, the distance is less than 50, such as less than 50 automatically
  // changes the position of the Form2, automatic adsorption onto Form1
  {
    if (GetWindowRect(snapwin, &sr)) // Form1 position
    {
      if ((msg.WindowPos->x <= (sr.right + thresh)) && (msg.WindowPos->x >= (sr.right - thresh)))
      {
        if ((msg.WindowPos->y > sr.top) && (msg.WindowPos->y < sr.bottom))
        {
          snapped = true;
          msg.WindowPos->x = sr.right;
        }
      }
      else if ((msg.WindowPos->x + msg.WindowPos->cx) >= (sr.left - thresh) && (msg.WindowPos->x + msg.WindowPos->cx) <= (sr.left + thresh))
      {
        if ((msg.WindowPos->y > sr.top) && (msg.WindowPos->y < sr.bottom))
        {
          snapped = true;
          msg.WindowPos->x = sr.left-msg.WindowPos->cx;
        }
      }

      if ((msg.WindowPos->y <= (sr.bottom + thresh)) && (msg.WindowPos->y >= (sr.bottom - thresh)))
      {
        if ((msg.WindowPos->x > sr.left) && (msg.WindowPos->x < sr.right))
        {
          snapped = true;
          msg.WindowPos->y = sr.bottom;
        }
      }
      else if ((msg.WindowPos->y + msg.WindowPos->cy) <= (sr.top + thresh) && (msg.WindowPos->y + msg.WindowPos->cy) >= (sr.top - thresh))
      {
        if ((msg.WindowPos->x > sr.left) && (msg.WindowPos->x < sr.right))
    {
          snapped = true;
          msg.WindowPos->y = sr.top-msg.WindowPos->cy;
        }
      }
    }
  }

  // Test screen
  sr = work_area;
  if (abs(msg.WindowPos->x) <= (sr.left + thresh))
  {
    snapped = true;
    msg.WindowPos->x = sr.left;
  }
  else if ((msg.WindowPos->x + msg.WindowPos->cx) >= (sr.right-thresh) && (msg.WindowPos->x + msg.WindowPos->cx) <= (sr.right + thresh))
  {
    snapped = true;
    msg.WindowPos->x = sr.right-msg.WindowPos->cx;
  }

  if (abs (msg.WindowPos->y) <= (sr.top + thresh))
  {
    snapped = true;
    msg.WindowPos->y = sr.top;
  }
  else if ((msg.WindowPos->y + msg.WindowPos->cy) >= (sr.bottom - thresh) && (msg.WindowPos->y + msg.WindowPos->cy) <= (sr.bottom + thresh))
  {
    snapped = true;
    msg.WindowPos->y = sr.bottom-msg.WindowPos->cy;
  }
}
*/
//---------------------------------------------------------------------------
// FYI:
//https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
//https://docs.microsoft.com/en-us/previous-versions/windows/desktop/api/wmp/nf-wmp-iwmpcontrols-playitem
//IWMPControls::
//fastForward - The fastForward method starts fast play of the media item in the forward direction.
//fastReverse - The fastReverse method starts fast play of the media item in the reverse direction.
//get_currentItem - The get_currentItem method retrieves the current media item in a playlist.
//get_currentMarker - The get_currentMarker method retrieves the current marker number.
//get_currentPosition - The get_currentPosition method retrieves the current position in the media item in seconds from the beginning.
//  HRESULT get_currentPosition(double &dCurrentPosition); // returns S_OK, pass address of double to receive current position in seconds
//get_currentPositionString - The get_currentPositionString method retrieves the current position in the media item as a BSTR formatted as HH:MM:SS (hours, minutes, and seconds).
//get_isAvailable - The get_isAvailable method indicates whether a specified type of information is available or a specified action can be performed.
//next - The next method sets the next item in the playlist as the current item.
//pause - The pause method pauses playback of the media item.
//play - The play method causes the current media item to start playing, or resumes play of a paused item.
//playItem - The playItem method plays the specified media item.
//previous - The previous method sets the previous item in the playlist as the current item.
//set_currentItem - The put_currentItem method specifies the current media item.
//set_currentMarker - The put_currentMarker method specifies the current marker number.
//set_currentPosition - The put_currentPosition method specifies the current position in the media item in seconds from the beginning.
//  HRESULT put_currentPosition(double dCurrentPosition); // returns S_OK
//stop
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#include <vcl.h>
#include "Main.h"
#pragma hdrstop
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
  CheckState = -1;
  bInhibitFlash = false;
  bDoubleClick = false;
  bCheckClick = false;
  Duration = 0;
  PrevState = 0;
  TimerMode = TM_NULL;
  bForceNextPlay = false;
  bSkipFilePrompt = false;
  bOpening = false;

  // properties
  FNextIndex = -1;
  FTargetIndex = -1;
  FTextColor = clBlack;
  FWmp = NULL;
  FOtherWmp = NULL;
  FPlayerA = true; // this will remain set if this list is for player A
  FInEditMode = false;
  pOFMSDlg = NULL;
  FCheckBox = new TMyCheckListBox(this);

  FCheckBox->Align = alClient;
  FCheckBox->AllowGrayed = true;
  FCheckBox->BorderStyle = bsNone;
  FCheckBox->Color = TColor(0xF5CFB8);
  FCheckBox->DragCursor = crDrag;
  FCheckBox->DragKind = dkDrag;
  FCheckBox->DragMode = dmAutomatic;
  FCheckBox->Enabled = true;
  FCheckBox->Flat = true;
  FCheckBox->Height = 143;
  FCheckBox->IntegralHeight = true;
  FCheckBox->ItemHeight = 13;
  FCheckBox->Left = 0;
  FCheckBox->Name = "MyCheckBox";
  FCheckBox->PopupMenu = PopupMenu1;
  FCheckBox->ShowHint = true;
  FCheckBox->Sorted = false;
  FCheckBox->Style = lbOwnerDrawVariable;
  FCheckBox->Top = 0;
  FCheckBox->Visible = true;
  FCheckBox->Width = 475;
  FCheckBox->Parent = this;

  FCheckBox->OnClick = CheckBoxClick;
  FCheckBox->OnClickCheck = CheckBoxClickCheck;
  FCheckBox->OnDblClick = CheckBoxDblClick;
  FCheckBox->OnDragDrop = CheckBoxDragDrop;
  FCheckBox->OnDragOver = CheckBoxDragOver;
  FCheckBox->OnMouseDown = CheckBoxMouseDown;
  FCheckBox->OnMouseMove = CheckBoxMouseMove;
  FCheckBox->OnDrawItem = CheckBoxDrawItem;
  FCheckBox->OnMeasureItem = CheckBoxMeasureItem;

  //enable drag&drop files
  ::DragAcceptFiles(this->Handle, true);

  // Start processing custom windows messages (for WM_SETTEXT to Unicode)
//  OldWinProc = WindowProc;
//  WindowProc = CustomMessageHandler;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormDestroy(TObject *Sender)
{
  if (FCheckBox != NULL)
    delete FCheckBox;

  DestroyFileDialog();
  DestroyImportDialog();
  DestroyExportDialog();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormClose(TObject* Sender, TCloseAction &Action)
{
  Timer1->Enabled = false;
  PositionTimer->Enabled = false;
  FlashTimer->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::Timer1Timer(TObject* Sender)
// Scroll to Tag item
{
  if (InEditMode) return;

  switch(TimerMode)
  {
    case TM_START_PLAYER:
      Timer1->Enabled = false;
      NextPlayer(true);
    break;

    case TM_STOP_PLAYER:
      Timer1->Enabled = false;
      StopPlayer(Wmp);
    break;

    case TM_CHECKBOX_CLICK:

      Timer1->Enabled = false;

      if (bDoubleClick)
      {
        if (!Wmp)
        {
          bDoubleClick = false;
          bCheckClick = false;
          return;
        }

        // Check the item so GetNext() will queue it
        FCheckBox->State[FCheckBox->ItemIndex] = cbGrayed;

        int oldtag = Tag;

        FNextIndex = FCheckBox->ItemIndex;

        Wmp->URL = GetNext();

        // Old listbox checkbox needs to be cleared...
        SetCheckState(oldtag);

        if (Tag >= 0)
        {
          bSkipFilePrompt = true; // don't ask for other-player files

          // Start this player
          StartPlayer(Wmp);
        }
      }
      else if (bCheckClick)
      {
        if (Wmp == NULL || MainForm->AutoFadeTimer->Enabled)
        {
          bDoubleClick = false;
          bCheckClick = false;
          return;
        }

        // disable highlight-scroll and flasher
        FlashTimer->Enabled = false;

        // Clicking on a gray will give us a checked
        // but it needs to be an unchecked
        if (FCheckBox->State[FCheckBox->ItemIndex] == cbGrayed)
        {
          CheckState = 1; // Checked
          FNextIndex = FCheckBox->ItemIndex; // start search here
          GetNext(true);
        }
        else // Checked
        {
          CheckState = 0; // Unchecked

          if (!FCheckBox->State[FCheckBox->ItemIndex] == cbUnchecked)
            FCheckBox->State[FCheckBox->ItemIndex] = cbUnchecked;

          // nothing playing?
          if (Tag < 0) QueueFirst();
          // if we unchecked an item that was queued, queue the
          // next item...
          else if (Tag == FCheckBox->ItemIndex)
          {
            NextPlayer();
            FCheckBox->ItemIndex = Tag;
          }
          else if (FTargetIndex == FCheckBox->ItemIndex)
          {
            FNextIndex = FCheckBox->ItemIndex; // start search here
            GetNext(true);
          }
        }

        SetTitleW();
      }
      else // single click an item
      {
        if (!Wmp || MainForm->AutoFadeTimer->Enabled)
        {
          bDoubleClick = false;
          bCheckClick = false;
          return;
        }

        // disable highlight-scroll and flasher
        FlashTimer->Enabled = false;

        // If we clicked on an item (not a checkbox)... queue it
        // or if we checked an unchecked box... queue it
        if (FCheckBox->ItemIndex >= 0 && FCheckBox->ItemIndex < FCheckBox->Items->Count)
        {
          // Check the item so GetNext() will queue it
          if (FCheckBox->State[FCheckBox->ItemIndex] != cbGrayed) FCheckBox->State[FCheckBox->ItemIndex] = cbGrayed;

          // If we were playing, may need to Check the item
          // (since we permit re-queuing a playing item)
          if (Tag >= 0 && Tag < FCheckBox->Items->Count && (Wmp ->playState == wmppsPlaying || Wmp->playState == wmppsPaused))
            FCheckBox->State[Tag] = cbChecked;

          QueueToIndex(FCheckBox->ItemIndex);
        }
      }

      bDoubleClick = false;
      bCheckClick = false;

    break;

    default:
      Timer1->Enabled = false;
      TimerMode = TM_NULL;
    break;
  };
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FlashTimerEvent(TObject* Sender)
{
  // 10=ready/stop
  // 1=stopped
  // 2=paused
  // 3=playing
  static bool bFlashOn = false;

  if (InEditMode) return;

  if (!Wmp || Timer1->Enabled) return;

  if (Focused()) FCheckBox->ItemIndex = FTargetIndex;
  // if no items, or the player's index is -1, or player is in an unknown state, turn off selection
  else if (!FCheckBox->Items->Count || Tag < 0 || Wmp->playState == wmposUndefined) FCheckBox->ItemIndex = -1;
  // if player is in pause or in play-mode
  else if (Wmp->playState == wmppsPaused || Wmp->playState == wmppsPlaying)
  {
    // Flash faster if in pause
    if (Wmp->playState == wmppsPaused)
    {
      if (FlashTimer->Interval != 250) FlashTimer->Interval = 250;
    }
    else if (FlashTimer->Interval != 500) FlashTimer->Interval = 500;

    if (bFlashOn)
    {
      FCheckBox->ItemIndex = -1;
      bFlashOn = false;
    }
    else
    {
      FCheckBox->ItemIndex = Tag;
      bFlashOn = true;
    }
  }
  // if player is in stop or ready mode
  else FCheckBox->ItemIndex = Tag;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxMouseMove(TObject* Sender,
      TShiftState Shift, int X, int Y)
{
  try
  {
    int Index = FCheckBox->ItemAtPos(Point(X,Y), true);

    if (Index >= 0 && Index < FCheckBox->Items->Count)
    {
      String S = MainForm->Utf8ToAnsi(FCheckBox->Items->Strings[Index]);

      if (S == Wmp->URL)
      {
        S = "";
        if (MediaInfo.artist[0] != NULLCHAR)
          S += "Artist: " + String(MediaInfo.artist) + String(LF);
        if (MediaInfo.album[0] != NULLCHAR)
          S += "Album: " + String(MediaInfo.album) + String(LF);
        if (MediaInfo.name[0] != NULLCHAR)
          S += "Song: " + String(MediaInfo.name);
      }

      if (!S.IsEmpty() && S != FCheckBox->Hint)
        FCheckBox->Hint = S;
  }
  }
  catch(...) {};
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxClickCheck(TObject* Sender)
{
  if (!InEditMode)
    bCheckClick = true;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxClick(TObject* Sender)
{
  if (!Timer1->Enabled && !InEditMode)
    SetTimer(TM_CHECKBOX_CLICK, TIME_300);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxDblClick(TObject* Sender)
{
  if (!InEditMode)
    bDoubleClick = true;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxMeasureItem(TWinControl *Control, int Index, int &Height)
{
  // here we need the height of the tallest char
  Height = FCheckBox->Canvas->TextHeight(FCheckBox->Items->Strings[Index]);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxDrawItem(TWinControl *Control, int Index, const TRect &Rect, TOwnerDrawState State)
{
  if (FCheckBox->Items->Strings[Index].IsEmpty() || !FCheckBox->Canvas->TryLock())
    return;

  try
  {
//    FCheckBox->Canvas->Font->Charset = ANSI_CHARSET;
//    FCheckBox->Canvas->TextFlags = ETO_OPAQUE;
    FCheckBox->Canvas->Font->Color = (TColor)FCheckBox->Items->Objects[Index];
//    FCheckBox->Canvas->Brush->Color = utils->YcToTColor(bg);
    WideString sOut = MainForm->Utf8ToWide(FCheckBox->Items->Strings[Index]);
    ::TextOutW(FCheckBox->Canvas->Handle, Rect.Left, Rect.Top, sOut.c_bstr(), sOut.Length());
  }
  catch(...) { }

  if (State.Contains(odFocused))
  {
    TRect r = FCheckBox->ItemRect(Index);
    FCheckBox->Canvas->DrawFocusRect(r); // erase artifact around the item

    // I added the CheckRect to MyCheckLst.pas and actually rolled my own
    // MyCheckListBox derrived component here...
    // (note: the compiler generates us a  MyCheckLst.hpp file to include)
    TRect RCheck = FCheckBox->CheckRect[Index];
    FCheckBox->Canvas->DrawFocusRect(RCheck); // erase artifact around the checkbox
  }
  FCheckBox->Canvas->Unlock();
}
//---------------------------------------------------------------------------
//HPEN __fastcall TPlaylistForm::CreateFocusPen()
//{
//  LONG width(1);
//  SystemParametersInfo(SPI_GETFOCUSBORDERHEIGHT, 0, &width, 0);
//  LOGBRUSH lb = { };     // initialize to zero
//  lb.lbColor = 0xffffff; // white
//  lb.lbStyle = BS_SOLID;
//  return ExtCreatePen(PS_GEOMETRIC | PS_DOT, width, &lb, 0, 0);
//}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::DrawFocusRect(TCanvas* c, HPEN hpen, TRect &r)
{
  HDC hdc = c->Handle;
  HPEN old_pen = SelectObject(hdc, hpen);
  int old_rop = SetROP2(hdc, R2_XORPEN);
  c->FrameRect(r);
  SelectObject(hdc, old_pen);
  SetROP2(hdc, old_rop);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormActivate(TObject* Sender)
{
  // disable highlight-scroll and flasher
  SetTitleW();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormDeactivate(TObject* Sender)
{
//  if (InEditMode)
//    ExitEditMode1Click(NULL);

  if (Visible)
    SetTitleW();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormHide(TObject* Sender)
// This will fire first, then Deactivate fires...
{
  FlashTimer->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxMouseDown(TObject* Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (Button != mbLeft)
    return;

  try
  {
    if (!InEditMode)
    {
      if (FCheckBox->Items->Count)
      {
        int Index = FCheckBox->ItemAtPos(Point(X,Y), true);

        // Drag-drop, need to store the index in the source-object
        if (Index >= 0 && Index < FCheckBox->Items->Count)
          FCheckBox->Tag = Index;
      }
    }
  }
  catch(...) { }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxDragDrop(TObject* Sender, TObject* Source, int X, int Y)
// User has released a dragged item(s) over the destination list...
{
  if (Sender->ClassNameIs("TMyCheckListBox") && Source->ClassNameIs("TMyCheckListBox"))
  {
    TMyCheckListBox* DestList = (TMyCheckListBox*)Sender;
    TMyCheckListBox* SourceList = (TMyCheckListBox*)Source;
    TPlaylistForm* SourceForm = (TPlaylistForm*)SourceList->Parent;

    int SourceIndex = SourceList->Tag;
    int DestIndex = DestList->ItemAtPos(Point(X,Y), true);

    if (SourceForm->InEditMode) // Dragging in edit-mode?
    {
      for (int ii = 0 ; ii < SourceList->Items->Count ; ii++)
        if (SourceList->Selected[ii])
          if (InsertNewDeleteOld(SourceList, DestList, ii, DestIndex))
          {
            ii--; // Keep index the same if item deleted from source-list

            // This prevents items from being inserted backward!!!
            if (DestIndex != -1) DestIndex++;
          }
    }
    else InsertNewDeleteOld(SourceList, DestList, SourceIndex, DestIndex);
  }
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::InsertNewDeleteOld(TMyCheckListBox* SourceList, TMyCheckListBox* DestList, int SourceIndex, int &DestIndex)
{
  try
  {
    if (SourceIndex < 0 || SourceIndex >= SourceList->Items->Count) return false;

    bool bWasPlaying = false;

    TPlaylistForm* SourceForm = (TPlaylistForm*)SourceList->Parent;

    // Don't allow move of source player's song
    // to other player if it is playing or paused!
    if (SourceForm->Tag == SourceIndex && (SourceForm->Wmp->playState == wmppsPlaying || SourceForm->Wmp->playState == wmppsPaused))
    {
      if (DestList != SourceList) return false;
      bWasPlaying = true;
    }

    if (!SourceForm->InEditMode && SourceList->ItemIndex > SourceIndex)
      SourceList->ItemIndex--;
    if (SourceForm->Tag > SourceIndex)
      SourceForm->Tag--;

    // Save item before it is deleted
    String S = SourceList->Items->Strings[SourceIndex];

    SourceList->Items->Delete(SourceIndex);

    if (!SourceList->Items->Count)
    {
      SourceList->ItemIndex = -1;
      SourceForm->TargetIndex = -1;
      SourceForm->NextIndex = -1;
      SourceForm->Tag = -1;
    }
    else if (SourceList->ItemIndex == -1)
      SourceList->ItemIndex = SourceList->Items->Count-1;

    // if not moving within the same list...
    if (DestList != SourceList)
      SourceForm->QueueToIndex(SourceList->ItemIndex);
    // If the same list for source/dest and we
    // deleted an item in front of the insert-point, must adjust!
    else if (DestIndex != -1 && SourceIndex < DestIndex)
      DestIndex--; // DestIndex is a referenced var!!

    // Insert into destination list...
    DestList->Items->Insert(DestIndex, S);

    // insert at end of list?
    if (DestIndex == -1)
      DestIndex = DestList->Items->Count-1;
    if (DestList->ItemIndex == -1)
      DestList->ItemIndex = DestList->Items->Count-1;

    if (!this->InEditMode && DestList->ItemIndex >= DestIndex)
      DestList->ItemIndex++;
    if (this->Tag >= DestIndex)
      this->Tag++;

    if (bWasPlaying)
    {
      DestList->State[DestIndex] = cbChecked;
      this->Tag = DestIndex;
    }
    else
    {
      DestList->State[DestIndex] = cbGrayed;
      this->QueueToIndex(DestIndex);
    }

    return true;
  }
  catch(...) { return false; }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxDragOver(TObject* Sender, TObject* Source, int X, int Y, TDragState State, bool &Accept)
{
  // Accept the item if its from a TMyCheckListBox
  Accept = Source->ClassNameIs("TMyCheckListBox");
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
      WideString w = MainForm->Utf8ToWide(String((char*)msg.LParam));
      DefWindowProcW(this->Handle, msg.Msg, 0, (LPARAM)w.c_bstr());
    }
    else
      this->OldWinProc(msg); // Call main handler
  }
  else
    this->OldWinProc(msg); // Call main handler
}
*/
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::WMListDropFile(TWMDropFiles &Msg)
{
  MainForm->LoadListWithDroppedFiles(Msg, this);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::WMSetText(TWMSetText &Msg)
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
void __fastcall TPlaylistForm::QueueToIndex(int Index)
{
  FNextIndex = Index;

  // Can't Set URL or we stop the player! If player playing or paused... don't set URL
  // because it will stop the current song
  if (!(Wmp->playState == wmppsPlaying || Wmp->playState == wmppsPaused))
    Wmp->URL = GetNext();

  FTargetIndex = Index;
  SetTitleW();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormKeyDown(TObject* Sender, WORD &Key, TShiftState Shift)
{
  if (InEditMode && Key == VK_ESCAPE)
  {
    ExitEditMode1Click(NULL);
    Key = 0;
    return;
  }

  if (Key != VK_DELETE) return;

  Key = 0;

  if (FCheckBox->Items->Count)
  {
    if (!Shift.Contains(ssShift))
      DeleteItem(FCheckBox->ItemIndex);
    else // delete all unchecked
    {
      for (int ii = FCheckBox->Items->Count-1 ; ii >= 0  ; ii--)
      {
        if (FCheckBox->State[ii] == cbUnchecked)
        {
          if (FTargetIndex >= ii)
            FTargetIndex--;
          if (Tag >= ii)
            Tag--;
          if (NextIndex >= ii)
            FNextIndex--;
          FCheckBox->Items->Delete(ii);
        }
      }
    }

    // nothing playing?
    if (Tag < 0)
      QueueFirst();

    SetTitleW();
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::DeleteItem(int ItemIndex)
{
  if (ItemIndex >= 0 && ItemIndex < FCheckBox->Items->Count)
  {
    // Is this song playing? If so, go to next song.
    if (ItemIndex == Tag)
    {
      FNextIndex = FTargetIndex;
      NextPlayer();
    }

    int SaveTargetIndex = FTargetIndex;
    int SaveTag = Tag;
    int SaveItemIndex = ItemIndex;

    if (FTargetIndex > SaveItemIndex)
      FTargetIndex--;
    if (Tag > SaveItemIndex)
      Tag--;
    if (FNextIndex > SaveItemIndex)
      FNextIndex--;

    try { FCheckBox->Items->Delete(SaveItemIndex); }
    catch(...) { ShowMessage("Error deleting list index: " + String(SaveItemIndex)); }

    if (!FCheckBox->Items->Count)
    {
      FTargetIndex = -1;
      FNextIndex = -1;
      Tag = -1;
    }

    // if we deleted an item that was queued, queue the
    // next item...
    if (SaveTag == SaveItemIndex)
    {
      NextPlayer();
      FTargetIndex = SaveTag;
      if (FTargetIndex >= FCheckBox->Items->Count)
        FTargetIndex = 0;
    }
    else if (SaveTargetIndex == SaveItemIndex)
    {
      FNextIndex = SaveItemIndex; // start search here
      if (NextIndex >= FCheckBox->Items->Count)
        FNextIndex = 0;
      GetNext(true);
    }
  }
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::QueueFirst(void)
{
  if (!Wmp || !FCheckBox->Items->Count) return false;

  try
  {
    if (Tag < 0)
    {
      FNextIndex = 0; // reset list
      Wmp->URL = GetNext();
      FCheckBox->ItemIndex = FTargetIndex;
      return true;
    }
  }
  catch(...) {}

  return false;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::NextPlayer(bool bForceStartPlay)
{
  if (!Wmp) return;

  bool bWasPlaying = false;

  // this player on now?
  if (Wmp->playState == wmppsPlaying)
    bWasPlaying = true; // playing?

  // Make sure Tag is in-bounds...
  if (Tag >= FCheckBox->Items->Count)
    Tag = 0;
  if (NextIndex >= FCheckBox->Items->Count)
    FNextIndex = 0;

  int oldtag = Tag;

  if (NextIndex < 0)
    FNextIndex = Tag + 1;

  WideString File = GetNext(false, true); // enable random

#if DEBUG_ON
  MainForm->CWrite("\r\nNextPlayer() Tag:" + String(Tag) + " Target:" + String(TargetIndex) + " File:" + String(File) + "\r\n");
#endif

  // Old listbox checkbox needs to be cleared...
  try { SetCheckState(oldtag); }
  catch(...) { ShowMessage(STR[1] + String(oldtag)); }

  if (Tag >= 0)
  {
    Wmp->URL = File;

    SetTitleW();

    if (bWasPlaying || bForceStartPlay)
    {
      bSkipFilePrompt = true;

      // Start player
      StartPlayer(Wmp);
    }
  }
  else if (!MainForm->ForceFade())  // no more checked items
    StopPlayer(Wmp); // Stop player
}

//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SetCheckState(int oldtag)
{
  if (oldtag >= 0 && oldtag <= FCheckBox->Items->Count)
  {
    bool bRepeatMode = PlayerA ? MainForm->RepeatModeA : MainForm->RepeatModeB;

    if (bRepeatMode)
      FCheckBox->State[oldtag] = cbGrayed;
    else
      FCheckBox->State[oldtag] = cbUnchecked;
  }
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
// Stop while playing: 1, 8, 6, 7, 6
//
void __fastcall TPlaylistForm::OpenStateChange(WMPOpenState NewState)
{
  if (!Wmp) return;

  try
  {
    if (NewState == wmposMediaOpen) // Media Open
    {
      Duration = (int)Wmp->currentMedia->duration;

      if (PlayerA)
        TimeDisplay(Duration, 1);
      else
        TimeDisplay(Duration, 4);

      // Green

#if DEBUG_ON
      MainForm->CWrite( "\r\nwmposMediaOpen: Tag:" + String(Tag) + " Target:" + String(TargetIndex) + "\r\n");
#endif
      if (bOpening && Tag >= 0 && Tag < FCheckBox->Items->Count)
        FCheckBox->Items->Objects[Tag] = (TObject*)clGreen;

      bOpening = false;
    }
    else if (NewState == wmposOpeningUnknownURL)
    {
#if DEBUG_ON
      MainForm->CWrite( "\r\nwmposOpeningUnknownURL: Tag:" + String(Tag) + " Target:" + String(TargetIndex) + "\r\n");
#endif
      bOpening = true;
    }
    else if (NewState == wmposMediaOpening)
    {
#if DEBUG_ON
      MainForm->CWrite( "\r\nwmposMediaOpening: Tag:" + String(Tag) + " Target:" + String(TargetIndex) + "\r\n");
#endif
      bOpening = true;
    }
    else if (NewState == wmposPlaylistOpenNoMedia)
    {
      // Red
#if DEBUG_ON
      MainForm->CWrite( "\r\nwmposPlaylistOpenNoMedia: Tag:" + String(Tag) + " Target:" + String(TargetIndex) + "\r\n");
#endif
      if (bOpening && Tag >= 0 && Tag < FCheckBox->Items->Count)
        FCheckBox->Items->Objects[Tag] = (TObject*)clRed;

      bOpening = false;
    }
#if DEBUG_ON
    else
    {
      MainForm->CWrite( "\r\Misc OpenStateChange: " + String(NewState) + "\r\n");
    }
#endif
  }
  catch(...) {}

  MainForm->SetCurrentPlayer(); // Set CurrentPlayer variable (used for color-coding)
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::PositionTimerEvent(TObject* Sender)
// Fires every second to check the remaining play-time
{
  // Return if this player is not playing...
  if (!Wmp || !OtherWmp || Wmp->playState != wmppsPlaying) return;

  try
  {
    if (!MainForm->ManualFade)
    {
      if (FOtherForm->Tag >= 0)
      {
        if (Duration-(int)Wmp->controls->currentPosition <= MainForm->FadeAt)
        {
          // Start Other Player
          if (OtherWmp->playState != wmppsPlaying)
            StartPlayer(OtherWmp);

          if (PlayerA)
          {
            if (MainForm->TrackBar1->Position != 100)
            {
              MainForm->bFadeRight = true; // Set fade-direction
              MainForm->AutoFadeTimer->Enabled = true; // Start a fade right
            }
          }
          else if (MainForm->TrackBar1->Position != 0)
          {
            MainForm->bFadeRight = false; // Set fade-direction
            MainForm->AutoFadeTimer->Enabled = true; // Start a fade left
          }
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
void __fastcall TPlaylistForm::PlayStateChange(WMPPlayState NewState)
{
  if (Wmp == NULL || OtherWmp == NULL) return;

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
      if (NewState == wmppsPlaying) // playing?
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
    // 1=stopped
    // 2=paused
    // 3=playing
    if (NewState == wmppsPaused) // pause?
    {
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
    else if (NewState == wmppsPlaying) // play?
    {
      if (Tag < 0)
      {
        // illegal to start if nothing checked
        PositionTimer->Enabled = false;

        SetTimer(TM_STOP_PLAYER, TIME_100);
      }
      else
      {
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
        if (!MainForm->ManualFade)
        {
//          String FileTopic;
//          String OpenDialogString;

          if (PlayerA)
          {
//            FileTopic = ADD_B_TITLE;
//            OpenDialogString = MainForm->SaveDirB;

            if (MainForm->TrackBar1->Position != 0) // stopped
            {
              MainForm->bFadeRight = false;
              MainForm->AutoFadeTimer->Enabled = true;
            }
          }
          else
          {
//            FileTopic = ADD_A_TITLE;
//            OpenDialogString = MainForm->SaveDirA;

            if (MainForm->TrackBar1->Position != 100) // stopped
            {
              MainForm->bFadeRight = true;
              MainForm->AutoFadeTimer->Enabled = true;
            }
          }

          // Prompt for files on other player if no list and Auto-Fade
          // and we were not in pause
  //        if (!FOtherForm->bSkipFilePrompt &&
  //                 FOtherForm->PrevState != 2 && FOtherForm->Tag < 0)
  //          if (MainForm->FileDialog(FOtherForm, OpenDialogString, FileTopic))
  //            FOtherForm->QueueFirst();
        }

        FCheckBox->State[Tag] = cbChecked;

        // Need to update target index without affecting TAG if
        // we just started a manually queued (by single-click) song.
        // (So that when clicking the list-box we see the next
        // grey-checked song in the list queued...)
        FNextIndex = this->Tag;
        GetNext(true, true); // allow random...

        bSkipFilePrompt = false;

        PositionTimer->Enabled = true;
        SetTitleW();
      }
    }
    else if (NewState == wmppsStopped) // stop?
    {
      PositionTimer->Enabled = false;
      UpdatePlayerStatus();

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

      if (bForceNextPlay) // set when NewState == wmppsMediaEnded
      {
        SetTimer(TM_START_PLAYER, TIME_100);
        bForceNextPlay = false;
      }
      else
        NextPlayer();

      SetTitleW();
    }
    else if (NewState == wmppsMediaEnded) // Song ended?
      bForceNextPlay = true;
  }
  catch(...) {}

  MainForm->SetCurrentPlayer();

  if (NewState != wmppsTransitioning) // not transitioning? save state...
    PrevState = NewState;
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

  String uPath = MainForm->WideToUtf8(Wmp->URL);
  strncpy(sms.path, uPath.c_str(), SONG_PATH_SIZE-1);
  sms.len_path = uPath.Length();
  if (sms.len_path > SONG_PATH_SIZE-1)
    sms.len_path = SONG_PATH_SIZE-1;

  String uTitle = MainForm->WideToUtf8(Wmp->currentMedia->name);
  strncpy(sms.name, uTitle.c_str(), SONG_NAME_SIZE-1);
  sms.len_name = uTitle.Length();
  if (sms.len_name > SONG_NAME_SIZE-1)
    sms.len_name = SONG_NAME_SIZE-1;

  // Note the "L" to make string-constants wide!!!!!!
  String uArtist = MainForm->WideToUtf8(Wmp->currentMedia->getItemInfo(L"WM/AlbumArtist"));
  if (uArtist == "" || uArtist.LowerCase() == "various artists" || uArtist.LowerCase() == "various")
    uArtist = MainForm->WideToUtf8(Wmp->currentMedia->getItemInfo(L"Author"));
  strncpy(sms.artist, uArtist.c_str(), SONG_NAME_SIZE-1);
  sms.len_artist = uArtist.Length();
  if (sms.len_artist > SONG_NAME_SIZE-1)
    sms.len_artist = SONG_NAME_SIZE-1;

  String uAlbum = MainForm->WideToUtf8(Wmp->currentMedia->getItemInfo(L"WM/AlbumTitle"));
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
    HANDLE hnd = FindWindow("TDTSColor", "YahCoLoRiZe");

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
  Timer1->Enabled = false;
  TimerMode = mode;
  Timer1->Interval = time;
  Timer1->Enabled = true;
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
      sms.duration = Duration;
      sms.current = CurrentPos;

//!!!!!!!!!!!!!! not yet implimented
      sms.color = 0;
      sms.redtime = MainForm->FadeAt;
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
void __fastcall TPlaylistForm::SetTitleW(void)
{
  if (InEditMode)
  {
    MySetCaption(String(STR[2]), false); // Ansi-mode
    return;
  }

  try
  {
    String S1, S2;

    S2 = "(nothing queued)";

    if (PlayerA)
      S1 = "PlayerA ";
    else
      S1 = "PlayerB ";

    // only flash if playing and not-active
    // assume no-flash
    FlashTimer->Enabled = false;
    int SelectIdx = -1;

    if (FCheckBox->Items->Count)
    {
      if (Active) // Window is focused?
      {
        if (Tag >= 0 && Tag < FCheckBox->Items->Count)
        {
          if (FCheckBox->State[Tag] == cbChecked) // playing?
          {
            if (FTargetIndex >= 0 && FTargetIndex < FCheckBox->Items->Count)
            {
              S1 += "(Q) ";
              S2 = FCheckBox->Items->Strings[FTargetIndex];
              SelectIdx = FTargetIndex;
            }
          }
          else // not playing...
          {
            S1 += "(Q) ";
            S2 = FCheckBox->Items->Strings[Tag];
            SelectIdx = Tag;
          }
        }
      }
      else // Not focused...
      {
        if (Tag >= 0)
        {
          if (FCheckBox->State[Tag] == cbChecked)
          {
            S1 += "(P) ";
            FlashTimer->Enabled = true;
          }
          else // not focused and player is idle...
            S1 += "(Q) ";

          if (Tag >= 0 && Tag < FCheckBox->Items->Count)
          {
            S2 = FCheckBox->Items->Strings[Tag];
            SelectIdx = Tag;
          }
        }
      }
    }

    // Set listbox selection to match title
    FCheckBox->ItemIndex = SelectIdx;

    // Buttons and icon
    int Misc = 3*GetSystemMetrics(SM_CXSMSIZE);

    #define TITLE_PIXELS_PER_CHAR 8
    // Available # chars for song title is the
    // total form width - icon widths - S1 width - "..."
    int W = (Width-Misc)/TITLE_PIXELS_PER_CHAR - S1.Length() - 3;

    WideString sW1 = WideString(S1);
    WideString sW2 = MainForm->Utf8ToWide(S2);

    if (sW2.Length() > W)
    {
      sW1 += L"...";
      int len = sW2.Length();
      MySetCaption(sW1 + sW2.SubString(len-W, len-(len-W)+1));
    }
    else
      MySetCaption(sW1 + sW2);
  }
  catch(...)
  {
    ShowMessage("SetTitle() threw an exception");
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::StartPlayer(TWindowsMediaPlayer* p)
{
  p->settings->mute = true;
  p->controls->play();
  p->settings->mute = false;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::StopPlayer(TWindowsMediaPlayer* p)
{
  p->settings->mute = true;
  p->controls->stop();
  p->settings->mute = false;
}
//---------------------------------------------------------------------------
// Gets a UTF-8 string from the listbox and converts it to UTF-16
WideString __fastcall TPlaylistForm::GetNext(bool bNoSet, bool bEnableRandom)
// Given pointer to a Form containing a listbox, returns the first file-name that has its check-box grayed (queued song).
// The new index is returned. -1 is returned if no play-enabled files remain.
//
// Do not include the second arguement to return Tag and FTargetIndex. FTargetIndex is the index of the next grey-checked item after Tag...
//
// Set bNoSet true to cause Tag to be unaffected, instead the next grey-checked item is returned in FTargetIndex.
//
// BEFORE calling this function, set FNextIndex to a >= 0 value to force searching to begin there.
//
// Will return with NextInxex set to -1.
{
  String sFile;

  try
  {
    int c = FCheckBox->Items->Count;

    // If Form pointer is null or no items in listbox, return ""
    if (!c)
    {
      if (!bNoSet) Tag = -1;
      return "";
    }

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
          FNextIndex = random(c);

          if (FCheckBox->State[NextIndex] == cbGrayed) break;
        }
      }

      loops = 2;
    }

    for (int ii = 0 ; ii < loops ; ii++)
    {
      if (NextIndex < 0)
        FNextIndex = Tag; // start at current song if NextInxex is -1

      if (NextIndex < 0) return "";

      int jj;
      for (jj = 0 ; jj < c ; jj++, FNextIndex++)
      {
        if (NextIndex >= c)
          FNextIndex = 0;

        if (FCheckBox->State[NextIndex] == cbGrayed)
        {
          if (ii == 0) // first loop...
            sFile = FCheckBox->Items->Strings[NextIndex];

          break;
        }
      }

      if (jj == c) FNextIndex = -1; // no Play-flags set

      if (NextIndex >= c)
      {
        ShowMessage("GetNext() Tag Index is out-of-range!");
        Tag = -1;
        FTargetIndex = -1;
      }
      else if (!bNoSet)
      {
        if (ii == 0) // first loop...
        {
          Tag = FNextIndex;
          FNextIndex = Tag+1;
        }
        else
          FTargetIndex = FNextIndex;
      }
      else
        FTargetIndex = FNextIndex;
    }

    FNextIndex = -1;
    SetTitleW();
  }
  catch(...) { ShowMessage("GetNext() threw an exception..."); }

  // URL in Windows Media Player is a WideString... do not percent-encode!
  // Convert UTF-8 to UTF-16!
  return MainForm->Utf8ToWide(sFile);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::ClearAndStop(void)
{
  if (!Wmp) return;

  FlashTimer->Enabled = false;
  Timer1->Enabled = false;
  PositionTimer->Enabled = false;

  // Stop and clear list...
  MainForm->AutoFadeTimer->Enabled = false;

  // Park the trackbar (seemed like a good idea - but on second thought
  // it's probably better to just freeze a fade-in-progress and let the user
  // handle the remaining fade... to avoid a sudden volume-change...)
//  if (PlayerA)
//    MainForm->TrackBar1->Position = 100;
//  else
//    MainForm->TrackBar1->Position = 0;

  StopPlayer(Wmp);

  Wmp->URL = L"";

  String S = PlayerA ? "PlayerA " : "PlayerB ";
  MySetCaption(S + "(nothing queued)", false); // Ansi-mode

  FCheckBox->Clear();

  Tag = -1;
  FTargetIndex = -1;
  FNextIndex = -1;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::EditMode1Click(TObject* Sender)
// PopupMenu1 F3
{
  // Enter Edit Mode
  InEditMode = true;

  FlashTimer->Enabled = false;
  Timer1->Enabled = false;
  bCheckClick = false;
  bDoubleClick = false;

  FCheckBox->PopupMenu = PopupMenu2;

  int SaveIdx = FCheckBox->ItemIndex;
  FCheckBox->ExtendedSelect = true;
  FCheckBox->MultiSelect = true;
  FCheckBox->Selected[SaveIdx] = true;

  SetTitleW();
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
  for (int ii = 0 ; ii < FCheckBox->Items->Count ; ii++)
    if (ii != Tag && ii != FTargetIndex)
      FCheckBox->State[ii] = cbGrayed;
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
  for (int ii = 0 ; ii < FCheckBox->Items->Count ; ii++)
    if (ii != Tag && ii != FTargetIndex)
      FCheckBox->State[ii] = cbUnchecked;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::DeleteSelected1Click(TObject* Sender)
// PopupMenu1 F8
{
  if (FCheckBox->Items->Count)
  {
    DeleteItem(FCheckBox->ItemIndex);

    // nothing playing?
    if (Tag < 0)
      QueueFirst();

    SetTitleW();
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::ExitEditMode1Click(TObject* Sender)
// PopupMenu2 F3
{
  // Exit Edit Mode
  FCheckBox->PopupMenu = PopupMenu1;
  FCheckBox->ExtendedSelect = false;
  FCheckBox->MultiSelect = false;

  InEditMode = false;
  SetTitleW(); // Start flashing again, etc.
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::MoveSelectedClick(TObject* Sender)
// PopupMenu2 F4
{
  // Copy Selected Items To Other Playlist

  String Item;

  if (FCheckBox->SelCount)
  {
    int InsertIdx;

    for (int ii = 0 ; ii < FCheckBox->Items->Count ; ii++)
    {
      if (FCheckBox->Selected[ii])
      {
        InsertIdx = FOtherForm->FCheckBox->ItemIndex;

        if (InsertIdx >= 0 && InsertIdx < FCheckBox->Items->Count)
        {
          // Insert above first selected item in other list
          FOtherForm->FCheckBox->Items->InsertObject(InsertIdx, FCheckBox->Items->Strings[ii], FCheckBox->Items->Objects[ii]);
          FOtherForm->FCheckBox->State[InsertIdx] = cbGrayed;
        }
        else
        {
          // Add to end of other list
          FOtherForm->FCheckBox->Items->AddObject(FCheckBox->Items->Strings[ii], FCheckBox->Items->Objects[ii]);
          FOtherForm->FCheckBox->State[FOtherForm->FCheckBox->Items->Count-1] = cbGrayed;
        }
      }
    }

    MainForm->ShowPlaylist(FOtherForm);

    // Remove from this list (must remove backward)
    for (int ii = FCheckBox->Items->Count-1 ; ii > 0  ; ii--)
      if (FCheckBox->Selected[ii])
        DeleteItem(ii);
  }
  else
    ShowMessage(STR[0]);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::DeleteSelectedClick(TObject* Sender)
// PopupMenu2 F5
{
  // Delete Selected Items
  if (FCheckBox->SelCount)
  {
    // Remove from this list (must remove backward)
    for (int ii = FCheckBox->Items->Count-1 ; ii > 0  ; ii--)
      if (FCheckBox->Selected[ii])
        DeleteItem(ii);
  }
  else
    ShowMessage(STR[0]);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::RemoveDuplicates1Click(TObject* Sender)
{
  // Return if no items or a song is playing
  if (Wmp == NULL || FCheckBox->Items->Count == 0 || Wmp->playState == wmppsPlaying)
  {
    ShowMessage(STR[3]);
    return;
  }

  String sTemp;

  for (int ii = 0; ii < FCheckBox->Items->Count; ii++)
  {
    sTemp = FCheckBox->Items->Strings[ii];
    for (int jj = FCheckBox->Items->Count-1; jj > ii; jj--)
      if (sTemp == FCheckBox->Items->Strings[jj])
        FCheckBox->Items->Delete(jj);
  }

// This worked and was fast but won't handle TextColor as an TObject*
/*
  TStringList* sl = new TStringList();
  sl->Sorted = true;
  sl->Duplicates = dupIgnore;

  sl->AddStrings(FCheckBox->Items);

  FCheckBox->Items->Clear();
  FCheckBox->Items->AddStrings(sl);

  delete sl;
*/

  Tag = -1;
  FTargetIndex = -1;
  CheckAllItems();

  QueueToIndex(0);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::RandomizeList1Click(TObject* Sender)
{
  // Return if no items or a song is playing
  if (FCheckBox->Items->Count == 0 || Wmp->playState == wmppsPlaying)
  {
    ShowMessage(STR[3]);
    return;
  }

  int Count = FCheckBox->Items->Count;

  TProgressForm::Init(Count);

  FCheckBox->Enabled = false;

  for(int ii = 0; ii < Count; ii++)
  {
    FCheckBox->Items->Move(random(Count), random(Count));
    TProgressForm::Move(ii);
  }

  TProgressForm::UnInit();

  FCheckBox->Enabled = true;

  Tag = -1;
  FTargetIndex = -1;
  CheckAllItems();

  QueueToIndex(0);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SelectAllItemsClick(TObject *Sender)
{
  int Count = FCheckBox->Items->Count;

  TProgressForm::Init(Count);

  FCheckBox->Enabled = false;

  for (int ii = 0; ii < Count; ii++)
  {
    FCheckBox->Selected[ii] = true;
    TProgressForm::Move(ii);
  }

  FCheckBox->Enabled = true;

  TProgressForm::UnInit();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CopySelectedClick(TObject* Sender)
// Copies data as UTF-16 format
{
  if (FCheckBox->SelCount == 0)
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
      for (int ii = 0 ; ii < FCheckBox->Items->Count ; ii++)
        if (FCheckBox->Selected[ii])
          sl->Add(FCheckBox->Items->Strings[ii]);

      WideString ws = MainForm->Utf8ToWide(sl->Text);

      int len = ws.Length() * sizeof(WideChar);

      int termlen = 4; // Allow space for 4 bytes of 0's to terminate

      hMem = GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE, len+termlen);

      if (hMem != NULL)
      {
        // Move string into global-memory
        char* lp = (char *)GlobalLock(hMem);

        CopyMemory(lp, ws.c_bstr(), len);

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
  if (pOFMSDlg != NULL)
  {
    pOFMSDlg->Close();
    pOFMSDlg->Release();
    pOFMSDlg = NULL;
  }
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
void __fastcall TPlaylistForm::DestroyImportDialog(void)
{
  if (pImportDlg != NULL)
  {
    pImportDlg->Close();
    pImportDlg->Release();
    pImportDlg = NULL;
  }
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
  if (pExportDlg != NULL)
  {
    pExportDlg->Close();
    pExportDlg->Release();
    pExportDlg = NULL;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CopySongInfoToClipboard1Click(TObject* Sender)
{
  try
  {
    String s;

    String Artist = Wmp->currentMedia->getItemInfo(L"WM/AlbumArtist");

    if (Artist == "" || Artist.LowerCase() == "various artists" ||
                                         Artist.LowerCase() == "various")
      Artist = Wmp->currentMedia->getItemInfo(L"Author");

    s = Artist;

    if (!s.IsEmpty())
      s += String(", ");

    s += Wmp->currentMedia->getItemInfo(L"WM/AlbumTitle");

    if (!s.IsEmpty())
      s += String(", ");

    s += Wmp->currentMedia->name;

    if (!s.IsEmpty())
      Clipboard()->AsText = s;
  }
  catch(...) {}
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::MySetCaption(String sStr, bool bStrIsUtf8)
// If you pass in an Ansi string, set bStrIsUtf8 false
{
  Caption = bStrIsUtf8 ? sStr : MainForm->AnsiToUtf8(sStr);
}

void __fastcall TPlaylistForm::MySetCaption(WideString wStr)
// wStr is WideString
{
  Caption = MainForm->WideToUtf8(wStr);
}
//---------------------------------------------------------------------------
//void __fastcall TPlaylistForm::WMMove(TWMMove &Msg)
//{
//  try
//  {
//    // first call the base class handler
//    TForm::Dispatch(&Msg);
//
//    if (MainForm->GDock != NULL)
//  		MainForm->GDock->WindowMoved(this->Handle);
//  }
//  catch(...) { }
//}
//In the the by adding in the of Form2 unit file unit.cpp the the are as follows code:
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


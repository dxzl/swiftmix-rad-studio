//---------------------------------------------------------------------------
#ifndef SMListH
#define SMListH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <Dialogs.hpp>
#include <StdCtrls.hpp>
#include <Vcl.CheckLst.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <System.WideStrUtils.hpp>

#include "..\..\21.0\Imports\WMPLib_OCX.h"
#include "..\..\21.0\Imports\WMPLib_TLB.h"
//---------------------------------------------------------------------------

#define PLAY_PREVIEW_START_TIME 5.0 // start time into song for play-preview

// Sort constants
#define SORTBY_ARTIST 0
#define SORTBY_ALBUM  1
#define SORTBY_TITLE  2

// Timer times
#define TIME_2000 1000  // used to exit edit mode after up/down/left/right scroll-keys were pressed
#define TIME_300  300  // used to sort out what kind of check-box click we got
#define TIME_50  50  // used to delay for start/stop player

// Timer modes
#define TM_NULL                 0
#define TM_START_PLAYER         1
#define TM_NEXT_SONG            2
#define TM_NEXT_SONG_CHECK      3
#define TM_FADE                 4
#define TM_STOP_PLAYER          5
#define TM_STOP_PLAY_PREVIEW    6
#define TM_CHECKBOX_CLICK       7
#define TM_SCROLL_KEY_PRESSED   8

#define RETRY_A 4
#define RETRY_B 6
//---------------------------------------------------------------------------

#define SONG_PATH_SIZE 4096
#define SONG_NAME_SIZE 1024

#define SPI_GETFOCUSBORDERHEIGHT 0x2010 // SystemParametersInfo
#define R2_XORPEN 7 // SetROP2
//---------------------------------------------------------------------------
// Forward class references...
class TImportForm;
class TExportForm;
class TOFMSDlgForm;
//---------------------------------------------------------------------------
struct STRUCT_A
{
  __int32 player;
  __int32 duration;
  __int32 len_path;
  __int32 len_name;
  __int32 len_artist;
  __int32 len_album;

  char path[SONG_PATH_SIZE];
  char name[SONG_NAME_SIZE];
  char artist[SONG_NAME_SIZE];
  char album[SONG_NAME_SIZE];

  __fastcall STRUCT_A(); // constructor
};
//---------------------------------------------------------------------------
struct STRUCT_B
{
  __int32 player;
  __int32 state;

  __fastcall STRUCT_B(); // constructor
};
//---------------------------------------------------------------------------
class TPlayerURL
{
  public:
    String path, cachePath;
    TColor color;
    bool bDownloaded, bIsUri;
    long cacheNumber; // has the current count of m_NumCachedFiles from FormMain
    TCheckBoxState state; // cbChecked indicates a playing song
    int listIndex;
};
//---------------------------------------------------------------------------
class TPlaylistForm : public TForm
{
__published:  // IDE-managed Components
  TTimer *GeneralPurposeTimer;
  TTimer *FlashTimer;
  TTimer *PositionTimer;
  TPopupMenu *PopupMenu1;
  TMenuItem *ClearList;
  TMenuItem *CheckAll;
  TMenuItem *UncheckAll;
  TPopupMenu *PopupMenu2;
  TMenuItem *EditMode1;
  TMenuItem *N1;
  TMenuItem *ExitEditMode1;
  TMenuItem *N2;
  TMenuItem *MoveSelected;
  TMenuItem *DeleteAllSelected;
  TMenuItem *RandomizeList1;
  TMenuItem *CopySelected;
  TMenuItem *CopyTagsToClipboard;
  TMenuItem *SelectAllItems;
  TMenuItem *CopyLinkToClipboard;
  TMenuItem *SearchandUncheck1;
  TCheckListBox *CheckBox;
  TTimer *MouseMoveDebounceTimer;
  TMenuItem *MenuScrollSelectedIntoView;
  TMenuItem *MenuSort;
  TMenuItem *SubmenuSortAlbum;
  TMenuItem *SubmenuSortArtist;
  TMenuItem *SubmenuSortTitle;
  TMenuItem *SubmenuSortByTrailingNumbers;
  TMenuItem *MenuScrollPlayingIntoView;
  TMenuItem *Delete1;
  TMenuItem *SubmenuDeleteEven;
  TMenuItem *SubmenuDeleteOdd;
  TMenuItem *SubmenuDeleteSelected;
  TMenuItem *SubmenuDeleteDups;
  void __fastcall GeneralPurposeTimerEvent(TObject *Sender);
  void __fastcall FlashTimerEvent(TObject *Sender);
  void __fastcall FormHide(TObject *Sender);
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall FormDeactivate(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall PositionTimerEvent(TObject *Sender);
  void __fastcall CheckBoxClick(TObject* Sender);
  void __fastcall CheckBoxClickCheck(TObject *Sender);
  void __fastcall CheckBoxDblClick(TObject *Sender);
  void __fastcall CheckBoxMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall CheckBoxContextPopup(TObject *Sender, TPoint &MousePos, bool &Handled);
  void __fastcall CheckBoxDragDrop(TObject *Sender, TObject *Source, int X, int Y);
  void __fastcall CheckBoxDragOver(TObject *Sender, TObject *Source, int X, int Y, TDragState State, bool &Accept);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall ClearListClick(TObject *Sender);
  void __fastcall CheckAllClick(TObject *Sender);
  void __fastcall UncheckAllClick(TObject *Sender);
  void __fastcall EditModeClick(TObject *Sender);
  void __fastcall ExitEditModeClick(TObject *Sender);
  void __fastcall DeleteAllSelectedClick(TObject *Sender);
  void __fastcall RandomizeList1Click(TObject *Sender);
  void __fastcall CopySelectedClick(TObject *Sender);
  void __fastcall CopyTagsToClipboardClick(TObject *Sender);
  void __fastcall FormDestroy(TObject *Sender);
  void __fastcall SelectAllItemsClick(TObject *Sender);
  void __fastcall CopyLinkToClipboardClick(TObject *Sender);
  void __fastcall MoveSelectedClick(TObject *Sender);
  void __fastcall OpenStateChange(WMPOpenState NewState);
  void __fastcall PlayStateChange(WMPPlayState NewState);
  void __fastcall PositionChange(double oldPosition, double newPosition);
  void __fastcall MediaError(LPDISPATCH Item);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall SearchandUncheckClick(TObject *Sender);
  void __fastcall MouseMoveDebounceTimerEvent(TObject *Sender);
  void __fastcall CheckBoxMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
  void __fastcall FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall CheckBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall MenuScrollSelectedIntoViewClick(TObject *Sender);
  void __fastcall SubmenuSortAlbumClick(TObject *Sender);
  void __fastcall SubmenuSortArtistClick(TObject *Sender);
  void __fastcall SubmenuSortTitleClick(TObject *Sender);
  void __fastcall SubmenuSortByTrailingNumbersClick(TObject *Sender);
  void __fastcall SubmenuDeleteEvenClick(TObject *Sender);
  void __fastcall SubmenuDeleteOddClick(TObject *Sender);
  void __fastcall SubmenuDeleteSelectedClick(TObject *Sender);
  void __fastcall SubmenuDeleteDupsClick(TObject *Sender);
  void __fastcall MenuScrollPlayingIntoViewClick(TObject *Sender);


private:  // User declarations

  void __fastcall MySort(int iSortType);
  bool __fastcall IsItemVisible(int idx);
  void __fastcall StartPlayPreview(void);
  bool __fastcall QueueToIndex(int idx);
  String __fastcall GetTags(TPlayerURL* p);
  int __fastcall GetTrailingDigits(String s, String &sLeadingPath, String &sExt);
  int __fastcall IndexOfSmallestNumber(TStringList* sl);
  String __fastcall GetMediaTags(void);
  void __fastcall MyMoveSelected(TCheckListBox* DestList, TCheckListBox* SourceList, int x=-1, int y=-1);
//  bool __fastcall InsertNewDeleteOld(TCheckListBox* SourceList,
//                TCheckListBox* DestList, int SourceIndex, int &DestIndex );
  bool __fastcall IsStateUnchecked(TCheckListBox* clb, int idx);
  bool __fastcall IsStateChecked(TCheckListBox* clb, int idx);
  bool __fastcall IsStateGrayed(TCheckListBox* clb, int idx);
  void __fastcall SetGrayedState(int idx);
  void __fastcall SetCheckedState(int idx);
  void __fastcall SetItemState(int idx);
  void __fastcall ClearCheckState(int idx, bool bRequeueIfRepeatMode=true);
  void __fastcall UpdatePlayerStatus(void);
  bool __fastcall SendToSwiftMix(void * sms, int size, int msg);
  void __fastcall SetTimer(int mode, int time=TIME_50);
  void __fastcall CheckAllItems(void);
  void __fastcall WMListDropFile(TWMDropFiles &Msg);
//  void __fastcall WMVListScroll(TWMScroll &Msg);
  void __fastcall WMSetText(TWMSetText &Msg);

  void __fastcall AddListItem(String s, TPlayerURL* p);
  void __fastcall InsertListItem(int idx, String s, TPlayerURL* p);
  void __fastcall ClearListItems(void);

  int m_TimerMode, m_failSafeCounter;
  bool m_bInhibitFlash;
  bool m_bDoubleClick, m_bCheckClick;
  int m_Duration, m_PrevState;
  bool m_bSkipFilePrompt, m_bOpening;

  // Properties
  TCheckListBox* FCheckBox;
  TPlaylistForm* FOtherForm;
  TWindowsMediaPlayer *FWmp, *FOtherWmp;
  int FNextIdx, FTargetIdx, FPlayIdx, FTempIdx, FOldMouseItemIndex;
  TColor FTextColor;
  bool FEditMode, FPlayPreview;
  bool FPlayerA, FKeySpaceDisable;
  long FCacheCount;

  TOFMSDlgForm* pOFMSDlg;
  TExportForm* pExportDlg;
  TImportForm* pImportDlg;
  TProgressForm* pProgress;

  // Added to intercept a WM_SETTEXT and set unicode window captions
//  Controls::TWndMethod OldWinProc;

protected:

  // property getters
  bool __fastcall GetIsExportDlg(void);
  bool __fastcall GetIsImportDlg(void);
  bool __fastcall GetIsOpenDlg(void);
//  void __fastcall WMMove(TWMMove &Msg);

  // property getters
  int __fastcall GetCount(void);

BEGIN_MESSAGE_MAP
  //add message handler for WM_MOVE
//  VCL_MESSAGE_HANDLER(WM_MOVE, TWMMove, WMMove)
  //add message handler for WM_DROPFILES
  VCL_MESSAGE_HANDLER(WM_DROPFILES, TWMDropFiles, WMListDropFile)
//  MESSAGE_HANDLER(WM_VSCROLL, TWMScroll, WMVListScroll)
  //add message handler for WM_SETEXT (allows UTF-8 title-bar)
  MESSAGE_HANDLER(WM_SETTEXT, TWMSetText, WMSetText)

  // define the message is called when the window is moved WMWindowPosChanging
//  MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, TWMWindowPosChanging, WMWindowPosChanging);
//  MESSAGE_HANDLER(WM_SETTINGCHANGE, TMessage, SettingChanged);
END_MESSAGE_MAP(TForm)

  // Added to intercept a WM_SETTEXT and set unicode window captions
//  void __fastcall CustomMessageHandler(TMessage &msg);

public:  // User declarations

  bool __fastcall UnplayedSongsInList(void);
  void __fastcall DestroyImportDialog(void);
  void __fastcall DestroyExportDialog(void);
  void __fastcall DestroyProgressForm(void);
  void __fastcall DestroyFileDialog(void);
  bool __fastcall QueueFirst(void);
  void __fastcall NextSong(bool bForceStartPlay = false);
  void __fastcall TimeDisplay(int t, int item);
  void __fastcall ClearAndStop(void);
  void __fastcall GetSongInfo(void);
  void __fastcall GetSongInfo(STRUCT_A &sms);
  void __fastcall SetTitle(void);
  String __fastcall GetNext(bool bNoSet = false, bool bEnableRandom = false);
  TImportForm* __fastcall CreateImportDialog(void);
  TExportForm* __fastcall CreateExportDialog(void);
  TOFMSDlgForm* __fastcall CreateFileDialog(void);
  void __fastcall AddListItem(String s);
  TPlayerURL* __fastcall InitTPlayerURL(String s);
  void __fastcall DeleteListItem(int idx, bool bDeleteFromCache=true);
  bool __fastcall RestoreCache(void);
  bool __fastcall IsPlayOrPause(void);
  void __fastcall StopPlayer(void);
  void __fastcall StartPlayer(void);
  void __fastcall StopPlayPreview(void);

  STRUCT_A MediaInfo;

//  __property TCheckListBox* CheckBox = {read = FCheckBox};
  __property int Count = {read = GetCount};
  __property int PlayIdx = {read = FPlayIdx, write = FPlayIdx};
  __property TPlaylistForm* OtherForm = {read = FOtherForm, write = FOtherForm};
  __property TWindowsMediaPlayer* Wmp = {read = FWmp, write = FWmp};
  __property TWindowsMediaPlayer* OtherWmp = {read = FOtherWmp, write = FOtherWmp};
  __property int NextIndex = {read = FNextIdx, write = FNextIdx};
  __property int TargetIndex = {read = FTargetIdx, write = FTargetIdx};
  __property bool PlayerA = {read = FPlayerA, write = FPlayerA};
  __property bool InEditMode = {read = FEditMode};
  __property bool InPlayPreview = {read = FPlayPreview};
  __property bool IsImportDlg = {read = GetIsImportDlg};
  __property bool IsExportDlg = {read = GetIsExportDlg};
  __property bool IsOpenDlg = {read = GetIsOpenDlg};
  __property TColor TextColor = {read = FTextColor};
  __property long CacheCount = {read = FCacheCount, write = FCacheCount};
  __property TProgressForm* Progress = {read = pProgress};
};
//---------------------------------------------------------------------------
extern PACKAGE TPlaylistForm *ListA;
extern PACKAGE TPlaylistForm *ListB;
//---------------------------------------------------------------------------
#endif

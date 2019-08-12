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

#include "..\..\19.0\Imports\WMPLib_OCX.h"
#include "..\..\19.0\Imports\WMPLib_TLB.h"
//---------------------------------------------------------------------------

// Timer times
#define TIME_2000 1000  // used to exit edit mode after up/down/left/right scroll-keys were pressed
#define TIME_300  300  // used to sort out what kind of check-box click we got
#define TIME_100  100  // used to delay for start/stop player

// Timer modes
#define TM_NULL                 0
#define TM_START_PLAYER         1
#define TM_NEXT_PLAYER          2
#define TM_STOP_PLAYER          3
#define TM_CHECKBOX_CLICK       4
#define TM_SCROLL_KEY_PRESSED   5
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
    String cachePath;
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
  TTimer *Timer1;
  TTimer *FlashTimer;
  TTimer *PositionTimer;
  TPopupMenu *PopupMenu1;
  TMenuItem *ClearList;
  TMenuItem *CheckAll;
  TMenuItem *UncheckAll;
  TMenuItem *DeleteSelected1;
  TPopupMenu *PopupMenu2;
  TMenuItem *EditMode1;
  TMenuItem *N1;
  TMenuItem *ExitEditMode1;
  TMenuItem *N2;
  TMenuItem *MoveSelected;
  TMenuItem *DeleteSelected;
  TMenuItem *RemoveDuplicates1;
  TMenuItem *RandomizeList1;
  TMenuItem *CopySelected;
  TMenuItem *CopyTagsToClipboard;
  TMenuItem *SelectAllItems;
  TMenuItem *CopyLinkToClipboard;
  void __fastcall Timer1Timer(TObject *Sender);
  void __fastcall FlashTimerEvent(TObject *Sender);
  void __fastcall FormHide(TObject *Sender);
//  void __fastcall CheckBoxMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
  void __fastcall FormActivate(TObject *Sender);
  void __fastcall FormDeactivate(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall PositionTimerEvent(TObject *Sender);
  void __fastcall CheckBoxClick(TObject* Sender);
  void __fastcall CheckBoxClickCheck(TObject *Sender);
  void __fastcall CheckBoxDblClick(TObject *Sender);
  void __fastcall CheckBoxMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall CheckBoxDragDrop(TObject *Sender, TObject *Source, int X, int Y);
  void __fastcall CheckBoxDragOver(TObject *Sender, TObject *Source, int X, int Y, TDragState State, bool &Accept);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall ClearListClick(TObject *Sender);
  void __fastcall CheckAllClick(TObject *Sender);
  void __fastcall UncheckAllClick(TObject *Sender);
  void __fastcall DeleteSelected1Click(TObject *Sender);
  void __fastcall EditModeClick(TObject *Sender);
  void __fastcall ExitEditModeClick(TObject *Sender);
  void __fastcall DeleteSelectedClick(TObject *Sender);
  void __fastcall RemoveDuplicates1Click(TObject *Sender);
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

private:  // User declarations

  String __fastcall GetMediaTags(void);
  bool __fastcall IsPlayOrPause(TPlaylistForm* f);
  void __fastcall MyMoveSelected(TCheckListBox* DestList, TCheckListBox* SourceList, int x=-1, int y=-1);
//  bool __fastcall InsertNewDeleteOld(TCheckListBox* SourceList,
//                TCheckListBox* DestList, int SourceIndex, int &DestIndex );
  void __fastcall ClearCheckState(int oldtag);
  void __fastcall QueueToIndex(int Index);
  void __fastcall UpdatePlayerStatus(void);
  bool __fastcall SendToSwiftMix(void * sms, int size, int msg);
  void __fastcall StopPlayer(TWindowsMediaPlayer* p);
  void __fastcall StartPlayer(TWindowsMediaPlayer* p);
  void __fastcall SetTimer(int mode, int time);
  void __fastcall CheckAllItems(void);
  void __fastcall WMListDropFile(TWMDropFiles &Msg);
//  void __fastcall WMVListScroll(TWMScroll &Msg);
  void __fastcall WMSetText(TWMSetText &Msg);

  void __fastcall AddListItem(String s, TPlayerURL* p);
  void __fastcall InsertListItem(int idx, String s, TPlayerURL* p);
  void __fastcall ClearListItems(void);

  int m_TimerMode, m_failSafeCounter;
  bool bInhibitFlash;
  bool bDoubleClick, bCheckClick;
  int m_Duration, m_PrevState;
  bool bForceNextPlay, bSkipFilePrompt, bOpening;

  // Properties
  TCheckListBox* FCheckBox;
  TPlaylistForm* FOtherForm;
  TWindowsMediaPlayer *FWmp, *FOtherWmp;
  int FNextIndex, FTargetIndex;
  TColor FTextColor;
  bool FEditMode;
  bool FPlayerA;
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

  void __fastcall DestroyImportDialog(void);
  void __fastcall DestroyExportDialog(void);
  void __fastcall DestroyProgressForm(void);
  void __fastcall DestroyFileDialog(void);
  bool __fastcall QueueFirst(void);
  void __fastcall NextPlayer(bool bForceStartPlay = false);
  void __fastcall TimeDisplay(int t, int item);
  void __fastcall ClearAndStop(void);
  void __fastcall GetSongInfo(void);
  void __fastcall GetSongInfo(STRUCT_A &sms);
  void __fastcall SetTitle(void);
  String __fastcall GetNext(bool bNoSet = false, bool bEnableRandom = false);
  String __fastcall GetNextCheckCache(bool bNoSet = false, bool bEnableRandom = false);
  TImportForm* __fastcall CreateImportDialog(void);
  TExportForm* __fastcall CreateExportDialog(void);
  TOFMSDlgForm* __fastcall CreateFileDialog(void);
  int __fastcall GetPlayTag(void);
  void __fastcall AddListItem(String s);
  TPlayerURL* __fastcall InitTPlayerURL(String s);
  void __fastcall DeleteListItem(int idx, bool bDeleteFromCache=true);
  bool __fastcall RestoreCache(void);

  STRUCT_A MediaInfo;

//  __property TCheckListBox* CheckBox = {read = FCheckBox};
  __property int Count = {read = GetCount};
  __property int PlayTag = {read = GetPlayTag};
  __property TPlaylistForm* OtherForm = {read = FOtherForm, write = FOtherForm};
  __property TWindowsMediaPlayer* Wmp = {read = FWmp, write = FWmp};
  __property TWindowsMediaPlayer* OtherWmp = {read = FOtherWmp, write = FOtherWmp};
  __property int NextIndex = {read = FNextIndex, write = FNextIndex};
  __property int TargetIndex = {read = FTargetIndex, write = FTargetIndex};
  __property bool PlayerA = {read = FPlayerA, write = FPlayerA};
  __property bool InEditMode = {read = FEditMode};
  __property bool IsImportDlg = {read = GetIsImportDlg};
  __property bool IsExportDlg = {read = GetIsExportDlg};
  __property bool IsOpenDlg = {read = GetIsOpenDlg};
  __property TColor TextColor = {read = FTextColor};
  __property long CacheCount = {read = FCacheCount, write = FCacheCount};
  __property TCheckListBox* CheckBox = {read = FCheckBox};
  __property TProgressForm* Progress = {read = pProgress};
};
//---------------------------------------------------------------------------
extern PACKAGE TPlaylistForm *ListA;
extern PACKAGE TPlaylistForm *ListB;
//---------------------------------------------------------------------------
#endif

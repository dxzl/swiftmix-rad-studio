//---------------------------------------------------------------------------
#ifndef MainH
#define MainH
//---------------------------------------------------------------------------
#define VERSION "2.04"
#define FREEWARE_EDITION true
#define DEBUG_ON false // Include a debug console, use MainForm->CWrite("")
//---------------------------------------------------------------------------
#include <Winapi.Windows.hpp>

#include <wininet.h>
#include <shlobj.hpp>

//https://docs.microsoft.com/en-us/windows/win32/menurc/resource-types
// resource types...
#include <Winuser.h>

// this has all the exceptions: EFOpenError, EFilerError etc.
// and the TStringlist and TList! It's in Include\vcl
#include <System.Classes.hpp>
#include <System.StrUtils.hpp>
#include <System.IOUtils.hpp>
#include <System.SysUtils.hpp>
#include <Vcl.Clipbrd.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.OleCtrls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.FileCtrl.hpp>
#include "WMPLib_OCX.h"

#include "Resource.h"
#include "DefaultStrings.h"
#include "CWindowDock.h"
#include "XmlTable.h"
#include "RegHelper.h"
#include "Progress.h"
#include "CopyFiles.h"
#include "CopyUrls.h"
//#include "EnterKey.h"
//#include "LicenseKey.h"
#include "AutoSize.h"
#include "About.h"
#include "FormImportMode.h"
#include "FormExportMode.h"
#include "FormSearchUncheck.h"
#include "SMList.h"
#include "FormOFMSDlg.h"
#include "FormImport.h" // include FormOFDlg.h and SMList.h before this
#include "FormExport.h"
#include "Z_TagLibSource\AudioFiles.hpp"
#include "TagEditForm.h"
#include "Help.h"
//#include "..\..\20.0\Imports\MediaPlayer_OCX.h"
#include "..\..\21.0\Imports\MediaPlayer_TLB.h"
#include "..\..\21.0\Imports\WMPLib_OCX.h"
#include <System.Classes.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.OleCtrls.hpp>
#include <Vcl.StdCtrls.hpp>
//---------------------------------------------------------------------------

#define WEBSITE L"http://www.yahcolorize.com/swiftmix/index.htm"
#define EMAIL L"dxzl@live.com"
#define REGISTRY_KEY L"Software\\Discrete-Time Systems\\MusicMixer\\"
#define FILE_CACHE_PATH1 L"\\Discrete-Time Systems"
#define FILE_CACHE_PATH2 L"\\MusicMixer"

#define MAX_CACHE_FILES 4 // max disk-cache files to keep for each player (so 8 total)
#define DEF_FADE_POINT 5
#define DEF_FADE_RATE 0

// max combination of both TMyFileCopy and TMyUrlCopy objects to allow
#define MAX_TMyFileCopy_OBJECTS 10

// Yahcolorize FindWindow
#define YC_CLASS L"TDTSColor"
#define YC_WINDOW L"YahCoLoRiZe"

// Common file dialogs DLL
#define COMMON_CONTROLS_LIB L"Comctl32.dll"

// Registry entries we save in HKEY_CURRENT_USER
#define SM_REGKEY_FADEPOINT L"FadePoint"
#define SM_REGKEY_FADERATE L"FadeRate"
#define SM_REGKEY_DIR_A L"DefDirA"
#define SM_REGKEY_DIR_B L"DefDirB"
#define SM_REGKEY_VOL_A L"VolA"
#define SM_REGKEY_VOL_B L"VolB"
#define SM_REGKEY_REPEAT_A L"RepeatA"
#define SM_REGKEY_REPEAT_B L"RepeatB"
#define SM_REGKEY_SHUFFLE_A L"ShuffleA"
#define SM_REGKEY_SHUFFLE_B L"ShuffleB"
#define SM_REGKEY_FADERTYPE L"FaderType"
#define SM_REGKEY_FADERMODE L"FaderMode"
#define SM_REGKEY_SENDTIMING L"SendTiming"
#define SM_REGKEY_IMPORTEXT L"ImportExt"
#define SM_REGKEY_EXPORTEXT L"ExportExt"
#define SM_REGKEY_CACHE_MAX_FILES L"CacheMaxFiles"
#define SM_REGKEY_CACHE_ENABLED L"CacheEnabled"

#define SHGFP_TYPE_CURRENT 0 // current value for user, verify it exists
#define SHGFP_TYPE_DEFAULT 1 // default value, may not exist

#define DISKSPACE_MESSAGEBOX_NONE  0
#define DISKSPACE_MESSAGEBOX_YESNO 1
#define DISKSPACE_MESSAGEBOX_OK    2

// Additional FindFirstEx flags
//#define FIND_FIRST_EX_CASE_SENSITIVE 1
//#define FindExInfoStandard 0
//#define FindExInfoBasic 1
//---------------------------------------------------------------------------

#define DVD_SL_MINUS_R_SIZE "4,707,319,808"

// (4.7GB) (4,714,397,696 bytes) (4496MB) DVD
#define DVD_SL_R_12_SIZE "4,700,372,992"

#define DVD_SL_8_SIZE "1,463,386,112"

// (8.5GB) (8,547,991,552 bytes) (8152MB) DVD
#define DVD_DL_12_SIZE "8,543,666,176"

#define CD_74_SIZE "681,984,000"
#define CD_80_SIZE "737,280,000"
#define CD_90_SIZE "829,440,000"
#define CD_99_SIZE "912,384,000"
// (25GB)
#define BLURAY_SL_12_SIZE "25,025,314,816"
// (50GB)
#define BLURAY_DL_12_SIZE "50,050,629,632"
//---------------------------------------------------------------------------

// Standard codes
#define NULLCHAR '\0'
#define TAB '\011'
#define CR '\015' // '\r'
#define LF '\012' // '\n'
#define VT '\013' // Used in RTF text to denote line-breaks (strange!)
#define FF '\014'
#define SPACE '\040'
//---------------------------------------------------------------------------
// Tag value for each WindowsMediaPlayer
#define PLAYER_1 0
#define PLAYER_2 1
//---------------------------------------------------------------------------
// String constants
#define ADD_A_TITLE L"Player A"
#define ADD_B_TITLE L"Player B"

#define EXPORT_FILE L"SwiftMiX"
#define EXPORT_DIR L"SwiftMiX\\"
#define EXPORT_EXT L"wpl"
#define IMPORT_EXT L"wpl"

#define C_NULL '\0'
//---------------------------------------------------------------------------
// Forward class references
//class TPlaylistForm;
//---------------------------------------------------------------------------

class TMainForm : public TForm
{
__published:  // IDE-managed Components

  TWindowsMediaPlayer *WindowsMediaPlayer1;
  TWindowsMediaPlayer *WindowsMediaPlayer2;
  TMainMenu *MainMenu1;
  TMenuItem *Player11;
  TMenuItem *File1;
  TMenuItem *MenuVolumeA;
  TMenuItem *N1;
  TMenuItem *Exit1;
  TMenuItem *VA_25;
  TMenuItem *VA_50;
  TMenuItem *VA_75;
  TMenuItem *VA_100;
  TMenuItem *Player21;
  TMenuItem *File2;
  TMenuItem *MenuVolumeB;
  TMenuItem *VB_25;
  TMenuItem *VB_50;
  TMenuItem *VB_75;
  TMenuItem *VB_100;
  TMenuItem *MenuFaderTypeNormal;
  TMenuItem *N2;
  TMenuItem *MenuAbout;
  TStatusBar *StatusBar1;
  TMenuItem *ClearPlaylistStop1;
  TMenuItem *ClearPlaylistStop2;
  TMenuItem *MenuFaderModeAuto;
  TTimer *AutoFadeTimer;
  TMenuItem *ViewPlaylist1;
  TMenuItem *ViewPlaylist2;
  TMenuItem *MenuHighPriority;
  TMenuItem *N4;
  TMenuItem *MenuForceFade;
  TMenuItem *Player1Next1;
  TMenuItem *Player2Next1;
  TMenuItem *N6;
  TMenuItem *N7;
  TMenuItem *Play1;
  TMenuItem *Stop1;
  TMenuItem *Pause1;
  TMenuItem *N5;
  TMenuItem *N8;
  TMenuItem *Play2;
  TMenuItem *Stop2;
  TMenuItem *Pause2;
  TMenuItem *ImportPlaylist1;
  TMenuItem *ExportPlaylist1;
  TMenuItem *ImportPlaylist2;
  TMenuItem *ExportPlaylist2;
  TMenuItem *MenuHelp;
  TMenuItem *MenuRepeatModeA;
  TMenuItem *N9;
  TMenuItem *MenuRepeatModeB;
  TMenuItem *N10;
  TMenuItem *VA_10;
  TMenuItem *VB_10;
  TMenuItem *N3;
  TMenuItem *MenuExportSongFilesandLists;
  TMenuItem *N11;
  TMenuItem *MenuSendTiming;
  TMenuItem *MenuShuffleModeA;
  TMenuItem *MenuShuffleModeB;
  TMenuItem *MenuViewDiscSpaceRequired;
  TMenuItem *MenuAutoFitToDVDCD;
    TPanel *Panel1;
    TPanel *Panel2;
  TUpDown *FRUpDown;
  TEdit *EditFadeRate;
  TUpDown *FPUpDown;
  TEdit *EditFadePoint;
  TTrackBar *FaderTrackBar;
  TMenuItem *MenuCacheFiles;

  void __fastcall File1Click(TObject *Sender);
  void __fastcall File2Click(TObject *Sender);
  void __fastcall VA_25Click(TObject *Sender);
  void __fastcall VA_50Click(TObject *Sender);
  void __fastcall VA_75Click(TObject *Sender);
  void __fastcall VA_100Click(TObject *Sender);
  void __fastcall VB_25Click(TObject *Sender);
  void __fastcall VB_50Click(TObject *Sender);
  void __fastcall VB_75Click(TObject *Sender);
  void __fastcall VB_100Click(TObject *Sender);
  void __fastcall MenuFaderTypeNormalClick(TObject *Sender);
  void __fastcall FaderTrackBarChange(TObject *Sender);
  void __fastcall Exit1Click(TObject *Sender);
  void __fastcall MenuAboutClick(TObject *Sender);
  void __fastcall ClearPlaylistStop1Click(TObject *Sender);
  void __fastcall ClearPlaylistStop2Click(TObject *Sender);
  void __fastcall MenuFaderModeAutoClick(TObject *Sender);
  void __fastcall AutoFadeTimerEvent(TObject *Sender);
  void __fastcall ViewPlaylist2Click(TObject *Sender);
  void __fastcall ViewPlaylist1Click(TObject *Sender);
  void __fastcall MenuHighPriorityClick(TObject *Sender);
  void __fastcall MenuForceFadeClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall Player1Next1Click(TObject *Sender);
  void __fastcall Player2Next1Click(TObject *Sender);
  void __fastcall Play1Click(TObject *Sender);
  void __fastcall Stop1Click(TObject *Sender);
  void __fastcall Pause1Click(TObject *Sender);
  void __fastcall Play2Click(TObject *Sender);
  void __fastcall Stop2Click(TObject *Sender);
  void __fastcall Pause2Click(TObject *Sender);
  void __fastcall ImportPlaylist1Click(TObject *Sender);
  void __fastcall ExportPlaylist1Click(TObject *Sender);
  void __fastcall ImportPlaylist2Click(TObject *Sender);
  void __fastcall ExportPlaylist2Click(TObject *Sender);
  void __fastcall MenuHelpClick(TObject *Sender);
  void __fastcall MenuRepeatModeAClick(TObject *Sender);
  void __fastcall MenuRepeatModeBClick(TObject *Sender);
  void __fastcall VA_10Click(TObject *Sender);
  void __fastcall VB_10Click(TObject *Sender);
  void __fastcall MenuExportSongFilesandListsClick(TObject *Sender);
  void __fastcall WindowsMediaPlayer1OpenStateChange(TObject *Sender, long NewState);
  void __fastcall WindowsMediaPlayer2OpenStateChange(TObject *Sender, long NewState);
  void __fastcall WindowsMediaPlayer1PlayStateChange(TObject *Sender, long NewState);
  void __fastcall WindowsMediaPlayer2PlayStateChange(TObject *Sender, long NewState);
  void __fastcall WindowsMediaPlayer1PositionChange(TObject *Sender, double oldPosition, double newPosition);
  void __fastcall WindowsMediaPlayer2PositionChange(TObject *Sender, double oldPosition, double newPosition);
  void __fastcall MenuSendTimingClick(TObject *Sender);
  void __fastcall MenuShuffleModeAClick(TObject *Sender);
  void __fastcall MenuShuffleModeBClick(TObject *Sender);
  void __fastcall WindowsMediaPlayer1MediaChange(TObject *Sender, LPDISPATCH Item);
  void __fastcall WindowsMediaPlayer2MediaChange(TObject *Sender, LPDISPATCH Item);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall MenuViewDiscSpaceRequiredClick(TObject *Sender);
  void __fastcall MenuAutoFitToDVDCDClick(TObject *Sender);
  void __fastcall FormDestroy(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall MenuCacheFilesClick(TObject *Sender);
  void __fastcall StatusBar1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
  void __fastcall FRUpDownChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction);
  void __fastcall FPUpDownChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction);
  void __fastcall WindowsMediaPlayer2MediaError(TObject *Sender, LPDISPATCH Item);
  void __fastcall WindowsMediaPlayer1MediaError(TObject *Sender, LPDISPATCH Item);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall EditFadePointChange(TObject *Sender);

private:  // User declarations
#if DEBUG_ON
  void __fastcall CInit(void);
#endif
  void __fastcall ProcessFileItem(TPlaylistForm* f, String s);
  String __fastcall GetShortcutTarget(String file);
  bool __fastcall UriIsDirectory(String sUri);
  int __fastcall MyFileCopy(TPlaylistForm* f, String &sDestDir, int idx);
  void __fastcall PromptRetry(void);
  int __fastcall CopyMusicFiles(TPlaylistForm* f, String sUserDir);
  long __fastcall MyGFS(String sPath); // MyGetFileSize retry-portion
  int __fastcall AutoFitToDVD(__int64 lBytesNeeded);
  String __fastcall DirectoryDialogW(String sInitialDir, String sTitle);
  bool __fastcall IsWinVistaOrHigher(void);
  void __fastcall ErrorCode(int Code);
  void __fastcall RecurseFileAdd(TPlaylistForm* f, TStringList* slFiles);
  void __fastcall FindFirstNextToStringLists(TStringList* slFiles, TStringList* slDirs);
  bool __fastcall IsPlaylistPath(String sSourcePath);
  bool __fastcall IsAudioFile(String sSourcePath);
  bool __fastcall IsPlaylistExtension(String sExt);
  String __fastcall MyExtractFileExt(String sPath);
  bool __fastcall SetVolumes(void);
  bool __fastcall SetVolumeA(int v);
  bool __fastcall SetVolumeA(void);
  bool __fastcall SetVolumeB(int v);
  bool __fastcall SetVolumeB(void);
  bool __fastcall SetVolumeAndCheckmarkA(int v);
  bool __fastcall SetVolumeAndCheckmarkB(int v);
  bool __fastcall SetCheckmarkA(int v);
  bool __fastcall SetCheckmarkB(int v);
  __int64 __fastcall ComputeDiskSpace(int Mode);
  __int64 __fastcall RandomRemove(__int64 TargetBytes);
  void __fastcall WMDropFile(TWMDropFiles &Msg);
  bool __fastcall DeleteDirAndFiles(String sDir);
  void __fastcall InitRegistryVars(void);
  bool __fastcall InitFileCaching(void);

  // property vars (mostly!)
  int FfadePoint, FfadeRate;;
  int FvolA, FvolB;
  int FcurrentVolA, FcurrentVolB; // volume as it transitions during a fade

  int FMaxCacheFiles; // volumes set in registry

  // Flags for registry settings
  bool bTypeCenterFade, bModeManualFade, bSendTiming;
  bool bRepeatModeA, bShuffleModeA, bRepeatModeB, bShuffleModeB;
  bool bFileCacheEnabled, bVistaOrHigher;

  // Globals for Trial-Version
  bool bTrialFeaturesEnabled;
  int CurrentPlayer;

  TPlaylistForm* GPlaylistForm;

  String FsDeskDir, FsCacheDir;
  String FsImportExt, FsExportExt; // Import/Export file-extension (UTF-8)
  String FsSaveDirA, FsSaveDirB; // open/save directory (UTF-8)

  bool FBypassFilters;
  unsigned FFilesAddedCount;

  // used for MoveFiles.cpp
  TList* pTMyFileCopyList; // list of TMyFileCopyList objects currently active
  TList* pTFailedToCopyList; // list of TMyFileCopyList objects that failed to copy

//  CWindowDock* FDock;

protected:

//  void __fastcall WMMove(TWMMove &Msg);

//  void __fastcall WndProc(TMessage &Message);

BEGIN_MESSAGE_MAP
  //add message handler for WM_DROPFILES
  // NOTE: All of the TWM* types are in messages.hpp!
  VCL_MESSAGE_HANDLER(WM_DROPFILES, TWMDropFiles, WMDropFile)
//  VCL_MESSAGE_HANDLER(WM_NOTIFY, TOFNotify, WMOFNotify)
  //add message handler for WM_MOVE
//  VCL_MESSAGE_HANDLER(WM_MOVE, TWMMove, WMMove)
END_MESSAGE_MAP(TForm)

public:    // User declarations

  __fastcall TMainForm(TComponent* Owner);

#if DEBUG_ON
  void __fastcall CWrite(String S);
#endif

  bool __fastcall GetShortcut(String &sPath, bool &bIsDirectory);
  FARPROC __fastcall LoadProcAddr(HINSTANCE hDll, String entry);
  long __fastcall MyGetFileSize(String sPath);
  int __fastcall ShowFailures(void);
  void __fastcall ClearFailedToCopyList(void);
  void __fastcall AddFailure(String sPath, int iIndex);
  void __fastcall AddFailure(String sSource, String sDest, int iSource, int iList);
  bool __fastcall IsSourcePathUri(String sIn);
  bool __fastcall IsUri(String sIn);
  bool __fastcall IsFileUri(String sIn);
  void __fastcall ShowPlaylist(TPlaylistForm* f);
  String __fastcall SetFlag(String S, int f);
  bool __fastcall ForceFade(void);
  bool __fastcall SetCurrentPlayer(void);
  void __fastcall RestoreFocus(void);
  bool __fastcall FileDialog(TPlaylistForm* f, String &d, String t);
  void __fastcall LoadListWithDroppedFiles(TPlaylistForm* f, TWMDropFiles &Msg);
  bool __fastcall AddFileToListBox(TPlaylistForm* f, String sFile);
  int __fastcall AddDirToListBox(TPlaylistForm* f, String sPath);
//  String __fastcall WideToUtf8U(WideString wIn);
//  WideString __fastcall Utf8ToWide(String sIn);

  AnsiString __fastcall WideToUtf8Ansi(WideString wIn);

  // helper functopns for FormOFMSDlg (to keep it compatible with old C++ Builder 4)
  WideString __fastcall GetCurrentDirW(void);
  bool __fastcall DirectoryExistsW(WideString wIn);
  bool __fastcall FileExistsW(WideString wIn);
  bool __fastcall IsUriW(WideString wIn);
  WideString __fastcall Utf8ToWide(String sIn);
  String __fastcall WideToUtf8(WideString wIn);

  bool __fastcall WriteStringToFile(String sPath, String sInfo);
  String __fastcall GetSpecialFolder(int csidl);
  bool __fastcall ShellCommand(String sVerb, String sFile, String sCmd, bool bWaitForCompletion);
  bool __fastcall CopyFileToCache(TPlaylistForm* f, int idx);
  bool __fastcall DeleteCacheFile(TPlaylistForm* f, long cacheNumber=0);
  String __fastcall GetURL(TCheckListBox* l, int idx);
  bool __fastcall ReleaseForm(TForm* f);
  bool __fastcall ReleaseFormNoClose(TForm* f);

  int RWM_SwiftMixPlay, RWM_SwiftMixTime, RWM_SwiftMixState;
  bool bFadeRight, bAutoSizePrompt;

  // properties
//  __property CWindowDock* GDock = {read = FDock, write = FDock};
  __property String ImportExt = {read = FsImportExt, write = FsImportExt};
  __property String ExportExt = {read = FsExportExt, write = FsExportExt};
  __property String SaveDirA = {read = FsSaveDirA, write = FsSaveDirA};
  __property String SaveDirB = {read = FsSaveDirB, write = FsSaveDirB};
  __property String CacheDir = {read = FsCacheDir};
  __property String DeskDir = {read = FsDeskDir};
  __property bool GBypassFilters = {read = FBypassFilters, write = FBypassFilters, default =  false};
  __property bool VistaOrHigher = {read = bVistaOrHigher};
  __property bool ManualFade = {read = bModeManualFade};
  __property bool CenterFade = {read = bTypeCenterFade};
  __property bool SendTiming = {read = bSendTiming};
  __property bool RepeatModeA = {read = bRepeatModeA};
  __property bool RepeatModeB = {read = bRepeatModeB};
  __property bool ShuffleModeA = {read = bShuffleModeA};
  __property bool ShuffleModeB = {read = bShuffleModeB};
  __property bool CacheEnabled = {read = bFileCacheEnabled, write = bFileCacheEnabled};
  __property int VolA = {read = FvolA};
  __property int VolB = {read = FvolB};
  __property int FadePoint = {read = FfadePoint};
  __property int FadeRate = {read = FfadeRate};
  __property unsigned FilesAddedCount = {read = FFilesAddedCount};
  __property int MaxCacheFiles = {read = FMaxCacheFiles};
  __property TList* MyFileCopyList = {read = pTMyFileCopyList};
  __property TList* FailedToCopyList = {read = pTFailedToCopyList};
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class TFailVars
{
  public:
    String m_sSource, m_sDest;
    int m_iSource, m_iList;
    int m_iRetryCount;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
extern const char Trial_Key[];

typedef HRESULT __stdcall (*tGetFolderPath)(HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPTSTR pszPath);
//---------------------------------------------------------------------------
#endif

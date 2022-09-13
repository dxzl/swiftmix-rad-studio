//---------------------------------------------------------------------------
#ifndef PlaylistHelperH
#define PlaylistHelperH

#include <Windows.h>
#include <Registry.hpp>
#include <Classes.hpp>
//---------------------------------------------------------------------------
// have to redefine this due to an ambiguity with Wmplib_tlb
//#if defined(_WIN64)
// typedef unsigned long long ULONG_PTR;
//#else
// typedef unsigned long ULONG_PTR;
//#endif
 //---------------------------------------------------------------------------
class TPlaylistHelper
{
private:
  void __fastcall ProcessFileItem(TPlaylistForm *f, String s);

protected:
public:
  __fastcall TPlaylistHelper(void);
  virtual __fastcall ~TPlaylistHelper();

  void __fastcall ShowPlaylist(TPlaylistForm *f);
  int __fastcall MyFileCopy(TPlaylistForm *f, String &sDestDir,
                                       int idx, bool bIsCacheFile=false);
  int __fastcall CopyMusicFiles(TPlaylistForm *f, String sUserDir);
  void __fastcall RecurseFileAdd(TPlaylistForm *f, TStringList *slFiles);
  String __fastcall GetURL(TPlaylistForm *f, int listIndex);
  bool __fastcall FileDialog(TPlaylistForm *f, String &d, String t);
  void __fastcall LoadListWithDroppedFiles(TPlaylistForm *f, TWMDropFiles &Msg);
  bool __fastcall AddFileToListBox(TPlaylistForm *f, String sFile);
  int __fastcall AddDirToListBox(TPlaylistForm *f, String sPath);
};
//---------------------------------------------------------------------------
extern TPlaylistHelper *pPH;
//---------------------------------------------------------------------------
#endif

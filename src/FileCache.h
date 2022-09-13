//---------------------------------------------------------------------------
#ifndef FileCacheH
#define FileCacheH

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
class TFileCache
{
private:
  bool __fastcall IsPointerInCache(TPlaylistForm *f, TPlayerURL *pNew);
  void __fastcall RenumberCacheList(TPlaylistForm *f);

protected:
public:
  __fastcall TFileCache(void);
  virtual __fastcall ~TFileCache();

  // file-cache
  bool __fastcall AddCacheEntry(TPlaylistForm *f, String sDestPath, int idx);
  void __fastcall DeleteCacheEntry(TPlaylistForm *f, int cacheNumber=0);
  bool __fastcall DeleteCacheFile(TPlaylistForm *f, int cacheNumber=0);
  void __fastcall DeleteAllCacheFiles(TPlaylistForm *f);
  String __fastcall GetCachePath(TPlaylistForm *f, TPlayerURL *p);
  bool __fastcall CopyFileToCache(TPlaylistForm *f, int idx);
};
//---------------------------------------------------------------------------
extern TFileCache *pFC;
//---------------------------------------------------------------------------
#endif

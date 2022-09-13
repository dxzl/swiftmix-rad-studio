//---------------------------------------------------------------------------
#include <vcl.h>
#include "Main.h"
#pragma hdrstop

#include "FileCache.h"

#pragma package(smart_init)
//---------------------------------------------------------------------------
//fyi:
//HKEY subKey = nullptr;
//LONG result = RegOpenKeyEx(key, subPath.c_str(), 0, KEY_READ, &subKey);
//if (result == ERROR_SUCCESS) - key exists!
//---------------------------------------------------------------------------
//  constructor
__fastcall TFileCache::TFileCache(void)
{
}
//------------------- destructor -------------------------
__fastcall TFileCache::~TFileCache()
{
}
//---------------------------------------------------------------------------
String __fastcall TFileCache::GetCachePath(TPlaylistForm *f, TPlayerURL *p)
{
  if (!p || !f->CacheList || p->cacheNumber <= 0 || p->cacheNumber > f->CacheList->Count)
    return "";
  int idx = p->cacheNumber-1;
  TPlayerURL *pCache = (TPlayerURL*)f->CacheList->Objects[idx];
  String sCacheFile = f->CacheList->Strings[idx];
  if (sCacheFile.IsEmpty() || !pCache){
    DeleteCacheEntry(f, idx);
    return "";
  }

  String sCachePath = IncludeTrailingPathDelimiter(f->CachePath) + sCacheFile;

  if (!FileExists(sCachePath)){
    DeleteCacheEntry(f, idx);
    return "";
  }

  return sCachePath;
}
//---------------------------------------------------------------------------
void __fastcall TFileCache::DeleteAllCacheFiles(TPlaylistForm *f)
{
  if (f->CacheList && f->CacheList->Count){
    for (int ii = f->CacheList->Count; ii > 0; ii--)
      DeleteCacheFile(f, ii); // FYI: we pass the actual 0-based list-index + 1
    f->UniqueNumber = 0;
  }
}
//---------------------------------------------------------------------------
// returns true if success
// cacheNumber defaults to 0 (delete oldest cache file)
// Set cacheNumber > 0 to delete a specific cached file
//
// NOTE: MainForm->CacheList is a TStringList *of up to MAX_CACHE_FILES.
// Each entry has a TPlayerURL *that's a copy of the TObject *for a song in a list.
// The Strings attribute has the cache filename. We delete the oldest cached
// file by deleting the first item in the TStringList and its associated file.
// Each filename has a unique number prepended to it from an ever incrementing
// long int (make number base 52???).
bool __fastcall TFileCache::DeleteCacheFile(TPlaylistForm *f, int cacheNumber)
{
  bool bRet = true;

  try
  {
    if (f->CacheList && f->CacheList->Count > 0 && cacheNumber >= 0 &&
                                         cacheNumber <= f->CacheList->Count)
    {
      int idx;
      if (cacheNumber == 0)
        idx = 0; // delete oldest cache-file
      else
        idx = cacheNumber-1;

      TPlayerURL *pCache = (TPlayerURL*)f->CacheList->Objects[idx];
      String sCacheFile = f->CacheList->Strings[idx];
      if (sCacheFile.IsEmpty() || !pCache){
#if DEBUG_ON
        MainForm->CWrite( "\r\nTFileCache::DeleteCacheFile failed! \"" + sCacheFile + "\"\r\n");
#endif
        bRet = false;
      }
      else{
        String sCachePath = IncludeTrailingPathDelimiter(f->CachePath) + sCacheFile;
        if (FileExists(sCachePath)){
          if (!DeleteFile(sCachePath)){
#if DEBUG_ON
            MainForm->CWrite( "\r\nTFileCache::DeleteCacheFile DeleteFile failed! \"" + sCachePath + "\"\r\n");
#endif
            bRet = false;
          }
        }
#if DEBUG_ON
        else{
          MainForm->CWrite( "\r\nTFileCache::DeleteCacheFile FILE NOT FOUND! \"" + sCachePath + "\"\r\n");
        }
#endif
      }
      DeleteCacheEntry(f, idx+1); // need to pass 1-based index (0=delete oldest)
    }
  }
  catch(...) { bRet = false; }

  return bRet;
}
//---------------------------------------------------------------------------
// delete item from CacheList
// pass cacheNumber 0 to delete oldest
// pass 1 to delete first item in list
// pass CacheList->Count to delete last item in list
void __fastcall TFileCache::DeleteCacheEntry(TPlaylistForm *f, int cacheNumber){
  if (!f || !f->CacheList)
    return;

  int count = f->CacheList->Count;

  if (count > 0 && cacheNumber >= 0 && cacheNumber <= count){
    int idx;
    if (cacheNumber == 0)
      idx = 0; // delete oldest cache-file
    else
      idx = cacheNumber-1;

    int iSaveCacheNum = cacheNumber;

    TPlayerURL *p = (TPlayerURL*)f->CacheList->Objects[idx];
    if (p)
      p->cacheNumber = 0;

    f->CacheList->Delete(idx);

    if (iSaveCacheNum != count)
      RenumberCacheList(f);
  }
}
//---------------------------------------------------------------------------
// Extracts the filename from sDest and adds it to a playlist's
// CacheList. If success, returns true.
bool __fastcall TFileCache::AddCacheEntry(TPlaylistForm *f,
                                                String sDestPath, int idx){
  bool bRet = false;

  // handle a cache-file write's special considerations...
  if (f && idx >= 0 && idx < f->Count){
    TPlayerURL *p = (TPlayerURL*)f->CheckBox->Items->Objects[idx];
    if (p){
      String sFileName = MainForm->MyExtractFileName(sDestPath);

      if (!sFileName.IsEmpty()){

        if (f->CacheList->Count >= MAX_CACHE_FILES)
          DeleteCacheFile(f); // delete oldest cached file
        f->CacheList->AddObject(sFileName, (TObject*)p);
        bRet = true;
      }
    }
  }

#if DEBUG_ON
  if (bRet)
    MainForm->CWrite( "\r\nTFileCache::AddCacheEntry cache-write success! \"" + sDestPath + "\"\r\n");
  else
    MainForm->CWrite( "\r\nTFileCache::AddCacheEntry cache-write file-empty! \"" + sDestPath + "\"\r\n");
#endif

  return bRet;
}
//---------------------------------------------------------------------------
// after deleting a CacheList item we need to renumber all the
// TPlayerURL *cachNumber fields.
void __fastcall TFileCache::RenumberCacheList(TPlaylistForm *f){
  for (int ii = 0; ii < f->CacheList->Count;){
    TPlayerURL *p = (TPlayerURL*)f->CacheList->Objects[ii];
    if (p){
      p->cacheNumber = ++ii;
#if DEBUG_ON
      MainForm->CWrite( "\r\nTFileCache::RenumberCacheList(): \"" +
         f->CacheList->Strings[ii] + "\" " + String(p->cacheNumber) + "\r\n");
#endif
    }
    else
      f->CacheList->Delete(ii);
  }
}
//---------------------------------------------------------------------------
bool __fastcall TFileCache::IsPointerInCache(TPlaylistForm *f, TPlayerURL *pNew){
  for (int ii = 0; ii < f->CacheList->Count; ii++){
    TPlayerURL *pOld = (TPlayerURL*)f->CacheList->Objects[ii];
    if (pOld && pOld == pNew)
      return true;
  }
  return false;
}
//---------------------------------------------------------------------------
// set idx -1 to reference the last item in the list
// returns false if list empty or error
bool __fastcall TFileCache::CopyFileToCache(TPlaylistForm *f, int idx)
{
  bool bRet = false;

  if (!f || f->CheckBox->Count == 0)
    return false;

  if (idx < 0)
    idx = f->CheckBox->Count-1;

  try
  {
    // prefetch the next file into our temporary cache directory
    if (MainForm->CacheEnabled && idx >= 0 && idx < f->CheckBox->Count)
    {
      // add a new file to the cache
      TPlayerURL *p = (TPlayerURL*)f->CheckBox->Items->Objects[idx];

      if (!p)
        return false;

      // here, we avoid a headache for a large music file that's already in the
      // process of being transferred but might not have completed.
      // So just return "success".
      //
      // We might have a problem if for any reason, the xfer fails - say a web
      // xfer via URL - then we will think we have a cache file but NOT have it.
      //
      // We might need to monitor a media-load failure (see OpenStateChange()
      // in SMList.cpp) and go back to the original url, and erase the cache
      // entry. [NOTE: this is now being done - failsafe monitoring!]
      if (p->cacheNumber == -1) // return true if cache file-write already pending
        return true;

      // return true if it's already in the cache
      if (IsPointerInCache(f, p))
        return true;

      // flag to CopyFiles.cpp or CopyURLs.cpp that this is a cache-file write
      // and we need to extract the filename and write it to the playlist
      // form's CacheList. Also connotes "pending write" status...
      p->cacheNumber = -1; // set pending cache-write flag

      // pass in f->CachePath - returns full write path (including
      // filename), by reference!
      String sDestPath = f->CachePath;
      int retCode = pPH->MyFileCopy(f, sDestPath, idx, true); // set bIsCacheFile true!
      if (retCode < 0)
      {
#if DEBUG_ON
        MainForm->CWrite( "\r\nTFileCache::MyFileCopy() error: " + String (retCode) + "\r\n");
#endif
        return false;
      }
    }
  }
  catch(...) {}

  return bRet;
}
//---------------------------------------------------------------------------

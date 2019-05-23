//---------------------------------------------------------------------------
#ifndef RegHelperH
#define RegHelperH

#include <Windows.h>
#include <Registry.hpp>
#include <Classes.hpp>
//---------------------------------------------------------------------------
// have to redefine this due to an ambiguity with Wmplib_tlb
#if  defined(_WIN64)
 typedef unsigned long long ULONG_PTR;
 #else
 typedef unsigned long ULONG_PTR;
 #endif
 //---------------------------------------------------------------------------
class TRegHelper
{
private:
  TRegistry* MyReg;

  bool bOpened;

protected:
public:
  __fastcall TRegHelper(bool bCreate);
  virtual __fastcall ~TRegHelper();

  void __fastcall ReadSetting(String keyName, bool &bVal, bool bDef);
  void __fastcall ReadSetting(String keyName, int &iVal, int iDef);
  String __fastcall ReadSetting(String keyName);
  void __fastcall WriteSetting(String keyName, bool bVal);
  void __fastcall WriteSetting(String keyName, int iVal);
  void __fastcall WriteSetting(String keyName, String sIn);
};
//---------------------------------------------------------------------------
#endif

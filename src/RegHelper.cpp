//---------------------------------------------------------------------------
#include <vcl.h>
#include "Main.h"
#pragma hdrstop

//#include <winreg.h>
#include "RegHelper.h"

#pragma package(smart_init)
//---------------------------------------------------------------------------
//fyi:
//HKEY subKey = nullptr;
//LONG result = RegOpenKeyEx(key, subPath.c_str(), 0, KEY_READ, &subKey);
//if (result == ERROR_SUCCESS) - key exists!
//---------------------------------------------------------------------------
//  constructor
__fastcall TRegHelper::TRegHelper(bool bCreate)
{
  MyReg = new TRegistry();

  if (MyReg == NULL)
  {
#if DEBUG_ON
    MainForm->CWrite( "\r\nUnable to create TMyRegistry()!\r\n");
#endif
    bOpened = false;
    return;
  }

  if (!MyReg->OpenKey(REGISTRY_KEY, bCreate))
  {
#if DEBUG_ON
    MainForm->CWrite( "\r\nUnable to open MyRegistry()!\r\n");
#endif
    bOpened = false;
    return;
  }

  // this is not working due to a pointer conflict
  //MyReg->RootKey = HKEY_CURRENT_USER;

  bOpened = true;
}
//------------------- destructor -------------------------
__fastcall TRegHelper::~TRegHelper()
{
  try
  {
    if (MyReg != NULL)
    {
      if (bOpened)
        MyReg->CloseKey();

      delete MyReg;
    }
  }
  catch(...) {}
}
//---------------------------------------------------------------------------
String __fastcall TRegHelper::ReadSetting(String keyName)
{
  String sOut;

  // Read key from registry at HKEY_CURRENT_USER
  if (!bOpened)
    return "";

  try
  {
    sOut = MyReg->ReadString(keyName);
  }
  catch (...)
  {
    sOut = "";
  }

  return sOut;
}
//---------------------------------------------------------------------------
void __fastcall TRegHelper::ReadSetting(String keyName, bool &bVal, bool bDef)
{
  // Read key from registry at HKEY_CURRENT_USER
  if (!bOpened)
  {
    bVal = bDef;
    return;
  }

  try
  {
    bVal = MyReg->ReadBool(keyName);
  }
  catch(...)
  {
    bVal = bDef;
  }
}
//---------------------------------------------------------------------------
void __fastcall TRegHelper::ReadSetting(String keyName, int &iVal, int iDef)
{
  // Read key from registry at HKEY_CURRENT_USER
  if (!bOpened)
  {
    iVal = iDef;
    return;
  }

  try
  {
    iVal = MyReg->ReadInteger(keyName);
  }
  catch(...)
  {
    iVal = iDef;
  }
}
//---------------------------------------------------------------------------
void __fastcall TRegHelper::WriteSetting(String keyName, String sIn)
{
  // Read key from registry at HKEY_CURRENT_USER
  if (!bOpened)
    return;

  try
  {
    MyReg->WriteString(keyName, sIn);
  }
  catch(...) {}
}
//---------------------------------------------------------------------------
void __fastcall TRegHelper::WriteSetting(String keyName, bool bVal)
{
  // Read key from registry at HKEY_CURRENT_USER
  if (!bOpened)
    return;

  try
  {
    MyReg->WriteBool(keyName, bVal);
  }
  catch(...) {}
}
//---------------------------------------------------------------------------
void __fastcall TRegHelper::WriteSetting(String keyName, int iVal)
{
  // Read key from registry at HKEY_CURRENT_USER
  if (!bOpened)
    return;

  try
  {
    MyReg->WriteInteger(keyName, iVal);
  }
  catch(...) {}
}
//---------------------------------------------------------------------------


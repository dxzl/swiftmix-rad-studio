//---------------------------------------------------------------------------
#include <vcl.h>
#include "Main.h"
#pragma hdrstop

#include <math.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAboutForm *AboutForm;
//---------------------------------------------------------------------------
__fastcall TAboutForm::TAboutForm(TComponent* Owner)
  : TForm(Owner)
{
  Panel1->Color = TColor(0xd3902c);

  Label1->Font->Color = TColor(0xc04f859);

  Label1->Caption = "SwiftMiX by\n"
            "Discrete-Time Systems,\n"
            "Mister Swift, " + String(Date().DateString()) + "\n"
            + String(EMAIL);

  RevLabel->Caption = String(VERSION);
//  RevLabel->Caption = String(VERSION) + "." + String(DEF_PRODUCT_ID) +
//                            "." + String(DEF_SUPER_REV);
}
//---------------------------------------------------------------------------
__fastcall TAboutForm::~TAboutForm()
{
}
//---------------------------------------------------------------------------
void __fastcall TAboutForm::FormDestroy(TObject *Sender)
{
#if DEBUG_ON
  MainForm->CWrite( "\r\nFormDestroy() in TAboutForm()!\r\n");
#endif
}
//---------------------------------------------------------------------------
void __fastcall TAboutForm::Button1Click(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TAboutForm::Button2Click(TObject *Sender)
{
#if (FREEWARE_EDITION == false)
  try
    {
    TLicenseKey * lk = new TLicenseKey();
    bool bOK = lk->DoKey( false ); // no message on cancel

    if ( !bOK )
      bOK = lk->ValidateLicenseKey( false ); // Re-evaluate old key

    delete lk;

    if ( bOK )
      DisplayDaysRemaining();
    else
      ShowMessage(KEYSTRINGS[18]);
    }
  catch(...)
    {
    ShowMessage("Critical Error in About Dialog");
    Application->Terminate();
    }
#else
  ShowMessage( "This edition is free." );
#endif
}
//---------------------------------------------------------------------------
void __fastcall TAboutForm::WebSiteClick(TObject *Sender)
{
  // launch internet explorer
//  ShellExecute(Handle, L"open", L"iexplore.exe", WEBSITE, NULL, SW_SHOW);
  ShellExecute(NULL, L"open", WEBSITE, NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------
void __fastcall TAboutForm::FormShow(TObject *Sender)
{
  DisplayDaysRemaining();
}
//---------------------------------------------------------------------------
void __fastcall TAboutForm::DisplayDaysRemaining( void )
{
  AnsiString S;

#if (FREEWARE_EDITION == false)
  int DaysRem = PK->ComputeDaysRemaining();

  // -100 if failure
  if ( DaysRem >= 0 || DaysRem > LK_DISPLAY_UNLIMITTED_DAYS )
    {
    S = String(KEYSTRINGS[0]);

    if ( DaysRem >= LK_DISPLAY_UNLIMITTED_DAYS )
      S += "(No Expiration)";
    else if ( DaysRem == 0 )
      S += "(License Expired)";
    else
      S += String(DaysRem);
    }
  else
    S = String(KEYSTRINGS[1]);
#else
    S = "(Freeware Edition)";
#endif

  Caption = S;
}
//---------------------------------------------------------------------------


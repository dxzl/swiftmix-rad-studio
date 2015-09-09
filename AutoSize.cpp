//---------------------------------------------------------------------------
#include <vcl.h>
#include "Main.h"
#pragma hdrstop

#include "AutoSize.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAutoSizeForm *AutoSizeForm = NULL;
//---------------------------------------------------------------------------
__fastcall TAutoSizeForm::TAutoSizeForm(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TAutoSizeForm::FormDestroy(TObject *Sender)
{
#if DEBUG_ON
  MainForm->CWrite( "\r\nFormDestroy() in TAutoSizeForm()!\r\n");
#endif
}
//---------------------------------------------------------------------------
void __fastcall TAutoSizeForm::RadioGroup1Click(TObject *Sender)
// DVD_SL_MINUS_R_SIZE "4,707,319,808"
//
// (4.7GB) (4,714,397,696 bytes) (4496MB) DVD
// DVD_SL_R_12_SIZE "4,700,372,992"
//
// DVD_SL_8_SIZE "1,463,386,112"
//
// (8.5GB) (8,547,991,552 bytes) (8152MB) DVD
// DVD_DL_12_SIZE "8,543,666,176"
//
// CD_74_SIZE "681,984,000"
// CD_80_SIZE "737,280,000"
// CD_90_SIZE "829,440,000"
// CD_99_SIZE "912,384,000"
{
  // See who we selected...
  switch (RadioGroup1->ItemIndex)
  {
    case 0:
      MaskEdit->EditMask = "0,000,000,000";
      MaskEdit->Text = DVD_SL_R_12_SIZE;
    break;

    case 1:
      MaskEdit->EditMask = "0,000,000,000";
      MaskEdit->Text = DVD_DL_12_SIZE;
    break;

    case 2:
      MaskEdit->EditMask = "000,000,000";
      MaskEdit->Text = CD_80_SIZE;
    break;

    case 3:
      MaskEdit->EditMask = "00,000,000,000";
      MaskEdit->Text = BLURAY_SL_12_SIZE;
    break;

    case 4:
      MaskEdit->EditMask = "00,000,000,000";
      MaskEdit->Text = BLURAY_DL_12_SIZE;
    break;

    case 5:
      MaskEdit->EditMask = "999,999,999,999,999";
      MaskEdit->Text = "000,015,076,554,752";
      MaskEdit->SetFocus();
    break;

    default:
    break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TAutoSizeForm::InfoButtonClick(TObject *Sender)
{
  String s =
    "CD 74 - 650.3 MiB of data (681984000 bytes).\n"
    "CD 80 - 703.1 MiB of data (737280000 bytes).\n"
    "CD 90 - 791.0 MiB of data (829440000 bytes).\n"
    "CD 99 - 870.1 MiB of data (912384000 bytes).\n"
    "DVD-R (DVD-5) - 4.7 GB 4.38 GiB of data (4707319808 bytes).\n"
    "DVD+R (DVD-5) - 4.7 GB 4.38 GiB of data (4700373992 bytes).\n"
    "DVD DL (DVD-9) - 8.5 GB 7.95 GiB of data (8543666176 bytes).\n"
    "HD-DVD Single Layer - 15 GB 14.0 GiB of data (15076554752 bytes).\n"
    "HD-DVD Dual Layer - 30 GB 31.1 GiB of data (33393473536 bytes).\n"
    "BD Single Layer - 25 GB 23.3 GiB of data (25025314816 bytes).\n"
    "BD Dual Layer - 50 GB 46.6 GiB of data (50050629632 bytes).";

  ShowMessage(s);
}
//---------------------------------------------------------------------------
void __fastcall TAutoSizeForm::FormShow(TObject *Sender)
{
  MaskEdit->EditMask = "0,000,000,000";
  MaskEdit->Text = DVD_SL_R_12_SIZE;
}
//---------------------------------------------------------------------------
void __fastcall TAutoSizeForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
  try
  {
    String S = MaskEdit->Text;
    String New;

    for ( int ii = 1 ; ii <= S.Length() ; ii++ )
      if ( isdigit(S[ii]) )
        New += S[ii];

    m_size64 = StrToInt64(New);
  }
  catch(...)
  {
    m_size64 = -1; // Error
  }

  MainForm->bAutoSizePrompt = CheckBox->Checked;
}
//---------------------------------------------------------------------------


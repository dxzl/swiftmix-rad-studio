//---------------------------------------------------------------------------
#ifndef FormImportModeH
#define FormImportModeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#define IMPORT_MODE_NONE     (-1)
#define IMPORT_MODE_AUTO      0
#define IMPORT_MODE_UTF8      1
#define IMPORT_MODE_ANSI      2
#define IMPORT_MODE_UTF16     3
#define IMPORT_MODE_UTF16BE   4
//---------------------------------------------------------------------------
class TImportModeForm : public TForm
{
__published:	// IDE-managed Components
  TRadioGroup *RadioGroup;
  TButton *ButtonOk;
  TButton *ButtonCancel;
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall ButtonCancelClick(TObject *Sender);
  void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations

  int FMode;
  String FTitle;

  void __fastcall SetTitle(String sTitle);

public:		// User declarations
  __fastcall TImportModeForm(TComponent* Owner);

  __property int Mode = {read = FMode, write = FMode};
  __property String OurTitle = {read = FTitle, write = SetTitle};
};
//---------------------------------------------------------------------------
extern PACKAGE TImportModeForm *ImportModeForm;
//---------------------------------------------------------------------------
#endif

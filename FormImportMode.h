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
public:		// User declarations
  __fastcall TImportModeForm(TComponent* Owner);

  __property int Mode = {read = FMode, write = FMode};
};
//---------------------------------------------------------------------------
extern PACKAGE TImportModeForm *ImportModeForm;
//---------------------------------------------------------------------------
#endif

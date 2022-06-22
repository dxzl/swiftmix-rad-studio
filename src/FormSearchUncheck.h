//---------------------------------------------------------------------------

#ifndef FormSearchUncheckH
#define FormSearchUncheckH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
//---------------------------------------------------------------------------
class TSearchUncheckForm : public TForm
{
__published:  // IDE-managed Components
  TEdit *Edit1;
  TButton *ButtonOk;
  TButton *ButtonCancel;
  void __fastcall ButtonOkClick(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall ButtonCancelClick(TObject *Sender);
private:  // User declarations
  String FText;
public:    // User declarations
  __fastcall TSearchUncheckForm(TComponent* Owner);


  __property String SearchText = {read = FText, write = FText};
};
//---------------------------------------------------------------------------
extern PACKAGE TSearchUncheckForm *SearchUncheckForm;
//---------------------------------------------------------------------------
#endif

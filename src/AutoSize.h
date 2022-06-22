//---------------------------------------------------------------------------
#ifndef AutoSizeH
#define AutoSizeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>

//---------------------------------------------------------------------------
class TAutoSizeForm : public TForm
{
__published:  // IDE-managed Components
  TButton *OkButton;
  TButton *CancelButton;
  TRadioGroup *RadioGroup1;
  TMaskEdit *MaskEdit;
  TButton *InfoButton;
  TCheckBox *CheckBox;
  void __fastcall RadioGroup1Click(TObject *Sender);
  void __fastcall InfoButtonClick(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormDestroy(TObject *Sender);
private:  // User declarations

  __int64 m_size64;

public:    // User declarations
  __fastcall TAutoSizeForm(TComponent* Owner);

  __property __int64 BytesOnMedia = {read = m_size64};
};
//---------------------------------------------------------------------------
#endif

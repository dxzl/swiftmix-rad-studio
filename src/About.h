//---------------------------------------------------------------------------
#ifndef AboutH
#define AboutH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TAboutForm : public TForm
{
__published:  // IDE-managed Components
  TButton *Button1;
  TButton *Button2;
  TPanel *Panel1;
  TPanel *Panel2;
  TImage *Image1;
  TImage *Image2;
  TLabel *Label1;
  TLabel *RevLabel;

  void __fastcall Button1Click(TObject *Sender);
  void __fastcall Button2Click(TObject *Sender);
  void __fastcall WebSiteClick(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall FormDestroy(TObject *Sender);

private:  // User declarations
  void __fastcall DisplayDaysRemaining( void );
public:    // User declarations
  __fastcall TAboutForm(TComponent* Owner);
  __fastcall ~TAboutForm();
};
//---------------------------------------------------------------------------
extern PACKAGE TAboutForm *AboutForm;
//---------------------------------------------------------------------------
#endif

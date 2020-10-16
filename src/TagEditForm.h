//---------------------------------------------------------------------------

#ifndef TagEditFormH
#define TagEditFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
//---------------------------------------------------------------------------
#define TOKENCOUNT 7
//---------------------------------------------------------------------------
class TPlayerURL;
//class TGeneralAudioFile;

class TFormTags : public TForm
{
__published:	// IDE-managed Components
  TGroupBox *GroupBox2;
  TLabel *Label1;
  TLabel *Label2;
  TLabel *Label4;
  TLabel *Label3;
  TLabel *Label6;
  TLabel *Label5;
  TEdit *EdtTitle;
  TEdit *EdtArtist;
  TEdit *EdtAlbum;
  TEdit *EdtGenre;
  TEdit *EdtYear;
  TEdit *EdtTrack;
  TMemo *Memo1;
  TButton *BtnSave;
  TLabel *LabelPath;
  TLabel *LabelCredit;
  TButton *ButtonCopyTags;
  TButton *ButtonPasteTags;
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall ButtonPasteTagsClick(TObject *Sender);
  void __fastcall ButtonCopyTagsClick(TObject *Sender);
  void __fastcall BtnSaveClick(TObject *Sender);
private:	// User declarations
  TGeneralAudioFile* f;
  String TagPath;
  bool ReadOnly;

  int __fastcall SetTagsFromString(String sIn);
  String __fastcall GetTagsAsString(void);
  String __fastcall StripTab(String sIn);
public:		// User declarations
  __fastcall TFormTags(TComponent* Owner);
  void __fastcall ShowTags(TPlayerURL* p);
};
//---------------------------------------------------------------------------
#endif

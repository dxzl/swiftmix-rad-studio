//---------------------------------------------------------------------------
#ifndef FormExportModeH
#define FormExportModeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TExportModeForm : public TForm
{
__published:	// IDE-managed Components
  TRadioGroup *RadioGroup;
  TButton *ButtonOk;
  TButton *Cancel;
  TCheckBox *SaveAsUtf8CheckBox;
  TLabel *FileLabel;
  TCheckBox *UncPathCheckBox;
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall CancelClick(TObject *Sender);
  void __fastcall FormDestroy(TObject *Sender);

private:	// User declarations
  void __fastcall WMSetText(TWMSetText &Msg);

  String Ext;

  int FMode;
  bool FSaveAsUtf8;
  bool FUncPathFormat;
  String FFile; // file-path (utf-8)
  String FTitle; // this is ANSI only!

protected:
BEGIN_MESSAGE_MAP
  //add message handler for WM_SETEXT (allows UTF-8 title-bar)
  MESSAGE_HANDLER(WM_SETTEXT, TWMSetText, WMSetText)
END_MESSAGE_MAP(TForm)

public:		// User declarations
  __fastcall TExportModeForm(TComponent* Owner);

  __property int Mode = {read = FMode, write = FMode};
  __property bool SaveAsUtf8 = {read = FSaveAsUtf8};
  __property bool UncPathFormat = {read = FUncPathFormat};
  __property String FileName = {read = FFile, write = FFile};
  __property String Title = {read = FTitle, write = FTitle}; // ANSI Only!
};
//---------------------------------------------------------------------------
extern PACKAGE TExportModeForm *ExportModeForm;
//---------------------------------------------------------------------------
#endif

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
#define EXPORT_MODE_NONE     (-1)
#define EXPORT_MODE_UTF8      0
#define EXPORT_MODE_ANSI      1
#define EXPORT_MODE_UTF16     2
#define EXPORT_MODE_UTF16BE   3
//---------------------------------------------------------------------------
#define EXPORT_PATH_NONE          (-1)
#define EXPORT_PATH_RELATIVE       0
#define EXPORT_PATH_ROOTED         1
#define EXPORT_PATH_ABSOLUTE       2
#define EXPORT_PATH_SWIFTMIX       100 // special mode, not in list
//---------------------------------------------------------------------------
class TExportModeForm : public TForm
{
__published:	// IDE-managed Components
    TRadioGroup *RGPathType;
  TButton *ButtonOk;
  TButton *Cancel;
  TLabel *FileLabel;
  TCheckBox *UncPathCheckBox;
  TCheckBox *WriteBOMCheckBox;
  TRadioGroup *RGEncoding;
  TLabel *LabelHint;
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall CancelClick(TObject *Sender);
  void __fastcall FormDestroy(TObject *Sender);

private:	// User declarations

  String Ext;

  int FMode, FEncoding;
  bool FUncPathFmt, FWriteBOM;
  String FFile; // file-path (utf-8)
  String FTitle; // this is ANSI only!

protected:

public:		// User declarations
  __fastcall TExportModeForm(TComponent* Owner);

  __property int Encoding = {read = FEncoding, write = FEncoding};
  __property int Mode = {read = FMode, write = FMode};
  __property bool UncPathFmt = {read = FUncPathFmt, write = FUncPathFmt};
  __property bool WriteBOM = {read = FWriteBOM, write = FWriteBOM};
  __property String FileName = {read = FFile, write = FFile};
  __property String Title = {read = FTitle, write = FTitle}; // ANSI Only!
};
//---------------------------------------------------------------------------
extern PACKAGE TExportModeForm *ExportModeForm;
//---------------------------------------------------------------------------
#endif

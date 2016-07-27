//---------------------------------------------------------------------------
#ifndef FormExportH
#define FormExportH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#define EXPORT_PATH_RELATIVE       0
#define EXPORT_PATH_ROOTED         1
#define EXPORT_PATH_ABSOLUTE       2
#define EXPORT_PATH_NONE           3
//---------------------------------------------------------------------------
// Forward class references...
class TSFDlgForm;
class TExportModeForm;
class TPlaylistForm;
//---------------------------------------------------------------------------

class TExportForm : public TForm
{
__published:	// IDE-managed Components
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormDestroy(TObject *Sender);

private:	// User declarations
  String __fastcall PercentEncode(String sIn, const char* table, bool bEncodeAbove127);
  String __fastcall InsertXMLSpecialCodes(String sIn);
  String __fastcall XmlSpecialCharEncode(char c);
  String __fastcall GetFileString(String uListFullPath,
                                      String sSongFullPath, int Mode);
  String __fastcall StripFileUriPrefixIfAny(String &sIn);
  String __fastcall UniversalExtractFileName(String sIn);
  bool __fastcall IsUri(String sIn);
  String __fastcall ProcessFileName(String &uName, String uListFullPath,
                            int Mode, bool bSaveAsUtf8, bool bUncPathFormat);

  TSFDlgForm* pSFDlg;
  TExportModeForm* pExpModeDlg;

public:		// User declarations
  __fastcall TExportForm(TComponent* Owner);
  __fastcall ~TExportForm();

  int __fastcall Dialog(TPlaylistForm* f, String d, String t);
  int __fastcall NoDialogU(TPlaylistForm* f, String uListFullPath, int Mode,
                                    bool bSaveAsUtf8, bool bUncPathFormat);
  int __fastcall NoDialogW(TPlaylistForm* f, WideString wListFullPath, int Mode,
                                    bool bSaveAsUtf8, bool bUncPathFormat);
};
//---------------------------------------------------------------------------
extern PACKAGE TExportForm *ExportForm;
//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
#ifndef FormImportH
#define FormImportH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#define IMPORT_MODE_AUTO   0
#define IMPORT_MODE_UTF8   1
#define IMPORT_MODE_ANSI   2
//---------------------------------------------------------------------------
// Forward class references...
class TOFMSDlgForm;
class TImportModeForm;
class TPlaylistForm;
//---------------------------------------------------------------------------
class TImportForm : public TForm
{
__published:	// IDE-managed Components
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormDestroy(TObject *Sender);

private:	// User declarations
//  String __fastcall TrimAndReplaceSpecial(String sIn);
//  char __fastcall XmlSpecialCharDecode(String In);
//  String __fastcall UriDecode(String sSrc);
  int __fastcall XmlParser(TPlaylistForm* f, String sType, String sIn, String sPath, bool bConvertToUtf8);
  bool __fastcall ParseFileLine(String &sRef, bool bTagHasUrl);
  String __fastcall ReplacePercentCodes(String sIn);
  String __fastcall ReplaceXmlCodes(String sIn);
  String __fastcall StripCrLfAndTrim(String sIn);
  bool __fastcall ReplaceRelativePath(String &sUrl, String sPath);
  int __fastcall xstrtoi(char *hex);
  int __fastcall HextoDec(char *hex);
  char __fastcall xtod(char c);
  bool __fastcall mishex(char c);
  bool __fastcall ReadIniFile(String sIniPath, TStringList* sl);
  bool __fastcall IsUtf8(const char* string);
  bool __fastcall IsUri(String sIn);

  int XMLCHARSLEN;

  TOFMSDlgForm* pOFMSDlg;
  TImportModeForm* pImpModeDlg;

public:		// User declarations
  __fastcall TImportForm(TComponent* Owner);
  __fastcall ~TImportForm();

  int __fastcall Dialog(TPlaylistForm* f, String d, String t);
  int __fastcall NoDialog(TPlaylistForm* f, String sPath, int Mode);
  int __fastcall GetMode(String Ext, int Mode);
};
//---------------------------------------------------------------------------
extern PACKAGE TImportForm *ImportForm;
//---------------------------------------------------------------------------
#endif

unit SMTagger;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, FileCtrl, StdCtrls, AudioFiles;

type
  TSMTags = class(TForm)
    GroupBox2: TGroupBox;
    EdtTitle: TEdit;
    EdtArtist: TEdit;
    EdtAlbum: TEdit;
    EdtGenre: TEdit;
    Label1: TLabel;
    Label2: TLabel;
    Label4: TLabel;
    Label3: TLabel;
    EdtYear: TEdit;
    EdtTrack: TEdit;
    Label6: TLabel;
    Label5: TLabel;
    Memo1: TMemo;
    BtnSave: TButton;
    LabelPath: TLabel;
    LabelCredit: TLabel;
    procedure FileListBox1Change(Sender: TObject);
    procedure BtnSaveClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private
    { Private }
    MainAudioFile: TGeneralAudioFile;
  public
    { Public }
    Path: String;
  end;

var
  SMTags: TSMTags;

implementation

{$R *.dfm}

procedure TSMTags.Create(Sender: TObject);
begin
    if assigned(MainAudioFile) then
        MainAudioFile.Free;

    MainAudioFile := TGeneralAudioFile.Create(Path);

    EdtTitle.Text  := MainAudioFile.Title;
    EdtArtist.Text := MainAudioFile.Artist;
    EdtAlbum.Text  := MainAudioFile.Album;
    EdtGenre.Text  := MainAudioFile.Genre;
    EdtYear.Text   := MainAudioFile.Year;
    EdtTrack.Text  := MainAudioFile.Track;
    Memo1.Clear;
    Memo1.Lines.Add(Format('Type:      %s',       [MainAudioFile.FileTypeName] ));
    Memo1.Lines.Add(Format('FileSize   %d Bytes', [MainAudioFile.FileSize]     ));
    Memo1.Lines.Add(Format('Duration   %d sec',   [MainAudioFile.Duration]     ));
    Memo1.Lines.Add(Format('Btrate     %d kBit/s',[MainAudioFile.Bitrate div 1000]));
    Memo1.Lines.Add(Format('Samplerate %d Hz',    [MainAudioFile.Samplerate]   ));
    Memo1.Lines.Add(Format('Channels:  %d',       [MainAudioFile.Channels]     ));
end;

pprocedure TSMTags.FormCreate(Sender: TObject);
begin

end;

rocedure TSMTags.BtnSaveClick(Sender: TObject);
begin
    if assigned(MainAudioFile) then
    begin
        MainAudioFile.Title  := EdtTitle.Text ;
        MainAudioFile.Artist := EdtArtist.Text;
        MainAudioFile.Album  := EdtAlbum.Text ;
        MainAudioFile.Genre  := EdtGenre.Text ;
        MainAudioFile.Year   := EdtYear.Text  ;
        MainAudioFile.Track  := EdtTrack.Text ;
        MainAudioFile.UpdateFile;
    end;
end;

initialization
  Self.LabelCredit := '(Thanks to Daniel Gaussmann for his tag-library ' +
                 '\"Audio Werkzeuge Bibliothek\" (AWB)';
  Self.Path := TSMTags.LabelPath.Text;

end.



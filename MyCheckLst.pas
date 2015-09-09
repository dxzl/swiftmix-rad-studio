
{*******************************************************}
{                                                       }
{       Borland Delphi Visual Component Library         }
{                                                       }
{       Copyright (c) 1995,98 Inprise Corporation       }
{                                                       }
{*******************************************************}

unit MyCheckLst;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls,
  StdCtrls;

type
  TMyCheckListBox = class(TCustomListBox)
  private
    FAllowGrayed: Boolean;
    FFlat: Boolean;
    FStandardItemHeight: Integer;
    FOnClickCheck: TNotifyEvent;
    FSaveStates: TList;
    procedure ResetItemHeight;
    procedure DrawCheck(R: TRect; AState: TCheckBoxState);
    procedure SetChecked(Index: Integer; Checked: Boolean);
    function GetChecked(Index: Integer): Boolean;
    procedure SetState(Index: Integer; AState: TCheckBoxState);
    function GetState(Index: Integer): TCheckBoxState;
    procedure SetCheckRect(Index: Integer; ACheckRect: TRect); // S.S.
    function GetCheckRect(Index: Integer): TRect; // S.S.
    procedure ToggleClickCheck(Index: Integer);
    procedure InvalidateCheck(Index: Integer);
    function CreateWrapper(Index: Integer): TObject;
    function ExtractWrapper(Index: Integer): TObject;
    function GetWrapper(Index: Integer): TObject;
    function HaveWrapper(Index: Integer): Boolean;
    procedure SetFlat(Value: Boolean);
    procedure CNDrawItem(var Message: TWMDrawItem); message CN_DRAWITEM;
    procedure CMFontChanged(var Message: TMessage); message CM_FONTCHANGED;
    procedure WMDestroy(var Msg : TWMDestroy);message WM_DESTROY;
  protected
    procedure DrawItem(Index: Integer; Rect: TRect; State: TOwnerDrawState); override;
    procedure SetItemData(Index: Integer; AData: LongInt); override;
    function GetItemData(Index: Integer): LongInt; override;
    procedure KeyPress(var Key: Char); override;
    procedure MouseDown(Button: TMouseButton; Shift: TShiftState; X, Y: Integer); override;
    procedure ResetContent; override;
    procedure DeleteString(Index: Integer); override;
    procedure ClickCheck; dynamic;
    procedure CreateParams(var Params: TCreateParams); override;
    procedure CreateWnd; override;
    procedure DestroyWnd; override;
    function GetCheckWidth: Integer;
  public
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;
    property Checked[Index: Integer]: Boolean read GetChecked write SetChecked;
    property State[Index: Integer]: TCheckBoxState read GetState write SetState;
    property CheckRect[Index: Integer]: TRect read GetCheckRect write SetCheckRect;
  published
    property AllowGrayed: Boolean read FAllowGrayed write FAllowGrayed default False;
    property Align;
    property Anchors;
    property BiDiMode;
    property BorderStyle;
    property Color;
    property Columns;
    property Constraints;
    property Ctl3D;
    property DragCursor;
    property DragKind;
    property DragMode;
    property Enabled;
    property ExtendedSelect;
    property Flat: Boolean read FFlat write SetFlat default True;
    property Font;
    property ImeMode;
    property ImeName;
    property IntegralHeight;
    property ItemHeight;
    property Items;
    property MultiSelect;
    property ParentBiDiMode;
    property ParentColor;
    property ParentCtl3D;
    property ParentFont;
    property ParentShowHint;
    property PopupMenu;
    property ShowHint;
    property Sorted;
    property Style;
    property TabOrder;
    property TabStop;
    property TabWidth;
    property Visible;
    property OnClick;
    property OnClickCheck: TNotifyEvent read FOnClickCheck write FOnClickCheck;
    property OnDblClick;
    property OnDragDrop;
    property OnDragOver;
    property OnDrawItem;
    property OnEndDock;
    property OnEndDrag;
    property OnEnter;
    property OnExit;
    property OnKeyDown;
    property OnKeyPress;
    property OnKeyUp;
    property OnMeasureItem;
    property OnMouseDown;
    property OnMouseMove;
    property OnMouseUp;
    property OnStartDock;
    property OnStartDrag;
  end;

implementation

uses Consts;

// Data for each item (SetItemData stores a pointer to this object) Comment by S.S.
type
  TMyCheckListBoxDataWrapper = class
  private
    FData: LongInt;
    FState: TCheckBoxState;
    FCheckRect: TRect; // S.S.
    procedure SetChecked(Check: Boolean);
    function GetChecked: Boolean;
  public
    class function GetDefaultState: TCheckBoxState;
    class function GetDefaultCheckRect: TRect;
    property Checked: Boolean read GetChecked write SetChecked;
    property State: TCheckBoxState read FState write FState;
    property CheckRect: TRect read FCheckRect write FCheckRect; // S.S.
  end;

var
  FCheckWidth, FCheckHeight: Integer;

procedure GetCheckSize;
begin
  with TBitmap.Create do
    try
      Handle := LoadBitmap(0, PChar(32759));
      FCheckWidth := Width div 4;
      FCheckHeight := Height div 3;
    finally
      Free;
    end;
end;
    
{ TMyCheckListBoxDataWrapper }

procedure TMyCheckListBoxDataWrapper.SetChecked(Check: Boolean);
begin
  if Check then FState := cbChecked else FState := cbUnchecked;
end;

function TMyCheckListBoxDataWrapper.GetChecked: Boolean;
begin
  Result := FState = cbChecked;
end;

class function TMyCheckListBoxDataWrapper.GetDefaultState: TCheckBoxState;
begin
  Result := cbUnchecked;
end;

class function TMyCheckListBoxDataWrapper.GetDefaultCheckRect: TRect;
var
  R: TRect;
begin
  R.Left := 0;
  R.Right := 0;
  R.Top := 0;
  R.Bottom := 0;
  Result := R;
end;

{ TMyCheckListBox }

constructor TMyCheckListBox.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
  FFlat := True;
end;

destructor TMyCheckListBox.Destroy;
begin
  FSaveStates.Free;
  inherited;
end;

procedure TMyCheckListBox.CreateWnd;
begin
  inherited CreateWnd;
  if FSaveStates <> nil then
  begin
    FSaveStates.Free;
    FSaveStates := nil;
  end;
  ResetItemHeight;
end;

procedure TMyCheckListBox.DestroyWnd;
var
  I: Integer;
begin
  if Items.Count > 0 then
  begin
    FSaveStates := TList.Create;
    for I := 0 to Items.Count -1 do
      FSaveStates.Add(TObject(State[I]));
  end;
  inherited DestroyWnd;
end;

procedure TMyCheckListBox.CreateParams(var Params: TCreateParams);
begin
  inherited;
  with Params do
    if Style and (LBS_OWNERDRAWFIXED or LBS_OWNERDRAWVARIABLE) = 0 then
      Style := Style or LBS_OWNERDRAWFIXED;
end;

function TMyCheckListBox.GetCheckWidth: Integer;
begin
  Result := FCheckWidth + 2;
end;

procedure TMyCheckListBox.CMFontChanged(var Message: TMessage);
begin
  inherited;
  ResetItemHeight;
end;

procedure TMyCheckListBox.ResetItemHeight;
begin
  if HandleAllocated and (Style = lbStandard) then
  begin
    Canvas.Font := Font;
    FStandardItemHeight := Canvas.TextHeight('Wg');
    Perform(LB_SETITEMHEIGHT, 0, FStandardItemHeight);
  end;
end;

procedure TMyCheckListBox.DrawItem(Index: Integer; Rect: TRect; State: TOwnerDrawState);
var
  R: TRect;
  SaveEvent: TDrawItemEvent;
  ACheckWidth: Integer;
begin
  ACheckWidth := GetCheckWidth;
  if Index < Items.Count then
  begin
    R := Rect;
    if not UseRightToLeftAlignment then
    begin
      R.Right := Rect.Left;
      R.Left := R.Right - ACheckWidth;
    end
    else
    begin
      R.Left := Rect.Right;
      R.Right := R.Left + ACheckWidth;
    end;
    TMyCheckListBoxDataWrapper(GetWrapper(Index)).CheckRect := R; // S.S. Save left orign of checkbox
    DrawCheck(R, GetState(Index));
  end;

  if (Style = lbStandard) and Assigned(OnDrawItem) then
  begin
    { Force lbStandard list to ignore OnDrawItem event. }
    SaveEvent := OnDrawItem;
    OnDrawItem := nil;
    try
      inherited;
    finally
      OnDrawItem := SaveEvent;
    end;
  end
  else
    inherited;
end;

procedure TMyCheckListBox.CNDrawItem(var Message: TWMDrawItem);
begin
  with Message.DrawItemStruct^ do
    if not UseRightToLeftAlignment then
      rcItem.Left := rcItem.Left + GetCheckWidth
    else
      rcItem.Right := rcItem.Right - GetCheckWidth;
  inherited;
end;

procedure TMyCheckListBox.DrawCheck(R: TRect; AState: TCheckBoxState);
var
  DrawState: Integer;
  DrawRect: TRect;
  OldBrushColor: TColor;
  OldBrushStyle: TBrushStyle;
  OldPenColor: TColor;
  Rgn, SaveRgn: HRgn;
begin
  SaveRgn := 0;
  DrawRect.Left := R.Left + (R.Right - R.Left - FCheckWidth) div 2;
  DrawRect.Top := R.Top + (R.Bottom - R.Top - FCheckWidth) div 2;
  DrawRect.Right := DrawRect.Left + FCheckWidth;
  DrawRect.Bottom := DrawRect.Top + FCheckHeight;
  case AState of
    cbChecked:
      DrawState := DFCS_BUTTONCHECK or DFCS_CHECKED;
    cbUnchecked:
      DrawState := DFCS_BUTTONCHECK;
    else // cbGrayed
      DrawState := DFCS_BUTTON3STATE or DFCS_CHECKED;
  end;
  with Canvas do
  begin
    if Flat then
    begin
      { Remember current clipping region }
      SaveRgn := CreateRectRgn(0,0,0,0);
      GetClipRgn(Handle, SaveRgn);
      { Clip 3d-style checkbox to prevent flicker }
      with DrawRect do
        Rgn := CreateRectRgn(Left + 2, Top + 2, Right - 2, Bottom - 2);
      SelectClipRgn(Handle, Rgn);
      DeleteObject(Rgn);
    end;
    DrawFrameControl(Handle, DrawRect, DFC_BUTTON, DrawState);
    if Flat then
    begin
      SelectClipRgn(Handle, SaveRgn);
      DeleteObject(SaveRgn);
      { Draw flat rectangle in-place of clipped 3d checkbox above }
      OldBrushStyle := Brush.Style;
      OldBrushColor := Brush.Color;
      OldPenColor := Pen.Color;
      Brush.Style := bsClear;
      Pen.Color := clBtnShadow;
      with DrawRect do
        Rectangle(Left + 1, Top + 1, Right - 1, Bottom - 1);
      Brush.Style := OldBrushStyle;
      Brush.Color := OldBrushColor;
      Pen.Color := OldPenColor;
    end;
  end;
end;

procedure TMyCheckListBox.SetChecked(Index: Integer; Checked: Boolean);
begin
  if Checked <> GetChecked(Index) then
  begin
    TMyCheckListBoxDataWrapper(GetWrapper(Index)).SetChecked(Checked);
    InvalidateCheck(Index);
  end;
end;

procedure TMyCheckListBox.SetState(Index: Integer; AState: TCheckBoxState);
begin
  if AState <> GetState(Index) then
  begin
    TMyCheckListBoxDataWrapper(GetWrapper(Index)).State := AState;
    InvalidateCheck(Index);
  end;
end;

// S.S.
procedure TMyCheckListBox.SetCheckRect(Index: Integer; ACheckRect: TRect);
var
  R: TRect;
begin
  R := GetCheckRect(Index);
  with ACheckRect do
    if (Left <> R.Left) or (Right <> R.Right) or (Top <> R.Top) or (Bottom <> R.Bottom) then
    begin
      TMyCheckListBoxDataWrapper(GetWrapper(Index)).CheckRect := ACheckRect;
    end;
end;

procedure TMyCheckListBox.InvalidateCheck(Index: Integer);
var
  R: TRect;
begin
  R := ItemRect(Index);
  if not UseRightToLeftAlignment then
    R.Right := R.Left + GetCheckWidth
  else
    R.Left := R.Right - GetCheckWidth;
  InvalidateRect(Handle, @R, not (csOpaque in ControlStyle));
  UpdateWindow(Handle);
end;

function TMyCheckListBox.GetChecked(Index: Integer): Boolean;
begin
  if HaveWrapper(Index) then
    Result := TMyCheckListBoxDataWrapper(GetWrapper(Index)).GetChecked
  else
    Result := False;
end;

function TMyCheckListBox.GetState(Index: Integer): TCheckBoxState;
begin
  if HaveWrapper(Index) then
    Result := TMyCheckListBoxDataWrapper(GetWrapper(Index)).State
  else
    Result := TMyCheckListBoxDataWrapper.GetDefaultState;
end;

// S.S.
function TMyCheckListBox.GetCheckRect(Index: Integer): TRect;
begin
  if HaveWrapper(Index) then
    Result := TMyCheckListBoxDataWrapper(GetWrapper(Index)).CheckRect
  else
    Result := TMyCheckListBoxDataWrapper.GetDefaultCheckRect;
end;

procedure TMyCheckListBox.KeyPress(var Key: Char);
begin
  inherited;
  if (Key = ' ') then ToggleClickCheck(ItemIndex);
end;

procedure TMyCheckListBox.MouseDown(Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
var
  Index: Integer;
begin
  inherited;
  Index := ItemAtPos(Point(X,Y),True);
  if Index <> -1 then
    if not UseRightToLeftAlignment then
    begin
      if X - ItemRect(Index).Left < GetCheckWidth then
        ToggleClickCheck(Index)
    end
    else
    begin
      Dec(X, ItemRect(Index).Right - GetCheckWidth);
      if (X > 0) and (X < GetCheckWidth) then
        ToggleClickCheck(Index)
    end;
end;
    
procedure TMyCheckListBox.ToggleClickCheck;
var
  State: TCheckBoxState;
begin
  if (Index >= 0) and (Index < Items.Count) then
  begin
    State := Self.State[Index];
    case State of
      cbUnchecked:
        if AllowGrayed then State := cbGrayed else State := cbChecked;
      cbChecked: State := cbUnchecked;
      cbGrayed: State := cbChecked;
    end;
    Self.State[Index] := State;
    ClickCheck;
  end;
end;

procedure TMyCheckListBox.ClickCheck;
begin
  if Assigned(FOnClickCheck) then FOnClickCheck(Self);
end;

function TMyCheckListBox.GetItemData(Index: Integer): LongInt;
begin
  Result := 0;
  if HaveWrapper(Index) then
    Result := TMyCheckListBoxDataWrapper(GetWrapper(Index)).FData;
end;

function TMyCheckListBox.GetWrapper(Index: Integer): TObject;
begin
  Result := ExtractWrapper(Index);
  if Result = nil then
    Result := CreateWrapper(Index);
end;

function TMyCheckListBox.ExtractWrapper(Index: Integer): TObject;
begin
  Result := TMyCheckListBoxDataWrapper(inherited GetItemData(Index));
  if LB_ERR = Integer(Result) then
    raise EListError.CreateFmt(SListIndexError, [Index]);
  if (Result <> nil) and (not (Result is TMyCheckListBoxDataWrapper)) then
    Result := nil;
end;

function TMyCheckListBox.CreateWrapper(Index: Integer): TObject;
begin
  Result := TMyCheckListBoxDataWrapper.Create;
  inherited SetItemData(Index, LongInt(Result));
end;

function TMyCheckListBox.HaveWrapper(Index: Integer): Boolean;
begin
  Result := ExtractWrapper(Index) <> nil;
end;

procedure TMyCheckListBox.SetItemData(Index: Integer; AData: LongInt);
var
  Wrapper: TMyCheckListBoxDataWrapper;
begin
  Wrapper := TMyCheckListBoxDataWrapper(GetWrapper(Index));
  Wrapper.FData := AData;
  if FSaveStates <> nil then
    if FSaveStates.Count > 0 then
    begin
     Wrapper.FState := TCheckBoxState(FSaveStates[0]);
     FSaveStates.Delete(0);
    end;
end;

procedure TMyCheckListBox.ResetContent;
var
  I: Integer;
begin
  for I := 0 to Items.Count - 1 do
    if HaveWrapper(I) then
      GetWrapper(I).Free;
  inherited;
end;

procedure TMyCheckListBox.DeleteString(Index: Integer);
begin
  if HaveWrapper(Index) then
    GetWrapper(Index).Free;
  inherited;
end;

procedure TMyCheckListBox.SetFlat(Value: Boolean);
begin
  if Value <> FFlat then
  begin
    FFlat := Value;
    Invalidate;
  end;
end;

procedure TMyCheckListBox.WMDestroy(var Msg: TWMDestroy);
var
  i: Integer;
begin
  for i := 0 to Items.Count -1 do
    ExtractWrapper(i).Free;
  inherited;
end;

initialization
  GetCheckSize;

end.


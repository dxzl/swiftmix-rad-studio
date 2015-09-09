// Borland C++ Builder
// Copyright (c) 1995, 1999 by Borland International
// All rights reserved

// (DO NOT EDIT: machine generated header) 'MyCheckLst.pas' rev: 4.00

#ifndef MyCheckLstHPP
#define MyCheckLstHPP

#pragma delphiheader begin
#pragma option push -w-
#include <Menus.hpp>	// Pascal unit
#include <Forms.hpp>	// Pascal unit
#include <StdCtrls.hpp>	// Pascal unit
#include <Controls.hpp>	// Pascal unit
#include <Graphics.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <SysUtils.hpp>	// Pascal unit
#include <Messages.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <System.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Mychecklst
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS TMyCheckListBox;
#pragma pack(push, 4)
class PASCALIMPLEMENTATION TMyCheckListBox : public Stdctrls::TCustomListBox 
{
	typedef Stdctrls::TCustomListBox inherited;
	
private:
	bool FAllowGrayed;
	bool FFlat;
	int FStandardItemHeight;
	Classes::TNotifyEvent FOnClickCheck;
	Classes::TList* FSaveStates;
	void __fastcall ResetItemHeight(void);
	void __fastcall DrawCheck(const Windows::TRect &R, Stdctrls::TCheckBoxState AState);
	void __fastcall SetChecked(int Index, bool Checked);
	bool __fastcall GetChecked(int Index);
	void __fastcall SetState(int Index, Stdctrls::TCheckBoxState AState);
	Stdctrls::TCheckBoxState __fastcall GetState(int Index);
	void __fastcall SetCheckRect(int Index, const Windows::TRect &ACheckRect);
	Windows::TRect __fastcall GetCheckRect(int Index);
	void __fastcall ToggleClickCheck(int Index);
	void __fastcall InvalidateCheck(int Index);
	System::TObject* __fastcall CreateWrapper(int Index);
	System::TObject* __fastcall ExtractWrapper(int Index);
	System::TObject* __fastcall GetWrapper(int Index);
	bool __fastcall HaveWrapper(int Index);
	void __fastcall SetFlat(bool Value);
	HIDESBASE MESSAGE void __fastcall CNDrawItem(Messages::TWMDrawItem &Message);
	HIDESBASE MESSAGE void __fastcall CMFontChanged(Messages::TMessage &Message);
	HIDESBASE MESSAGE void __fastcall WMDestroy(Messages::TWMNoParams &Msg);
	
protected:
	virtual void __fastcall DrawItem(int Index, const Windows::TRect &Rect, Stdctrls::TOwnerDrawState State
		);
	DYNAMIC void __fastcall SetItemData(int Index, int AData);
	DYNAMIC int __fastcall GetItemData(int Index);
	DYNAMIC void __fastcall KeyPress(char &Key);
	DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, 
		int Y);
	DYNAMIC void __fastcall ResetContent(void);
	DYNAMIC void __fastcall DeleteString(int Index);
	DYNAMIC void __fastcall ClickCheck(void);
	virtual void __fastcall CreateParams(Controls::TCreateParams &Params);
	virtual void __fastcall CreateWnd(void);
	virtual void __fastcall DestroyWnd(void);
	int __fastcall GetCheckWidth(void);
	
public:
	__fastcall virtual TMyCheckListBox(Classes::TComponent* AOwner);
	__fastcall virtual ~TMyCheckListBox(void);
	__property bool Checked[int Index] = {read=GetChecked, write=SetChecked};
	__property Stdctrls::TCheckBoxState State[int Index] = {read=GetState, write=SetState};
	__property Windows::TRect CheckRect[int Index] = {read=GetCheckRect, write=SetCheckRect};
	
__published:
	__property bool AllowGrayed = {read=FAllowGrayed, write=FAllowGrayed, default=0};
	__property Align ;
	__property Anchors ;
	__property BiDiMode ;
	__property BorderStyle ;
	__property Color ;
	__property Columns ;
	__property Constraints ;
	__property Ctl3D ;
	__property DragCursor ;
	__property DragKind ;
	__property DragMode ;
	__property Enabled ;
	__property ExtendedSelect ;
	__property bool Flat = {read=FFlat, write=SetFlat, default=1};
	__property Font ;
	__property ImeMode ;
	__property ImeName ;
	__property IntegralHeight ;
	__property ItemHeight ;
	__property Items ;
	__property MultiSelect ;
	__property ParentBiDiMode ;
	__property ParentColor ;
	__property ParentCtl3D ;
	__property ParentFont ;
	__property ParentShowHint ;
	__property PopupMenu ;
	__property ShowHint ;
	__property Sorted ;
	__property Style ;
	__property TabOrder ;
	__property TabStop ;
	__property TabWidth ;
	__property Visible ;
	__property OnClick ;
	__property Classes::TNotifyEvent OnClickCheck = {read=FOnClickCheck, write=FOnClickCheck};
	__property OnDblClick ;
	__property OnDragDrop ;
	__property OnDragOver ;
	__property OnDrawItem ;
	__property OnEndDock ;
	__property OnEndDrag ;
	__property OnEnter ;
	__property OnExit ;
	__property OnKeyDown ;
	__property OnKeyPress ;
	__property OnKeyUp ;
	__property OnMeasureItem ;
	__property OnMouseDown ;
	__property OnMouseMove ;
	__property OnMouseUp ;
	__property OnStartDock ;
	__property OnStartDrag ;
public:
	#pragma option push -w-inl
	/* TWinControl.CreateParented */ inline __fastcall TMyCheckListBox(HWND ParentWindow) : Stdctrls::TCustomListBox(
		ParentWindow) { }
	#pragma option pop
	
};

#pragma pack(pop)

//-- var, const, procedure ---------------------------------------------------

}	/* namespace Mychecklst */
#if !defined(NO_IMPLICIT_NAMESPACE_USE)
using namespace Mychecklst;
#endif
#pragma option pop	// -w-

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// MyCheckLst

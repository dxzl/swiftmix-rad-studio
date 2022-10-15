//---------------------------------------------------------------------------
#include <vcl.h>
#include "Main.h"
#pragma hdrstop

#include "Urlmon.h"
#include "Progress.h"
#include <WinUser.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TPlaylistForm *ListA = NULL;
TPlaylistForm *ListB = NULL;


//---------------------------------------------------------------------------
// TCheckListBox properties:
//---------------------------------------------------------------------------
// Align  published  Determines how the control aligns within its container (parent control).
// AlignDisabled  public  Indicates child control realignment disabled.
// AlignWithMargins  published  Specifies whether a control should be constrained by margins.
// AllowGrayed  published  Determines whether a check box can have a dimmed appearance (be unavailable).
// Anchors  published  Specifies how the control is anchored to its parent.
// AutoComplete  published  Determines whether the user can give focus to items by typing in the list.
// AutoCompleteDelay  public  Specifies the delay between a key press and an attempt to autocomplete the field.
// BevelEdges  published  Specifies which edges of the control are beveled.
// BevelInner  published  Specifies the cut of the inner bevel.
// BevelKind  published  Specifies the control's bevel style.
// BevelOuter  published  Specifies the cut of the outer bevel.
// BevelWidth  published  Specifies the width of the inner and outer bevels.
// BiDiMode  published  Specifies the bidirectional mode for the control.
// BorderStyle  published  Determines whether the list box has a border.
// BoundsRect  public  Specifies the bounding rectangle of the control, expressed in the coordinate system of the parent control.
// Brush  public  Determines the color and pattern used for painting the background of the control.
// Canvas  public  Provides a drawing surface when implementing a handler for the OnDrawItem event of an owner-draw list box.
// CheckBoxPadding  published
// Checked  public  Indicates which items are checked.
// ClientHeight  public  Specifies the height of the control's client area in pixels.
// ClientOrigin  public  Specifies the screen coordinates (in pixels) of the upper-left corner of a control's client area.
// ClientRect  public  Specifies the size of a control's client area in pixels.
// ClientWidth  public  Specifies the horizontal size of the control's client area in pixels.
// Color  published  Specifies the background color of the control.
// Columns  published  Specifies the number of columns, in a multi-column list box, that are visible without having to scroll.
// ComObject  public  Specifies the interface reference implemented by the component.
// ComponentCount  public  Indicates the number of components owned by the component.
// ComponentIndex  public  Indicates the position of the component in its owner's Components property array.
// Components  public  Lists all components owned by the component.
// ComponentState  public  Describes the current state of the component, indicating when a component needs to avoid certain actions.
// ComponentStyle  public  Governs the behavior of the component.
// Constraints  published  Specifies the size constraints for the control.
// ControlCount  public  Returns the number of child controls.
// Controls  public  Lists all child controls.
// ControlState  public  Specifies the current state of a control at run time.
// ControlStyle  public  Determines style characteristics of the control.
// Count  public  Indicates the number of items in the list box.
// Ctl3D  published  Determines whether a control has a three-dimensional (3-D) or two-dimensional look.
// CurrentPPI  public
// Cursor  published  Specifies the image used to represent the mouse pointer when it passes into the region covered by the control.
// CustomHint  published  CustomHint is a custom hint for the control.
// DesignInfo  public  Contains information used by the Form designer.
// DockClientCount  public  Specifies the number of controls that are docked on the windowed control.
// DockClients  public  Lists the controls that are docked to the windowed control.
// DockManager  public  Specifies the control's docking manager interface.
// DockOrientation  public  DockOrientation specifies how the control is docked relative to other controls docked in the same parent.
// DockSite  public  Specifies whether the control can be the target of drag-and-dock operations.
// DoubleBuffered  published  Determines whether the control's image is rendered directly to the window or painted to an in-memory bitmap first.
// DragCursor  published  Specifies the image used to represent the mouse pointer when the control is being dragged.
// DragKind  published  Specifies whether the control is being dragged normally or for docking.
// DragMode  published  Specifies how the control initiates drag-and-drop or drag-and-dock operations.
// Enabled  published  Controls whether the control responds to mouse, keyboard, and timer events.
// ExplicitHeight  public  Specifies the explicit vertical size of the control in pixels.
// ExplicitLeft  public  Specifies the explicit horizontal pixel coordinate of the left edge of a component relative to its parent.
// ExplicitTop  public  Specifies the explicit vertical pixel coordinate of the top edge of a component relative to its parent.
// ExplicitWidth  public  Specifies the explicit horizontal size of the control in pixels.
// Flat  published  Determines whether the check boxes appear three-dimensional.
// Floating  public  Specifies whether the control is floating.
// FloatingDockSiteClass  public  Specifies the class of the temporary control that hosts the control when it is floating.
// Font  published  Specifies the attributes of text written on or in the control.
// Handle  public  Provides access to the underlying Windows screen object for the control.
// Header  public  Indicates which items act as headers.
// HeaderBackgroundColor  published  Specifies the background color for a header item in the listbox.
// HeaderColor  published  Specifies the font color for a header item in the list box.
// Height  published  Specifies the vertical size of the control in pixels.
// HelpContext  published  The HelpContext property contains the numeric context ID that identifies the Help topic for the control.
// HelpKeyword  published  The HelpKeyword property contains the keyword string that identifies the Help topic for the control.
// HelpType  published  Specifies whether the control's context-sensitive Help topic is identified by a context ID or by keyword.
// Hint  published  Hint contains the text string that appears when the user moves the mouse over the control.
// HostDockSite  public  Specifies the control in which the control is docked.
// ImeMode  published  Determines the behavior of the input method editor (IME).
// ImeName  published  Specifies the input method editor (IME) to use for converting keyboard input to Asian language characters.
// IntegralHeight  published  Determines whether the list box displays the partial items.
// ItemEnabled  public  Enables or disables individual items in the list.
// ItemHeight  published  Specifies the height in pixels of an item in an owner-draw list box.
// ItemIndex  public  Specifies the index of the selected item.
// Items  published  Contains the strings that appear in the list box.
// Left  published  Specifies the horizontal coordinate of the left edge of a component relative to its parent.
// LRDockWidth  public  Specifies the width of the control when it is docked horizontally.
// Margins  published  Specifies the margins of the control.
// MouseInClient  public  Indicates whether the mouse pointer is currently in the client area of the control.
// MultiSelect  public  Specifies whether the user can select more than one item.
// Name  published  Specifies the name of the component as referenced in code.
// Observers  public  Indicates the TObservers object added to the TComponent.
// Owner  public  Indicates the component that is responsible for streaming and freeing this component.
// Padding  public  Specifies the padding of a control.
// Parent  public  Specifies the parent of the control.
// ParentBiDiMode  published  Specifies whether the control uses its parent's BiDiMode.
// ParentColor  published  Specifies where a control looks for its color information.
// ParentCtl3D  published  Determines where a component looks to determine whether it should have a three-dimensional look.
// ParentCustomHint  published  Specifies where a control looks for its custom hint.
// ParentDoubleBuffered  published  ParentDoubleBuffered defers the DoubleBuffered property of this component to the value of the parent's DoubleBuffered property.
// ParentFont  published  Specifies where a control looks for its font information.
// ParentShowHint  published  Specifies where a control looks to find out if its Help Hint should be shown.
// ParentWindow  public  Reference to parent's underlying control.
// PopupMenu  published  Specifies the pop-up menu associated with the control.
// ScaleFactor  public
// ScrollWidth  published  Specifies the width, in pixels, by which the list box can scroll horizontally.
// SelCount  public  Indicates the number of selected items.
// Selected  public  Indicates whether a particular item is selected.
// ShowHint  published  ShowHint specifies whether to show the Help Hint when the mouse pointer moves over the control.
// Showing  public  Indicates whether the control is showing on the screen.
// Sorted  published  Specifies whether the items in a list box are arranged alphabetically.
// State  public  Indicates which items are checked.
// Style  published  Determines whether the list box is standard or owner-draw and whether it is virtual.
// StyleElements  published  Specifies the style elements that are used by the control.
// StyleName  published
// TabOrder  published  Indicates the position of the control in its parent's tab order.
// TabStop  published  Determines whether the user can tab to a control.
// TabWidth  published  Specifies the size of the tabs in the list box.
// Tag  published  Stores a NativeInt integral value as a part of a component.
// TBDockHeight  public  Specifies the height of the control when it is docked vertically.
// Top  published  Specifies the Y coordinate of the upper-left corner of a control, relative to its parent or containing control in pixels.
// TopIndex  public  Specifies the index number of the item that appears at the top of the list box.
// Touch  published  Specifies the touch manager component associated with the control.
// UndockHeight  public  Specifies the height of the control when it is floating.
// UndockWidth  public  Specifies the width of the control when it is floating.
// UseDockManager  public  Specifies whether the docking manager is used in drag-and-dock operations.
// VCLComObject  public  Represents information used internally by components that support COM.
// Visible  published  Specifies whether the component appears onscreen.
// VisibleDockClientCount  public  Specifies the number of visible controls that are docked on the windowed control.
// Width  published  Specifies the horizontal size of the control or form in pixels.
//---------------------------------------------------------------------------
//
// TCheckListBox Methods
//
//AddItem  public  Adds an item to the list box
//AfterConstruction  public  Responds after the last constructor has executed.
//Assign  public  Copies the contents of another similar object.
//BeforeDestruction  public  Performs any necessary actions before the first destructor is called.
//BeginDrag  public  Starts the dragging of a control.
//BeginInvoke  public  Performs an asynchronous call to the method specified by either AProc or AFunc.
//BringToFront  public  Puts the control in front of all other controls in its parent control.
//Broadcast  public  Sends a message to each of the child controls.
//CanFocus  public  Indicates whether a control can receive focus.
//CheckAll  public  Changes the state of the check boxes in the TCheckListBox control.
//ClassInfo  public  Returns a pointer to the run-time type information (RTTI) table for the object type.
//ClassName  public  Returns a string indicating the type of the object instance (as opposed to the type of the variable passed as an argument).
//ClassNameIs  public  Determines whether an object is of a specific type.
//ClassParent  public  Returns the type of the immediate ancestor of a class.
//ClassType  public  Returns the class reference for the object's class.
//CleanupInstance  public  Performs finalization on long strings, variants, and interface variables within a class.
//Clear  public  Deletes all items from the list box.
//ClearSelection  public  Deselects all selected items in the list box, if possible.
//ClientToParent  public  Translates client coordinates to parent coordinates.
//ClientToScreen  public  Translates a given point from client area coordinates to global screen coordinates.
//ContainsControl  public  Indicates whether a specified control exists within the control.
//ControlAtPos  public  Returns the child control located at a specified position within the control.
//CopySelection  public  Copies the selected items to another list-type control.
//Create  public
//CreateParented  public  Creates and initializes a control as the child of a specified non-VCL container.
//CreateParentedControl  public  Creates and initializes a control as the child of a specified non-VCL window.
//DefaultHandler  public  Provides message handling for all messages that the control does not fully process by itself.
//DeleteSelected  public  Deletes the selected items from the list box.
//Destroy  public  Destroys the TCheckListBox instance and frees its memory.
//DestroyComponents  public  Destroys all owned components.
//Destroying  public  Indicates that the component and its owned components are about to be destroyed.
//DisableAlign  public  Disables the realignment of child controls.
//Dispatch  public  Calls message-handling methods for the object, based on the contents of the Message parameter.
//DisposeOf  public  DisposeOf forces the execution of the destructor code in an object.
//Dock  public  Used internally to dock the control.
//DockDrop  public  Generates an OnDockDrop event.
//DragDrop  public  Is an OnDragDrop event dispatcher.
//Dragging  public  Indicates whether a control is being dragged.
//DrawTextBiDiModeFlags  public  Returns the text flags that reflect the current setting of the BiDiMode property.
//DrawTextBiDiModeFlagsReadingOnly  public  Returns the text flag to add that indicates whether the control's text should read from right to left.
//EnableAlign  public  Decrements the reference count incremented by the DisableAlign method, eventually realigning the child controls.
//EndDrag  public  Stops a control from being dragged any further.
//EndFunctionInvoke  public  Blocks the caller until the specified ASyncResult completes.
//EndInvoke  public  Blocks the caller until the specified ASyncResult completes.
//Equals  public  Checks whether the current instance and the Obj parameter are equal.
//ExecuteAction  public  Executes an action.
//FieldAddress  public  Returns the address of a published object field.
//FindChildControl  public  Returns a child control given its name.
//FindComponent  public  Indicates whether a given component is owned by the component.
//FlipChildren  public  Reverses the positions of child controls.
//Focused  public  Determines whether the control has input focus.
//Free  public  Destroys an object and frees its associated memory, if necessary.
//FreeInstance  public  Deallocates memory allocated by a previous call to the NewInstance method.
//FreeNotification  public  Ensures that AComponent is notified that the component is going to be destroyed.
//FreeOnRelease  public  Frees the interface reference for components that were created from COM classes.
//GetChildren  public  Calls a specified method for each child of the control.
//GetControlsAlignment  public  Indicates how text is aligned within the control.
//GetCount  public  Returns the value of the Count property.
//GetEnumerator  public  Returns a TComponent enumerator.
//GetHashCode  public  Returns an integer containing the hash code.
//GetInterface  public  Retrieves a specified interface.
//GetInterfaceEntry  public  Returns the entry for a specific interface implemented in a class.
//GetInterfaceTable  public  Returns a pointer to a structure containing all of the interfaces implemented by a given class.
//GetNamePath  public  Returns a string used by the Object Inspector.
//GetParentComponent  public  Returns the parent of the control.
//GetStyleName  public
//GetSystemMetrics  public
//GetTabControlList  public  Builds a list of owned controls.
//GetTabOrderList  public  Builds a list of controls in tab order.
//GetTextBuf  public  Retrieves the control's text, copies it into a buffer, and returns the number of characters copied.
//GetTextLen  public  Returns the length of the control's text.
//HandleAllocated  public  Reports whether a screen object handle exists for the control.
//HandleNeeded  public  Creates a screen object for the control if it doesn't already exist.
//HasParent  public  Indicates whether the control has a parent.
//Hide  public  Makes the control invisible.
//InheritsFrom  public  Determines the relationship of two object types.
//InitiateAction  public  Calls the action link's Update method if the control is associated with an action link.
//InitInstance  public  Initializes a newly allocated object instance to all zeros and initializes the instance's virtual method table pointer.
//InsertComponent  public  Establishes the component as the owner of a specified component.
//InsertControl  public  Inserts a control into the Controls array property.
//InstanceSize  public  Returns the size in bytes of each instance of the object type.
//Invalidate  public  Schedules a control repaint.
//IsCustomStyleActive  public
//IsImplementorOf  public  Indicates whether the component implements a specified interface.
//IsLightStyleColor  public
//IsRightToLeft  public  Indicates whether the control should be reversed right to left.
//ItemAtPos  public  Returns the index of the list box item indicated by the coordinates of a point on the control.
//ItemRect  public  Returns the rectangle that surrounds the item specified in the Item parameter.
//ManualDock  public  Docks the control.
//ManualFloat  public  Undocks the control.
//MethodAddress  public  Returns the address of a class method by name.
//MethodName  public  Returns the name of a class method by address.
//MouseWheelHandler  public  Dispatches messages received from a mouse wheel.
//MoveSelection  public  Moves all selected items to another list control.
//NewInstance  public  Allocates memory for an instance of an object type and returns a pointer to that new instance.
//operator ()  public  C++ conversion of a TComponent into a System.Classes.IInterfaceComponentReference or a System.IInterface, depending on casting.
//PaintTo  public  Draws the windowed control to a device context.
//ParentToClient  public  Translate parent coordinates to client coordinates.
//Perform  public  Responds as if the control received a specified Windows message.
//PreProcessMessage  public  Returns False, no matter the value of Msg.
//QualifiedClassName  public  Returns the qualified name of the class.
//Realign  public  Forces the control to realign children.
//ReferenceInterface  public  Establishes or removes internal links that cause this component to be notified when the implementer of a specified interface is destroyed.
//Refresh  public  Repaints the control on the screen.
//RemoveComponent  public  Removes a specified component specified from the component's Components list.
//RemoveControl  public  Removes a specified control from the Controls array.
//RemoveFreeNotification  public  Disables the destruction notification that was enabled by FreeNotification.
//Repaint  public  Repaints the entire control.
//ReplaceDockedControl  public  Docks the control in the place where another control is already docked.
//SafeCallException  public  Handles exceptions in methods declared using the safecall calling convention.
//ScaleBy  public  Rescale control and its children.
//ScaleForPPI  public
//ScaleRectSize  public
//ScaleValue  public
//ScreenToClient  public  Converts the screen coordinates of a specified point on the screen to client coordinates.
//ScrollBy  public  Scroll control contents.
//SelectAll  public  Selects the entire block of text in the list box.
//SendToBack  public  Puts a windowed control behind all other windowed controls, or puts a non-windowed control behind all other non-windowed controls.
//SetBounds  public  Sets the windowed control's boundary properties all at once.
//SetDesignVisible  public
//SetFocus  public  Gives the input focus to the control.
//SetParentComponent  public  Sets the parent for the control.
//SetSubComponent  public  Identifies whether the component is a subcomponent.
//SetTextBuf  public  Sets the text of the control.
//Show  public  Makes a control visible.
//ToString  public  Returns a string containing the class name.
//UnitName  public  Returns the name of the unit where the class is defined.
//UnitScope  public  Returns the class's unit scope.
//Update  public  Forces the control to update.
//UpdateAction  public  Updates the state of an action.
//UpdateControlState  public  Respond to state change.
//UseRightToLeftAlignment  public  Specifies whether the control's alignment should be reversed right-to-left.
//UseRightToLeftReading  public  Specifies whether the control is using a right-to-left reading order.
//UseRightToLeftScrollBar  public  Specifies whether the vertical scroll bar appears on the left side of the control.
//---------------------------------------------------------------------------
__fastcall STRUCT_A::STRUCT_A()
{
  player = -1;
  duration = 0;

  len_path = 0;
  len_name = 0;
  len_artist = 0;
  len_album = 0;

  path[0] = NULLCHAR;
  name[0] = NULLCHAR;
  artist[0] = NULLCHAR;
  album[0] = NULLCHAR;
}
//---------------------------------------------------------------------------
__fastcall STRUCT_B::STRUCT_B()
{
  player = -1;
  state = 0;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormCreate(TObject *Sender)
{
  m_bInhibitFlash = false;
  m_bDoubleClick = false;
  m_bCheckClick = false;
  m_Duration = 0;
  m_PrevState = 0;
  m_failSafeCounter = 0;
  m_TimerMode = TM_NULL;
  m_bSkipFilePrompt = false;
  m_bOpening = false;

  // properties
  FPlayIdx = -1;
  FTempIdx = -1;
  FTargetIdx = -1;
  FPlayerId = -1; // 0 for player A, 1 for PlayerB
  FOldMouseItemIndex = -1;
  FuniqueNumber = 0;
  FTextColor = clWhite; // this is used in CheckBoxDrawItem()
  FWmp = NULL;
  FOtherWmp = NULL;
  FEditMode = false;
  FPlayPreview = false;
  FKeySpaceDisable = false;
  pOFMSDlg = NULL;

  // NOTE: color is overridden by the visual styles in RAD Studio!
  Color = DEFAULT_PLAYLIST_COLOR;
  CheckBox->Color = DEFAULT_PLAYLIST_COLOR;
  CheckBox->Parent = this;

//  CheckBox->OnMouseMove = CheckBoxMouseMove;

  pCacheList = new TStringList();
  Application->CreateForm(__classid(TProgressForm), &pProgress);

  // Start processing custom windows messages (for WM_SETTEXT to Unicode)
//  OldWinProc = WindowProc;
//  WindowProc = CustomMessageHandler;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormShow(TObject *Sender)
{
  //enable drag&drop files
  ::DragAcceptFiles(this->Handle, true);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  if (pProgress && pProgress->Count)
  {
    for (int ii = 0; ii < pProgress->Count; ii++)
      pProgress->Canceled = true;

    CanClose = false;
    ShowMessage("This list is busy, cancelling... try again in 15 seconds.");
  }
  else
  {
    CanClose = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormClose(TObject *Sender, TCloseAction &Action)
{
  //disable drag&drop files
  ::DragAcceptFiles(this->Handle, false);

  StopPlayPreview();
  DestroyFileDialog();
  DestroyImportDialog();
  DestroyExportDialog();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormDestroy(TObject *Sender)
{
  ClearAndStop();
  DestroyProgressForm(); // have to do this here since it's created in FormCreate()!

  if (pCacheList)
  {
    delete pCacheList;
    pCacheList = NULL;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y)
{
  // restart debounce timer
  MouseMoveDebounceTimer->Enabled = false;
  MouseMoveDebounceTimer->Interval = 500;
  MouseMoveDebounceTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::MouseMoveDebounceTimerEvent(TObject *Sender){
  StartPlayPreview();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::GeneralPurposeTimerEvent(TObject *Sender)
// Scroll to FPlayIdx item
{
  // timeout after user has pressed left, right up or down before
  // we exit Edit mode
  if (m_TimerMode == TM_SCROLL_KEY_PRESSED)
  {
    // this will set the item to the one the user has scrolled to
    // when SetTitle() is called
    if (!IsPlayOrPause())
      FPlayIdx = CheckBox->ItemIndex;
    FEditMode = false;
    QueueToIndex(CheckBox->ItemIndex);
    FEditMode = true;

    ExitEditModeClick(NULL);
  }

  if (FEditMode)
    return;

  switch(m_TimerMode)
  {
    case TM_START_PLAYER:
      GeneralPurposeTimer->Enabled = false;
      StartPlayer();
    break;

    case TM_FADE:

      GeneralPurposeTimer->Enabled = false;

      if (OtherForm->UnplayedSongsInList()){
#if DEBUG_ON
        MainForm->CWrite( "\r\nTPlaylistForm: TM_FADE unplayed songs found in other list. calling MainForm->ForceFade()\r\n");
#endif
        MainForm->ForceFade();
      }
      else
        NextSong(true);
    break;

    case TM_NEXT_SONG:
      GeneralPurposeTimer->Enabled = false;
      NextSong(true); // force start of player
    break;

    case TM_NEXT_SONG_CHECK:

      GeneralPurposeTimer->Enabled = false;

      if (!OtherForm->UnplayedSongsInList())
        NextSong(true);

    break;

    case TM_STOP_PLAYER:
      GeneralPurposeTimer->Enabled = false;
      StopPlayer();
    break;

    case TM_STOP_PLAY_PREVIEW:
      GeneralPurposeTimer->Enabled = false;
      StopPlayPreview();
    break;

    case TM_CHECKBOX_CLICK:

      GeneralPurposeTimer->Enabled = false;

      if (m_bDoubleClick){

        if (!Wmp){
          m_bDoubleClick = false;
          m_bCheckClick = false;
          return;
        }

        // Check the item so GetNext() will queue it
        if (!IsStateGrayed(CheckBox, CheckBox->ItemIndex))
          SetGrayedState(CheckBox->ItemIndex);

        // sometimes we have a stopped player but FPlayIdx was not set to -1
        // this should not be needed if we eventually fix that...
        if (FPlayIdx >= 0 && !IsPlayOrPause())
          FPlayIdx = -1;

        int savePlayIdx = FPlayIdx;
        int saveTargetIdx = FTargetIdx;

        Wmp->URL = GetNext(CheckBox->ItemIndex); // tell GetNext() where to start search...

        // Currently playing song's checkbox may need to be cleared...
        ClearCheckState(savePlayIdx, false);

        // keep song queued-up prior to user's double-click
        if (saveTargetIdx >= 0){
          FTargetIdx = saveTargetIdx;
// uncommenting this causes active list to jump to the targetidx song after
// double-clicking a song to start it playing... not desirable? I think not.
//          CheckBox->ItemIndex = FTargetIdx;
        }

        if (FPlayIdx >= 0){
          m_bSkipFilePrompt = true; // don't ask for other-player files

          // Start this player with new song
          StartPlayer();
        }
      }
      else if (m_bCheckClick){

        if (Wmp == NULL){
          m_bDoubleClick = false;
          m_bCheckClick = false;
          return;
        }

        // disable highlight-scroll and flasher
        FlashTimer->Enabled = false;

        // we just clicked the song's checkbox and now it's gray (enabled,
        // but not playing), queue it up...
        if (CheckBox->State[CheckBox->ItemIndex] == cbGrayed){ // don't use IsStateGrayed() here!
#if DEBUG_ON
          MainForm->CWrite("\r\nTM_CHECKBOX_CLICK:m_bCheckClick: 0 (State == cbGrayed)\r\n");
#endif
          SetItemState(CheckBox->ItemIndex); // set the TPlayerURL state to match the list-box item's state
          QueueToIndex(CheckBox->ItemIndex);
        }
        else{
          ClearCheckState(CheckBox->ItemIndex, false);

          // nothing playing?
          if (FPlayIdx < 0 || !IsPlayOrPause()){
#if DEBUG_ON
            MainForm->CWrite("\r\nTM_CHECKBOX_CLICK:m_bCheckClick: 1 (State != cbGrayed): queue next item\r\n");
#endif
            // if we unchecked an item that was queued, queue the
            // next item...
            GetNext(CheckBox->ItemIndex);
          }
          else if (FPlayIdx == CheckBox->ItemIndex){
#if DEBUG_ON
            MainForm->CWrite("\r\nTM_CHECKBOX_CLICK:m_bCheckClick: 2 (State != cbGrayed): start timer fade\r\n");
#endif
            SetTimer(TM_FADE);
          }
          else if (FTargetIdx == CheckBox->ItemIndex){
#if DEBUG_ON
            MainForm->CWrite("\r\nTM_CHECKBOX_CLICK:m_bCheckClick: 3 (State != cbGrayed): GetNext(true)\r\n");
#endif
            GetNext(CheckBox->ItemIndex, true);
          }
          else{
#if DEBUG_ON
            MainForm->CWrite("\r\nTM_CHECKBOX_CLICK:m_bCheckClick: 4 (State != cbGrayed): no action SetTitle()\r\n");
#endif
          }
        }

        SetTitle();
      }
      else{ // single click an item

        if (!Wmp){
          m_bDoubleClick = false;
          m_bCheckClick = false;
          return;
        }

        // disable highlight-scroll and flasher
        FlashTimer->Enabled = false;

        // If we clicked on an item (not a checkbox)... queue it
        // or if we checked an unchecked box... queue it
        // FTempIdx will have the ItemIndex from the CheckBoxMouseDown()
        // event
        if (FTempIdx >= 0 && FTempIdx < CheckBox->Count){
#if DEBUG_ON
          MainForm->CWrite("\r\nTM_CHECKBOX_CLICK: NOT m_bCheckClick: FTempIdx in-bounds...\r\n");
#endif
          // Check the item so GetNext() will queue it
          if (!IsStateGrayed(CheckBox, FTempIdx)){
            SetGrayedState(FTempIdx);
#if DEBUG_ON
            MainForm->CWrite("\r\nTM_CHECKBOX_CLICK: NOT m_bCheckClick: (State != cbGrayed) setting grayed state...\r\n");
#endif
          }

          // If we were playing, may need to Check the item
          if (IsPlayOrPause() && FPlayIdx >= 0 && FPlayIdx < CheckBox->Count &&
                                       !IsStateChecked(CheckBox, FPlayIdx)){
            SetCheckedState(FPlayIdx);
#if DEBUG_ON
            MainForm->CWrite("\r\nTM_CHECKBOX_CLICK: NOT m_bCheckClick: (State != cbGrayed) setting checked state...\r\n");
#endif
          }

          // Copy the clicked file to the cache - (however, we still might be
          // drag-dropping it someplace...)
          pFC->CopyFileToCache(this, FTempIdx);

          // don't queue item under mouse-pointer if mouse
          // button down for pending drag-drop...
          if((GetKeyState(VK_LBUTTON) & 0x8000) == 0){
            this->QueueToIndex(FTempIdx);
            FTempIdx = -1;
          }
        }
      }

      m_bDoubleClick = false;
      m_bCheckClick = false;

    break;

    default:
      GeneralPurposeTimer->Enabled = false;
      m_TimerMode = TM_NULL;
    break;
  };
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::UnplayedSongsInList(void)
{
  for (int ii=0; ii < CheckBox->Count; ii++)
    if (CheckBox->State[ii] != cbUnchecked)
      return true;
  return false;

//  GetNext(0, true); // set bNoSet since we're just checking
//  return (FTargetIdx < 0) ? false : true;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FlashTimerEvent(TObject *Sender)
{
  // 10=ready/stop
  // 1=stopped
  // 2=paused
  // 3=playing
  static bool bFlashOn = false;

  if (FEditMode || FPlayPreview || !Wmp || GeneralPurposeTimer->Enabled)
  {
    if (CheckBox->ItemIndex != -1)
      CheckBox->ItemIndex = -1;
    return;
  }

  if (Active)
  {
    if (CheckBox->ItemIndex != FTargetIdx && IsItemVisible(FTargetIdx))
      CheckBox->ItemIndex = FTargetIdx;
  }
  // if no items, or the player's index is -1, or player is in an unknown state, turn off selection
  else if (!CheckBox->Count || FPlayIdx < 0 || Wmp->playState == WMPOpenState::wmposUndefined)
  {
    if (CheckBox->ItemIndex != -1)
      CheckBox->ItemIndex = -1;
  }
  // if player is in pause or in play-mode
  else if (IsPlayOrPause())
  {
    // Flash faster if in pause
    if (Wmp->playState == WMPPlayState::wmppsPaused)
    {
      if (FlashTimer->Interval != 250)
        FlashTimer->Interval = 250;
    }
    else if (FlashTimer->Interval != 500)
      FlashTimer->Interval = 500;

    if (bFlashOn)
    {
      if (CheckBox->ItemIndex != -1)
        CheckBox->ItemIndex = -1;
      bFlashOn = false;
    }
    else
    {
      CheckBox->ItemIndex = FPlayIdx;
      bFlashOn = true;
    }
  }
  // if player is in stop or ready mode
  else if (CheckBox->ItemIndex != FPlayIdx && IsItemVisible(FPlayIdx))
    CheckBox->ItemIndex = FPlayIdx;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxClickCheck(TObject *Sender)
{
  if (!FEditMode && !FPlayPreview)
  {
    m_bCheckClick = true;

    // when clicking a check-box with the mouse, CheckBoxClick() (below)
    // is also triggered, which starts TM_CHECKBOX_CLICK - but just pressing the
    // spacebar toggles the check-state and triggers CheckBoxClickCheck() but NOT
    // CheckBoxClick()! So - SetTimer(TM_CHECKBOX_CLICK, TIME_300) here also
    // fixes the problem - otherwise, the checkbox goes unprocessed...
    SetTimer(TM_CHECKBOX_CLICK, TIME_300);

#if DEBUG_ON
    MainForm->CWrite("\r\nTPlaylistForm::CheckBoxClickCheck()\r\n");
#endif
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxClick(TObject *Sender)
{
  // problem I'm having is that this event happens when mouse button is released on another
  // item after a list item drag-drop. need to ignore it if the index has changed.
  // the original index was saved in FTempIdx. The drop event might not have happened
  // yet - we need ItemAtPos and to get the mouse X,Y coordinates.
  //  if (FTempIdx != CheckBox->ItemAtPos(Mouse->CursorPos, true))
  //    ShowMessage("moved");
  if (!FEditMode && !FPlayPreview)
  {
    SetTimer(TM_CHECKBOX_CLICK, TIME_300);
#if DEBUG_ON
    MainForm->CWrite("\r\nTPlaylistForm::CheckBoxClick() TM_CHECKBOX_CLICK timer started!\r\n");
#endif
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxDblClick(TObject *Sender)
{
  if (!FEditMode && !FPlayPreview){
    m_bDoubleClick = true;
#if DEBUG_ON
    MainForm->CWrite("\r\nTPlaylistForm::CheckBoxDblClick m_bDoubleClick set!\r\n");
#endif
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormActivate(TObject *Sender)
{
  // disable highlight-scroll and flasher
  SetTitle();

  // when this playlist gains focus, we want the other playlist to
  // stop flashing a playing selection...
  OtherForm->SetTitle();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormDeactivate(TObject *Sender)
{
//  if (FEditMode)
//    ExitEditModeClick(NULL);

  StopPlayPreview();

  if (Visible)
    SetTitle();

  // other playlist needs to start flashing a playing selection...
  if (OtherForm->Visible)
    OtherForm->SetTitle();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormHide(TObject *Sender)
// This will fire first, then Deactivate fires...
{
  FlashTimer->Enabled = false;
}
//---------------------------------------------------------------------------
// We need to get listbox item index when popup menu right-click
// is performed. We save it in FTempIdx for use by various
// routines in the popup menu, such as getting the song's tag-info.
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxContextPopup(TObject *Sender, TPoint &MousePos,
          bool &Handled)
{
  FTempIdx = CheckBox->ItemAtPos(MousePos, true);
  Handled = false;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxMouseDown(TObject *Sender,
               TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (Button != mbLeft)
    return;

  try
  {
    if (!FEditMode && !FPlayPreview && CheckBox->Count)
    {
      int Index = CheckBox->ItemAtPos(Point(X,Y), true);

      // FTempIndex is used in timer-event TM_CHECKBOX_CLICK
      // and in MyMoveSelected() for non-EditMode drag-drop
      if (Index >= 0 && Index < CheckBox->Count)
      {
        FTempIdx = Index;

        // MyMoveSelected() needs a selected item!
// don't like forcing select...
//        CheckBox->Selected[FTempIdx] = true;
      }
    }
  }
  catch(...) { }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxDragDrop(TObject *Sender, TObject *Source, int X, int Y)
// User has released a dragged item(s) over the destination list...
{
  if (!Sender->ClassNameIs("TCheckListBox") || !Source->ClassNameIs("TCheckListBox"))
    return;

  TCheckListBox *DestList = (TCheckListBox*)Sender;
  TCheckListBox *SourceList = (TCheckListBox*)Source;
  MyMoveSelected(DestList, SourceList, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::MoveSelectedClick(TObject *Sender)
{
  MyMoveSelected(OtherForm->CheckBox, CheckBox);
}
//---------------------------------------------------------------------------
// x and y are default mouse coordinates (-1)
void __fastcall TPlaylistForm::MyMoveSelected(TCheckListBox *DestList, TCheckListBox *SourceList, int x, int y)
// PopupMenu2 F4
{
  TStringList *sl = NULL;

  try
  {
    TPlaylistForm *SourceForm = (TPlaylistForm*)SourceList->Parent;
    TPlaylistForm *DestForm = (TPlaylistForm*)DestList->Parent;

    // get destination list index
    int DestIndex;
    if (x >= 0 && y >= 0)
      DestIndex = DestList->ItemAtPos(Point(x, y), true);
    else
      DestIndex = DestList->Items->Count; // insert at bottom

    // mark the DestIndex item so we can find it after deleting
    // selected items...
    TObject *SavePointer;
    if (DestIndex < DestList->Items->Count && DestIndex >= 0)
    {
      SavePointer = DestList->Items->Objects[DestIndex];
      DestList->Items->Objects[DestIndex] = NULL; // mark it with NULL
    }
    else
      SavePointer = NULL;

    sl = new TStringList();

    // add selected items to sl and delete them
    if (SourceForm->InEditMode)
    {
      int iCount = SourceList->Count;
      pProgress->Init(iCount);

      for (int ii = iCount-1; ii >= 0; ii--)
      {
        if (SourceList->Selected[ii])
        {
          // prevent move to a selected item in same list
          if (SourceForm == DestForm)
          {
            if (DestIndex == ii)
            {
              ShowMessage("You can't move selected items onto a selected item!");
              return; // finally will clean up...
            }
          }
          // skip move of playing song to other player's list
          else if (IsStateChecked(SourceList, ii))
            continue;

          TPlayerURL *p = (TPlayerURL*)SourceList->Items->Objects[ii];
          if (p)
            p->state = SourceList->State[ii]; // cbChecked indicates a playing song

          sl->AddObject(SourceList->Items->Strings[ii], (TObject*)p);
          SourceList->Items->Delete(ii);
        }

        if (pProgress->Move(ii))
          return;
      }
    }
    else // not in edit-mode - dragging a single item from playlist...
    {
      int idx = SourceForm->TempIndex;

#if DEBUG_ON
      MainForm->CWrite("\r\nFTempIdx: " + String(idx) + "\r\n");
#endif
      if (idx < 0 || idx >= SourceForm->Count)
        return;

      // prevent move to a selected item in same list
      if (SourceForm == DestForm)
      {
        if (DestIndex == idx)
        {
          ShowMessage("You can't move selected items onto a selected item!");
          return; // finally will clean up...
        }
      }
      // skip move of playing song to other player's list
      // (cbChecked indicates a playing song)
      else if (!IsStateChecked(SourceList, idx))
      {
        TPlayerURL *p = (TPlayerURL*)SourceList->Items->Objects[idx];
        if (p)
          p->state = SourceList->State[idx];

        sl->AddObject(SourceList->Items->Strings[idx], (TObject*)p);
        SourceList->Items->Delete(idx);
      }
    }

    // find the new location of DestIndex from our NULL marker...
    DestIndex = DestList->Items->Count; // if NULL not found use end of list
    for (int ii = 0; ii < DestList->Items->Count; ii++)
    {
      if (DestList->Items->Objects[ii] == NULL)
      {
         DestList->Items->Objects[ii] = SavePointer; // restore pointer
         DestIndex = ii; // new DestIndex!
         break;
      }
    }

    if (DestIndex < 0)
    {
      ShowMessage("Error: DestIndex < 0!");
      return; // __ finally will clean up...
    }

    // add stringlist items into destination list
    for (int ii = 0; ii < sl->Count; ii++)
    {
      TPlayerURL *p = (TPlayerURL*)sl->Objects[ii];
      DestList->Items->InsertObject(DestIndex, sl->Strings[ii], (TObject*)p);

      if (p)
      {
        DestList->State[DestIndex] = p->state;

        // remove pertinent cache files...
        if (SourceForm != DestForm)
        {
          if (p->cacheNumber > 0)
            pFC->DeleteCacheFile(SourceForm, p->cacheNumber);

          p->cacheNumber = 0;
        }
      }
      else
      {
        ShowMessage("Error: Missing TPlayerURL object! (" + String(ii) + ")");
        return; // __finally will clean up...
      }
    }

    // scan source-list to reset the playing song's FPlayIdx and ItemIndex
    int SourceFPlayIdx = -1;
    for (int ii = 0 ; ii < SourceList->Count ; ii++)
    {
      if (IsStateChecked(SourceList, ii))
      {
        SourceFPlayIdx = ii;
        break;
      }
    }

    // TODO!!!!!!!!!!!!!!!!!
    // might want to add a field in p for the index of the next queued song and
    // be able to restore that for both source and dest lists...
    // - for now, force it to the first song
    if (SourceFPlayIdx < 0 && SourceList->Count)
      SourceFPlayIdx = 0;

    SourceForm->FPlayIdx = SourceFPlayIdx;
    SourceList->ItemIndex = SourceFPlayIdx;

    if (SourceList->Count)
    {
      // Queue a song in the source list
      if (SourceForm != DestForm)
      {
        pFC->CopyFileToCache(SourceForm, SourceForm->FPlayIdx);
        SourceForm->QueueToIndex(SourceForm->FPlayIdx);
      }
      else
      {
        pFC->CopyFileToCache(SourceForm, DestIndex);
        SourceForm->QueueToIndex(DestIndex);
      }
    }

    // Exit edit-mode
    if (SourceForm->FEditMode)
      SourceForm->ExitEditModeClick(NULL);

    if (SourceForm != DestForm)
    {
      // scan destination-list to reset the playing song's FPlayIdx and ItemIndex
      int DestFPlayIdx = -1;
      for (int ii = 0 ; ii < DestList->Count ; ii++)
      {
        if (IsStateChecked(DestList, ii))
        {
          DestFPlayIdx = ii;
          break;
        }
      }

      DestForm->FPlayIdx = DestFPlayIdx;
// we don't want to jump to the playing song in destination list
// after a drag-drop...
//      DestList->ItemIndex = DestFPlayIdx;

      // Queue a song in the destination list
      if (DestIndex >= 0)
        DestForm->QueueToIndex(DestIndex);

      // Exit edit-mode
      if (DestForm->FEditMode)
        DestForm->ExitEditModeClick(NULL);
    }
  }
  __finally
  {
    if (sl)
      delete sl;

    // set flag true if this is the "top-level" method in the overall progress-chain...
    // (it deletes all the history of prior progressbar nestings)
    pProgress->UnInit(true);

//    if (DestForm != SourceForm)
//      MainForm->ShowPlaylist(DestForm);
  }
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::IsItemVisible(int idx)
{
  if (CheckBox->Count == 0 || idx < 0)
    return false;

  int topIndex = CheckBox->TopIndex;
  int numItemsVisible = CheckBox->ClientHeight/CheckBox->ItemHeight;
  int bottomIndex = topIndex + numItemsVisible;
  return idx >= topIndex && idx < bottomIndex;
}
//---------------------------------------------------------------------------
// bAllowDequeue defaults true
void __fastcall TPlaylistForm::DeleteListItem(int idx, bool bDeleteFromCache)
{
  try
  {
    if (idx < 0 || idx >= CheckBox->Count)
      return;

    // it's messy to remove an item that's playing... taking the easy way out!
    if (FPlayIdx == idx && IsPlayOrPause())
      return;

    int SaveTargetIdx = FTargetIdx;

    if (FTargetIdx > idx)
      FTargetIdx--;
    if (FPlayIdx > idx)
      FPlayIdx--;

    // delete TPlayerURL object
    if (MainForm->CacheEnabled)
    {
      TPlayerURL *p = (TPlayerURL*)CheckBox->Items->Objects[idx];
      if (p)
      {
        // delete a specific cached-file (if it exists)
        if (bDeleteFromCache && p->cacheNumber > 0)
          pFC->DeleteCacheFile(this, p->cacheNumber);

        delete p;
      }
    }

    // remove item from list
    CheckBox->Items->Delete(idx);

    if (!CheckBox->Count)
    {
      FTargetIdx = -1;
      FPlayIdx = -1;
    }

    if (SaveTargetIdx == idx)
      GetNext(idx, true);
  }
  catch(...) { ShowMessage("Error deleting list index: " + String(idx)); }
}
//---------------------------------------------------------------------------
// overloaded... (called from Main.cpp AddFileToListBox()

void __fastcall TPlaylistForm::AddListItem(String s)
{
  TPlayerURL *p = InitTPlayerURL(s);
  AddListItem(s, p);
}

void __fastcall TPlaylistForm::AddListItem(String s, TPlayerURL *p)
{
  String sTags = (p == NULL) ? s : GetTags(p);
  CheckBox->Items->AddObject(sTags, (TObject*)p);
  SetGrayedState(CheckBox->Count-1);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::InsertListItem(int idx, String s, TPlayerURL *p)
{
  CheckBox->Items->InsertObject(idx, s, (TObject*)p);
  SetGrayedState(idx);
}
//---------------------------------------------------------------------------
//EdtTitle.Text  := MainAudioFile.Title;
//EdtArtist.Text := MainAudioFile.Artist;
//EdtAlbum.Text  := MainAudioFile.Album;
//EdtGenre.Text  := MainAudioFile.Genre;
//EdtYear.Text   := MainAudioFile.Year;
//EdtTrack.Text  := MainAudioFile.Track;
//Memo1.Clear;
//Memo1.Lines.Add(Format('Type:      %s',       [MainAudioFile.FileTypeName] ));
//Memo1.Lines.Add(Format('FileSize   %d Bytes', [MainAudioFile.FileSize]     ));
//Memo1.Lines.Add(Format('Duration   %d sec',   [MainAudioFile.Duration]     ));
//Memo1.Lines.Add(Format('Btrate     %d kBit/s',[MainAudioFile.Bitrate div 1000]));
//Memo1.Lines.Add(Format('Samplerate %d Hz',    [MainAudioFile.Samplerate]   ));
//Memo1.Lines.Add(Format('Channels:  %d',       [MainAudioFile.Channels]     ));
String __fastcall TPlaylistForm::GetTags(TPlayerURL *p)
{
  if (!p) return "(TPlayerURL object missing!)";

  TGeneralAudioFile *f = NULL;
  String sTitle, sAlbum, sArtist, sDuration;
//  int iFileSize = 0;
  try
  {
    f = new TGeneralAudioFile(p->path);
    if (f)
    {
      sTitle = f->Title;
      sAlbum = f->Album;
      sArtist = f->Artist;
      sDuration = f->Duration;
//      iFileSize = f->FileSize;
    }
  }
  __finally
  {
   if (f) delete f;
  }

  String sOut;
  if (sTitle == "" || sArtist == "" ||
     (sTitle.Length() >= 6 && sTitle.SubString(1, 6) == "Track "))
    sOut = p->path;
  else
    sOut = sTitle + " (" + sArtist + ")";

  return sOut;
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::RestoreCache(String sCachePath)
{
  // reset/restore all song-file TPlayerURL structs
  for (int ii = 0; ii < CheckBox->Count; ii++)
  {
    TPlayerURL *p = (TPlayerURL*)CheckBox->Items->Objects[ii];
    if (p)
    {
      InitTPlayerURL(p);
      CheckBox->Items->Strings[ii] = GetTags(p);
    }
    else
      CheckBox->Items->Strings[ii] = "(OBJECT MISSING)";
  }
  this->CacheList->Clear();
  this->CachePath = sCachePath;
  return true;
}
//---------------------------------------------------------------------------
TPlayerURL *__fastcall TPlaylistForm::InitTPlayerURL(String s)
{
  TPlayerURL *p = new TPlayerURL();
  if (!p) return p;
  p->path = s;
  return InitTPlayerURL(p);
}

// call if object already exists and has a valid p->path
TPlayerURL *__fastcall TPlaylistForm::InitTPlayerURL(TPlayerURL *p)
{
  if (!p) return NULL;
  p->color = FTextColor;
  p->bDownloaded = false;
  p->bIsUri = MainForm->IsUri(p->path);
  p->cacheNumber = 0; // 0 = not yet cached
  p->state = cbGrayed;
  return p;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxDragOver(TObject *Sender,
          TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
  // Accept the item if its from a TCheckListBox
  Accept = Source->ClassNameIs("TCheckListBox");
}
//----------------------------------------------------------------------------
/*
void __fastcall TPlaylistForm::CustomMessageHandler(TMessage &msg)
// Allow Unicode Window Caption (the Caption property is ANSI-only)
{
  if (msg.Msg == WM_SETTEXT)
  {
    if (msg.LParam != NULL)
    {
      String w = String((char*)msg.LParam);
      DefWindowProcW(this->Handle, msg.Msg, 0, (LPARAM)w.c_bstr());
    }
    else
      this->OldWinProc(msg); // Call main handler
  }
  else
    this->OldWinProc(msg); // Call main handler
}
*/
// can't get scroll notifications without subclassing the list control! (sadly)
//---------------------------------------------------------------------------
//void __fastcall TPlaylistForm::WMVListScroll(TWMScroll &Msg)
// int Msg.Msg, int Msg.Result, wchar_t *Msg.Text
//{
//  HWND hSB = Msg.ScrollBar;
//#if DEBUG_ON
//    MainForm->CWrite("\r\nVScroll: ScrollCode: \"" + String(Msg.ScrollCode) +
//     "\", Result:" + String(Msg.Result) + ", Msg:" + String(Msg.Msg) + ", Pos:" + String(Msg.Pos) + "\r\n");
//#endif
//}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::WMListDropFile(TWMDropFiles &Msg)
{
  pPH->LoadListWithDroppedFiles(this, Msg);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::WMSetText(TWMSetText &Msg)
// Allow Unicode Window Caption (the Caption property is ANSI-only)
{
  if (Msg.Text != NULL)
  {
    String w = Msg.Text;
    DefWindowProc(this->Handle, Msg.Msg, 0, (LPARAM)w.w_str());
    Msg.Result = TRUE;
  }
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::QueueFirst(void)
{
  return QueueToIndex(0);
}

bool __fastcall TPlaylistForm::QueueToIndex(int idx)
{
  if (!Wmp || !CheckBox->Count)
    return false;

  try
  {
    if (idx < 0)
      idx = 0;

    // Can't Set URL or we stop the player! If player playing or paused... don't set URL
    // because it will stop the current song
    if (!IsPlayOrPause())
      Wmp->URL = GetNext(idx);

    int ct = CheckBox->Count;
    int iTarg = idx;
    for (int ii = 0; ii < ct; ii++){
      if (IsStateGrayed(CheckBox, iTarg))
        break;

      // wrap to beginning of list to search
      if (++iTarg >= ct)
        iTarg = 0;
    }

    FTargetIdx = iTarg;

    SetTitle();

    return true;
  }
  catch(...) { return false; }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::NextSong(bool bForceStartPlay)
{
  if (!Wmp) return;

  try
  {
    bool bWasPlaying = IsPlayOrPause() ? true : false; // playing?

    int savePlayIdx = FPlayIdx;

    String sFile = GetNext(FTargetIdx, false, true); // enable random

  #if DEBUG_ON
    MainForm->CWrite("\r\nNextSong() FPlayIdx:" + String(FPlayIdx) +
     " Target:" + String(FTargetIdx) + " File: \"" + String(sFile) +
      "\" bForceStartPlay: " + (bForceStartPlay ? "yes" : "no") + "\r\n");
  #endif

  // Old checkbox may need to be cleared...
    ClearCheckState(savePlayIdx);

    if (!sFile.IsEmpty())
    {
      Wmp->URL = sFile;

#if DEBUG_ON
      MainForm->CWrite("\r\nCall SetTitle\r\n");
#endif
      SetTitle();

      if (bWasPlaying || bForceStartPlay)
      {
        m_bSkipFilePrompt = true;

#if DEBUG_ON
        MainForm->CWrite("\r\nCall StartPlayer\r\n");
#endif
        // Start player
        StartPlayer();
      }
    }
    else if (!MainForm->ForceFade())  // no more checked items
      StopPlayer(); // Stop player
  }
  catch(...)
  {
#if DEBUG_ON
    MainForm->CWrite("\r\nException thrown in NextSong()\r\n");
#endif
  }
}
//---------------------------------------------------------------------------
// Gets a UTF-8 string from the listbox and converts it to UTF-16
String __fastcall TPlaylistForm::GetNext(int iNext, bool bNoSet, bool bEnableRandom)
// Given pointer to a Form containing a listbox, returns the first file-name
// that has its check-box grayed (queued song).
//
// Set bNoSet true to cause FPlayIdx to be unaffected, instead the next grey-checked
// item is returned in FTargetIdx.
//
// BEFORE calling this function, set iNext to a >= 0 value to force
// searching to begin there. Set iNext -1 to begin the search at FPlayIdx+1.
//
// NOTE: this->FPlayIdx is used to hold the list-index of the currently playing song.
// iNext is the start-index of the search on entry. It's set to -1 on return.
// On return, FPlayIdx is set to the next available song and FTargetIdx is set to the
// next available song (grey-checked item) that follows FPlayIdx.
{
  String sFile;

#if DEBUG_ON
  MainForm->CWrite( "\r\nTPlaylistForm::GetNext() (onenter): NextIdx=" +
      String(iNext) +  ", FPlayIdx=" + String(FPlayIdx) +  ", TargetIdx=" +
       String(FTargetIdx) + ", Player: " + (FPlayerId == PLAYER_A_ID ? String("A") : String("B")) + "\r\n");
#endif
  try
  {
    try
    {
      int count = CheckBox->Count;

      // If Form pointer is null or no items in listbox, return ""
      if (!count)
      {
        FPlayIdx = -1;
        FTargetIdx = -1;
#if DEBUG_ON
        MainForm->CWrite("\r\nTPlaylistForm::GetNext: Empty list, RETURNING EMPTY!\r\n");
#endif
        return "";
      }

      if (iNext >= count)
        iNext = 0;

      bool bListReset = (iNext >= 0) ? true : false;

      // Random (Shuffle) play?
      bool bRandom = false;

      if (bEnableRandom)
        bRandom = (((PlayerID == PLAYER_A_ID) && MainForm->ShuffleModeA) ||
          (!(PlayerID == PLAYER_B_ID) && MainForm->ShuffleModeB)) ? true : false;

      int loops;

      if (bNoSet)
        loops = 1;
      else
      {
        if (bRandom)
        {
          for (int ii = 0 ; ii < count ; ii++)
          {
            iNext = ::Random(count);

            if (IsStateGrayed(CheckBox, iNext))
              break;
          }
        }

        loops = 2;
      }

      for (int ii = 0 ; ii < loops ; ii++)
      {
        if (iNext < 0)
          iNext = FPlayIdx; // start at current song if NextInxex is -1

        if (iNext < 0)
        {
          FPlayIdx = -1;
          FTargetIdx = -1;
#if DEBUG_ON
          MainForm->CWrite("\r\nTPlaylistForm::GetNext: RETURNING EMPTY (FPlayIdx was -1)!!!!\r\n");
#endif
          return "";
        }

        int jj;
        for (jj = 0 ; jj < count ; jj++, iNext++)
        {
          if (iNext >= count)
            iNext = 0;

          if (IsStateGrayed(CheckBox, iNext))
          {
            if (ii == 0) // first loop...
            {
              if (!bNoSet && bListReset)
                // prefetch the next file into our temporary cache directory
                pFC->CopyFileToCache(this, iNext);

              sFile = pPH->GetURL(this, iNext);
            }

            break;
          }
        }

        if (jj == count){
          iNext = -1; // no Play-flags set
          FTargetIdx = -1;
        }

        if (iNext >= count)
        {
          ShowMessage("GetNext() FPlayIdx Index is out-of-range!");
          FPlayIdx = -1;
          FTargetIdx = -1;
        }
        else if (!bNoSet)
        {
          if (ii == 0) // first loop...
          {
            FPlayIdx = iNext;
            iNext = FPlayIdx+1;
          }
          else
          {
            FTargetIdx = iNext;

            // prefetch the next file into our temporary cache directory
            pFC->CopyFileToCache(this, FTargetIdx);
          }
        }
        else
          FTargetIdx = iNext;
      }

      iNext = -1;
      SetTitle();
    }
    catch(...) { ShowMessage("GetNext() threw an exception..."); }
  }
  __finally
  {
#if DEBUG_ON
    MainForm->CWrite( "TPlaylistForm::GetNext() (onexit): NextIdx=" +
      String(iNext) +  ", FPlayIdx=" + String(FPlayIdx) +  ", TargetIdx=" +
       String(FTargetIdx) + "\r\n");
    MainForm->CWrite( "TPlaylistForm::GetNext() sFile: \"" + sFile +  "\"\r\n\r\n");
#endif
  }

  // URL in Windows Media Player is a String... do not percent-encode!
  // Convert UTF-8 to UTF-16!
  return sFile;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::ClearCheckState(int idx, bool bRequeueIfRepeatMode)
{
  if (idx >= 0 && idx <= CheckBox->Count)
  {
    bool bRepeatMode = (PlayerID == PLAYER_A_ID) ? MainForm->RepeatModeA : MainForm->RepeatModeB;

    if (bRepeatMode && bRequeueIfRepeatMode)
      CheckBox->State[idx] = cbGrayed;
    else
      CheckBox->State[idx] = cbUnchecked;

    SetItemState(idx);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SetCheckedState(int idx)
{
  if (idx >= 0 && idx <= CheckBox->Count)
  {
    CheckBox->State[idx] = cbChecked;
    SetItemState(idx);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SetGrayedState(int idx)
{
  if (idx >= 0 && idx <= CheckBox->Count)
  {
    CheckBox->State[idx] = cbGrayed;
    SetItemState(idx);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SetItemState(int idx)
{
  TPlayerURL *p = (TPlayerURL*)CheckBox->Items->Objects[idx];
  if (p)
    p->state = CheckBox->State[idx];
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::IsStateGrayed(TCheckListBox *clb, int idx)
{
  TPlayerURL *p = (TPlayerURL*)clb->Items->Objects[idx];
  if (p)
    return (p->state == cbGrayed) ? true : false;
  return false;
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::IsStateChecked(TCheckListBox *clb, int idx)
{
  TPlayerURL *p = (TPlayerURL*)clb->Items->Objects[idx];
  if (p)
    return (p->state == cbChecked) ? true : false;
  return false;
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::IsStateUnchecked(TCheckListBox *clb, int idx)
{
  TPlayerURL *p = (TPlayerURL*)clb->Items->Objects[idx];
  if (p)
    return (p->state == cbUnchecked) ? true : false;
  return false;
}
//---------------------------------------------------------------------------
//typedef enum WMPOpenState
//{
//  wmposUndefined = 0,
//  wmposPlaylistChanging = 1,
//  wmposPlaylistLocating = 2,
//  wmposPlaylistConnecting = 3,
//  wmposPlaylistLoading = 4,
//  wmposPlaylistOpening = 5,
//  wmposPlaylistOpenNoMedia = 6,
//  wmposPlaylistChanged = 7,
//  wmposMediaChanging = 8,
//  wmposMediaLocating = 9,
//  wmposMediaConnecting = 10,
//  wmposMediaLoading = 11,
//  wmposMediaOpening = 12,
//  wmposMediaOpen = 13,
//  wmposBeginCodecAcquisition = 14,
//  wmposEndCodecAcquisition = 15,
//  wmposBeginLicenseAcquisition = 16,
//  wmposEndLicenseAcquisition = 17,
//  wmposBeginIndividualization = 18,
//  wmposEndIndividualization = 19,
//  wmposMediaWaiting = 20,
//  wmposOpeningUnknownURL = 21
//} WMPOpenState;
//
// Select a song: 1, 7, 6
// Play a song (and it's there): 21, 13
// Play a song (and it's NOT there): 21, 6
// Play a song (and it's there but wrong kind of WAV): 21, 8, 6, 12, 8, 6
// Stop while playing: 1, 8, 6, 7, 6
void __fastcall TPlaylistForm::OpenStateChange(WMPOpenState NewState)
{
  if (!Wmp) return;

  try{
    if (NewState == WMPOpenState::wmposMediaOpen){ // Media Open
#if DEBUG_ON
      MainForm->CWrite( "\r\nOpenStateChange():wmposMediaOpen: FPlayIdx:" + String(FPlayIdx) + " Target:" + String(FTargetIdx) + "\r\n");
#endif

      m_Duration = (int)Wmp->currentMedia->duration;

      if (PlayerID == PLAYER_A_ID)
        TimeDisplay(m_Duration, 1);
      else
        TimeDisplay(m_Duration, 4);

      // Green
      if (m_bOpening && FPlayIdx >= 0 && FPlayIdx < CheckBox->Count){
        TPlayerURL *p = (TPlayerURL*)CheckBox->Items->Objects[FPlayIdx];
        if (p)
          p->color = clLime;
      }

      m_bOpening = false;
    }
    else if (NewState == WMPOpenState::wmposOpeningUnknownURL){
#if DEBUG_ON
      MainForm->CWrite( "\r\nOpenStateChange():wmposOpeningUnknownURL: FPlayIdx:" + String(FPlayIdx) + " Target:" + String(FTargetIdx) + "\r\n");
#endif
      m_bOpening = true;
    }
    else if (NewState == WMPOpenState::wmposMediaOpening){
#if DEBUG_ON
      MainForm->CWrite( "\r\nOpenStateChange():wmposMediaOpening: FPlayIdx:" + String(FPlayIdx) + " Target:" + String(FTargetIdx) + "\r\n");
#endif
      m_bOpening = true;
    }
    else if (NewState == WMPOpenState::wmposPlaylistOpenNoMedia){
      // Red
#if DEBUG_ON
      MainForm->CWrite( "\r\nOpenStateChange():wmposPlaylistOpenNoMedia: FPlayIdx:" + String(FPlayIdx) + " Target:" + String(FTargetIdx) + "\r\n");
#endif
      m_bOpening = false;
    }
#if DEBUG_ON
    else if (NewState == WMPOpenState::wmposMediaChanging){
      MainForm->CWrite( "\r\nOpenStateChange():wmposMediaChanging: FPlayIdx:" + String(FPlayIdx) + " Target:" + String(FTargetIdx) + "\r\n");
    }
    else{
      MainForm->CWrite( "\r\nMisc OpenStateChange: " + String(NewState) + "\r\n");
    }
#endif
  }
  catch(...) {}

  MainForm->SetCurrentPlayer(); // Set CurrentPlayer variable (used for color-coding)
}
//---------------------------------------------------------------------------
// Called from WindowsMediaPlayer1MediaError and WindowsMediaPlayer2MediaError
// hooks in Main.cpp
void __fastcall TPlaylistForm::MediaError(LPDISPATCH Item)
{
  if (m_failSafeCounter < RETRY_A){
    if (FPlayIdx >= 0 && FPlayIdx < CheckBox->Count){
      TPlayerURL *p = (TPlayerURL*)CheckBox->Items->Objects[FPlayIdx];
      if (p){
        String sPath = p->path;

        if (MainForm->CacheEnabled){
          String sCachePath = pFC->GetCachePath(this, p);
          if (!sCachePath.IsEmpty()){
            sPath = sCachePath;
#if DEBUG_ON
            MainForm->CWrite("\r\nTPlaylistForm::MediaError() cach-file found!\r\n");
#endif
          }
#if DEBUG_ON
          else{
            MainForm->CWrite("\r\nTPlaylistForm::MediaError() no cach-file!\r\n");
          }
#endif
        }
#if DEBUG_ON
        MainForm->CWrite("\r\nTPlaylistForm::MediaError() setting WMP URL to: \"" +
                                                               sPath + "\"\r\n");
#endif
        if (p)
          p->color = clYellow;

        Wmp->URL = sPath;
        SetTimer(TM_START_PLAYER);
      }
    }

    m_failSafeCounter++;
  }
  else if (m_failSafeCounter < RETRY_B){
    // clear the check-state for the unresponsive song
    if (FPlayIdx >= 0 && FPlayIdx < CheckBox->Count){
#if DEBUG_ON
      MainForm->CWrite("\r\nMediaError(" + String(FPlayIdx) + "): Gave up on this song... running MainForm->ForceFade() or NextSong(): \"" +
                  String(Wmp->URL) + "\"\r\n");
#endif
      if (!IsStateUnchecked(CheckBox, FPlayIdx)){
        // clear the check-state for the unresponsive song
        CheckBox->State[FPlayIdx] = cbUnchecked;
        TPlayerURL *p = (TPlayerURL*)CheckBox->Items->Objects[FPlayIdx];
        if (p){
          if (p->cacheNumber > 0)
            pFC->DeleteCacheFile(this, p->cacheNumber);
          p->cacheNumber = 0;
          p->state = cbUnchecked;
          p->color = clRed;
        }
      }
    }

    SetTimer(TM_NEXT_SONG); // NextSong
    m_failSafeCounter++;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::PositionTimerEvent(TObject *Sender)
// Fires every second to check the remaining play-time
{
  // Return if this player is not playing...
  if (!Wmp || !OtherWmp || Wmp->playState != WMPPlayState::wmppsPlaying)
    return;

  try{
    if (!MainForm->ManualFade && MainForm->FadePoint != 0){
      // refer also to AutoFadeTimerEvent() in Main.cpp
      int iFadePoint = MainForm->FadePoint; // FadePoint can be 0-99 seconds before the end

      // TODO: need a solution to juggle this and AutoFadeTimerEvent() in Main.cpp
      // in the case of very short music clips!!!!!!!!!!!!!!!!!!!!!!! (below does NOT work)
      //if (iFadePoint < m_Duration)
      //{
      //  iFadePoint = m_Duration;
      //  MainForm->AutoFadeTimer->Enabled = false;
      //}

      if (OtherForm->FPlayIdx >= 0 &&
          (m_Duration-(int)Wmp->controls->currentPosition <= iFadePoint)){
        // Start Other Player
        if (!OtherForm->IsPlayOrPause())
          OtherForm->StartPlayer();

        if (PlayerID == PLAYER_A_ID){
          if (MainForm->FaderTrackBar->Position != MainForm->FaderTrackBar->Max){
            MainForm->bFadeRight = true; // Set fade-direction
            MainForm->AutoFadeTimer->Enabled = true; // Start a fade right
          }
        }
        else if (MainForm->FaderTrackBar->Position != MainForm->FaderTrackBar->Min){
          MainForm->bFadeRight = false; // Set fade-direction
          MainForm->AutoFadeTimer->Enabled = true; // Start a fade left
        }
      }
    }
  }
  catch(...) { }

  UpdatePlayerStatus();

}
//---------------------------------------------------------------------------
// Value State Description
// 0 Undefined Windows Media Player is in an undefined state.
// 1 Stopped Playback of the current media item is stopped.
// 2 Paused Playback of the current media item is paused. When
//   a media item is paused, resuming playback begins from the same location.
// 3 Playing The current media item is playing.
// 4 ScanForward The current media item is fast forwarding.
// 5 ScanReverse The current media item is fast rewinding.
// 6 Buffering The current media item is getting additional data from the server.
// 7 Waiting Connection is established, but the server is not sending data.
//   Waiting for session to begin.
// 8 MediaEnded Media item has completed playback.
// 9 Transitioning Preparing new media item.
// 10 Ready Ready to begin playing.
// 11 Reconnecting Reconnecting to stream.
//
//typedef enum WMPPlayState
//{
//  wmppsUndefined = 0,
//  wmppsStopped = 1,
//  wmppsPaused = 2,
//  wmppsPlaying = 3,
//  wmppsScanForward = 4,
//  wmppsScanReverse = 5,
//  wmppsBuffering = 6,
//  wmppsWaiting = 7,
//  wmppsMediaEnded = 8,
//  wmppsTransitioning = 9,
//  wmppsReady = 10,
//  wmppsReconnecting = 11,
//  wmppsLast = 12
//} WMPPlayState;

// sequence when media plays then stops:
// wmppsPlaying3,wmppsMediaEnded8,wmppsTransitioning9,1wmppsStopped1
void __fastcall TPlaylistForm::PlayStateChange(WMPPlayState NewState)
{
  if (Wmp == NULL || OtherWmp == NULL || FPlayPreview) return;

#if DEBUG_ON
  MainForm->CWrite( "\r\PlayStateChange: " + String(NewState) + "\r\n");
#endif

  try
  {
    if (MainForm->SendTiming) // Send Telemetry?
    {
      // 10=ready/stop
      // 1=stopped
      // 2=paused
      // 3=playing
      if (NewState == WMPPlayState::wmppsPlaying) // playing?
      {
        // "Play" state-change is received here...
        STRUCT_A sms;

        // Populate SwiftMixStruct
        GetSongInfo(sms);

        int size = sizeof(STRUCT_A);

        SendToSwiftMix(&sms, size, MainForm->RWM_SwiftMixPlay);
      }
      else
      {
        // Send data to YahCoLoRiZe
        STRUCT_B sms;

        // Populate SwiftMixStruct
        sms.player = PlayerID;
        sms.state = (int)NewState;

        int size = sizeof(STRUCT_B);

        SendToSwiftMix(&sms, size, MainForm->RWM_SwiftMixState);
      }
    }
  }
  catch(...) {}

  try
  {
    // 10=ready/stop
    // 8=media ended
    // 1=stopped
    // 2=paused
    // 3=playing
    if (NewState == WMPPlayState::wmppsReady) // song ready to play...
    {
#if DEBUG_ON
      MainForm->CWrite( "\r\nWMPPlayState():wmppsReady " + String(FPlayerId == PLAYER_A_ID ? "A" : "B") + ": FPlayIdx=" + String(FPlayIdx) + "\r\n");
      if (FTargetIdx < 0)
        MainForm->CWrite( "(Good place to prompt for more music files via timer???\r\n");
#endif
    }
    else if (NewState == WMPPlayState::wmppsPaused) // pause?
    {
#if DEBUG_ON
      MainForm->CWrite( "\r\nWMPPlayState():wmppsPaused " + String(FPlayerId == PLAYER_A_ID ? "A" : "B") + ": FPlayIdx=" + String(FPlayIdx) + "\r\n");
#endif
      if (PlayerID == PLAYER_A_ID)
      {
        MainForm->Stop1->Checked = false;
        MainForm->Play1->Checked = false;
        MainForm->Pause1->Checked = true;
      }
      else
      {
        MainForm->Stop2->Checked = false;
        MainForm->Play2->Checked = false;
        MainForm->Pause2->Checked = true;
      }
    }
    else if (NewState == WMPPlayState::wmppsPlaying) // play?
    {
#if DEBUG_ON
      MainForm->CWrite( "\r\nWMPPlayState():wmppsPlaying " + String(FPlayerId == PLAYER_A_ID ? "A" : "B") + ": FPlayIdx=" + String(FPlayIdx) + "\r\n");
#endif
      if (FPlayIdx < 0)
    {
        // illegal to start if nothing checked
        PositionTimer->Enabled = false;

        SetTimer(TM_STOP_PLAYER);
      }
      else
      {
        // display the media info in the hint of the status-bar
        MainForm->StatusBar1->Hint = GetMediaTags();

        if (PlayerID == PLAYER_A_ID)
        {
          MainForm->Stop1->Checked = false;
          MainForm->Play1->Checked = true;
          MainForm->Pause1->Checked = false;
        }
        else
        {
          MainForm->Stop2->Checked = false;
          MainForm->Play2->Checked = true;
          MainForm->Pause2->Checked = false;
        }

        // Start this player with other player stopped and fader in wrong position
        // Autofade to this player...
        if (!MainForm->ManualFade && !MainForm->AutoFadeTimer->Enabled)
        {
          if (PlayerID == PLAYER_A_ID)
          {
            if (MainForm->FaderTrackBar->Position != MainForm->FaderTrackBar->Min)
            {
              MainForm->bFadeRight = false;
              MainForm->AutoFadeTimer->Enabled = true;
            }
          }
          else
          {
            if (MainForm->FaderTrackBar->Position != MainForm->FaderTrackBar->Max)
            {
              MainForm->bFadeRight = true;
              MainForm->AutoFadeTimer->Enabled = true;
            }
          }
        }

        SetCheckedState(FPlayIdx);

        m_failSafeCounter = 0; // reset failsafe counter

        // Need to update target index without affecting FPlayIdx if
        // we just started a manually queued (by single-click) song.
        // (So that when clicking the list-box we see the next
        // grey-checked song in the list queued...)
        if (FPlayIdx == FTargetIdx)
          GetNext(FPlayIdx, true, true); // allow random...

        m_bSkipFilePrompt = false;

        PositionTimer->Enabled = true;
        SetTitle();
      }

      MainForm->SetCurrentPlayer();
    }
    else if (NewState == WMPPlayState::wmppsStopped) // stop?
  {
#if DEBUG_ON
      MainForm->CWrite( "\r\nWMPPlayState():wmppsStopped " + String(FPlayerId == PLAYER_A_ID ? "A" : "B") + ": FPlayIdx=" + String(FPlayIdx) + "\r\n");
#endif
      PositionTimer->Enabled = false;
      UpdatePlayerStatus();
      ClearCheckState(FPlayIdx);

      if (PlayerID == PLAYER_A_ID)
      {
        MainForm->Stop1->Checked = true;
        MainForm->Play1->Checked = false;
        MainForm->Pause1->Checked = false;
      }
      else
      {
        MainForm->Stop2->Checked = true;
        MainForm->Play2->Checked = false;
        MainForm->Pause2->Checked = false;
      }

      QueueToIndex(FTargetIdx);
      SetTitle();
      MainForm->SetCurrentPlayer();
    }
    else if (NewState == WMPPlayState::wmppsMediaEnded) // Song ended?
    {
#if DEBUG_ON
      MainForm->CWrite( "\r\nWMPPlayState():wmppsMediaEnded " + String(FPlayerId == PLAYER_A_ID ? "A" : "B") + ": FPlayIdx=" + String(FPlayIdx) + "\r\n");
#endif
      if (!IsPlayOrPause())
        ClearCheckState(FPlayIdx);

      if (!MainForm->ManualFade && !MainForm->AutoFadeTimer->Enabled)
      {
        QueueToIndex(FTargetIdx);
        SetTimer(TM_FADE); // NextSong
      }
    }
#if DEBUG_ON
    else if (NewState == WMPPlayState::wmppsTransitioning)
      MainForm->CWrite( "\r\nWMPPlayState():wmppsTransitioning " + String(FPlayerId == PLAYER_A_ID ? "A" : "B") + ": FPlayIdx=" + String(FPlayIdx) + "\r\n");
    else
      MainForm->CWrite( "\r\nMisc. WMPPlayState(): " + String(NewState) + "\r\n");
#endif

  }
  catch(...) {}

  if (NewState != WMPPlayState::wmppsTransitioning) // not transitioning? save state...
    m_PrevState = NewState;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::GetSongInfo(void)
{
  GetSongInfo(MediaInfo);
}

// This info is in utf-8 strings... the length fields are in bytes
void __fastcall TPlaylistForm::GetSongInfo(STRUCT_A &sms)
{
  sms.player = PlayerID;

  sms.duration = (int)Wmp->currentMedia->duration;

  AnsiString uPath = MainForm->WideToUtf8Ansi(Wmp->URL);
  strncpy(sms.path, uPath.c_str(), SONG_PATH_SIZE-1);
  sms.len_path = uPath.Length();
  if (sms.len_path > SONG_PATH_SIZE-1)
    sms.len_path = SONG_PATH_SIZE-1;


  AnsiString uTitle = MainForm->WideToUtf8Ansi(Wmp->currentMedia->name);
  strncpy(sms.name, uTitle.c_str(), SONG_NAME_SIZE-1);
  sms.len_name = uTitle.Length();
  if (sms.len_name > SONG_NAME_SIZE-1)
    sms.len_name = SONG_NAME_SIZE-1;

  // Note the "L" to make string-constants wide!!!!!!
  AnsiString uArtist = MainForm->WideToUtf8Ansi(Wmp->currentMedia->getItemInfo(L"WM/AlbumArtist"));
  if (uArtist == "" || uArtist.LowerCase() == "various artists" || uArtist.LowerCase() == "various")
    uArtist = MainForm->WideToUtf8Ansi(Wmp->currentMedia->getItemInfo(L"Author"));
  strncpy(sms.artist, uArtist.c_str(), SONG_NAME_SIZE-1);
  sms.len_artist = uArtist.Length();
  if (sms.len_artist > SONG_NAME_SIZE-1)
    sms.len_artist = SONG_NAME_SIZE-1;

  AnsiString uAlbum = MainForm->WideToUtf8Ansi(Wmp->currentMedia->getItemInfo(L"WM/AlbumTitle"));
  strncpy(sms.album, uAlbum.c_str(), SONG_NAME_SIZE-1);
  sms.len_album = uAlbum.Length();
  if (sms.len_album > SONG_NAME_SIZE-1)
    sms.len_album = SONG_NAME_SIZE-1;
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::SendToSwiftMix(void *sms, int size, int msg)
{
  try
  {
    bool RetVal = false;

    // Populate CopyDataStruct
    COPYDATASTRUCT cds;
    cds.dwData = msg; // Unique windows message we registered
    cds.cbData = size; // size of data
    cds.lpData = sms; // pointer to data

    // Find the target application window (if running)
    HWND hnd = FindWindow(YC_CLASS, YC_WINDOW);

    // Send the message to target
    if (hnd != 0 && msg != 0)
    {
      SendMessage(hnd, WM_COPYDATA, (WPARAM)Application->Handle, (LPARAM)&cds);
      RetVal = true;
    }

    return RetVal;
  }
  catch (...)
  {
    return false;
  }
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::SetTimer(int mode, int time)
{
  if (GeneralPurposeTimer->Enabled)
    return false;
  GeneralPurposeTimer->Enabled = false;
  m_TimerMode = mode;
  GeneralPurposeTimer->Interval = time;
  GeneralPurposeTimer->Enabled = true;
  return true;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::PositionChange(double oldPosition, double newPosition)
{
  // User dragged the position-bar...
  UpdatePlayerStatus();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::UpdatePlayerStatus(void)
{
  if (!Wmp)
    return;

  try
  {
    int CurrentPos = (int)Wmp->controls->currentPosition+1;

    if (PlayerID == PLAYER_A_ID)
      TimeDisplay(CurrentPos, 0);
    else
      TimeDisplay(CurrentPos, 3);

    if (MainForm->SendTiming) // Send Telemetry?
    {
      // NOTE: this struct is also implimented in my YahCoLoRiZe
      // project and in SwiftMiX C# version... (so it should not
      // be changed!). SwiftMiX C# changes color of the time
      // indicators as a song nears the end - a warning to a D.J.
      // in yellow, then it turns red.
      typedef struct
      {
        __int32 player;
        __int32 duration;
        __int32 current;
        __int32 color;
        __int32 redtime;
        __int32 yellowtime;
      } SWIFTMIX_STRUCT;

      // Send data to YahCoLoRiZe
      int size = sizeof(SWIFTMIX_STRUCT);

      // Populate SwiftMixStruct
      SWIFTMIX_STRUCT sms;
      sms.player = PlayerID;
      sms.duration = m_Duration;
      sms.current = CurrentPos;

//!!!!!!!!!!!!!! not yet implimented
      sms.color = 0;
      sms.redtime = MainForm->FadePoint;
      sms.yellowtime = 0;

      SendToSwiftMix(&sms, size, MainForm->RWM_SwiftMixTime);
    }
  }
  catch(...)
  {
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::TimeDisplay(int t, int item)
{
  int hours = t / (60  *60);
  int minutes = t / 60;
  int seconds = t % 60;

  MainForm->StatusBar1->Panels->Items[item]->Text =
      Format("%2.2d:%02.2d:%2.2d", ARRAYOFCONST((hours, minutes, seconds)));
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SetTitle(void)
{
  try
  {
    String S1, S2;

    S2 = "(nothing queued)";

    if (FEditMode){
      if (PlayerID == PLAYER_A_ID)
        S1 = "EditA ";
      else
        S1 = "EditB ";
    }
    else{
      if (PlayerID == PLAYER_A_ID)
        S1 = "PlayerA ";
      else
        S1 = "PlayerB ";
    }

    // only flash if playing and not-active
    // assume no-flash
    FlashTimer->Enabled = false;
    int SelectIdx = -1;

    if (CheckBox->Count)
    {
      bool bPlayOk = FPlayIdx >= 0 && FPlayIdx < CheckBox->Items->Count;
      bool bTargetOk = FTargetIdx >= 0 && FTargetIdx < CheckBox->Items->Count;

      if (Active) // Window is focused?
      {
        S1 += "(Q) ";

        if (bTargetOk)
        {
          S2 = pPH->GetURL(this, FTargetIdx);
          SelectIdx = FTargetIdx;
        }
      }
      else if (!OtherForm->Active) // if neither playlist is focused...
      {
        if (bPlayOk && IsPlayOrPause())
        {
          S1 += "(P) ";
          S2 = pPH->GetURL(this, FPlayIdx);
          SelectIdx = FPlayIdx;
          FlashTimer->Enabled = true;
        }
        else
        {
          S1 += "(Q) ";

          if (bTargetOk)
          {
            S2 = pPH->GetURL(this, FTargetIdx);
            SelectIdx = FTargetIdx;
          }
        }
      }
      else // other playlist-form is active
      {
        S1 += "(Q) ";

        if (bTargetOk)
        {
          S2 = pPH->GetURL(this, FTargetIdx);
          SelectIdx = FTargetIdx;
        }
      }
    }

    // Set listbox selection to match title
    if (CheckBox->ItemIndex != SelectIdx)
      CheckBox->ItemIndex = SelectIdx;

    // Buttons and icon
    int Misc = 3*GetSystemMetrics(SM_CXSMSIZE);

    #define TITLE_PIXELS_PER_CHAR 8
    // Available # chars for song title is the
    // total form width - icon widths - S1 width - "..."
    int W = (Width-Misc)/TITLE_PIXELS_PER_CHAR - S1.Length() - 3;

    if (S2.Length() > W)
    {
      S1 += L"...";
      int len = S2.Length();
      this->Caption = S1 + S2.SubString(len-W, len-(len-W)+1);
    }
    else
      this->Caption = S1 + S2;
  }
  catch(...)
  {
    ShowMessage("SetTitle() threw an exception");
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::StartPlayPreview(void)
{
  // press and hold control key to play-preview songs in the list
  if (FEditMode || !CheckBox->Focused() || !(GetKeyState(VK_CONTROL)&0x8000)){
    return;
  }

  // don't start play if either player is already playing normally...
  if (!FPlayPreview && (IsPlayOrPause() || OtherForm->IsPlayOrPause())){
    return;
  }

  try{
    int Index = CheckBox->ItemAtPos(ScreenToClient(Mouse->CursorPos), true);

    // check for index change of song under mouse pointer
    if (Index != FOldMouseItemIndex){
      if (Index >= 0 && Index < CheckBox->Count){
        String S = pPH->GetURL(this, Index);

//        if (S == Wmp->URL){
//          S = "";
//          if (MediaInfo.artist[0] != NULLCHAR)
//            S += "Artist: " + String(MediaInfo.artist) + String(LF);
//          if (MediaInfo.album[0] != NULLCHAR)
//            S += "Album: " + String(MediaInfo.album) + String(LF);
//          if (MediaInfo.name[0] != NULLCHAR)
//            S += "Song: " + String(MediaInfo.name);
//        }
//
//        if (!S.IsEmpty() && S != CheckBox->Hint)
//          CheckBox->Hint = S;

        // Handle "play preview" feature when Ctrl key is held down...
        if (!S.IsEmpty()){
          FPlayPreview = true;
          Wmp->URL = S;
          Wmp->controls->set_currentPosition(PLAY_PREVIEW_START_TIME);
          if (PlayerID == PLAYER_A_ID){
            MainForm->FaderTrackBar->Position = MainForm->FaderTrackBar->Min;
          }
          else{
            MainForm->FaderTrackBar->Position = MainForm->FaderTrackBar->Max;
          }
          StartPlayer();
//          SetTimer(TM_STOP_PLAY_PREVIEW, 10000);
        }
      }
      else{
        SetTimer(TM_STOP_PLAY_PREVIEW, 50);
      }

      FOldMouseItemIndex = Index;
    }
  }
  catch(...){};
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::StopPlayPreview(void)
{
  if (FPlayPreview){
    StopPlayer();
    FPlayPreview = false;
    Application->ProcessMessages();
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::StartPlayer(void)
{
  Wmp->settings->mute = true;
  Wmp->controls->play();
  Wmp->settings->mute = false;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::StopPlayer(void)
{
  Wmp->settings->mute = true;
  Wmp->controls->stop();
  Wmp->settings->mute = false;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::ClearAndStop(void)
{
  // Stop and clear list...

  FEditMode = true;

  FlashTimer->Enabled = false;
  GeneralPurposeTimer->Enabled = false;
  PositionTimer->Enabled = false;
  MainForm->AutoFadeTimer->Enabled = false;

  // Park the trackbar (seemed like a good idea - but on second thought
  // it's probably better to just freeze a fade-in-progress and let the user
  // handle the remaining fade... to avoid a sudden volume-change...)
//  if (PlayerA)
//    MainForm->TrackBar1->Position = 100;
//  else
//    MainForm->TrackBar1->Position = 0;

  if (Wmp)
  {
    StopPlayPreview();
    StopPlayer();
    Wmp->URL = "";
  }

  String S = (PlayerID == PLAYER_A_ID) ? "PlayerA " : "PlayerB ";
  this->Caption =  S + "(nothing queued)";

  ClearListItems();
  FEditMode = false;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::ClearListItems(void)
{
  // set cancel flag on every instance of TMyFileCopy and/or
  // TMyUrlCopy in MainForm->MyFileCopyList that has its ListID
  // property set to "this" playlist (PLAYER_ID_A or PLAYER_ID_B)
  MainForm->CancelFileAndUrlCopyInstances(this->PlayerID);

  pFC->DeleteAllCacheFiles(this);

  int numItems = CheckBox->Count;
  for (int ii = 0; ii < numItems; ii++)
  {
    TPlayerURL *p = (TPlayerURL*)CheckBox->Items->Objects[ii];
    if (p) delete p;
  }

  CheckBox->Clear();

  FPlayIdx = -1;
  FTargetIdx = -1;
  FTempIdx = -1;

//  Color = TColor(0xF5CFB8);

  m_bInhibitFlash = false;
  m_bDoubleClick = false;
  m_bCheckClick = false;
  m_Duration = 0;
  m_PrevState = 0;
  m_failSafeCounter = 0;
  m_TimerMode = TM_NULL;
  m_bSkipFilePrompt = false;
  m_bOpening = false;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::EditModeClick(TObject *Sender)
// PopupMenu1 F3
{
  StopPlayPreview();

  // Enter Edit Mode
  FEditMode = true;

  FlashTimer->Enabled = false;
  GeneralPurposeTimer->Enabled = false;
  m_bCheckClick = false;
  m_bDoubleClick = false;

  CheckBox->PopupMenu = PopupMenu2;

  int saveSelected = CheckBox->ItemIndex;
  CheckBox->MultiSelect = true;
  CheckBox->Selected[saveSelected] = true;

  SetTitle();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::ExitEditModeClick(TObject *Sender)
// PopupMenu2 F3
{
  // Exit Edit Mode
  CheckBox->PopupMenu = PopupMenu1;
  CheckBox->MultiSelect = false;

  FEditMode = false;

  SetTitle(); // Start flashing again, etc.
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::MenuScrollQueuedIntoViewClick(TObject *Sender)
{
  CheckBox->TopIndex = FTargetIdx;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::MenuScrollPlayingIntoViewClick(TObject *Sender)
{
  CheckBox->TopIndex = FPlayIdx;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::ClearListClick(TObject *Sender)
// PopupMenu1 F4
{
  // Clear Playlist And Stop
  ClearAndStop();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckAllItems(void)
{
  for (int ii = 0 ; ii < CheckBox->Count ; ii++)
    if (ii != FPlayIdx && ii != FTargetIdx)
      SetGrayedState(ii);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckAllClick(TObject *Sender)
// PopupMenu1 F5
{
  CheckAllItems();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::UncheckAllClick(TObject *Sender)
// PopupMenu1 F6
{
  if (IsPlayOrPause())
  {
    for (int ii = 0 ; ii < CheckBox->Count ; ii++)
      if (ii != FPlayIdx && ii != FTargetIdx)
        ClearCheckState(ii, false);
  }
  else
  {
    FPlayIdx = -1;
    FTargetIdx = -1;
    CheckBox->ItemIndex = -1;
    for (int ii = 0 ; ii < CheckBox->Count ; ii++)
      ClearCheckState(ii, false);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SubmenuDeleteEvenClick(TObject *Sender)
{
  // Return if no items or a song is playing
  if (Wmp == NULL || CheckBox->Items->Count == 0 || IsPlayOrPause())
  {
    ShowMessage(STR[3]);
    return;
  }

  try
  {
    for (int ii = 1; ii < CheckBox->Items->Count; ii++)
      DeleteListItem(ii);
    QueueFirst();
  }
  catch(...) {}
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SubmenuDeleteOddClick(TObject *Sender)
{
 // Return if no items or a song is playing
  if (Wmp == NULL || CheckBox->Items->Count == 0 || IsPlayOrPause())
  {
    ShowMessage(STR[3]);
    return;
  }

  try
  {
    for (int ii = 0; ii < CheckBox->Items->Count; ii++)
      DeleteListItem(ii);
    QueueFirst();
  }
  catch(...) {}
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SubmenuDeleteSelectedClick(TObject *Sender)
{
  if (CheckBox->Count)
  {
    DeleteListItem(CheckBox->ItemIndex);

    // nothing playing?
    if (FPlayIdx < 0)
      QueueFirst();

    SetTitle();
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::DeleteAllSelectedClick(TObject *Sender)
// PopupMenu2 F5
{
  // Delete Selected Items
  if (CheckBox->SelCount)
  {
    // Remove from this list (must remove backward)
    for (int ii = CheckBox->Count-1 ; ii >= 0  ; ii--)
      if (CheckBox->Selected[ii])
        DeleteListItem(ii);

    // Exit edit-mode
    if (FEditMode)
      ExitEditModeClick(NULL);
  }
  else
    ShowMessage(STR[0]);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SubmenuDeleteDupsClick(TObject *Sender)
{
  // Return if no items or a song is playing
  if (Wmp == NULL || CheckBox->Count == 0 || Wmp->playState == WMPPlayState::wmppsPlaying)
  {
    ShowMessage(STR[3]);
    return;
  }

  for (int ii = 0; ii < CheckBox->Count; ii++)
  {
    TPlayerURL *p = (TPlayerURL*)CheckBox->Items->Objects[ii];
    if (!p) continue;
    for (int jj = CheckBox->Count-1; jj > ii; jj--)
    {
      TPlayerURL *p2 = (TPlayerURL*)CheckBox->Items->Objects[jj];
      if (!p2) continue;
      if (p->path == p2->path)
        DeleteListItem(jj);
    }
  }

// This worked and was fast but won't handle TextColor as an TObject*
/*
  TStringList *sl = new TStringList();
  sl->Sorted = true;
  sl->Duplicates = dupIgnore;

  sl->AddStrings(CheckBox->Items);

  CheckBox->Items->Clear(); // NOTE: this won't work now - need ClearListItems()
  CheckBox->Items->AddStrings(sl);

  delete sl;
*/

  FPlayIdx = -1;
  FTargetIdx = -1;

  // Probably don't want to reenable played items
  // CheckAllItems();

  this->QueueFirst();
}
//---------------------------------------------------------------------------
// Ok, so what's this for? Good question. Because I wrote it and need to
// remember what it does myself :-)
// It's for the case where the songs from an album(s) were ripped and
// assigned a name followed by a number for the filename. The order of the songs
// in the playlist will not be as you expect to see them. You want see
// path.MySongs1, path.MySongs2, path.MySongs3 - in numerical order. But
// the built-in list sorting sorts from the beginning of a string, not by
// the numbers at the end of the string - so - if any songs are like I've
// described, this function fixes their order so that a complete album plays
// as expected and not out of sequence.
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SubmenuSortByTrailingNumbersClick(TObject *Sender)

{
  // Return if no items or a song is playing
  if (Wmp == NULL || CheckBox->Count == 0 || Wmp->playState == WMPPlayState::wmppsPlaying)
  {
    ShowMessage(STR[3]);
    return;
  }

  int ct = CheckBox->Count;
  if (ct < 2)
    return;
  TStringList *sl = NULL;
  TStringList *slTemp = NULL;
  try
  {
    sl = new TStringList(); // used to build a new song-list
    slTemp = new TStringList(); // holds interim songs with same base name

    // iterate through songs in list, grouping songs with the same
    // file path and extension except for trailing digits.
    for (int ii = 0; ii < ct; ii++)
    {
      TPlayerURL *p = (TPlayerURL*)CheckBox->Items->Objects[ii];
      if (!p) continue; // item already processed...
      CheckBox->Items->Objects[ii] = NULL; // mark it as processed
      String sLeadingPath1, sExt1; // returned by reference
      int iDigits = GetTrailingDigits(p->path, sLeadingPath1, sExt1);
      if (iDigits < 0)
      {
        sl->AddObject(CheckBox->Items->Strings[ii], (TObject*)p);
        continue;
      }
      // add song at ii
      if (p->cacheNumber > 0)
        pFC->DeleteCacheFile(this, p->cacheNumber);
      p->cacheNumber = 0;
      p->state = cbGrayed;
      p->temp = iDigits; // use this field to hold trailing number
      slTemp->AddObject(CheckBox->Items->Strings[ii], (TObject*)p);
      for (int jj = ii+1; jj < ct; jj++)
      {
        p = (TPlayerURL*)CheckBox->Items->Objects[jj];
        if (!p) continue; // item already processed...
        String sLeadingPath2, sExt2; // get these by reference below...
        iDigits = GetTrailingDigits(p->path, sLeadingPath2, sExt2);
        if (iDigits < 0 || sLeadingPath1 != sLeadingPath2 || sExt1 != sExt2)
          continue;
        // add song at jj
        if (p->cacheNumber > 0)
          pFC->DeleteCacheFile(this, p->cacheNumber);
        p->cacheNumber = 0;
        p->state = cbGrayed;
        p->temp = iDigits; // use this field to hold trailing number
        slTemp->AddObject(CheckBox->Items->Strings[jj], (TObject*)p);
        CheckBox->Items->Objects[jj] = NULL; // mark it as processed
      }

      // sort items and add to to sl
      if (slTemp->Count > 0)
      {
        while (slTemp->Count > 0)
        {
          int idx = IndexOfSmallestNumber(slTemp);
          if (idx >= 0)
          {
            p = (TPlayerURL*)slTemp->Objects[idx];
            if (p)
              sl->AddObject(slTemp->Strings[idx], (TObject*)p);
            slTemp->Delete(idx);
          }
          else
            break; // error... shouldn't happen!
        }
        slTemp->Clear();
      }
    }

    // now clear the main songlist and move new songs to it from sl
    CheckBox->Clear();
    for (int ii = 0; ii < sl->Count; ii++){
      CheckBox->Items->AddObject(sl->Strings[ii], (TObject*)sl->Objects[ii]);
      CheckBox->State[ii] = cbGrayed;
    }

    this->QueueFirst();
  }
  __finally
  {
    if (slTemp) delete slTemp;
    if (sl) delete sl;
  }
}
//---------------------------------------------------------------------------
int __fastcall TPlaylistForm::IndexOfSmallestNumber(TStringList *sl)
{
  int iSmallest = INT_MAX;
  int idx = -1;
  for (int ii=0; ii<sl->Count; ii++)
  {
    TPlayerURL *p = (TPlayerURL*)sl->Objects[ii];
    if (p && p->temp < iSmallest)
    {
      iSmallest = p->temp;
      idx = ii;
    }
  }
  return idx;
}
//---------------------------------------------------------------------------
// returns last file-extension in sExt
// returns int for trailing digits in the file name part before the first "."
// returns -1 if no digits
// sNamePart has the name part without the digits
// NOTE: if the file's name has multiple extensions, the in-between ones will
// NOT be returned, so save the original full file-path before calling!
int __fastcall TPlaylistForm::GetTrailingDigits(String s, String &sLeadingPath, String &sExt)
{
  String sDigits;
  int idx = s.LastDelimiter('.');
  if (idx > 1)
  {
    sExt = s.SubString(idx, s.Length()-idx+1); // return trailing file-extension by reference
    s = s.SubString(1, idx-1);
  }
  else
    sExt = "";
  int len = s.Length();
  int jj = len;
  for (; jj>0; jj--) // trim trailing spaces
    if (s[jj] != ' ')
      break;
  for (; jj>0; jj--)
  {
    Char c = s[jj];
    if (!isdigit(c))
      break;
    sDigits.Insert(c, 1);
  }
  if (jj > 0)
    sLeadingPath = s.SubString(1, jj);
  else
    sLeadingPath = "";
  return (sDigits.Length() > 0) ? sDigits.ToIntDef(-1) : -1;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SubmenuSortArtistClick(TObject *Sender)
{
  MySort(SORTBY_ARTIST);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SubmenuSortAlbumClick(TObject *Sender)
{
  MySort(SORTBY_ALBUM);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SubmenuSortTitleClick(TObject *Sender)
{
  MySort(SORTBY_TITLE);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::MySort(int iSortType)
{
  // Return if no items or a song is playing
  if (Wmp == NULL || CheckBox->Count == 0 || Wmp->playState == WMPPlayState::wmppsPlaying)
  {
    ShowMessage(STR[3]);
    return;
  }

  int ct = CheckBox->Count;
  if (ct < 2)
    return;
  TStringList *sl = NULL;
  TStringList *slTemp = NULL;
  TStringList *slNoTag = NULL;
  try
  {
    // slTemp will hold sorted list of artist names with Object field of
    // each item holding the original list-index into CheckBox
    if ((slTemp = new TStringList()) == NULL)
      return;
    if ((sl = new TStringList()) == NULL)
      return;
    if ((slNoTag = new TStringList()) == NULL)
      return;

    // iterate through songs in CheckBox, adding their MP3 tag artist-names
    // to slTemp, then set the Sort flag to sort slTemp by Artist.
    TGeneralAudioFile *f = NULL;
    String sSortStr;
    TPlayerURL *p;
    for (int ii = 0; ii < ct; ii++)
    {
      p = (TPlayerURL*)CheckBox->Items->Objects[ii];
      if (!p) continue;

      sSortStr = "";
      f = NULL;

      try{
        if ((f = new TGeneralAudioFile(p->path)) != NULL){
          if (iSortType == SORTBY_ARTIST)
            sSortStr = f->Artist;
          else if (iSortType == SORTBY_ALBUM)
            sSortStr = f->Album;
          else
            sSortStr = f->Title;
          slTemp->AddObject(sSortStr, (TObject*)ii);
        }
        else // no tag available... use file-path
          // NOTE: could write function to break file-path into artist, album, Etc.
          slNoTag->AddObject(p->path, (TObject*)ii);
      }
      __finally{
       if (f)
         delete f;
      }
    }

    slTemp->Sorted = true; // sort the list
    Application->ProcessMessages();

    // add entries with no MP3 tag-info to end of list...
    slTemp->AddStrings(slNoTag);

    // pluck entries from CheckBox and add them to sl
    ct = slTemp->Count;
    for (int ii = 0; ii < ct; ii++)
    {
      int idx = (int)slTemp->Objects[ii];
      p = (TPlayerURL*)CheckBox->Items->Objects[idx];
      if (!p) continue;
      if (p->cacheNumber > 0)
        pFC->DeleteCacheFile(this, p->cacheNumber);
      p->cacheNumber = 0;
      p->state = cbGrayed;
      sl->AddObject(CheckBox->Items->Strings[idx], (TObject*)p);
      Application->ProcessMessages();
    }

    // now clear the main songlist and move new songs to it from sl
    CheckBox->Clear();
    for (int ii = 0; ii < sl->Count; ii++){
      CheckBox->Items->AddObject(sl->Strings[ii], (TObject*)sl->Objects[ii]);
      CheckBox->State[ii] = cbGrayed;
    }

    this->QueueFirst();
  }
  __finally
  {
    if (sl) delete sl;
    if (slTemp) delete slTemp;
    if (slNoTag) delete slNoTag;
  }
  CheckBox->Update();
}
//---------------------------------------------------------------------------
// randomize the songs that have not already been played
// (NOTE: quirk I've noticed - songs with a funny character seem to end up
// in the same vacinity and at the top of the list??? I don't think Move
// is working for those song-titles.)
void __fastcall TPlaylistForm::RandomizeList1Click(TObject *Sender)
{
  // Return if no items or a song is playing
  if (CheckBox->Count == 0 || Wmp->playState == WMPPlayState::wmppsPlaying)
  {
    ShowMessage(STR[3]);
    return;
  }

  TList *la = NULL;
  TList *lb = NULL;
  TList *lc = NULL;
  CheckBox->Enabled = false;

  try
  {
    // first create a list of the playlist indexes of all unplayed songs
    la = new TList();
    lb = new TList();
    lc = new TList();

    if (!la || !lb || !lc)
    return;

    // fresh random # generator seed
    ::Randomize();

    int Count = CheckBox->Count;

    // add unplayed songs to list A
    for(int ii = 0; ii < Count; ii++)
      if (IsStateGrayed(CheckBox, ii))
        la->Add((void*)ii);

    // save unplayed list in c
    lc->Assign(la);

    // select random list A indicies and add them to list B
    // and removing from list A
    Count = la->Count;
    for(int ii = 0; ii < Count; ii++)
    {
      int idx = ::Random(la->Count);
      lb->Add(la->Items[idx]);
      la->Delete(idx);
    }

    if (lc->Count != lb->Count)
    {
      ShowMessage("Mismatch in counters: TPlaylistForm::RandomizeList1Click(): ");
      return;
    }

    Count = lc->Count;
    pProgress->Init(Count);

    // now randomize only the unplayed songs
    for(int ii = 0; ii < Count; ii++)
    {
      int dstIdx = (int)lb->Items[ii];
      int srcIdx = (int)lc->Items[ii];
      CheckBox->Items->Move(srcIdx, dstIdx);

      if (pProgress->Move(ii))
        return;
    }

    pProgress->UnInit();

    FPlayIdx = -1;
    FTargetIdx = -1;

    // Probably don't want to reenable played items
    //CheckAllItems();

    this->QueueFirst();
  }
  __finally
  {
    if (la) delete la;
    if (lb) delete lb;
    if (lc) delete lc;
    CheckBox->Enabled = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SelectAllItemsClick(TObject *Sender)
{
  int Count = CheckBox->Count;
  pProgress->Init(Count);
  CheckBox->Enabled = false;

  try
  {
    for (int ii = 0; ii < Count; ii++)
    {
      CheckBox->Selected[ii] = true;
      if (pProgress->Move(ii))
        return;
    }
  }
  __finally
  {
    pProgress->UnInit();
    CheckBox->Enabled = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CopySelectedClick(TObject *Sender)
// Copies data as UTF-16 format
{
  if (CheckBox->SelCount == 0)
  {
    ShowMessage(String(STR[0]));
    return;
  }

  if (!OpenClipboard(MainForm->Handle))
    return;

  HGLOBAL hMem = NULL;;
  TStringList *sl = NULL;

  try
  {
    EmptyClipboard();

    sl = new TStringList();

    if (sl != NULL)
    {
      for (int ii = 0 ; ii < CheckBox->Count ; ii++)
        if (CheckBox->Selected[ii])
          sl->Add(pPH->GetURL(this, ii));

      String ws = sl->Text;

      int len = ws.Length()  *sizeof(WideChar);

      int termlen = 4; // Allow space for 4 bytes of 0's to terminate

      hMem = GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE, len+termlen);

      if (hMem != NULL)
      {
        // Move string into global-memory
        char *lp = (char *)GlobalLock(hMem);

        CopyMemory(lp, ws.w_str(), len);

        char term[] = "\0\0\0\0"; // Terminator

        CopyMemory(lp+len, &term, termlen);

    // Put the memory-handle on the clipboard
        // DO NOT FREE this memory!
        SetClipboardData(CF_UNICODETEXT, hMem);
      }
    }
  }
  __finally
  {
    if (hMem != NULL) GlobalUnlock(hMem);
    try { if (sl != NULL) delete sl; } catch(...) {}
  }

  CloseClipboard();
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::GetIsOpenDlg(void)
{
  return pOFMSDlg != NULL ? true : false;
}
//---------------------------------------------------------------------------
TOFMSDlgForm *__fastcall TPlaylistForm::CreateFileDialog(void)
{
  DestroyFileDialog(); // destroy the old one...
  Application->CreateForm(__classid(TOFMSDlgForm), &pOFMSDlg);
  return pOFMSDlg;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::DestroyFileDialog(void)
{
  if (MainForm->ReleaseFormNoClose((TForm*)pOFMSDlg))
    pOFMSDlg = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::DestroyImportDialog(void)
{
  if (MainForm->ReleaseForm((TForm*)pImportDlg))
    pImportDlg = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::DestroyProgressForm(void)
{
  if (MainForm->ReleaseForm((TForm*)pProgress))
    pProgress = NULL;
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::GetIsImportDlg(void)
{
  return pImportDlg != NULL ? true : false;
}
//---------------------------------------------------------------------------
TImportForm *__fastcall TPlaylistForm::CreateImportDialog(void)
{
  DestroyImportDialog(); // destroy the old one...
  Application->CreateForm(__classid(TImportForm), &pImportDlg);
  return pImportDlg;
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::GetIsExportDlg(void)
{
  return pExportDlg != NULL ? true : false;
}
//---------------------------------------------------------------------------
TExportForm *__fastcall TPlaylistForm::CreateExportDialog(void)
{
  DestroyExportDialog(); // destroy the old one...
  Application->CreateForm(__classid(TExportForm), &pExportDlg);
  return pExportDlg;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::DestroyExportDialog(void)
{
  if (MainForm->ReleaseForm((TForm*)pExportDlg))
    pExportDlg = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CopyLinkToClipboardClick(TObject *Sender)
{
  // call same function used to copy selected item links in EditMode
  CopySelectedClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::SearchandUncheckClick(TObject *Sender)
{
  // get search-text
  TSearchUncheckForm *f = NULL;
  Application->CreateForm(__classid(TSearchUncheckForm), &f);

  if (f)
  {
    try
    {
      f->SearchText = "";

      f->ShowModal();

      if (!f->SearchText.IsEmpty())
      {
        for (int ii = 0; ii < CheckBox->Count; ii++)
        {
          String lcSearchText = f->SearchText.LowerCase();
          if (IsStateGrayed(CheckBox, ii))
          {
            TPlayerURL *p = (TPlayerURL*)CheckBox->Items->Objects[ii];
            if (!p) continue;
            String s = p->path;
            s = s.LowerCase();
            if (s.Pos(lcSearchText) > 0)
            {
              ClearCheckState(ii, false);

              if (FTargetIdx == ii)
                GetNext(ii);
            }
          }
        }
      }
    }
    __finally
    {
      MainForm->ReleaseForm((TForm*)f);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CopyTagsToClipboardClick(TObject *Sender)
{
  if (FTempIdx < 0) return;

  try
  {
    TPlayerURL *p = (TPlayerURL*)CheckBox->Items->Objects[FTempIdx];
    if (!p || !FileExists(p->path)) return;
    TFormTags *f = new TFormTags(this); // form will delete itself on close
    if (!f) return;
    f->ShowTags(p);
  }
  catch(...) {}
//  String s = GetMediaTags();
//  if (!s.IsEmpty())
//  {
//    Clipboard()->AsText = s;
//    s += "Copied song-tags:\n";
//    ShowMessage(s);
//  }
}
//---------------------------------------------------------------------------
String __fastcall TPlaylistForm::GetMediaTags(void)
{
  String sTags;

  try
  {
    sTags = Wmp->currentMedia->getItemInfo(L"WM/AlbumArtist");

    if (sTags == "" || sTags.LowerCase() == "various artists" ||
                                          sTags.LowerCase() == "various")
      sTags = Wmp->currentMedia->getItemInfo(L"Author");

    if (!sTags.IsEmpty())
      sTags += String(", ");

    sTags += Wmp->currentMedia->getItemInfo(L"WM/AlbumTitle");

    if (!sTags.IsEmpty())
      sTags += String(", ");

    sTags += Wmp->currentMedia->name;
  }
  catch(...) {}

  return sTags;
}
//---------------------------------------------------------------------------
//void __fastcall TPlaylistForm::WMMove(TWMMove &Msg)
//{
//  try
//  {
//    if (MainForm->GDock != NULL)
//        MainForm->GDock->WindowMoved(this->Handle);
//    // call the base class handler
//    TForm::Dispatch(&Msg);
//  }
//  catch(...) { }
//}
// Property getter
//---------------------------------------------------------------------------
int __fastcall TPlaylistForm::GetCount(void)
{
  return CheckBox->Count;
}
//---------------------------------------------------------------------------
bool __fastcall TPlaylistForm::IsPlayOrPause(void)
{
  return Wmp->playState == WMPPlayState::wmppsPlaying ||
                Wmp->playState == WMPPlayState::wmppsPaused;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if (FPlayPreview && !Shift.Contains(ssCtrl)){
    StopPlayPreview();
  }

  if (FKeySpaceDisable && Key == VK_SPACE){
    FKeySpaceDisable = false;
    Key = 0;
  }
}
//---------------------------------------------------------------------------
// need this to stop selected item check-state from toggelling when you press
// (and hold) the spacebar to check/uncheck the FPlayPreview item under the mouse!
void __fastcall TPlaylistForm::CheckBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if (Key == VK_SPACE)
    Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if (FEditMode){
    if (Key == VK_ESCAPE){
      ExitEditModeClick(NULL);
      Key = 0;
      return;
    }
  }
  else{
    if (FPlayPreview){
      if (!FKeySpaceDisable && Key == VK_SPACE){
        if (FOldMouseItemIndex >= 0 && FOldMouseItemIndex < CheckBox->Count){
          if (IsStateGrayed(CheckBox, FOldMouseItemIndex)){
            ClearCheckState(FOldMouseItemIndex, false);
          }
          else{
            SetGrayedState(FOldMouseItemIndex);
          }
        }
        Key = 0;
        FKeySpaceDisable = true; // key-repeat if spacebar held down is a problem!
        return;
      }
    }
    else{
      // ssShift, ssAlt, ssCtrl, ssLeft, ssRight, ssMiddle, ssDouble,
      // ssTouch, ssPen, ssCommand, ssHorizontal
      if (Shift.Contains(ssCtrl)){
        FOldMouseItemIndex = -1;
        StartPlayPreview();
        return;
      }
    }
  }

  if (Key == VK_UP || Key == VK_DOWN || Key == VK_LEFT || Key == VK_RIGHT)
  {
    // enter edit mode when keys pressed to scroll, then time out
    if (!FEditMode)
      EditModeClick(NULL);
    SetTimer(TM_SCROLL_KEY_PRESSED, TIME_2000);
    return;
  }

  if (Key == VK_DELETE)
  {
    Key = 0;

    if (FEditMode)
      DeleteAllSelectedClick(NULL);
    else if (CheckBox->Count)
    {
      if (!Shift.Contains(ssShift))
        DeleteListItem(CheckBox->ItemIndex);
      else // delete all unchecked songs if the shift key is held down
      {
        for (int ii = CheckBox->Count-1 ; ii >= 0  ; ii--)
        {
          if (IsStateUnchecked(CheckBox, ii))
          {
            if (FTargetIdx >= ii)
              FTargetIdx--;
            if (FPlayIdx >= ii)
              FPlayIdx--;

            DeleteListItem(ii);
          }
        }
      }

    // nothing playing?
      if (FPlayIdx < 0)
        QueueFirst();

      SetTitle();
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::CheckBoxDrawItem(TWinControl *Control,
                            int Index, TRect &Rect, TOwnerDrawState State)
{
  TCheckListBox* lb = (TCheckListBox*)Control;
  if (!lb) return;

  TCanvas *pc = lb->Canvas;

  if (Index < 0 || Index >= lb->Items->Count ||
     lb->Items->Strings[Index].IsEmpty() || !pc->TryLock())
    return;

  TPlayerURL *p = (TPlayerURL*)lb->Items->Objects[Index];
  if (!p) return;

  try
  {
    if (!lb->UseRightToLeftAlignment())
      Rect.Left += 2;
    else
      Rect.Right -= 2;

    pc->Font->Color = p->color;
    if (State.Contains(odSelected))
      pc->Brush->Color = clHighlight;
    pc->FillRect(Rect);
    pc->TextOut(Rect.Left, Rect.Top, lb->Items->Strings[Index]);
    if (State.Contains(odFocused))
    {
      pc->Brush->Color = lb->Color;
      pc->DrawFocusRect(Rect);
    }
  }
  __finally
  {
    pc->Unlock();
  }
}
//---------------------------------------------------------------------------
//void __fastcall TPlaylistForm::CheckBoxMeasureItem(TWinControl *Control,
//                                                 int Index, int &Height)
//{
//  if (Index < 0 || Index >= FCheckBox->Items->Count ||
//                         FCheckBox->Items->Strings[Index].IsEmpty())
//    return;
//
//  // here we need the height of the tallest char
//  Height = FCheckBox->Canvas->TextHeight(FCheckBox->Items->Strings[Index]);
//}
//---------------------------------------------------------------------------
//HPEN __fastcall TPlaylistForm::CreateFocusPen()
//{
//  LONG width(1);
//  SystemParametersInfo(SPI_GETFOCUSBORDERHEIGHT, 0, &width, 0);
//  LOGBRUSH lb = { };     // initialize to zero
//  lb.lbColor = 0xffffff; // white
//  lb.lbStyle = BS_SOLID;
//  return ExtCreatePen(PS_GEOMETRIC | PS_DOT, width, &lb, 0, 0);
//}
//---------------------------------------------------------------------------
//void __fastcall TPlaylistForm::DrawFocusRect(TCanvas* c, HPEN hpen, TRect &r)
//{
//  HDC hdc = c->Handle;
//  HPEN old_pen = SelectObject(hdc, hpen);
//  int old_rop = SetROP2(hdc, R2_XORPEN);
//  c->FrameRect(r);
//  SelectObject(hdc, old_pen);
//  SetROP2(hdc, old_rop);
//}
//---------------------------------------------------------------------------
/*
void __fastcall TPlaylistForm::SettingChanged(TMessage &msg)
{
  UpdateWorkArea();
}
//---------------------------------------------------------------------------
void __fastcall TPlaylistForm::UpdateWorkArea()
{
  SystemParametersInfo(SPI_GETWORKAREA, 0, &work_area, 0);
}
// ------------------------------------------------ ---------------------------
void __fastcall TPlaylistForm::WMWindowPosChanging(TWMWindowPosChanging &msg)
{
  RECT sr;
  snapped = false; // Set does not adsorb
  // Test window.
  snapwin = MainForm->Handle;

  // the definition of Form2 adsorbed to the on Form1, here such as be changed to other software the window
  // handle of can also be adsorbed to the on a separate software window where

  if (snapwin && IsWindowVisible(snapwin))
  // The current position of the segment window Form1, Form2 mobile Form2 Form1 of the function to determine
  // whether in the vertical and horizontal directions, the distance is less than 50, such as less than 50 automatically
  // changes the position of the Form2, automatic adsorption onto Form1
  {
    if (GetWindowRect(snapwin, &sr)) // Form1 position
    {
      if ((msg.WindowPos->x <= (sr.right + thresh)) && (msg.WindowPos->x >= (sr.right - thresh)))
      {
        if ((msg.WindowPos->y > sr.top) && (msg.WindowPos->y < sr.bottom))
        {
          snapped = true;
          msg.WindowPos->x = sr.right;
        }
      }
      else if ((msg.WindowPos->x + msg.WindowPos->cx) >= (sr.left - thresh) && (msg.WindowPos->x + msg.WindowPos->cx) <= (sr.left + thresh))
      {
        if ((msg.WindowPos->y > sr.top) && (msg.WindowPos->y < sr.bottom))
        {
          snapped = true;
          msg.WindowPos->x = sr.left-msg.WindowPos->cx;
        }
      }

      if ((msg.WindowPos->y <= (sr.bottom + thresh)) && (msg.WindowPos->y >= (sr.bottom - thresh)))
      {
        if ((msg.WindowPos->x > sr.left) && (msg.WindowPos->x < sr.right))
        {
          snapped = true;
          msg.WindowPos->y = sr.bottom;
        }
      }
      else if ((msg.WindowPos->y + msg.WindowPos->cy) <= (sr.top + thresh) && (msg.WindowPos->y + msg.WindowPos->cy) >= (sr.top - thresh))
      {
        if ((msg.WindowPos->x > sr.left) && (msg.WindowPos->x < sr.right))
    {
          snapped = true;
          msg.WindowPos->y = sr.top-msg.WindowPos->cy;
        }
      }
    }
  }

  // Test screen
  sr = work_area;
  if (abs(msg.WindowPos->x) <= (sr.left + thresh))
  {
    snapped = true;
    msg.WindowPos->x = sr.left;
  }
  else if ((msg.WindowPos->x + msg.WindowPos->cx) >= (sr.right-thresh) && (msg.WindowPos->x + msg.WindowPos->cx) <= (sr.right + thresh))
  {
    snapped = true;
    msg.WindowPos->x = sr.right-msg.WindowPos->cx;
  }

  if (abs (msg.WindowPos->y) <= (sr.top + thresh))
  {
    snapped = true;
    msg.WindowPos->y = sr.top;
  }
  else if ((msg.WindowPos->y + msg.WindowPos->cy) >= (sr.bottom - thresh) && (msg.WindowPos->y + msg.WindowPos->cy) <= (sr.bottom + thresh))
  {
    snapped = true;
    msg.WindowPos->y = sr.bottom-msg.WindowPos->cy;
  }
}
*/
//---------------------------------------------------------------------------
// FYI:
//https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
//https://docs.microsoft.com/en-us/previous-versions/windows/desktop/api/wmp/nf-wmp-iwmpcontrols-playitem
//IWMPControls::
//fastForward - The fastForward method starts fast play of the media item in the forward direction.
//fastReverse - The fastReverse method starts fast play of the media item in the reverse direction.
//get_currentItem - The get_currentItem method retrieves the current media item in a playlist.
//get_currentMarker - The get_currentMarker method retrieves the current marker number.
//get_currentPosition - The get_currentPosition method retrieves the current position in the media item in seconds from the beginning.
//  HRESULT get_currentPosition(double &dCurrentPosition); // returns S_OK, pass address of double to receive current position in seconds
//get_currentPositionString - The get_currentPositionString method retrieves the current position in the media item as a BSTR formatted as HH:MM:SS (hours, minutes, and seconds).
//get_isAvailable - The get_isAvailable method indicates whether a specified type of information is available or a specified action can be performed.
//next - The next method sets the next item in the playlist as the current item.
//pause - The pause method pauses playback of the media item.
//play - The play method causes the current media item to start playing, or resumes play of a paused item.
//playItem - The playItem method plays the specified media item.
//previous - The previous method sets the previous item in the playlist as the current item.
//set_currentItem - The put_currentItem method specifies the current media item.
//set_currentMarker - The put_currentMarker method specifies the current marker number.
//set_currentPosition - The put_currentPosition method specifies the current position in the media item in seconds from the beginning.
//  HRESULT put_currentPosition(double dCurrentPosition); // returns S_OK
//stop
//---------------------------------------------------------------------------


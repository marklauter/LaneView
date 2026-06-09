//==========================================================================
// HISTORY:
// Author      Date      Description
// Mark Lauter   15 DEC 96   Contains all functions
//                     relating to TRIGGERS
//
//==========================================================================

#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>
#include <conio.h>
#include <string.h>

#include "cebus.h"
#include "2box.h"
#include "inside.h"
#include "vga.h"
#include "scrnobjt.h"
#include "textbox.h"
#include "pshbtn.h"
#include "lwindow.h"
#include "frame_3d.h"
#include "scrtypnl.h"
#include "ismenu.h"
#include "dvc_evnt.h"
#include "msgbox.h"
#include "checkbox.h"
#include "listbox.h"
#include "dropdown.h"
#include "moodmenu.h"
#include "inio.h"
#include "device.h"
#include "trigger.h"
#include "scrolbar.h"
#include "inutil.h"
#include "spinner.h"

extern char ADVANCED;

TRIGGER_T triggers[MAX_TRIGGERS];
USHORT triggers_used;

static TRIGGER_T this_trigger;
static TRIGGER_T* this_trigger_ptr;
static UCHAR trig_opt;

static UCHAR* actions_t[8] =
    {
        "Start Mood",
        "End Mood",
        "End All Moods",
        "Day Type Today",
        "Chg Master Schd",
        "Chg HVAC Schd",

        "Chg Season",
        "Chg Occupied"};

extern UCHAR* action_types[2];
MOOD_T* mood_sublist[MAX_MOODS];
//extern UCHAR sec_mood_names[MAX_MOODS][21];

static void ReturnClick(void);
static void DurationChange(void);
static void ShowAction(void);
static void DisplayTrigger(TRIGGER_T* trigger, int top, int left, UCHAR color);
static void TrigAddBtnClick(void);
static void TrigRmvBtnClick(void);
static void TrigSaveBtnClick(void);
static void TrigCancelBtnClick(void);
static void TriggerCheckBox1Click(void);
static void TriggerCheckBox2Click(void);
static void TrigEdtBtnClick(void);
static void TriggerNumberChange(void);
static void TriggerLetterChange(void);
static void TriggerActionChange(void);
static void TriggerMoodChange(void);
static void TriggerProtChange(void);
static void TriggerListBoxChange(void);

static SPINNER_T hourspin =
    {
        340, 141, 1, 1,   // position
        FALSE,            //focus
        TIME_ENTRY,       //type
        TRUE,             // visible
        SpinnerMouseDown, //OnMouseDown()
        SpinnerMouseUp,   //OnMouseUp()
        SpinnerClick,     // OnClick()
        SpinnerKeyPress,  // OnKeyPress()
        DisplaySpinner,   // DisplayObject()
        0,                //value
        99,               //max
        0,                //min
        1,                //increment
        DurationChange};

static SPINNER_T minspin =
    {
        340, 216, 1, 1,   // position
        FALSE,            //focus
        TIME_ENTRY,       //type
        TRUE,             // visible
        SpinnerMouseDown, //OnMouseDown()
        SpinnerMouseUp,   //OnMouseUp()
        SpinnerClick,     // OnClick()
        SpinnerKeyPress,  // OnKeyPress()
        DisplaySpinner,   // DisplayObject()
        0,                //value
        59,               //max
        0,                //min
        1,                //increment
        DurationChange};

PUSH_BUTTON_T triggerbtn[2] = { // BEGIN SCREEN OBJECT DEFINITION
    419, 445, 89, 33,           // position
    FALSE,                      // focus
    PUSH_BUTTON,                // type
    TRUE,                       // visible
    BtnMouseDown,               // OnMouseDown()
    BtnMouseUp,                 // OnMouseUp()
    BtnClick,                   // OnClick()
    BtnKeyPress,                // OnKeyPress()
    PutButton,                  // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,            //default button
    "Apply",          //text
    push,             //type
    up,               //state
    NULL,             //icon
    TrigSaveBtnClick, //function
    125,

    // BEGIN SCREEN OBJECT DEFINITION
    419, 545, 89, 33, // position
    FALSE,            // focus
    PUSH_BUTTON,      // type
    TRUE,             // visible
    BtnMouseDown,     // OnMouseDown()
    BtnMouseUp,       // OnMouseUp()
    BtnClick,         // OnClick()
    BtnKeyPress,      // OnKeyPress()
    PutButton,        // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,              //default button
    "Cancel",           //text
    push,               //type
    up,                 //state
    NULL,               //icon
    TrigCancelBtnClick, //function
    126};

CHECK_BOX_T trigger_checkbox[2] = {175, 169, 0, 0,      // position
                                   FALSE,               // got_focus
                                   CHECK_BOX,           // type
                                   TRUE,                // visible
                                   CheckBoxOnMouseDown, // OnMouseDown()
                                   CheckBoxOnMouseUp,   // OnMouseUp()
                                   CheckBoxOnClick,     // OnClick()
                                   CheckBoxOnKeyPress,  // OnKeyPress()
                                   DisplayCheckBox,     // DisplayObject()
                                   "On",
                                   TRUE,
                                   TriggerCheckBox1Click,

                                   175, 216, 0, 0,      // position
                                   FALSE,               // got_focus
                                   CHECK_BOX,           // type
                                   TRUE,                // visible
                                   CheckBoxOnMouseDown, // OnMouseDown()
                                   CheckBoxOnMouseUp,   // OnMouseUp()
                                   CheckBoxOnClick,     // OnClick()
                                   CheckBoxOnKeyPress,  // OnKeyPress()
                                   DisplayCheckBox,     // DisplayObject()
                                   "Off",
                                   FALSE,
                                   TriggerCheckBox2Click};

UCHAR* mode_sublistMAST[2] =
    {
        "Normal Schedule",
        "Vacation Schedule"};

UCHAR* mode_sublistHVAC[2] =
    {
        "Cooling Schedule",
        "Heating Schedule"};

UCHAR* mode_sublistSEAS[4] =
    {
        "Spring Schedule",
        "Summer Schedule",
        "Autumn Schedule",
        "Winter Schedule"};

UCHAR* mode_sublistOCCU[2] =
    {
        "At Home Schedule",
        "Temp Away Schedule"};

void DisplayMode(UCHAR* mode, int top, int left, UCHAR color);
void DisplayMood(MOOD_T* mood, int top, int left, UCHAR color);
static void list_boxOnChange(void);
static LISTBOX_T listbox =
    {
        // BEGIN SCREEN OBJECT DEFINITION
        110, 320, 170, 270, // position
        FALSE,              // focus
        LIST_BOX,           // type
        TRUE,               // visible
        ListBoxMouseDown,   // OnMouseDown()
        ListBoxMouseUp,     // OnMouseUp()
        ListBoxClick,       // OnClick()
        ListBoxKeyPress,    // OnKeyPress()
        PutListBox,         // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        0,               // item count
        0,               // item selected
        0,               // top_item
        0,               // display_count
        mood_sublist,    // sub_list
        DisplayMood,     // display function pointer
        list_boxOnChange // OnChange()
};
static void sbOnClick1(int direction, int how_far, int thumb_pos);
static SCROLL_BAR_T sb1 =
    {
        // BEGIN SCREEN OBJECT DEFINITION
        110, 491, 17, 270,    // position
        FALSE,                // focus
        SCROLL_BARV,          // type
        TRUE,                 // visible
        ScrollBarOnMouseDown, // OnMouseDown()
        ScrollBarOnMouseUp,   // OnMouseUp()
        ScrollBarOnClick,     // OnClick()
        NULL,                 // OnKeyPress()
        DisplayScrollBar,     // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        0,           // min
        15,          // max
        1,           // small change
        5,           // large change
        0,           // thumb possition
        SB_VERTICAL, // type
        sbOnClick1   // on scroll function
};
static void sbOnClick1(int direction, int how_far, int thumb_pos) {
  switch (direction) {
  case SCROLL_UP:
    switch (how_far) {
    case SCROLL_LARGE:
      PageUp(&listbox);
      break;
    case SCROLL_SMALL:
      UpArrow(&listbox);
      break;
    }
    break;
  case SCROLL_DOWN:
    switch (how_far) {
    case SCROLL_LARGE:
      PageDown(&listbox);
      break;
    case SCROLL_SMALL:
      DownArrow(&listbox);
      break;
    }
    break;
  }
  ListBoxScroll(&listbox);
  //PutListBox(&listbox);
  list_boxOnChange();
}
static void list_boxOnChange(void) {
  sb1.max = listbox.item_count - 1;
  sb1.thumb_pos = listbox.item_selected;
  sb1.large_chg = listbox.so.position.height / 14;
  if (sb1.large_chg > listbox.item_count) {
    sb1.large_chg = listbox.item_count;
  }
  DisplayScrollBar(&sb1);
}
void DisplayMood(MOOD_T* mood, int top, int left, UCHAR color) {
  UCHAR desc[21];

  sprintf(desc, "%20.20s", mood->name);
  ptext(desc, top, left, color);
}

DROPDOWN_BOX_T trigger_dropdown[4] = {
    //x10 letter
    128, 169, 40, 17,
    FALSE,               // got_focus
    DROP_DOWN_BOX,       // type
    TRUE,                // visible
    DropDownMouseDown,   // OnMouseDown()
    DropDownMouseUp,     // OnMouseUp()
    DropDownClick,       // OnClick()
    DropDownKeyPress,    // OnKeyPress()
    DisplayDropDown,     // DisplayObject()
    x10_ltr,             // strings
    0,                   // item selected
    FALSE,               // dropped
    16,                  // item count
    TriggerLetterChange, // OnChange

    //x10 number
    128, 216, 40, 17,
    FALSE,               // got_focus
    DROP_DOWN_BOX,       // type
    TRUE,                // visible
    DropDownMouseDown,   // OnMouseDown()
    DropDownMouseUp,     // OnMouseUp()
    DropDownClick,       // OnClick()
    DropDownKeyPress,    // OnKeyPress()
    DisplayDropDown,     // DisplayObject()
    x10_nmr,             // strings
    0,                   // item selected
    FALSE,               // dropped
    16,                  // item count
    TriggerNumberChange, // OnChange

    //action
    260, 141, 150, 17,
    FALSE,               // got_focus
    DROP_DOWN_BOX,       // type
    TRUE,                // visible
    DropDownMouseDown,   // OnMouseDown()
    DropDownMouseUp,     // OnMouseUp()
    DropDownClick,       // OnClick()
    DropDownKeyPress,    // OnKeyPress()
    DisplayDropDown,     // DisplayObject()
    actions_t,           // strings
    0,                   // item selected
    FALSE,               // dropped
    8,                   // item count
    TriggerActionChange, // OnChange

    //type (protected, unprotected)
    300, 141, 150, 17,
    FALSE,             // got_focus
    DROP_DOWN_BOX,     // type
    TRUE,              // visible
    DropDownMouseDown, // OnMouseDown()
    DropDownMouseUp,   // OnMouseUp()
    DropDownClick,     // OnClick()
    DropDownKeyPress,  // OnKeyPress()
    DisplayDropDown,   // DisplayObject()
    action_types,      // strings
    0,                 // item selected
    FALSE,             // dropped
    2,                 // item count
    TriggerProtChange  // OnChange
};

SCREEN_OBJECT_T* trigger_so[12];
static void TriggerWindowDraw(void);
UCHAR trigger_title[] = "Remote Control Trigger Definition";
LWINDOW_T trigger_window[1] = {
    35, 0, 640, 445, //window position
    trigger_so,      //screen object
    TriggerWindowDraw,
    12,               //item count
    0,                //cursor position 0-79 left to right
    0,                //element that has current focus
    fcsBtn,           //focus type
    trigger_title,    // title
    DefaultOnKeyPress //OnKeyPress();
};

//===========================================================================================
// NAME:   EditTrigger()
//
// HISTORY:
// Author         Date         Description
// M. Lauter      15 DEC 96      Initial Version
//
//
//
// INPUTS:
// Name            Type         Description
// trigger_ptr      TRIGGER_T *      pointer to trigger to be added or changed
// opt            UCHAR         'A' = add new trigger
//                           'C' = change existing trigger
//
// OUTPUTS:
// Name            Type         Description
// void
//
// DESCRIPTION:
//
// EditTrigger() allows a user to edit trigger
// devices through a visual interface
//
//===========================================================================================
int trigger_index = 0;
void EditTrigger(TRIGGER_T* trig_ptr, UCHAR opt) {
  int i;
  int hr;
  int min;

  trig_opt = opt;
  this_trigger = *trig_ptr;
  this_trigger_ptr = trig_ptr;

  trigger_so[0] = &trigger_dropdown[0].so;
  trigger_so[1] = &trigger_dropdown[1].so;
  trigger_so[2] = &trigger_checkbox[0].so;
  trigger_so[3] = &trigger_checkbox[1].so;
  trigger_so[4] = &trigger_dropdown[2].so;
  if (ADVANCED == TRUE) {
    trigger_dropdown[2].count = 8;
  } else {
    trigger_dropdown[2].count = 6;
  }
  trigger_so[5] = &trigger_dropdown[3].so;
  trigger_so[6] = &hourspin.so;
  trigger_so[7] = &minspin.so;
  trigger_so[8] = &listbox.so;
  trigger_so[9] = &sb1.so;
  trigger_so[10] = &triggerbtn[0].so;
  trigger_so[11] = &triggerbtn[1].so;

  hourspin.value = this_trigger.minutes / 60;
  minspin.value = this_trigger.minutes % 60;

  trigger_window[0].focus.current = 0;
  triggerbtn[0].so.got_focus = FALSE;
  triggerbtn[1].so.got_focus = FALSE;
  trigger_dropdown[0].so.got_focus = TRUE;

  if (this_trigger.mood_id == 0) {
    this_trigger.mood_id = mood_list[0].mood_id;
  }

  if ((this_trigger.code_letter > 'P') || (this_trigger.code_letter < 'A')) {
    trigger_dropdown[0].selected = 0;
    this_trigger.code_letter = 'A';
  } else {
    trigger_dropdown[0].selected = this_trigger.code_letter - 65;
  }
  if ((this_trigger.code_number < 1) || (this_trigger.code_number > 16)) {
    trigger_dropdown[1].selected = 0;
    this_trigger.code_number = 1;
  } else {
    trigger_dropdown[1].selected = this_trigger.code_number - 1;
  }

  if (this_trigger.incoming_state == 0) {
    trigger_checkbox[0].checked = FALSE; //on
    trigger_checkbox[1].checked = TRUE;  //off
  } else {
    trigger_checkbox[0].checked = TRUE;  //on
    trigger_checkbox[1].checked = FALSE; //off
  }
  trigger_dropdown[3].selected = 0;
  trigger_dropdown[2].selected = this_trigger.trigger_action - 1;

  for (i = 0; i < moods_used; i++) {
    mood_sublist[i] = &mood_list[i];
    if (mood_list[i].mood_id == this_trigger.mood_id) {
      listbox.item_selected = i;
    }
  }
  switch (trigger_dropdown[2].selected) {
  case 0:
    trigger_dropdown[3].selected = this_trigger.execute_type;
  case 1:
    listbox.sub_list = mood_sublist;
    listbox.item_count = moods_used;
    listbox.DisplayItem = DisplayMood;
    break;
  case 2:
  case 3:
    listbox.item_count = 0;
    break;
  case 4: //   "Chg Master Mode"
    listbox.item_selected = this_trigger.execute_type;
    listbox.sub_list = mode_sublistMAST;
    listbox.item_count = 2;
    listbox.DisplayItem = DisplayMode;
    break;
  case 5: //   "Chg HVAC Mode"
    listbox.item_selected = this_trigger.execute_type;
    listbox.sub_list = mode_sublistHVAC;
    listbox.item_count = 2;
    listbox.DisplayItem = DisplayMode;
    break;
  case 6: //   "Chg Season"
    listbox.item_selected = this_trigger.execute_type;
    listbox.sub_list = mode_sublistSEAS;
    listbox.item_count = 4;
    listbox.DisplayItem = DisplayMode;
    break;
  case 7: //   "Chg Occupied"
    listbox.item_selected = this_trigger.execute_type;
    listbox.sub_list = mode_sublistOCCU;
    listbox.item_count = 2;
    listbox.DisplayItem = DisplayMode;
    break;
  }

  hr = this_trigger.minutes / 60;
  min = this_trigger.minutes % 60;

  sb1.max = listbox.item_count - 1;
  sb1.thumb_pos = listbox.item_selected;
  sb1.large_chg = listbox.so.position.height / 14;
  if (sb1.large_chg > listbox.item_count) {
    sb1.large_chg = listbox.item_count;
  }

  if (trigger_dropdown[2].selected != 0) {
    trigger_dropdown[3].so.visible = FALSE;
  } else {
    trigger_dropdown[3].so.visible = TRUE;
    if (trigger_dropdown[3].selected == 1) //protected
    {
      hourspin.so.visible = TRUE;
      minspin.so.visible = TRUE;
    } else {
      hourspin.so.visible = FALSE;
      minspin.so.visible = FALSE;
    }
  }
  glbWindow = trigger_window;
  PutWindow(glbWindow);
}

static void TriggerWindowDraw(void) {
  ptext("Action", 244, 141, BG_TRANSPARENT + FG_BLK);
  ptext("Input Device", 113, 168, BG_TRANSPARENT + FG_BLK);
  ptext("Input State", 160, 168, BG_TRANSPARENT + FG_BLK);

  GroupBox("X10 Remote Input", 80, 127, 130, 170);
  GroupBox("Action to Take", 225, 127, 170, 170);

  ShowAction();
}

void TrigCancelBtnClick(void) {
  TriggerMenu();
}

void TriggerActionChange(void) {
  this_trigger.trigger_action = trigger_dropdown[2].selected + 1;
  switch (trigger_dropdown[2].selected) {
  case 0: // start mood
  case 1: // end mood
    listbox.sub_list = mood_sublist;
    listbox.item_count = moods_used;
    listbox.DisplayItem = DisplayMood;
    PutListBox(&listbox);
    break;
  case 2: // end all moods
  case 3: // day type to today
    listbox.item_count = 0;
    PutListBox(&listbox);
    break;
  case 4: //   "Chg Master Mode"
    listbox.sub_list = mode_sublistMAST;
    listbox.item_count = 2;
    listbox.DisplayItem = DisplayMode;
    PutListBox(&listbox);
    break;
  case 5: //   "Chg HVAC Mode"
    listbox.sub_list = mode_sublistHVAC;
    listbox.item_count = 2;
    listbox.DisplayItem = DisplayMode;
    PutListBox(&listbox);
    break;
  case 6: //   "Chg Season"
    listbox.sub_list = mode_sublistSEAS;
    listbox.item_count = 4;
    listbox.DisplayItem = DisplayMode;
    PutListBox(&listbox);
    break;
  case 7: //   "Chg Occupied"
    listbox.sub_list = mode_sublistOCCU;
    listbox.item_count = 2;
    listbox.DisplayItem = DisplayMode;
    PutListBox(&listbox);
    break;
  }
  ShowAction();
}

void ShowAction(void) {
  if (trigger_dropdown[2].selected != 0) {
    trigger_dropdown[3].so.visible = FALSE;
    hourspin.so.visible = FALSE;
    minspin.so.visible = FALSE;
    area_clear(283, 388, 139, 294, BG_WHT + FG_WHT);
  } else {
    if (trigger_dropdown[3].so.visible != TRUE) {
      trigger_dropdown[3].so.visible = TRUE;
      DisplayDropDown(&trigger_dropdown[3]);
    }
    ptext("Protect from Sched.", 284, 141, BG_TRANSPARENT + FG_BLK);
    if (trigger_dropdown[3].selected == 1) //protected
    {
      ptext("Hours    Minutes", 324, 141, BG_TRANSPARENT + FG_BLK);
      ptext("Duration of 0 time", 362, 141, BG_TRANSPARENT + FG_BLK);
      ptext("means run forever.", 376, 141, BG_TRANSPARENT + FG_BLK);
      hourspin.so.visible = TRUE;
      minspin.so.visible = TRUE;
      DisplaySpinner(&hourspin);
      DisplaySpinner(&minspin);
    } else {
      hourspin.so.visible = FALSE;
      minspin.so.visible = FALSE;
      area_clear(322, 389, 139, 294, BG_WHT + FG_WHT);
    }
  }
  switch (trigger_dropdown[2].selected) {
  case 0:
  case 1:
    area_clear(81, 94, 311, 516, FG_WHT + BG_WHT);
    GroupBox("Select a Mood", 80, 303, 315, 220);
    break;
  case 4:
  case 5:
  case 6:
  case 7:
    area_clear(81, 94, 311, 516, FG_WHT + BG_WHT);
    GroupBox("Select a Schedule Type", 80, 303, 315, 220);
    break;
  case 2:
  case 3:
  default:
    area_clear(81, 94, 311, 516, FG_WHT + BG_WHT);
    GroupBox("N/A", 80, 303, 315, 220);
    break;
  }
}

void TriggerMoodChange(void) {
  this_trigger.mood_id = mood_sublist[listbox.item_selected]->mood_id;
}

void TriggerProtChange(void) {
  this_trigger.execute_type = trigger_dropdown[3].selected;
  ShowAction();
}

void TriggerCheckBox1Click(void) {
  this_trigger.incoming_state = 1; //on
  trigger_checkbox[1].checked = FALSE;
  DisplayCheckBox(&trigger_checkbox[1]);
  trigger_checkbox[0].checked = TRUE;
  DisplayCheckBox(&trigger_checkbox[0]);
}

void TriggerCheckBox2Click(void) {
  this_trigger.incoming_state = 0; //off
  trigger_checkbox[0].checked = FALSE;
  DisplayCheckBox(&trigger_checkbox[0]);
  trigger_checkbox[1].checked = TRUE;
  DisplayCheckBox(&trigger_checkbox[1]);
}

static void TriggerNumberChange(void) {
  this_trigger.code_number = trigger_dropdown[1].selected + 1;
}

static void TriggerLetterChange(void) {
  this_trigger.code_letter = trigger_dropdown[0].selected + 65;
}

////////////////////////////////////////////////////////
//
//   END of EditTrigger()
//
////////////////////////////////////////////////////////

static TRIGGER_T* trigger_sublist[MAX_TRIGGERS];

PUSH_BUTTON_T trigger_menu_btn[4] = { // BEGIN SCREEN OBJECT DEFINITION
    419, 230, 89, 33,                 // position
    FALSE,                            // focus
    PUSH_BUTTON,                      // type
    TRUE,                             // visible
    BtnMouseDown,                     // OnMouseDown()
    BtnMouseUp,                       // OnMouseUp()
    BtnClick,                         // OnClick()
    BtnKeyPress,                      // OnKeyPress()
    PutButton,                        // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,           //default button
    "Add",           //text
    push,            //type
    up,              //state
    NULL,            //icon
    TrigAddBtnClick, //function
    121,

    // BEGIN SCREEN OBJECT DEFINITION
    419, 330, 89, 33, // position
    FALSE,            // focus
    PUSH_BUTTON,      // type
    TRUE,             // visible
    BtnMouseDown,     // OnMouseDown()
    BtnMouseUp,       // OnMouseUp()
    BtnClick,         // OnClick()
    BtnKeyPress,      // OnKeyPress()
    PutButton,        // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,           //default button
    "Remove",        //text
    push,            //type
    up,              //state
    NULL,            //icon
    TrigRmvBtnClick, //function
    122,

    // BEGIN SCREEN OBJECT DEFINITION
    419, 430, 89, 33, // position
    FALSE,            // focus
    PUSH_BUTTON,      // type
    TRUE,             // visible
    BtnMouseDown,     // OnMouseDown()
    BtnMouseUp,       // OnMouseUp()
    BtnClick,         // OnClick()
    BtnKeyPress,      // OnKeyPress()
    PutButton,        // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,           //default button
    "Change",        //text
    push,            //type
    up,              //state
    NULL,            //icon
    TrigEdtBtnClick, //function
    123,

    // BEGIN SCREEN OBJECT DEFINITION
    419, 545, 89, 33, // position
    FALSE,            // focus
    PUSH_BUTTON,      // type
    TRUE,             // visible
    BtnMouseDown,     // OnMouseDown()
    BtnMouseUp,       // OnMouseUp()
    BtnClick,         // OnClick()
    BtnKeyPress,      // OnKeyPress()
    PutButton,        // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    TRUE,        //default button
    NULL,        //text
    push,        //type
    up,          //state
    icon[12],    //icon
    ReturnClick, //function
    124};
static LISTBOX_T list_box = {
    // BEGIN SCREEN OBJECT DEFINITION
    80, 3, 615, 325,  // position
    FALSE,            // focus
    LIST_BOX,         // type
    TRUE,             // visible
    ListBoxMouseDown, // OnMouseDown()
    ListBoxMouseUp,   // OnMouseUp()
    ListBoxClick,     // OnClick()
    ListBoxKeyPress,  // OnKeyPress()
    PutListBox,       // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    0,                   // item count
    0,                   // item selected
    0,                   // top_item
    0,                   // display_count
    trigger_sublist,     // sub_list
    DisplayTrigger,      // display function pointer
    TriggerListBoxChange // on change event
};

static void sbOnClick(int direction, int how_far, int thumb_pos);
static SCROLL_BAR_T sb =
    {
        // BEGIN SCREEN OBJECT DEFINITION
        80, 619, 17, 325,     // position
        FALSE,                // focus
        SCROLL_BARV,          // type
        TRUE,                 // visible
        ScrollBarOnMouseDown, // OnMouseDown()
        ScrollBarOnMouseUp,   // OnMouseUp()
        ScrollBarOnClick,     // OnClick()
        NULL,                 // OnKeyPress()
        DisplayScrollBar,     // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        0,           // min
        15,          // max
        1,           // small change
        5,           // large change
        0,           // thumb possition
        SB_VERTICAL, // type
        sbOnClick    // on scroll function
};

SCREEN_OBJECT_T* trigger_menu_so[6];
UCHAR trigger_menu_title[] = "Remote Control Setup";
static void trigger_menu_windowDraw(void);
LWINDOW_T trigger_menu_window[1] = {
    35, 0, 640, 445, //window position
    trigger_menu_so, //screen object
    trigger_menu_windowDraw,
    6,                  //item count
    0,                  //cursor position 0-79 left to right
    0,                  //element that has current focus
    fcsBtn,             //focus type
    trigger_menu_title, // title
    DefaultOnKeyPress   //OnKeyPress();
};

//===========================================================================================
// NAME:   TriggerMenu()
//
// HISTORY:
// Author         Date         Description
// M. Lauter      15 DEC 96      Initial Version
//
//
//
// INPUTS:
// Name            Type         Description
// void
//
// OUTPUTS:
// Name            Type         Description
// void
//
// DESCRIPTION:
//
// TriggerMenu() allows a user to add & remove
// trigger devices through a visual interface
//
//===========================================================================================
void TriggerMenu(void) {
  USHORT i;
  UCHAR* rtn_btn[2] = {"Main|Menu", "Mood|Setup"};

  if (Sender == IntroScreen) {
    trigger_menu_btn[3].text = rtn_btn[0];
  } else {
    trigger_menu_btn[3].text = rtn_btn[1];
  }

  trigger_menu_so[0] = &trigger_menu_btn[0].so;
  trigger_menu_so[1] = &trigger_menu_btn[1].so;
  trigger_menu_so[2] = &trigger_menu_btn[2].so;
  trigger_menu_so[3] = &trigger_menu_btn[3].so;
  trigger_menu_so[4] = &list_box.so;
  trigger_menu_so[5] = &sb.so;

  for (i = 0; i < triggers_used; i++) {
    trigger_sublist[i] = &triggers[i];
  }
  list_box.item_selected = 0;
  list_box.item_count = triggers_used;
  sb.max = list_box.item_count - 1;
  sb.thumb_pos = list_box.item_selected;
  sb.large_chg = list_box.so.position.height / 14;
  if (sb.large_chg > list_box.item_count) {
    sb.large_chg = list_box.item_count;
  }
  glbWindow = trigger_menu_window;
  PutWindow(glbWindow);
}

static void trigger_menu_windowDraw(void) {
  ptext("Remote Address", 64, 6, BG_TRANSPARENT + FG_BLK);
  ptext("Remote Action", 64, 167, BG_TRANSPARENT + FG_BLK);
}

void TrigAddBtnClick(void) {
  TRIGGER_T new_trig;

  memset(&new_trig, 0, sizeof(TRIGGER_T));
  new_trig.code_letter = 1;
  new_trig.code_number = 'A';
  new_trig.incoming_state = 0;
  new_trig.trigger_action = 1;
  new_trig.execute_type = 0;
  new_trig.minutes = 0;
  trigger_index = -1;
  EditTrigger(&new_trig, 'A');
}

void TrigRmvBtnClick(void) {
  int result;

  if (list_box.item_count > 0) {
    if (confirmation == TRUE) {
      result = msgbox("Are you sure?",
                      "Caution", MB_YES_NO);

    } else {
      result = MB_RESULT_YES;
    }

    if (result == MB_RESULT_YES) {
      if (RemoveTrigger(trigger_sublist[list_box.item_selected]) == SUCCESS) {
        Status("Successfully removed.");
        --list_box.item_count;
        PutListBox(&list_box);
      } else {
        msgbox("Unable to remove RC trigger.|Please try again.", "Information", MB_OK);
      }
    }
  }
}

static void TriggerListBoxChange(void) {
  sb.max = list_box.item_count - 1;
  sb.thumb_pos = list_box.item_selected;
  sb.large_chg = list_box.so.position.height / 14;
  if (sb.large_chg > list_box.item_count) {
    sb.large_chg = list_box.item_count;
  }
  DisplayScrollBar(&sb);
}

void TrigEdtBtnClick(void) {
  if (list_box.item_count == 0) {
    return;
  }
  trigger_index = list_box.item_selected;
  EditTrigger(trigger_sublist[list_box.item_selected], 'C');
}

void DisplayTrigger(TRIGGER_T* trigger, int top, int left, UCHAR color) {
  UCHAR trigger_info[85];
  UCHAR* in_state[2] = {"Off", "On"};
  UCHAR mood_affected[45];

  memset(mood_affected, 0, 45);
  switch (trigger->trigger_action) {
  case 1: // start a mood
    if (trigger->execute_type == 0) {
      sprintf(mood_affected,
              "%-20s Unprotected",
              mood_list[LookupMoodIndex(trigger->mood_id)].name);
    } else {
      if (trigger->minutes > 0) {
        sprintf(mood_affected,
                "%-20s Prot. %2dhr %2dmn",
                mood_list[LookupMoodIndex(trigger->mood_id)].name,
                trigger->minutes / 60,
                trigger->minutes % 60);
      } else {
        sprintf(mood_affected,
                "%-20s Prot. Infinite",
                mood_list[LookupMoodIndex(trigger->mood_id)].name,
                trigger->minutes / 60,
                trigger->minutes % 60);
      }
    }
    break;
  case 2: // end a mood
    sprintf(mood_affected,
            "%-20s",
            mood_list[LookupMoodIndex(trigger->mood_id)].name);
    break;
  case 3: // end all moods
    sprintf(mood_affected,
            "All Moods");
    break;
  case 4: // set day type to today
    break;
  case 5: //   "Chg Master Mode"
    sprintf(mood_affected,
            "%-20s",
            mode_sublistMAST[trigger->execute_type]);
    break;
  case 6: //   "Chg HVAC Mode"
    sprintf(mood_affected,
            "%-20s",
            mode_sublistHVAC[trigger->execute_type]);
    break;
  case 7: //   "Chg Season"
    sprintf(mood_affected,
            "%-20s",
            mode_sublistSEAS[trigger->execute_type]);
    break;
  case 8: //   "Chg Occupied"
    sprintf(mood_affected,
            "%-20s",
            mode_sublistOCCU[trigger->execute_type]);
    break;
  }

  sprintf(trigger_info,
          "X-10: %c:%-2d:%-3s   %-15s   %-s",
          trigger->code_letter,
          trigger->code_number,
          in_state[trigger->incoming_state],
          actions_t[trigger->trigger_action - 1],
          mood_affected);
  ptext(trigger_info, top, left, color);
}

static void sbOnClick(int direction, int how_far, int thumb_pos) {
  switch (direction) {
  case SCROLL_UP:
    switch (how_far) {
    case SCROLL_LARGE:
      PageUp(&list_box);
      break;
    case SCROLL_SMALL:
      UpArrow(&list_box);
      break;
    }
    break;
  case SCROLL_DOWN:
    switch (how_far) {
    case SCROLL_LARGE:
      PageDown(&list_box);
      break;
    case SCROLL_SMALL:
      DownArrow(&list_box);
      break;
    }
    break;
  }
  ListBoxScroll(&list_box);
  //PutListBox(&list_box);
}

static void DurationChange(void) {
  this_trigger.minutes = (unsigned)((hourspin.value * 60) + minspin.value);
}

void TrigSaveBtnClick(void) {
  int i;

  DurationChange();
  TriggerNumberChange();
  TriggerLetterChange();
  if (trigger_checkbox[0].checked == TRUE) {
    this_trigger.incoming_state = 1; //on
  } else {
    this_trigger.incoming_state = 0; //off
  }

  for (i = 0; (USHORT)i < triggers_used; i++) {
    if (
        (triggers[i].code_letter == this_trigger.code_letter) &&
        (triggers[i].code_number == this_trigger.code_number) &&
        (triggers[i].incoming_state == this_trigger.incoming_state) &&
        (i != trigger_index)) {
      msgbox("An RC trigger already exists|with this input state.|Change it and try again.", "Error Saving Definition", MB_OK);
      return;
    }
  }
  for (i = 0; i < dev_list.devices_used; i++) {
    if (
        (dev_list.devices[i].code_letter == this_trigger.code_letter) &&
        (dev_list.devices[i].code_number == this_trigger.code_number)) {
      msgbox("A device already exists|with this X-10 address.|Change it and try again.", "Error Saving Definition", MB_OK);
      return;
    }
  }

  //TriggerActionChange();
  this_trigger.trigger_action = trigger_dropdown[2].selected + 1;
  switch (trigger_dropdown[2].selected) {
  case 0: // start a mood
    this_trigger.execute_type = trigger_dropdown[3].selected;
  case 1: // end a mood
    TriggerMoodChange();
    if (listbox.item_count == 0) {
      msgbox("You can't do this until|you have defined some moods.", "Error Saving Definition", MB_OK);
      return;
    }
    break;

  case 2: // end all moods
  case 3: // set day type to today
    break;

  case 4: //   "Chg Master Mode"
  case 5: //   "Chg HVAC Mode"
  case 6: //   "Chg Season"
  case 7: //   "Chg Occupied"
    this_trigger.execute_type = listbox.item_selected;
    break;
  }

  sb.max = list_box.item_count - 1;
  sb.thumb_pos = list_box.item_selected;
  sb.large_chg = list_box.so.position.height / 14;
  if (sb.large_chg > list_box.item_count) {
    sb.large_chg = list_box.item_count;
  }

  if (SendObject(&this_trigger, trig_opt, sizeof(TRIGGER_T), TRIGGER_TABLE) == SUCCESS) {
    if (trig_opt == 'A') {
      triggers[triggers_used] = this_trigger;
      triggers_used++;
    } else {
      memcpy(this_trigger_ptr, &this_trigger, sizeof(TRIGGER_T));
    }
    if (user_backup() == SUCCESS) {
      Status("RC Trigger successfully saved.");
    } else {
      msgbox("Unable to save RC trigger.|Check disk space.", "Warning", MB_OK);
      return;
    }
  } else {
    msgbox("Unable to save RC trigger.|Please try again.", "Warning", MB_OK);
    return;
  }
  TriggerMenu();
}

void DisplayMode(UCHAR* mode, int top, int left, UCHAR color) {
  UCHAR desc[21];

  sprintf(desc, "%-20.20s", mode);
  ptext(desc, top, left, color);
}

static void ReturnClick(void) {
  (*Sender)();
}

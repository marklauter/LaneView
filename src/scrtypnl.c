//security panel

#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>
#include <conio.h>
#include <string.h>
#include <sys\types.h>
#include <sys\utime.h>

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
#include "inutil.h"
#include "spinner.h"
#include "scrolbar.h"
#include "mouse.h"

//int   moods_used;
//int   active_moods;
//MOOD_T mood_list[MAX_MOODS];

extern time_t security_chg_icu;
static void sp_CancelBtnClick(void);
static void sp_SaveBtnClick(void);
static void spCheckBox1Click(void);
static void spCheckBox2Click(void);
static void DisplaySecurityState(UCHAR* security_state, int top, int left, UCHAR color);
static void splistboxOnChange(void);
static void ddActionOnChange(void);
static void ddMoodOnChange(void);
static void ddProtOnChange(void);
static void CEGateSetupClick(void);
static void DurationChange(void);

SECURITY_T security_device;

static UCHAR* actions_t[9] =
    {
        "No Action",
        "Start Mood",
        "End Mood",
        "End All Moods",
        "Day Type Today",
        "Chg Master Mode",
        "Chg HVAC Mode",
        "Chg Season",
        "Chg Occupied"};

UCHAR* action_types[2] =
    {
        "Unprotected",
        "Protected"};
extern MOOD_T* mood_sublist[MAX_MOODS];

extern void DisplayMode(UCHAR* mode, int top, int left, UCHAR color);
extern void DisplayMood(MOOD_T* mood, int top, int left, UCHAR color);

extern UCHAR* mode_sublistHVAC[2];
extern UCHAR* mode_sublistSEAS[4];
extern UCHAR* mode_sublistOCCU[2];
extern UCHAR* mode_sublistMAST[2];

static UCHAR* security_states[10] = {
    "Disarmed",
    "Armed - Away",
    "Armed - Stay",
    "Armed - Instant",
    "Armed - Maximum",
    "Alarm Condition",
    "Alarm - End",
    "Alarm - Fire",
    "Alarm - Intrusion",
    "Alarm - Trouble"};

PUSH_BUTTON_T sp_btn_list[3] = {
    // BEGIN SCREEN OBJECT DEFINITION
    419, 347, 89, 33, // position
    FALSE,            // focus
    PUSH_BUTTON,      // type
    TRUE,             // visible
    BtnMouseDown,     // OnMouseDown()
    BtnMouseUp,       // OnMouseUp()
    BtnClick,         // OnClick()
    BtnKeyPress,      // OnKeyPress()
    PutButton,        // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,            // default button
    "CEGate|Setup",   // text
    push,             // type
    up,               // state
    NULL,             // icon
    CEGateSetupClick, // function
    87,

    // BEGIN SCREEN OBJECT DEFINITION
    419, 446, 89, 33, // position
    FALSE,            // focus
    PUSH_BUTTON,      // type
    TRUE,             // visible
    BtnMouseDown,     // OnMouseDown()
    BtnMouseUp,       // OnMouseUp()
    BtnClick,         // OnClick()
    BtnKeyPress,      // OnKeyPress()
    PutButton,        // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,           // default button
    "Apply",         // text
    push,            // type
    up,              // state
    NULL,            // icon
    sp_SaveBtnClick, // function
    88,

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
    TRUE,              // default button
    "Cancel",          // text
    push,              // type
    up,                // state
    icon[12],          // icon
    sp_CancelBtnClick, // function
    89};

static LISTBOX_T list_box = {
    // BEGIN SCREEN OBJECT DEFINITION
    65, 10, 620, 146, // position
    FALSE,            // focus
    LIST_BOX,         // type
    TRUE,             // visible
    ListBoxMouseDown, // OnMouseDown()
    ListBoxMouseUp,   // OnMouseUp()
    ListBoxClick,     // OnClick()
    ListBoxKeyPress,  // OnKeyPress()
    PutListBox,       // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    10,                   // item count
    0,                    // item selected
    0,                    // top_item
    0,                    // display_count
    security_states,      // sub_list
    DisplaySecurityState, // display function pointer
    splistboxOnChange     // on change event
};

static void sbOnClick1(int direction, int how_far, int thumb_pos);
static void list_boxOnChange(void);
static LISTBOX_T listbox =
    {
        // BEGIN SCREEN OBJECT DEFINITION
        250, 330, 170, 155, // position
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
static SCROLL_BAR_T sb1 =
    {
        // BEGIN SCREEN OBJECT DEFINITION
        250, 501, 17, 155,    // position
        FALSE,                // focus
        SCROLL_BARV,          // type
        TRUE,                 // visible
        ScrollBarOnMouseDown, // OnMouseDown()
        NULL,                 // OnMouseUp()
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
  (*listbox.OnChange)();
}

static void list_boxOnChange(void) {
  sb1.max = listbox.item_count - 1;
  sb1.thumb_pos = listbox.item_selected;
  sb1.large_chg = listbox.so.position.height / 14;
  if (sb1.large_chg > listbox.item_count) {
    sb1.large_chg = listbox.item_count;
  }
  DisplayScrollBar(&sb1);
  ddMoodOnChange();
}

//UCHAR *sec_moods[MAX_MOODS];
//UCHAR sec_mood_names[MAX_MOODS][21];

DROPDOWN_BOX_T sp_ddb_list[2] = {
    //action
    270, 135, 155, 17,
    FALSE,             // got_focus
    DROP_DOWN_BOX,     // type
    TRUE,              // visible
    DropDownMouseDown, // OnMouseDown()
    DropDownMouseUp,   // OnMouseUp()
    DropDownClick,     // OnClick()
    DropDownKeyPress,  // OnKeyPress()
    DisplayDropDown,   // DisplayObject()
    actions_t,         // strings
    0,                 // item selected
    FALSE,             // dropped
    9,                 // item count
    ddActionOnChange,  // OnChange

    //type (protected, unprotected)
    316, 135, 155, 17,
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
    ddProtOnChange     // OnChange
};

TEXT_BOX_T sp_txt_list[2] = { //house code
    0, 0, 0, 14,              // position
    TRUE,                     // got_focus
    TEXT_BOX,                 // type
    TRUE,                     // visible
    TxtMouseDown,             // OnMouseDown()
    TxtMouseUp,               // OnMouseUp()
    TxtClick,                 // OnClick()
    TxtKeyPress,              // OnKeyPress()
    PutTxtBox,                // DisplayObject()
    //////////////////////////////////////end of screen object definition
    12,     // row
    42,     // column
    4,      // box length
    tb_hex, // type
    "",     // text
    NULL,

    //unit/mac
    0, 0, 0, 14,  // position
    TRUE,         // got_focus
    TEXT_BOX,     // type
    TRUE,         // visible
    TxtMouseDown, // OnMouseDown()
    TxtMouseUp,   // OnMouseUp()
    TxtClick,     // OnClick()
    TxtKeyPress,  // OnKeyPress()
    PutTxtBox,    // DisplayObject()
    //////////////////////////////////////end of screen object definition
    14,     // row
    42,     // column
    4,      // box length
    tb_hex, // type
    "",     // text
    NULL};

static SPINNER_T hourspin =
    {
        360, 135, 1, 1,
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
        360, 213, 1, 1,
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

#define sp_object_count 10
SCREEN_OBJECT_T* sp_so_list[sp_object_count];

static void SPDraw(void);
LWINDOW_T sp_window = {
    35, 0, 640, 445, //window position
    sp_so_list,      //screen object
    SPDraw,
    sp_object_count,         //item count
    0,                       //cursor position 0-79 left to right
    0,                       //element that has current focus
    fcsBtn,                  //focus type
    "Security System Setup", // title
    DefaultOnKeyPress        //OnKeyPress();
};

extern LWINDOW_T progWindow[1];
static UCHAR sec_title[] = {"Security"};

void SecurityPanel(void) {
  int i;

  sp_so_list[0] = &list_box.so;

  sp_so_list[1] = &sp_ddb_list[0].so;
  sp_so_list[2] = &sp_ddb_list[1].so;

  sp_so_list[3] = &hourspin.so;
  sp_so_list[4] = &minspin.so;

  sp_so_list[5] = &listbox.so;

  sp_so_list[6] = &sp_btn_list[0].so; //cegate setup
  sp_so_list[7] = &sp_btn_list[1].so; //save
  sp_so_list[8] = &sp_btn_list[2].so; //cancel

  sp_so_list[9] = &sb1.so;

  for (i = 0; i < moods_used; i++) {
    mood_sublist[i] = &mood_list[i];
    if (mood_list[i].mood_id == security_device.arm_level[0].mood_id) {
      listbox.item_selected = i;
    }
  }

  switch (sp_ddb_list[0].selected - 1) {
  case 0: //start a mood
    sp_ddb_list[1].selected = security_device.arm_level[0].execute_type;
  case 1: //end a mood
    listbox.sub_list = mood_sublist;
    listbox.item_count = moods_used;
    listbox.DisplayItem = DisplayMood;
    break;
  case 4: //   "Chg Master Mode"
    listbox.item_selected = security_device.arm_level[0].execute_type;
    listbox.sub_list = mode_sublistMAST;
    listbox.item_count = 2;
    listbox.DisplayItem = DisplayMode;
    break;
  case 5: //   "Chg HVAC Mode"
    listbox.item_selected = security_device.arm_level[0].execute_type;
    listbox.sub_list = mode_sublistHVAC;
    listbox.item_count = 2;
    listbox.DisplayItem = DisplayMode;
    break;
  case 6: //   "Chg Season"
    listbox.item_selected = security_device.arm_level[0].execute_type;
    listbox.sub_list = mode_sublistSEAS;
    listbox.item_count = 4;
    listbox.DisplayItem = DisplayMode;
    break;
  case 7: //   "Chg Occupied"
    listbox.item_selected = security_device.arm_level[0].execute_type;
    listbox.sub_list = mode_sublistOCCU;
    listbox.item_count = 2;
    listbox.DisplayItem = DisplayMode;
    break;
  case 2:  //   end all moods
  case 3:  //   set day type to today
  default: //   no action
    listbox.item_count = 0;
    security_device.arm_level[list_box.item_selected].execute_type = 0;
    break;
  }

  list_box.item_selected = 0;
  hourspin.value = security_device.arm_level[0].minutes / 60;
  minspin.value = security_device.arm_level[0].minutes % 60;

  listbox.item_count = moods_used;
  glbWindow = &sp_window;
  PutWindow(glbWindow);
}

static void SPDraw(void) {
  ptext("Action", 255, 136, BG_TRANSPARENT + FG_BLK);
  ptext("Mood Type", 301, 136, BG_TRANSPARENT + FG_BLK);
  ptext("Duration (hr:min)", 344, 136, BG_TRANSPARENT + FG_BLK);

  GroupBox("Action to Take", 225, 116, 185, 190);
  GroupBox("Moods & Modes", 225, 315, 185, 214);
  splistboxOnChange();
}

void splistboxOnChange(void) {
  int i;

  if (list_box.item_selected == 5) {
    list_box.item_selected = 6;
    //PutListBox(&list_box);
    ListBoxScroll(&list_box);
  }
  sp_ddb_list[0].selected = security_device.arm_level[list_box.item_selected].trigger_action;
  hourspin.value = security_device.arm_level[list_box.item_selected].minutes / 60;
  minspin.value = security_device.arm_level[list_box.item_selected].minutes % 60;
  sp_ddb_list[1].selected = 0;
  switch (sp_ddb_list[0].selected - 1) {
  case 0: //start a mood
    sp_ddb_list[1].selected = security_device.arm_level[list_box.item_selected].execute_type;
  case 1: //end a mood
    i = LookupMoodIndex(security_device.arm_level[list_box.item_selected].mood_id);
    if (i != -1) {
      listbox.item_selected = i;
    } else {
      listbox.item_selected = 0;
    }
    listbox.sub_list = mood_sublist;
    listbox.item_count = moods_used;
    listbox.DisplayItem = DisplayMood;
    break;
  case 4: //   "Chg Master Mode"
    listbox.item_selected = security_device.arm_level[list_box.item_selected].execute_type;
    listbox.sub_list = mode_sublistMAST;
    listbox.item_count = 2;
    listbox.DisplayItem = DisplayMode;
    break;
  case 5: //   "Chg HVAC Mode"
    listbox.item_selected = security_device.arm_level[list_box.item_selected].execute_type;
    listbox.sub_list = mode_sublistHVAC;
    listbox.item_count = 2;
    listbox.DisplayItem = DisplayMode;
    break;
  case 6: //   "Chg Season"
    listbox.item_selected = security_device.arm_level[list_box.item_selected].execute_type;
    listbox.sub_list = mode_sublistSEAS;
    listbox.item_count = 4;
    listbox.DisplayItem = DisplayMode;
    break;
  case 7: //   "Chg Occupied"
    listbox.item_selected = security_device.arm_level[list_box.item_selected].execute_type;
    listbox.sub_list = mode_sublistOCCU;
    listbox.item_count = 2;
    listbox.DisplayItem = DisplayMode;
    break;
  case 2:  //   end all moods
  case 3:  //   set day type to today
  default: //   no action
    listbox.item_count = 0;
    security_device.arm_level[list_box.item_selected].execute_type = 0;
    break;
  }
  DisplayDropDown(&sp_ddb_list[0]);
  DisplayDropDown(&sp_ddb_list[1]);
  PutListBox(&listbox);
  DisplayDropDown(&sp_ddb_list[1]);
}

void DisplaySecurityState(UCHAR* security_state, int top, int left, UCHAR color) {
  char lot[81]; //line of text
  char moodname[21];
  char action[15];
  int i;
  int j;

  memset(lot, 0, 81);
  memset(moodname, 0, 21);
  memset(action, 0, 13);

  for (i = 0; i < 10; i++) {
    if (strcmp(security_state, security_states[i]) == 0) {
      break;
    }
  }
  switch (security_device.arm_level[i].trigger_action - 1) {
  case 0: //start a mood
    sprintf(action, "%-14.14s", action_types[security_device.arm_level[i].execute_type]);
  case 1: //end a mood
    for (j = 0; j < moods_used; j++) {
      if (mood_list[j].mood_id == security_device.arm_level[i].mood_id) {
        sprintf(moodname, "%-20.20s", mood_list[j].name);
        break;
      }
    }
    break;
  case 4: //   "Chg Master Mode"
    sprintf(moodname, "%-20.20s", mode_sublistMAST[security_device.arm_level[i].execute_type]);
    break;
  case 5: //   "Chg HVAC Mode"
    sprintf(moodname, "%-20.20s", mode_sublistHVAC[security_device.arm_level[i].execute_type]);
    break;
  case 6: //   "Chg Season"
    sprintf(moodname, "%-20.20s", mode_sublistSEAS[security_device.arm_level[i].execute_type]);
    break;
  case 7: //   "Chg Occupied"
    sprintf(moodname, "%-20.20s", mode_sublistOCCU[security_device.arm_level[i].execute_type]);
    break;
  case 2:  //   end all moods
  case 3:  //   set day type to today
  default: //   no action
    break;
  }
  sprintf(lot,
          "%-19s%-17s%-23s%-12s",
          security_state,
          actions_t[security_device.arm_level[i].trigger_action],
          moodname,
          action);
  ptext(lot, top, left, color);
}

void sp_SaveBtnClick(void) {
  Busy();
  Status("Please wait...");
  if (SendObject(&security_device, 'C', sizeof(SECURITY_T), SECURITY_TABLE) == SUCCESS) {
    user_backup();
    IntroScreen();
    NotBusy();
    Status("Security setup saved successfully.");
  } else {
    Status("");
    NotBusy();
    msgbox("Unable to save security device.|Try again.", "Save Security Information", MB_OK);
    return;
  }
}

void sp_CancelBtnClick(void) {
  IntroScreen();
}

void ddActionOnChange(void) {
  security_device.arm_level[list_box.item_selected].execute_type = 0;
  sp_ddb_list[1].selected = 0;
  switch (sp_ddb_list[0].selected - 1) {
  case 0: //start a mood
  case 1: //end a mood
    listbox.sub_list = mood_sublist;
    listbox.item_count = moods_used;
    listbox.DisplayItem = DisplayMood;
    security_device.arm_level[list_box.item_selected].execute_type = sp_ddb_list[1].selected;
    security_device.arm_level[list_box.item_selected].mood_id = mood_sublist[listbox.item_selected]->mood_id;
    break;
  case 4: //   "Chg Master Mode"
    listbox.item_selected = 0;
    listbox.sub_list = mode_sublistMAST;
    listbox.item_count = 2;
    listbox.DisplayItem = DisplayMode;
    break;
  case 5: //   "Chg HVAC Mode"
    listbox.item_selected = 0;
    listbox.sub_list = mode_sublistHVAC;
    listbox.item_count = 2;
    listbox.DisplayItem = DisplayMode;
    break;
  case 6: //   "Chg Season"
    listbox.item_selected = 0;
    listbox.sub_list = mode_sublistSEAS;
    listbox.item_count = 4;
    listbox.DisplayItem = DisplayMode;
    break;
  case 7: //   "Chg Occupied"
    listbox.item_selected = 0;
    listbox.sub_list = mode_sublistOCCU;
    listbox.item_count = 2;
    listbox.DisplayItem = DisplayMode;
    break;
  case 2:  //   end all moods
  case 3:  //   set day type to today
  default: //   no action
    listbox.item_count = 0;
    break;
  }
  security_device.arm_level[list_box.item_selected].trigger_action = (UCHAR)sp_ddb_list[0].selected;
  //PutListBox(&list_box);
  ListBoxScroll(&list_box);
  PutListBox(&listbox);
  DisplayDropDown(&sp_ddb_list[1]);
}

void ddMoodOnChange(void) {
  switch (sp_ddb_list[0].selected - 1) {
  case 0: //start a mood
  case 1: //end a mood
    security_device.arm_level[list_box.item_selected].execute_type = sp_ddb_list[1].selected;
    security_device.arm_level[list_box.item_selected].mood_id = mood_sublist[listbox.item_selected]->mood_id;
    break;
  case 4: //   "Chg Master Mode"
  case 5: //   "Chg HVAC Mode"
  case 6: //   "Chg Season"
  case 7: //   "Chg Occupied"
    security_device.arm_level[list_box.item_selected].execute_type = listbox.item_selected;
    break;
  case 2:  //   end all moods
  case 3:  //   set day type to today
  default: //   no action
    security_device.arm_level[list_box.item_selected].execute_type = 0;
    break;
  }
  //PutListBox(&list_box);
  ListBoxScroll(&list_box);
}

void ddProtOnChange(void) {
  if ((sp_ddb_list[0].selected == 1) || (sp_ddb_list[0].selected == 1)) {
    security_device.arm_level[list_box.item_selected].execute_type = sp_ddb_list[1].selected;
    //PutListBox(&list_box);
    ListBoxScroll(&list_box);
  }
}

static void SetupOkClick(void);
static void SetupCancelClick(void);

static PUSH_BUTTON_T setup_buttons[2] = {
    // BEGIN SCREEN OBJECT DEFINITION
    230, 225, 89, 33, // position
    FALSE,            // focus
    PUSH_BUTTON,      // type
    TRUE,             // visible
    BtnMouseDown,     // OnMouseDown()
    BtnMouseUp,       // OnMouseUp()
    BtnClick,         // OnClick()
    BtnKeyPress,      // OnKeyPress()
    PutButton,        // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    TRUE,         // default button
    "Ok",         // text
    push,         // type
    up,           // state
    NULL,         // icon
    SetupOkClick, // function
    90,

    // BEGIN SCREEN OBJECT DEFINITION
    230, 324, 89, 33, // position
    FALSE,            // focus
    PUSH_BUTTON,      // type
    TRUE,             // visible
    BtnMouseDown,     // OnMouseDown()
    BtnMouseUp,       // OnMouseUp()
    BtnClick,         // OnClick()
    BtnKeyPress,      // OnKeyPress()
    PutButton,        // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,            // default button
    "Cancel",         // text
    push,             // type
    up,               // state
    NULL,             // icon
    SetupCancelClick, // function
    91};

static SCREEN_OBJECT_T* setup_so[4];
void CEGateSetupClickDraw(void);
static LWINDOW_T cegate_setup_window =
    {
        105, 190, 260, 180, //window position
        setup_so,           //screen object
        CEGateSetupClickDraw,
        4,                //item count
        0,                //cursor position 0-79 left to right
        0,                //element that has current focus
        fcsBtn,           //focus type
        "CEGate Setup",   // title
        DefaultOnKeyPress //OnKeyPress();
};

static void CEGateSetupClick(void) {
  setup_so[0] = &sp_txt_list[0].so;
  setup_so[1] = &sp_txt_list[1].so;
  setup_so[2] = &setup_buttons[0].so;
  setup_so[3] = &setup_buttons[1].so;

  sprintf(sp_txt_list[0].text, "%.4X", security_device.house_code);
  sprintf(sp_txt_list[1].text, "%.4X", security_device.unit);

  glbWindow = &cegate_setup_window;
  PutWindow(&cegate_setup_window);
}

void CEGateSetupClickDraw(void) {
  ptext("House Code", 153, 240, BG_TRANSPARENT + FG_BLK);
  ptext("Unit/MAC", 181, 240, BG_TRANSPARENT + FG_BLK);
}

static void SetupOkClick(void) {
  security_device.house_code = htoi(sp_txt_list[0].text);
  security_device.unit = htoi(sp_txt_list[1].text);
  if (SendObject(&security_device, 'C', sizeof(SECURITY_T), SECURITY_TABLE) == SUCCESS) {
    if (user_backup() == SUCCESS) {
      Status("Security device saved successfully.");
    } else {
      msgbox("Unable to save security|device to disk.|Check disk space.", "Warning", MB_OK);
      return;
    }
  } else {
    msgbox("Unable to save security|device to ICU.|Try again.", "Warning", MB_OK);
    return;
  }
  SecurityPanel();
}

static void SetupCancelClick(void) {
  SecurityPanel();
}

static void DurationChange(void) {
  if (list_box.item_selected < 6) // arm_level
  {
    security_device.arm_level[list_box.item_selected].minutes = (unsigned)((hourspin.value * 60) + minspin.value);
  } else //alarm_level
  {
    security_device.alarm_level[list_box.item_selected - 6].minutes = (unsigned)((hourspin.value * 60) + minspin.value);
  }
}

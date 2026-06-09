//MoodMenuWindow
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
#include "listbox.h"
#include "mouse.h"
#include "lwindow.h"
#include "ismenu.h"
#include "moodmenu.h"
#include "frame_3d.h"
#include "device.h"
#include "msgbox.h"
#include "scrolbar.h"
#include "dropdown.h"
#include "device.h"
#include "insubs.h"
#include "inutil.h"
#include "inio.h"
#include "trigger.h"
#include "event.h"

int moods_used;
int active_moods;
MOOD_T mood_list[MAX_MOODS];
static ACTIVE_MOOD act_mood_list[MAX_MOODS];
extern MOOD_T* mood_sublist[MAX_MOODS];
static MOOD_T this_mood;
static int glbOpt;

static void TriggerSetupClick(void);
static int IsMoodActive(MOOD_T* mood);
static int IsDeviceInMood(MOOD_T* mood, USHORT device_id);
static int LookupActMood(USHORT mood_id);
static void DisplayBrightness(UCHAR dim_level);
static void EndAllMoodBtnClick(void);
static void AddMoodBtnClick(void);
static void EditMoodBtnClick(void);
static void AddDeviceBtnClick(void);
static void adCancelBtnClick(void);
static void AddDevice(void);
static void adSaveBtnClick(void);
static void DisplayMoodEntry(MOOD_ENTRY* me, int top, int left, UCHAR color);
static void em_list_boxChange(void);
static void emSaveBtnClick(void);
static void emsbOnScroll(int direction, int how_far, int thumb_pos);
static void DisplayAction(void);
static void ActMoodBtnClick(void);
static void DeActMoodBtnClick(void);
static void RmvBtnClick(void);
static void RmvDeviceBtnClick(void);
static void DisplayMood(MOOD_T* md, int top, int left, UCHAR color);
static void EditMood(MOOD_T* mood, int opt);

PUSH_BUTTON_T mmButton[8] = {
    // BEGIN SCREEN OBJECT DEFINITION
    115, 380, 89, 33, // position
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
    "Add Mood",      //text
    push,            //type
    up,              //state
    NULL,            //icon
    AddMoodBtnClick, //function
    127,

    // BEGIN SCREEN OBJECT DEFINITION
    115, 485, 89, 33, // position
    FALSE,            // focus
    PUSH_BUTTON,      // type
    TRUE,             // visible
    BtnMouseDown,     // OnMouseDown()
    BtnMouseUp,       // OnMouseUp()
    BtnClick,         // OnClick()
    BtnKeyPress,      // OnKeyPress()
    PutButton,        // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,            //default button
    "Change|Mood",    //text
    push,             //type
    up,               //state
    NULL,             //icon
    EditMoodBtnClick, //function
    128,

    // BEGIN SCREEN OBJECT DEFINITION
    160, 380, 89, 33, // position
    FALSE,            // focus
    PUSH_BUTTON,      // type
    TRUE,             // visible
    BtnMouseDown,     // OnMouseDown()
    BtnMouseUp,       // OnMouseUp()
    BtnClick,         // OnClick()
    BtnKeyPress,      // OnKeyPress()
    PutButton,        // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,         //default button
    "Remove|Mood", //text
    push,          //type
    up,            //state
    NULL,          //icon
    RmvBtnClick,   //function
    129,

    // BEGIN SCREEN OBJECT DEFINITION
    250, 380, 89, 33, // position
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
    "Start|Mood",    //text
    push,            //type
    up,              //state
    NULL,            //icon
    ActMoodBtnClick, //function
    130,

    // BEGIN SCREEN OBJECT DEFINITION
    250, 485, 89, 33, // position
    FALSE,            // focus
    PUSH_BUTTON,      // type
    TRUE,             // visible
    BtnMouseDown,     // OnMouseDown()
    BtnMouseUp,       // OnMouseUp()
    BtnClick,         // OnClick()
    BtnKeyPress,      // OnKeyPress()
    PutButton,        // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,             //default button
    "End|Mood",        //text
    push,              //type
    up,                //state
    NULL,              //icon
    DeActMoodBtnClick, //function
    131,

    // BEGIN SCREEN OBJECT DEFINITION
    295, 380, 89, 33, // position
    FALSE,            // focus
    PUSH_BUTTON,      // type
    TRUE,             // visible
    BtnMouseDown,     // OnMouseDown()
    BtnMouseUp,       // OnMouseUp()
    BtnClick,         // OnClick()
    BtnKeyPress,      // OnKeyPress()
    PutButton,        // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,              // default button
    "End All|Moods",    // text
    push,               // type
    up,                 // state
    NULL,               // icon
    EndAllMoodBtnClick, // function
    132,

    295, 485, 89, 33,  // position
    FALSE,             //focus
    PUSH_BUTTON,       //type
    TRUE,              // visible
    BtnMouseDown,      //OnMouseDown()
    BtnMouseUp,        //OnMouseUp()
    BtnClick,          // OnClick()
    BtnKeyPress,       // OnKeyPress()
    PutButton,         // DisplayObject()
    FALSE,             //default button
    "Remote|Control",  //text
    push,              //type
    up,                //state
    NULL,              //icon
    TriggerSetupClick, //function
    133,

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
    "Main|Menu", //text
    push,        //type
    up,          //state
    icon[12],    //icon
    IntroScreen, //function
    134};

static void list_boxOnChange(void);
static LISTBOX_T mm_list_box = { // BEGIN SCREEN OBJECT DEFINITION
    110, 60, 244, 314,           // position
    FALSE,                       // focus
    LIST_BOX,                    // type
    TRUE,                        // visible
    ListBoxMouseDown,            // OnMouseDown()
    ListBoxMouseUp,              // OnMouseUp()
    ListBoxClick,                // OnClick()
    ListBoxKeyPress,             // OnKeyPress()
    PutListBox,                  // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    0,            // item count
    0,            // item selected
    0,            // top_item
    0,            // display_count
    mood_sublist, // sub_list
    DisplayMood,  // display function pointer
    list_boxOnChange};

///////////////////////////////////////////
static void sbOnClick(int direction, int how_far, int thumb_pos);
static SCROLL_BAR_T sb =
    {
        // BEGIN SCREEN OBJECT DEFINITION
        110, 305, 17, 314,    // position
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

static void sbOnClick(int direction, int how_far, int thumb_pos) {
  switch (direction) {
  case SCROLL_UP:
    switch (how_far) {
    case SCROLL_LARGE:
      PageUp(&mm_list_box);
      break;
    case SCROLL_SMALL:
      UpArrow(&mm_list_box);
      break;
    }
    break;
  case SCROLL_DOWN:
    switch (how_far) {
    case SCROLL_LARGE:
      PageDown(&mm_list_box);
      break;
    case SCROLL_SMALL:
      DownArrow(&mm_list_box);
      break;
    }
    break;
  }
  ListBoxScroll(&mm_list_box);
  //PutListBox(&mm_list_box);
  (*mm_list_box.OnChange)();
}
///////////////////////////////////////////

SCREEN_OBJECT_T* mmSo[10];
static void mmWindowDraw(void);
LWINDOW_T mmWindow[1] = {
    35, 0, 640, 445, //window position
    mmSo,            //screen object
    mmWindowDraw,
    10,                      //item count
    0,                       //cursor position 0-79 left to right
    0,                       //element that has current focus
    fcsBtn,                  //focus type
    "Moods Setup & Control", //title
    DefaultOnKeyPress        //OnKeyPress();
};

extern LWINDOW_T progWindow[1];
static UCHAR mlot[] = {"Moods"};

void MoodMenu(void);
static void (*MM_addr)() = MoodMenu;

void MoodMenu(void) {
  int i;
  UCHAR receive_buffer[12];

  memset(mood_sublist, 0, MAX_MOODS * sizeof(MOOD_T*));
  memset(act_mood_list, 0, MAX_MOODS * sizeof(ACTIVE_MOOD));
  memset(receive_buffer, 0, 12);
  active_moods = 0;

  glbWindow = progWindow;
  progWindow[0].title = mlot;
  PutWindow(glbWindow);
  ptext("Counting Active Moods", 205 + 115, 175, BG_WHT + FG_BLK);
  if ((i = GetTable(&receive_buffer,
                    sizeof(receive_buffer),
                    VERSION_TABLE2,
                    version,
                    strlen(version),
                    RETRY_COUNT,
                    TRUE)) != SUCCESS) {
    msgbox("Unable to get active|mood count from ICU.", "Warning", MB_OK);
    //if (i == FAILURE)
    //{
    //   msgbox("FAILURE","",MB_OK);
    //}
  } else {
    memcpy(&active_moods, &receive_buffer[8], sizeof(int));
    if (active_moods > 0) {
      area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
      ptext("Reading Active Moods", 205 + 115, 175, BG_WHT + FG_BLK);
      if (GetTable(act_mood_list,
                   active_moods * sizeof(ACTIVE_MOOD),
                   ACTIVE_MOODS_LIST,
                   NULL,
                   0,
                   RETRY_COUNT,
                   TRUE) != SUCCESS) {
        msgbox("Unable to get active moods from ICU.", "Warning", MB_OK);
      }
    }
  }

  for (i = 0; i < moods_used; i++) {
    mood_sublist[i] = &mood_list[i];
  }
  mm_list_box.item_count = moods_used;
  if (mm_list_box.item_count > 0) {
    mm_list_box.item_selected = 0;
  }
  for (i = 0; i < 8; i++) {
    mmSo[i] = &mmButton[i].so;
  }
  mmSo[8] = &mm_list_box.so;
  mmSo[9] = &sb.so;

  if (moods_used == 0) // Disable some buttons
  {
    mmButton[1].so.visible = FALSE;
    mmButton[2].so.visible = FALSE;
    mmButton[3].so.visible = FALSE;
    mmButton[4].so.visible = FALSE;
    mmButton[6].so.visible = FALSE;
  } else // enable/re-enable same buttons
  {
    mmButton[1].so.visible = TRUE;
    mmButton[2].so.visible = TRUE;
    mmButton[3].so.visible = TRUE;
    mmButton[4].so.visible = TRUE;
    mmButton[6].so.visible = TRUE;
  }

  sb.max = mm_list_box.item_count - 1;
  sb.thumb_pos = mm_list_box.item_selected;
  sb.large_chg = mm_list_box.so.position.height / 14;
  if (sb.large_chg > mm_list_box.item_count) {
    sb.large_chg = mm_list_box.item_count;
  }

  glbWindow = mmWindow;
  PutWindow(glbWindow);
}

static void mmWindowDraw(void) {
  GroupBox("Mood List", 85, 40, 350, 303);
  GroupBox("Mood Setup", 85, 360, 125, 230);
  GroupBox("Mood Control", 220, 360, 125, 230);
}

void DisplayMood(MOOD_T* md, int top, int left, UCHAR color) {
  UCHAR mood_text[60];
  if (IsMoodActive(md) == TRUE) {
    sprintf(mood_text,
            "%s   ACTIVE",
            md->name);
  } else {
    sprintf(mood_text,
            "%s",
            md->name);
  }

  ptext(mood_text, top, left, color);
}

int IsMoodActive(MOOD_T* mood) {
  int i;
  for (i = 0; i < active_moods; i++) {
    if (mood->mood_id == act_mood_list[i].id) {
      return TRUE;
    }
  }
  return FALSE;
}

//#define MD_OFF  0
//#define MD_ON   1
//#define MD_DIM1 2
//#define MD_DIM2 3
//#define MD_DIM3 4
//#define MD_DIM4 5
//#define MD_DIM5 6
//#define MD_DIM6 7
//#define MD_DIM7 8
//#define MD_DIM8 9
//#define MD_DIM9 10
//#define MD_DIM10 11
//#define MD_DIM11 12
//#define MD_DIM12 13
//#define MD_DIM13 14
//#define MD_DIM14 15
//#define MD_DIM15 16

void AddMoodBtnClick(void) {
  MOOD_T mood;
  if (moods_used == MAX_MOODS) {
    msgbox("Moods list is full.", "Information", MB_OK);
    return;
  }
  memset(&mood, 0, sizeof(MOOD_T));
  EditMood(&mood, 'A');
}

void EditMoodBtnClick(void) {
  if (mm_list_box.item_count == 0) {
    msgbox("There are no moods to change.|Add a mood before changing it.",
           "Information",
           MB_OK);
  } else {
    EditMood(&mood_list[mm_list_box.item_selected], 'C');
  }
}

PUSH_BUTTON_T emButton[4] = { // BEGIN SCREEN OBJECT DEFINITION
    245, 50, 150, 25,         // position
    FALSE,                    // focus
    PUSH_BUTTON,              // type
    TRUE,                     // visible
    BtnMouseDown,             // OnMouseDown()
    BtnMouseUp,               // OnMouseUp()
    BtnClick,                 // OnClick()
    BtnKeyPress,              // OnKeyPress()
    PutButton,                // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,                  // default button
    "\x9D Add Device \x9D", // text
    push,                   // type
    up,                     // state
    NULL,                   // icon
    AddDeviceBtnClick,      // function
    135,

    245, 220, 150, 25, // position
    FALSE,             // focus
    PUSH_BUTTON,       // type
    TRUE,              // visible
    BtnMouseDown,      // OnMouseDown()
    BtnMouseUp,        // OnMouseUp()
    BtnClick,          // OnClick()
    BtnKeyPress,       // OnKeyPress()
    PutButton,         // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,                     // default button
    "\x9C Remove Device \x9C", // text
    push,                      // type
    up,                        // state
    NULL,                      // icon
    RmvDeviceBtnClick,         // function
    136,

    // BEGIN SCREEN OBJECT DEFINITION
    419, 445, 89, 33, // position
    FALSE,            // focus
    PUSH_BUTTON,      // type
    TRUE,             // visible
    BtnMouseDown,     // OnMouseDown()
    BtnMouseUp,       // OnMouseUp()
    BtnClick,         // OnClick()
    BtnKeyPress,      // OnKeyPress()
    PutButton,        // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,          //default button
    "Apply",        //text
    push,           //type
    up,             //state
    NULL,           //icon
    emSaveBtnClick, //function
    137,

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
    FALSE,    //default button
    "Cancel", //text
    push,     //type
    up,       //state
    NULL,     //icon
    MoodMenu, //function
    138};

SCROLL_BAR_T emScrollBar[1] = { // BEGIN SCREEN OBJECT DEFINITION
    235, 430, 150, 17,          // position
    FALSE,                      // focus
    SCROLL_BARH,                // type
    FALSE,                      // visible
    ScrollBarOnMouseDown,       // OnMouseDown()
    ScrollBarOnMouseUp,         // OnMouseUp()
    ScrollBarOnClick,           // OnClick()
    NULL,                       // OnKeyPress()
    DisplayScrollBar,           // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    0,
    15,
    1,
    3,
    0,
    SB_HORIZONTAL,
    emsbOnScroll};

static void ddChange(void);
static UCHAR* ddstringlist[3] = {"Off", "On", "Dim"};
DROPDOWN_BOX_T emddblist = {
    //Action drop down box
    180, 465, 80, 17,
    FALSE,             // got_focus
    DROP_DOWN_BOX,     // type
    FALSE,             // visible
    DropDownMouseDown, // OnMouseDown()
    DropDownMouseUp,   // OnMouseUp()
    DropDownClick,     // OnClick()
    DropDownKeyPress,  // OnKeyPress()
    DisplayDropDown,   // DisplayObject()
    ddstringlist,      // strings
    0,                 // item selected
    FALSE,             // dropped
    3,                 // item count
    ddChange           // OnChange
};

TEXT_BOX_T emtxtlist[1] = {
    //mood desc
    0,
    0,
    0,
    14,           // position
    FALSE,        // got_focus
    TEXT_BOX,     // type
    TRUE,         // visible
    TxtMouseDown, // OnMouseDown()
    TxtMouseUp,   // OnMouseUp()
    TxtClick,     // OnClick()
    TxtKeyPress,  // OnKeyPress()
    PutTxtBox,    // DisplayObject()
    //end of screen object definition
    8,        // row
    54,       // column
    20,       // box length
    tb_alpha, // type
    "",       // text
    NULL,
};

// mood entry list
static MOOD_ENTRY* em_sublist[32];
static DEVICE* dvc_sl[MAX_DEVICES];

LISTBOX_T em_list_box[2] = { // BEGIN SCREEN OBJECT DEFINITION
    80, 30, 340, 145,        // position
    FALSE,                   // focus
    LIST_BOX,                // type
    TRUE,                    // visible
    ListBoxMouseDown,        // OnMouseDown()
    ListBoxMouseUp,          // OnMouseUp()
    ListBoxClick,            // OnClick()
    ListBoxKeyPress,         // OnKeyPress()
    PutListBox,              // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    0,                 // item count
    0,                 // item selected
    0,                 // top_item
    0,                 // display_count
    em_sublist,        // sub_list
    DisplayMoodEntry,  // display function pointer
    em_list_boxChange, // on change event

    295, 30, 340, 145, // position
    FALSE,             // focus
    LIST_BOX,          // type
    TRUE,              // visible
    ListBoxMouseDown,  // OnMouseDown()
    ListBoxMouseUp,    // OnMouseUp()
    ListBoxClick,      // OnClick()
    ListBoxKeyPress,   // OnKeyPress()
    PutListBox,        // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    0,                  // item count
    0,                  // item selected
    0,                  // top_item
    0,                  // display_count
    dvc_sl,             // sub_list
    DisplayDeviceShort, // display function pointer
    em_list_boxChange};

///////////////////////////////////////////
static void sbOnClick0(int direction, int how_far, int thumb_pos);
static void sbOnClick1(int direction, int how_far, int thumb_pos);

static SCROLL_BAR_T sba[2] =
    {
        // BEGIN SCREEN OBJECT DEFINITION
        80, 371, 17, 145,     // position
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
        sbOnClick0,  // on scroll function
        0,           // attached object

        295, 371, 17, 145,    // position
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
        sbOnClick1,  // on scroll function
        0            // attached object
};

static void sbOnClick0(int direction, int how_far, int thumb_pos) {
  switch (direction) {
  case SCROLL_UP:
    switch (how_far) {
    case SCROLL_LARGE:
      PageUp(&em_list_box[0]);
      break;
    case SCROLL_SMALL:
      UpArrow(&em_list_box[0]);
      break;
    }
    break;
  case SCROLL_DOWN:
    switch (how_far) {
    case SCROLL_LARGE:
      PageDown(&em_list_box[0]);
      break;
    case SCROLL_SMALL:
      DownArrow(&em_list_box[0]);
      break;
    }
    break;
  }
  ListBoxScroll(&em_list_box[0]);
  //PutListBox(&em_list_box[0]);
  (*em_list_box[0].OnChange)();
  //em_list_boxChange();
}

static void sbOnClick1(int direction, int how_far, int thumb_pos) {
  switch (direction) {
  case SCROLL_UP:
    switch (how_far) {
    case SCROLL_LARGE:
      PageUp(&em_list_box[1]);
      break;
    case SCROLL_SMALL:
      UpArrow(&em_list_box[1]);
      break;
    }
    break;
  case SCROLL_DOWN:
    switch (how_far) {
    case SCROLL_LARGE:
      PageDown(&em_list_box[1]);
      break;
    case SCROLL_SMALL:
      DownArrow(&em_list_box[1]);
      break;
    }
    break;
  }
  ListBoxScroll(&em_list_box[1]);
  //PutListBox(&em_list_box[1]);
  (*em_list_box[1].OnChange)();
}
///////////////////////////////////////////

SCREEN_OBJECT_T* emSo[11];
static UCHAR emtitle[15];
static void emWindowDraw(void);
LWINDOW_T emWindow[1] = {
    35, 0, 640, 445, //window position
    emSo,            //screen object
    emWindowDraw,
    11,               //item count
    2,                //cursor position 0-79 left to right
    0,                //element that has current focus
    fcsBtn,           //focus type
    emtitle,          // title
    DefaultOnKeyPress //OnKeyPress();
};

void EditMood(MOOD_T* mood, int opt) {
  int i;
  int j;
  int dev_ndx;

  glbOpt = opt;
  this_mood = *mood;

  if (opt == 'A') {
    strcpy(emtitle, "Add Mood");
  } else {
    strcpy(emtitle, "Change Mood");
  }

  memset(em_sublist, 0, 32 * sizeof(MOOD_ENTRY*));
  for (i = 0; i < this_mood.items; i++) {
    em_sublist[i] = &this_mood.entry[i];
  }
  em_list_box[0].item_count = this_mood.items;
  if (em_list_box[0].item_count > 0) {
    em_list_box[0].item_selected = 0;
  }
  memset(dvc_sl, 0, MAX_DEVICES * sizeof(DEVICE*));
  for (i = 0, j = 0; i < dev_list.devices_used; i++) {
    if ((IsSubmeter(&dev_list.devices[i]) == FALSE) &&
        (IsDeviceInMood(&this_mood, dev_list.devices[i].device_slot) == FALSE)) {
      dvc_sl[j++] = &dev_list.devices[i];
    }
  }
  em_list_box[1].item_count = j;
  if (em_list_box[1].item_count > 0) {
    em_list_box[1].item_selected = 0;
  }

  emSo[0] = &emButton[0].so;
  emSo[1] = &emButton[1].so;
  emSo[2] = &em_list_box[0].so;
  emSo[3] = &em_list_box[1].so;
  emSo[4] = &emButton[2].so;
  emSo[5] = &emButton[3].so;
  emSo[6] = &emtxtlist[0].so;
  emSo[7] = &emddblist.so;
  emSo[8] = &emScrollBar[0].so;
  emSo[9] = &sba[0].so;
  emSo[10] = &sba[1].so;

  emddblist.so.visible = FALSE;
  emScrollBar[0].so.visible = FALSE;

  memset(emtxtlist[0].text, 0, MAX_TEXT);
  memcpy(emtxtlist[0].text, this_mood.name, 20);
  trim(emtxtlist[0].text);

  if (em_list_box[0].item_count > 0) {
    dev_ndx = LookupDev(em_sublist[em_list_box[0].item_selected]->device_slot);
    if (IsTstat(&dev_list.devices[dev_ndx])) //.x10_device_type == X10_RCS_TSTAT)
    {
      emddblist.strings = tstat_strings;
      emddblist.count = 5;
    } else {
      emddblist.strings = ddstringlist;
      if (IsDimmer(&dev_list.devices[dev_ndx])) {
        emddblist.count = 3;
      } else {
        emddblist.count = 2;
      }
    }
  } else {
    emddblist.selected = 0;
    emddblist.strings = ddstringlist;
    emddblist.count = 3;
  }

  glbWindow = emWindow;
  PutWindow(glbWindow);
}

static void emWindowDraw(void) {
  sba[0].max = em_list_box[0].item_count - 1;
  sba[0].thumb_pos = em_list_box[0].item_selected;
  sba[0].large_chg = em_list_box[0].so.position.height / 14;
  if (sba[0].large_chg > em_list_box[0].item_count) {
    sba[0].large_chg = em_list_box[0].item_count;
  }

  sba[1].max = em_list_box[1].item_count - 1;
  sba[1].thumb_pos = em_list_box[1].item_selected;
  sba[1].large_chg = em_list_box[1].so.position.height / 14;
  if (sba[1].large_chg > em_list_box[1].item_count) {
    sba[1].large_chg = em_list_box[1].item_count;
  }

  //GroupBox(UCHAR *text, int top, int left, int height, int width)
  GroupBox("Devices in this Mood", 65, 20, 170, 380);
  GroupBox("Available Devices", 280, 20, 170, 380);
  GroupBox("Mood Description", 65, 410, 70, 190);
  if (em_list_box[0].item_count > 0) {
    DisplayAction();
  } else {
    emddblist.selected = 0;
    emddblist.strings = ddstringlist;
    emddblist.count = 3;
    DisplayDropDown(&emddblist);
  }
}

void DisplayAction(void) {
  int dev_ndx;
  int rc;

  GroupBox("Action", 150, 410, 140, 190);
  emddblist.so.visible = TRUE;

  dev_ndx = LookupDev(em_sublist[em_list_box[0].item_selected]->device_slot);
  rc = IsTstat(&dev_list.devices[dev_ndx]);
  if (rc == X10_RCS_TSTAT) {
    switch (em_sublist[em_list_box[0].item_selected]->action) {
    case TSTAT_OFF:
    case TSTAT_HEAT:
    case TSTAT_COOL:
    case TSTAT_AUTO:
      emddblist.selected = em_sublist[em_list_box[0].item_selected]->action;
      emScrollBar[0].so.visible = FALSE;
      area_clear(200,
                 270,
                 430,
                 590,
                 FG_WHT);
      break;
    case TSTAT_40:
    case TSTAT_60:
    case TSTAT_62:
    case TSTAT_63:
    case TSTAT_64:
    case TSTAT_65:
    case TSTAT_66:
    case TSTAT_67:
    case TSTAT_68:
    case TSTAT_69:
    case TSTAT_70:
    case TSTAT_71:
    case TSTAT_72:
    case TSTAT_73:
    case TSTAT_74:
    case TSTAT_75:
    case TSTAT_76:
    case TSTAT_77:
    case TSTAT_78:
    case TSTAT_79:
    case TSTAT_80:
    case TSTAT_81:
    case TSTAT_82:
    case TSTAT_83:
    case TSTAT_84:
    case TSTAT_86:
    case TSTAT_88:
    case TSTAT_90:
      emddblist.selected = 4;
      DisplayBrightness(em_sublist[em_list_box[0].item_selected]->action);
      break;
    }
  } else if (rc == X10_RCS_CENTI_TSTAT) {
    switch (em_sublist[em_list_box[0].item_selected]->action) {
    case TSTAT_OFF:
    case TSTAT_HEAT:
    case TSTAT_COOL:
    case TSTAT_AUTO:
      emddblist.selected = em_sublist[em_list_box[0].item_selected]->action;
      emScrollBar[0].so.visible = FALSE;
      area_clear(200,
                 270,
                 430,
                 590,
                 FG_WHT);
      break;
    case TSTAT_05:
    case TSTAT_06:
    case TSTAT_07:
    case TSTAT_08:
    case TSTAT_09:
    case TSTAT_10:
    case TSTAT_11:
    case TSTAT_12:
    case TSTAT_13:
    case TSTAT_14:
    case TSTAT_15:
    case TSTAT_16:
    case TSTAT_17:
    case TSTAT_18:
    case TSTAT_19:
    case TSTAT_20:
    case TSTAT_21:
    case TSTAT_22:
    case TSTAT_23:
    case TSTAT_24:
    case TSTAT_25:
    case TSTAT_26:
    case TSTAT_27:
    case TSTAT_28:
    case TSTAT_29:
    case TSTAT_30:
    case TSTAT_31:
    case TSTAT_32:
      emddblist.selected = 4;
      DisplayBrightness(em_sublist[em_list_box[0].item_selected]->action);
      break;
    }
  } else {
    switch (em_sublist[em_list_box[0].item_selected]->action) {
    case MD_OFF:
    case MD_ON:
      emddblist.selected = em_sublist[em_list_box[0].item_selected]->action;
      emScrollBar[0].so.visible = FALSE;
      area_clear(200,
                 270,
                 430,
                 590,
                 FG_WHT);
      break;
    case MD_DIM0:
    case MD_DIM1:
    case MD_DIM2:
    case MD_DIM3:
    case MD_DIM4:
    case MD_DIM5:
    case MD_DIM6:
    case MD_DIM7:
    case MD_DIM8:
    case MD_DIM9:
    case MD_DIM10:
    case MD_DIM11:
    case MD_DIM12:
    case MD_DIM13:
    case MD_DIM14:
    case MD_DIM15:
      emddblist.selected = 2;
      DisplayBrightness(em_sublist[em_list_box[0].item_selected]->action);
      break;
    default:
      emddblist.selected = MD_OFF; //no action
      break;
    }
  }

  DisplayDropDown(&emddblist);
}

void emSaveBtnClick(void) {
  if (strlen(emtxtlist[0].text) == 0) {
    msgbox("Mood description field is empty.|Give the mood a description|before saving the mood.",
           "Empty Field", MB_OK);
    return;
  }
  if (em_list_box[0].item_count == 0) {
    msgbox("This mood is empty.|Add at least one device|before saving the mood.",
           "Empty Mood", MB_OK);
    return;
  }

  FitText(this_mood.name, emtxtlist[0].text, 20);

  if (SendObject(&this_mood, glbOpt, sizeof(MOOD_T), EXT_MOOD_TABLE) == SUCCESS) {
    if (glbOpt == 'A') {
      mood_list[moods_used] = this_mood;
      moods_used++;
    } else {
      *mood_sublist[mm_list_box.item_selected] = this_mood;
    }
    if (user_backup() == SUCCESS) {
      Status("Mood successfully added.");
    } else {
      msgbox("Unable to add mood.|Check disk space.", "Warning", MB_OK);
      return;
    }
  } else {
    msgbox("Unable to add mood.|Check ICU.", "Warning", MB_OK);
    return;
  }
  MoodMenu();
}

void ddChange(void) {
  int dev_ndx;
  int rc;

  dev_ndx = LookupDev(em_sublist[em_list_box[0].item_selected]->device_slot);
  rc = IsTstat(&dev_list.devices[dev_ndx]);
  if (rc == X10_RCS_TSTAT) {
    switch (emddblist.selected) {
    case 0:
    case 1:
    case 2:
    case 3:
      em_sublist[em_list_box[0].item_selected]->action = emddblist.selected;
      emScrollBar[0].so.visible = FALSE;
      area_clear(200,
                 270,
                 430,
                 590,
                 FG_WHT);
      break;
    case 4: //deg F
      em_sublist[em_list_box[0].item_selected]->action = TSTAT_70;
      DisplayBrightness(em_sublist[em_list_box[0].item_selected]->action);
      break;
    }
  } else if (rc == X10_RCS_CENTI_TSTAT) {
    switch (emddblist.selected) {
    case 0:
    case 1:
    case 2:
    case 3:
      em_sublist[em_list_box[0].item_selected]->action = emddblist.selected;
      emScrollBar[0].so.visible = FALSE;
      area_clear(200,
                 270,
                 430,
                 590,
                 FG_WHT);
      break;
    case 4: //deg C
      em_sublist[em_list_box[0].item_selected]->action = TSTAT_21;
      DisplayBrightness(em_sublist[em_list_box[0].item_selected]->action);
      break;
    }
  } else {
    switch (emddblist.selected) {
    case 0: //off
    case 1: //on
      em_sublist[em_list_box[0].item_selected]->action = emddblist.selected;
      emScrollBar[0].so.visible = FALSE;
      area_clear(200,
                 270,
                 430,
                 590,
                 FG_WHT);
      break;
    case 2: //DIM
      if (IsDimmer(&dev_list.devices[dev_ndx]) == TRUE) {
        em_sublist[em_list_box[0].item_selected]->action = MD_DIM0;
        DisplayBrightness(em_sublist[em_list_box[0].item_selected]->action);
      } else {
        Status("This device is not a dimmer.");
        em_sublist[em_list_box[0].item_selected]->action = MD_OFF;
        emddblist.selected = MD_OFF;
        DisplayDropDown(&emddblist);
      }
      break;
    }
  }
  PutListBox(&em_list_box[0]);
}

void AddDeviceBtnClick(void) {
  int i, j;

  if (this_mood.items == MAX_MOODS) {
    msgbox("This mood is full.", "Information", MB_OK);
    return;
  }
  if (em_list_box[1].item_count == 0) // no more available devices
  {
    return;
  }

  //   if (dvc_sl[em_list_box[1].item_selected]->x10_device_type == 3) //tstat
  if (IsTstat(dvc_sl[em_list_box[1].item_selected]) != 0) // Is a thermostat.
  {
    this_mood.entry[this_mood.items].device_slot = dvc_sl[em_list_box[1].item_selected]->device_slot;
    this_mood.entry[this_mood.items].action = MD_OFF;
    this_mood.items++;
    this_mood.entry[this_mood.items].device_slot = dvc_sl[em_list_box[1].item_selected]->device_slot;
    this_mood.entry[this_mood.items].action = MD_OFF;
    this_mood.items++;
  } else {
    this_mood.entry[this_mood.items].device_slot = dvc_sl[em_list_box[1].item_selected]->device_slot;
    this_mood.entry[this_mood.items].action = MD_OFF;
    this_mood.items++;
  }

  memset(em_sublist, 0, 32 * sizeof(MOOD_ENTRY*));
  for (i = 0; i < this_mood.items; i++) {
    em_sublist[i] = &this_mood.entry[i];
  }
  em_list_box[0].item_count = this_mood.items;

  //This is handled by the list box component
  em_list_box[0].item_selected = this_mood.items - 1;

  //build sub list for bottom listbox
  memset(dvc_sl, 0, MAX_DEVICES * sizeof(DEVICE*));
  for (i = 0, j = 0; i < dev_list.devices_used; i++) {
    if ((IsSubmeter(&dev_list.devices[i]) == FALSE) &&
        (IsDeviceInMood(&this_mood, dev_list.devices[i].device_slot) == FALSE)) {
      dvc_sl[j++] = &dev_list.devices[i];
    }
  }
  em_list_box[1].item_count = j;

  //This is handled by the list box component
  if (em_list_box[1].item_selected > j - 1) {
    em_list_box[1].item_selected = j - 1;
  }

  PutListBox(&em_list_box[0]);
  PutListBox(&em_list_box[1]);

  em_list_boxChange();
}

void RmvDeviceBtnClick(void) {
  int i;
  int j;

  if (em_list_box[0].item_count == 0) {
    emddblist.so.visible = FALSE;
    area_clear(145, 295, 405, 605, FG_WHT);
    return;
  }

  //clean up the mood entry list...
  for (i = em_list_box[0].item_selected; i < this_mood.items; i++) {
    this_mood.entry[i] = this_mood.entry[i + 1];
  }
  this_mood.items--;
  memset(&this_mood.entry[this_mood.items], 0, sizeof(MOOD_ENTRY));
  memset(em_sublist, 0, 32 * sizeof(MOOD_ENTRY*));

  //add all mood entries to the listbox's sublist
  em_list_box[0].item_count = this_mood.items;
  for (i = 0; i < this_mood.items; i++) {
    em_sublist[i] = &this_mood.entry[i];
  }

  //This is handled by the list box component
  if (em_list_box[0].item_selected > (this_mood.items - 1)) {
    em_list_box[0].item_selected = this_mood.items - 1;
  }

  //clean up the listbox containing available devices
  memset(dvc_sl, 0, MAX_DEVICES * sizeof(DEVICE*));
  for (i = 0, j = 0; i < dev_list.devices_used; i++) {
    if ((IsSubmeter(&dev_list.devices[i]) == FALSE) &&
        (IsDeviceInMood(&this_mood, dev_list.devices[i].device_slot) == FALSE)) {
      dvc_sl[j++] = &dev_list.devices[i];
    }
  }
  em_list_box[1].item_count = j;

  //This is handled by the list box component
  if (em_list_box[1].item_selected > j - 1) {
    em_list_box[1].item_selected = j - 1;
  }

  PutListBox(&em_list_box[0]);
  PutListBox(&em_list_box[1]);

  em_list_boxChange();
  if (em_list_box[0].item_count == 0) {
    emddblist.so.visible = FALSE;
    area_clear(145, 295, 405, 605, FG_WHT);
  }
}

void DisplayBrightness(UCHAR dim_level) {
  char work[20];
  int dev_ndx;
  int i;
  int rc;

  emScrollBar[0].so.visible = TRUE;

  dev_ndx = LookupDev(em_sublist[em_list_box[0].item_selected]->device_slot);
  rc = IsTstat(&dev_list.devices[dev_ndx]);
  if (rc == X10_RCS_TSTAT) {
    sprintf(work, "%2d%cF", dim_level, 0x0A4);
    ptext(work, 215, 430, BG_WHT + FG_BLK);
    emScrollBar[0].min = 0;
    emScrollBar[0].max = 27;
    emScrollBar[0].small_chg = 1;
    emScrollBar[0].large_chg = 7;
    for (i = 4; i < 32; i++) {
      if (tstat_table[i] == dim_level) {
        dim_level = i;
        break;
      }
    }
    emScrollBar[0].thumb_pos = dim_level - 4;
    ptext("Cooler       Warmer", 255, 431, BG_WHT + FG_BLK);
  } else if (rc == X10_RCS_CENTI_TSTAT) {
    sprintf(work, "%2d%cC", dim_level, 0x0A4);
    ptext(work, 215, 430, BG_WHT + FG_BLK);
    emScrollBar[0].min = 0;
    emScrollBar[0].max = 27;
    emScrollBar[0].small_chg = 1;
    emScrollBar[0].large_chg = 7;
    for (i = 4; i < 32; i++) {
      if (centi_tstat_table[i] == dim_level) {
        dim_level = i;
        break;
      }
    }
    emScrollBar[0].thumb_pos = dim_level - 4;
    ptext("Cooler       Warmer", 255, 431, BG_WHT + FG_BLK);
  } else {
    emScrollBar[0].min = 0;
    emScrollBar[0].max = 15;
    emScrollBar[0].small_chg = 1;
    emScrollBar[0].large_chg = 5;
    emScrollBar[0].thumb_pos = dim_level >> 4;
    ptext("Bright          Dim", 255, 431, BG_WHT + FG_BLK);
  }
  DisplayScrollBar(&emScrollBar[0]);
}

void em_list_boxChange(void) {
  int dev_ndx;

  if (em_list_box[0].item_count == 0) {
    return;
  }

  dev_ndx = LookupDev(em_sublist[em_list_box[0].item_selected]->device_slot);
  if (IsTstat(&dev_list.devices[dev_ndx]) != FALSE) {
    emddblist.strings = tstat_strings;
    emddblist.count = 5;
  } else {
    emddblist.strings = ddstringlist;
    if (IsDimmer(&dev_list.devices[dev_ndx])) {
      emddblist.count = 3;
    } else {
      emddblist.count = 2;
    }
  }

  sba[0].max = em_list_box[0].item_count - 1;
  sba[0].thumb_pos = em_list_box[0].item_selected;
  sba[0].large_chg = em_list_box[0].so.position.height / 14;
  if (sba[0].large_chg > em_list_box[0].item_count) {
    sba[0].large_chg = em_list_box[0].item_count;
  }
  sba[1].max = em_list_box[1].item_count - 1;
  sba[1].thumb_pos = em_list_box[1].item_selected;
  sba[1].large_chg = em_list_box[1].so.position.height / 14;
  if (sba[1].large_chg > em_list_box[1].item_count) {
    sba[1].large_chg = em_list_box[1].item_count;
  }
  DisplayScrollBar(&sba[1]);
  DisplayScrollBar(&sba[0]);
  DisplayAction();
}

static void emsbOnScroll(int direction, int how_far, int thumb_pos) {
  int dev_ndx;
  int rc;

  dev_ndx = LookupDev(em_sublist[em_list_box[0].item_selected]->device_slot);
  rc = IsTstat(&dev_list.devices[dev_ndx]);
  if (rc == X10_RCS_TSTAT) {
    em_sublist[em_list_box[0].item_selected]->action = tstat_table[emScrollBar[0].thumb_pos + 4];
  } else if (rc == X10_RCS_CENTI_TSTAT) {
    em_sublist[em_list_box[0].item_selected]->action = centi_tstat_table[emScrollBar[0].thumb_pos + 4];
  } else {
    em_sublist[em_list_box[0].item_selected]->action = (emScrollBar[0].thumb_pos << 4) + 0x02;
  }
  PutListBox(&em_list_box[0]);
  (*em_list_box[0].OnChange)();
}

int IsDeviceInMood(MOOD_T* mood, USHORT device_id) {
  int i;

  for (i = 0; i < mood->items; ++i) {
    if (mood->entry[i].device_slot == device_id) {
      return TRUE;
    }
  }
  return FALSE;
}

void AdjustMoods(USHORT which_id) {
  int i, j, k;

  for (i = 0; i < moods_used; ++i) {
    for (j = 0; j < mood_list[i].items; ++j) {
      if (mood_list[i].entry[j].device_slot == which_id) {
        for (k = j; k < mood_list[i].items; ++k) {
          mood_list[i].entry[k] = mood_list[i].entry[k + 1];
        }
        --mood_list[i].items;
        mood_list[i].entry[mood_list[i].items].device_slot = 0;
        mood_list[i].entry[mood_list[i].items].action = 0;
        //if (mood_list[i].items == 0)
        //{
        //   RemoveMood(mood_sublist[mm_list_box.item_selected]);
        //}
      }
    }
  }
  return;
}

void ActMoodBtnClick(void) {
  int prot;
  if (mm_list_box.item_count == 0) {
    Status("Add some moods before activating them.");
    return;
  }
  if (msgbox("Protect this mood?", "Start Mood", MB_YES_NO) == MB_RESULT_YES) {
    prot = MD_PROT;
  } else {
    prot = MD_UNPROT;
  }
  if (SetMoodState(mood_sublist[mm_list_box.item_selected]->mood_id, MD_ON, prot, 0) == SUCCESS) {
    Status("Mood successfully activated.");
    if ((IsMoodActive(mood_sublist[mm_list_box.item_selected]) == FALSE) &&
        (prot == MD_PROT)) {
      act_mood_list[active_moods].id = mood_sublist[mm_list_box.item_selected]->mood_id;
      act_mood_list[active_moods].duration = 0;
      act_mood_list[active_moods].start_time = 0;
      active_moods++;
    }
    ListBoxScroll(&mm_list_box);
    //PutListBox(&mm_list_box);
  } else {
    msgbox("Mood was NOT activated.|Try again.", "Information", MB_OK);
  }
}

void EndAllMoodBtnClick(void) {
  if (SetMoodState(0xFFFF, MD_OFF, MD_PROT, 0) == SUCCESS) {
    Status("All moods successfully ended.");
    active_moods = 0;
    ListBoxScroll(&mm_list_box);
    //PutListBox(&mm_list_box);
  } else {
    msgbox("All moods were NOT ended.|Please try again.", "Information", MB_OK);
  }
}

int LookupActMood(USHORT mood_id) {
  int i;

  for (i = 0; i < moods_used; ++i) {
    if (act_mood_list[i].id == mood_id) {
      return i;
    }
  }
  return -1;
}

void DeActMoodBtnClick(void) {
  int mood_id, i;
  if (IsMoodActive(mood_sublist[mm_list_box.item_selected]) == TRUE) {
    if (SetMoodState(mood_sublist[mm_list_box.item_selected]->mood_id, MD_OFF, MD_PROT, 0) == SUCCESS) {
      Status("Selected mood successfully ended.");
      if (IsMoodActive(mood_sublist[mm_list_box.item_selected]) == TRUE) {
        mood_id = LookupActMood(mood_sublist[mm_list_box.item_selected]->mood_id);
        for (i = mood_id; i < active_moods; i++) {
          act_mood_list[i] = act_mood_list[i + 1];
        }
        active_moods--;
        memset(&act_mood_list[active_moods], 0, sizeof(ACTIVE_MOOD));
      }
      ListBoxScroll(&mm_list_box);
      //PutListBox(&mm_list_box);
    } else {
      msgbox("Unable to end selected mood.|Please try again.", "Information", MB_OK);
    }
  } else {
    msgbox("This mood is not active.|Moods must be protected from|schedule to be deactivated.", "Information", MB_OK);
  }
}

void RmvBtnClick(void) {
  int result;
  int i;
  int removed = TRUE;

  if (mm_list_box.item_count > 0) {
    if (confirmation == TRUE) {
      result = msgbox("Removing this mood will also|remove associated triggers.|Are you sure?",
                      "Caution",
                      MB_YES_NO);
    } else {
      result = MB_RESULT_YES;
    }
    if (result == MB_RESULT_YES) {
      Status("Removing associated triggers.");
      while (removed == TRUE) {
        removed = FALSE;
        for (i = 0; (USHORT)i < triggers_used; i++) {
          if (
              (
                  (triggers[i].trigger_action == 1) ||
                  (triggers[i].trigger_action == 2)) &&
              (triggers[i].mood_id == mood_sublist[mm_list_box.item_selected]->mood_id)) {
            RemoveTrigger(&triggers[i]);
            removed = TRUE;
            break;
          }
        }
      }
      if (RemoveMood(mood_sublist[mm_list_box.item_selected]) == SUCCESS) {
        Status("Mood successfully removed.");
        --mm_list_box.item_count;
        if (mm_list_box.item_selected > mm_list_box.item_count - 1) {
          mm_list_box.item_selected = mm_list_box.item_count - 1;
        }
        list_boxOnChange();
        PutListBox(&mm_list_box);
      } else {
        msgbox("Unable to remove mood.|Please try again.", "Information", MB_OK);
      }
    }
  }
  MoodMenu();
}

int LookupMoodIndex(USHORT mood_id) {
  int i;

  for (i = 0; i < moods_used; ++i) {
    if (mood_list[i].mood_id == mood_id) {
      return i;
    }
  }
  return -1;
}

void DisplayMoodEntry(MOOD_ENTRY* me, int top, int left, UCHAR color) {
  int i;
  int rc;
  UCHAR mood_text[100];
  UCHAR work[20];
  UCHAR deg[20];

  if ((i = LookupDev(me->device_slot)) != -1) {
    sprintf(mood_text,
            "%.13s %.18s ",
            dev_list.devices[i].desc,
            dev_list.devices[i].location);

    rc = IsTstat(&dev_list.devices[i]);
    if (rc == X10_RCS_TSTAT) {
      switch (me->action) {
      default:
      case TSTAT_OFF:
        strcat(mood_text, "Off");
        break;
      case TSTAT_HEAT:
        strcat(mood_text, "Heat");
        break;
      case TSTAT_COOL:
        strcat(mood_text, "Cool");
        break;
      case TSTAT_AUTO:
        strcat(mood_text, "Auto");
        break;
      case TSTAT_40:
      case TSTAT_60:
      case TSTAT_62:
      case TSTAT_63:
      case TSTAT_64:
      case TSTAT_65:
      case TSTAT_66:
      case TSTAT_67:
      case TSTAT_68:
      case TSTAT_69:
      case TSTAT_70:
      case TSTAT_71:
      case TSTAT_72:
      case TSTAT_73:
      case TSTAT_74:
      case TSTAT_75:
      case TSTAT_76:
      case TSTAT_77:
      case TSTAT_78:
      case TSTAT_79:
      case TSTAT_80:
      case TSTAT_81:
      case TSTAT_82:
      case TSTAT_83:
      case TSTAT_84:
      case TSTAT_86:
      case TSTAT_88:
      case TSTAT_90:
        sprintf(deg, "%2d%cF", me->action, 0xA4);
        strcat(mood_text, deg);
        break;
      }
    } else if (rc == X10_RCS_CENTI_TSTAT) {
      switch (me->action) {
      default:
      case TSTAT_OFF:
        strcat(mood_text, "Off");
        break;
      case TSTAT_HEAT:
        strcat(mood_text, "Heat");
        break;
      case TSTAT_COOL:
        strcat(mood_text, "Cool");
        break;
      case TSTAT_AUTO:
        strcat(mood_text, "Auto");
        break;
      case TSTAT_05:
      case TSTAT_06:
      case TSTAT_07:
      case TSTAT_08:
      case TSTAT_09:
      case TSTAT_10:
      case TSTAT_11:
      case TSTAT_12:
      case TSTAT_13:
      case TSTAT_14:
      case TSTAT_15:
      case TSTAT_16:
      case TSTAT_17:
      case TSTAT_18:
      case TSTAT_19:
      case TSTAT_20:
      case TSTAT_21:
      case TSTAT_22:
      case TSTAT_23:
      case TSTAT_24:
      case TSTAT_25:
      case TSTAT_26:
      case TSTAT_27:
      case TSTAT_28:
      case TSTAT_29:
      case TSTAT_30:
      case TSTAT_31:
      case TSTAT_32:
        sprintf(deg, "%2d%cF", me->action, 0xA4);
        strcat(mood_text, deg);
        break;
      }
    } else {
      switch (me->action) {
      case MD_OFF:
        strcat(mood_text, "Off");
        break;
      case MD_ON:
        strcat(mood_text, "On");
        break;
      case MD_DIM0:
      case MD_DIM1:
      case MD_DIM2:
      case MD_DIM3:
      case MD_DIM4:
      case MD_DIM5:
      case MD_DIM6:
      case MD_DIM7:
      case MD_DIM8:
      case MD_DIM9:
      case MD_DIM10:
      case MD_DIM11:
      case MD_DIM12:
      case MD_DIM13:
      case MD_DIM14:
      case MD_DIM15:
        sprintf(work, "%.0f%% Bright", (double)((16.0 - (me->action / 16)) * 6.25));
        //               sprintf(work, "Dim %2d", me->action/16);
        strcat(mood_text, work);
        break;
      default:
        strcat(mood_text, "UNK");
        break;
      }
    }
    ptext(mood_text, top, left, color);
  } else {
    msgbox("Invalid device in this mood.|Delete this mood and reenter it.", "Information", MB_OK);
    ptext("INVALID DEVICE", top, left, BG_TRANSPARENT + FG_RED);
  }
}

static void list_boxOnChange(void) {
  sb.max = mm_list_box.item_count - 1;
  sb.thumb_pos = mm_list_box.item_selected;
  sb.large_chg = mm_list_box.so.position.height / 14;
  if (sb.large_chg > mm_list_box.item_count) {
    sb.large_chg = mm_list_box.item_count;
  }
  DisplayScrollBar(&sb);
}

static void TriggerSetupClick(void) {
  //   Sender = MoodMenu;
  Sender = MM_addr;
  TriggerMenu();
}

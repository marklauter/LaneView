//==========================================================================
// HISTORY:
// Author      Date      Description
// Mark Lauter   22 DEC 96   Contains all functions
//                     relating to SMART SCHEDULES
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
#include "dropdown.h"
#include "listbox.h"
#include "mouse.h"
#include "lwindow.h"
#include "ismenu.h"
#include "frame_3d.h"
#include "devcat.h"
#include "ctrlschd.h"
#include "device.h"
#include "dvc_evnt.h"
#include "msgbox.h"
#include "checkbox.h"
#include "insubs.h"
#include "inio.h"
#include "inutil.h"
#include "event.h"
#include "scrolbar.h"
#include "moodmenu.h"
#include "spinner.h"

static DEVICE Sthis_device;
extern DEVICE* dev_subliste[MAX_DEVICES + 1];
extern LISTBOX_T dev_listbox;
extern DEVICE Ethis_device;
extern void EventAddBtnClick(void);
static void CbHourClick(void);
static void SmartCancelBtnClick(void);
static void SmartSaveBtnClick(void);
static void ScheduleTypeDropdownChange(void);
static void CbORYesClick(void);
static void SmartTimeTextChange(void);
static void SmartCashTextChange(void);
static void SelAll(void);
static void ClearAll(void);
static void CbVacationClick(void);
void SmartSchedule(DEVICE* device, int cat);
extern EVENT this_event;
extern int glb_cat_index;
extern int event_opt;
extern void EditEvent(EVENT* event, UCHAR opt, int sender);
static CHECK_BOX_T cb_hour[24] =
    {
        115,
        310,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "12mn - 1am",
        TRUE,
        CbHourClick,

        135,
        310,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        " 1 - 2am",
        TRUE,
        CbHourClick,

        155,
        310,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        " 2 - 3am",
        TRUE,
        CbHourClick,

        175,
        310,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        " 3 - 4am",
        TRUE,
        CbHourClick,

        195,
        310,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        " 4 - 5am",
        TRUE,
        CbHourClick,

        215,
        310,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        " 5 - 6am",
        TRUE,
        CbHourClick,

        235,
        310,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        " 6 - 7am",
        TRUE,
        CbHourClick,

        255,
        310,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        " 7 - 8am",
        TRUE,
        CbHourClick,

        275,
        310,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        " 8 - 9am",
        TRUE,
        CbHourClick,

        295,
        310,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        " 9 - 10am",
        TRUE,
        CbHourClick,

        315,
        310,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "10 - 11am",
        TRUE,
        CbHourClick,

        335,
        310,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "11am - 12noon",
        TRUE,
        CbHourClick,

        115,
        455,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "12noon - 1pm",
        TRUE,
        CbHourClick,

        135,
        455,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        " 1 - 2pm",
        TRUE,
        CbHourClick,

        155,
        455,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        " 2 - 3pm",
        TRUE,
        CbHourClick,

        175,
        455,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        " 3 - 4pm",
        TRUE,
        CbHourClick,

        195,
        455,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        " 4 - 5pm",
        TRUE,
        CbHourClick,

        215,
        455,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        " 5 - 6pm",
        TRUE,
        CbHourClick,

        235,
        455,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        " 6 - 7pm",
        TRUE,
        CbHourClick,

        255,
        455,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        " 7 - 8pm",
        TRUE,
        CbHourClick,

        275,
        455,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        " 8 - 9pm",
        TRUE,
        CbHourClick,

        295,
        455,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        " 9 - 10pm",
        TRUE,
        CbHourClick,

        315,
        455,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "10 - 11pm",
        TRUE,
        CbHourClick,

        335,
        455,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "11pm - 12mn",
        TRUE,
        CbHourClick,
};

static CHECK_BOX_T cb_override =
    {
        350,
        112,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        FALSE,               // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Run for duration",
        FALSE,
        CbORYesClick,
};

static CHECK_BOX_T cb_vacation =
    {
        300,
        112,
        0,
        0,                   // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Run in vacation.",
        TRUE,
        CbVacationClick,
};

static PUSH_BUTTON_T smartbtn[4] = {
    // BEGIN SCREEN OBJECT DEFINITION
    365, 345, 90, 22, // position
    FALSE,            // focus
    PUSH_BUTTON,      // type
    TRUE,             // visible
    BtnMouseDown,     // OnMouseDown()
    BtnMouseUp,       // OnMouseUp()
    BtnClick,         // OnClick()
    BtnKeyPress,      // OnKeyPress()
    PutButton,        // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,        //default button
    "Select All", //text
    push,         //type
    up,           //state
    NULL,         //icon
    SelAll,       //function
    92,

    // BEGIN SCREEN OBJECT DEFINITION
    365, 444, 90, 22, // position
    FALSE,            // focus
    PUSH_BUTTON,      // type
    TRUE,             // visible
    BtnMouseDown,     // OnMouseDown()
    BtnMouseUp,       // OnMouseUp()
    BtnClick,         // OnClick()
    BtnKeyPress,      // OnKeyPress()
    PutButton,        // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,       //default button
    "Clear All", //text
    push,        //type
    up,          //state
    NULL,        //icon
    ClearAll,    //function
    93,

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
    FALSE,             //default button
    "Apply",           //text
    push,              //type
    up,                //state
    NULL,              //icon
    SmartSaveBtnClick, //function
    94,

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
    FALSE,               //default button
    "Cancel",            //text
    push,                //type
    up,                  //state
    NULL,                //icon
    SmartCancelBtnClick, //function
    95};

static UCHAR* smart_strings[5] = {"Time of Day",
                                  "Dawn",
                                  "Dusk",
                                  "Random",
                                  "Smart Shopper"};
extern void EventTypeDropdownChange(void);
extern DROPDOWN_BOX_T ddevent_type;
DROPDOWN_BOX_T smart_dropdown =
    {
        //Event action
        //120, 65, 155, 17,
        129, 112, 146, 17,
        FALSE,                     // got_focus
        DROP_DOWN_BOX,             // type
        TRUE,                      // visible
        DropDownMouseDown,         // OnMouseDown()
        DropDownMouseUp,           // OnMouseUp()
        DropDownClick,             // OnClick()
        DropDownKeyPress,          // OnKeyPress()
        DisplayDropDown,           // DisplayObject()
        smart_strings,             // strings
        0,                         // item selected
        FALSE,                     // dropped
        5,                         // item count
        ScheduleTypeDropdownChange // OnChange
};

SPINNER_T costspin =
    {
        179, 190, 1, 1,   // position
        FALSE,            //focus
        TIME_ENTRY,       //type
        TRUE,             // visible
        SpinnerMouseDown, //OnMouseDown()
        SpinnerMouseUp,   //OnMouseUp()
        SpinnerClick,     // OnClick()
        SpinnerKeyPress,  // OnKeyPress()
        DisplaySpinner,   // DisplayObject()
        10,               //value
        99,               //max
        2,                //min
        1,                //increment
        SmartCashTextChange};

SPINNER_T hourspin =
    {
        260, 112, 1, 1,   // position
        FALSE,            //focus
        TIME_ENTRY,       //type
        TRUE,             // visible
        SpinnerMouseDown, //OnMouseDown()
        SpinnerMouseUp,   //OnMouseUp()
        SpinnerClick,     // OnClick()
        SpinnerKeyPress,  // OnKeyPress()
        DisplaySpinner,   // DisplayObject()
        0,                //value
        24,               //max
        0,                //min
        1,                //increment
        SmartTimeTextChange};

SPINNER_T minspin =
    {
        260, 190, 1, 1,   // position
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
        SmartTimeTextChange};

SCREEN_OBJECT_T* smart_so[34];
UCHAR smart_title[90];
static void smart_windowDraw(void);
LWINDOW_T smart_window[1] = {
    35, 0, 640, 445, //window position
    smart_so,        //screen object
    smart_windowDraw,
    34,               //item count
    0,                //cursor position 0-79 left to right
    0,                //element that has current focus
    fcsBtn,           //focus type
    smart_title,      // title
    DefaultOnKeyPress //OnKeyPress();
};

int this_cat;
void SmartSchedule(DEVICE* device, int cat) {
  int i;
  //   int hr;
  //   int min;

  Sthis_device = *device;
  this_cat = cat;

  sprintf(smart_title, "Smart Shopper for: %.13s %.18s", Sthis_device.desc, Sthis_device.location);

  smart_so[0] = &smart_dropdown.so;
  smart_dropdown.selected = 4;
  smart_so[1] = &costspin.so;

  switch (Sthis_device.control_type) {
  case 0:
    costspin.value = 10;
    Sthis_device.control_type = 2;
    break;
  case 1: //vacation off
  case 2: //cents per kwh
    costspin.value = (int)(Sthis_device.cost_day_hour / 1000.0);
    break;
  }
  smart_so[2] = &hourspin.so;
  smart_so[3] = &minspin.so;

  if (Sthis_device.max_mins_day > 0) {
    hourspin.value = Sthis_device.max_mins_day / 60;
    minspin.value = Sthis_device.max_mins_day % 60;
  } else {
    hourspin.value = 0;
    minspin.value = 0;
  }
  smart_so[4] = &cb_override.so;
  smart_so[5] = &cb_vacation.so;

  if (Sthis_device.control_type != 1) {
    cb_vacation.checked = TRUE;
  } else {
    cb_vacation.checked = FALSE;
  }

  if (Sthis_device.ss_override == TRUE) {
    cb_override.checked = TRUE;
  } else {
    cb_override.checked = FALSE;
  }
  for (i = 0; i < 24; i++) {
    smart_so[i + 6] = &cb_hour[i].so;
    if (Sthis_device.operate_hours[i / 8] & (0x80 >> (i % 8))) {
      cb_hour[i].checked = TRUE;
    } else {
      cb_hour[i].checked = FALSE;
    }
  }
  smart_so[30] = &smartbtn[0].so;
  smart_so[31] = &smartbtn[1].so;
  smart_so[32] = &smartbtn[2].so;
  smart_so[33] = &smartbtn[3].so;

  glbWindow = smart_window;
  PutWindow(glbWindow);
}

static void smart_windowDraw(void) {
  GroupBox("Event Definition", 83, 92, 311, 185);
  GroupBox("Hours of Operation", 83, 285, 311, 300);

  ptext("Run this device for", 227, 112, BG_WHT + FG_BLK);
  ptext("Hours", 241, 112, BG_WHT + FG_BLK);
  ptext("Minutes", 241, 190, BG_WHT + FG_BLK);

  // cost per kWh
  ptext("I don't want to pay", 162, 112, BG_WHT + FG_BLK);
  ptext("more than", 181, 112, BG_WHT + FG_BLK);
  ptext("cents per kWh.", 200, 112, BG_WHT + FG_BLK);

  //ptext("regardless of cost.", 366, 112, BG_WHT + FG_BLK);
}

void SmartCancelBtnClick(void) {
  if (dev_listbox.item_selected == 0) {
    ScheduleEvents(NULL, glb_cat_index, 1);
  } else {
    ScheduleEvents(&Ethis_device, glb_cat_index, 1);
  }
  //Device_Event_Menu(this_cat);
}

void SmartSaveBtnClick(void) {
  int i;
  int try;
  int finished;
  char buf[100];

  if (cb_vacation.checked == TRUE) {
    Sthis_device.control_type = 2;
  } else {
    Sthis_device.control_type = 1;
  }

  Sthis_device.cost_day_hour = (costspin.value * 1000);

  if (msgbox("The regular schedule|will be deleted.  Are you|sure you want to continue?",
             "Information",
             MB_YES_NO) == MB_RESULT_NO) {
    return;
  } else {
    Status("Removing regular schedule...");
    finished = FALSE;
    while (finished == FALSE) {
      finished = TRUE;
      for (i = 0; i < events_used; i++) {
        if (events[i].device == Sthis_device.device_slot) {
          Status("Removing events...");
          try = 0;
          sprintf(buf, "Retry");
          while ((RemoveEvent(&events[i]) == FALSE) && (try++ < 10)) {
            strcat(buf, ".");
            Status(buf);
            Wait(6L);
          }
          Wait(6L);
          finished = FALSE;
          break;
        }
      }
    }
    Status("");
  }
  try = FALSE;
  for (i = 0; i < 24; i++) {
    if (cb_hour[i].checked == TRUE) {
      try = TRUE;
      break;
    }
  }
  if (try == FALSE) {
    msgbox("No hours have been chosen|within which to run this|device.  Choose at least one hour.",
           "Information", MB_OK);
    return;
  }

  if (SendObject(&Sthis_device, 'C', sizeof(DEVICE), DEVICE_TABLE) == FAILURE) {
    msgbox("Unable to change device's|schedule.  Try again.", "Warning", MB_OK);
    return;
  } else {
    i = LookupDev(Sthis_device.device_slot);
    dev_list.devices[i] = Sthis_device;
    if (Sthis_device.control_type != 0) {
      if (user_backup() == SUCCESS) {
        Status("Smart scheduling enabled|for this device.");
      } else {
        msgbox("Error updating file:  devices.dat|Changes NOT saved to disk!",
               "Warning",
               MB_OK);
      }
    } else {
      if (user_backup() == SUCCESS) {
        msgbox("Smart scheduling disabled|for this device.", "Confirmation", MB_OK);
      } else {
        msgbox("Error updating file:  devices.dat|Changes NOT saved to disk!",
               "Warning",
               MB_OK);
      }
    }
    if (dev_listbox.item_selected == 0) {
      ScheduleEvents(NULL, glb_cat_index, 1);
    } else {
      ScheduleEvents(&Sthis_device, glb_cat_index, 1);
    }
  }
}

void ScheduleTypeDropdownChange(void) {
  int i;
  switch (smart_dropdown.selected) {
  case 0:
  case 1:
  case 2:
  case 3:
    if (Sthis_device.control_type != 0) //means that it is a "change" not an "add new"
    {
      if (dev_listbox.item_selected == 0) {
        for (i = 0; i < dev_listbox.item_count; i++) {
          if (dev_subliste[i]->device_slot == Sthis_device.device_slot) {
            dev_listbox.item_selected = i;
            break;
          }
        }
      }

      EventAddBtnClick();
      //dev_listbox.item_selected = 0;
    } else {
      EditEvent(&this_event, (UCHAR)event_opt, 1);
    }
    EventTypeDropdownChange();
    break;
  case 4:
    if (cb_vacation.checked == TRUE) {
      Sthis_device.control_type = 2;
    } else {
      Sthis_device.control_type = 1;
    }
    break;
  }
}

void CbORYesClick(void) {
  if (cb_override.checked == TRUE) {
    Sthis_device.ss_override = TRUE;
  } else {
    Sthis_device.ss_override = FALSE;
  }
}

void SmartCashTextChange(void) {
  Sthis_device.cost_day_hour = (costspin.value * 1000);
}

void SmartTimeTextChange(void) {
  Sthis_device.max_mins_day = (unsigned)((hourspin.value * 60) + minspin.value);
}

static void SelAll(void) {
  int i;
  hide_mouse();
  for (i = 0; i < 24; i++) {
    cb_hour[i].checked = TRUE;
    DisplayCheckBox(&cb_hour[i]);
    Sthis_device.operate_hours[i / 8] |= 0xFF;
  }
  unhide_mouse();
}

static void ClearAll(void) {
  int i;

  hide_mouse();
  for (i = 0; i < 24; i++) {
    cb_hour[i].checked = FALSE;
    DisplayCheckBox(&cb_hour[i]);
    Sthis_device.operate_hours[i / 8] &= 0x00;
  }
  unhide_mouse();
}

static void CbHourClick(void) {
  int i;

  for (i = 0; i < 24; i++) {
    if (cb_hour[i].so.got_focus == TRUE) {
      Sthis_device.operate_hours[i / 8] ^= (0x80 >> (i % 8));
      break;
    }
  }
}

static void CbVacationClick(void) {
  if (cb_vacation.checked == TRUE) {
    Sthis_device.control_type = 2;
  } else {
    Sthis_device.control_type = 1;
  }
}

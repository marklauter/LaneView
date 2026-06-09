#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>
#include <conio.h>
#include <string.h>

#include "cebus.h"
#include "2box.h"
#include "inio.h"
#include "inside.h"
#include "vga.h"
#include "scrnobjt.h"
#include "textbox.h"
#include "pshbtn.h"
#include "listbox.h"
#include "mouse.h"
#include "lwindow.h"
#include "ismenu.h"
#include "frame_3d.h"
#include "devcat.h"
#include "ctrlschd.h"
#include "msgbox.h"
#include "device.h"
#include "inio.h"
#include "event.h"
#include "smartscd.h"
#include "whmenu.h"
#include "scrolbar.h"
#include "dropdown.h"
#include "inutil.h"

static void DescriptionSort(void);
static void LocationSort(void);
static void EditDeviceBtnClick(void);
static void AddDeviceBtnClick(void);
static void RSDBtnClick(void);
static void ScheduleEventBtnClick(void);
static void SmartSchedClick(void);
static void TypeChange(void);
static void UpdateDeviceListBox(int cat_index);
static void list_boxdOnChange(void);
static void ControlChange(void);
static void SendControl(void);
static void DisplayDeviceAction(DEVICE* dev, int top, int left, UCHAR color);
static void DisplayDeviceActionNoReason(DEVICE* dev, int top, int left, UCHAR color);
static void ViewChange(void);

#define LOCATIONSORT 1
#define DESCRIPTIONSORT 0
static int sortorder;

static UCHAR action; // low nibble, 0 = off, 1 = on, 2 = dim
                     // high nibble: Dim Level 0 = none...
                     //                       15 = most;

extern char* tstat_strings[5];

void (*DDL_addr)() = DisplayDeviceLong;
void (*DDA_addr)() = DisplayDeviceAction;
void (*DDANR_addr)() = DisplayDeviceActionNoReason;

PUSH_BUTTON_T dneButton[9] =
    {
        // BEGIN SCREEN OBJECT DEFINITION
        395, 15, 130, 25,
        FALSE,        // focus
        PUSH_BUTTON,  // type
        TRUE,         // visible
        BtnMouseDown, // OnMouseDown()
        BtnMouseUp,   // OnMouseUp()
        BtnClick,     // OnClick()
        BtnKeyPress,  // OnKeyPress()
        PutButton,    // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        FALSE,                 // default button
        "Schedule Events",     // text
        push,                  // type
        up,                    // state
        NULL,                  // icon
        ScheduleEventBtnClick, //SmartSchedClick,      // function
        65,

        // BEGIN SCREEN OBJECT DEFINITION
        425, 20, 125, 25,
        FALSE,        // focus
        PUSH_BUTTON,  // type
        FALSE,        // visible
        BtnMouseDown, // OnMouseDown()
        BtnMouseUp,   // OnMouseUp()
        BtnClick,     // OnClick()
        BtnKeyPress,  // OnKeyPress()
        PutButton,    // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        FALSE,           // default button
        "Util. Control", // text
        push,            // type
        up,              // state
        NULL,            // icon
        NULL,            //LoadMgmtMenu,      // function     if(dev_list.devices_used == MAX_DEVICES) device table full
        66,

        // BEGIN SCREEN OBJECT DEFINITION
        362, 175, 77, 25,
        FALSE,        // focus
        PUSH_BUTTON,  // type
        TRUE,         // visible
        BtnMouseDown, // OnMouseDown()
        BtnMouseUp,   // OnMouseUp()
        BtnClick,     // OnClick()
        BtnKeyPress,  // OnKeyPress()
        PutButton,    // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        FALSE,             // default button
        "Add",             // text
        push,              // type
        up,                // state
        NULL,              // icon
        AddDeviceBtnClick, // function     if(dev_list.devices_used == MAX_DEVICES) device table full
        67,

        // BEGIN SCREEN OBJECT DEFINITION
        392, 175, 77, 25,
        FALSE,        // focus
        PUSH_BUTTON,  // type
        TRUE,         // visible
        BtnMouseDown, // OnMouseDown()
        BtnMouseUp,   // OnMouseUp()
        BtnClick,     // OnClick()
        BtnKeyPress,  // OnKeyPress()
        PutButton,    // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        FALSE,              // default button
        "Change",           // text
        push,               // type
        up,                 // state
        NULL,               // icon
        EditDeviceBtnClick, // function
        68,

        // BEGIN SCREEN OBJECT DEFINITION
        422, 175, 77, 25,
        FALSE,        // focus
        PUSH_BUTTON,  // type
        TRUE,         // visible
        BtnMouseDown, // OnMouseDown()
        BtnMouseUp,   // OnMouseUp()
        BtnClick,     // OnClick()
        BtnKeyPress,  // OnKeyPress()
        PutButton,    // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        FALSE,       //default button
        "Remove",    // text
        push,        // type
        up,          // state
        NULL,        // icon
        RSDBtnClick, // function
        69,

        // BEGIN SCREEN OBJECT DEFINITION
        435, 380, 60, 17,
        FALSE,        // focus
        PUSH_BUTTON,  // type
        TRUE,         // visible
        BtnMouseDown, // OnMouseDown()
        BtnMouseUp,   // OnMouseUp()
        BtnClick,     // OnClick()
        BtnKeyPress,  // OnKeyPress()
        PutButton,    // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        FALSE,       // default button
        "Apply",     // text
        push,        // type
        up,          // state
        NULL,        // icon
        SendControl, // function
        70,

        // BEGIN SCREEN OBJECT DEFINITION
        60, 5, 110, 16,
        FALSE,        // focus
        PUSH_BUTTON,  // type
        TRUE,         // visible
        BtnMouseDown, // OnMouseDown()
        BtnMouseUp,   // OnMouseUp()
        BtnClick,     // OnClick()
        BtnKeyPress,  // OnKeyPress()
        PutButton,    // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        FALSE,           //default button
        "Description",   //text
        set,             //type
        up,              //state
        NULL,            //icon
        DescriptionSort, //function
        71,

        // BEGIN SCREEN OBJECT DEFINITION
        60, 117, 145, 16,
        FALSE,        // focus
        PUSH_BUTTON,  // type
        TRUE,         // visible
        BtnMouseDown, // OnMouseDown()
        BtnMouseUp,   // OnMouseUp()
        BtnClick,     // OnClick()
        BtnKeyPress,  // OnKeyPress()
        PutButton,    // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        FALSE,        //default button
        "Location",   //text
        set,          //type
        up,           //state
        NULL,         //icon
        LocationSort, //function
        72,

        // BEGIN SCREEN OBJECT DEFINITION
        419, 530, 89, 33,
        FALSE,        // focus
        PUSH_BUTTON,  // type
        TRUE,         // visible
        BtnMouseDown, // OnMouseDown()
        BtnMouseUp,   // OnMouseUp()
        BtnClick,     // OnClick()
        BtnKeyPress,  // OnKeyPress()
        PutButton,    // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        TRUE,        //default button
        "Main|Menu", //text
        push,        //type
        up,          //state
        icon[12],    //icon
        IntroScreen, //function
        73};

DEVICE* dev_sublist[MAX_DEVICES];

LISTBOX_T list_boxd =
    {
        // BEGIN SCREEN OBJECT DEFINITION
        80, 5, 611, 250,  // position
        FALSE,            // focus
        LIST_BOX,         // type
        TRUE,             // visible
        ListBoxMouseDown, // OnMouseDown()
        ListBoxMouseUp,   // OnMouseUp()
        ListBoxClick,     // OnClick()
        ListBoxKeyPress,  // OnKeyPress()
        PutListBox,       // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        0,                 // item count
        0,                 // item selected
        0,                 // top_item
        0,                 // display_count
        dev_sublist,       // sub_list
        DisplayDeviceLong, // display function pointer
        list_boxdOnChange  // OnChange()
};

static void sbOnClick(int direction, int how_far, int thumb_pos);
static SCROLL_BAR_T sb =
    {
        // BEGIN SCREEN OBJECT DEFINITION
        80, 617, 17, 250,     // position
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
      PageUp(&list_boxd);
      break;
    case SCROLL_SMALL:
      UpArrow(&list_boxd);
      break;
    }
    break;
  case SCROLL_DOWN:
    switch (how_far) {
    case SCROLL_LARGE:
      PageDown(&list_boxd);
      break;
    case SCROLL_SMALL:
      DownArrow(&list_boxd);
      break;
    }
    break;
  }
  ListBoxScroll(&list_boxd);
  //PutListBox(&list_boxd);
  (*list_boxd.OnChange)();
}

static void sbEventOnScroll(int direction, int how_far, int thumb_pos);
static SCROLL_BAR_T sbEvent =
    {
        // BEGIN SCREEN OBJECT DEFINITION
        400, 292, 140, 17,    // position
        FALSE,                // focus
        SCROLL_BARV,          // type
        TRUE,                 // visible
        ScrollBarOnMouseDown, // OnMouseDown()
        ScrollBarOnMouseUp,   // OnMouseUp()
        ScrollBarOnClick,     // OnClick()
        NULL,                 // OnKeyPress()
        DisplayScrollBar,     // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        0,              // min
        15,             // max
        1,              // small change
        5,              // large change
        0,              // thumb possition
        SB_HORIZONTAL,  // type
        sbEventOnScroll // on scroll function
};

static void sbEventOnScroll(int direction, int how_far, int thumb_pos) {
  int rc;
  char deg[20];

  rc = IsTstat(dev_sublist[list_boxd.item_selected]);
  if (rc == X10_RCS_TSTAT) {
    sprintf(deg, "%2d%cF     ", tstat_table[sbEvent.thumb_pos + 4], 0xA4);
    ptext(deg, 385, 292, BG_WHT + FG_BLK);
  } else if (rc == X10_RCS_CENTI_TSTAT) {
    sprintf(deg, "%2d%cC     ", centi_tstat_table[sbEvent.thumb_pos + 4], 0xA4);
    ptext(deg, 385, 292, BG_WHT + FG_BLK);
  } else {
    sprintf(deg, "%.0f%% Bright   ",
            (double)(16 - sbEvent.thumb_pos) * 6.25);
    //         ((15 - sbEvent.thumb_pos + 1)/(15.0))*93.46);
    ptext(deg, 385, 292, BG_WHT + FG_BLK);
  }
}

extern int glb_cat_index;
static char* catlist[7] =
    {
        "All",
        "Lights",
        "Comfort",
        "Pool",
        "Sprinkler",
        "Security",
        "Monitor"};
static DROPDOWN_BOX_T ddtype =
    {
        355, 30, 100, 17,
        FALSE,             // got_focus
        DROP_DOWN_BOX,     // type
        TRUE,              // visible
        DropDownMouseDown, // OnMouseDown()
        DropDownMouseUp,   // OnMouseUp()
        DropDownClick,     // OnClick()
        DropDownKeyPress,  // OnKeyPress()
        DisplayDropDown,   // DisplayObject()
        catlist,           // strings
        0,                 // item selected
        FALSE,             // dropped
        7,                 // item count
        TypeChange         // OnChange
};

static char* controllist1[3] = {"Off", "On", "Dim"};
static char* controllist2[5] = {"Off", "Heat", "Cool", "Auto", "Temp"};
static DROPDOWN_BOX_T ddcontrol =
    {
        355, 312, 100, 17,
        FALSE,             // got_focus
        DROP_DOWN_BOX,     // type
        TRUE,              // visible
        DropDownMouseDown, // OnMouseDown()
        DropDownMouseUp,   // OnMouseUp()
        DropDownClick,     // OnClick()
        DropDownKeyPress,  // OnKeyPress()
        DisplayDropDown,   // DisplayObject()
        controllist1,      // strings
        0,                 // item selected
        FALSE,             // dropped
        3,                 // item count
        ControlChange      // OnChange
};

static char* viewlist[3] = {"Status Summary", "Status Detail", "Address Detail"};
static DROPDOWN_BOX_T ddview =
    {
        355, 465, 140, 17,
        FALSE,             // got_focus
        DROP_DOWN_BOX,     // type
        TRUE,              // visible
        DropDownMouseDown, // OnMouseDown()
        DropDownMouseUp,   // OnMouseUp()
        DropDownClick,     // OnClick()
        DropDownKeyPress,  // OnKeyPress()
        DisplayDropDown,   // DisplayObject()
        viewlist,          // strings
        0,                 // item selected
        FALSE,             // dropped
        3,                 // item count
        ViewChange         // OnChange
};

typedef struct
{
  UCHAR state[2];
  UCHAR reason[2];
} DEVICE_STATUS_T;
DEVICE_STATUS_T device_state[MAX_DEVICES];

SCREEN_OBJECT_T* dneSo[15];
static void dneDraw(void);
LWINDOW_T dneWindow[1] = {
    35, 0, 640, 445, //window position
    dneSo,           //screen object
    dneDraw,
    15,                        //item count
    0,                         //cursor position 0-79 left to right
    0,                         //element that has current focus
    fcsBtn,                    //focus type
    "Device Status & Control", // title
    DefaultOnKeyPress          //OnKeyPress();
};

//int GetTable(void* table_ptr, int table_size, int table_id, void* request_data,
//            int data_length, int retries, int show_status)

void Device_Event_Menu(int cat_index) {
  int i = 0;
  int j = 0;
  FILE* f;

  Busy();

  memset(dev_sublist, 0, MAX_DEVICES * sizeof(DEVICE*));
  memset(device_state, 0, sizeof(DEVICE_STATUS_T) * MAX_DEVICES);

  if (secured_user != TRUE) {
    f = fopen("view.dat", "rb");
    if (f == NULL) {
      ddview.selected = 0;
    } else {
      fread(&ddview.selected, sizeof(int), 1, f);
      fclose(f);
    }
  }

  if (ddview.selected != 2) {
    Status("Please wait...");
    if (GetTable(device_state,
                 sizeof(DEVICE_STATUS_T) * dev_list.devices_used,
                 ALL_DEVICES_STATUS,
                 NULL,
                 0,
                 RETRY_COUNT,
                 FALSE) != SUCCESS) {
      msgbox("Unable to get status|table from ICU.", "Warning", MB_OK);
    }
  }

  switch (ddview.selected) {
  case 2: // address detail
    list_boxd.DisplayItem = DDL_addr;
    break;
  case 1: // status detail
    list_boxd.DisplayItem = DDA_addr;
    break;
  case 0: // status summary
    list_boxd.DisplayItem = DDANR_addr;
    break;
  default:
    ddview.selected = 0;
    list_boxd.DisplayItem = DDANR_addr;
    break;
  }

  ddtype.selected = cat_index;
  glb_cat_index = cat_index;

  dneSo[0] = &list_boxd.so;
  dneSo[1] = &ddtype.so;
  dneSo[2] = &dneButton[0].so;
  dneSo[3] = &dneButton[1].so;
  dneSo[4] = &dneButton[2].so;
  dneSo[5] = &dneButton[3].so;
  dneSo[6] = &dneButton[4].so;
  dneSo[7] = &ddcontrol.so;
  dneSo[8] = &dneButton[5].so;
  dneSo[9] = &ddview.so;
  dneSo[10] = &dneButton[8].so;
  dneSo[11] = &dneButton[6].so;
  dneSo[12] = &dneButton[7].so;
  dneSo[13] = &sb.so;
  dneSo[14] = &sbEvent.so;

  list_boxd.so.visible = FALSE;

  sbEvent.so.visible = FALSE;
  if (sortorder == DESCRIPTIONSORT) {
    dneButton[6].state = down;
    dneButton[7].state = up;
  } else {
    dneButton[7].state = down;
    dneButton[6].state = up;
  }

  if (cat_index != 0) {
    for (i = 0, j = 0; i < dev_list.devices_used; i++) {
      if (secured_user == TRUE) {
        if (dev_list.devices[i].category == cat_index) {
          dev_sublist[j++] = &dev_list.devices[i];
        }
      } else {
        if ((dev_list.devices[i].category == cat_index) &&
            (IsSubmeter(&dev_list.devices[i]) == FALSE)) {
          dev_sublist[j++] = &dev_list.devices[i];
        }
      }
    }
    list_boxd.item_count = j;
  } else // all devices
  {
    for (i = 0, j = 0; i < dev_list.devices_used; i++) {
      if (secured_user == TRUE) {
        dev_sublist[j++] = &dev_list.devices[i];
      } else {
        if (IsSubmeter(&dev_list.devices[i]) == FALSE) {
          dev_sublist[j++] = &dev_list.devices[i];
        }
      }
    }
    list_boxd.item_count = j;
  }

  if (list_boxd.item_count == 0) {
    dneButton[0].so.visible = FALSE; // "Schedule events"
    dneButton[3].so.visible = FALSE; // "Change device"
    dneButton[4].so.visible = FALSE; // "Remove device"
  } else {
    dneButton[0].so.visible = TRUE; // "Schedule events"
    dneButton[3].so.visible = TRUE; // "Change device"
    dneButton[4].so.visible = TRUE; // "Remove device"
  }

  sb.max = list_boxd.item_count - 1;
  sb.thumb_pos = list_boxd.item_selected;
  sb.large_chg = list_boxd.so.position.height / 14;
  if (sb.large_chg > list_boxd.item_count) {
    sb.large_chg = list_boxd.item_count;
  }
  glbWindow = dneWindow;
  PutWindow(glbWindow);
  Status("");
  NotBusy();
}
static void dneDraw(void) {
  switch (ddview.selected) {
  case 2: // address detail
    ptext("Pri Type                 Address     SU ", 63, 268, BG_WHT + FG_BLK);
    break;
  case 1: // status detail
    ptext("  State       Reason                    ", 63, 263, BG_WHT + FG_BLK);
    break;
  case 0: // status summary
    ptext("  State                                 ", 63, 263, BG_WHT + FG_BLK);
    break;
  default:
    ddview.selected = 0;
    ptext("  State                                 ", 63, 263, BG_WHT + FG_BLK);
    break;
  }
  //void GroupBox(UCHAR *text, int top, int left, int height, int width)
  GroupBox("Setup", 335, 155, 120, 115);
  GroupBox("Category", 335, 15, 55, 130);
  GroupBox("Immediate Control", 335, 280, 120, 165);
  GroupBox("View", 335, 455, 55, 165);
  list_boxd.so.visible = TRUE;
  if (sortorder == DESCRIPTIONSORT) {
    DescriptionSort();
  } else {
    LocationSort();
  }
  if (IsTstat(dev_sublist[list_boxd.item_selected]) != FALSE) {
    ddcontrol.selected = 0;
    ddcontrol.strings = controllist2;
    ddcontrol.count = 5;
    DisplayDropDown(&ddcontrol);
  } else {
    ddcontrol.selected = 0;
    ddcontrol.strings = controllist1;
    if (IsDimmer(dev_sublist[list_boxd.item_selected])) {
      ddcontrol.count = 3;
    } else {
      ddcontrol.count = 2;
    }
    DisplayDropDown(&ddcontrol);
  }
}

static void EditDeviceBtnClick(void) {
  if (list_boxd.item_count == 0) {
    return;
  }
  if ((IsSubmeter(dev_sublist[list_boxd.item_selected])) && (secured_user != TRUE)) {
    msgbox("Permission Denied!  Only|an authorized utility representative|may change InterLane Sub Meters.",
           "Invalid Access Level", MB_OK);
    return;
  }
  EditDevice(dev_sublist[list_boxd.item_selected], (int)'C', glb_cat_index);
}

static void AddDeviceBtnClick(void) {
  DEVICE new_dev;

  if (dev_list.devices_used == MAX_DEVICES) {
    msgbox("Device table full.|Delete some devices to make|room for new ones.", "Information", MB_OK);
  } else {
    memset(&new_dev, 0, sizeof(DEVICE));
    new_dev.code_letter = 'A';
    new_dev.code_number = 1;
    memset(new_dev.desc, ' ', 13);
    memset(new_dev.location, ' ', 18);
    new_dev.cebus_type = 3;
    memset(new_dev.serial_no, ' ', 12);
    new_dev.category = glb_cat_index;
    EditDevice(&new_dev, (int)'A', glb_cat_index);
  }
}

static void RSDBtnClick(void) //remove selected device
{
  int result;

  if (list_boxd.item_count == 0) {
    return;
  }
  if ((IsSubmeter(dev_sublist[list_boxd.item_selected])) && (secured_user != TRUE)) {
    msgbox("Permission Denied!  Only|an authorized utility representative|may remove InterLane Sub Meters.",
           "Invalid Access Level", MB_OK);
    return;
  }

  if (confirmation == TRUE) {
    result = msgbox("Are you sure you want|to remove this device?",
                    "Caution", MB_YES_NO);

  } else {
    result = MB_RESULT_YES;
  }
  if (result == MB_RESULT_YES) {
    if (RemoveDevice(dev_sublist[list_boxd.item_selected]) == SUCCESS) {
      if (user_backup() == SUCCESS) {
        Status("Device successfully removed.");
      } else {
        msgbox("Error saving change to disk.|Device successfully removed from ICU.|devices.dat and ICU don't match.",
               "Warning",
               MB_OK);
      }
      //Device_Event_Menu(glb_cat_index);
      UpdateDeviceListBox(ddtype.selected);
    }
  }
  Device_Event_Menu(glb_cat_index);
}

static void ScheduleEventBtnClick(void) {
  //   if (list_boxd.item_count == 0)
  //{
  //ScheduleEvents(NULL, glb_cat_index, 1);
  //return;
  //}
  //if (dev_sublist[list_boxd.item_selected]->control_type != 0)
  //{
  //msgbox(   "This device is Smart Scheduled.|Disable Smart Scheduling for this|device before adding events.",
  //"Information", MB_OK);
  //return;
  //}
  if (IsSubmeter(dev_sublist[list_boxd.item_selected]) == TRUE) {
    msgbox("Sub Meter devices|cannot be scheduled.", "Information", MB_OK);
    return;
  }
  ScheduleEvents(dev_sublist[list_boxd.item_selected], glb_cat_index, 1);
}

static void SmartSchedClick(void) {
  if (dev_list.devices_used == 0) {
    msgbox("There are no devices to Smart|Schedule.  Add devices first.",
           "Information", MB_OK);
    return;
  }
  SmartSchedule(dev_sublist[list_boxd.item_selected], glb_cat_index);
}

static void UpdateDeviceListBox(int cat_index) {
  int i;
  int j;

  glb_cat_index = cat_index;

  ////////////////////////////////////
  //
  // update device status table
  //
  ////////////////////////////////////
  if (ddview.selected != 2) {
    Busy();
    Status("Please wait, updating device list...");
    memset(device_state, 0, sizeof(DEVICE_STATUS_T) * MAX_DEVICES);
    if (GetTable(device_state,
                 sizeof(DEVICE_STATUS_T) * dev_list.devices_used,
                 ALL_DEVICES_STATUS,
                 NULL,
                 0,
                 RETRY_COUNT,
                 FALSE) != SUCCESS) {
      msgbox("Unable to get status|table from ICU.", "Warning", MB_OK);
    }
    Status("");
    NotBusy();
  }
  ////////////////////////////////////

  //list_boxd.item_selected = 0;
  if (cat_index != 0) {
    for (i = 0, j = 0; i < dev_list.devices_used; i++) {
      if (secured_user == TRUE) {
        if (dev_list.devices[i].category == cat_index) {
          dev_sublist[j++] = &dev_list.devices[i];
        }
      } else {
        if ((dev_list.devices[i].category == cat_index) &&
            (IsSubmeter(&dev_list.devices[i]) == FALSE)) {
          dev_sublist[j++] = &dev_list.devices[i];
        }
      }
    }
    list_boxd.item_count = j;
  } else // all devices
  {
    for (i = 0, j = 0; i < dev_list.devices_used; i++) {
      if (secured_user == TRUE) {
        dev_sublist[j++] = &dev_list.devices[i];
      } else {
        if (IsSubmeter(&dev_list.devices[i]) == FALSE) {
          dev_sublist[j++] = &dev_list.devices[i];
        }
      }
    }
    list_boxd.item_count = j;
  }
  if (list_boxd.item_selected > list_boxd.item_count) {
    list_boxd.item_selected = 0;
  }
  if (sortorder == DESCRIPTIONSORT) {
    DescriptionSort();
  } else {
    LocationSort();
  }
  sb.max = list_boxd.item_count - 1;
  sb.thumb_pos = list_boxd.item_selected;
  sb.large_chg = list_boxd.so.position.height / 14;
  if (sb.large_chg > list_boxd.item_count) {
    sb.large_chg = list_boxd.item_count;
  }
  DisplayScrollBar(&sb);
  list_boxdOnChange();
}

static void TypeChange(void) {
  glb_cat_index = ddtype.selected;
  UpdateDeviceListBox(ddtype.selected);
}

static void list_boxdOnChange(void) {
  sb.max = list_boxd.item_count - 1;
  sb.thumb_pos = list_boxd.item_selected;
  sb.large_chg = list_boxd.so.position.height / 14;
  if (sb.large_chg > list_boxd.item_count) {
    sb.large_chg = list_boxd.item_count;
  }
  DisplayScrollBar(&sb);
  if (IsTstat(dev_sublist[list_boxd.item_selected]) != FALSE) {
    ddcontrol.selected = 0;
    ddcontrol.strings = controllist2;
    ddcontrol.count = 5;
    DisplayDropDown(&ddcontrol);
  } else {
    ddcontrol.selected = 0;
    ddcontrol.strings = controllist1;
    if (IsDimmer(dev_sublist[list_boxd.item_selected])) {
      ddcontrol.count = 3;
    } else {
      ddcontrol.count = 2;
    }
    DisplayDropDown(&ddcontrol);
  }
  sbEvent.so.visible = FALSE;
  area_clear(385, 433, 289, 435, FG_WHT + BG_WHT);
}

static void DescriptionSort(void) {
  int i;
  int swap;
  DEVICE* devholder;

  swap = TRUE;
  while (swap == TRUE) {
    swap = FALSE;
    for (i = 0; i < list_boxd.item_count - 1; i++) {
      if (strnicmp(dev_sublist[i]->desc, dev_sublist[i + 1]->desc, 13) > 0) {
        devholder = dev_sublist[i];
        dev_sublist[i] = dev_sublist[i + 1];
        dev_sublist[i + 1] = devholder;
        swap = TRUE;
      }
    }
  }
  sortorder = DESCRIPTIONSORT;
  PutListBox(&list_boxd);
}

static void LocationSort(void) {
  int i;
  int swap;
  DEVICE* devholder;

  swap = TRUE;
  while (swap == TRUE) {
    swap = FALSE;
    for (i = 0; i < list_boxd.item_count - 1; i++) {
      if (strnicmp(dev_sublist[i]->location, dev_sublist[i + 1]->location, 18) > 0) {
        devholder = dev_sublist[i];
        dev_sublist[i] = dev_sublist[i + 1];
        dev_sublist[i + 1] = devholder;
        swap = TRUE;
      }
    }
  }
  sortorder = LOCATIONSORT;
  PutListBox(&list_boxd);
}

static void ControlChange(void) {
  int rc;
  char deg[20];

  sbEvent.thumb_pos = 0;
  sbEvent.so.visible = FALSE;
  area_clear(385, 433, 289, 435, FG_WHT + BG_WHT);
  rc = IsTstat(dev_sublist[list_boxd.item_selected]);
  if (dev_sublist[list_boxd.item_selected]->x10_device_type == X10_RCS_TSTAT) {
    if (ddcontrol.selected == 4) {
      sprintf(deg, "%2d%cF     ", tstat_table[sbEvent.thumb_pos + 4], 0xA4);
      ptext(deg, 385, 292, BG_WHT + FG_BLK);
      sbEvent.max = 27;
      sbEvent.large_chg = 7;
      sbEvent.so.visible = TRUE;
      DisplayScrollBar(&sbEvent);
      ptext("Cooler      Warmer", 420, 292, BG_TRANSPARENT + FG_BLK);
    }
  } else if (dev_sublist[list_boxd.item_selected]->x10_device_type == X10_RCS_CENTI_TSTAT) {
    if (ddcontrol.selected == 4) {
      sprintf(deg, "%2d%cC     ", centi_tstat_table[sbEvent.thumb_pos + 4], 0xA4);
      ptext(deg, 385, 292, BG_WHT + FG_BLK);
      sbEvent.max = 27;
      sbEvent.large_chg = 7;
      sbEvent.so.visible = TRUE;
      DisplayScrollBar(&sbEvent);
      ptext("Cooler      Warmer", 420, 292, BG_TRANSPARENT + FG_BLK);
    }
  } else {
    if (IsDimmer(dev_sublist[list_boxd.item_selected]) == TRUE) {
      if (ddcontrol.selected == 2) {
        sprintf(deg, "%.0f%% Bright   ",
                (double)(16 - sbEvent.thumb_pos) * 6.25);
        //               ((15 - sbEvent.thumb_pos + 1)/(15.0))*93.46);
        ptext(deg, 385, 292, BG_WHT + FG_BLK);
        sbEvent.max = 15;
        sbEvent.large_chg = 5;
        sbEvent.so.visible = TRUE;
        DisplayScrollBar(&sbEvent);
        ptext("Bright         Dim", 420, 292, BG_TRANSPARENT + FG_BLK);
      }
    } else {
      if (ddcontrol.selected > 1) {
        ddcontrol.selected = 0;
        DisplayDropDown(&ddcontrol);
        Status("This device is not a dimmer.");
      }
    }
  }
}

static void SendControl(void) {
  int i;
  int rc;
  //int sel;

  extern int SendTable(int table_no, void* request_data, int data_length);
  typedef struct
  {
    USHORT id;
    UCHAR state;
  } CTRL;

  CTRL ctrl;

  rc = IsTstat(dev_sublist[list_boxd.item_selected]);
  if (rc == X10_RCS_TSTAT) {
    if (ddcontrol.selected == 4) {
      ctrl.state = tstat_table[sbEvent.thumb_pos + 4];
    } else {
      ctrl.state = ddcontrol.selected;
    }
  } else if (rc == X10_RCS_CENTI_TSTAT) {
    if (ddcontrol.selected == 4) {
      ctrl.state = centi_tstat_table[sbEvent.thumb_pos + 4];
    } else {
      ctrl.state = ddcontrol.selected;
    }
  } else {
    if (ddcontrol.selected == 2) // dim command
    {
      ctrl.state = (sbEvent.thumb_pos << 4) + 0x02;
    } else {
      ctrl.state = ddcontrol.selected;
    }
  }

  ctrl.id = dev_sublist[list_boxd.item_selected]->device_slot;

  for (i = 0; i < 5; i++) {
    if (SendTable(IMMEDIATE_CTRL_TBL, &ctrl, sizeof(CTRL)) == SUCCESS) {
      Status("Control successful");
      //sel = list_boxd.item_selected;
      UpdateDeviceListBox(ddtype.selected);
      //list_boxd.item_selected  = sel;
      return;
    }
  }
  msgbox("Unable to send control.|Try Again.", "Immediate Control", MB_OK);
}

//typedef struct
//{
//   UCHAR state[2];
//   UCHAR reason;
//} DEVICE_STATUS_T;

//DEVICE_STATUS_T device_state.reason[MAX_DEVICES];

static void DisplayDeviceAction(DEVICE* dev, int top, int left, UCHAR color) {
  UCHAR desc[100];
  UCHAR work[100];
  UCHAR action[20];
  int device_index;
  int rc;

  memset(desc, 0, 100);
  memset(work, 0, 100);
  memset(action, 0, 20);

  sprintf(desc, "%.13s %.18s",
          dev->desc,
          dev->location);

  if ((device_index = LookupDev(dev->device_slot)) < 0) {
    sprintf(desc, "ERROR DEVICE ID: %d", dev->device_slot);
    ptext(desc, top, left, color);
  } else {
    // last event sent to device
    rc = IsTstat(dev);
    if (rc == X10_RCS_TSTAT) {
      switch (device_state[device_index].state[1]) {
      case TSTAT_OFF:
        //tstat_table[]
        sprintf(work, "%2d%cF/Off", device_state[device_index].state[0], 0xA4);
        break;
      case TSTAT_HEAT:
        sprintf(work, "%2d%cF/Heat", device_state[device_index].state[0], 0xA4);
        break;
      case TSTAT_COOL:
        sprintf(work, "%2d%cF/Cool", device_state[device_index].state[0], 0xA4);
        break;
      case TSTAT_AUTO:
        sprintf(work, "%2d%cF/Auto", device_state[device_index].state[0], 0xA4);
        break;
      default:
        sprintf(work, "UNKNOWN");
        break;
      }
    } else if (rc == X10_RCS_CENTI_TSTAT) {
      switch (device_state[device_index].state[1]) {
      case TSTAT_OFF:
        //tstat_table[]
        sprintf(work, "%2d%cC/Off", device_state[device_index].state[0], 0xA4);
        break;
      case TSTAT_HEAT:
        sprintf(work, "%2d%cC/Heat", device_state[device_index].state[0], 0xA4);
        break;
      case TSTAT_COOL:
        sprintf(work, "%2d%cC/Cool", device_state[device_index].state[0], 0xA4);
        break;
      case TSTAT_AUTO:
        sprintf(work, "%2d%cC/Auto", device_state[device_index].state[0], 0xA4);
        break;
      default:
        sprintf(work, "UNKNOWN");
        break;
      }
    } else {
      switch (device_state[device_index].state[0]) {
      case EVNT_OFF:
        sprintf(work, "Off");
        //strcat(displ_event_text, "  Off");
        break;
      case EVNT_ON:
        sprintf(work, "On");
        //strcat(displ_event_text, "  On ");
        break;
      case EVNT_DIM0:
      case EVNT_DIM1:
      case EVNT_DIM2:
      case EVNT_DIM3:
      case EVNT_DIM4:
      case EVNT_DIM5:
      case EVNT_DIM6:
      case EVNT_DIM7:
      case EVNT_DIM8:
      case EVNT_DIM9:
      case EVNT_DIM10:
      case EVNT_DIM11:
      case EVNT_DIM12:
      case EVNT_DIM13:
      case EVNT_DIM14:
      case EVNT_DIM15:
        sprintf(work,
                "%.0f%% Bright",
                (double)((16.0 - (device_state[device_index].state[0] / 16)) * 6.25));
        break;
      default:
        sprintf(work, "UNKNOWN");
        break;
      }
    }
    sprintf(action, "  %-11s ", work);
    strcat(desc, action);

    // reason
    if (IsTstat(dev)) {
      switch (device_state[device_index].reason[0]) {
      case SM_REGULAR_EVENT:
        sprintf(work, "Standard Event/");
        break;
      case SM_DEVICE_RESET:
        sprintf(work, "Schedule Change/");
        break;
      case SM_END_LOADCTRL:
        sprintf(work, "Util Ctrl End/");
        break;
      case SM_LOADCTRL:
        sprintf(work, "Util Ctrl/");
        break;
      case SM_SMART_SCHED:
        sprintf(work, "Smart Shopper/");
        break;
      case SM_ENDMOOD6:
        sprintf(work, "All Moods End/");
        break;
      case SM_IMMEDIATE:
        sprintf(work, "User Control/");
        break;
      case SM_PROTECTED_MOOD:
        sprintf(work, "Protected Mood/");
        break;
      case SM_UNPROTECTED_MOOD:
        sprintf(work, "Unprot. Mood/");
        break;
      case SM_END_MOOD:
        sprintf(work, "Mood End/");
        break;
      case SM_TRIGGER:
        sprintf(work, "Remote Control/");
        break;
      case SM_SECURITY_MODE:
        sprintf(work, "Security Sys./");
        break;
      case SM_SECURITY_ALARM:
        sprintf(work, "Security ALARM/");
        break;
      case SM_NEW_RATE:
        sprintf(work, "Price Change/");
        break;
      case SM_REBOOT:
        sprintf(work, "System Reboot/");
        break;
      case SM_REQUEST:
        sprintf(work, "User Reset/");
        break;
      case SM_XFI_IMMED_CTRL:
        sprintf(work, "Phone Ctrl/");
        break;
      case SM_XFI_MOOD:
        sprintf(work, "Phone Mood/");
        break;
      case SM_EMAIL_ALERT:
        sprintf(work, "Email Rec'd/");
        break;
      default:
        if (DEMO) {
          sprintf(work, "Standard Event/");
        } else {
          sprintf(work, "UNKNOWN/");
        }
        break;
      }
      strcat(desc, work);
      switch (device_state[device_index].reason[1]) {
      case SM_REGULAR_EVENT:
        sprintf(work, "Standard Event");
        break;
      case SM_DEVICE_RESET:
        sprintf(work, "Schedule Change");
        break;
      case SM_END_LOADCTRL:
        sprintf(work, "Util Ctrl End");
        break;
      case SM_LOADCTRL:
        sprintf(work, "Util Ctrl");
        break;
      case SM_SMART_SCHED:
        sprintf(work, "Smart Shopper");
        break;
      case SM_ENDMOOD6:
        sprintf(work, "All Moods End");
        break;
      case SM_IMMEDIATE:
        sprintf(work, "User Control");
        break;
      case SM_PROTECTED_MOOD:
        sprintf(work, "Protected Mood");
        break;
      case SM_UNPROTECTED_MOOD:
        sprintf(work, "Unprot. Mood");
        break;
      case SM_END_MOOD:
        sprintf(work, "Mood End");
        break;
      case SM_TRIGGER:
        sprintf(work, "Remote Control");
        break;
      case SM_SECURITY_MODE:
        sprintf(work, "Security Sys.");
        break;
      case SM_SECURITY_ALARM:
        sprintf(work, "Security ALARM");
        break;
      case SM_NEW_RATE:
        sprintf(work, "Price Change");
        break;
      case SM_REBOOT:
        sprintf(work, "System Reboot");
        break;
      case SM_REQUEST:
        sprintf(work, "User Reset");
        break;
      case SM_XFI_IMMED_CTRL:
        sprintf(work, "Phone Ctrl");
        break;
      case SM_XFI_MOOD:
        sprintf(work, "Phone Mood");
        break;
      case SM_EMAIL_ALERT:
        sprintf(work, "Email Received");
        break;
      default:
        if (DEMO) {
          sprintf(work, "Standard Event");
        } else {
          sprintf(work, "UNKNOWN");
        }
        break;
      }
    } else {
      switch (device_state[device_index].reason[0]) {
      case SM_REGULAR_EVENT:
        sprintf(work, "Standard Event");
        break;
      case SM_DEVICE_RESET:
        sprintf(work, "Schedule Type Change");
        break;
      case SM_END_LOADCTRL:
        sprintf(work, "Utility Control Ended");
        break;
      case SM_LOADCTRL:
        sprintf(work, "Under Utility Control");
        break;
      case SM_SMART_SCHED:
        sprintf(work, "Smart Schedule in Effect");
        break;
      case SM_ENDMOOD6:
        sprintf(work, "All Moods Ended");
        break;
      case SM_IMMEDIATE:
        sprintf(work, "Immediate Control from LV");
        break;
      case SM_PROTECTED_MOOD:
        sprintf(work, "Protected Mood Started");
        break;
      case SM_UNPROTECTED_MOOD:
        sprintf(work, "Unprotected Mood Started");
        break;
      case SM_END_MOOD:
        sprintf(work, "Protected Mood Ended");
        break;
      case SM_TRIGGER:
        sprintf(work, "Remote Control");
        break;
      case SM_SECURITY_MODE:
        sprintf(work, "Security System");
        break;
      case SM_SECURITY_ALARM:
        sprintf(work, "Security ALARM");
        break;
      case SM_NEW_RATE:
        sprintf(work, "Price Change");
        break;
      case SM_REBOOT:
        sprintf(work, "System Reboot");
        break;
      case SM_REQUEST:
        sprintf(work, "User Requested Reset");
        break;
      case SM_XFI_IMMED_CTRL:
        sprintf(work, "Phone Ctrl");
        break;
      case SM_XFI_MOOD:
        sprintf(work, "Phone Mood");
        break;
      case SM_EMAIL_ALERT:
        sprintf(work, "Email Received");
        break;
      default:
        if (DEMO) {
          sprintf(work, "Standard Event");
        } else {
          sprintf(work, "UNKNOWN");
        }
        break;
      }
    }
    strcat(desc, work);
    ptext(desc, top, left, color);
  }
}

static void DisplayDeviceActionNoReason(DEVICE* dev, int top, int left, UCHAR color) {
  UCHAR desc[100];
  UCHAR work[100];
  UCHAR action[20];
  int device_index;
  int rc;

  memset(desc, 0, 100);
  memset(work, 0, 100);
  memset(action, 0, 20);

  sprintf(desc, "%.13s %.18s",
          dev->desc,
          dev->location);

  if ((device_index = LookupDev(dev->device_slot)) < 0) {
    sprintf(desc, "ERROR DEVICE ID: %d", dev->device_slot);
    ptext(desc, top, left, color);
  } else {
    // last event sent to device
    rc = IsTstat(dev);
    if (rc == X10_RCS_TSTAT) {
      switch (device_state[device_index].state[1]) {
      case TSTAT_OFF:
        //tstat_table[]
        sprintf(work, "%2d%cF/Off", device_state[device_index].state[0], 0xA4);
        break;
      case TSTAT_HEAT:
        sprintf(work, "%2d%cF/Heat", device_state[device_index].state[0], 0xA4);
        break;
      case TSTAT_COOL:
        sprintf(work, "%2d%cF/Cool", device_state[device_index].state[0], 0xA4);
        break;
      case TSTAT_AUTO:
        sprintf(work, "%2d%cF/Auto", device_state[device_index].state[0], 0xA4);
        break;
      default:
        sprintf(work, "UNKNOWN");
        break;
      }
    } else if (rc == X10_RCS_CENTI_TSTAT) {
      switch (device_state[device_index].state[1]) {
      case TSTAT_OFF:
        //tstat_table[]
        sprintf(work, "%2d%cC/Off", device_state[device_index].state[0], 0xA4);
        break;
      case TSTAT_HEAT:
        sprintf(work, "%2d%cC/Heat", device_state[device_index].state[0], 0xA4);
        break;
      case TSTAT_COOL:
        sprintf(work, "%2d%cC/Cool", device_state[device_index].state[0], 0xA4);
        break;
      case TSTAT_AUTO:
        sprintf(work, "%2d%cC/Auto", device_state[device_index].state[0], 0xA4);
        break;
      default:
        sprintf(work, "UNKNOWN");
        break;
      }
    } else {
      switch (device_state[device_index].state[0]) {
      case EVNT_OFF:
        sprintf(work, "Off");
        //strcat(displ_event_text, "  Off");
        break;
      case EVNT_ON:
        sprintf(work, "On");
        //strcat(displ_event_text, "  On ");
        break;
      case EVNT_DIM0:
      case EVNT_DIM1:
      case EVNT_DIM2:
      case EVNT_DIM3:
      case EVNT_DIM4:
      case EVNT_DIM5:
      case EVNT_DIM6:
      case EVNT_DIM7:
      case EVNT_DIM8:
      case EVNT_DIM9:
      case EVNT_DIM10:
      case EVNT_DIM11:
      case EVNT_DIM12:
      case EVNT_DIM13:
      case EVNT_DIM14:
      case EVNT_DIM15:
        //               sprintf(work, "Dim %2d", device_state[device_index].state[0]/16);
        sprintf(work,
                "%.0f%% Bright",
                (double)((16.0 - (device_state[device_index].state[0] / 16)) * 6.25));
        break;
      default:
        sprintf(work, "UNKNOWN");
        break;
      }
    }
    sprintf(action, "  %-11s ", work);
    strcat(desc, action);
    ptext(desc, top, left, color);
  }
}

static void ViewChange(void) {
  FILE* f;

  if (secured_user == FALSE) {
    f = fopen("view.dat", "wb");
    fwrite(&ddview.selected, sizeof(int), 1, f);
    fclose(f);
  }
  switch (ddview.selected) {
  case 2: // address detail
    ptext("Pri Type                 Address     SU ", 63, 268, BG_WHT + FG_BLK);
    list_boxd.DisplayItem = DDL_addr;
    break;
  case 1: // status detail
    ptext("  State       Reason                    ", 63, 263, BG_WHT + FG_BLK);
    list_boxd.DisplayItem = DDA_addr;
    break;
  case 0: // status summary
    ptext("  State                                 ", 63, 263, BG_WHT + FG_BLK);
    list_boxd.DisplayItem = DDANR_addr;
    break;
  default:
    ddview.selected = 0;
    ptext("  State                                 ", 63, 263, BG_WHT + FG_BLK);
    list_boxd.DisplayItem = DDANR_addr;
    break;
  }
  PutListBox(&list_boxd);
}

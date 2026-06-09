//==========================================================================
// HISTORY:
// Author      Date      Description
// Mark Lauter   15 DEC 96   Contains all functions
//                     relating to EVENTS
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
#include "tmentry.h"
#include "spinner.h"

EVENT events[MAX_EVENTS];
int events_used;

#define DEVICESORT 1
#define EVENTSORT 0

static int schedule_sort;

static void ResetFilters(void);
static void DeviceSort(void);
static void EventSort(void);

extern char ADVANCED;
extern void SmartSchedule(DEVICE* device, int cat);
DEVICE* dev_subliste[MAX_DEVICES + 1];
void dev_listboxOnChange(void);
extern void DisplayDeviceShort(DEVICE* dev, int top, int left, UCHAR color);
LISTBOX_T dev_listbox =
    {
        // BEGIN SCREEN OBJECT DEFINITION
        308, 217, 280, 137, // position
        FALSE,              // focus
        LIST_BOX,           // type
        TRUE,               // visible
        ListBoxMouseDown,   // OnMouseDown()
        ListBoxMouseUp,     // OnMouseUp()
        ListBoxClick,       // OnClick()
        ListBoxKeyPress,    // OnKeyPress()
        PutListBox,         // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        0,                  // item count
        0,                  // item selected
        0,                  // top_item
        0,                  // display_count
        dev_subliste,       // sub_list
        DisplayDeviceShort, // display function pointer
        dev_listboxOnChange // on change event
};

extern int glb_cat_index;
char* tstat_strings[5] =
    {
        "Off",
        "Heat",
        "Cool",
        "Auto",
        "Deg's"};

int tstat_table[32] =
    {
        0,  // off mode
        1,  // HEAT Mode
        2,  // Cool Mode
        3,  // Auto Mode
        40, //1   scroll bar value
        60, //2
        62, //3
        63, //4
        64, //5
        65, //6
        66, //7
        67, //8
        68, //9
        69, //10
        70, //11
        71, //12
        72, //13
        73, //14
        74, //15
        75, //16
        76, //17
        77, //18
        78, //19
        79, //20
        80, //21
        81, //22
        82, //23
        83, //24
        84, //25
        86, //26
        88, //27
        90  //28
};

int centi_tstat_table[32] =
    {
        0,  // off mode
        1,  // HEAT Mode
        2,  // Cool Mode
        3,  // Auto Mode
        5,  //1   scroll bar value
        6,  //2
        7,  //3
        8,  //4
        9,  //5
        10, //6
        11, //7
        12, //8
        13, //9
        14, //10
        15, //11
        16, //12
        17, //13
        18, //14
        19, //15
        20, //16
        21, //17
        22, //18
        23, //19
        24, //20
        25, //21
        26, //22
        27, //23
        28, //24
        29, //25
        30, //26
        31, //27
        32  //28
};

DEVICE Ethis_device;

void EditEvent(EVENT* event, UCHAR opt, int sender);
void EventAddBtnClick(void);

static void UpdateListBox(void);
static void UpdateDeviceListBox(int cat_index);
static void TypeChange(void);
static void EventDisplayBrightness(UCHAR dim_level);
static void DisplayEvent(EVENT* event, int top, int left, UCHAR color);
static void EventRmvBtnClick(void);
static void EventEdtBtnClick(void);
static void DeviceSetupScreenClick(void);
static void DayTypeClick(void);
static void Mask1Click(void);
static void Mask2Click(void);
static void EventCancelBtnClick(void);
static void EventSaveBtnClick(void);
static void EventOnScroll(void);
static void EventActionDropdownChange(void);
void EventTypeDropdownChange(void);
void EventPMDropdownChange(void);
static void EventTODTextChange(void);
static void OffSetSpinnerChange(void);
static void EventRandomTextChange(void);

void (*ETODTC_addr)() = EventTODTextChange;
void (*ERTC_addr)() = EventRandomTextChange;

void AdjustEvents(int device_slot) {
  int i;
  int j;
  int searching = TRUE;

  while (searching == TRUE) {
    searching = FALSE;
    for (i = 0, j = 0; i < events_used; ++i) {
      if (events[i].device == (USHORT)device_slot) {
        for (j = i; j < events_used; ++j) {
          events[j] = events[j + 1];
        }
        --events_used;
        memset(&events[events_used], 0, sizeof(EVENT));
        searching = TRUE;
        break;
      }
    }
  }
}

static UCHAR displ_event_text[100];
static UCHAR work[100];
static UCHAR devname[100];
void DisplayEvent(EVENT* event, int top, int left, UCHAR color) {
  int i;
  int rc;
  UCHAR* dawn_dusk[2] = {"Dawn", "Dusk"};
  UCHAR* day_type[7] = {"\xA6", "M", "T", "W", "\xAA", "F", "\xE7"};
  UCHAR* mask1[8] = {"W", "A", "\xA6", "\xA7", "C", "H", "N", "V"};
  //{"Winter", "Fall", "Summer", "Spring", "Cool", "Heat", "Normal", "Vacation"};
  UCHAR* mask2[8] = {"N", "N", "A", "H", "L", "M", "H", "C"};
  //{"N/A", "N/A", "A", "H", "Low", "Moderate", "High", "maXimum"};
  UCHAR am_pm[2] = {'a', 'p'};
  UCHAR ap_sel;
  UCHAR hour;
  DEVICE* this_dev;

  memset(work, 0, 100);
  memset(displ_event_text, 0, 100);
  memset(devname, 0, 100);

  // Add device name to the front of the event listing...
  strcat(devname, " ");
  if (dev_listbox.item_selected == 0) // long list with device name
  {
    for (i = 0; i < dev_listbox.item_count; i++) {
      if (event->device == dev_subliste[i]->device_slot) {
        memcpy(devname, dev_subliste[i]->desc, 13);
        this_dev = dev_subliste[i];
      }
    }
  } else {
    memcpy(devname, dev_subliste[dev_listbox.item_selected]->desc, 13);
    this_dev = dev_subliste[dev_listbox.item_selected];
  }
  strcat(devname, ":");

  // Add the time next...
  switch (event->event_type) {
  case 255:
    sprintf(work, "Smart Shopper");
    break;
  case 0: //time of day
    hour = event->hour;
    if (hour > 11) {
      hour -= 12;
      ap_sel = 1;
    } else {
      ap_sel = 0;
    }
    if (hour == 0) {
      hour = 12;
    }
    sprintf(work,
            "TOD %2d:%02d%c",
            hour,
            event->min,
            am_pm[ap_sel]);
    break;
  case 2: //dawn
  case 3: //dusk
    if (event->offset_mins >= 0) {
      sprintf(work,
              "%.4s +%1d:%02d",
              dawn_dusk[event->event_type - 2],
              event->offset_mins / 60,
              event->offset_mins % 60);
    } else {
      sprintf(work,
              "%.4s -%1d:%02d",
              dawn_dusk[event->event_type - 2],
              (-event->offset_mins) / 60,
              (-event->offset_mins) % 60);
    }
    break;
  case 4: //random
    hour = event->base_hour;
    if (hour > 11) {
      hour -= 12;
      ap_sel = 1;
    } else {
      ap_sel = 0;
    }
    if (hour == 0) {
      hour = 12;
    }
    if (event->offset_mins < 60) {
      sprintf(work,
              "RND %2d:%02d%c %c%:%02d",
              hour,
              event->base_min,
              am_pm[ap_sel],
              0xA5,
              event->offset_mins);
    } else {
      sprintf(work,
              "RND %2d:%02d%c%c%1d:%02d",
              hour,
              event->base_min,
              am_pm[ap_sel],
              0xA5,
              event->offset_mins / 60,
              event->offset_mins % 60);
    }
    break;
  case 1: //1 time event (not used)
  default:
    return;
  }
  //%[flags] [width] [.precision] [{F | N | h | l | L}]type
  sprintf(displ_event_text, " %-15s", work);
  memset(work, 0, sizeof(work));

  // add the time to the device name...
  strcat(devname, displ_event_text);

  // if not a smart schedule then add the action to the device name...
  if (event->event_type != 255) {
    rc = IsTstat(this_dev);
    if (rc == X10_RCS_TSTAT) {
      switch (event->action) {
      case TSTAT_OFF:
        sprintf(work, "Off");
        //strcat(displ_event_text, "  Off");
        break;
      case TSTAT_HEAT:
        sprintf(work, "Heat");
        //strcat(displ_event_text, "  Heat");
        break;
      case TSTAT_COOL:
        sprintf(work, "Cool");
        //strcat(displ_event_text, "  Cool");
        break;
      case TSTAT_AUTO:
        sprintf(work, "Auto");
        //strcat(displ_event_text, "  Auto");
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
        sprintf(work, "  %2d%cF", event->action, 0xA4);
        //strcat(displ_event_text, work);
        break;
      default:
        strcpy(work, "Invalid");
        break;
      }
    } else if (rc == X10_RCS_CENTI_TSTAT) {
      switch (event->action) {
      case TSTAT_OFF:
        sprintf(work, "Off");
        //strcat(displ_event_text, "  Off");
        break;
      case TSTAT_HEAT:
        sprintf(work, "Heat");
        //strcat(displ_event_text, "  Heat");
        break;
      case TSTAT_COOL:
        sprintf(work, "Cool");
        //strcat(displ_event_text, "  Cool");
        break;
      case TSTAT_AUTO:
        sprintf(work, "Auto");
        //strcat(displ_event_text, "  Auto");
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
        sprintf(work, "  %2d%cC", event->action, 0xA4);
        //strcat(displ_event_text, work);
        break;
      default:
        strcpy(work, "Invalid");
        break;
      }
    } else {
      switch (event->action) {
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
        sprintf(work, "%.0f%% On", (double)((16.0 - (event->action / 16)) * 6.25));
        //               sprintf(work, "Dim %2d", event->action/16);
        break;
      }
    }
  } else {
    sprintf(work, "N/A");
  }
  sprintf(displ_event_text, " %-7s", work);
  strcat(devname, displ_event_text);

  //modes
  //    if (event->event_type != 255)
  //    {
  strcat(devname, " ");
  // day type
  for (i = 0; i < 7; i++) {
    if (event->type_day & (0x01 << i)) {
      strcat(devname, day_type[i]);
    } else {
      strcat(devname, "_");
    }
  }
  strcat(devname, "   ");
  //UCHAR *mask1[8] =    {"W", "A", "\xA6", "\xA7", "C", "H", "N", "V"};
  // mask 1
  for (i = 4; i < 8; i++) {
    if (event->mask1 & (0x01 << i)) {
      //devname[strlen(devname)] = mask1[i];
      strcat(devname, mask1[i]);
    } else {
      //devname[strlen(devname)] = '_';
      strcat(devname, "_");
    }
    if (i == 5) {
      strcat(devname, "   ");
    }
  }
  if (ADVANCED == TRUE) {
    strcat(devname, "   ");
    for (i = 3; i >= 0; i--) {
      if (event->mask1 & (0x01 << i)) {
        //devname[strlen(devname)] = mask1[i];
        strcat(devname, mask1[i]);
      } else {
        //devname[strlen(devname)] = '_';
        strcat(devname, "_");
      }
    }
    // mask 2
    strcat(devname, "   ");
    for (i = 2; i < 8; i++) {
      if (event->mask2 & (0x01 << i)) {
        //devname[strlen(devname)] = mask2[i];
        strcat(devname, mask2[i]);
        //there's a bug right herererererere
      } else {
        strcat(devname, "_");
      }
      if (i == 3) {
        strcat(devname, "   ");
      }
    }
  }

  //   }

  switch (event->event_type) {
  case 255: // smart scheduled
    //color ^= FG_BLU;
    break;
  case 0: // tod
    //color ^= FG_MAG;
    break;
  case 2:
  case 3: // dawn dusk
    //color ^= FG_GRN;
    break;
  case 4: // random
    //color ^= FG_MAG;
    break;
  }
  ptext(devname, top, left, color);
}

PUSH_BUTTON_T resetbutton =
    { // BEGIN SCREEN OBJECT DEFINITION
        402, 75, 60, 17,
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
        "Reset",      //text
        push,         //type
        up,           //state
        NULL,         //icon
        ResetFilters, //function
        700};

PUSH_BUTTON_T event_menu_btn[7] =
    { // BEGIN SCREEN OBJECT DEFINITION
        285, 547, 70, 25,
        FALSE,        // focus
        PUSH_BUTTON,  // type
        TRUE,         // visible
        BtnMouseDown, // OnMouseDown()
        BtnMouseUp,   // OnMouseUp()
        BtnClick,     // OnClick()
        BtnKeyPress,  // OnKeyPress()
        PutButton,    // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        FALSE,            //default button
        "Add",            //text
        push,             //type
        up,               //state
        NULL,             //icon
        EventAddBtnClick, //function
        112,

        // BEGIN SCREEN OBJECT DEFINITION
        315, 547, 70, 25,
        FALSE,        // focus
        PUSH_BUTTON,  // type
        TRUE,         // visible
        BtnMouseDown, // OnMouseDown()
        BtnMouseUp,   // OnMouseUp()
        BtnClick,     // OnClick()
        BtnKeyPress,  // OnKeyPress()
        PutButton,    // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        FALSE,            //default button
        "Change",         //text
        push,             //type
        up,               //state
        NULL,             //icon
        EventEdtBtnClick, //function
        113,

        // BEGIN SCREEN OBJECT DEFINITION
        345, 547, 70, 25,
        FALSE,        // focus
        PUSH_BUTTON,  // type
        TRUE,         // visible
        BtnMouseDown, // OnMouseDown()
        BtnMouseUp,   // OnMouseUp()
        BtnClick,     // OnClick()
        BtnKeyPress,  // OnKeyPress()
        PutButton,    // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        FALSE,            //default button
        "Remove",         //text
        push,             //type
        up,               //state
        NULL,             //icon
        EventRmvBtnClick, //function
        114,

        // BEGIN SCREEN OBJECT DEFINITION
        59, 6, 109, 15,
        FALSE,        // focus
        PUSH_BUTTON,  // type
        TRUE,         // visible
        BtnMouseDown, // OnMouseDown()
        BtnMouseUp,   // OnMouseUp()
        BtnClick,     // OnClick()
        BtnKeyPress,  // OnKeyPress()
        PutButton,    // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        FALSE,      //default button
        "Device",   //text
        set,        //type
        down,       //state
        NULL,       //icon
        DeviceSort, //function
        115,

        // BEGIN SCREEN OBJECT DEFINITION
        59, 117, 130, 15,
        FALSE,        // focus
        PUSH_BUTTON,  // type
        TRUE,         // visible
        BtnMouseDown, // OnMouseDown()
        BtnMouseUp,   // OnMouseUp()
        BtnClick,     // OnClick()
        BtnKeyPress,  // OnKeyPress()
        PutButton,    // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        FALSE,     //default button
        "Time",    //text
        set,       //type
        up,        //state
        NULL,      //icon
        EventSort, //function
        116,

        // BEGIN SCREEN OBJECT DEFINITION
        432, 22, 166, 20,
        FALSE,        // focus
        PUSH_BUTTON,  // type
        TRUE,         // visible
        BtnMouseDown, // OnMouseDown()
        BtnMouseUp,   // OnMouseUp()
        BtnClick,     // OnClick()
        BtnKeyPress,  // OnKeyPress()
        PutButton,    // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        FALSE,                  //default button
        "Shortcut to Devices",  //text
        push,                   //type
        up,                     //state
        NULL,                   //icon
        DeviceSetupScreenClick, //function
        117,

        // BEGIN SCREEN OBJECT DEFINITION
        419, 535, 89, 33,
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
        118};

// Event type 255 is gonna be used by LaneView
// to spoof the system into thinking
// a smartschedule is an event
EVENT* event_sublist[MAX_EVENTS + MAX_DEVICES]; // lots of buffer space for bogus smart schedule events...
static void list_boxOnChange(void);
static LISTBOX_T list_box =
    {
        // BEGIN SCREEN OBJECT DEFINITION
        76, 5, 611, 185,  // position
        FALSE,            // focus
        LIST_BOX,         // type
        TRUE,             // visible
        ListBoxMouseDown, // OnMouseDown()
        ListBoxMouseUp,   // OnMouseUp()
        ListBoxClick,     // OnClick()
        ListBoxKeyPress,  // OnKeyPress()
        PutListBox,       // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        0,               // item count
        0,               // item selected
        0,               // top_item
        0,               // display_count
        event_sublist,   // sub_list
        DisplayEvent,    // display function pointer
        list_boxOnChange // on change event
};

static void devicesbOnClick(int direction, int how_far, int thumb_pos);
static SCROLL_BAR_T devicesb =
    {
        // BEGIN SCREEN OBJECT DEFINITION
        308, 235 + 260 + 3, 17, 137, // position
        FALSE,                       // focus
        SCROLL_BARV,                 // type
        TRUE,                        // visible
        ScrollBarOnMouseDown,        // OnMouseDown()
        ScrollBarOnMouseUp,          // OnMouseUp()
        ScrollBarOnClick,            // OnClick()
        NULL,                        // OnKeyPress()
        DisplayScrollBar,            // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        0,              // min
        1,              // max
        1,              // small change
        5,              // large change
        0,              // thumb possition
        SB_VERTICAL,    // type
        devicesbOnClick // on scroll function
};

static void devicesbOnClick(int direction, int how_far, int thumb_pos) {
  switch (direction) {
  case SCROLL_UP:
    switch (how_far) {
    case SCROLL_LARGE:
      PageUp(&dev_listbox);
      break;
    case SCROLL_SMALL:
      UpArrow(&dev_listbox);
      break;
    }
    break;
  case SCROLL_DOWN:
    switch (how_far) {
    case SCROLL_LARGE:
      PageDown(&dev_listbox);
      break;
    case SCROLL_SMALL:
      DownArrow(&dev_listbox);
      break;
    }
    break;
  }
  ListBoxScroll(&dev_listbox);
  //PutListBox(&dev_listbox);
  dev_listboxOnChange();
}

static void sbOnClick(int direction, int how_far, int thumb_pos);
static SCROLL_BAR_T sb =
    {
        // BEGIN SCREEN OBJECT DEFINITION
        76, 618, 17, 185,     // position
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
  list_boxOnChange();
}

static char* catlist[6] = {"All", "Lights", "Comfort", "Pool", "Sprinkler", "Security"};
static DROPDOWN_BOX_T ddtype =
    {
        286, 346, 90, 17,  // position
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
        6,                 // item count
        TypeChange         // OnChange
};

static char* daylist[8] = {"All", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
static DROPDOWN_BOX_T ddday =
    {
        284, 102, 90, 17,  // position
        FALSE,             // got_focus
        DROP_DOWN_BOX,     // type
        TRUE,              // visible
        DropDownMouseDown, // OnMouseDown()
        DropDownMouseUp,   // OnMouseUp()
        DropDownClick,     // OnClick()
        DropDownKeyPress,  // OnKeyPress()
        DisplayDropDown,   // DisplayObject()
        daylist,           // strings
        0,                 // item selected
        FALSE,             // dropped
        8,                 // item count
        UpdateListBox      // OnChange
};

static char* HVAClist[3] = {"Both", "Cooling", "Heating"};
static DROPDOWN_BOX_T ddHVAC =
    {
        303, 102, 90, 17,  // position
        FALSE,             // got_focus
        DROP_DOWN_BOX,     // type
        TRUE,              // visible
        DropDownMouseDown, // OnMouseDown()
        DropDownMouseUp,   // OnMouseUp()
        DropDownClick,     // OnClick()
        DropDownKeyPress,  // OnKeyPress()
        DisplayDropDown,   // DisplayObject()
        HVAClist,          // strings
        0,                 // item selected
        FALSE,             // dropped
        3,                 // item count
        UpdateListBox      // OnChange
};

static char* seasonlist[5] = {"All", "Winter", "Autumn", "Summer", "Spring"};
static DROPDOWN_BOX_T ddseason =
    {
        322, 102, 90, 17,  // position
        FALSE,             // got_focus
        DROP_DOWN_BOX,     // type
        TRUE,              // visible
        DropDownMouseDown, // OnMouseDown()
        DropDownMouseUp,   // OnMouseUp()
        DropDownClick,     // OnClick()
        DropDownKeyPress,  // OnKeyPress()
        DisplayDropDown,   // DisplayObject()
        seasonlist,        // strings
        0,                 // item selected
        FALSE,             // dropped
        5,                 // item count
        UpdateListBox      // OnChange
};

static char* costlist[5] = {"All", "Low", "Medium", "High", "Critical"};
static DROPDOWN_BOX_T ddcost =
    {
        341, 102, 90, 17,  // position
        FALSE,             // got_focus
        DROP_DOWN_BOX,     // type
        TRUE,              // visible
        DropDownMouseDown, // OnMouseDown()
        DropDownMouseUp,   // OnMouseUp()
        DropDownClick,     // OnClick()
        DropDownKeyPress,  // OnKeyPress()
        DisplayDropDown,   // DisplayObject()
        costlist,          // strings
        0,                 // item selected
        FALSE,             // dropped
        5,                 // item count
        UpdateListBox      // OnChange
};

static char* systemlist[3] = {"Both", "Normal", "Vacation"};
static DROPDOWN_BOX_T ddsystem =
    {
        360, 102, 90, 17,  // position
        FALSE,             // got_focus
        DROP_DOWN_BOX,     // type
        TRUE,              // visible
        DropDownMouseDown, // OnMouseDown()
        DropDownMouseUp,   // OnMouseUp()
        DropDownClick,     // OnClick()
        DropDownKeyPress,  // OnKeyPress()
        DisplayDropDown,   // DisplayObject()
        systemlist,        // strings
        0,                 // item selected
        FALSE,             // dropped
        3,                 // item count
        UpdateListBox      // OnChange
};

static char* occupiedlist[3] = {"Both", "Away", "At Home"};
static DROPDOWN_BOX_T ddoccupied =
    {
        379, 102, 90, 17,  // position
        FALSE,             // got_focus
        DROP_DOWN_BOX,     // type
        TRUE,              // visible
        DropDownMouseDown, // OnMouseDown()
        DropDownMouseUp,   // OnMouseUp()
        DropDownClick,     // OnClick()
        DropDownKeyPress,  // OnKeyPress()
        DisplayDropDown,   // DisplayObject()
        occupiedlist,      // strings
        0,                 // item selected
        FALSE,             // dropped
        3,                 // item count
        UpdateListBox      // OnChange
};

SCREEN_OBJECT_T* event_menu_so[19];
//UCHAR event_menu_title[100];
static void EventMenuWindowDraw(void);
LWINDOW_T event_menu_window[1] =
    {
        35, 0, 640, 445, //window position
        event_menu_so,   //screen object
        EventMenuWindowDraw,
        19,                //item count
        0,                 //cursor position 0-79 left to right
        0,                 //element that has current focus
        fcsBtn,            //focus type
        "Schedule Events", //event_menu_title,         // title
        DefaultOnKeyPress  //OnKeyPress();
};

DEVICE alldevice;
static int gsender;

void ScheduleEvents(DEVICE* dev, int cat_index, int sender) {
  UCHAR work[100];
  memset(work, 0, 100);
  //memset(event_menu_title, 0, 100);
  memset(dev_subliste, 0, (MAX_DEVICES + 1) * sizeof(DEVICE*));
  memset(&alldevice, 0, sizeof(DEVICE));

  memcpy(alldevice.desc, "ALL DEVICES  ", 13);
  memcpy(alldevice.location, "MY HOME           ", 18);

  if (dawn_dusk_time.dawn_hour == 0) {
    read_dawn_dusk_info((UCHAR)tod->tm_year, (UCHAR)(tod->tm_mon + 1), (UCHAR)tod->tm_mday);
  }

  if (schedule_sort == EVENTSORT) {
    event_menu_btn[4].state = down;
    event_menu_btn[3].state = up;
  } else {
    event_menu_btn[3].state = down;
    event_menu_btn[4].state = up;
  }
  memcpy(&Ethis_device, dev, sizeof(DEVICE));
  glb_cat_index = cat_index;
  ddtype.selected = cat_index;
  gsender = sender;

  event_menu_so[0] = &list_box.so;
  list_box.so.visible = FALSE;
  event_menu_so[1] = &sb.so;

  event_menu_so[2] = &ddday.so;
  event_menu_so[3] = &ddHVAC.so;
  event_menu_so[4] = &ddseason.so;
  event_menu_so[5] = &ddcost.so;
  event_menu_so[6] = &ddsystem.so;
  event_menu_so[7] = &ddoccupied.so;
  event_menu_so[8] = &resetbutton.so;

  event_menu_so[9] = &event_menu_btn[5].so;
  event_menu_so[10] = &ddtype.so;
  event_menu_so[11] = &dev_listbox.so;
  dev_listbox.so.visible = FALSE;
  event_menu_so[12] = &devicesb.so;
  event_menu_so[13] = &event_menu_btn[0].so;
  event_menu_so[14] = &event_menu_btn[1].so;
  event_menu_so[15] = &event_menu_btn[2].so;
  event_menu_so[16] = &event_menu_btn[6].so;
  event_menu_so[17] = &event_menu_btn[3].so;
  event_menu_so[18] = &event_menu_btn[4].so;

  if (ADVANCED == TRUE) {
    ddday.so.visible = TRUE;
    ddHVAC.so.visible = TRUE;
    ddseason.so.visible = TRUE;
    ddcost.so.visible = TRUE;
    ddsystem.so.visible = TRUE;
    ddoccupied.so.visible = TRUE;
    resetbutton.so.visible = TRUE;
  } else {
    ddday.so.visible = FALSE;
    ddHVAC.so.visible = FALSE;
    ddseason.so.visible = FALSE;
    ddcost.so.visible = FALSE;
    ddsystem.so.visible = FALSE;
    ddoccupied.so.visible = FALSE;
    resetbutton.so.visible = FALSE;
  }

  glbWindow = event_menu_window;
  PutWindow(glbWindow);
}

static void EventMenuWindowDraw(void) {
  //ptext("Device", 60, 8, BG_TRANSPARENT + FG_BLK);
  //ptext("Time", 60, 140, BG_TRANSPARENT + FG_BLK);
  if (ADVANCED == TRUE) {
    ptext("Action  Days      HVAC Sys. Season Occ. Cost", 60, 256, BG_WHT + FG_BLK);
    GroupBox("Schedule Filters", 263, 7, 160, 192);
    ptext("Day:", 284 + 2, 70, BG_TRANSPARENT + FG_BLK);
    ptext("HVAC:", 303 + 2, 62, BG_TRANSPARENT + FG_BLK);
    ptext("Season:", 322 + 2, 46, BG_TRANSPARENT + FG_BLK);
    ptext("Cost:", 341 + 2, 62, BG_TRANSPARENT + FG_BLK);
    ptext("System:", 360 + 2, 46, BG_TRANSPARENT + FG_BLK);
    ptext("Occupied:", 379 + 2, 30, BG_TRANSPARENT + FG_BLK);
  } else {
    ptext("Action  Days      HVAC Sys.                 ", 60, 256, BG_WHT + FG_BLK);
  }

  GroupBox("Device Filter & Selection", 263, 208, 192, 318);
  GroupBox("Setup", 263, 535, 120, 95);
  ptext("Category Filter:", 288, 217, BG_TRANSPARENT + FG_BLK);

  list_box.so.visible = TRUE;
  dev_listbox.so.visible = TRUE;
  UpdateDeviceListBox(glb_cat_index);
  UpdateListBox();
}

void EventAddBtnClick(void) {
  EVENT new_event;
  int i;

  memset(&new_event, 0, sizeof(EVENT));
  if (dev_listbox.item_selected == 0) {
    if (list_box.item_count == 0) {
      msgbox("Select a specific device first.", "Add Event to Schedule", MB_OK);
      return;
    }
    for (i = 0; i < dev_listbox.item_count; i++) {
      if (event_sublist[list_box.item_selected]->device == dev_subliste[i]->device_slot) {
        dev_listbox.item_selected = i;
        break;
      }
    }
  }
  memcpy(&Ethis_device, dev_subliste[dev_listbox.item_selected], sizeof(DEVICE));
  new_event.device = dev_subliste[dev_listbox.item_selected]->device_slot;
  //new_event.device = dev_subliste[dev_listbox.item_selected]->device_slot;
  new_event.type_day = 0xFF;
  new_event.mask1 = 0xFF;
  new_event.mask2 = 0xFF;
  new_event.mask3 = 0xFF;

  EditEvent(&new_event, 'A', 0);
}

extern LISTBOX_T list_boxd;
static void DeviceSetupScreenClick(void) {
  int i;
  DEVICE* d1;
  DEVICE* d2;
  if (dev_listbox.item_selected != 0) {
    for (i = 0; i < dev_list.devices_used; i++) {
      d1 = (DEVICE*)list_boxd.sub_list[i];
      d2 = (DEVICE*)dev_listbox.sub_list[dev_listbox.item_selected];
      if (d1->device_slot == d2->device_slot) {
        list_boxd.item_selected = i;
        break;
      }
    }
  } else {
    list_boxd.item_selected = 0;
  }
  Device_Event_Menu(glb_cat_index);
}

void EventRmvBtnClick(void) {
  int result;
  int i;
  DEVICE a_device;

  if (list_box.item_count == 0) {
    msgbox("There are no events to remove!", "Information", MB_OK);
    return;
  }
  if (confirmation == TRUE) {
    result = msgbox("Are you sure you want|to remove this event?",
                    "Caution", MB_YES_NO);
  } else {
    result = MB_RESULT_YES;
  }
  if (result == MB_RESULT_YES) {
    if (event_sublist[list_box.item_selected]->event_type == 255) // if this is a smart scheduled device
    {
      for (i = 0; i < dev_listbox.item_count; i++) // find the device that is refered to...
      {
        if (event_sublist[list_box.item_selected]->device == dev_subliste[i]->device_slot) {
          memcpy(&a_device, dev_subliste[i], sizeof(DEVICE)); // copy it to a temp location
          break;
        }
      }
      a_device.control_type = 0;
      a_device.max_mins_day = 0;
      a_device.cost_day_hour = 0;
      a_device.ss_override = 0;
      memset(a_device.operate_hours, 0, 3);
      if (SendObject(&a_device, 'C', sizeof(DEVICE), DEVICE_TABLE) == FAILURE) {
        msgbox("Unable to change device's|schedule.  Try again.", "Warning", MB_OK);
        return;
      } else {
        i = LookupDev(a_device.device_slot);
        memcpy(&dev_list.devices[i], &a_device, sizeof(DEVICE));
        if (user_backup() == SUCCESS) {
          Status("Smart scheduling disabled.");
          UpdateListBox();
          //ScheduleEvents(&Ethis_device, glb_cat_index, gsender);
        } else {
          msgbox("Error updating file: devices.dat|Changes NOT saved to disk!",
                 "Warning",
                 MB_OK);
        }
      }
    } else {
      if (RemoveEvent(event_sublist[list_box.item_selected]) == SUCCESS) {
        Status("Event successfully removed.");
        UpdateListBox();
        //ScheduleEvents(&Ethis_device, glb_cat_index, gsender);
      } else {
        msgbox("Unable to remove event.|Please try again.", "Warning", MB_OK);
      }
    }
  }
}

void EventEdtBtnClick(void) {
  int i;

  if (list_box.item_count > 0) {
    if (event_sublist[list_box.item_selected]->event_type == 255) {
      for (i = 0; i < dev_listbox.item_count; i++) {
        if (event_sublist[list_box.item_selected]->device == dev_subliste[i]->device_slot) {
          SmartSchedule(dev_subliste[i], glb_cat_index);
          break;
        }
      }
    } else {
      EditEvent(event_sublist[list_box.item_selected], 'C', 0);
    }
  } else {
    msgbox("There are no events to remove!", "Information", MB_OK);
  }
}

////////////////////////////////////////////////////////////////////
//
// Edit Event Screen Definition Starts Here 1.4
//
////////////////////////////////////////////////////////////////////
CHECK_BOX_T cbmask1[8] =
    {
        366, 434, 0, 0,      // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Winter",
        FALSE,
        Mask1Click,

        346, 434, 0, 0,      // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Autumn",
        FALSE,
        Mask1Click,

        326, 434, 0, 0,      // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Summer",
        FALSE,
        Mask1Click,

        306, 434, 0, 0,      // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Spring",
        FALSE,
        Mask1Click,

        167, 434, 0, 0,      // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Cooling",
        FALSE,
        Mask1Click,

        186, 434, 0, 0,      // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Heating",
        FALSE,
        Mask1Click,

        102, 434, 0, 0,      // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Normal",
        TRUE,
        Mask1Click,

        121, 434, 0, 0,      // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Vacation",
        FALSE,
        Mask1Click};

CHECK_BOX_T cbmask2[8] =
    {
        0, 0, 0, 0,          // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        FALSE,               // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "NA 1",
        FALSE,
        Mask2Click,

        50, 0, 0, 0,         // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        FALSE,               // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "NA 2",
        FALSE,
        Mask2Click,

        251, 434, 0, 0,      // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Temp Away",
        FALSE,
        Mask2Click,

        232, 434, 0, 0,      // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "At Home",
        FALSE,
        Mask2Click,

        366, 299, 0, 0,      // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Low",
        FALSE,
        Mask2Click,

        346, 299, 0, 0,      // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Medium",
        FALSE,
        Mask2Click,

        326, 299, 0, 0,      // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "High",
        FALSE,
        Mask2Click,

        306, 299, 0, 0,      // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Critical",
        TRUE,
        Mask2Click};

CHECK_BOX_T cbday_type[7] =
    {
        113, 299, 0, 0,      // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Sunday",
        TRUE,
        DayTypeClick, //SundayClick,

        133, 299, 0, 0,      // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Monday",
        FALSE,
        DayTypeClick, //MondayClick,

        153, 299, 0, 0,      // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Tuesday",
        TRUE,
        DayTypeClick, //TuesdayClick,

        173, 299, 0, 0,      // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Wednesday",
        FALSE,
        DayTypeClick, //WednesdayClick,

        193, 299, 0, 0,      // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Thursday",
        TRUE,
        DayTypeClick, //ThursdayClick,

        213, 299, 0, 0,      // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Friday",
        FALSE,
        DayTypeClick, //FridayClick,

        233, 299, 0, 0,      // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Saturday",
        FALSE,
        DayTypeClick, //SaturdayClick
};

static UCHAR* e_action_strings[3] = {"Off", "On", "Dim"};
static UCHAR* event_type_strings[5] = {"Time of Day",
                                       "Dawn",
                                       "Dusk",
                                       "Random",
                                       "Smart Shopper"};

extern DROPDOWN_BOX_T smart_dropdown;
DROPDOWN_BOX_T ddevent_type =
    {
        129, 112, 146, 17,
        FALSE,                  // got_focus
        DROP_DOWN_BOX,          // type
        TRUE,                   // visible
        DropDownMouseDown,      // OnMouseDown()
        DropDownMouseUp,        // OnMouseUp()
        DropDownClick,          // OnClick()
        DropDownKeyPress,       // OnKeyPress()
        DisplayDropDown,        // DisplayObject()
        event_type_strings,     // strings
        0,                      // item selected
        FALSE,                  // dropped
        5,                      // item count
        EventTypeDropdownChange // OnChange
};

static DROPDOWN_BOX_T ddevent_action =
    {
        169, 112, 146, 17,
        FALSE,                    // got_focus
        DROP_DOWN_BOX,            // type
        TRUE,                     // visible
        DropDownMouseDown,        // OnMouseDown()
        DropDownMouseUp,          // OnMouseUp()
        DropDownClick,            // OnClick()
        DropDownKeyPress,         // OnKeyPress()
        DisplayDropDown,          // DisplayObject()
        e_action_strings,         // strings
        0,                        // item selected
        FALSE,                    // dropped
        3,                        // item count
        EventActionDropdownChange // OnChange
};

char* plus_minus_strings[2] = {"+", "-"};

DROPDOWN_BOX_T ddplusminus =
    {
        310, 112, 30, 17,     //310, 112, 146, 17,
        FALSE,                // got_focus
        DROP_DOWN_BOX,        // type
        TRUE,                 // visible
        DropDownMouseDown,    // OnMouseDown()
        DropDownMouseUp,      // OnMouseUp()
        DropDownClick,        // OnClick()
        DropDownKeyPress,     // OnKeyPress()
        DisplayDropDown,      // DisplayObject()
        plus_minus_strings,   // strings
        0,                    // item selected
        FALSE,                // dropped
        2,                    // item count
        EventPMDropdownChange // OnChange
};

PUSH_BUTTON_T eventbtn[2] =
    {
        // BEGIN SCREEN OBJECT DEFINITION
        419, 446, 89, 33,
        FALSE,        // focus
        PUSH_BUTTON,  // type
        TRUE,         // visible
        BtnMouseDown, // OnMouseDown()
        BtnMouseUp,   // OnMouseUp()
        BtnClick,     // OnClick()
        BtnKeyPress,  // OnKeyPress()
        PutButton,    // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        FALSE,             //default button
        "Apply",           //text
        push,              //type
        up,                //state
        NULL,              //icon
        EventSaveBtnClick, //function
        119,

        // BEGIN SCREEN OBJECT DEFINITION
        419, 545, 89, 33,
        FALSE,        // focus
        PUSH_BUTTON,  // type
        TRUE,         // visible
        BtnMouseDown, // OnMouseDown()
        BtnMouseUp,   // OnMouseUp()
        BtnClick,     // OnClick()
        BtnKeyPress,  // OnKeyPress()
        PutButton,    // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        FALSE,               //default button
        "Cancel",            //text
        push,                //type
        up,                  //state
        NULL,                //icon
        EventCancelBtnClick, //function
        120};

SCROLL_BAR_T event_scroll_bar =
    {
        // BEGIN SCREEN OBJECT DEFINITION
        208, 112, 146, 17,    // position
        FALSE,                // focus
        SCROLL_BARH,          // type
        FALSE,                // visible
        ScrollBarOnMouseDown, // OnMouseDown()
        ScrollBarOnMouseUp,   // OnMouseUp()
        ScrollBarOnClick,     // OnClick()
        NULL,                 // OnKeyPress()
        DisplayScrollBar,     // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        0,             // min
        15,            // max
        1,             // small change
        5,             // large change
        0,             // thumb position
        SB_HORIZONTAL, // type of scroll bar
        EventOnScroll  // on scroll event
};

static TIME_ENTRY_T get_tod =
    {
        266, 112, 0, 0,     // location
        FALSE,              // focus
        TIME_ENTRY,         // type
        TRUE,               // visible
        TimeEntryMouseDown, // OnMouseDown()
        TimeEntryMouseUp,   // OnMouseUp()
        TimeEntryClick,     // OnClick()
        TimeEntryKeyPress,  // OnKeyPress()
        DisplayTimeEntry,   // DisplayObject()
        time_ent            // type
};

static TIME_ENTRY_T get_basetime =
    {
        266, 112, 0, 0,     // location
        FALSE,              // focus
        TIME_ENTRY,         // type
        TRUE,               // visible
        TimeEntryMouseDown, // OnMouseDown()
        TimeEntryMouseUp,   // OnMouseUp()
        TimeEntryClick,     // OnClick()
        TimeEntryKeyPress,  // OnKeyPress()
        DisplayTimeEntry,   // DisplayObject()
        time_ent            // type
};

SPINNER_T hr_offsetspin =
    {
        310, 148, 1, 1,   // position      // was 310,112,....
        FALSE,            //focus
        TIME_ENTRY,       //type
        TRUE,             // visible
        SpinnerMouseDown, //OnMouseDown()
        SpinnerMouseUp,   //OnMouseUp()
        SpinnerClick,     // OnClick()
        SpinnerKeyPress,  // OnKeyPress()
        DisplaySpinner,   // DisplayObject()
        0,                //value
        9,                //max
        0,                //min
        1,                //increment
        OffSetSpinnerChange};

SPINNER_T offsetspin =
    {
        310, 205, 1, 1,   // position      // was 310,112,....
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
        OffSetSpinnerChange};

SCREEN_OBJECT_T* event_so[34];

UCHAR event_title[150];
static void EventWindowDraw(void);
LWINDOW_T event_window[1] = {
    35, 0, 640, 445, //window position
    event_so,        //screen object
    EventWindowDraw,
    34,               //item count
    0,                //cursor position 0-79 left to right
    0,                //element that has current focus
    fcsBtn,           //focus type
    event_title,      //title
    DefaultOnKeyPress //OnKeyPress();
};

EVENT this_event;
int event_opt;

#define TE_AM 0
#define TE_PM 1
void EditEvent(EVENT* event, UCHAR opt, int sender) {
  int j;
  int i;
  int hour;
  int rc;

  event_opt = opt;
  memcpy(&this_event, event, sizeof(EVENT));

  get_tod.hour = 12;
  get_tod.minute = 30;
  get_tod.am_pm = 0;
  get_tod.OnChange = ETODTC_addr;
  get_basetime.hour = 12;
  get_basetime.minute = 30;
  get_basetime.am_pm = 0;
  get_basetime.OnChange = ERTC_addr;

  if (opt == 'A') {
    sprintf(event_title,
            "Add Event for: %.13s %.18s",
            dev_subliste[dev_listbox.item_selected]->desc,
            dev_subliste[dev_listbox.item_selected]->location);
  } else // == 'C'
  {
    for (i = 0; i < dev_listbox.item_count; i++) {
      if (event_sublist[list_box.item_selected]->device == dev_subliste[i]->device_slot) {
        sprintf(event_title, "Change Event for: %.13s %.18s", dev_subliste[i]->desc, dev_subliste[i]->location);
        memcpy(&Ethis_device, dev_subliste[i], sizeof(DEVICE));
        break;
      }
    }
  }

  memset(work, 0, 100);

  if (ADVANCED == TRUE) {
    cbmask2[7].so.visible = TRUE; // maximum
    cbmask2[6].so.visible = TRUE; // high
    cbmask2[5].so.visible = TRUE; // medium
    cbmask2[4].so.visible = TRUE; // low

    cbmask2[3].so.visible = TRUE; // at home
    cbmask2[2].so.visible = TRUE; // temp away

    cbmask1[3].so.visible = TRUE; // spring
    cbmask1[2].so.visible = TRUE; // summer
    cbmask1[1].so.visible = TRUE; // autumn
    cbmask1[0].so.visible = TRUE; // winter
  } else {
    cbmask2[7].so.visible = FALSE; // maximum
    cbmask2[6].so.visible = FALSE; // high
    cbmask2[5].so.visible = FALSE; // medium
    cbmask2[4].so.visible = FALSE; // low

    cbmask2[3].so.visible = FALSE; // at home
    cbmask2[2].so.visible = FALSE; // temp away

    cbmask1[3].so.visible = FALSE; // spring
    cbmask1[2].so.visible = FALSE; // summer
    cbmask1[1].so.visible = FALSE; // autumn
    cbmask1[0].so.visible = FALSE; // winter
  }

  event_so[0] = &ddevent_type.so;
  event_so[1] = &ddevent_action.so;
  event_so[2] = &event_scroll_bar.so;
  event_so[3] = &get_tod.so;
  event_so[4] = &get_basetime.so;
  event_so[5] = &ddplusminus.so;
  event_so[6] = &hr_offsetspin.so;
  event_so[7] = &offsetspin.so;

  event_so[8] = &cbday_type[0].so;
  event_so[9] = &cbday_type[1].so;
  event_so[10] = &cbday_type[2].so;
  event_so[11] = &cbday_type[3].so;
  event_so[12] = &cbday_type[4].so;
  event_so[13] = &cbday_type[5].so;
  event_so[14] = &cbday_type[6].so;
  event_so[15] = &cbday_type[7].so;

  event_so[16] = &cbmask2[7].so; // maximum
  event_so[17] = &cbmask2[6].so; // high
  event_so[18] = &cbmask2[5].so; // medium
  event_so[19] = &cbmask2[4].so; // low

  event_so[20] = &cbmask1[6].so; // normal
  event_so[21] = &cbmask1[7].so; // vacation

  event_so[22] = &cbmask1[4].so; // cooling
  event_so[23] = &cbmask1[5].so; // heating

  event_so[24] = &cbmask2[3].so; // at home
  event_so[25] = &cbmask2[2].so; // temp away

  event_so[26] = &cbmask1[3].so; // spring
  event_so[27] = &cbmask1[2].so; // summer
  event_so[28] = &cbmask1[1].so; // autumn
  event_so[29] = &cbmask1[0].so; // winter

  event_so[30] = &cbmask2[0].so; // n/a 1
  event_so[31] = &cbmask2[1].so; // n/a 2

  event_so[32] = &eventbtn[0].so;
  event_so[33] = &eventbtn[1].so;

  //==============================
  if (dawn_dusk_time.dawn_hour == 0) {
    read_dawn_dusk_info((UCHAR)tod->tm_year, (UCHAR)(tod->tm_mon + 1), (UCHAR)tod->tm_mday);
  }
  //==============================

  switch (this_event.event_type) {
  case 0: // TOD
  case 1: // 1 time only - not supported
    if (sender == 0) {
      ddevent_type.selected = 0;
    } else {
      ddevent_type.selected = smart_dropdown.selected;
    }
    get_tod.hour = this_event.hour;
    if (get_tod.hour > 11) {
      get_tod.hour -= 12;
      get_tod.am_pm = TE_PM;
    } else {
      get_tod.am_pm = TE_AM;
    }
    if (get_tod.hour == 0) {
      get_tod.hour = 12;
    }
    get_tod.minute = this_event.min;
    get_tod.so.visible = TRUE;
    hr_offsetspin.so.visible = FALSE;
    offsetspin.so.visible = FALSE;
    ddplusminus.so.visible = FALSE;
    get_basetime.so.visible = FALSE;
    break;
  case 2: //dawn
  case 3: //dusk
    hr_offsetspin.min = 0;
    offsetspin.min = 0; // -59
    offsetspin.value = 0;
    ddplusminus.selected = 0;
    get_tod.so.visible = FALSE;
    hr_offsetspin.so.visible = TRUE;
    offsetspin.so.visible = TRUE;
    ddplusminus.so.visible = TRUE;
    get_basetime.so.visible = FALSE;

    if (this_event.offset_mins >= 0) {
      ddplusminus.selected = 0;
      hr_offsetspin.value = this_event.offset_mins / 60;
      offsetspin.value = this_event.offset_mins % 60; //this_event.offset_mins;
    } else {
      ddplusminus.selected = 1;
      hr_offsetspin.value = (-this_event.offset_mins) / 60;
      offsetspin.value = (-this_event.offset_mins) % 60; //this_event.offset_mins;
    }
    if (sender == 0) {
      ddevent_type.selected = this_event.event_type - 1;
    } else {
      ddevent_type.selected = smart_dropdown.selected;
    }

    if (dawn_dusk_time.dawn_hour != 0) // if read didn't fail
    {
      sprintf(work,
              "Dawn: %2d:%02d am - Dusk: %2d:%02d pm",
              dawn_dusk_time.dawn_hour,
              dawn_dusk_time.dawn_min,
              dawn_dusk_time.dusk_hour - 12,
              dawn_dusk_time.dusk_min);
    }
    break;
  case 4: //random
    hr_offsetspin.min = 0;
    offsetspin.min = 0;
    ddplusminus.selected = 0;
    hr_offsetspin.value = this_event.offset_mins / 60;
    offsetspin.value = this_event.offset_mins % 60; //this_event.offset_mins;
    get_tod.so.visible = FALSE;
    hr_offsetspin.so.visible = TRUE;
    offsetspin.so.visible = TRUE;
    ddplusminus.so.visible = FALSE;
    get_basetime.so.visible = TRUE;

    if (sender == 0) {
      ddevent_type.selected = this_event.event_type - 1;
    } else {
      ddevent_type.selected = smart_dropdown.selected;
    }
    get_basetime.hour = event->base_hour;
    get_basetime.minute = event->base_min;
    if (get_basetime.hour > 11) {
      get_basetime.hour -= 12;
      get_basetime.am_pm = TE_PM;
    } else {
      get_basetime.am_pm = TE_AM;
    }
    if (hour == 0) {
      get_basetime.hour = 12;
    }
    // offset
    hr_offsetspin.value = this_event.offset_mins / 60;
    offsetspin.value = this_event.offset_mins % 60;
    //offsetspin.value = this_event.offset_mins;
    break;
  }

  rc = IsTstat(&Ethis_device);
  if (rc == X10_RCS_TSTAT) {
    event_scroll_bar.min = 0;
    event_scroll_bar.max = 27;
    event_scroll_bar.small_chg = 1;
    event_scroll_bar.large_chg = 7;

    ddevent_action.strings = tstat_strings;
    ddevent_action.count = 5;
    switch (this_event.action) {
    case TSTAT_OFF:
      ddevent_action.selected = 0;
      event_scroll_bar.so.visible = FALSE;
      break;
    case TSTAT_HEAT:
      ddevent_action.selected = 1;
      event_scroll_bar.so.visible = FALSE;
      break;
    case TSTAT_COOL:
      ddevent_action.selected = 2;
      event_scroll_bar.so.visible = FALSE;
      break;
    case TSTAT_AUTO:
      ddevent_action.selected = 3;
      event_scroll_bar.so.visible = FALSE;
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
      ddevent_action.selected = 4;
      event_scroll_bar.so.visible = TRUE;
      break;
    }
  } else if (rc == X10_RCS_CENTI_TSTAT) {
    event_scroll_bar.min = 0;
    event_scroll_bar.max = 27;
    event_scroll_bar.small_chg = 1;
    event_scroll_bar.large_chg = 7;

    ddevent_action.strings = tstat_strings;
    ddevent_action.count = 5;
    switch (this_event.action) {
    case TSTAT_OFF:
      ddevent_action.selected = 0;
      event_scroll_bar.so.visible = FALSE;
      break;
    case TSTAT_HEAT:
      ddevent_action.selected = 1;
      event_scroll_bar.so.visible = FALSE;
      break;
    case TSTAT_COOL:
      ddevent_action.selected = 2;
      event_scroll_bar.so.visible = FALSE;
      break;
    case TSTAT_AUTO:
      ddevent_action.selected = 3;
      event_scroll_bar.so.visible = FALSE;
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
      ddevent_action.selected = 4;
      event_scroll_bar.so.visible = TRUE;
      break;
    }
  } else {
    ddevent_action.strings = e_action_strings;
    ddevent_action.count = 3;
    event_scroll_bar.min = 0;
    event_scroll_bar.max = 15;
    event_scroll_bar.small_chg = 1;
    event_scroll_bar.large_chg = 5;
    switch (this_event.action) {
    case EVNT_OFF:
      ddevent_action.selected = 0;
      event_scroll_bar.so.visible = FALSE;
      break;
    case EVNT_ON:
      ddevent_action.selected = 1;
      event_scroll_bar.so.visible = FALSE;
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
      ddevent_action.selected = 2;
      event_scroll_bar.so.visible = TRUE;
      break;
    }
  }

  // set day types to current event
  for (j = 0; j < 7; j++) {
    if ((1 << j) & this_event.type_day) {
      cbday_type[j].checked = TRUE;
    } else {
      cbday_type[j].checked = FALSE;
    }
  }

  //set mode mask 1 & 2 to current event
  for (j = 0; j < 8; j++) {
    if ((1 << j) & this_event.mask1) {
      cbmask1[j].checked = TRUE;
    } else {
      cbmask1[j].checked = FALSE;
    }
    if ((1 << j) & this_event.mask2) {
      cbmask2[j].checked = TRUE;
    } else {
      cbmask2[j].checked = FALSE;
    }
  }

  glbWindow = event_window;
  PutWindow(glbWindow);
}

static void EventWindowDraw(void) {
  if (event_scroll_bar.so.visible == TRUE) {
    EventDisplayBrightness(this_event.action);
  }
  // GroupBox(UCHAR *text, int top, int left, int height, int width)
  GroupBox("Event Definition", 83, 92, 311, 185);
  GroupBox("Days", 83, 282, 190, 130);
  GroupBox("Master Schedule", 83, 417, 60, 130);
  GroupBox("HVAC Schedule", 148, 417, 60, 130);
  if (ADVANCED == TRUE) {
    GroupBox("Cost Schedule", 278, 282, 116, 130);
    GroupBox("Season Schedule", 278, 417, 116, 130);
    GroupBox("Occpd Schedule", 213, 417, 60, 130);
  }

  //time box labels
  switch (this_event.event_type) {
  case 0: // TOD
  case 1: // one time only - not used
    ptext("Time         ", 252, 112, BG_WHT + FG_BLK);
    break;
  case 2: // dawn
  case 3: // dusk
          //         ptext("Offset (minutes)", 296, 112, BG_WHT + FG_BLK);
    ptext("Offset", 296, 112, BG_WHT + FG_BLK);
    ptext(":", 310, 197, BG_WHT + FG_BLK);
    break;
  case 4: // random
    ptext("Base Time", 252, 112, BG_WHT + FG_BLK);
    //         ptext("Offset (minutes)", 296, 112, BG_WHT + FG_BLK);
    ptext("Offset", 296, 112, BG_WHT + FG_BLK);
    ptext(":", 310, 197, BG_WHT + FG_BLK);
    break;
  }
  Status(work);
}

// normal vacation change for 1.4
void DayTypeClick(void) {
  int i;
  for (i = 0; i < 7; i++) {
    if (cbday_type[i].checked == TRUE) {
      if (!(this_event.type_day & (0x01 << i))) // the bit is turned off. so now turn it on.
      {
        this_event.type_day ^= (0x01 << i); // toggle the bit on
      }
    } else {
      if (this_event.type_day & (0x01 << i)) // the bit is turned on. so now turn it off.
      {
        this_event.type_day ^= (0x01 << i); // toggle the bit off
      }
    }
  }
}

void Mask1Click(void) {
  int i;
  for (i = 0; i < 8; i++) {
    if (cbmask1[i].checked == TRUE) {
      if (!(this_event.mask1 & (0x01 << i))) // the bit is turned off. so now turn it on.
      {
        this_event.mask1 ^= (0x01 << i); // toggle the bit on
      }
    } else {
      if (this_event.mask1 & (0x01 << i)) // the bit is turned on. so now turn it off.
      {
        this_event.mask1 ^= (0x01 << i); // toggle the bit off
      }
    }
  }
}

void Mask2Click(void) {
  int i;
  for (i = 0; i < 8; i++) {
    if (cbmask2[i].checked == TRUE) {
      if (!(this_event.mask2 & (0x01 << i))) // the bit is turned off. so now turn it on.
      {
        this_event.mask2 ^= (0x01 << i); // toggle the bit on
      }
    } else {
      if (this_event.mask2 & (0x01 << i)) // the bit is turned on. so now turn it off.
      {
        this_event.mask2 ^= (0x01 << i); // toggle the bit off
      }
    }
  }
}

void EventTypeDropdownChange(void) {
  memset(work, 0, 100);
  area_clear(250,
             390,
             105,
             270,
             FG_WHT);
  switch (ddevent_type.selected) {
  case 0: //TOD
    get_tod.so.visible = TRUE;
    hr_offsetspin.so.visible = FALSE;
    offsetspin.so.visible = FALSE;
    ddplusminus.so.visible = FALSE;
    get_basetime.so.visible = FALSE;
    DisplayTimeEntry(&get_tod);
    this_event.event_type = ddevent_type.selected;
    ptext("Time         ", 252, 112, BG_WHT + FG_BLK);
    break;
  case 1: // dawn
  case 2: // dusk
    hr_offsetspin.min = 0;
    offsetspin.min = 0;
    //offsetspin.min = -59;
    hr_offsetspin.value = 0;
    offsetspin.value = 0;
    get_tod.so.visible = FALSE;
    hr_offsetspin.so.visible = TRUE;
    offsetspin.so.visible = TRUE;
    ddplusminus.so.visible = TRUE;
    DisplayDropDown(&ddplusminus);
    get_basetime.so.visible = FALSE;
    DisplaySpinner(&hr_offsetspin);
    DisplaySpinner(&offsetspin);
    this_event.event_type = ddevent_type.selected + 1;
    //         ptext("Offset (minutes)", 296, 112, BG_WHT + FG_BLK);
    ptext("Offset", 296, 112, BG_WHT + FG_BLK);
    ptext(":", 310, 197, BG_WHT + FG_BLK);
    // get dawn dusk time from ICU
    if (dawn_dusk_time.dawn_hour == 0) {
      read_dawn_dusk_info((UCHAR)tod->tm_year, (UCHAR)(tod->tm_mon + 1), (UCHAR)tod->tm_mday);
    }
    if (dawn_dusk_time.dawn_hour != 0) // if read didn't fail
    {
      sprintf(work,
              "Dawn: %2d:%02d am - Dusk: %2d:%02d pm",
              dawn_dusk_time.dawn_hour,
              dawn_dusk_time.dawn_min,
              dawn_dusk_time.dusk_hour - 12,
              dawn_dusk_time.dusk_min);
    }
    break;
  case 3: // random
    hr_offsetspin.min = 0;
    offsetspin.min = 0;
    //hr_offsetspin.value = 0;
    //offsetspin.value = 0;
    get_tod.so.visible = FALSE;
    hr_offsetspin.so.visible = TRUE;
    offsetspin.so.visible = TRUE;
    ddplusminus.so.visible = FALSE;
    get_basetime.so.visible = TRUE;

    DisplaySpinner(&hr_offsetspin);
    DisplaySpinner(&offsetspin);
    DisplayTimeEntry(&get_basetime);
    this_event.event_type = ddevent_type.selected + 1;
    ptext("Base Time  ", 252, 112, BG_WHT + FG_BLK);
    //         ptext("Offset (minutes)", 296, 112, BG_WHT + FG_BLK);
    ptext("Offset", 296, 112, BG_WHT + FG_BLK);
    ptext(":", 310, 197, BG_WHT + FG_BLK);
    break;
  case 4:
    memset(&Ethis_device.operate_hours, 0xFF, 3);
    SmartSchedule(&Ethis_device, glb_cat_index);
    break;
  }
  Status(work);
}

void EventPMDropdownChange(void) {
  memset(work, 0, 100);
  switch (ddplusminus.selected) {
  case 0: // Plus
    this_event.offset_mins = offsetspin.value + 60 * hr_offsetspin.value;
    break;
  case 1: // Minus
    this_event.offset_mins = -offsetspin.value - (60 * hr_offsetspin.value);
    break;
  }
  Status(work);
}

void EventDisplayBrightness(UCHAR dim_level) {
  char deg[20];
  int i;
  int rc;

  event_scroll_bar.so.visible = TRUE;
  rc = IsTstat(&Ethis_device);
  if (rc == X10_RCS_TSTAT) {
    for (i = 4; i < 32; i++) {
      if (tstat_table[i] == dim_level) {
        dim_level = i;
        break;
      }
    }
    event_scroll_bar.thumb_pos = dim_level - 4;
    DisplayScrollBar(&event_scroll_bar);
    ptext("Cooler       Warmer", 228, 110, BG_TRANSPARENT + FG_BLK);
    sprintf(deg, "%2d%cF  ", tstat_table[event_scroll_bar.thumb_pos + 4], 0xA4);
    ptext(deg, 193, 112, BG_WHT + FG_BLK);
  } else if (rc == X10_RCS_CENTI_TSTAT) {
    for (i = 4; i < 32; i++) {
      if (centi_tstat_table[i] == dim_level) {
        dim_level = i;
        break;
      }
    }
    event_scroll_bar.thumb_pos = dim_level - 4;
    DisplayScrollBar(&event_scroll_bar);
    ptext("Cooler       Warmer", 228, 110, BG_TRANSPARENT + FG_BLK);
    sprintf(deg, "%2d%cC  ", centi_tstat_table[event_scroll_bar.thumb_pos + 4], 0xA4);
    ptext(deg, 193, 112, BG_WHT + FG_BLK);
  } else {
    event_scroll_bar.thumb_pos = dim_level >> 4;
    DisplayScrollBar(&event_scroll_bar);
    ptext("Bright          Dim", 228, 110, BG_TRANSPARENT + FG_BLK);
    sprintf(deg, "%.0f%% Bright   ",
            (double)(16 - event_scroll_bar.thumb_pos) * 6.25);
    //((15 - event_scroll_bar.thumb_pos + 1)/(15.0)) * 93.46);

    ptext(deg, 193, 112, BG_WHT + FG_BLK);
  }
}

void EventActionDropdownChange(void) {
  int i;
  int rc;

  rc = IsTstat(&Ethis_device);
  if (rc == X10_RCS_TSTAT) {
    switch (ddevent_action.selected) {
    case TSTAT_OFF:  //off
    case TSTAT_HEAT: //heat
    case TSTAT_COOL: //cool
    case TSTAT_AUTO: //auto
      this_event.action = ddevent_action.selected;
      event_scroll_bar.so.visible = FALSE;
      area_clear(191,
                 241,
                 107,
                 271,
                 FG_WHT);
      break;
    case 4: //Deg F
      this_event.action = TSTAT_70;
      EventDisplayBrightness(this_event.action);
      break;
    }
  } else if (rc == X10_RCS_CENTI_TSTAT) {
    switch (ddevent_action.selected) {
    case TSTAT_OFF:  //off
    case TSTAT_HEAT: //heat
    case TSTAT_COOL: //cool
    case TSTAT_AUTO: //auto
      this_event.action = ddevent_action.selected;
      event_scroll_bar.so.visible = FALSE;
      area_clear(191,
                 241,
                 107,
                 271,
                 FG_WHT);
      break;
    case 4: //Deg C
      this_event.action = TSTAT_21;
      EventDisplayBrightness(this_event.action);
      break;
    }
  } else {
    switch (ddevent_action.selected) {
    case 0: //off
    case 1: //on
      this_event.action = ddevent_action.selected;
      event_scroll_bar.so.visible = FALSE;
      area_clear(191,
                 241,
                 107,
                 271,
                 FG_WHT);
      break;
    case 2: //DIM
      i = LookupDev(this_event.device);
      if (IsDimmer(&dev_list.devices[i]) == TRUE) {
        this_event.action = MD_DIM0;
        EventDisplayBrightness(this_event.action);
      } else {
        msgbox("This device is not a dimmer.", "Information", MB_OK);
        ddevent_action.selected = this_event.action = MD_OFF;
        DisplayDropDown(&ddevent_action);
        event_scroll_bar.so.visible = FALSE;
        area_clear(191,
                   241,
                   107,
                   271,
                   FG_WHT);
      }
      break;
    }
  }
}

void EventCancelBtnClick(void) {
  if (dev_listbox.item_selected == 0) {
    ScheduleEvents(NULL, glb_cat_index, gsender);
  } else {
    ScheduleEvents(&Ethis_device, glb_cat_index, gsender);
  }
}

void EventSaveBtnClick(void) {
  UCHAR action;
  int i;

  Status("Please wait...");
  if (dev_subliste[dev_listbox.item_selected]->control_type > 0) // doing a smart shopper...
  {
    dev_subliste[dev_listbox.item_selected]->control_type = 0;
    if (SendObject(dev_subliste[dev_listbox.item_selected], 'C', sizeof(DEVICE), DEVICE_TABLE) == FAILURE) {
      Status("");
      msgbox("Unable to change schedule.  Try again.", "Warning", MB_OK);
      return;
    } else {
      i = LookupDev(Ethis_device.device_slot);
      memcpy(&dev_list.devices[i], dev_subliste[dev_listbox.item_selected], sizeof(DEVICE));
      if (user_backup() == SUCCESS) {
        Status("Smart shopper disabled.");
        Wait(18L); //wait before trying to send more stuff to the ICU
      } else {
        Status("");
        msgbox("Error updating file:  devices.dat|Changes NOT saved to disk!",
               "Warning",
               MB_OK);
      }
    }
  }

  action = this_event.action;
  EventRandomTextChange();
  OffSetSpinnerChange();
  EventTODTextChange();
  EventActionDropdownChange();
  EventTypeDropdownChange();
  this_event.action = action;

  if (SendObject(&this_event, event_opt, sizeof(EVENT), EVENT_TABLE) == SUCCESS) {
    if (event_opt == 'A') {
      memcpy(&events[events_used], &this_event, sizeof(EVENT));
      events_used++;
    } else {
      memcpy(event_sublist[list_box.item_selected], &this_event, sizeof(EVENT));
    }
    //InsertEvent(&this_event);
    if (user_backup() != SUCCESS) {
      Status("");
      msgbox("Unable to save event.|Check disk space.", "Warning", MB_OK);
      return;
    }
  } else {
    Status("");
    msgbox("Unable to save event.|Please try again.", "Warning", MB_OK);
    return;
  }

#if 0 // don't reset filter, give the operator a chance!
   // Reset all the filters so the user
   // won't think that the event didn't get saved
   // or doesn't exist
   ddday.selected = 0;
   ddHVAC.selected = 0;
   ddseason.selected = 0;
   ddcost.selected = 0;
   ddsystem.selected = 0;
   ddoccupied.selected = 0;
#endif

  if (dev_listbox.item_selected == 0) {
    ScheduleEvents(NULL, glb_cat_index, gsender);
    Status("Event successfully saved.");
  } else {
    ScheduleEvents(dev_subliste[dev_listbox.item_selected], glb_cat_index, gsender);
    Status("Event successfully saved.");
  }
}

void EventOnScroll(void) {
  char deg[20];
  int rc;

  rc = IsTstat(&Ethis_device);
  if (rc == X10_RCS_TSTAT) {
    this_event.action = tstat_table[event_scroll_bar.thumb_pos + 4];
    sprintf(deg, "%2d%cF     ", tstat_table[event_scroll_bar.thumb_pos + 4], 0xA4);
    ptext(deg, 193, 112, BG_WHT + FG_BLK);
  } else if (rc == X10_RCS_CENTI_TSTAT) {
    this_event.action = centi_tstat_table[event_scroll_bar.thumb_pos + 4];
    sprintf(deg, "%2d%cC     ", centi_tstat_table[event_scroll_bar.thumb_pos + 4], 0xA4);
    ptext(deg, 193, 112, BG_WHT + FG_BLK);
  } else {
    this_event.action = (event_scroll_bar.thumb_pos << 4) + 0x02;
    sprintf(deg, "%.0f%% Bright   ",
            (double)(16 - event_scroll_bar.thumb_pos) * 6.25);
    //((16 - event_scroll_bar.thumb_pos + 1)/(15.0)) * 93.46);
    ptext(deg, 193, 112, BG_WHT + FG_BLK);
  }
}

void EventTODTextChange(void) {
  //UCHAR   buf[15];
  //UCHAR   hour;
  //UCHAR   min;
  //char   *cpos;

  //if( (strlen(offsetspin[0].text) == 0) || (IsValid(&offsetspin[0]) == FALSE) )
  //{
  //   return;
  //}

  //memset(buf, 0, 15);

  //if ( (offsetspin[0].text[strlen(offsetspin[0].text) - 1] == 'a') || (offsetspin[0].text[strlen(offsetspin[0].text) - 1] == 'A') )
  //{   //AM
  //   if( (cpos = strchr(offsetspin[0].text, ':')) == NULL)
  //   {
  //      return;
  //   }
  //   hour = atoi(&offsetspin[0].text[0]);
  //   if (hour == 12)
  //   {
  //      hour = 0;
  //   }
  //   min = atoi(cpos + 1);
  //      position(20,20);
  //      printf("TIME: %02d:%02d am     ", hour, min);
  //}
  //else
  //{   //PM
  //   if( (cpos = strchr(offsetspin[0].text, ':')) == NULL)
  //   {
  //      return;
  //   }
  //   hour = atoi(&offsetspin[0].text[0]);
  //   if (hour == 12)
  //   {
  //      hour = 0;
  //   }
  //   hour += 12;
  //   min = atoi(cpos + 1);
  //      position(20,20);
  //      printf("TIME: %02d:%02d pm     ", hour, min);
  //}
  this_event.min = get_tod.minute;
  if ((get_tod.hour == 12) && (get_tod.am_pm == TE_AM)) {
    this_event.hour = 0;
    return;
  }
  if ((get_tod.am_pm == TE_PM) && (get_tod.hour < 12)) {
    this_event.hour = get_tod.hour + 12;
    return;
  }
  if (((get_tod.am_pm == TE_PM) && (get_tod.hour == 12)) || (get_tod.hour < 12)) {
    this_event.hour = get_tod.hour;
    return;
  }
}

void OffSetSpinnerChange(void) {
  //this_event.offset_mins = offsetspin.value;
  if (ddplusminus.selected == 0) // Plus
  {
    this_event.offset_mins = offsetspin.value + 60 * hr_offsetspin.value;
  } else // Minus
  {
    this_event.offset_mins = -offsetspin.value - (60 * hr_offsetspin.value);
  }
}

void EventRandomTextChange(void) //random time change
{
  //   UCHAR   buf[15];
  //   USHORT   i;
  //   UCHAR   base_hour;
  //   UCHAR   base_min;

  //if( (strlen(offsetspin[2].text) == 0) || (IsValid(&offsetspin[2]) == FALSE) )
  //{
  //   return;
  //}

  //memset(buf, 0, 15);

  //if ( (offsetspin[2].text[strlen(offsetspin[2].text) - 1] == 'a') || (offsetspin[2].text[strlen(offsetspin[2].text) - 1] == 'A') )
  //{   //AM
  //   for( i = 0; i < strlen(offsetspin[2].text) - 1; i++)
  //   {
  //      if (offsetspin[2].text[i] != ':')
  //      {
  //         buf[i] = offsetspin[2].text[i];
  //      }
  //      else
  //      {
  //         break;
  //      }
  //   }
  //   base_hour = atoi(buf);
  //   buf[0] = offsetspin[2].text[i + 1];
  //   buf[1] = offsetspin[2].text[i + 2];
  //   base_min = atoi(buf);
  //}
  //else
  //{   //PM
  //   for( i = 0; i < strlen(offsetspin[2].text) - 1; i++)
  //   {
  //      if (offsetspin[2].text[i] != ':')
  //      {
  //         buf[i] = offsetspin[2].text[i];
  //      }
  //      else
  //      {
  //         break;
  //      }
  //   }
  //   base_hour = atoi(buf);
  //   if (base_hour == 12)
  //   {
  //      base_hour = 0;
  //   }
  //   base_hour += 12;
  //   buf[0] = offsetspin[2].text[i + 1];
  //   buf[1] = offsetspin[2].text[i + 2];
  //   base_min = atoi(buf);
  //}
  //this_event.base_hour = base_hour;
  //this_event.base_min = base_min;

  this_event.base_min = get_basetime.minute;
  if ((get_basetime.hour == 12) && (get_basetime.am_pm == TE_AM)) {
    this_event.base_hour = 0;
    return;
  }
  if ((get_basetime.am_pm == TE_PM) && (get_basetime.hour < 12)) {
    this_event.base_hour = get_basetime.hour + 12;
    return;
  }
  if (((get_basetime.am_pm == TE_PM) && (get_basetime.hour == 12)) || (get_basetime.hour < 12)) {
    this_event.base_hour = get_basetime.hour;
    return;
  }
}

int LookupEvent(USHORT event_id) {
  int i;

  for (i = 0; i < events_used; ++i) {
    if (events[i].event_id == event_id) {
      return i;
    }
  }
  return -1;
}

// inserts a new event into the existing event list (sorted on time)
void InsertEvent(EVENT* event) {
  int i;
  int event_position;
  int event_index;

  event_index = -1;
  for (i = 0; i < events_used; ++i) {
    if (events[i].event_id == event->event_id) {
      event_index = i;
      break;
    }
  }
  if (event_index >= 0) {
    for (i = event_index; i < events_used; ++i) {
      events[i] = events[i + 1];
    }
    --events_used;
    memset(&events[events_used], 0, sizeof(EVENT));
  }
  switch (event->event_type) {
  case 0: // Standard
  case 1: // One Time
    break;
  case 2:                                                                               // +/- sunrise
    i = (dawn_dusk_time.dawn_hour * 60) + dawn_dusk_time.dawn_min + event->offset_mins; //      360 minutes = 6:00am
    event->hour = i / 60;
    event->min = i % 60;
    break;
  case 3:                                                                               // +/- sunset
    i = (dawn_dusk_time.dusk_hour * 60) + dawn_dusk_time.dusk_min + event->offset_mins; //      1080 minutes = 6:00pm
    event->hour = i / 60;
    event->min = i % 60;
    break;
  case 4: // random
    event->hour = event->base_hour;
    event->min = event->base_min;
    break;
  default:
    break;
  }
  // find proper slot
  event_position = -1;
  for (i = 0; i < events_used; ++i) {
    if (event->hour < events[i].hour) {
      event_position = i;
      break;
    } else if (event->hour == events[i].hour) {
      if (event->min < events[i].min) {
        event_position = i;
        break;
      }
    }
  }
  if (event_position < 0) {
    event_position = events_used;
    i = event_position;
  } else {
    for (i = MAX_EVENTS - 1; i > event_position; --i) // move events down for new entry
    {
      events[i] = events[i - 1]; // move events up
    }
  }
  memset(&events[i], 0, sizeof(EVENT));
  events[i].event_id = event->event_id;
  events[i].device = event->device;
  events[i].hour = event->hour;
  events[i].min = event->min;
  events[i].action = event->action;
  events[i].type_day = event->type_day;
  events[i].mask1 = event->mask1;
  events[i].mask2 = event->mask2;
  events[i].event_type = event->event_type;
  events[i].offset_mins = event->offset_mins;
  events[i].base_hour = event->base_hour;
  events[i].base_min = event->base_min;
  ++events_used;
}

#if 0
void ShowDayType(void)
{
   int i;
   UCHAR buf[100];
   UCHAR buf2[50];

   memset(buf, 0, 100);
   memset(buf2, 0, 50);

   sprintf(buf, "type_day = ");
   for (i = 0; i < 8; i++)
   {
      if ((1 << i) & this_event.type_day)
      {
         strcat(buf, "1");
      }
      else
      {
         strcat(buf, "0");
      }
   }

   switch (this_event.event_type)
   {
   case 0: //standard
      strcat(buf, "  STD");
      break;
   case 1:   //1 time event
      strcat(buf, "  1TE");
      break;
   case 2:   //dawn
      strcat(buf, "  DWN");
      break;
   case 3:   //dusk
      strcat(buf, "  DSK");
      break;
   case 4:   //random
      strcat(buf, "  RND");
      break;
   }
   switch (this_event.action)
   {
   case EVNT_OFF:
      strcat(buf, "  Off");
      break;
   case EVNT_ON:
      strcat(buf, "  On ");
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
      sprintf(buf2, "%.0f%% Brt", (double)((16.0 - (this_event.action / 16)) * 6.25));
      //         sprintf(buf2, "  Dim %2d", this_event.action/16);
      strcat(buf, buf2);
      break;
   }
   sprintf(buf2, "  ID: %d", this_event.event_id);
   strcat(buf, buf2);

   Status(buf);
}
#endif

static void OnScroll(int direction, int how_far, int thumb_pos) {
  list_box.item_selected = thumb_pos;
  ListBoxScroll(&list_box);
  //PutListBox(&list_box);
  if (list_box.OnChange != NULL) {
    (*list_box.OnChange)();
  }
}

static void list_boxOnChange(void) {
  sb.max = list_box.item_count - 1;
  sb.thumb_pos = list_box.item_selected;
  sb.large_chg = list_box.so.position.height / 14;
  if (sb.large_chg > list_box.item_count) {
    sb.large_chg = list_box.item_count;
  }
  DisplayScrollBar(&sb);
}

void dev_listboxOnChange(void) {
  devicesb.max = dev_listbox.item_count - 1;
  devicesb.thumb_pos = dev_listbox.item_selected;
  devicesb.large_chg = dev_listbox.so.position.height / 14;
  if (devicesb.large_chg > dev_listbox.item_count) {
    devicesb.large_chg = dev_listbox.item_count;
  }
  DisplayScrollBar(&devicesb);

  UpdateListBox();
  list_boxOnChange();
}

static void TypeChange(void) {
  if (ddtype.selected == 0) {
    dev_listbox.item_selected = 0;
  }
  glb_cat_index = ddtype.selected;
  UpdateDeviceListBox(ddtype.selected);
  //UpdateListBox();
  dev_listboxOnChange();
}

static void UpdateDeviceListBox(int cat_index) {
  int i;
  int j;

  dev_subliste[0] = &alldevice;
  dev_listbox.item_selected = 0;
  if (cat_index != 0) {
    for (i = 0, j = 1; i < dev_list.devices_used; i++) {
      if ((dev_list.devices[i].category == cat_index) && (IsSubmeter(&dev_list.devices[i]) == FALSE)) {
        dev_subliste[j++] = &dev_list.devices[i];
        if (memcmp(&Ethis_device, &dev_list.devices[i], sizeof(DEVICE)) == 0) {
          dev_listbox.item_selected = j - 1;
        }
      }
    }
  } else // all devices
  {
    for (i = 0, j = 1; i < dev_list.devices_used; i++) {
      if (IsSubmeter(&dev_list.devices[i]) == FALSE) {
        dev_subliste[j++] = &dev_list.devices[i];
        if (memcmp(&Ethis_device, &dev_list.devices[i], sizeof(DEVICE)) == 0) {
          dev_listbox.item_selected = j - 1;
        }
      }
    }
  }
  dev_listbox.item_count = j;
  devicesb.max = dev_listbox.item_count - 1;
  devicesb.thumb_pos = dev_listbox.item_selected;
  devicesb.large_chg = dev_listbox.so.position.height / 14;
  if (devicesb.large_chg > dev_listbox.item_count) {
    devicesb.large_chg = dev_listbox.item_count;
  }
  PutListBox(&dev_listbox);
  DisplayScrollBar(&devicesb);
}

//===========================================================================================
// NAME:   EventSort()
//
// HISTORY:
// Author         Date         Description
// M. Lauter      30 AUG 97      Initial Version
//
//
//
// INPUTS: N/A
// Name            Type         Description
//
// OUTPUTS: N/A
// Name            Type         Description
//
// DESCRIPTION:
//
// EventSort() sorts the event list box by event rather than by device.
// The sister function is DeviceSort() which sorts the list box
// by device.
// Both routines simply rearrange the list box's sub_list of pointers
// and then redisplay the list box.
//
//===========================================================================================
static void EventSort(void) {
  int i;
  int holder;
  int sscount;
  int swap;
  EVENT* tmpevent;

  schedule_sort = EVENTSORT;
  if (list_box.item_count > 1) {
    //============================================================================
    //
    // insert correct time values for comparision purposes
    //
    //============================================================================
    sscount = 0;

    for (i = 0; i < list_box.item_count; i++) {
      switch (event_sublist[i]->event_type) {
      case 0: // Standard
      case 1: // One Time
        break;
      case 2:                                                                                               // +/- sunrise
        holder = (dawn_dusk_time.dawn_hour * 60) + dawn_dusk_time.dawn_min + event_sublist[i]->offset_mins; //      360 minutes = 6:00am
        event_sublist[i]->hour = holder / 60;
        event_sublist[i]->min = holder % 60;
        break;
      case 3:                                                                                               // +/- sunset
        holder = (dawn_dusk_time.dusk_hour * 60) + dawn_dusk_time.dusk_min + event_sublist[i]->offset_mins; //      1080 minutes = 6:00pm
        event_sublist[i]->hour = holder / 60;
        event_sublist[i]->min = holder % 60;
        break;
      case 4: // random
        event_sublist[i]->hour = event_sublist[i]->base_hour;
        event_sublist[i]->min = event_sublist[i]->base_min;
        break;
      case 255:
        sscount++;
        break;
      default:
        break;
      }
    }

    //============================================================================
    //
    // move smart schedules to the beginning of the list (event type 255)
    //
    //============================================================================
    swap = TRUE;
    while (swap == TRUE) {
      swap = FALSE;
      for (i = 0; i < list_box.item_count - 1; i++) {
        if ((event_sublist[i]->event_type != 255) && (event_sublist[i + 1]->event_type == 255)) {
          tmpevent = event_sublist[i];
          event_sublist[i] = event_sublist[i + 1];
          event_sublist[i + 1] = tmpevent;
          swap = TRUE;
        }
      }
    }

    //============================================================================
    //
    // now sort the rest
    //
    //============================================================================
    swap = TRUE;
    while (swap == TRUE) {
      swap = FALSE;
      for (i = sscount; i < list_box.item_count - 1; i++) {
        if (((event_sublist[i]->hour * 60) + event_sublist[i]->min) >
            ((event_sublist[i + 1]->hour * 60) + event_sublist[i + 1]->min)) {
          tmpevent = event_sublist[i];
          event_sublist[i] = event_sublist[i + 1];
          event_sublist[i + 1] = tmpevent;
          swap = TRUE;
        }
      }
    }
  }
  //ListBoxScroll(&list_box);
  PutListBox(&list_box);
}

//===========================================================================================
// NAME:   DeviceSort()
//
// HISTORY:
// Author         Date         Description
// M. Lauter      30 AUG 97      Initial Version
//
//
//
// INPUTS: N/A
// Name            Type         Description
//
// OUTPUTS: N/A
// Name            Type         Description
//
// DESCRIPTION:
//
// DeviceSort() sorts the event list box by device rather than by event.
// The sister function is EventSort() which sorts the list box
// by event.
// Both routines simply rearrange the list box's sub_list of pointers
// and then redisplay the list box.
//
//===========================================================================================
static void DeviceSort(void) {
  int i;
  int id;
  int d1;
  int d2;
  int swap;
  EVENT* tmpevent;

  schedule_sort = DEVICESORT;
  swap = TRUE;
  while (swap == TRUE) {
    swap = FALSE;
    for (i = 0; i < list_box.item_count - 1; i++) {
      for (id = 1; id < dev_listbox.item_count; id++) {
        if (event_sublist[i]->device == dev_subliste[id]->device_slot) {
          d1 = id;
        }
        if (event_sublist[i + 1]->device == dev_subliste[id]->device_slot) {
          d2 = id;
        }
      }
      if (strnicmp(dev_subliste[d1]->desc, // dev_subliste[0] == "all devices"
                   dev_subliste[d2]->desc, // dev_subliste[0] == "all devices"
                   13) > 0)                // string one greater than string two so swap 'em
      {
        tmpevent = event_sublist[i];
        event_sublist[i] = event_sublist[i + 1];
        event_sublist[i + 1] = tmpevent;
        swap = TRUE;
      }
    }
  }
  //ListBoxScroll(&list_box);
  PutListBox(&list_box);
}

EVENT bogus_event[MAX_DEVICES];
static void UpdateListBox(void) {
  int i;
  int j;
  int a;
  int smartcount;

  if (dev_listbox.item_selected == 0) // if "all devices" is selected from the device list box....
  {
    // build the first part is built up of "bogus" events
    //   representing smart scheduled devices
    //
    // the second part is built after this for loop
    // the second part of the list with REAL events
    j = 0;
    for (a = 1, smartcount = 0; a < dev_listbox.item_count; a++) // dev_liste[0] = "all devices"
    {
      if (dev_subliste[a]->control_type > 0) // device is smart scheduled
      {
        memset(&bogus_event[smartcount], 0, sizeof(EVENT));
        memset(&bogus_event[smartcount].type_day, 0xFF, sizeof(UCHAR));
        memset(&bogus_event[smartcount].mask1, 0xFF, sizeof(UCHAR));
        memset(&bogus_event[smartcount].mask2, 0xFF, sizeof(UCHAR));
        memset(&bogus_event[smartcount].mask3, 0xFF, sizeof(UCHAR));
        bogus_event[smartcount].event_type = 255;
        bogus_event[smartcount].device = dev_subliste[a]->device_slot;
        event_sublist[j++] = &bogus_event[smartcount++];
      }
    }
    for (i = 0; i < events_used; i++) {
      // in case the category is not "all"
      for (a = 1; a < dev_listbox.item_count; a++) // start from 1 cause item 0 is "all devices"
      {
        // first check to see if the event should even be added to the list
        // it has to belong to a device in this category
        if (events[i].device == dev_subliste[a]->device_slot) {
          // now check the "schedule by mode" dropdown boxes to see
          // if the event should be displayed
          if (ddday.selected != 0) {
            if (!(events[i].type_day & (0x01 << (ddday.selected - 1)))) {
              continue;
            }
          }
          if (ddHVAC.selected != 0) {
            if (!(events[i].mask1 & (0x01 << (ddHVAC.selected + 3)))) {
              continue;
            }
          }
          if (ddseason.selected != 0) {
            if (!(events[i].mask1 & (0x01 << (ddseason.selected - 1)))) {
              continue;
            }
          }
          if (ddcost.selected != 0) {
            if (!(events[i].mask2 & (0x01 << (ddcost.selected + 3)))) {
              continue;
            }
          }
          if (ddsystem.selected != 0) {
            if (!(events[i].mask1 & (0x01 << (ddsystem.selected + 5)))) {
              continue;
            }
          }
          if (ddoccupied.selected != 0) {
            if (!(events[i].mask2 & (0x01 << (ddoccupied.selected + 1)))) {
              continue;
            }
          }
          // ok it matches so add it to the sub list
          event_sublist[j++] = &events[i];
        }
      }
    }
  } else // a specific device was chosen
  {
    j = 0;
    if (dev_subliste[dev_listbox.item_selected]->control_type > 0) {
      memset(&bogus_event[0], 0, sizeof(EVENT));
      bogus_event[0].event_type = 255;
      memset(&bogus_event[0].type_day, 0xFF, sizeof(UCHAR));
      memset(&bogus_event[0].mask1, 0xFF, sizeof(UCHAR));
      memset(&bogus_event[0].mask2, 0xFF, sizeof(UCHAR));
      memset(&bogus_event[0].mask3, 0xFF, sizeof(UCHAR));
      bogus_event[0].device = dev_subliste[dev_listbox.item_selected]->device_slot;
      event_sublist[0] = &bogus_event[0];
      j = 1;
    } else {
      for (i = 0; i < events_used; i++) {
        if (events[i].device == dev_subliste[dev_listbox.item_selected]->device_slot) {
          if (ddday.selected != 0) {
            if (!(events[i].type_day & (0x01 << (ddday.selected - 1)))) {
              continue;
            }
          }
          if (ddHVAC.selected != 0) {
            if (!(events[i].mask1 & (0x01 << (ddHVAC.selected + 3)))) {
              continue;
            }
          }
          if (ddseason.selected != 0) {
            if (!(events[i].mask1 & (0x01 << (ddseason.selected - 1)))) {
              continue;
            }
          }
          if (ddcost.selected != 0) {
            if (!(events[i].mask2 & (0x01 << (ddcost.selected + 3)))) {
              continue;
            }
          }
          if (ddsystem.selected != 0) {
            if (!(events[i].mask1 & (0x01 << (ddsystem.selected + 5)))) {
              continue;
            }
          }
          if (ddoccupied.selected != 0) {
            if (!(events[i].mask2 & (0x01 << (ddoccupied.selected + 1)))) {
              continue;
            }
          }
          // ok it matches so add it to the sub list
          event_sublist[j++] = &events[i];
        }
      }
    }
  }
  list_box.item_count = j;
  if (list_box.item_selected > j) {
    list_box.item_selected = 0;
  }

  sb.max = list_box.item_count - 1;
  sb.thumb_pos = list_box.item_selected;
  sb.large_chg = list_box.so.position.height / 14;
  if (sb.large_chg > list_box.item_count) {
    sb.large_chg = list_box.item_count;
  }

  if (dev_listbox.item_selected != 0) // force an event sort
  {
    BtnClick(&event_menu_btn[4]);
  }
  if (schedule_sort == EVENTSORT) {
    EventSort();
  } else {
    DeviceSort();
  }
}

static void ResetFilters(void) {
  ddday.selected = 0;
  ddHVAC.selected = 0;
  ddseason.selected = 0;
  ddcost.selected = 0;
  ddsystem.selected = 0;
  ddoccupied.selected = 0;

  DisplayDropDown(&ddday);
  DisplayDropDown(&ddHVAC);
  DisplayDropDown(&ddseason);
  DisplayDropDown(&ddcost);
  DisplayDropDown(&ddsystem);
  DisplayDropDown(&ddoccupied);

  UpdateListBox();
}

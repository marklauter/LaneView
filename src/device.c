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
//#include "moodmenu.h"
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

#define LOAD_CONTROL_TECO 0

DEVICE_LIST_T dev_list;
DEVICE_HISTORY device_history[MAX_HISTORY];

static DEVICE this_dev;
static int dev_option;
static int current_cat;
static UCHAR edtitle[15];
static DEVICE* dev_sublist[MAX_DEVICES];
static UCHAR mm_strings[10][34];
static UCHAR* mm_str2[10];

void DisplayDeviceShort(DEVICE* dev, int top, int left, UCHAR color);
void DisplayDeviceLong(DEVICE* dev, int top, int left, UCHAR color);

static void DisplayX10(DEVICE* dev);
static void DisplayCEBus(DEVICE* dev);
static void DisplayMeterModule(void);
static void PriorityChange(void);
static void SaveDeviceChanges(void);
static void CheckBox1Click(void);
static void CheckBox2Click(void);
static void button1(void);
static void button2(void);
static void CebusTypeChange(void);
static void button2(void);
static void button1(void);
static void DisplayGetMonitor(void);
static void MonitorDropOnChange(void);

PUSH_BUTTON_T edbtnlist[2] = { // BEGIN SCREEN OBJECT DEFINITION
    419, 446, 89, 33,          // position
    FALSE,                     // focus
    PUSH_BUTTON,               // type
    TRUE,                      // visible
    BtnMouseDown,              // OnMouseDown()
    BtnMouseUp,                // OnMouseUp()
    BtnClick,                  // OnClick()
    BtnKeyPress,               // OnKeyPress()
    PutButton,                 // DisplayObject()
                               // END SCREEN OBJECT DEFINITION
    FALSE,                     //default button
    "Apply",                   //text
    push,                      //type
    up,                        //state
    NULL,                      //icon
    button1,                   //function
    141,

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
    FALSE,            //default button
    "Cancel",         //text
    push,             //type
    up,               //state
    NULL,             //icon
    button2,          //function
    142};

/////////////////////////////////////
//
// X10 #defines
// from 2box.h
//
/////////////////////////////////////
// #define X10_RELAY            0
// #define X10_DIMMER            1
// #define X10_ANNUNCIATOR         2
// #define X10_RCS_TSTAT         3
// #define X10_RCS_2WAY            4
// #define X10_DECORA            5
/////////////////////////////////////

/////////////////////////////////////
//CEBus defines
//from 2box.h
/////////////////////////////////////
//CEBUS_DIMMER
//CEBUS_RELAY
//CEBUS_LOAD_SHEDDER
//CEBUS_OLD_MM
//CEBUS_NEW_MM
/////////////////////////////////////

static char* catlist[7] = {"All", "Lights", "Comfort", "Pool", "Sprinkler", "Security", "Monitor"};

static UCHAR* x10_type[6] = {"Relay",
                             "Dimmer",
                             "Sounder",
                             "F\x0A4 HVAC",
                             //                        "2 Way HVAC",
                             "Leviton Dimmer",
                             "C\x0A4 HVAC"};

static UCHAR* cb_type[4] = {"Dimmer",
                            "Relay",
                            "Load Control Module",
                            "InterLane Sub Meter"};

static UCHAR* dev_pri[3] = {"0", "1", "2"};

UCHAR* x10_ltr[16] = {"A", "B", "C", "D", "E", "F", "G", "H",
                      "I", "J", "K", "L", "M", "N", "O", "P"};

UCHAR* x10_nmr[16] = {"1", "2", "3", "4", "5", "6", "7", "8",
                      "9", "10", "11", "12", "13", "14", "15", "16"};

static UCHAR* d_volts[4] = {"120", "240", "208", "220"};

static UCHAR* d_sub_unit[4] = {"0", "1", "2", "3"};

static DROPDOWN_BOX_T ddcategory = {
    319, 93, 88, 17,
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
    NULL               // OnChange
};

DROPDOWN_BOX_T edddblist[8] = {
    //x10 type drop down box
    176, 430, 120, 17,
    FALSE,             // got_focus
    DROP_DOWN_BOX,     // type
    FALSE,             // visible
    DropDownMouseDown, // OnMouseDown()
    DropDownMouseUp,   // OnMouseUp()
    DropDownClick,     // OnClick()
    DropDownKeyPress,  // OnKeyPress()
    DisplayDropDown,   // DisplayObject()
    x10_type,          // strings
    0,                 // item selected
    FALSE,             // dropped
    6,                 // item count
    NULL,              // OnChange

    //cebus type
    208, 429, 176, 17,
    FALSE,             // got_focus
    DROP_DOWN_BOX,     // type
    FALSE,             // visible
    DropDownMouseDown, // OnMouseDown()
    DropDownMouseUp,   // OnMouseUp()
    DropDownClick,     // OnClick()
    DropDownKeyPress,  // OnKeyPress()
    DisplayDropDown,   // DisplayObject()
    cb_type,           // strings
    0,                 // item selected
    FALSE,             // dropped
    4,                 // item count
    CebusTypeChange,   // OnChange

    //device priority
    367 + 14, 93, 32, 17,
    FALSE,             // got_focus
    DROP_DOWN_BOX,     // type
    TRUE,              // visible
    DropDownMouseDown, // OnMouseDown()
    DropDownMouseUp,   // OnMouseUp()
    DropDownClick,     // OnClick()
    DropDownKeyPress,  // OnKeyPress()
    DisplayDropDown,   // DisplayObject()
    dev_pri,           // strings
    0,                 // item selected
    FALSE,             // dropped
    3,                 // item count
    PriorityChange,    // OnChange

    //x10 letter
    120, 430, 40, 17,
    FALSE,             // got_focus
    DROP_DOWN_BOX,     // type
    FALSE,             // visible
    DropDownMouseDown, // OnMouseDown()
    DropDownMouseUp,   // OnMouseUp()
    DropDownClick,     // OnClick()
    DropDownKeyPress,  // OnKeyPress()
    DisplayDropDown,   // DisplayObject()
    x10_ltr,           // strings
    0,                 // item selected
    FALSE,             // dropped
    16,                // item count
    NULL,              // OnChange

    //x10 number
    148, 430, 40, 17,
    FALSE,             // got_focus
    DROP_DOWN_BOX,     // type
    FALSE,             // visible
    DropDownMouseDown, // OnMouseDown()
    DropDownMouseUp,   // OnMouseUp()
    DropDownClick,     // OnClick()
    DropDownKeyPress,  // OnKeyPress()
    DisplayDropDown,   // DisplayObject()
    x10_nmr,           // strings
    0,                 // item selected
    FALSE,             // dropped
    16,                // item count
    NULL,              // OnChange

    //meter module volts 120, 240, 208, 220
    336, 429, 45, 17,
    FALSE,             // got_focus
    DROP_DOWN_BOX,     // type
    FALSE,             // visible
    DropDownMouseDown, // OnMouseDown()
    DropDownMouseUp,   // OnMouseUp()
    DropDownClick,     // OnClick()
    DropDownKeyPress,  // OnKeyPress()
    DisplayDropDown,   // DisplayObject()
    d_volts,           // strings
    0,                 // item selected
    FALSE,             // dropped
    4,                 // item count
    NULL,              // OnChange

    //meter module sub unit 1 - 4
    365, 429, 45, 17,
    FALSE,             // got_focus
    DROP_DOWN_BOX,     // type
    FALSE,             // visible
    DropDownMouseDown, // OnMouseDown()
    DropDownMouseUp,   // OnMouseUp()
    DropDownClick,     // OnClick()
    DropDownKeyPress,  // OnKeyPress()
    DisplayDropDown,   // DisplayObject()
    d_sub_unit,        // strings
    0,                 // item selected
    FALSE,             // dropped
    4,                 // item count
    NULL,              // OnChange

    //M&M drop down box
    295, 320, 280, 17,
    FALSE,              // got_focus
    DROP_DOWN_BOX,      // type
    FALSE,              // visible
    DropDownMouseDown,  // OnMouseDown()
    DropDownMouseUp,    // OnMouseUp()
    DropDownClick,      // OnClick()
    DropDownKeyPress,   // OnKeyPress()
    DisplayDropDown,    // DisplayObject()
    mm_str2,            // strings
    0,                  // item selected
    FALSE,              // dropped
    0,                  // item count
    MonitorDropOnChange // OnChange
};

TEXT_BOX_T edtxtlist[7] = { //device desc
    0, 0, 0, 14,            // position
    TRUE,                   // got_focus
    TEXT_BOX,               // type
    TRUE,                   // visible
    TxtMouseDown,           // OnMouseDown()
    TxtMouseUp,             // OnMouseUp()
    TxtClick,               // OnClick()
    TxtKeyPress,            // OnKeyPress()
    PutTxtBox,              // DisplayObject()
    //end of screen object definition
    17,       // row
    13,       // column
    13,       // box length
    tb_alpha, // type
    "",       // text
    NULL,

    //device location
    0, 0, 0, 14,  // position
    FALSE,        // got_focus
    TEXT_BOX,     // type
    TRUE,         // visible
    TxtMouseDown, // OnMouseDown()
    TxtMouseUp,   // OnMouseUp()
    TxtClick,     // OnClick()
    TxtKeyPress,  // OnKeyPress()
    PutTxtBox,    // DisplayObject()
    //end of screen object definition
    20,       // row
    13,       // column
    18,       // box length
    tb_alpha, // type
    "",       // text
    NULL,

    //watts
    0, 0, 0, 14,  // position
    FALSE,        // got_focus
    TEXT_BOX,     // type
    FALSE,        // visible
    TxtMouseDown, // OnMouseDown()
    TxtMouseUp,   // OnMouseUp()
    TxtClick,     // OnClick()
    TxtKeyPress,  // OnKeyPress()
    PutTxtBox,    // DisplayObject()
    //end of screen object definition
    27,     // row
    13,     // column
    5,      // box length
    tb_int, // type
    "",     // text
    NULL,

    //cebus serial number
    0, 0, 0, 14,  // position
    FALSE,        // got_focus
    TEXT_BOX,     // type
    FALSE,        //visible
    TxtMouseDown, // OnMouseDown()
    TxtMouseUp,   // OnMouseUp()
    TxtClick,     // OnClick()
    TxtKeyPress,  // OnKeyPress()
    PutTxtBox,    // DisplayObject()
    //////////////////////////////////////end of screen object definition
    10,       // row
    55,       // column
    12,       // box length
    tb_alpha, // type
    "",       // text
    NULL,

    //cebus house code
    0, 0, 0, 14,  // position
    FALSE,        // got_focus
    TEXT_BOX,     // type
    FALSE,        //visible
    TxtMouseDown, // OnMouseDown()
    TxtMouseUp,   // OnMouseUp()
    TxtClick,     // OnClick()
    TxtKeyPress,  // OnKeyPress()
    PutTxtBox,    // DisplayObject()
    //////////////////////////////////////end of screen object definition
    12,     // row
    55,     // column
    4,      // box length
    tb_hex, // type
    "",     // text
    NULL,

    //cebus unit/mac
    0, 0, 0, 14,  // position
    FALSE,        // got_focus
    TEXT_BOX,     // type
    FALSE,        //visible
    TxtMouseDown, // OnMouseDown()
    TxtMouseUp,   // OnMouseUp()
    TxtClick,     // OnClick()
    TxtKeyPress,  // OnKeyPress()
    PutTxtBox,    // DisplayObject()
    //////////////////////////////////////end of screen object definition
    14,     // row
    55,     // column
    4,      // box length
    tb_hex, // type
    "",     // text
    NULL,

    //meter module FACTOR
    0, 0, 0, 14,  // position
    FALSE,        // got_focus
    TEXT_BOX,     // type
    FALSE,        //visible
    TxtMouseDown, // OnMouseDown()
    TxtMouseUp,   // OnMouseUp()
    TxtClick,     // OnClick()
    TxtKeyPress,  // OnKeyPress()
    PutTxtBox,    // DisplayObject()
    //////////////////////////////////////end of screen object definition
    23,       // row
    55,       // column
    5,        // box length
    tb_float, // type
    "",       // text
    NULL};

CHECK_BOX_T edCheckBoxList[2] = {115, 93, 0, 0,       // position
                                 FALSE,               // got_focus
                                 CHECK_BOX,           // type
                                 TRUE,                //visible
                                 CheckBoxOnMouseDown, // OnMouseDown()
                                 CheckBoxOnMouseUp,   // OnMouseUp()
                                 CheckBoxOnClick,     // OnClick()
                                 CheckBoxOnKeyPress,  // OnKeyPress()
                                 DisplayCheckBox,     // DisplayObject()
                                 "X-10",
                                 1,
                                 CheckBox1Click,

                                 135, 93, 0, 0,       // position
                                 FALSE,               // got_focus
                                 CHECK_BOX,           // type
                                 TRUE,                //visible
                                 CheckBoxOnMouseDown, // OnMouseDown()
                                 CheckBoxOnMouseUp,   // OnMouseUp()
                                 CheckBoxOnClick,     // OnClick()
                                 CheckBoxOnKeyPress,  // OnKeyPress()
                                 DisplayCheckBox,     // DisplayObject()
                                 "CEBus",
                                 0,
                                 CheckBox2Click};

#define EDVCWINCOUNT 20
SCREEN_OBJECT_T* edSo[EDVCWINCOUNT];
static void edWindowDraw(void);
LWINDOW_T edWindow = {
    35, 0, 640, 445, //window position
    edSo,            //screen object
    edWindowDraw,
    EDVCWINCOUNT,     //item count
    12,               //cursor position 0-79 left to right
    0,                //element that has current focus
    fcsBtn,           //focus type
    edtitle,          // title
    DefaultOnKeyPress //OnKeyPress();
};

void EditDevice(DEVICE* dev, int opt, int category) {
  int i;

  this_dev = *dev;
  dev_option = opt;
  edWindow.cursor_pos = 12;
  current_cat = category;

  if (opt == 'A') {
    strcpy(edWindow.title, "Add Device");
    ddcategory.selected = current_cat;
  } else {
    strcpy(edWindow.title, "Edit Device");
    ddcategory.selected = this_dev.category;
  }

  for (i = 0; i < 7; i++) {
    memset(edtxtlist[i].text, 0, MAX_TEXT);
  }

  //screen object list index doubles as the tab order
  edSo[0] = &edtxtlist[0].so; //device desc     //this one always starts with focus
  edtxtlist[0].so.got_focus = TRUE;
  edSo[1] = &edtxtlist[1].so; //device location

  edSo[2] = &ddcategory.so;

  edSo[3] = &edddblist[2].so; //device load control priority
  if (secured_user == FALSE) {
    edddblist[2].so.visible = FALSE;
  } else {
    edddblist[2].so.visible = TRUE;
  }

  edSo[4] = &edCheckBoxList[0].so; //x-10
  edSo[5] = &edCheckBoxList[1].so; //cebus
  edSo[6] = &edtxtlist[2].so;      //device watts
  edSo[7] = &edddblist[3].so;      //x-10 letter
  edddblist[3].so.visible = FALSE;
  edSo[8] = &edddblist[4].so; //x-10 number
  edddblist[4].so.visible = FALSE;
  edSo[9] = &edddblist[0].so; //x-10 type drop down box
  edddblist[0].so.visible = FALSE;
  edSo[10] = &edtxtlist[3].so; //cebus serial number
  edtxtlist[3].so.visible = FALSE;
  edSo[11] = &edtxtlist[4].so; //cebus house code
  edtxtlist[4].so.visible = FALSE;
  edSo[12] = &edtxtlist[5].so; //cebus unit/mac
  edtxtlist[5].so.visible = FALSE;
  edSo[13] = &edddblist[1].so; //cebus type drop down box
  edddblist[1].so.visible = FALSE;
  edSo[14] = &edtxtlist[6].so; // M&M factor
  edtxtlist[6].so.visible = FALSE;
  edSo[15] = &edddblist[5].so; // M&M volts
  edddblist[5].so.visible = FALSE;
  edSo[16] = &edddblist[6].so; // M&M sub unit
  edddblist[6].so.visible = FALSE;
  edSo[17] = &edddblist[7].so; //attached M&M, when priority > 0
  edddblist[7].so.visible = FALSE;

  edSo[18] = &edbtnlist[0].so; //ok btn
  edbtnlist[0].so.got_focus = FALSE;
  edSo[19] = &edbtnlist[1].so; //cancel btn
  edbtnlist[1].so.got_focus = FALSE;

  edWindow.focus.current = 0;
  glbWindow = &edWindow;

  // put device data into screen objects
  if (dev->code_letter != '@') { //x-10
    edddblist[3].selected = dev->code_letter - 65;
    edCheckBoxList[0].checked = TRUE;
    edCheckBoxList[1].checked = FALSE;
  } else { //cebus
    edddblist[3].selected = 0;
    edCheckBoxList[0].checked = FALSE;
    edCheckBoxList[1].checked = TRUE;
  }
  edddblist[6].selected = dev->cebus_subunit;                                  // M&M sub unit
  sprintf(edtxtlist[6].text, "%d.%02d", dev->factor / 100, dev->factor % 100); // M&M factor
  switch (dev->volts)                                                          // M&M volts
  {
  case 120:
    edddblist[5].selected = 0;
    break;
  case 240:
    edddblist[5].selected = 1;
    break;
  case 208:
    edddblist[5].selected = 2;
    break;
  case 220:
    edddblist[5].selected = 3;
    break;
  default:
    edddblist[5].selected = 0;
    break;
  }
  movez(edtxtlist[0].text, dev->desc, 13);
  trim(edtxtlist[0].text);
  movez(edtxtlist[1].text, dev->location, 18);
  trim(edtxtlist[1].text);
  edddblist[2].selected = dev->priority;
  sprintf(edtxtlist[2].text, "%.5d", dev->watts);
  if (dev->x10_device_type < 4) {
    edddblist[0].selected = dev->x10_device_type;
  } else if (dev->x10_device_type >= 5) {
    edddblist[0].selected = dev->x10_device_type - 1; // That pesky (&unused 2-way) tstat caused this
  }
  movez(edtxtlist[3].text, dev->serial_no, 12);
  trim(edtxtlist[3].text);
  sprintf(edtxtlist[4].text, "%.4X", dev->cebus_hc);
  sprintf(edtxtlist[5].text, "%.4X", dev->cebus_unit);

  switch (dev->cebus_type) {
  case CEBUS_DIMMER:
    edddblist[1].selected = 0;
    break;
  case CEBUS_RELAY:
    edddblist[1].selected = 1;
    break;
  case CEBUS_LOAD_SHEDDER:
    edddblist[1].selected = 2;
    break;
  case CEBUS_OLD_MM: // tecom submeters (M&M's)
  case CEBUS_NEW_MM:
    edddblist[1].selected = 3;
    break;
  default:
    edddblist[1].selected = 0;
    break;
  }

  PutWindow(glbWindow);
}

static void edWindowDraw(void) {
  if (edCheckBoxList[1].checked == TRUE) {
    DisplayCEBus(&this_dev);
  } else {
    DisplayX10(&this_dev);
  }

  GroupBox("Device Type", 95, 80, 65, 200);
  GroupBox("General Device Info", 175, 80, 240, 200);

  ptext("Device Description", 204, 93, BG_TRANSPARENT + FG_BLK);
  ptext("Device Location", 246, 93, BG_TRANSPARENT + FG_BLK);
  ptext("Device Category", 262 + 14 + 25, 93, BG_TRANSPARENT + FG_BLK);

  if (secured_user == TRUE) {
    ptext("Device Load", 348, 93, BG_TRANSPARENT + FG_BLK);
    ptext("Control Priority", 348 + 14, 93, BG_TRANSPARENT + FG_BLK);
  }
}

void DisplayX10(DEVICE* dev) {
  area_clear(90, 400, 290, 625, FG_WHT);

  if (dev->code_letter != '@') { //x-10
    edddblist[4].selected = dev->code_number - 1;
    edddblist[3].selected = dev->code_letter - 65;
  } else { //cebus
    edddblist[4].selected = 0;
    edddblist[3].selected = 0;
  }

  edddblist[3].so.visible = TRUE; //x-10 letter
  edddblist[4].so.visible = TRUE; //x-10 number
  edddblist[0].so.visible = TRUE; //x-10 type drop down box

  edtxtlist[3].so.visible = FALSE; //cebus serial number
  edtxtlist[4].so.visible = FALSE; //cebus house code
  edtxtlist[5].so.visible = FALSE; //cebus unit/mac
  edddblist[1].so.visible = FALSE; //cebus type drop down box
  edddblist[5].so.visible = FALSE; //M&M volts
  edddblist[6].so.visible = FALSE; //M&M sub unit
  edtxtlist[6].so.visible = FALSE; //M&M factor

  DisplayDropDown(&edddblist[3]);
  DisplayDropDown(&edddblist[4]);
  DisplayDropDown(&edddblist[0]);

  ptext("X-10 Letter", 125, 325, BG_WHT + FG_BLK);
  ptext("X-10 Number", 151, 325, BG_WHT + FG_BLK);
  ptext("X-10 Type", 179, 325, BG_WHT + FG_BLK);

  GroupBox("X-10 Address", 95, 305, 110, 250);

#if LOAD_CONTROL_TECO
  if (dev->priority > 0) {
    DisplayGetMonitor();
  }
#endif
}

void DisplayCEBus(DEVICE* dev) {
  area_clear(90, 400, 290, 625, FG_WHT);

  if (dev->code_letter != '@') { //x-10
    edddblist[3].selected = dev->code_letter - 65;
  } else { //cebus
    edddblist[3].selected = 0;
  }

  edddblist[3].so.visible = FALSE; //x-10 letter
  edddblist[4].so.visible = FALSE; //x-10 number
  edddblist[0].so.visible = FALSE; //x-10 type drop down box

  edtxtlist[3].so.visible = TRUE; //cebus serial number
  edtxtlist[4].so.visible = TRUE; //cebus house code
  edtxtlist[5].so.visible = TRUE; //cebus unit/mac
  edddblist[1].so.visible = TRUE; //cebus type drop down box

  ptext("Serial Number", 126, 320, BG_TRANSPARENT + FG_BLK);
  ptext("House Code", 153, 320, BG_TRANSPARENT + FG_BLK);
  ptext("Unit/MAC", 181, 320, BG_TRANSPARENT + FG_BLK);
  ptext("CEBus Type", 209, 320, BG_TRANSPARENT + FG_BLK);

  PutTxtBox(&edtxtlist[3]);
  PutTxtBox(&edtxtlist[4]);
  PutTxtBox(&edtxtlist[5]);

  DisplayDropDown(&edddblist[1]);

  if (edddblist[1].selected == 3) {
    DisplayMeterModule();
  }
#if LOAD_CONTROL_TECO
  if ((dev->priority > 0) && (edddblist[1].selected != 3)) {
    DisplayGetMonitor();
  }
#endif

  GroupBox("CEBus Address & Type", 95, 305, 140, 315);
}

void DisplayMeterModule(void) {
  ptext("Factor", 308, 320, BG_WHT + FG_BLK);
  ptext("Volts", 337, 320, BG_WHT + FG_BLK);
  ptext("Sub Unit", 366, 320, BG_WHT + FG_BLK);

  edddblist[5].so.visible = TRUE; //M&M volts
  edddblist[6].so.visible = TRUE; //M&M sub unit
  edtxtlist[6].so.visible = TRUE; //M&M factor

  PutTxtBox(&edtxtlist[6]);
  DisplayDropDown(&edddblist[2]);
  DisplayDropDown(&edddblist[5]);
  DisplayDropDown(&edddblist[6]);

  GroupBox("InterLane Meter Module", 280, 305, 110, 315);
}

void SaveDeviceChanges(void) {
  DEVICE dev;
  int i;
  int mm_pri = 0;
  //   double fp;
  //   UCHAR *cpos;

  memset(&dev, 0, sizeof(DEVICE));
  memcpy(&dev, &this_dev, sizeof(DEVICE));

  FitText(dev.desc, edtxtlist[0].text, 13);
  FitText(dev.location, edtxtlist[1].text, 18);
  dev.priority = edddblist[2].selected;
  dev.watts = atoi(edtxtlist[3].text);
  dev.category = ddcategory.selected;
  if (edCheckBoxList[1].checked == TRUE) { //cebus
    dev.code_letter = '@';
    FitText(dev.serial_no, edtxtlist[3].text, 12); //cebus serial number
    dev.cebus_hc = htoi(edtxtlist[4].text);        //cebus house code
    dev.cebus_unit = htoi(edtxtlist[5].text);      //cebus unit

    switch (edddblist[1].selected) //cebus type drop down box
    {
    case 0:
      dev.cebus_type = CEBUS_DIMMER;
      break;
    case 1:
      dev.cebus_type = CEBUS_RELAY;
      break;
    case 2:
      dev.cebus_type = CEBUS_LOAD_SHEDDER;
      break;
    case 3:
      dev.cebus_type = CEBUS_NEW_MM; //M&M
      switch (edddblist[5].selected) //M&M volts
      {
      case 0:
        dev.volts = 120;
        break;
      case 1:
        dev.volts = 240;
        break;
      case 2:
        dev.volts = 208;
        break;
      case 3:
        dev.volts = 220;
        break;
      default:
        msgbox("Invalid volts.", "Warning", MB_OK);
        break;
      }
      dev.cebus_subunit = edddblist[6].selected; //M&M subunit
      dev.factor = (USHORT)(atof(edtxtlist[6].text) * 100 + .5);
      //            if ((cpos = strchr(edtxtlist[6].text,'.')) != NULL)
      //            {
      //               ++cpos;
      //               fp = atoi(cpos);
      //            }
      //            else
      //            {
      //               fp = 0;
      //            }
      //            ip = atoi(edtxtlist[6].text);
      //            dev.factor = (ip * 100) + fp;
      break;
    default:
      msgbox("Invalid CEBus type.", "Warning", MB_OK);
      break;
    }
  } else {                                        //x-10
    dev.code_letter = edddblist[3].selected + 65; //x10 letter
    dev.code_number = edddblist[4].selected + 1;  //x10 number
    if (edddblist[0].selected >= 4) {
      dev.x10_device_type = edddblist[0].selected + 1; //x10 type
    } else {
      dev.x10_device_type = edddblist[0].selected; //x10 type
    }
    //if (edddblist[0].selected == X10_RCS_TSTAT)
    if (IsTstat(&dev) != FALSE) {
      dev.category = ENVIRONMENTAL_CATEGORY;
    }
  }

  if (IsSubmeter(&dev) == TRUE) {
    for (i = 0; i < dev_list.devices_used; i++) {
      if (IsSubmeter(&dev_list.devices[i]) == TRUE) {
        if ((dev.cebus_hc == dev_list.devices[i].cebus_hc) &&
            (dev.cebus_unit == dev_list.devices[i].cebus_unit) &&
            (dev.cebus_subunit == dev_list.devices[i].cebus_subunit) &&
            (dev.device_slot != dev_list.devices[i].device_slot)) {
          msgbox("This device is already defined.|Change its HC, Unit or Sub Unit|and try again.",
                 "Information", MB_OK);
          return;
        }
      }
    }
  }
#if LOAD_CONTROL_TECO
  switch (dev.priority) {
  case 0:
    for (i = 0; i < dev_list.devices_used; i++) {
      if ((IsSubmeter(&dev_list.devices[i]) == TRUE) &&
          (dev.monitor_hc == dev_list.devices[i].cebus_hc) &&
          (dev.monitor_subunit == dev_list.devices[i].cebus_subunit) &&
          (dev.monitor_unit == dev_list.devices[i].cebus_unit)) {
        mm_pri = dev_sublist[edddblist[7].selected]->priority;
        dev.monitor_hc = 0;
        dev.monitor_subunit = 0;
        dev.monitor_subunit = 0;
        dev_list.devices[i].priority = 0;
        if (SendObject(&dev_list.devices[i], 'C', sizeof(DEVICE), DEVICE_TABLE) == FAILURE) {
          msgbox("Unable to change corresponding|Sub Meter device's priority.|Please try again.",
                 "Warning",
                 MB_OK);
          dev_sublist[edddblist[7].selected]->priority = mm_pri;
          return;
        }
        break;
      }
    }
    break;
  case 1:
  case 2:
    if (IsSubmeter(&dev) == FALSE) {
      for (i = 0; i < dev_list.devices_used; i++) //search for previously attached sub meter and change it to 0
      {
        if ((IsSubmeter(&dev_list.devices[i]) == TRUE) &&
            (dev.monitor_hc == dev_list.devices[i].cebus_hc) &&
            (dev.monitor_subunit == dev_list.devices[i].cebus_subunit) &&
            (dev.monitor_unit == dev_list.devices[i].cebus_unit)) {
          if (dev_list.devices[i].device_slot != dev_sublist[edddblist[7].selected]->device_slot) {
            mm_pri = dev_list.devices[i].priority;
            dev_list.devices[i].priority = 0;
            if (SendObject(&dev_list.devices[i], 'C', sizeof(DEVICE), DEVICE_TABLE) == FAILURE) {
              msgbox("Unable to change corresponding|Sub Meter device's priority.|Try again.",
                     "Warning",
                     MB_OK);
              dev_list.devices[i].priority = mm_pri;
              return;
            }
            break;
          }
        }
      }
      dev.monitor_hc = dev_sublist[edddblist[7].selected]->cebus_hc;
      dev.monitor_unit = dev_sublist[edddblist[7].selected]->cebus_unit;
      dev.monitor_subunit = dev_sublist[edddblist[7].selected]->cebus_subunit;
      mm_pri = dev_sublist[edddblist[7].selected]->priority;
      dev_sublist[edddblist[7].selected]->priority = dev.priority;
      if (SendObject(dev_sublist[edddblist[7].selected], 'C', sizeof(DEVICE), DEVICE_TABLE) == FAILURE) {
        msgbox("Unable to change corresponding|Sub Meter device's priority.|Try again.",
               "Warning",
               MB_OK);
        dev_sublist[edddblist[7].selected]->priority = mm_pri;
        return;
      }
    }
    break;
  }
#endif

  if (SendObject(&dev, dev_option, sizeof(DEVICE), DEVICE_TABLE) == SUCCESS) {
    if (dev_option == 'C') {
      if ((i = LookupDev(dev.device_slot)) != -1) {
        dev_list.devices[i] = dev;
        if (user_backup() == SUCCESS) {
          Status("Device successfully changed.");
        } else {
          msgbox("Error updating file: devices.dat|Changes will not take effect.",
                 "Warning",
                 MB_OK);
        }
      } else {
        msgbox("Invalid device ID.|Device not changed.", "Warning", MB_OK);
      }
    } else //'A'
    {
      dev_list.devices[dev_list.devices_used] = dev;
      ++dev_list.devices_used;
      if (user_backup() == SUCCESS) {
        Status("Device successfully added.");
      } else {
        msgbox("Error updating file:  devices.dat|New device NOT saved to disk!",
               "Disk Space Low",
               MB_OK);
      }
    }
  }
}

void CheckBox1Click(void) {
  if ((secured_user == FALSE) && (this_dev.priority > 0)) {
    msgbox("Permission Denied!  Only an|authorized utility representative|may this change device type.",
           "Invalid Access Level", MB_OK);
    if (this_dev.code_letter != '@') { //x-10
      edddblist[3].selected = this_dev.code_letter - 65;
      edCheckBoxList[0].checked = TRUE;
      edCheckBoxList[1].checked = FALSE;
    } else { //cebus
      edddblist[3].selected = 0;
      edCheckBoxList[0].checked = FALSE;
      edCheckBoxList[1].checked = TRUE;
    }
    return;
  }
  edCheckBoxList[1].checked = FALSE;
  DisplayCheckBox(&edCheckBoxList[1]);
  edCheckBoxList[0].checked = TRUE;
  DisplayCheckBox(&edCheckBoxList[0]);
  DisplayX10(&this_dev);
}

void CheckBox2Click(void) {
  if ((secured_user == FALSE) && (this_dev.priority > 0)) {
    msgbox("Permission Denied!  Only an|authorized utility representative|may this change device type.",
           "Invalid Access Level", MB_OK);
    if (this_dev.code_letter != '@') { //x-10
      edddblist[3].selected = this_dev.code_letter - 65;
      edCheckBoxList[0].checked = TRUE;
      edCheckBoxList[1].checked = FALSE;
    } else { //cebus
      edddblist[3].selected = 0;
      edCheckBoxList[0].checked = FALSE;
      edCheckBoxList[1].checked = TRUE;
    }
    return;
  }
  edCheckBoxList[0].checked = FALSE;
  DisplayCheckBox(&edCheckBoxList[0]);
  edCheckBoxList[1].checked = TRUE;
  DisplayCheckBox(&edCheckBoxList[1]);
  DisplayCEBus(&this_dev);
}

void PriorityChange(void) {
  if ((edCheckBoxList[1].checked == TRUE) && (edddblist[1].selected == 3)) {
#if LOAD_CONTROL_TECO
    msgbox("InterLane Sub Meter device|priority will be set to|match the device it monitors.",
           "Information", MB_OK);
    edddblist[2].selected = this_dev.priority;
    DisplayDropDown(&edddblist[2]);
    return;
#else
    msgbox("Monitoring devices|cannot be controlled",
           "Information", MB_OK);
    edddblist[2].selected = 0;
    DisplayDropDown(&edddblist[2]);
    return;
#endif
  }
  if (secured_user == TRUE) {
#if LOAD_CONTROL_TECO
    if (edddblist[2].selected != 0) {
      DisplayGetMonitor();
    } else {
      area_clear(270, 400, 290, 625, FG_WHT);
    }
#endif
  } else {
    msgbox("Permission Denied!  Only an|authorized utility representative|may change priority.",
           "Invalid Access Level", MB_OK);
    edddblist[2].selected = this_dev.priority;
    DisplayDropDown(&edddblist[2]);
  }
}

void CebusTypeChange(void) {
  if (edddblist[1].selected == 3) //if m&m
  {
    if (secured_user == TRUE) {
      edddblist[2].selected = 0;
      DisplayDropDown(&edddblist[2]);
      edddblist[7].so.visible = FALSE;
      area_clear(270, 400, 290, 625, FG_WHT);
      edddblist[2].selected = 0;
      DisplayMeterModule();
    } else {
      msgbox("Permission Denied!  Only an|authorized utility representative|may add InterLane Sub Meter devices.",
             "Invalid Access Level", MB_OK);
      edddblist[1].selected = 0;
      DisplayDropDown(&edddblist[1]);
    }
  } else {
    area_clear(270, 400, 290, 625, FG_WHT);
  }
}

int IsDimmer(DEVICE* dev) {
  if (dev->code_letter != '@') { //x10
    if ((dev->x10_device_type == X10_DIMMER) || (dev->x10_device_type == X10_DECORA)) {
      return TRUE;
    }
  } else { //cebus
    if (dev->cebus_type == 2) {
      return TRUE;
    }
  }
  return FALSE;
}

int IsTstat(DEVICE* dev) {
  int rc;

  rc = FALSE;
  if (dev->code_letter != '@') { //x10
    if (dev->x10_device_type == X10_RCS_TSTAT) {
      rc = X10_RCS_TSTAT;
    } else if (dev->x10_device_type == X10_RCS_CENTI_TSTAT) {
      rc = X10_RCS_CENTI_TSTAT;
    }
  } else { //cebus
  }
  return rc;
}

int LookupDev(USHORT device_id) {
  int i;

  for (i = 0; i < dev_list.devices_used; ++i) {
    if (dev_list.devices[i].device_slot == device_id) {
      return i;
    }
  }
  return -1;
}

void DisplayDeviceLong(DEVICE* dev, int top, int left, UCHAR color) {
  UCHAR desc[81];
  UCHAR* x10_type[7] =
      {
          "X10 Relay",
          "X10 Dimmer",
          "X10 Annunciator",
          "X10 RCS F\xA4 TStat",
          "X10 RCS 2 Way TStat",
          "X10 Decora Dimmer",
          "X10 RCS C\xA4 TStat"};

  UCHAR* CB_type[5] =
      {
          "CEBus Dimmer",
          "CEBus Relay",
          "CEBus Load Shed",
          "CEBus OLD MM",
          "CEBus TeCom MM"};

  memset(desc, 0, 81);
  if (dev->code_letter != '@') {
    //X-10
    sprintf(desc, "%.13s %.18s %1d   %-19.19s X10: %c %2d",
            dev->desc,
            dev->location,
            dev->priority,
            x10_type[dev->x10_device_type],
            dev->code_letter,
            dev->code_number);
  } else {
    //CEBus
    sprintf(desc, "%.13s %.18s %1d   %-19.19s CEB: %04X:%04X %1d",
            dev->desc,
            dev->location,
            dev->priority,
            CB_type[dev->cebus_type - 2],
            dev->cebus_hc,
            dev->cebus_unit,
            dev->cebus_subunit);
  }
  ptext(desc, top, left, color);
}

void DisplayDeviceShort(DEVICE* dev, int top, int left, UCHAR color) {
  UCHAR desc[45];

  memset(desc, 0, 45);
  sprintf(desc, "%.13s - %.18s",
          dev->desc,
          dev->location);
  //   if (dev->code_letter != '@')
  //   {
  //      //X-10
  //      sprintf(desc,    "X10   %.13s %.18s",
  //                  dev->desc,
  //                  dev->location);
  //   }
  //   else
  //   {
  //      //CEBus
  //      sprintf(desc,    "CEBus %.13s %.18s",
  //                  dev->desc,
  //                  dev->location);
  //   }
  ptext(desc, top, left, color);
}

void button1(void) {
  glbWindow = NULL;

  if ((IsSubmeter(&this_dev)) && (secured_user != TRUE)) {
    msgbox("Permission Denied!  Only|an authorized utility representative|may change InterLane Sub Meters.",
           "Invalid Access Level", MB_OK);

  } else {
    SaveDeviceChanges();
  }
  Device_Event_Menu(current_cat);
}

void button2(void) {
  Device_Event_Menu(current_cat);
}

void DisplayGetMonitor(void) {
  int i = 0;
  int j = 0;
  int k = 0;

#if LOAD_CONTROL_TECO
  memset(mm_strings, 0, 340);
  for (i = 0; i < dev_list.devices_used; i++) {
    if (IsSubmeter(&dev_list.devices[i]) == TRUE) {
      if (dev_list.devices[i].priority == 0) {

        sprintf(mm_strings[j], "%.13s %.18s",
                dev_list.devices[i].desc,
                dev_list.devices[i].location);
        dev_sublist[j] = &dev_list.devices[i];
        mm_str2[j] = mm_strings[j];
        j++;
      } else {
        if ((this_dev.monitor_hc == dev_list.devices[i].cebus_hc) &&
            (this_dev.monitor_subunit == dev_list.devices[i].cebus_subunit) &&
            (this_dev.monitor_unit == dev_list.devices[i].cebus_unit)) {
          sprintf(mm_strings[j], "%.13s %.18s",
                  dev_list.devices[i].desc,
                  dev_list.devices[i].location);
          dev_sublist[j] = &dev_list.devices[i];
          mm_str2[j] = mm_strings[j];
          k = j;
          j++;
        }
      }
    }
  }
  if (j == 0) {
    msgbox("There are no Sub Meter devices available.|Priority cannot be greater than 0 w/o|a corresponding Sub Meter device.",
           "Information",
           MB_OK);
    edddblist[2].selected = 0;
    DisplayDropDown(&edddblist[2]);
    return;
  }

  //DisplayDeviceLong(dev_sublist[0], 100,10, BG_BLK + FG_HWHT);
  edddblist[7].selected = k;
  edddblist[7].count = j;
  edddblist[7].so.visible = TRUE;
  DisplayDropDown(&edddblist[7]);
  GroupBox("Sub Meter Device", 280, 305, 110, 315);
#endif
}

static void MonitorDropOnChange(void) {
#if LOAD_CONTROL_TECO
  if (secured_user == FALSE) {
    msgbox("Permission Denied!  Only an|authorized utility representative|may change Sub Meter devices.",
           "Invalid Access Level", MB_OK);
    DisplayGetMonitor();
  }
#endif
}

int IsSubmeter(DEVICE* dev) {
  if ((dev->cebus_type == 5 || dev->cebus_type == 6) && (dev->code_letter == '@')) {
    return TRUE;
  }
  return FALSE;
}

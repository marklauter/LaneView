//DeviceMenuWindow
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>
#include <conio.h>
#include <math.h>
#include <string.h>

#include "cebus.h"
#include "2box.h"
#include "inside.h"
#include "vga.h"
#include "scrnobjt.h"
#include "pshbtn.h"
#include "checkbox.h"
#include "lwindow.h"
#include "ismenu.h"
#include "moodmenu.h"
#include "frame_3d.h"
#include "devcat.h"
#include "inside.h"
#include "dvc_evnt.h"
#include "ctrlschd.h"
#include "calendar.h"
#include "msgbox.h"
#include "inio.h"
#include "inutil.h"
#include "dropdown.h"
#include "textbox.h"
#include "spinner.h"
#include "device.h"
#include "mouse.h"
#include "device.h"

static void SetTimeClick(void);
static void SaveOnClick(void);

extern int in_phone_view;

struct tm* tod;
char confirmation;
char ADVANCED;

static EMAIL_INDICATORS_T email_def;

extern int outside_time(struct tm* outside_tm); //from inio.c
extern void SetTime(void);                      //from inio.c
static void CbClick(void);
static void SetSystemMode(void);
static void ddSetDayType(void);
static void SetupWindowDraw(void);
static void ClearPassWord(void);
static void SetPassWord(void);
static void HVACSetbackWindowDraw(void);
static void DefSetBacks(void);
static void EmailSetup(void);
static void ThresholdDraw(void);
static void ThresholdCancel(void);
static void ThresholdSave(void);
static void ThresholdSetup(void);
static void ddSetHVACMode(void);
static void ddSetsecurityMode(void);
static void ddSetSeaonMode(void);
static void InterfaceModeOnChange(void);

extern INTERVAL_DATA$ interval$;
extern int SendTable(int table_no, void* request_data, int data_length);

static CHECK_BOX_T checkbox =
    {
        430, 146, 0, 0,      // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Enable Confirmation",
        TRUE,
        CbClick};

TEXT_BOX_T txtpassword =
    {
        //////////////////////////////////////begin screen object definition
        0, 0, 0, 14,  // position
        TRUE,         // got_focus
        TEXT_BOX,     // type
        TRUE,         // visible
        TxtMouseDown, // OnMouseDown()
        TxtMouseUp,   // OnMouseUp()
        TxtClick,     // OnClick()
        TxtKeyPress,  // OnKeyPress()
        PutTxtBox,    // DisplayObject()
        //////////////////////////////////////end screen object definition
        20,           // row
        43,           // column
        8,            // box length
        tb_pass_word, // type
        "",           // text
        NULL};

static PUSH_BUTTON_T __far dmButton[9] = {
    // BEGIN SCREEN OBJECT DEFINITION
    92, 338, 150, 25, // position
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
    "Setup Rate Tiers", //text
    push,               //type
    up,                 //state
    NULL,               //icon
    ThresholdSetup,     //function
    187,

    // BEGIN SCREEN OBJECT DEFINITION
    169, 338, 150, 25, // position
    FALSE,             // focus
    PUSH_BUTTON,       // type
    TRUE,              // visible
    BtnMouseDown,      // OnMouseDown()
    BtnMouseUp,        // OnMouseUp()
    BtnClick,          // OnClick()
    BtnKeyPress,       // OnKeyPress()
    PutButton,         // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,            //default button
    "ICU Time to PC", //text
    push,             //type
    up,               //state
    NULL,             //icon
    SetTimeClick,     //function
    188,

    // BEGIN SCREEN OBJECT DEFINITION
    264, 425, 50, 20, // position
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
    "Clear",       //text
    push,          //type
    up,            //state
    NULL,          //icon
    ClearPassWord, //function
    189,

    288, 425, 50, 20, // position
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
    "Apply",     //text
    push,        //type
    up,          //state
    NULL,        //icon
    SetPassWord, //function
    190,

    // BEGIN SCREEN OBJECT DEFINITION
    343, 338, 150, 25, // position
    FALSE,             // focus
    PUSH_BUTTON,       // type
    TRUE,              // visible
    BtnMouseDown,      // OnMouseDown()
    BtnMouseUp,        // OnMouseUp()
    BtnClick,          // OnClick()
    BtnKeyPress,       // OnKeyPress()
    PutButton,         // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,             //default button
    "Define Setbacks", //text
    push,              //type
    up,                //state
    NULL,              //icon
    DefSetBacks,       //function
    191,

    // BEGIN SCREEN OBJECT DEFINITION
    412, 338, 150, 25, // position
    FALSE,             // focus
    PUSH_BUTTON,       // type
    TRUE,              // visible
    BtnMouseDown,      // OnMouseDown()
    BtnMouseUp,        // OnMouseUp()
    BtnClick,          // OnClick()
    BtnKeyPress,       // OnKeyPress()
    PutButton,         // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,          //default button
    "E-mail Setup", //text
    push,           //type
    up,             //state
    NULL,           //icon
    EmailSetup,     //function
    191,

    // BEGIN SCREEN OBJECT DEFINITION
    265, 430, 150, 25, // position
    FALSE,             // focus
    PUSH_BUTTON,       // type
    FALSE,             // visible
    BtnMouseDown,      // OnMouseDown()
    BtnMouseUp,        // OnMouseUp()
    BtnClick,          // OnClick()
    BtnKeyPress,       // OnKeyPress()
    PutButton,         // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,             //default button
    "PhoneView Setup", //text
    push,              //type
    up,                //state
    NULL,              //icon
    NULL,              //function
    192,

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
    193,

    350, 155, 145, 25, // position
    FALSE,             // focus
    PUSH_BUTTON,       // type
    TRUE,              // visible
    BtnMouseDown,      // OnMouseDown()
    BtnMouseUp,        // OnMouseUp()
    BtnClick,          // OnClick()
    BtnKeyPress,       // OnKeyPress()
    PutButton,         // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,            //default button
    "Apply Schedule", //text
    push,             //type
    up,               //state
    NULL,             //icon
    SaveOnClick,      //function
    194};

UCHAR* day_type_list[7] =
    {
        "Sunday",
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday"};

static char* sysmodes[3] = {"Normal", "Vacation", "Monitor"};
static DROPDOWN_BOX_T ddsysmode =
    {
        115, 155, 145, 17,
        FALSE,             // got_focus
        DROP_DOWN_BOX,     // type
        TRUE,              // visible
        DropDownMouseDown, // OnMouseDown()
        DropDownMouseUp,   // OnMouseUp()
        DropDownClick,     // OnClick()
        DropDownKeyPress,  // OnKeyPress()
        DisplayDropDown,   // DisplayObject()
        sysmodes,          // strings
        0,                 // item selected
        FALSE,             // dropped
        3,                 // item count
        NULL               // OnChange
};

static DROPDOWN_BOX_T dddaytype =
    {
        265, 155, 145, 17,
        FALSE,             // got_focus
        DROP_DOWN_BOX,     // type
        TRUE,              // visible
        DropDownMouseDown, // OnMouseDown()
        DropDownMouseUp,   // OnMouseUp()
        DropDownClick,     // OnClick()
        DropDownKeyPress,  // OnKeyPress()
        DisplayDropDown,   // DisplayObject()
        day_type_list,     // strings
        0,                 // item selected
        FALSE,             // dropped
        7,                 // item count
        NULL               // OnChange
};

static char* seasons[4] = {"Winter", "Autumn", "Summer", "Spring"};
static DROPDOWN_BOX_T ddseason =
    {
        215, 155, 145, 17,
        FALSE,             // got_focus
        DROP_DOWN_BOX,     // type
        TRUE,              // visible
        DropDownMouseDown, // OnMouseDown()
        DropDownMouseUp,   // OnMouseUp()
        DropDownClick,     // OnClick()
        DropDownKeyPress,  // OnKeyPress()
        DisplayDropDown,   // DisplayObject()
        seasons,           // strings
        0,                 // item selected
        FALSE,             // dropped
        4,                 // item count
        NULL               // OnChange
};

static char* hvac[2] = {"Cooling", "Heating"};
static DROPDOWN_BOX_T ddhvac =
    {
        165, 155, 145, 17,
        FALSE,             // got_focus
        DROP_DOWN_BOX,     // type
        TRUE,              // visible
        DropDownMouseDown, // OnMouseDown()
        DropDownMouseUp,   // OnMouseUp()
        DropDownClick,     // OnClick()
        DropDownKeyPress,  // OnKeyPress()
        DisplayDropDown,   // DisplayObject()
        hvac,              // strings
        0,                 // item selected
        FALSE,             // dropped
        2,                 // item count
        NULL               // OnChange
};

static char* security[2] = {"Temp Away", "At Home"};
static DROPDOWN_BOX_T ddsecurity =
    {
        315, 155, 145, 17,
        FALSE,             // got_focus
        DROP_DOWN_BOX,     // type
        TRUE,              // visible
        DropDownMouseDown, // OnMouseDown()
        DropDownMouseUp,   // OnMouseUp()
        DropDownClick,     // OnClick()
        DropDownKeyPress,  // OnKeyPress()
        DisplayDropDown,   // DisplayObject()
        security,          // strings
        0,                 // item selected
        FALSE,             // dropped
        2,                 // item count
        NULL               // OnChange
};

static char* interfacemode[2] = {"Beginner", "Advanced User"};
static DROPDOWN_BOX_T ddinterfacemode =
    {
        395, 166, 122, 17,
        FALSE,                // got_focus
        DROP_DOWN_BOX,        // type
        TRUE,                 // visible
        DropDownMouseDown,    // OnMouseDown()
        DropDownMouseUp,      // OnMouseUp()
        DropDownClick,        // OnClick()
        DropDownKeyPress,     // OnKeyPress()
        DisplayDropDown,      // DisplayObject()
        interfacemode,        // strings
        0,                    // item selected
        FALSE,                // dropped
        2,                    // item count
        InterfaceModeOnChange // OnChange
};

static SCREEN_OBJECT_T* dmSo[17];

static LWINDOW_T dmWindow =
    {
        35, 0, 640, 445,          // window position
        dmSo,                     // screen object
        SetupWindowDraw,          // window draw
        17,                       // item count
        0,                        // cursor position 0-79 left to right
        0,                        // element that has current focus
        fcsBtn,                   // focus type
        "System Setup & Control", // title
        DefaultOnKeyPress         // OnKeyPress();
};

//System Setup Screen
void MasterControls(void) {
  int i;
  UCHAR mode_tbl[3];

  Busy();

  dmSo[0] = &ddsysmode.so;
  dmSo[1] = &ddhvac.so;
  dmSo[2] = &ddseason.so;
  dmSo[3] = &dddaytype.so;

  dmSo[4] = &ddsecurity.so;
  dmSo[5] = &dmButton[7].so;

  dmSo[6] = &dmButton[0].so;
  dmSo[7] = &dmButton[1].so;

  dmSo[8] = &txtpassword.so;

  dmSo[9] = &dmButton[2].so;

  dmSo[10] = &dmButton[3].so;
  dmSo[11] = &dmButton[4].so;
  dmSo[12] = &dmButton[5].so;
  dmSo[13] = &dmButton[7].so;

  dmSo[14] = &checkbox.so;

  dmSo[15] = &ddinterfacemode.so;
  ddinterfacemode.selected = ADVANCED;

  dmSo[16] = &dmButton[8].so;

  if (ADVANCED == FALSE) {
    dddaytype.so.visible = FALSE;
    ddseason.so.visible = FALSE;
    ddsecurity.so.visible = FALSE;

    dmButton[0].so.visible = FALSE;
    dmButton[1].so.visible = FALSE;

    checkbox.so.visible = FALSE; // delete confirmation
  } else {
    dddaytype.so.visible = TRUE;
    ddseason.so.visible = TRUE;
    ddsecurity.so.visible = TRUE;

    dmButton[0].so.visible = TRUE;
    dmButton[1].so.visible = TRUE;

    checkbox.so.visible = TRUE; // delete confirmation
  }

  if (GetTable(mode_tbl,
               3,
               GET_SYSTEM_MODES_TBL,
               NULL,
               0,
               RETRY_COUNT,
               FALSE) == FAILURE) {
    interval$.mode_byte1 = 0;
    interval$.mode_byte2 = 0;
    interval$.mode_byte3 = 0;
  } else {
    interval$.mode_byte1 = mode_tbl[0];
    interval$.mode_byte2 = mode_tbl[1];
    interval$.mode_byte3 = mode_tbl[2];
  }

  glbWindow = &dmWindow;

  checkbox.checked = confirmation;

  if (current_day_type == 0) {
    current_day_type = 1 << tod->tm_wday;
  }

  for (i = 0; i < 7; i++) {
    if (current_day_type & (0x01 << i)) {
      dddaytype.selected = i;
      break;
    }
  }
  ddsysmode.selected = 2;
  for (i = 0; i < 8; i++) {
    if (interval$.mode_byte1 & (0x01 << i)) {
      switch (i) {
      case 0: // winter mode
      case 1: // autumn mode
      case 2: // summer mode
      case 3: // spring mode
        ddseason.selected = i;
        break;
      case 4: // cooling mode
      case 5: // heating mode
        ddhvac.selected = i - 4;
        break;
      case 6: // normal mode
      case 7: // vacation mode
        ddsysmode.selected = i - 6;
        break;
      }
    }
  }
  for (i = 0; i < 8; i++) {
    if (interval$.mode_byte2 & (0x01 << i)) {
      switch (i) {
      case 0: // N/A
      case 1: // N/A
        break;
      case 2: // away mode
      case 3: // home mode
        ddsecurity.selected = i - 2;
        break;
      case 4: // low cost mode
      case 5: // medium cost mode
      case 6: // high cost mode
      case 7: // critical mode
        break;
      }
    }
  }

  //   if (in_phone_view == TRUE)
  //   {
  //      dmButton[4].so.visible = TRUE;
  //   }
  //   else
  //   {
  //      dmButton[4].so.visible = FALSE;
  //   }

  PutWindow(glbWindow);
  NotBusy();
}

void SetupWindowDraw(void) {
  struct tm outside_tm;
  struct tm* tod;
  char time_buffer[80];

  GroupBox("Schedule Selection", 70, 138, 317, 175);
  ptext("Master Schedule", 98, 155, FG_BLK + BG_WHT);
  ptext("HVAC Schedule", 148, 155, FG_BLK + BG_WHT);

  GroupBox("Password", 250, 325, 63, 175);
  GroupBox("HVAC Setbacks", 320, 325, 60, 175);
  GroupBox("E-mail Indicators", 390, 325, 55, 175);

  if (ADVANCED == TRUE) {
    Busy();
    ptext("Day Schedule", 248, 155, FG_BLK + BG_WHT);
    ptext("Season Schedule", 198, 155, FG_BLK + BG_WHT);
    ptext("Occupied Schedule", 298, 155, FG_BLK + BG_WHT);
    GroupBox("Rate Based Control", 70, 325, 60, 175);
    GroupBox("Update Time From ICU", 140, 325, 105, 175);
    memset(time_buffer, 0, 80);
    outside_time(&outside_tm);
    sprintf(time_buffer,
            "ICU Time: %2d:%02d   ",
            outside_tm.tm_hour,
            outside_tm.tm_min);
    ptext(time_buffer, 206, 350, FG_BLK + BG_WHT);
    time(&ltime);
    tod = localtime(&ltime);
    sprintf(time_buffer,
            "PC Time:  %2d:%02d   ",
            tod->tm_hour,
            tod->tm_min);
    ptext(time_buffer, 222, 350, FG_BLK + BG_WHT);
    NotBusy();
  }
}

void CbClick(void) {
  FILE* stream;
  confirmation = checkbox.checked;
  if ((stream = fopen("setup.ini", "wb")) != NULL) {
    fwrite(&confirmation, sizeof(int), 1, stream);
    fclose(stream);
  }
}

static void ddSetDayType(void) {
  int i;

  i = 1 << dddaytype.selected;

  if (SetDayType((UCHAR)(dddaytype.selected + 1)) == SUCCESS) {
    current_day_type = i;
    Status("Day Schedule successfully set.");
  } else {
    msgbox("Unable to set day Schedule.", "Failure", MB_OK);
    for (i = 0; i < 7; i++) {
      if (current_day_type & (0x01 << i)) {
        dddaytype.selected = i;
        break;
      }
    }
    DisplayDropDown(&dddaytype);
  }
}

static void ClearPassWord(void) {
  memset(txtpassword.text, 0, MAX_TEXT);
  PutTxtBox(&txtpassword);
  if (SendTable(PASSWORD_TBL, txtpassword.text, 8) == SUCCESS) {
    Status("Password cleared successfully");
  } else {
    msgbox("Password not cleared.|Please try again.", "Password Error", MB_OK);
  }
}

static void SetPassWord(void) {
  if (SendTable(PASSWORD_TBL, txtpassword.text, 8) == SUCCESS) {
    Status("Password set successfully");
  } else {
    msgbox("Password not set.|Please try again.", "Password Error", MB_OK);
  }
}

//=======================================================================================
//
// HVAC SETBACK SECTION START
//
//=======================================================================================

static void ddhvacselectOnChange(void);
char* HVACavail[MAX_DEVICES];
char __far availHVAC[MAX_DEVICES][14];
USHORT __far hvac_index[MAX_DEVICES];
static DROPDOWN_BOX_T ddhvacselect =
    {
        130, 250, 145, 17,
        FALSE,               // got_focus
        DROP_DOWN_BOX,       // type
        TRUE,                // visible
        DropDownMouseDown,   // OnMouseDown()
        DropDownMouseUp,     // OnMouseUp()
        DropDownClick,       // OnClick()
        DropDownKeyPress,    // OnKeyPress()
        DisplayDropDown,     // DisplayObject()
        HVACavail,           // strings
        0,                   // item selected
        FALSE,               // dropped
        0,                   // item count
        ddhvacselectOnChange // OnChange
};

static void SetBackSaveClick(void);
static PUSH_BUTTON_T cbutton =
    {
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
        FALSE,            // default button
        "Cancel",         //text
        push,             // type
        up,               // state
        NULL,             // icon
        MasterControls,   //function
        195};

static PUSH_BUTTON_T sbutton =
    {
        419, 445, 89, 33, // position
        FALSE,            // focus
        PUSH_BUTTON,      // type
        TRUE,             // visible
        BtnMouseDown,     // OnMouseDown()
        BtnMouseUp,       // OnMouseUp()
        BtnClick,         // OnClick()
        BtnKeyPress,      // OnKeyPress()
        PutButton,        // DisplayObject()
        FALSE,            // default button
        "Apply",          // text
        push,             // type
        up,               // state
        NULL,             // icon
        SetBackSaveClick, // function
        196};

static void SpinHvacOnChange(void);
SPINNER_T spin_cool[4] =
    {
        200, 160, 1, 1,   // position
        FALSE,            // focus
        TIME_ENTRY,       // type
        TRUE,             // visible
        SpinnerMouseDown, // OnMouseDown()
        SpinnerMouseUp,   // OnMouseUp()
        SpinnerClick,     // OnClick()
        SpinnerKeyPress,  // OnKeyPress()
        DisplaySpinner,   // DisplayObject()
        0,                // value
        40,               // max
        0,                // min
        1,                // increment
        SpinHvacOnChange,

        240, 160, 1, 1,   // position
        FALSE,            // focus
        TIME_ENTRY,       // type
        TRUE,             // visible
        SpinnerMouseDown, // OnMouseDown()
        SpinnerMouseUp,   // OnMouseUp()
        SpinnerClick,     // OnClick()
        SpinnerKeyPress,  // OnKeyPress()
        DisplaySpinner,   // DisplayObject()
        0,                // value
        40,               // max
        0,                // min
        1,                // increment
        SpinHvacOnChange,

        280, 160, 1, 1,   // position
        FALSE,            // focus
        TIME_ENTRY,       // type
        TRUE,             // visible
        SpinnerMouseDown, // OnMouseDown()
        SpinnerMouseUp,   // OnMouseUp()
        SpinnerClick,     // OnClick()
        SpinnerKeyPress,  // OnKeyPress()
        DisplaySpinner,   // DisplayObject()
        0,                // value
        40,               // max
        0,                // min
        1,                // increment
        SpinHvacOnChange,

        320, 160, 1, 1,   // position
        FALSE,            //focus
        TIME_ENTRY,       //type
        TRUE,             // visible
        SpinnerMouseDown, //OnMouseDown()
        SpinnerMouseUp,   //OnMouseUp()
        SpinnerClick,     // OnClick()
        SpinnerKeyPress,  // OnKeyPress()
        DisplaySpinner,   // DisplayObject()
        0,                //value
        40,               //max
        0,                //min
        1,                //increment
        SpinHvacOnChange};

static SPINNER_T spin_heat[4] =
    {
        200, 345, 1, 1,   // position
        FALSE,            //focus
        TIME_ENTRY,       //type
        TRUE,             // visible
        SpinnerMouseDown, //OnMouseDown()
        SpinnerMouseUp,   //OnMouseUp()
        SpinnerClick,     // OnClick()
        SpinnerKeyPress,  // OnKeyPress()
        DisplaySpinner,   // DisplayObject()
        0,                //value
        40,               //max
        0,                //min
        1,                //increment
        SpinHvacOnChange,

        240, 345, 1, 1,   // position
        FALSE,            //focus
        TIME_ENTRY,       //type
        TRUE,             // visible
        SpinnerMouseDown, //OnMouseDown()
        SpinnerMouseUp,   //OnMouseUp()
        SpinnerClick,     // OnClick()
        SpinnerKeyPress,  // OnKeyPress()
        DisplaySpinner,   // DisplayObject()
        0,                //value
        40,               //max
        0,                //min
        1,                //increment
        SpinHvacOnChange,

        280, 345, 1, 1,   // position
        FALSE,            //focus
        TIME_ENTRY,       //type
        TRUE,             // visible
        SpinnerMouseDown, //OnMouseDown()
        SpinnerMouseUp,   //OnMouseUp()
        SpinnerClick,     // OnClick()
        SpinnerKeyPress,  // OnKeyPress()
        DisplaySpinner,   // DisplayObject()
        0,                //value
        40,               //max
        0,                //min
        1,                //increment
        SpinHvacOnChange,

        320, 345, 1, 1,   // position
        FALSE,            //focus
        TIME_ENTRY,       //type
        TRUE,             // visible
        SpinnerMouseDown, //OnMouseDown()
        SpinnerMouseUp,   //OnMouseUp()
        SpinnerClick,     // OnClick()
        SpinnerKeyPress,  // OnKeyPress()
        DisplaySpinner,   // DisplayObject()
        0,                //value
        40,               //max
        0,                //min
        1,                //increment
        SpinHvacOnChange};

static SCREEN_OBJECT_T* hsso[11];
static LWINDOW_T hvac_setbacks =
    {
        35, 0, 640, 445,             // window position
        hsso,                        // screen object
        HVACSetbackWindowDraw,       // window draw
        11,                          // item count
        0,                           // cursor position 0-79 left to right
        0,                           // element that has current focus
        fcsBtn,                      // focus type
        "HVAC Temperature Setbacks", // title
        DefaultOnKeyPress            // OnKeyPress();
};

static void DefSetBacks(void) {
  int i;
  int j;

  hsso[0] = &ddhvacselect.so;
  hsso[1] = &spin_cool[0].so;
  hsso[2] = &spin_cool[1].so;
  hsso[3] = &spin_cool[2].so;
  hsso[4] = &spin_cool[3].so;
  hsso[5] = &spin_heat[0].so;
  hsso[6] = &spin_heat[1].so;
  hsso[7] = &spin_heat[2].so;
  hsso[8] = &spin_heat[3].so;
  hsso[9] = &sbutton.so;
  hsso[10] = &cbutton.so;

  memset(availHVAC, 0, 20 * 14);
  ddhvacselect.count = 0;
  for (i = 0, j = 0; i < dev_list.devices_used; i++) {
    if (IsTstat(&dev_list.devices[i]) != FALSE) {
      strncpy(availHVAC[j], dev_list.devices[i].desc, 13);
      HVACavail[j] = availHVAC[j];
      hvac_index[j] = i;
      ddhvacselect.count++;
      if (++j > MAX_DEVICES) {
        break;
      }
    }
  }

  spin_heat[0].value = dev_list.devices[hvac_index[ddhvacselect.selected]].heating_setbacks[3];
  spin_heat[1].value = dev_list.devices[hvac_index[ddhvacselect.selected]].heating_setbacks[2];
  spin_heat[2].value = dev_list.devices[hvac_index[ddhvacselect.selected]].heating_setbacks[1];
  spin_heat[3].value = dev_list.devices[hvac_index[ddhvacselect.selected]].heating_setbacks[0];
  spin_cool[0].value = dev_list.devices[hvac_index[ddhvacselect.selected]].cooling_setbacks[3];
  spin_cool[1].value = dev_list.devices[hvac_index[ddhvacselect.selected]].cooling_setbacks[2];
  spin_cool[2].value = dev_list.devices[hvac_index[ddhvacselect.selected]].cooling_setbacks[1];
  spin_cool[3].value = dev_list.devices[hvac_index[ddhvacselect.selected]].cooling_setbacks[0];

  glbWindow = &hvac_setbacks;
  PutWindow(&hvac_setbacks);
}

static void HVACSetbackWindowDraw(void) {
  GroupBox("Cooling Setbacks", 162, 143, 215, 176);
  GroupBox("Heating Setbacks", 162, 330, 215, 176);

  ptext("HVAC Selection", 115, 250, FG_BLK + BG_TRANSPARENT);

  ptext("Critical Cost Level", 185, 160, FG_BLK + BG_TRANSPARENT);
  ptext("High Cost Level", 226, 160, FG_BLK + BG_TRANSPARENT);
  ptext("Medium Cost Level", 266, 160, FG_BLK + BG_TRANSPARENT);
  ptext("Low Cost Level", 306, 160, FG_BLK + BG_TRANSPARENT);

  ptext("DEGREES", 185 + 16, 213, FG_BLK + BG_TRANSPARENT);
  ptext("DEGREES", 226 + 16, 213, FG_BLK + BG_TRANSPARENT);
  ptext("DEGREES", 266 + 16, 213, FG_BLK + BG_TRANSPARENT);
  ptext("DEGREES", 306 + 16, 213, FG_BLK + BG_TRANSPARENT);

  ptext("(Gets Warmer)", 352, 180, FG_BLK + BG_TRANSPARENT);

  ptext("Critical Cost Level", 185, 345, FG_BLK + BG_TRANSPARENT);
  ptext("High Cost Level", 226, 345, FG_BLK + BG_TRANSPARENT);
  ptext("Medium Cost Level", 266, 345, FG_BLK + BG_TRANSPARENT);
  ptext("Low Cost Level", 306, 345, FG_BLK + BG_TRANSPARENT);

  ptext("DEGREES", 185 + 16, 398, FG_BLK + BG_TRANSPARENT);
  ptext("DEGREES", 226 + 16, 398, FG_BLK + BG_TRANSPARENT);
  ptext("DEGREES", 266 + 16, 398, FG_BLK + BG_TRANSPARENT);
  ptext("DEGREES", 306 + 16, 398, FG_BLK + BG_TRANSPARENT);

  ptext("(Gets Cooler)", 352, 369, FG_BLK + BG_TRANSPARENT);

  ptext("Setback units are in degrees.", 385, 220, FG_BLK + BG_TRANSPARENT);
}

static SPINNER_T __far spthreshold[3] =
    {
        180, 270, 1, 1,   // position
        TRUE,             //focus
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
        NULL,

        206, 270, 1, 1,   // position
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
        NULL,

        231, 270, 1, 1,   // position
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
        NULL};

static PUSH_BUTTON_T btnthreshold[2] =
    {                     // BEGIN SCREEN OBJECT DEFINITION
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
        FALSE,         //default button
        "Apply",       //text
        push,          //type
        up,            //state
        NULL,          //icon
        ThresholdSave, //function
        197,

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
        FALSE,           //default button
        "Cancel",        //text
        push,            //type
        up,              //state
        NULL,            //icon
        ThresholdCancel, //function
        198};

static SCREEN_OBJECT_T* rtsso[5];
static LWINDOW_T rate_threshold_setup_window =
    {
        35, 0, 640, 445,       // window position
        rtsso,                 // screen object
        ThresholdDraw,         // window draw
        5,                     // item count
        0,                     // cursor position 0-79 left to right
        0,                     // element that has current focus
        fcsBtn,                // focus type
        "Rate Schedule Setup", // title
        DefaultOnKeyPress      // OnKeyPress();
};

typedef struct
{
  ULONG critical;
  ULONG high;
  ULONG medium;
} THRESHOLD_TBL;
THRESHOLD_TBL threshold_tbl;

static void ThresholdSetup(void) {
  rtsso[0] = &spthreshold[0].so;
  rtsso[1] = &spthreshold[1].so;
  rtsso[2] = &spthreshold[2].so;
  rtsso[3] = &btnthreshold[0].so;
  rtsso[4] = &btnthreshold[1].so;

  glbWindow = &rate_threshold_setup_window;
  PutWindow(glbWindow);

  Busy();
  memset(&threshold_tbl, 0, sizeof(THRESHOLD_TBL));

  if (GetTable(&threshold_tbl,
               sizeof(THRESHOLD_TBL),
               RATE_LEVEL_POINTS,
               NULL,
               0,
               2,
               FALSE) == FAILURE) {
    msgbox("Unable to get rate|thresholds from ICU.", "Warning", MB_OK);
    spthreshold[0].value = 0;
    spthreshold[1].value = 0;
    spthreshold[2].value = 0;
  } else {
    spthreshold[0].value = (int)(threshold_tbl.critical / 1000L);
    spthreshold[1].value = (int)(threshold_tbl.high / 1000L);
    spthreshold[2].value = (int)(threshold_tbl.medium / 1000L);
  }
  DisplaySpinner(&spthreshold[0]);
  DisplaySpinner(&spthreshold[1]);
  DisplaySpinner(&spthreshold[2]);
  NotBusy();
}

static void ThresholdDraw(void) {
  ptext("Choose the cost per kWh that will", 100, 180, FG_BLK + BG_TRANSPARENT);
  ptext("initiate the corresponding rate", 115, 180, FG_BLK + BG_TRANSPARENT);
  ptext("schedule.", 130, 180, FG_BLK + BG_TRANSPARENT);

  GroupBox("Rate Schedule Change-Points", 155, 180, 145, 275);

  ptext("Critical:", 183, 193, FG_BLK + BG_TRANSPARENT);
  ptext("    High:", 208, 193, FG_BLK + BG_TRANSPARENT);
  ptext("  Medium:", 233, 193, FG_BLK + BG_TRANSPARENT);
  ptext("     Low: Any cost per kWh", 260, 193, FG_BLK + BG_TRANSPARENT);
  ptext("          below medium.", 274, 193, FG_BLK + BG_TRANSPARENT);

  ptext("\xA0 and up...", 183, 325, FG_BLK + BG_TRANSPARENT);
  ptext("\xA0 and up...", 208, 325, FG_BLK + BG_TRANSPARENT);
  ptext("\xA0 and up...", 233, 325, FG_BLK + BG_TRANSPARENT);

  ptext("Units are in cents per kWh. (\xA0/kWh)", 312, 182, FG_BLK + BG_TRANSPARENT);
}

static void ThresholdCancel(void) {
  MasterControls();
}

static void ThresholdSave(void) {
  int i;

  Busy();
  threshold_tbl.critical = spthreshold[0].value * 1000L;
  threshold_tbl.high = spthreshold[1].value * 1000L;
  threshold_tbl.medium = spthreshold[2].value * 1000L;

  for (i = 0; i < 5; i++) {
    if (SendTable(RATE_LEVEL_POINTS, &threshold_tbl, sizeof(THRESHOLD_TBL)) == SUCCESS) {
      MasterControls();
      Status("Rate threshold save successful.");
      return;
    }
  }
  NotBusy();
  msgbox("Unable to send changes.|Try Again.", "Rate Thresholds Save", MB_OK);
}

extern long time_delta;
static void SetTimeClick(void) {
  struct tm outside_tm;
  struct tm* tod;
  char time_buffer[40];
  long outsidetime;

  Busy();
  SetTime();
  memset(time_buffer, 0, 40);
  Wait(6L);
  outside_time(&outside_tm);
  time(&ltime);
  outside_tm.tm_isdst = _daylight;
  outsidetime = mktime(&outside_tm);
  time_delta = outsidetime - ltime;

  sprintf(time_buffer,
          "ICU Time: %2d:%02d   ",
          outside_tm.tm_hour,
          outside_tm.tm_min);
  ptext(time_buffer, 206, 350, FG_BLK + BG_WHT);

  tod = localtime(&ltime);
  sprintf(time_buffer,
          "PC Time:  %2d:%02d   ",
          tod->tm_hour,
          tod->tm_min);
  ptext(time_buffer, 222, 350, FG_BLK + BG_WHT);
  Status("PC time set successfully.");
  NotBusy();
}

static void ddSetHVACMode(void) {
  int count;
  int i;
  UCHAR mode = 0x00;
  // 0 - heating
  // 1 - cooling

  Busy();
  if (ddhvac.selected == 0) // cooling
  {
    mode = 0x01;
  }

  for (count = 0; count < 5; count++) {
    if (SendTable(HVAC_MODE_TABLE, &mode, sizeof(UCHAR)) == SUCCESS) {
      Status("HVAC schedule successfully set.");
      return;
    }
  }
  for (i = 0; i < 8; i++) {
    if (interval$.mode_byte1 & (0x01 << i)) {
      switch (i) {
      case 4: // cooling mode
      case 5: // heating mode
        ddhvac.selected = i - 4;
        break;
      }
    }
  }
  DisplayDropDown(&ddhvac);
  NotBusy();
  msgbox("Unable to set HVAC schedule.", "Failure", MB_OK);
}

static void ddSetsecurityMode(void) {
  int count;
  int i;
  UCHAR mode = 0x00;
  // 0 - away
  // 1 - home

  Busy();
  mode = ddsecurity.selected;
  for (count = 0; count < 5; count++) {
    if (SendTable(OCCUPIED_MODE_TABLE, &mode, sizeof(UCHAR)) == SUCCESS) {
      Status("Occupancy schedule successfully set.");
      return;
    }
  }
  for (i = 0; i < 8; i++) {
    if (interval$.mode_byte2 & (0x01 << i)) {
      switch (i) {
      case 2: // away mode
      case 3: // home mode
        ddsecurity.selected = i - 2;
        break;
      }
    }
  }
  DisplayDropDown(&ddsecurity);
  NotBusy();
  msgbox("Unable to set|occupancy schedule.", "Failure", MB_OK);
}

// OnChange
static void ddSetSeaonMode(void) {
  int count;
  int i;

  UCHAR mode = 0x00;
  // 0 - spring
  // 1 - summer
  // 2 - fall
  // 3 - winter

  //static char *seasons[4] = {"Winter", "Autumn", "Summer", "Spring"};
  Busy();
  switch (ddseason.selected) {
  case 0:
    mode = 3;
    break;
  case 1:
    mode = 2;
    break;
  case 2:
    mode = 1;
    break;
  case 3:
    mode = 0;
    break;
  }
  for (count = 0; count < 5; count++) {
    if (SendTable(SEASON_MODE_TABLE, &mode, sizeof(UCHAR)) == SUCCESS) {
      Status("Season schedule successfully set.");
      return;
    }
  }
  for (i = 0; i < 8; i++) {
    if (interval$.mode_byte1 & (0x01 << i)) {
      switch (i) {
      case 0: // winter mode
      case 1: // autumn mode
      case 2: // summer mode
      case 3: // spring mode
        ddseason.selected = i;
        break;
      }
    }
  }
  DisplayDropDown(&ddseason);
  NotBusy();
  msgbox("Unable to set|season schedule.", "Failure", MB_OK);
}

static void SetSystemMode(void) {
  int count;
  int i;

  UCHAR mode = 0x00;
  // 0 - normal
  // 1 - vacation
  // 2 - monitor

  mode = ddsysmode.selected;

  Busy();
  for (count = 0; count < 5; count++) {
    if (SendTable(MASTER_MODE_TABLE, &mode, sizeof(UCHAR)) == SUCCESS) {
      Status("Master schedule successfully set.");
      return;
    }
  }
  ddsysmode.selected = 2;
  for (i = 0; i < 8; i++) {
    if (interval$.mode_byte1 & (0x01 << i)) {
      switch (i) {
      case 6: // normal mode
      case 7: // vacation mode
        ddsysmode.selected = i - 6;
        break;
      }
    }
  }
  DisplayDropDown(&ddsysmode);
  NotBusy();
  msgbox("Unable to set system schedule.", "Failure", MB_OK);
}

static void ddhvacselectOnChange(void) {
  spin_heat[0].value = dev_list.devices[hvac_index[ddhvacselect.selected]].heating_setbacks[3];
  spin_heat[1].value = dev_list.devices[hvac_index[ddhvacselect.selected]].heating_setbacks[2];
  spin_heat[2].value = dev_list.devices[hvac_index[ddhvacselect.selected]].heating_setbacks[1];
  spin_heat[3].value = dev_list.devices[hvac_index[ddhvacselect.selected]].heating_setbacks[0];
  spin_cool[0].value = dev_list.devices[hvac_index[ddhvacselect.selected]].cooling_setbacks[3];
  spin_cool[1].value = dev_list.devices[hvac_index[ddhvacselect.selected]].cooling_setbacks[2];
  spin_cool[2].value = dev_list.devices[hvac_index[ddhvacselect.selected]].cooling_setbacks[1];
  spin_cool[3].value = dev_list.devices[hvac_index[ddhvacselect.selected]].cooling_setbacks[0];

  DisplaySpinner(&spin_cool[3]);
  DisplaySpinner(&spin_cool[2]);
  DisplaySpinner(&spin_cool[1]);
  DisplaySpinner(&spin_cool[0]);
  DisplaySpinner(&spin_heat[3]);
  DisplaySpinner(&spin_heat[2]);
  DisplaySpinner(&spin_heat[1]);
  DisplaySpinner(&spin_heat[0]);
}

static void SpinHvacOnChange(void) {
  dev_list.devices[hvac_index[ddhvacselect.selected]].heating_setbacks[3] = spin_heat[0].value;
  dev_list.devices[hvac_index[ddhvacselect.selected]].heating_setbacks[2] = spin_heat[1].value;
  dev_list.devices[hvac_index[ddhvacselect.selected]].heating_setbacks[1] = spin_heat[2].value;
  dev_list.devices[hvac_index[ddhvacselect.selected]].heating_setbacks[0] = spin_heat[3].value;
  dev_list.devices[hvac_index[ddhvacselect.selected]].cooling_setbacks[3] = spin_cool[0].value;
  dev_list.devices[hvac_index[ddhvacselect.selected]].cooling_setbacks[2] = spin_cool[1].value;
  dev_list.devices[hvac_index[ddhvacselect.selected]].cooling_setbacks[1] = spin_cool[2].value;
  dev_list.devices[hvac_index[ddhvacselect.selected]].cooling_setbacks[0] = spin_cool[3].value;
}

static void SetBackSaveClick(void) {
  int i;
  int count;

  Busy();
  for (i = 0; i < ddhvacselect.count; i++) {
    if (SendObject(&dev_list.devices[hvac_index[i]],
                   'C',
                   sizeof(DEVICE),
                   DEVICE_TABLE) != SUCCESS) {
      NotBusy();
      msgbox("Error saving changes.|Please try again.", "Save Rate Based Setbacks", MB_OK);
      return;
    }
  }
  if (user_backup() == SUCCESS) {
    for (count = 0; count < 5; count++) {
      if (SendTable(EXECUTE_SYSTEM_RESET, NULL, 1) == SUCCESS) {
        MasterControls();
        NotBusy();
        Status("HVAC changes successfully saved.");
        return;
      }
    }
    NotBusy();
    msgbox("Unable to save changes", "HVAC Setback Change", MB_OK);
  } else {
    NotBusy();
    msgbox("Error updating file: devices.dat|Check disk space.",
           "Disk Error Encountered",
           MB_OK);
    MasterControls();
  }
}

static void SaveOnClick(void) {
  int count;

  Busy();

  SetSystemMode();
  ddSetDayType();
  ddSetSeaonMode();
  ddSetHVACMode();
  ddSetsecurityMode();

  for (count = 0; count < 5; count++) {
    if (SendTable(EXECUTE_SYSTEM_RESET, NULL, 1) == SUCCESS) {
      NotBusy();
      //IntroScreen();
      Status("System reset successful.");
      return;
    }
  }
  NotBusy();
  msgbox("Unable to set system schedule.", "Apply Schedule", MB_OK);
}

static void InterfaceModeOnChange(void) {
  FILE* fstream;

  ADVANCED = ddinterfacemode.selected;

  if (secured_user == FALSE) {
    fstream = fopen("iface.dat", "wb");
    if (fstream != NULL) {
      fwrite(&ADVANCED, sizeof(ADVANCED), 1, fstream);
      fclose(fstream);
    }
  }
  if (ddinterfacemode.selected == 0) {
    msgbox("Changing to Beginner mode will|cause important schedule information|not to be displayed.  This might|make it difficult to trouble shoot|scheduling problems.",
           "Changing User Modes", MB_OK);
  }
  MasterControls();
}

////////////////////////
//////////////////////// E-mail Setups
////////////////////////

static void OKHourClick(void);

static CHECK_BOX_T __far vis_hour[24] =
    {
        170,
        36,
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
        OKHourClick,

        190,
        36,
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
        OKHourClick,

        210,
        36,
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
        OKHourClick,

        230,
        36,
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
        OKHourClick,

        250,
        36,
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
        OKHourClick,

        270,
        36,
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
        OKHourClick,

        290,
        36,
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
        OKHourClick,

        310,
        36,
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
        OKHourClick,

        330,
        36,
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
        OKHourClick,

        350,
        36,
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
        OKHourClick,

        370,
        36,
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
        OKHourClick,

        390,
        36,
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
        "11am - Noon",
        TRUE,
        OKHourClick,

        170,
        176,
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
        OKHourClick,

        190,
        176,
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
        OKHourClick,

        210,
        176,
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
        OKHourClick,

        230,
        176,
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
        OKHourClick,

        250,
        176,
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
        OKHourClick,

        270,
        176,
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
        OKHourClick,

        290,
        176,
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
        OKHourClick,

        310,
        176,
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
        OKHourClick,

        330,
        176,
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
        OKHourClick,

        350,
        176,
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
        OKHourClick,

        370,
        176,
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
        OKHourClick,

        390,
        176,
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
        OKHourClick,
};

static CHECK_BOX_T __far aud_hour[24] =
    {
        170,
        349,
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
        OKHourClick,

        190,
        349,
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
        OKHourClick,

        210,
        349,
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
        OKHourClick,

        230,
        349,
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
        OKHourClick,

        250,
        349,
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
        OKHourClick,

        270,
        349,
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
        OKHourClick,

        290,
        349,
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
        OKHourClick,

        310,
        349,
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
        OKHourClick,

        330,
        349,
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
        OKHourClick,

        350,
        349,
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
        OKHourClick,

        370,
        349,
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
        OKHourClick,

        390,
        349,
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
        "11am - Noon",
        TRUE,
        OKHourClick,

        170,
        489,
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
        OKHourClick,

        190,
        489,
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
        OKHourClick,

        210,
        489,
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
        OKHourClick,

        230,
        489,
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
        OKHourClick,

        250,
        489,
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
        OKHourClick,

        270,
        489,
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
        OKHourClick,

        290,
        489,
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
        OKHourClick,

        310,
        489,
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
        OKHourClick,

        330,
        489,
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
        OKHourClick,

        350,
        489,
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
        OKHourClick,

        370,
        489,
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
        OKHourClick,

        390,
        489,
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
        OKHourClick,
};

static char __far ind_devices_desc[MAX_DEVICES + 1][14];
static char __far* ind_devices[MAX_DEVICES + 1];
int __far ind_index[MAX_DEVICES];

void IndDevOnChange(void);

static DROPDOWN_BOX_T dd_v_ind_dev =
    {
        91, 28, 145, 17,
        FALSE,             // got_focus
        DROP_DOWN_BOX,     // type
        TRUE,              // visible
        DropDownMouseDown, // OnMouseDown()
        DropDownMouseUp,   // OnMouseUp()
        DropDownClick,     // OnClick()
        DropDownKeyPress,  // OnKeyPress()
        DisplayDropDown,   // DisplayObject()
        ind_devices,       // strings
        0,                 // item selected
        FALSE,             // dropped
        1,                 // item count
        IndDevOnChange     // OnChange
};

static DROPDOWN_BOX_T dd_a_ind_dev =
    {
        91, 341, 145, 17,
        FALSE,             // got_focus
        DROP_DOWN_BOX,     // type
        TRUE,              // visible
        DropDownMouseDown, // OnMouseDown()
        DropDownMouseUp,   // OnMouseUp()
        DropDownClick,     // OnClick()
        DropDownKeyPress,  // OnKeyPress()
        DisplayDropDown,   // DisplayObject()
        ind_devices,       // strings
        0,                 // item selected
        FALSE,             // dropped
        1,                 // item count
        IndDevOnChange     // OnChange
};

static void SpinEmailOnChange(void);
SPINNER_T __far spin_email[4] =
    {
        89, 212, 1, 1,    // position
        FALSE,            // focus
        TIME_ENTRY,       // type
        TRUE,             // visible
        SpinnerMouseDown, // OnMouseDown()
        SpinnerMouseUp,   // OnMouseUp()
        SpinnerClick,     // OnClick()
        SpinnerKeyPress,  // OnKeyPress()
        DisplaySpinner,   // DisplayObject()
        0,                // value
        60,               // max
        0,                // min
        1,                // increment
        SpinEmailOnChange,

        128, 212, 1, 1,   // position
        FALSE,            // focus
        TIME_ENTRY,       // type
        TRUE,             // visible
        SpinnerMouseDown, // OnMouseDown()
        SpinnerMouseUp,   // OnMouseUp()
        SpinnerClick,     // OnClick()
        SpinnerKeyPress,  // OnKeyPress()
        DisplaySpinner,   // DisplayObject()
        0,                // value
        60,               // max
        0,                // min
        1,                // increment
        SpinEmailOnChange,

        89, 526, 1, 1,    // position
        FALSE,            // focus
        TIME_ENTRY,       // type
        TRUE,             // visible
        SpinnerMouseDown, // OnMouseDown()
        SpinnerMouseUp,   // OnMouseUp()
        SpinnerClick,     // OnClick()
        SpinnerKeyPress,  // OnKeyPress()
        DisplaySpinner,   // DisplayObject()
        0,                // value
        60,               // max
        0,                // min
        1,                // increment
        SpinEmailOnChange,

        128, 526, 1, 1,   // position
        FALSE,            // focus
        TIME_ENTRY,       // type
        TRUE,             // visible
        SpinnerMouseDown, // OnMouseDown()
        SpinnerMouseUp,   // OnMouseUp()
        SpinnerClick,     // OnClick()
        SpinnerKeyPress,  // OnKeyPress()
        DisplaySpinner,   // DisplayObject()
        0,                // value
        60,               // max
        0,                // min
        1,                // increment
        SpinEmailOnChange};

static void EmailSetupDraw(void);
static void EmailSaveClick(void);

static PUSH_BUTTON_T emapbutton =
    {
        419, 445, 89, 33, // position
        FALSE,            // focus
        PUSH_BUTTON,      // type
        TRUE,             // visible
        BtnMouseDown,     // OnMouseDown()
        BtnMouseUp,       // OnMouseUp()
        BtnClick,         // OnClick()
        BtnKeyPress,      // OnKeyPress()
        PutButton,        // DisplayObject()
        FALSE,            // default button
        "Apply",          // text
        push,             // type
        up,               // state
        NULL,             // icon
        EmailSaveClick,   // function
        196};

static SCREEN_OBJECT_T* emso[56];
static LWINDOW_T email_setup =
    {
        35, 0, 640, 445,                   // window position
        emso,                              // screen object
        EmailSetupDraw,                    // window draw
        56,                                // item count
        0,                                 // cursor position 0-79 left to right
        0,                                 // element that has current focus
        fcsBtn,                            // focus type
        "E-mail Waiting Indicators Setup", // title
        DefaultOnKeyPress                  // OnKeyPress();
};

static void EmailSetup(void) {
  int i;
  int j;

  memset(&email_def, 0, sizeof(EMAIL_INDICATORS_T));

#if 1
  if (FetchTable(&email_def,                 // buffer location
                 sizeof(EMAIL_INDICATORS_T), // Table size
                 EMAIL_INDICATOR,            // Table #
                 NULL,                       // ptr to parameters
                 0,                          // parameter length
                 20,                         // retries (don't think this is used)
                 FALSE                       // Show Status
                 ) == 0) {
    ;
  }
#endif

  // Visual indicator
  emso[0] = &dd_v_ind_dev.so;
  emso[1] = &spin_email[0].so;
  emso[2] = &spin_email[1].so;

  spin_email[0].value = email_def.VisualMinutesOn;
  spin_email[1].value = email_def.VisualMinutesOff;

  for (i = 0; i < 24; i++) {
    emso[i + 3] = &vis_hour[i].so;
    if (email_def.VisualHours[i / 8] & (0x80 >> (i % 8))) {
      vis_hour[i].checked = TRUE;
    } else {
      vis_hour[i].checked = FALSE;
    }
  }

  // Audible indicator
  emso[27] = &dd_a_ind_dev.so;
  emso[28] = &spin_email[2].so;
  emso[29] = &spin_email[3].so;

  spin_email[2].value = email_def.AudibleMinutesOn;
  spin_email[3].value = email_def.AudibleMinutesOff;

  for (i = 0; i < 24; i++) {
    emso[i + 30] = &aud_hour[i].so;
    if (email_def.AudibleHours[i / 8] & (0x80 >> (i % 8))) {
      aud_hour[i].checked = TRUE;
    } else {
      aud_hour[i].checked = FALSE;
    }
  }

  // standard exit buttons
  emso[54] = &emapbutton.so; // Apply
  emso[55] = &cbutton.so;    // Cancel

  dd_v_ind_dev.count = 0;
  dd_a_ind_dev.count = 0;

  strcpy(ind_devices_desc[0], "None");
  ind_devices[0] = ind_devices_desc[0];
  ind_index[0] = 0;
  dd_v_ind_dev.count = 1;
  dd_a_ind_dev.count = 1;
  for (i = 0, j = 1; i < dev_list.devices_used; i++) {
    if ((IsSubmeter(&dev_list.devices[i]) == FALSE) && (IsTstat(&dev_list.devices[i]) == FALSE)) {
      strncpy(ind_devices_desc[j], dev_list.devices[i].desc, 13);
      ind_devices_desc[j][13] = 0;
      ind_devices[j] = ind_devices_desc[j];
      ind_index[j] = i;
      dd_v_ind_dev.count++;
      dd_a_ind_dev.count++;
      if (dev_list.devices[i].device_slot == email_def.VisualDeviceID) {
        dd_v_ind_dev.selected = j;
      }
      if (dev_list.devices[i].device_slot == email_def.AudibleDeviceID) {
        dd_a_ind_dev.selected = j;
      }

      if (++j > MAX_DEVICES) {
        break;
      }
    }
  }

  glbWindow = &email_setup;
  PutWindow(&email_setup);
}

static void EmailSetupDraw(void) {
  GroupBox("Visual Indicator", 60, 16, 356, 290);
  GroupBox("Audible Indicator", 60, 329, 356, 290);

  ptext("Indicator Device:", 77, 28, FG_BLK + BG_TRANSPARENT);
  ptext("Indicator Device:", 77, 341, FG_BLK + BG_TRANSPARENT);

  ptext("Minutes On:", 73, 196, FG_BLK + BG_TRANSPARENT);
  ptext("Minutes Off:", 114, 196, FG_BLK + BG_TRANSPARENT);

  ptext("Minutes On:", 73, 509, FG_BLK + BG_TRANSPARENT);
  ptext("Minutes Off:", 114, 509, FG_BLK + BG_TRANSPARENT);

  GroupBox("Allowed Hours", 152, 24, 257, 276);
  GroupBox("Allowed Hours", 152, 337, 257, 276);
}

void EmailSaveClick(void) {
  FILE* stream;

  Busy();

  if (SendTable(EMAIL_INDICATOR,
                &email_def,
                sizeof(EMAIL_INDICATORS_T)) != SUCCESS) {
    NotBusy();
    msgbox("Error saving changes.|Please try again.", "Save E-mail Indicator Settings", MB_OK);
    return;
  }

  stream = fopen("EMAIL.IND", "wb");
  if (stream == NULL) {
    NotBusy();
    msgbox("Unable to save changes", "HVAC Setback Change", MB_OK);
  } else {
    fwrite(&email_def, 1, sizeof(EMAIL_INDICATORS_T), stream);
    fclose(stream);
    NotBusy();
    MasterControls();
  }
  MasterControls();
}

static void SpinEmailOnChange(void) {
  email_def.VisualMinutesOn = (UCHAR)spin_email[0].value;
  email_def.VisualMinutesOff = (UCHAR)spin_email[1].value;
  email_def.AudibleMinutesOn = (UCHAR)spin_email[2].value;
  email_def.AudibleMinutesOff = (UCHAR)spin_email[3].value;
}

void OKHourClick(void) {
  int i;
  int i_work;

  for (i = 0; i < 24; ++i) {
    if (vis_hour[i].checked == TRUE) {
      email_def.VisualHours[i / 8] |= (0x80 >> (i % 8)); // Turn it on...
    } else {
      i_work = (0x80 >> (i % 8));
      email_def.VisualHours[i / 8] &= (~i_work); // Turn it off
    }
  }

  for (i = 0; i < 24; ++i) {
    if (aud_hour[i].checked == TRUE) {
      email_def.AudibleHours[i / 8] |= (0x80 >> (i % 8)); // Turn it on...
    } else {
      i_work = (0x80 >> (i % 8));
      email_def.AudibleHours[i / 8] &= (~i_work); // Turn it off
    }
  }
}

void IndDevOnChange(void) {
  if (dd_v_ind_dev.selected > 0) {
    email_def.VisualDeviceID = dev_list.devices[ind_index[dd_v_ind_dev.selected]].device_slot;
  } else {
    email_def.VisualDeviceID = 0; // "None"
  }
  if (dd_a_ind_dev.selected > 0) {
    email_def.AudibleDeviceID = dev_list.devices[ind_index[dd_a_ind_dev.selected]].device_slot;
  } else {
    email_def.AudibleDeviceID = 0; // "None"
  }
}

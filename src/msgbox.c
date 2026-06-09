#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <bios.h>
#include <malloc.h>

#include "cebus.h"
#include "2box.h"
#include "inio.h"
#include "inside.h"
#include "vga.h"
#include "scrnobjt.h"
#include "textbox.h"
#include "pshbtn.h"
#include "mouse.h"
#include "lwindow.h"
#include "msgbox.h"
#include "mloop.h"
#include "insubs.h"

extern long ltime;
extern int outside_box;
extern struct tm* tod;
extern int item_index; //item counter
extern int last_index;

extern int kb_stat(void);
extern void demo(void);

static int msg_result;
UCHAR* input_result;
static LWINDOW_T* tmpWindow;

extern int im_busy;
extern CURSOR_DEF_T arrow_ms;
extern CURSOR_DEF_T finger_ms;
extern CURSOR_DEF_T hourglass_ms;
extern CURSOR_DEF_T bar_ms;

static void MsgBoxOk(void);
static void MsgBoxCancel(void);
static void MsgBoxYes(void);
static void MsgBoxNo(void);
static void MsgInputBoxOk(void);
static void MsgInputBoxCancel(void);

PUSH_BUTTON_T msgbox_btn_lst[2] = {300, 0, 70, 30,
                                   FALSE,
                                   PUSH_BUTTON,
                                   FALSE, // visible
                                   BtnMouseDown,
                                   BtnMouseUp,
                                   BtnClick,
                                   BtnKeyPress,
                                   PutButton,
                                   FALSE,
                                   NULL, //text
                                   push,
                                   up,
                                   NULL,
                                   NULL,
                                   139,

                                   300, 0, 70, 30,
                                   FALSE,
                                   PUSH_BUTTON,
                                   FALSE, // visible
                                   BtnMouseDown,
                                   BtnMouseUp,
                                   BtnClick,
                                   BtnKeyPress,
                                   PutButton,
                                   FALSE,
                                   NULL, //text
                                   push,
                                   up,
                                   NULL,
                                   NULL,
                                   140};

TEXT_BOX_T msgbox_txt = {0, 0, 0, 14,  // position
                         TRUE,         // got_focus
                         TEXT_BOX,     // type
                         FALSE,        // visible
                         TxtMouseDown, // OnMouseDown()
                         TxtMouseUp,   // OnMouseUp()
                         TxtClick,     // OnClick()
                         TxtKeyPress,  // OnKeyPress()
                         PutTxtBox,    // DisplayObject()
                         18,           // row
                         28,           // column
                         25,           // box length
                         tb_alpha,     // type
                         "",           // text
                         NULL};

SCREEN_OBJECT_T* msgboxSO[3];

LWINDOW_T message_box[1] = {120, 150, 340, 220,
                            msgboxSO,
                            NULL,
                            3,
                            0,
                            0,
                            fcsBtn,
                            NULL,
                            DefaultOnKeyPress};

//===========================================================================================
// NAME:   msgbox()
//
// HISTORY:
// Author         Date         Description
// M. Lauter      UNK            Initial Version
//
//
//
// INPUTS:
// Name            Type         Description
// text            UCHAR*         text to be displayed by message box
// title         UCHAR*         text to be displayed by title bar of message box
// mb_type         int            MB_OK         displays one OK button
//                           MB_YES_NO      displays one YES button and one NO button
//                           MB_OK_CANCEL   displays one OK button and one CANCEL button
//
// OUTPUTS:
// Name            Type         Description
// msg_result      int            MB_RESULT_YES      YES button was pushed
//                           MB_RESULT_NO      NO button was pushed
//                           MB_RESULT_OK      OK button was pushed
//                           MB_RESULT_CANCEL   CANCEL button was pushed
//
// DESCRIPTION:
//
// msgbox() displays a windows like message box
// and returns which button was clicked to the
// calling routine.
//
// EXAMPLE:
//
//   if (msgbox(&question_text, &title_text, MB_YES_NO) == MB_RESULT_NO)
//   {
//      DoThisFunction();
//   }
//
//===========================================================================================
int msgbox(UCHAR* text, UCHAR* title, int mb_type) {
  long last_shown = 0;
  int mouse_down = -1; // receives the item index
  int c = 0;
  int v_center = 0;
  int text_focus_color = FG_BLK + BG_BLK, btn_fcs_clr = FG_BLK + BG_BLK;
  int i = 0, j = 0;
  int do_it;
  UCHAR buf[6][80];
  UCHAR* button_text[4] = {"Ok", "Cancel", "Yes", "No"};
  UCHAR far* saved_window = NULL;
  int which_mouse = M_ARROW; //0 = arrow, 1 = finger
  int changed_cursor;

  //position_mouse(315,315);
  message_box->title = title;

  msgboxSO[0] = &msgbox_btn_lst[0].so;
  msgboxSO[1] = &msgbox_btn_lst[1].so;

  switch (mb_type) {
  case MB_OK:
    msgbox_txt.so.visible = FALSE;
    message_box->item_count = 1;
    msgbox_btn_lst[0].so.position.left = 285;
    msgbox_btn_lst[1].so.position.left = 285;
    msgbox_btn_lst[0].text = button_text[0];
    msgbox_btn_lst[1].text = NULL;
    msgbox_btn_lst[0].pbf = MsgBoxOk;
    msgbox_btn_lst[1].pbf = NULL;
    msgbox_btn_lst[0].so.visible = TRUE;
    msgbox_btn_lst[1].so.visible = FALSE;
    msgbox_btn_lst[0].so.got_focus = FALSE;
    msgbox_btn_lst[1].so.got_focus = FALSE;
    break;
  case MB_OK_CANCEL:
    msgbox_txt.so.visible = FALSE;
    message_box->item_count = 2;
    msgbox_btn_lst[0].so.visible = TRUE;
    msgbox_btn_lst[1].so.visible = TRUE;
    msgbox_btn_lst[1].so.position.left = 247 + 75;
    msgbox_btn_lst[0].so.position.left = 247;
    msgbox_btn_lst[0].so.got_focus = FALSE;
    msgbox_btn_lst[1].so.got_focus = FALSE;
    msgbox_btn_lst[0].text = button_text[0];
    msgbox_btn_lst[1].text = button_text[1];
    msgbox_btn_lst[0].pbf = MsgBoxOk;
    msgbox_btn_lst[1].pbf = MsgBoxCancel;
    break;
  case MB_YES_NO:
    msgbox_txt.so.visible = FALSE;
    message_box->item_count = 2;
    msgbox_btn_lst[0].so.position.left = 247;
    msgbox_btn_lst[0].so.visible = TRUE;
    msgbox_btn_lst[1].so.position.left = 247 + 75;
    msgbox_btn_lst[1].so.visible = TRUE;
    msgbox_btn_lst[0].so.got_focus = FALSE;
    msgbox_btn_lst[1].so.got_focus = FALSE;
    msgbox_btn_lst[1].text = button_text[3];
    msgbox_btn_lst[0].text = button_text[2];
    msgbox_btn_lst[1].pbf = MsgBoxNo;
    msgbox_btn_lst[0].pbf = MsgBoxYes;
    break;
  }

  tmpWindow = glbWindow;
  glbWindow = message_box;
  saved_window = get_window(9, 19, 44, 17);
  PutWindow(glbWindow);

  memset(buf, 0, 6 * 80);

  mouse_cursor(&arrow_ms);
  unhide_mouse();

  for (i = 0; i < (signed)strlen(text); i++) {
    if (text[i] == 124) // vertical bar character >>>------> |
    {
      strcat(buf[j], "\0");
      j++;
    } else {
      strncat(buf[j], &text[i], 1);
    }
  }
  //   for (i = 0; i < (j + 1); i++)
  //   {
  //      trim(buf[i]);
  //   }

  v_center = (message_box->win_pos.height - ((j + 1) * 14)) / 2;

  for (i = 0; i < j + 1; i++) {
    ptext(buf[i],
          message_box->win_pos.top + v_center + (i * 14),
          message_box->win_pos.left + (message_box->win_pos.width - (strlen(buf[i]) * 8)) / 2,
          BG_TRANSPARENT + FG_BLK);
  }

  msg_result = 0;
  while (1) {
    //get mouse info and store it in mouse_info
    mouse_data(&mouse_info);
    //show_mouse(); //print mouse details

    changed_cursor = FALSE;

    do_it = FALSE;
    if (last_index == -1) {
      do_it = TRUE;
    } else if (!((MouseOver(&glbWindow->so[last_index]->position) == TRUE) &&
                 (glbWindow->so[last_index]->visible == TRUE))) {
      do_it = TRUE;
    }

    if (do_it) {
      if ((MouseOver(&glbWindow->so[item_index]->position) == TRUE) &&
          (glbWindow->so[item_index]->visible == TRUE)) {
        changed_cursor = TRUE;
        last_index = item_index;
        if (glbWindow->so[item_index]->type != TEXT_BOX) {
          if (which_mouse != M_FINGER) {
            which_mouse = M_FINGER;
            mouse_cursor(&finger_ms);
          }
        } else {
          if (which_mouse != M_TEXTBAR) {
            which_mouse = M_TEXTBAR;
            mouse_cursor(&bar_ms);
          }
        }
      }
      if (changed_cursor == FALSE) //change to arrow
      {
        if (which_mouse != M_ARROW) {
          which_mouse = M_ARROW;
          last_index = -1;
          mouse_cursor(&arrow_ms);
        }
      }
    }

    if (msg_result != 0) {
      glbWindow = tmpWindow;
      put_window(saved_window);
      if (im_busy == TRUE) {
        mouse_cursor(&hourglass_ms);
      } else {
        mouse_cursor(&arrow_ms);
      }
      return msg_result;
    }

    //if left mouse button pressed
    if (mouse_info.buttons == 1) {
      if (mouse_down == -1) {
        if ((MouseOver(&glbWindow->so[item_index]->position) == TRUE) &&
            (glbWindow->so[item_index]->OnMouseDown != NULL) &&
            (glbWindow->so[glbWindow->focus.current]->DisplayObject != NULL) &&
            (glbWindow->so[item_index]->visible == TRUE)) {
          glbWindow->so[glbWindow->focus.current]->got_focus = FALSE;
          (*glbWindow->so[glbWindow->focus.current]->DisplayObject)(glbWindow->so[glbWindow->focus.current]);
          mouse_down = glbWindow->focus.current = item_index;
          glbWindow->so[glbWindow->focus.current]->got_focus = TRUE;
          (*glbWindow->so[glbWindow->focus.current]->DisplayObject)(glbWindow->so[glbWindow->focus.current]);
          if (glbWindow->so[glbWindow->focus.current]->OnMouseDown != NULL) { // call the object's mouse down event handler
            (*glbWindow->so[glbWindow->focus.current]->OnMouseDown)(glbWindow->so[glbWindow->focus.current]);
          }
        }
      } else //mouse was pressed over an object, but now the mouse (still pressed) was moved off the object
      {
        if ((MouseOver(&glbWindow->so[glbWindow->focus.current]->position) == FALSE) &&
            (glbWindow->so[glbWindow->focus.current]->OnMouseUp != NULL)) {
          mouse_down = -1;
          // call the object's MouseUp() event handler
          (*glbWindow->so[glbWindow->focus.current]->OnMouseUp)(glbWindow->so[glbWindow->focus.current]);
        }
      }
    }

    //if mouse button is released after being pressed while over a screen object
    if ((mouse_info.buttons == 0) && (mouse_down != -1)) {
      mouse_down = -1;
      //make sure mouse is over screen object
      if ((MouseOver(&glbWindow->so[glbWindow->focus.current]->position)) &&
          (glbWindow->so[glbWindow->focus.current]->OnClick != NULL)) {
        // call selected object's OnClick() event
        (*glbWindow->so[glbWindow->focus.current]->OnClick)(glbWindow->so[glbWindow->focus.current]);
      }
    }

    time(&ltime);
    tod = localtime(&ltime);

    if (LOGGED_IN == TRUE) {
      CheckCEBus();
    }

    if (MainLoopOpt) {
      if (last_shown != ltime) {
        UpdateTop(&last_shown);
      }
    }

    /*
         This next section filters key strokes.

         "system" keystrokes are handled here

         "window" keystrokes are passed to the
            current window's OnKeyPress() event
            handler

         "object" keystrokes are passed by the
            current window's OnKeyPress() event
            handler to the object's OnKeyPress()
            event handler
      */
    if (kb_stat()) {
      c = kbchar();
      switch (c) {
      case 9: // tab
        TabRight();
        break;
      case 0:
        c = kbchar();
        switch (c) {
        case 68: // F10
          lv_stop();
        case 15: //shift tab
          TabLeft();
          break;
        default:
          unkbchar(c);
          if ((glbWindow->OnKeyPress != NULL) &&
              (glbWindow->so[glbWindow->focus.current]->visible == TRUE)) {
            (*glbWindow->OnKeyPress)(glbWindow->so[glbWindow->focus.current], 0);
          }
        }
        break;
      case CR: // carriage return
      case 32: // SPACE
      default: // call current window's OnKeyPress() event handler
        if ((glbWindow->OnKeyPress != NULL) &&
            (glbWindow->so[glbWindow->focus.current]->visible == TRUE)) {
          (*glbWindow->OnKeyPress)(glbWindow->so[glbWindow->focus.current], c);
        }
        break;
      }
    }
    if (++item_index > glbWindow->item_count - 1) {
      item_index = 0;
    }
  }
}

//===========================================================================================
// NAME:   inputbox()
//
// HISTORY:
// Author         Date         Description
// M. Lauter      UNK            Initial Version
//
//
// INPUTS:
// Name            Type         Description
// text            UCHAR*         text to be displayed by message box
// title         UCHAR*         text to be displayed by title bar of message box
// dflt_text      UCHAR*         default text to display in text box
//
// OUTPUTS:
// Name            Type         Description
// input_result      UCHAR*         pointer to text in text box
//                           if CANCEL is pushed its a zero length NULL terminated string
//                           if OK is pushed its a ">0" length NULL terminated string
//
//
// DESCRIPTION:
//
// inputbox() displays a windows like message box
// with a text box available for user entered data.
// inputbox() returns a pointer to the string in
// the text box to the calling routine.
//
//===========================================================================================
//tb_pass_word
UCHAR* inputbox(UCHAR* text, UCHAR* title, UCHAR* dflt_text) {
  long last_shown = 0;
  int item_index = 0;  //item counter
  int mouse_down = -1; // receives the item index
  int c = 0;
  int v_center = 0;
  int text_focus_color = FG_BLK + BG_BLK, btn_fcs_clr = FG_BLK + BG_BLK;
  int i = 0, j = 0;
  int do_it;
  UCHAR* button_text[4] = {"Ok", "Cancel", "Yes", "No"};
  UCHAR far* saved_window = NULL;
  int which_mouse = 0; //0 = arrow, 1 = finger
  int changed_cursor;

  message_box->title = title;

  message_box->cursor_pos = 0;

  msgboxSO[1] = &msgbox_btn_lst[0].so;
  msgboxSO[2] = &msgbox_btn_lst[1].so;
  msgboxSO[0] = &msgbox_txt.so;

  memset(msgbox_txt.text, 0, MAX_TEXT);

  if (strcmp(dflt_text, "*") == 0) {
    msgbox_txt.type = tb_pass_word;
    sprintf(dflt_text, "");
  } else {
    msgbox_txt.type = tb_alpha;
  }

  if (strlen(dflt_text) > 0) {
    strcpy(msgbox_txt.text, dflt_text);
  }

  msgbox_txt.so.visible = TRUE;
  msgbox_txt.so.got_focus = FALSE;
  message_box->item_count = 3;
  msgbox_btn_lst[0].so.visible = TRUE;
  msgbox_btn_lst[1].so.visible = TRUE;
  msgbox_btn_lst[0].so.position.left = 247;
  msgbox_btn_lst[1].so.position.left = 247 + 75;
  msgbox_btn_lst[0].so.got_focus = FALSE;
  msgbox_btn_lst[1].so.got_focus = FALSE;
  msgbox_btn_lst[0].text = button_text[0];
  msgbox_btn_lst[1].text = button_text[1];
  msgbox_btn_lst[0].pbf = MsgInputBoxOk;
  msgbox_btn_lst[1].pbf = MsgInputBoxCancel;

  tmpWindow = glbWindow;
  glbWindow = message_box;
  saved_window = get_window(9, 19, 44, 17);
  PutWindow(glbWindow);

  ptext(text,
        message_box->win_pos.top + 50,
        message_box->win_pos.left + (message_box->win_pos.width - (strlen(text) * 8)) / 2,
        BG_TRANSPARENT + FG_BLK);

  mouse_cursor(&arrow_ms);
  unhide_mouse();

  input_result = NULL;
  ticks_set(BlinkTimer, BLINK_RATE);
  while (1) {
    //get mouse info and store it in mouse_info
    mouse_data(&mouse_info);
    //show_mouse(); //print mouse details

    changed_cursor = FALSE;

    do_it = FALSE;
    if (last_index == -1) {
      do_it = TRUE;
    } else if (!((MouseOver(&glbWindow->so[last_index]->position) == TRUE) &&
                 (glbWindow->so[last_index]->visible == TRUE))) {
      do_it = TRUE;
    }

    if (do_it) {
      if ((MouseOver(&glbWindow->so[item_index]->position) == TRUE) &&
          (glbWindow->so[item_index]->visible == TRUE)) {
        changed_cursor = TRUE;
        last_index = item_index;
        if (glbWindow->so[item_index]->type != TEXT_BOX) {
          if (which_mouse != M_FINGER) {
            which_mouse = M_FINGER;
            mouse_cursor(&finger_ms);
          }
        } else {
          if (which_mouse != M_TEXTBAR) {
            which_mouse = M_TEXTBAR;
            mouse_cursor(&bar_ms);
          }
        }
      }
      if (changed_cursor == FALSE) //change to arrow
      {
        if (which_mouse != M_ARROW) {
          which_mouse = M_ARROW;
          last_index = -1;
          mouse_cursor(&arrow_ms);
        }
      }
    }

    if (ticks_check(BlinkTimer)) {
      BlinkCursor();
    }

    if (input_result != NULL) {
      glbWindow = tmpWindow;
      put_window(saved_window);
      if (im_busy == TRUE) {
        mouse_cursor(&hourglass_ms);
      } else {
        mouse_cursor(&arrow_ms);
      }
      return input_result;
    }

    //if left mouse button pressed
    if (mouse_info.buttons == 1) {
      if (mouse_down == -1) {
        if ((MouseOver(&glbWindow->so[item_index]->position) == TRUE) &&
            (glbWindow->so[item_index]->OnMouseDown != NULL) &&
            (glbWindow->so[glbWindow->focus.current]->DisplayObject != NULL) &&
            (glbWindow->so[item_index]->visible == TRUE)) {
          glbWindow->so[glbWindow->focus.current]->got_focus = FALSE;
          (*glbWindow->so[glbWindow->focus.current]->DisplayObject)(glbWindow->so[glbWindow->focus.current]);
          mouse_down = glbWindow->focus.current = item_index;
          glbWindow->so[glbWindow->focus.current]->got_focus = TRUE;
          (*glbWindow->so[glbWindow->focus.current]->DisplayObject)(glbWindow->so[glbWindow->focus.current]);
          if (glbWindow->so[glbWindow->focus.current]->OnMouseDown != NULL) { // call the object's mouse down event handler
            (*glbWindow->so[glbWindow->focus.current]->OnMouseDown)(glbWindow->so[glbWindow->focus.current]);
          }
        }
      } else //mouse was pressed over an object, but now the mouse (still pressed) was moved off the object
      {
        if ((MouseOver(&glbWindow->so[glbWindow->focus.current]->position) == FALSE) &&
            (glbWindow->so[glbWindow->focus.current]->OnMouseUp != NULL)) {
          mouse_down = -1;
          // call the object's MouseUp() event handler
          (*glbWindow->so[glbWindow->focus.current]->OnMouseUp)(glbWindow->so[glbWindow->focus.current]);
        }
      }
    }

    //if mouse button is released after being pressed while over a screen object
    if ((mouse_info.buttons == 0) && (mouse_down != -1)) {
      mouse_down = -1;
      //make sure mouse is over screen object
      if ((MouseOver(&glbWindow->so[glbWindow->focus.current]->position)) &&
          (glbWindow->so[glbWindow->focus.current]->OnClick != NULL)) {
        // call selected object's OnClick() event
        (*glbWindow->so[glbWindow->focus.current]->OnClick)(glbWindow->so[glbWindow->focus.current]);
      }
    }
    time(&ltime);
    tod = localtime(&ltime);

    if (LOGGED_IN == TRUE) {
      CheckCEBus();
    }

    if (MainLoopOpt) {
      if (last_shown != ltime) {
        UpdateTop(&last_shown);
      }
    }

    /*
         This next section filters key strokes.

         "system" keystrokes are handled here

         "window" keystrokes are passed to the
            current window's OnKeyPress() event
            handler

         "object" keystrokes are passed by the
            current window's OnKeyPress() event
            handler to the object's OnKeyPress()
            event handler
      */
    if (kb_stat()) {
      c = kbchar();
      switch (c) {
      case 9: // tab
        TabRight();
        break;
      case 0:
        c = kbchar();
        switch (c) {
        case 68: // F10
          lv_stop();
        case 15: //shift tab
          TabLeft();
          break;
        default:
          unkbchar(c);
          if ((glbWindow->OnKeyPress != NULL) &&
              (glbWindow->so[glbWindow->focus.current]->visible == TRUE)) {
            (*glbWindow->OnKeyPress)(glbWindow->so[glbWindow->focus.current], 0);
          }
        }
        break;
      case CR: // carriage return
      case 32: // SPACE
      default: // call current window's OnKeyPress() event handler
        if ((glbWindow->OnKeyPress != NULL) &&
            (glbWindow->so[glbWindow->focus.current]->visible == TRUE)) {
          (*glbWindow->OnKeyPress)(glbWindow->so[glbWindow->focus.current], c);
        }
        break;
      }
    }
    if (++item_index > glbWindow->item_count - 1) {
      item_index = 0;
    }
  }
}

void MsgInputBoxOk(void) {
  if (strlen(msgbox_txt.text) > 0) {
    input_result = msgbox_txt.text;
  }
}

void MsgInputBoxCancel(void) {
  memset(msgbox_txt.text, 0, MAX_TEXT);
  input_result = msgbox_txt.text;
}

void MsgBoxOk(void) {
  msg_result = MB_RESULT_OK;
}

void MsgBoxCancel(void) {
  msg_result = MB_RESULT_CANCEL;
}

void MsgBoxYes(void) {
  msg_result = MB_RESULT_YES;
}

void MsgBoxNo(void) {
  msg_result = MB_RESULT_NO;
}

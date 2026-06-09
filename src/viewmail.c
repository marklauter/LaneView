// ==========================================================================
// $Workfile:   viewmail.c  $
// $Revision:   1.7  $
//
// HISTORY:
// Author            Date              Description
// --------------------------------------------------------------------------
// Mark Lauter         March 11, 1998      View and send e-mail
//
//
// DESCRIPTION
// ==========================================================================
//   Copyright 1998 TeCom Inc. All rights reserved
// ==========================================================================

/*
$Log:   J:/intrlane/src/laneview/vcs/viewmail.c_v  $
 *
 *    Rev 1.7   Sep 11 1998 12:22:22   tmges
 * 1. Don't divide by zero when no incoming
 *     mail is present and user clicks on
 *     scrollbar.
 *
 *    Rev 1.6   Jul 31 1998 14:49:24   tmges
 * Change name of button from "Trash" to
 * "Delete"
 *
 *    Rev 1.5   Jul 30 1998 11:03:46   tmges
 * Make scrolling of incoming message
 * simpler and easier.
 *
 *    Rev 1.4   Jun 30 1998 13:17:00   tmges
 * Make mail entry start in Insert mode.
 *
 *    Rev 1.3   May 01 1998 14:11:54   tmges
 * More of the original "creation" of e-mail
 * functionality.
 *
 *    Rev 1.2   Mar 16 1998 10:23:58   tmmsl
 * fixed the crash when entering
 * ViewMail() with no email.db
 *
 *    Rev 1.1   Mar 16 1998 02:18:24   tmmsl
 * added email to the program
 *
 *    Rev 1.0   Mar 13 1998 00:03:26   tmmsl
 *

*/

// includes
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>
#include <conio.h>
#include <string.h>
#include <sys\stat.h>

#include "cebus.h"
#include "2box.h"
#include "vga.h"
#include "scrnobjt.h"
#include "textbox.h"
#include "pshbtn.h"
#include "mouse.h"
#include "lwindow.h"
#include "whmenu.h"
#include "frame_3d.h"
#include "msgbox.h"
#include "tmentry.h"
#include "spinner.h"
#include "listbox.h"
#include "dropdown.h"
#include "mloop.h"
#include "ismenu.h"
#include "mailhdr.h"
#include "memobox.h"
#include "scrolbar.h"
#include "inside.h"
#include "inio.h"

#define MAIL_SCREEN_OBJECTS 8
#define NEW_MAIL_SCREEN_OBJECTS 6
// cannot access more than 31 mails with out supporting
// a __huge array and __huge pointers which is not
// compatible with current listbox...
#define MAX_MAIL 20

static SCREEN_OBJECT_T* mail_so[MAIL_SCREEN_OBJECTS];
static MAIL_HEADER_T mail_hdr[MAX_MAIL];
static MAIL_HEADER_T* mail_hdr_list[MAX_MAIL]; //for list box
static MAIL_T __far mail;
static char __far vmail[sizeof(MAIL_T) + sizeof(MAIL_HEADER_T) + 1];
int MsgsOnIcu;
static void GetMsgsOnIcu(void);

extern int SendPrg(char* src, char* dst); // from inio.c
extern int NEW_MAIL;

static void ParseAddress(char* from, char* address);
static void ShowMessageWindow(void);
static void Message(MAIL_HEADER_T* a_hdr);
static void UpdateView(void);
static int UpdateListBox(void);
static void ReplyClick(void);
static void TrashClick(void);
static void NewMessage(void);
static void SendMailClick(void);
static void ViewSelectedMail(void);
static void HeaderTrunk(UCHAR* text, int len_to_trunkate);
static void MainClick(void);
static void ShowViewWindow(void);

static void sbOnClick(int direction, int how_far, int thumb_pos);
static SCROLL_BAR_T sb =
    {
        // BEGIN SCREEN OBJECT DEFINITION
        80, 611, 17, 101,     // position
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
        0,           // thumb position
        SB_VERTICAL, // type
        sbOnClick    // on scroll function
};

void DisplayMailHdr(MAIL_HEADER_T* mail_hdr, int top, int left, UCHAR color);

static void Memo1sbOnClick(int direction, int how_far, int thumb_pos);
static SCROLL_BAR_T memo1sb =
    {
        // BEGIN SCREEN OBJECT DEFINITION
        185, 514, 17, 268,    // position
        FALSE,                // focus
        SCROLL_BARV,          // type
        TRUE,                 // visible
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
        SB_VERTICAL,   // type
        Memo1sbOnClick // on scroll function
};

void memo1OnChange(void);

MEMOBOX_T memo1 =
    {                      // BEGIN SCREEN OBJECT DEFINITION
        185, 10, 503, 268, // position
        FALSE,             // focus
        MEMO_BOX,          // type
        TRUE,              // visible
        MemoBoxMouseDown,  // OnMouseDown()
        MemoBoxMouseUp,    // OnMouseUp()
        MemoBoxClick,      // OnClick()
        MemoBoxKeyPress,   // OnKeyPress()
        MemoBoxPaint,      // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        MAX_MAIL_LENGTH - sizeof(MAIL_HEADER_T), // max length
        FALSE,                                   // enabled
        vmail,                                   //mail.text
        &memo1sb,                                // attached scroll bar or NULL if none
        memo1OnChange};

LISTBOX_T mail_listbox =
    {
        // BEGIN SCREEN OBJECT DEFINITION
        80, 10, 600, 101, // position
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
        mail_hdr_list,   // sub_list
        DisplayMailHdr,  // display function pointer
        ViewSelectedMail // on change event
};

char back[25];
PUSH_BUTTON_T m_btn_main =
    {
        419, 540, 89, 33, // position
        FALSE,            // focus
        PUSH_BUTTON,      // type
        TRUE,             // visible
        BtnMouseDown,     // OnMouseDown()
        BtnMouseUp,       // OnMouseUp()
        BtnClick,         // OnClick()
        BtnKeyPress,      // OnKeyPress()
        PutButton,        // DisplayObject()
        TRUE,             // default button
        back,             // text
        push,             // type
        up,               // state
        icon[12],         // icon
        MainClick,        // function
        0};

PUSH_BUTTON_T m_btn_reply =
    {
        314 - 100, 540, 89, 33, // position
        FALSE,                  // focus
        PUSH_BUTTON,            // type
        TRUE,                   // visible
        BtnMouseDown,           // OnMouseDown()
        BtnMouseUp,             // OnMouseUp()
        BtnClick,               // OnClick()
        BtnKeyPress,            // OnKeyPress()
        PutButton,              // DisplayObject()
        FALSE,                  // default button
        "Reply",                // text
        push,                   // type
        up,                     // state
        NULL,                   // icon
        ReplyClick,             // function
        0};

PUSH_BUTTON_T m_btn_trash =
    {
        349 - 100, 540, 89, 33, // position
        FALSE,                  // focus
        PUSH_BUTTON,            // type
        TRUE,                   // visible
        BtnMouseDown,           // OnMouseDown()
        BtnMouseUp,             // OnMouseUp()
        BtnClick,               // OnClick()
        BtnKeyPress,            // OnKeyPress()
        PutButton,              // DisplayObject()
        FALSE,                  // default button
        "Delete",               // text
        push,                   // type
        up,                     // state
        NULL,                   // icon
        TrashClick,             // function
        0};

PUSH_BUTTON_T m_btn_newmessage =
    {
        384 - 100, 540, 89, 33, // position
        FALSE,                  // focus
        PUSH_BUTTON,            // type
        TRUE,                   // visible
        BtnMouseDown,           // OnMouseDown()
        BtnMouseUp,             // OnMouseUp()
        BtnClick,               // OnClick()
        BtnKeyPress,            // OnKeyPress()
        PutButton,              // DisplayObject()
        FALSE,                  // default button
        "New|Message",          // text
        push,                   // type
        up,                     // state
        NULL,                   // icon
        NewMessage,             //MakeClick,   // function
        0};

LWINDOW_T mail_window =
    {
        35, 0, 640, 445,     // window position
        mail_so,             // this window's screen object list
        ShowViewWindow,      // window display function
        MAIL_SCREEN_OBJECTS, // item_count
        0,                   // cursor position 0-79 left to right
        0,                   // element that has current focus
        fcsBtn,              // focus type
        "MAIL",              // title, set & displayed by display_calendar()
        DefaultOnKeyPress    // OnKeyPress();
};

void ViewMail(void) {
  mail_so[0] = &mail_listbox.so;
  mail_so[1] = &memo1.so;
  mail_so[2] = &m_btn_reply.so;
  mail_so[3] = &m_btn_trash.so;
  mail_so[4] = &m_btn_newmessage.so;
  mail_so[5] = &m_btn_main.so;
  mail_so[6] = &sb.so;
  mail_so[7] = &memo1sb.so;
  sb.attached_object = 0 + 1;
  memo1sb.attached_object = 1 + 1;
  MemoRefreshAll(&memo1);

  memset(&mail, 0, sizeof(MAIL_T));
  memset(&mail_hdr, 0, sizeof(MAIL_HEADER_T) * MAX_MAIL);
  memset(&vmail, 0, sizeof(MAIL_T) + sizeof(MAIL_HEADER_T));

  if (Sender == AdvancedFeatures) {
    sprintf(back, "ADV.");
  } else {
    sprintf(back, "Main|Menu");
  }

  NEW_MAIL = FALSE;

  memo1.cursor_col = 0;
  memo1.cursor_row = 0;
  memo1.display_top = 0;
  MemoRefreshAll(&memo1);

  UpdateListBox();
  //mail_listbox.item_selected = 0;

  glbWindow = &mail_window;
  PutWindow(&mail_window);
  GetMsgsOnIcu();
}

static void sbOnClick(int direction, int how_far, int thumb_pos) {
  switch (direction) {
  case SCROLL_UP:
    switch (how_far) {
    case SCROLL_LARGE:
      PageUp(&mail_listbox);
      break;
    case SCROLL_SMALL:
      UpArrow(&mail_listbox);
      break;
    }
    break;
  case SCROLL_DOWN:
    switch (how_far) {
    case SCROLL_LARGE:
      PageDown(&mail_listbox);
      break;
    case SCROLL_SMALL:
      DownArrow(&mail_listbox);
      break;
    }
    break;
  }
  ListBoxScroll(&mail_listbox);
  (*mail_listbox.OnChange)();
}

static void ShowViewWindow(void) {
  ptext("From", 65, 13, BG_WHT + FG_BLK);
  ptext("Subject", 65, 277, BG_WHT + FG_BLK);
  ptext("Date/Time", 65, 445, BG_WHT + FG_BLK);
}

static void MainClick(void) {
  (*Sender)();
  //IntroScreen();
}

void DisplayMailHdr(MAIL_HEADER_T* mail_hdr, int top, int left, UCHAR color) {
  UCHAR from_address[131];
  UCHAR subject[131];
  UCHAR date_time[131];
  UCHAR hdr[80];

  memset(from_address, 0, 131);
  memset(subject, 0, 131);
  memset(date_time, 0, 131);
  memset(hdr, 0, 80);

  memcpy(from_address, mail_hdr->from_address, 130);
  HeaderTrunk(from_address, 32);

  memcpy(subject, mail_hdr->subject, 130);
  HeaderTrunk(subject, 20);

  memcpy(date_time, mail_hdr->date_time, 130);
  HeaderTrunk(date_time, 20);

  sprintf(hdr, "%-32.32s %-20.20s %-20.20s",
          from_address,
          subject,
          date_time);

  ptext(hdr, top, left, color);
}

// Makes total length the value of len and adds three dots.
// Don't try to trunk beyond the size of the text array
// or you'll be sorry.
// By the way, this function MODIFIES
// the text parameter.  Notice that lack of a CONST
// declaration.  BE FORWARNED
static void HeaderTrunk(UCHAR* text, int len) {
  int i = 0;

  i = trim(text);

  if (i > len) {
    text[len - 1] = 0x0B0;
    text[len + 1] = 0;
  }
}

static void ViewSelectedMail(void) {
  UpdateView();
  memo1.cursor_col = 0;
  memo1.cursor_row = 0;
  memo1.display_top = 0;
  MemoRefreshAll(&memo1);
  MemoBoxPaint(&memo1);
  ///
  memo1sb.max = memo1.rows;
  DisplayScrollBar(&memo1sb);
  ////
  sb.thumb_pos = mail_listbox.item_selected;
  DisplayScrollBar(&sb);
}

static void UpdateView(void) {
  FILE* f;
  MAIL_HEADER_T a_hdr;
  char subject[131];
  int found;

  found = FALSE;

  if ((f = fopen("email.db", "rb")) != NULL) {
    while (1) {
      fread(&a_hdr, sizeof(MAIL_HEADER_T), 1, f);
      fread(&mail, sizeof(MAIL_T), 1, f);
      if (!memcmp(&a_hdr, mail_hdr_list[mail_listbox.item_selected], sizeof(MAIL_HEADER_T))) {
        found = TRUE;
        break;
      }
      if (feof(f)) {
        break;
      }
    }

    fclose(f);

    memset(vmail, 0, sizeof(MAIL_T) + sizeof(MAIL_HEADER_T));
    if (found == TRUE) {
      memset(subject, 0, 131);
      sprintf(subject, "%.130s", a_hdr.subject);
      trim(subject);
      trim(mail.text);
      sprintf(vmail, "FROM: %.55s%cSUBJECT: %s%cDATE TIME: %.50s"
                     "%c%c%s",
              a_hdr.from_address,
              CR,
              subject,
              CR,
              a_hdr.date_time,
              CR, CR,
              mail.text);
    }
  }
}

static void ReplyClick(void) {
  if (mail_listbox.item_count > 0) {
    Message(mail_hdr_list[mail_listbox.item_selected]);
  }
}

static void TrashClick(void) {
  FILE* f;
  FILE* of;
  FILE* trash;
  int i;
  MAIL_HEADER_T a_hdr;
  struct _stat stat_buf;

  i = mail_listbox.item_selected;

  if (!_stat("email.db", &stat_buf)) {
    f = fopen("email.db", "r+b");
    fseek(f, (ULONG)((((sizeof(MAIL_T) + sizeof(MAIL_HEADER_T)) * mail_listbox.item_selected) + sizeof(MAIL_HEADER_T))), SEEK_SET);
    mail.trashed = TRUE; //marked for deletion
    fwrite(&mail, sizeof(MAIL_T), 1, f);
    fclose(f);

    f = fopen("email.db", "rb");
    of = fopen("email.tmp", "wb");
    trash = fopen("trash.db", "ab");
    while (1) {
      fread(&a_hdr, sizeof(MAIL_HEADER_T), 1, f);
      if (feof(f)) {
        break;
      }
      fread(&mail, sizeof(MAIL_T), 1, f);
      if (mail.trashed == FALSE) {
        fwrite(&a_hdr, sizeof(MAIL_HEADER_T), 1, of);
        fwrite(&mail, sizeof(MAIL_T), 1, of);
      } else {
        fwrite(&a_hdr, sizeof(MAIL_HEADER_T), 1, trash);
        fwrite(&mail, sizeof(MAIL_T), 1, trash);
      }
    }
    fclose(trash);
    fclose(of);
    fclose(f);

    unlink("email.db");
    rename("email.tmp", "email.db");

    UpdateListBox();
    //mail_listbox.item_selected = 0;
    PutListBox(&mail_listbox);
    memo1.cursor_col = 0;
    memo1.cursor_row = 0;
    memo1.display_top = 0;
    MemoRefreshAll(&memo1);
    MemoBoxPaint(&memo1);
  }
}

static void NewMessage(void) {
  Message(NULL);
}

static int UpdateListBox(void) {
  FILE* f;
  int msg_count;

  msg_count = 0;

  if ((f = fopen("email.db", "rb")) != NULL) {
    while (1) {
      fread(&mail_hdr[msg_count], sizeof(MAIL_HEADER_T), 1, f);
      fread(&mail, sizeof(MAIL_T), 1, f);
      if (feof(f)) {
        break;
      }
      mail_hdr_list[msg_count] = &mail_hdr[msg_count];
      msg_count++;
      if (msg_count == MAX_MAIL) {
        break;
      }
    }
    fclose(f);

    mail_listbox.item_count = msg_count;
    sb.max = msg_count - 1;
    sb.thumb_pos = msg_count - 1;

    if (msg_count > 0) {
      trim(mail.text);
      mail_listbox.item_selected = msg_count - 1;
    } else {
      mail_listbox.item_selected = 0;
      memset(&mail, 0, sizeof(MAIL_T));
    }
    UpdateView();
  }

  return msg_count;
}

//////////////////////////////////////////////////////////
// new message and reply start here.
//////////////////////////////////////////////////////////

PUSH_BUTTON_T m_btn_cancel =
    {
        419, 540, 89, 33, // position
        FALSE,            // focus
        PUSH_BUTTON,      // type
        TRUE,             // visible
        BtnMouseDown,     // OnMouseDown()
        BtnMouseUp,       // OnMouseUp()
        BtnClick,         // OnClick()
        BtnKeyPress,      // OnKeyPress()
        PutButton,        // DisplayObject()
        FALSE,            // default button
        "Cancel",         // text
        push,             // type
        up,               // state
        NULL,             // icon
        ViewMail,         // function
        0};

PUSH_BUTTON_T m_btn_send =
    {
        419, 540 - 92, 89, 33, // position
        FALSE,                 // focus
        PUSH_BUTTON,           // type
        TRUE,                  // visible
        BtnMouseDown,          // OnMouseDown()
        BtnMouseUp,            // OnMouseUp()
        BtnClick,              // OnClick()
        BtnKeyPress,           // OnKeyPress()
        PutButton,             // DisplayObject()
        FALSE,                 // default button
        "Send",                // text
        push,                  // type
        up,                    // state
        NULL,                  // icon
        SendMailClick,         // function
        0};

static void Memo2sbOnClick(int direction, int how_far, int thumb_pos);
static SCROLL_BAR_T memo2sb =
    {
        // BEGIN SCREEN OBJECT DEFINITION
        120, 611, 17, 290,    // position
        FALSE,                // focus
        SCROLL_BARV,          // type
        TRUE,                 // visible
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
        SB_VERTICAL,   // type
        Memo2sbOnClick // on scroll function
};

void memo2OnChange(void);

MEMOBOX_T memo2 =
    {                      // BEGIN SCREEN OBJECT DEFINITION
        120, 10, 600, 290, // position
        FALSE,             // focus
        MEMO_BOX,          // type
        TRUE,              // visible
        MemoBoxMouseDown,  // OnMouseDown()
        MemoBoxMouseUp,    // OnMouseUp()
        MemoBoxClick,      // OnClick()
        MemoBoxKeyPress,   // OnKeyPress()
        MemoBoxPaint,      // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        MAX_MAIL_LENGTH - sizeof(MAIL_HEADER_T), // max length
        TRUE,                                    // enabled
        mail.text,
        &memo2sb, // attached scroll bar or NULL if none
        memo2OnChange};

TEXT_BOX_T txt_from =
    {
        0, 0, 0, 14,  // position
        FALSE,        // got_focus
        TEXT_BOX,     // type
        FALSE,        // visible
        TxtMouseDown, // OnMouseDown()
        TxtMouseUp,   // OnMouseUp()
        TxtClick,     // OnClick()
        TxtKeyPress,  // OnKeyPress()
        PutTxtBox,    // DisplayObject()
        //////////////////////////////////////end of screen object definition
        1,          // row
        5,          // column
        70,         // box length
        tb_alpha,   // type
        "LaneView", // text
        NULL};

TEXT_BOX_T txt_from_name =
    {
        0, 0, 0, 14,  // position
        FALSE,        // got_focus
        TEXT_BOX,     // type
        FALSE,        // visible
        TxtMouseDown, // OnMouseDown()
        TxtMouseUp,   // OnMouseUp()
        TxtClick,     // OnClick()
        TxtKeyPress,  // OnKeyPress()
        PutTxtBox,    // DisplayObject()
        //////////////////////////////////////end of screen object definition
        3,          // row
        5,          // column
        70,         // box length
        tb_alpha,   // type
        "LaneView", // text
        NULL};

TEXT_BOX_T txt_reply_to =
    {
        0, 0, 0, 14,  // position
        FALSE,        // got_focus
        TEXT_BOX,     // type
        FALSE,        // visible
        TxtMouseDown, // OnMouseDown()
        TxtMouseUp,   // OnMouseUp()
        TxtClick,     // OnClick()
        TxtKeyPress,  // OnKeyPress()
        PutTxtBox,    // DisplayObject()
        //////////////////////////////////////end of screen object definition
        3,          // row
        5,          // column
        70,         // box length
        tb_alpha,   // type
        "LaneView", // text
        NULL};

TEXT_BOX_T txt_to_address =
    {
        0, 0, 0, 14,  // position
        FALSE,        // got_focus
        TEXT_BOX,     // type
        TRUE,         // visible
        TxtMouseDown, // OnMouseDown()
        TxtMouseUp,   // OnMouseUp()
        TxtClick,     // OnClick()
        TxtKeyPress,  // OnKeyPress()
        PutTxtBox,    // DisplayObject()
        //////////////////////////////////////end of screen object definition
        6,        // row
        10,       // column
        69,       // box length
        tb_alpha, // type
        "",       // text
        NULL};

TEXT_BOX_T txt_subject =
    {
        0, 0, 0, 14,  // position
        FALSE,        // got_focus
        TEXT_BOX,     // type
        TRUE,         // visible
        TxtMouseDown, // OnMouseDown()
        TxtMouseUp,   // OnMouseUp()
        TxtClick,     // OnClick()
        TxtKeyPress,  // OnKeyPress()
        PutTxtBox,    // DisplayObject()
        //////////////////////////////////////end of screen object definition
        8,        // row
        10,       // column
        69,       // box length
        tb_alpha, // type
        "",       // text
        NULL};

static SCREEN_OBJECT_T* new_mail_so[NEW_MAIL_SCREEN_OBJECTS];
char new_mail_title[80];
LWINDOW_T new_mail_window =
    {
        35, 0, 640, 445,         // window position
        new_mail_so,             // this window's screen object list
        ShowMessageWindow,       // window display function
        NEW_MAIL_SCREEN_OBJECTS, // item_count
        0,                       // cursor position 0-79 left to right
        0,                       // element that has current focus
        fcsTxt,                  // focus type
        new_mail_title,          // title, set & displayed by display_calendar()
        DefaultOnKeyPress        // OnKeyPress();
};

static void Message(MAIL_HEADER_T* a_hdr) {
  char reply_to[131];

  memset(txt_to_address.text, 0, MAX_TEXT);
  memset(txt_subject.text, 0, MAX_TEXT);

  memo2.cursor_col = 0;
  memo2.cursor_row = 0;
  memo2.display_top = 0;
  memo2.length = 0;

  over_write = FALSE;

  if (a_hdr == NULL) {
    sprintf(new_mail_title, "New Message");
    //new_mail_window.focus.current = 0;

    new_mail_so[0] = &txt_to_address.so;
    new_mail_so[1] = &txt_subject.so;
    new_mail_so[2] = &memo2.so;
    new_mail_so[3] = &m_btn_send.so;
    new_mail_so[4] = &m_btn_cancel.so;
    new_mail_so[5] = &memo2sb.so;
    memo2sb.attached_object = 2 + 1;
  } else {
    ParseAddress(a_hdr->from_address, reply_to);
    memcpy(txt_to_address.text, reply_to, MAX_TEXT - 1);
    sprintf(new_mail_title, "Reply: %.60s", a_hdr->subject);
    trim(new_mail_title);
    memcpy(txt_subject.text, new_mail_title, MAX_TEXT - 1);
    strcat(new_mail_title, "...");
    new_mail_so[3] = &txt_to_address.so;
    new_mail_so[4] = &txt_subject.so;
    new_mail_so[5] = &memo2sb.so;
    new_mail_so[0] = &memo2.so;
    new_mail_so[1] = &m_btn_send.so;
    new_mail_so[2] = &m_btn_cancel.so;
    memo2sb.attached_object = 0 + 1;
  }

  memset(&mail, 0, sizeof(MAIL_T));

  MemoRefreshAll(&memo2); // force a re-display;
  //memo2.enabled = TRUE;
  glbWindow = &new_mail_window;
  PutWindow(&new_mail_window);
}

extern LWINDOW_T progWindow[];
char* send_mail_title = {"Sending E-Mail"};
static void SendMailClick(void) {
  FILE* f;
  MAIL_HEADER_T a_hdr;
  long my_time;
  struct tm* tod;

  progWindow->title = send_mail_title;
  PutWindow(progWindow);
  ptext("Sending E-Mail", 205 + 115, 175, BG_WHT + FG_BLK);

  time(&my_time);
  tod = localtime(&my_time);

  memset(&a_hdr, ' ', sizeof(MAIL_HEADER_T));

  sprintf(a_hdr.date_time, "%d/%d/%4d %02d:%02d:%02d", tod->tm_mon + 1, tod->tm_mday, tod->tm_year + 1900, tod->tm_hour, tod->tm_min, tod->tm_sec);
  a_hdr.date_time[strlen(a_hdr.date_time)] = ' ';

  memcpy(a_hdr.from_address, txt_from.text, strlen(txt_from.text));
  memcpy(a_hdr.from_name, txt_from_name.text, strlen(txt_from_name.text));
  memcpy(a_hdr.reply_to, txt_reply_to.text, strlen(txt_reply_to.text));
  memcpy(a_hdr.to_address, txt_to_address.text, strlen(txt_to_address.text));
  memcpy(a_hdr.subject, txt_subject.text, strlen(txt_subject.text));

  f = fopen("mail.out", "wb");
  //f = fopen("mail.out", "ab");

  fwrite(&a_hdr, sizeof(MAIL_HEADER_T), 1, f);
  //fwrite(memo2.text, strlen(memo2.text), 1, f);
  fwrite(memo2.text, strlen(memo2.text), 1, f);
  fclose(f);

  SendPrg("mail.out", "mail");
  ++MsgsOnIcu;
  ViewMail();
}

static void ShowMessageWindow(void) {
  ptext("     To", 69, 5, BG_WHT + FG_BLK);
  ptext("Subject", 97, 5, BG_WHT + FG_BLK);
}

static void Memo2sbOnClick(int direction, int how_far, int thumb_pos) {
  switch (direction) {
  case SCROLL_UP:
    switch (how_far) {
    case SCROLL_LARGE:
      MemoPageUp(&memo2);
      break;
    case SCROLL_SMALL:
      MemoUpArrow(&memo2);
      break;
    }
    break;
  case SCROLL_DOWN:
    switch (how_far) {
    case SCROLL_LARGE:
      MemoPageDown(&memo2);
      break;
    case SCROLL_SMALL:
      MemoDownArrow(&memo2);
      break;
    }
    break;
  }
  //memo2sb.thumb_pos = (int)( ((long)(memo2.cursor_row+1) * (long)memo2sb.max)/(long)(memo2.rows));
  //DisplayScrollBar(&memo2sb);
}

static void Memo1sbOnClick(int direction, int how_far, int thumb_pos) {
  switch (direction) {
  case SCROLL_UP:
    switch (how_far) {
    case SCROLL_LARGE:
      MemoPageUp(&memo1);
      break;
    case SCROLL_SMALL:
      if (memo1.cursor_row > memo1.display_top) {
        memo1.cursor_row = memo1.display_top;
      }
      MemoUpArrow(&memo1);
      break;
    }
    break;
  case SCROLL_DOWN:
    switch (how_far) {
    case SCROLL_LARGE:
      MemoPageDown(&memo1);
      break;
    case SCROLL_SMALL:
      if (memo1.cursor_row < (memo1.display_top + memo1.display_height)) {
        memo1.cursor_row = memo1.display_top + memo1.display_height;
      }
      MemoDownArrow(&memo1);
      break;
    }
    break;
  }
  if (memo1.rows == 0) {
    memo1sb.thumb_pos = 0;
  } else {
    memo1sb.thumb_pos = (int)(((long)(memo1.cursor_row + 1) * (long)memo1sb.max) / (long)(memo1.rows));
  }
  DisplayScrollBar(&memo1sb);
}

// from  is 130 bytes long
// address is 131 bytes long
// puts the return to in address
static void ParseAddress(char* from, char* address) {
  //   int i;
  //   int   j;
  //   int   start;

  memset(address, 0, 131);

#if 1

  memcpy(address, from, 130);
  trim(address);

#else

  j = 0;
  start = FALSE;
  for (i = 0; i < 130; i++) {
    if (start == TRUE) {
      if (from[i] == '>') {
        break;
      } else {
        address[j++] = from[i];
      }
    } else if (from[i] == '<') {
      start = TRUE;
    }
  }

#endif
}

void Memo1OnChange(void) {
  MEMOBOX_T* memo;

  memo = &memo2;

  if (memo->sbobj != NULL) {
    if (memo->rows > 1) {
      memo->sbobj->max = (memo->rows) - 1;
      memo->sbobj->thumb_pos = memo->cursor_row;
    } else {
      memo->sbobj->max = 1;
      memo->sbobj->thumb_pos = 0;
    }
    DisplayScrollBar(memo->sbobj);
  }
}

void Memo2OnChange(void) {
  MEMOBOX_T* memo;

  memo = &memo2;

  if (memo->sbobj != NULL) {
    if (memo->rows > 0) {
      memo->sbobj->max = (memo->rows) - 1;
      memo->sbobj->thumb_pos = memo->cursor_row;
    } else {
      memo->sbobj->max = 1;
      memo->sbobj->thumb_pos = 0;
    }
    DisplayScrollBar(memo->sbobj);
  }
}

void GetMsgsOnIcu(void) {
  int entries;
  DIR_TABLE* dt;

  if (GetTable(&f_buffer[60L * 1024L],
               2048, // size is variable length
               GET_DIR_TABLE,
               "MAILOUT\\*.*",
               11,
               RETRY_COUNT,
               FALSE) == SUCCESS) {
    dt = (void*)&f_buffer[60L * 1024L];
    entries = (dt->dir_size - 6) / (sizeof(ENTRY));
    if (entries > 2) // the first 2 entries are:  mailout\. & mailout\..
    {
      MsgsOnIcu = (int)(entries - 2);
    }
  }
}

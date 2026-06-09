#ifndef _message_box_h
#define _message_box_h

#define MB_OK 1
#define MB_OK_CANCEL 2
#define MB_YES_NO 3
#define MB_INPUT 4

#define MB_RESULT_OK 1
#define MB_RESULT_CANCEL 2
#define MB_RESULT_YES 3
#define MB_RESULT_NO 4

extern UCHAR* inputbox(UCHAR* text, UCHAR* title, UCHAR* dflt_text);
extern int msgbox(UCHAR* text, UCHAR* title, int mb_type);

#endif

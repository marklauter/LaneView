#ifndef mail_header_h
#define mail_header_h

#define MAX_MAIL_LENGTH 2048

typedef struct
{
  UCHAR type;
  UCHAR unused[4];
  UCHAR from_address[130];
  UCHAR from_name[130];
  UCHAR reply_to[130];
  UCHAR to_address[130];
  UCHAR date_time[130];
  UCHAR subject[130];
} MAIL_HEADER_T;

typedef struct
{
  UCHAR text[MAX_MAIL_LENGTH - sizeof(MAIL_HEADER_T)];
  UCHAR trashed;
} MAIL_T;

#endif

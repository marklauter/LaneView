#ifndef _laneview_io_h
#define _laneview_io_h

#define RETRY_COUNT 10

extern int monitor_cents[15];
extern int dmonitor_cents[MAX_HISTORY][15];
extern double monitor_kwh[15];
extern double dmonitor_kwh[MAX_HISTORY][15];
extern int monitor_count;
extern int dmonitor_count[MAX_HISTORY];
extern int meter_hour;
extern int in_ce_bus_reset;
extern UCHAR far f_buffer[]; // 63*1024+1023
extern DAWN_DUSK_TIME dawn_dusk_time;
extern DEVICE_STATUS device_status;
extern LOAD_CTRL load_ctrl_tbl[];
extern long int TableSize;

typedef struct
{
  char fname[13];
  ULONG fsize;
  USHORT fdate; // Bits:   15-9 = year - 1980
                //          8-5  = month (1-12)
                //         4-0  = day   (1-31)
  USHORT ftime; // Bits:   15-11 = hour (0-23)
                //         10-5  = minute (0-59)
                //         4-0   = seconds/2 (0-29)
} ENTRY;

typedef struct
{
  USHORT dir_size;
  ULONG free_space;
  ENTRY entry[2]; // may be 100's
} DIR_TABLE;

extern int SendObject(void* object, int option, int o_size, int table_type);
extern int SetDayType(UCHAR new_day_type);
extern int htoi(char* cp);
extern int user_backup(void);
extern int SetMoodState(USHORT mood_id, int active, int prot, USHORT duration);
extern int synchronize(void);
extern int RemoveDevice(DEVICE* r_dev);
extern int GetTable(void* table_ptr, int table_size, int table_id,
                    void* request_data, int data_length, int retries,
                    int show_status);
extern int RemoveMood(MOOD_T* r_mood);
extern int user_backup(void);
extern int request_rates_table(UCHAR, UCHAR, UCHAR);
extern int read_device_history(int);
extern int request_device_history(USHORT);
extern int receive_device_history(int);
extern int request_billing_info(void);
extern int request_dawn_dusk_info(UCHAR, UCHAR, UCHAR);
extern int request_file(UCHAR* file_name);
extern int read_device_status_info(USHORT device_id);
extern int read_load_control_info(void);
extern int send_load_ctrl_override(USHORT device_id, time_t start_time, time_t end_time);
extern int FetchTable(void* table_buf, int table_size, int table_id, void* request_data,
                      int req_length, int retries, int show_status);
extern int FetchFile(char* fname);

extern void ExceptionLog(char* description);

extern void CheckCEBus(void);

extern int log_off(void);
extern int RemoveTrigger(TRIGGER_T* trig);
int read_dawn_dusk_info(UCHAR year, UCHAR month, UCHAR day);

extern void decompress(char* ocp, char far* icp, USHORT size);

#endif

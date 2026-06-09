#ifndef _inhist_h
#define _inhist_h

struct HIST_TRAN {
  UCHAR ht_yr;
  UCHAR ht_mo;
  UCHAR ht_da;
  UCHAR ht_hr;
  UCHAR ht_qthr;
  UCHAR ht_rate_type;
  UCHAR ht_flags;
  USHORT ht_device_id;
  double ht_kwh;
  double ht_kwh_cost;
};

extern struct HIST_TRAN hist_tran;
extern char* day_abr[];
extern int load_ctrl_hist_count;
extern double monthly_total_credit;
extern double monthly_total_penalty;

extern int process_interval_15m(INTERVAL_DATA_15*);
extern void close_history(void);
extern int open_history(void);
extern int next_history(char* last_key);
extern int BuildLoadCtrlHistory(int do_year, int do_mon, DEVICE_LOAD_CTRL* load_ctrl_history);
extern int UpdateHistoryFast(int days_back_to_get_history);

#endif

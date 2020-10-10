// ACCESS FOR CONTROLLER
struct switch_info {
    char*   swi;
    char*   swj;
    char*   swk;
    char*   IP_range;
};
struct switch_info sw_struct[MAX_NSW+1];

struct controller_data {
  //Received
  int     open;
  int     query;
  //Transmitted
  int     ack;
  int     add;
};
struct controller_data cont_data;

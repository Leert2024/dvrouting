#include "common.h"

static const int is_neighbor3[4] = {1, 0, 1, 0};
static const int connectcosts3[4] = {7, INFINITY, 2, 0};
struct distance_table dt3;

extern int TRACE;
extern int YES;
extern int NO;

/* students to write the following two routines, and maybe some others */

void rtinit3() 
{
  rtinit(3, &dt3, is_neighbor3, connectcosts3);
}

void rtupdate3(rcvdpkt)
  struct rtpkt *rcvdpkt;
  
{
  rtupdate(rcvdpkt, &dt3, 3, is_neighbor3);
}


printdt3()
{
  printdt(dt3, 3, is_neighbor3);
}








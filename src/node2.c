#include "common.h"

static const int is_neighbor2[4] = {1, 1, 0, 1};
int connectcosts2[4] = {3, 1, 0, 2};
struct distance_table dt2;

extern int TRACE;
extern int YES;
extern int NO;

/* students to write the following two routines, and maybe some others */

void rtinit2() 
{
  rtinit(2, &dt2, is_neighbor2, connectcosts2);
}

void rtupdate2(rcvdpkt)
  struct rtpkt *rcvdpkt;
{
  rtupdate(rcvdpkt, &dt2, 2, is_neighbor2, connectcosts2);
}


printdt2()
{
  printdt(dt2, 2, is_neighbor2);
}








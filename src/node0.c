#include "common.h"

static const int is_neighbor0[4] = {0, 1, 1, 1};
static const int connectcosts0[4] = {0, 1, 3, 7};
struct distance_table dt0;

extern int TRACE;
extern int YES;
extern int NO;

/* students to write the following two routines, and maybe some others */

void rtinit0() 
{
  rtinit(0, &dt0, is_neighbor0, connectcosts0);
}

void rtupdate0(rcvdpkt)
  struct rtpkt *rcvdpkt;
{
  rtupdate(rcvdpkt, &dt0, 0, is_neighbor0);
}


printdt0()
{
  printdt(dt0, 0, is_neighbor0);
}

linkhandler0(linkid, newcost)   
  int linkid, newcost;

/* called when cost from 0 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
	
{
}


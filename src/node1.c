#include "common.h"

static const int is_neighbor1[4] = {1, 0, 1, 0};
int connectcosts1[4] = {1, 0, 1, INFINITY};
struct distance_table dt1;

extern int TRACE;
extern int YES;
extern int NO;

/* students to write the following two routines, and maybe some others */


rtinit1() 
{
  rtinit(1, &dt1, is_neighbor1, connectcosts1);
}

rtupdate1(rcvdpkt)
  struct rtpkt *rcvdpkt;
{
  rtupdate(rcvdpkt, &dt1, 1, is_neighbor1, connectcosts1);
}


printdt1()
{
  printdt(dt1, 1, is_neighbor1);
}



linkhandler1(linkid, newcost)   
int linkid, newcost;   
/* called when cost from 1 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
	
{
  linkhandler(linkid, newcost, 1, &dt1, is_neighbor1, connectcosts1);
}



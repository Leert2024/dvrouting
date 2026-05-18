# ifndef __COMMON_H__
# define __COMMON_H__

# include <stdio.h>
# include <string.h>

# define INFINITY 999

struct distance_table 
{
  int costs[4][4];
};

struct rtpkt {
  int sourceid;       /* id of sending router sending this pkt */
  int destid;         /* id of router to which pkt being sent 
                         (must be an immediate neighbor) */
  int mincost[4];    /* min cost to node 0 ... 3 */
};

void printdt(dt, node_idx, is_neighbor);
void rtupdate(rcvdpkt, dtptr, node_idx, is_neighbor, connectcosts);
void rtinit(node_idx, dtptr, is_neighbor, connectcosts);
void linkhandler(linkid, newcost, node_idx, dtptr, is_neighbor, connectcosts);

# endif

#include "common.h"

# define __LIMIT(x) x = (x > INFINITY ? INFINITY : x)

// # define DEST_VIA
# define FROM_TO

/**
 * @brief    通知所有邻接结点
 * @param    dt 距离表
 * @param    node_idx 当前结点索引
 * @param    is_neighbor 指示是否为邻接结点的数组
 */
static void notify_neighbors(dt, node_idx, is_neighbor)
    struct distance_table dt;
    int node_idx;
    int is_neighbor[4];
{
    struct rtpkt pkt;
    int mincosts[4];
    int dest, via;

    /* 构造“到所有结点的最小开销”数组 */
# ifdef DEST_VIA
    for (dest = 0; dest < 4; dest++) {
        int min_val = INFINITY;
        for (via = 0; via < 4; via++) {
            if (dt.costs[dest][via] < min_val) {
                min_val = dt.costs[dest][via];
            }
        }
        mincosts[dest] = min_val;
    }
# endif // DEST_VIA

# ifdef FROM_TO
    memcpy(mincosts, dt.costs[node_idx], sizeof(mincosts));
# endif // FROM_TO

    /* 向所有邻居发送 */
    for (dest = 0; dest < 4; dest++) {
        if (is_neighbor[dest] == 0) continue;
        creatertpkt(&pkt, node_idx, dest, mincosts);
        tolayer2(pkt);
        printf("node %d send pkt to %d\n", node_idx, dest);
    }
}

void printdt(dt, node_idx, is_neighbor)
    struct distance_table dt;
    int node_idx;
    int is_neighbor[4];
{
# ifdef DEST_VIA
    int via, dest, neighbor_cnt = 0;
    printf("\t   via\n");
    printf("   D%d |", node_idx);
    for (via = 0; via < 4; via++) {
        if(is_neighbor[via]){
            neighbor_cnt++;
            printf("\t%d", via);
        }
    }
    printf("\n");
    printf("  ----|");
    for(via = 0; via < neighbor_cnt; via++)printf("--------");
    printf("\n");
    for (dest = 0; dest < 4; dest++) {
        if(dest == node_idx)continue;
        printf("    %d |", dest);
        for (via = 0; via < 4; via++){
            if(is_neighbor[via])printf("\t%d", dt.costs[dest][via]);
        }
        printf("\n");
    }
# endif // DEST_VIA

# ifdef FROM_TO
    int from, to;
    printf("\t   to\n");
    printf("   D%d |", node_idx);
    for (to = 0; to < 4; to++) {
        printf("\t%d", to);
    }
    printf("\n");
    printf("  ----|--------------------------------\n");
    for (from = 0; from < 4; from++) {
        if(!is_neighbor[from] && from != node_idx)continue;
        printf("    %d |", from);
        for (to = 0; to < 4; to++){
            printf("\t%d", dt.costs[from][to]);
        }
        printf("\n");
    }
# endif // FROM_TO
    printf("\n");
}

void rtinit(node_idx, dtptr, is_neighbor, connectcosts)
    int node_idx;
    struct distance_table *dtptr;
    int is_neighbor[4];
    int connectcosts[4];
{
# ifdef DEST_VIA
    int i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (i == j) {
                dtptr->costs[i][j] = connectcosts[i]; /* 到目的地 i 经由邻居 i 的开销 */
            } else {
                dtptr->costs[i][j] = INFINITY; /* 其它均初始化为无穷大 */
            }
        }
    }
    dtptr->costs[node_idx][node_idx] = 0;
   # endif // DEST_VIA

# ifdef FROM_TO
    int from, to;
    for (from = 0; from < 4; from++) {
        for (to = 0; to < 4; to++) {
            if(from == node_idx)dtptr->costs[from][to] = connectcosts[to];
            else dtptr->costs[from][to] = INFINITY;
        }
    }
# endif // FROM_TO

    printf("node %d init distance table:\n", node_idx);
    printdt(*dtptr, node_idx, is_neighbor);
    notify_neighbors(*dtptr, node_idx, is_neighbor);
}

void rtupdate(rcvdpkt, dtptr, node_idx, is_neighbor)
    struct rtpkt *rcvdpkt;
    int node_idx;
    struct distance_table *dtptr;
    int is_neighbor[4];
{
    if (rcvdpkt->destid != node_idx) return;
    printf("node %d recv from %d\n", node_idx, rcvdpkt->sourceid);

    int src = rcvdpkt->sourceid;
    int updateFlag = 0; // 距离向量是否有更新，若有更新则需要通知所有邻居
    int changeFlag = 0; // 表是否有更新，若有更新则需要打印
    int dest, via;

# ifdef DEST_VIA
    /* 遍历每一个目的地dest */
    for (dest = 0; dest < 4; dest++) {
        if (dest == node_idx) continue;

        /* 算出更新前到dest的最小开销 */
        int old_min = INFINITY;
        for (via = 0; via < 4; via++) {
            if (dtptr->costs[dest][via] < old_min) {
                old_min = dtptr->costs[dest][via];
            }
        }
        
        /* 经src到dest的最短开销 = src到dest的最短开销 + 自己到src的直连开销 */
        int old_cost = dtptr->costs[dest][src];
        dtptr->costs[dest][src] = dtptr->costs[src][src] + rcvdpkt->mincost[dest];
        __LIMIT(dtptr->costs[dest][src]);
        if(old_cost != dtptr->costs[dest][src])changeFlag = 1;

        /* 算出更新后到dest的最小开销 */
        int new_min = INFINITY;
        for (via = 0; via < 4; via++) {
            if (dtptr->costs[dest][via] < new_min) {
                new_min = dtptr->costs[dest][via];
            }
        }
        
        /* 如果最小开销变了，需要通知所有邻居 */
        if (old_min != new_min) {
            updateFlag = 1;
        }
    }
# endif // DEST_VIA

# ifdef FROM_TO
    /* 保存邻居的距离向量 */
    int old_cost[4];
    memcpy(old_cost, dtptr->costs[src], sizeof(old_cost));
    memcpy(dtptr->costs[src], rcvdpkt->mincost, sizeof(rcvdpkt->mincost));
    for (dest = 0; dest < 4; dest++){
        if (old_cost[dest] != dtptr->costs[src][dest])changeFlag = 1;
    }

    /* 更新自己的距离向量 */
    for (dest = 0; dest < 4; dest++){
        /* 贝尔曼-福特算法(无需遍历所有邻居，因为只有通知自己的邻居有可能提供更短的开销) */
        if(dtptr->costs[node_idx][dest] > rcvdpkt->mincost[dest] + dtptr->costs[node_idx][src]){
            dtptr->costs[node_idx][dest] = rcvdpkt->mincost[dest] + dtptr->costs[node_idx][src];
            changeFlag = 1;
            updateFlag = 1;
        }
    }
# endif // FROM_TO

    if (changeFlag) {
        printf("node %d update distance table:\n", node_idx);
        printdt(*dtptr, node_idx, is_neighbor);
    }
    if(updateFlag){
        notify_neighbors(*dtptr, node_idx, is_neighbor);
    } else {
        printf("node %d DV remain.\n", node_idx);
    }
}

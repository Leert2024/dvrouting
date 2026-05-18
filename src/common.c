#include "common.h"

# define __LIMIT(x) x = (x > INFINITY ? INFINITY : x)

# define DEST_VIA
// # define FROM_TO

/**
 * @brief    获取当前节点的距离向量
 * @param    node_idx 当前节点索引
 * @param    buffer 存放距离向量的缓冲区
 * @param    dt 距离表
 */
static void get_dv(int node_idx, int buffer[4], struct distance_table dt){
# ifdef DEST_VIA
    int dest, via;
    for (dest = 0; dest < 4; dest++) {
        int old_min = INFINITY;
        for (via = 0; via < 4; via++) {
            if (dt.costs[dest][via] < old_min) {
                old_min = dt.costs[dest][via];
            }
        }
        buffer[dest] = old_min;
    }
# endif // DEST_VIA

# ifdef FROM_TO
    memcpy(buffer, dt.costs[node_idx], sizeof(dt.costs[node_idx]));
# endif // FROM_TO
}

/**
 * @brief    贝尔曼-福特算法
 * @param    dtptr 距离表指针
 * @param    node_idx 当前节点索引
 * @param    is_neighbor 指示是否为邻接结点的数组(1表示是邻居，0表示不是邻居)
 * @param    connectcosts 节点n到邻居frm的直连开销数组
 */
static void bellman_ford(struct distance_table *dtptr, int node_idx, int is_neighbor[4], int connectcosts[4]){
    int y, v, min_cost;

    /* 对每个终点y，计算到y的最短开销 */
    for (y = 0; y < 4; y++){
        if(y == node_idx)continue; // 自己到自己，开销为0
        min_cost = INFINITY;
        for (v = 0; v < 4; v++){
            if(!is_neighbor[v])continue; // 非邻居，跳过
            if(dtptr->costs[v][y] + connectcosts[v] < min_cost){
                min_cost = dtptr->costs[v][y] + connectcosts[v];
            }
        }
        dtptr->costs[node_idx][y] = min_cost;
    }
}

/**
 * @brief    通知所有邻接结点
 * @param    dt 距离表
 * @param    node_idx 当前结点索引
 * @param    is_neighbor 指示是否为邻接结点的数组(1表示是邻居，0表示不是邻居)
 */
static void notify_neighbors(struct distance_table dt, int node_idx, int is_neighbor[4]){
    struct rtpkt pkt;
    int mincosts[4];
    int dest, via;

    /* 获取距离向量 */
    get_dv(node_idx, mincosts, dt);

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

void rtupdate(rcvdpkt, dtptr, node_idx, is_neighbor, connectcosts)
    struct rtpkt *rcvdpkt;
    int node_idx;
    struct distance_table *dtptr;
    int is_neighbor[4];
    int connectcosts[4];
{
    if (rcvdpkt->destid != node_idx) return;
    printf("node %d recv from %d\n", node_idx, rcvdpkt->sourceid);

    int src = rcvdpkt->sourceid;
    int dest, via;

    /* 保存旧的数组 */
    int old_dt[4][4];
    memcpy(old_dt, dtptr->costs, sizeof(old_dt));

    /* 计算旧的距离向量 */
    int old_dv[4];
    get_dv(node_idx, old_dv, *dtptr);

# ifdef DEST_VIA
    for (dest = 0; dest < 4; dest++) {
        if (dest == node_idx) continue;
        
        /* 经src到dest的最短开销 = src到dest的最短开销 + 自己到src的直连开销 */
        int old_cost = dtptr->costs[dest][src];
        dtptr->costs[dest][src] = dtptr->costs[src][src] + rcvdpkt->mincost[dest];
        __LIMIT(dtptr->costs[dest][src]);
    }
# endif // DEST_VIA

# ifdef FROM_TO
    /* 保存邻居的距离向量 */
    memcpy(dtptr->costs[src], rcvdpkt->mincost, sizeof(rcvdpkt->mincost));

    /* 使用贝尔曼-福特算法更新自己的距离向量 */
    bellman_ford(dtptr, node_idx, is_neighbor, connectcosts);
# endif // FROM_TO

    /* 计算新的距离向量 */
    int new_dv[4];
    get_dv(node_idx, new_dv, *dtptr);

    /* 如果距离表变了，打印距离表 */
    if (memcmp(old_dt, dtptr->costs, sizeof(old_dt))) {
        printf("node %d update distance table:\n", node_idx);
        printdt(*dtptr, node_idx, is_neighbor);
    }

    /* 如果距离向量变了，通知所有邻居 */
    if(memcmp(old_dv, new_dv, sizeof(old_dv))){
        notify_neighbors(*dtptr, node_idx, is_neighbor);
    } else {
        printf("node %d DV remain.\n", node_idx);
    }
}

/**
 * @brief   节点处理链路开销变化
 * @param   linkid 开销变化链路的另一端节点
 * @param   newcost 新的链路开销
 * @param   node_idx 节点索引
 * @param   dtptr 距离表指针
 * @param   is_neighbor 指示是否为邻接结点的数组
 * @param   connectcosts 邻接结点的开销数组
 */
void linkhandler(linkid, newcost, node_idx, dtptr, is_neighbor, connectcosts)
    int linkid, newcost, node_idx;
    struct distance_table *dtptr;
    int is_neighbor[4];
    int connectcosts[4];
{
    connectcosts[linkid] = newcost;
    printf("Link between %d and %d cost changed from %d to %d\n", node_idx, linkid, dtptr->costs[linkid][linkid], newcost);

    /* 保存旧的数组 */
    int old_dt[4][4];
    memcpy(old_dt, dtptr->costs, sizeof(old_dt));

    /* 计算旧的距离向量 */
    int old_dv[4];
    get_dv(node_idx, old_dv, *dtptr);

# ifdef DEST_VIA
    dtptr->costs[linkid][linkid] = newcost;
# endif // DEST_VIA

# ifdef FROM_TO
    bellman_ford(dtptr, node_idx, is_neighbor, connectcosts);
# endif // FROM_TO

    /* 计算新的距离向量 */
    int new_dv[4];
    get_dv(node_idx, new_dv, *dtptr);

    /* 如果距离表变了，打印距离表 */
    if (memcmp(old_dt, dtptr->costs, sizeof(old_dt))) {
        printf("node %d update distance table:\n", node_idx);
        printdt(*dtptr, node_idx, is_neighbor);
    }

    /* 如果距离向量变了，通知所有邻居 */
    if(memcmp(old_dv, new_dv, sizeof(old_dv))){
        notify_neighbors(*dtptr, node_idx, is_neighbor);
    } else {
        printf("node %d DV remain.\n", node_idx);
    }
}
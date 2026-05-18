# DV算法实验报告

## 作业要求
https://media.pearsoncmg.com/aw/aw_kurose_network_3/labs/lab6/lab6.html

## 目录说明
- [docs](docs)：包含作业要求和实验结果
- [src](src)：C源文件
- [inc](inc)：头文件目录
- [CMakeLists.txt](CMakeLists.txt)：CMake构建文件

## 使用方式
1. 新建build目录：`mkdir build`，进入build目录：`cd build`
2. 使用CMake构建项目：`cmake ..`
3. 编译项目：`make`
4. 运行项目：`run`，根据提示输入`TRACE`值(0~4，值越大输出的信息约详细)，即可查看实验结果。

## DV算法类型
DV算法的核心是为每个节点维护一个距离表。  
“距离表”有两种：
1. dest-via式：节点`n`的距离表中，第`dest`行第`via`列，表示节点`n`经由邻居`via`到达`dest`的开销。这是本实验要求的距离表样式：
    > “ ...where entry [i,j] in the distance table in node 0 is node 0's currently computed cost to node i **via** direct neighbor j.”

    因此，dest-via式距离表的形状为：`(总节点数-1, 节点邻居数)`

2. from-to式：节点`n`的距离表中，第`from`行第`dest`列，表示从节点`n`看来，节点`from`到达`dest`的开销。这是《计算机网络：自顶向下方法》中使用的距离表样式：
    > “每个节点的路由选择表包括了它的距离向量和它的**每个邻居**的距离向量。”

    因此，from-to式距离表的形状为：`(节点邻居数+1, 总节点数)`

本实验要求的DV算法是dest-via式。但是，我也同时实现了from-to式。  
通过修改[common.c](src/common.c)中的宏定义`# define DEST_VIA`和`# define FROM_TO`，可以切换两种DV算法。  

## 第一种DV算法(dest-via)
节点`n`的距离表初始化(Python风格伪代码)：
```python
for dest in range(4):
    for via in range(4):
        if dest == via:
            costs[dest][via] = 节点n到邻居dest的直连开销
        else:
            costs[dest][via] = 999
costs[n][n] = 0 # 特别地，节点n“经过自己到自己”的开销为0
计算自己的距离向量()
向所有邻居汇报自己的距离向量()
```
节点`n`收到来自邻居`src`的距离向量`mincosts`(长度为4的数组)后，执行的更新算法：
```python
def update(src: int, mincosts: list[int]):
    计算自己旧的距离向量()
    for dest in range(4):
        costs[dest][src] = costs[src][src] + mincosts[dest]
    计算自己新的距离向量()
    if 自己的距离向量有更新:
        向所有邻居汇报自己的距离向量()
```
直连开销出现波动时的应对算法：
```python
def linkhandler(linkid: int, newcost: int):
    connectcosts[linkid] = newcost
    计算自己旧的距离向量()
    costs[linkid][linkid] = newcost
    计算自己新的距离向量()
    if 自己的距离向量有更新:
        向所有邻居汇报自己的距离向量()
```

## 第二种DV算法(from-to)
节点`n`的距离表初始化(Python风格伪代码)：
```python
for frm in range(4):
    for to in range(4):
        if frm == n:
            costs[frm][to] = 节点n到邻居to的直连开销
        else:
            costs[frm][to] = 999
向所有邻居汇报自己的距离向量()
```

节点`n`收到来自邻居`src`的距离向量`mincosts`(长度为4的数组)后，执行的更新算法：
```python
def update(src: int, mincosts: list[int]):
    costs[src] = mincosts
    记录自己的旧的距离向量()

    # 贝尔曼-福特算法
    for to in range(4):
        if to == n:
            continue # 自己到自己，开销为0
        min = INFINITY
        for frm in n的邻居:
            if costs[frm][to] + connectcosts[frm] < min:
                min = costs[frm][to] + connectcosts[frm]
        costs[to] = min

    if 自己的距离向量有更新:
        向所有邻居汇报自己的距离向量()
```
直连开销出现波动时的应对算法：
```python
def linkhandler(linkid: int, newcost: int):
    connectcosts[linkid] = newcost
    记录自己旧的距离向量()
    # 贝尔曼-福特算法
    # ...(同上一栏)
    if 自己的距离向量有更新:
        向所有邻居汇报自己的距离向量()
```

## 实验过程
1. 从网站上下载源码；
2. 对源码进行整理，发现`nodex.c`中有很多重复的部分。对于重复的部分，将其提取出来，放在[common.c](src/common.c)中，方便算法编写。common只提供函数接口，不包含任何节点间数据共享。
3. 编写CMakeLists，使用CMake构建项目；
4. 按照算法的伪代码，使用C语言实现算法。

## 实验结果
无开销波动([prog3.c](src/prog3.c)中`#define LINKCHANGES 0`)情况下：
- dest-via式算法运行结果见[dest_via.txt](docs/dest_via.txt)  
- from-to式算法运行结果见[from_to.txt](docs/from_to.txt)

有开销波动([prog3.c](src/prog3.c)中`#define LINKCHANGES 1`)情况下，当0到1间的直连开销突变为20时：
- dest-via式算法运行结果见[dest_via_chg20.txt](docs/dest_via_chg20.txt)  
- from-to式算法运行结果见[from_to_chg20.txt](docs/from_to_chg20.txt)

稳定后，0到1间的直连开销又变为1时：
- dest-via式算法运行结果见[dest_via_chg1.txt](docs/dest_via_chg1.txt)  
- from-to式算法运行结果见[from_to_chg1.txt](docs/from_to_chg1.txt)

## 遇到的问题与解决办法
- 第一次编译报很多错，后发代码采用C89标准，故在CMakeLists.txt中添加`set(CMAKE_C_STANDARD 90)`，成功编译；
- 需注意贝尔曼-福特算法中，
    > “x到y的最短距离=min{邻居到y的最短距离+x到邻居的**直连**距离}”

    容易错写为
    > “x到y的最短距离=min{邻居到y的最短距离+x到邻居的**最短**距离}”

    后者导致波动无反应。

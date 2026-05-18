# DV算法实验报告
2024217803班 李睿彤 2024212654

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
    # 贝尔曼-福特算法(无需遍历所有邻居，因为只有通知自己的邻居有可能提供更短的开销)
    for to in range(4):
        if(costs[n][to] > costs[n][src] + mincosts[to]):
            costs[n][to] = costs[n][src] + mincosts[to]
    if 自己的距离向量有更新:
        向所有邻居汇报自己的距离向量()
```

## 实验过程
1. 从网站上下载源码；
2. 对源码进行整理，发现`nodex.c`中有很多重复的部分。对于重复的部分，将其提取出来，放在[common.c](src/common.c)中，方便算法编写。common只提供函数接口，不包含任何节点间数据共享。
3. 编写CMakeLists，使用CMake构建项目；
4. 按照算法的伪代码，使用C语言实现算法。

## 实验结果
dest-via式算法运行结果见[dest_via.txt](docs/dest_via.txt)
from-to式算法运行结果见[from_to.txt](docs/from_to.txt)

## 遇到的问题与解决办法
- 第一次编译报很多错，后发代码采用C89标准，故在CMakeLists.txt中添加`set(CMAKE_C_STANDARD 90)`，成功编译；

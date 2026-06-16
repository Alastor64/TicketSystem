## 如何运行本仓库
```bash
cmake . && make
./code
```
## 类设计
所有下标都是0-base的
|名称|功能|
|:---:|:---:|
Filer<typename,int>|数据文件读写类
BPT<typename,int degree>|B+树,degree有默认值使得一个节点大小小于等于4096B并尽可能大。
User|用户详细数据
Train|火车详细数据（不包括每天剩余座位数）成员leaveTime[i]表示从第i站离开的绝对时间、成员arriveTime[i]表示到达第i+1站的绝对时间。绝对时间定义：从发车日零点开始经过的分钟数
LeaveTrain|从某站离开的火车数据，包括：**从此站离开**的日期范围、火车的index、此站到最末站票价和、离开的绝对时间、此站在火车途径站中的下标（pos）。类型大小关系以火车index为准
ArriveTrain|到达某站的火车数据，包括：火车下标、此站到最末站票价和、到达的绝对时间、此站在火车途径站中的下标（pos）。类型大小关系以火车index为准
order_detailed|订单详细信息，包括：query_order中需要输出的所有信息、火车的index、订单起点与终点在火车途径站中的下标、车次发车日、离开起点与到达终点的绝对时间、订单时间戳。
order|订单简略信息，包括：订单起点与终点在火车途径站中的下标（成语begin与end）、购买座位数、订单下标、时间戳。大小关系以时间戳为准。
arrystring<int>|静态长度字符串，方便字符串数据在文件中存储
TrainID|arraystring<21>
Username|arraystring<21>
Password|arraystring<31>
Name|arraystring<5 * 3 + 1>
MailAddr|arraystring<31>
StationName|arraystring<31>



## 数据存储
|类型|名称|功能|
|:---:|:---:|:---:|
|外存||
|Filer< User >|userData|每个用户详细数据。用户的index即为其在userData中的位置|
|BPT< pair< Username,int > >|userIndex|用户id与index|
|Filer< Train >|trainData|每列火车的详细数据。火车的index即为其在trainData中的位置|
|BPT< pair< TrainID, int > >|trainIndex|火车id与index
|BPT< int >|releasedTrainIndex|发行火车的index
|Filer< int[100] >|seatData|每天每辆车的余座数据（仅考虑余座数据有过改动的车次），余座的index即为其在seatData中的位置
|BPT< pair< pair< int(day), int(trainIndex) >, int > > |seatIndex|发车日，火车index与余座index（仅考虑余座数据有过改动的车次）。用于余座查询
|BPT< pair< int(trainIndex), int(seatIndex) > > |releasedSeatNum|火车index与其原始座位数（仅限发行的火车）|
|BPT< pair<StationName, LeaveTrain > > |leaveTrain|记录从某一站离开的火车信息（仅限发行的火车）。用于车票查询|
|BPT< pair< StationName, ArriveTrain > > |arriveTrain|记录到达某一战的火车信息（仅限发行的火车）。用于车票查询|
|BPT< pair< int(用户index), pair< int(-timestamp), int(订单index) > > > |orderIndex|用于查询用户订单|
|BPT< pair< pair< int(day), int(trainIndex) >, order > > |pendingOrder|发车日，火车index与带候补订单简略信息。用于退票时候补|
|Filer< order_detailed > |orderData|订单详细数据。订单的index即为其在orderData中的位置
|内存||
|map< Username,int >|loggeduser|登录的用户id到权限的映射|

## 部分算法设计
query_ticket：在外存中同时根据起点站与终点站遍历leaveTrain与arriveTrain，将重合的火车index存入内存依次暴力判断其是否符合条件  
  
query_transfer：将所有经过起终点的车的index存入内存，两两搭配、暴力枚举中转站
    
refund_ticket：退票成功时，根据被退票的发车日和火车index遍历pendingOrder，暴力判断能否候补  

## 一些说明
函数get_BPT_pointer_T(BPT<>*)没有实现，其作用只是为了获得B+树指针的value_type  
  
函数BPTValue(BPT< pair<T,U> >*,pair<T,U>&tmp)会在B+树中查找key=tmp.first的最小键值对，如果没有返回0，否则将tmp.second设为最小键值对的value并返回1 
   
函数BPTContain(BPT<T>*,T tmp)返回tmp是否在B+树中  
  
对于涉及外存的对象，使用指针的目的是为了方便clean时重新构造。  
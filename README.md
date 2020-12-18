# 概述
行情支持两种行情协议。  
一种是基于小内存块(小于 20K)的数据推送和点播方式，传输使用自定义二进制数据流， 典型的数据类型包括：单笔实时行情、批量实时行情、各类排行数据、K 线数据（512 条之 内）、交易明细数据(1024 条之内)，等等。服务程序名称为 fix_qengin。  
另外一种是基于文件方式的方式下载，支持大文件下载，传输使用 FIX 的扩展文件协议， 典型的数据类型包括：除权数据、财务数据、资金数据、K 线数据（大于 512 条，按品种、 按日期下载）、交易明细（大于 1024 条，按品种、按日期下载）。服务程序名称为 fix_tengin。    
行情接口 API 的库名称分别为 blueye_client.dll（32 位）和 blueye_client64.dll（64 位），以及 blueye_fix_api.dll（32 位）。    
使用类 CBlueyeQuote 对 blueye_client.dll 或者 blueye_client64.dll 进行封装。    
使用 data_fix 类 blueye_fix_api.dll 进行封装。
# 0.blueye_client.dll(blueye_client64.dll)行情接口初始化
## 0.1.实时行情数据回调函数
 ```cpp
 static void blueye_call_back(PRCV_REPORT_STRUCTEx _report_data, int _rec_count); 
 //PRCV_REPORT_STRUCTEx _report_data， 收到的行情推送数据包指针，该数据结构体 详见附录。
 //int _rec_count ,行情记录条数。
```
对该函数体进行修改，添加数据处理模块。例如保存到文件或者发送给窗体显示（数据通知 windows 窗体建议使用队列并用 PostMessage 函数以避免界面刷新死锁），或者喂数据给算法模块。
## 0.2.K线数据回调函数
```cpp
static void blueye_kline_call_back(ROM_KLINE_DATA *_kline_data, int _rec_count); 
 //ROM_KLINE_DATA *_kline_data， K 线数据结构体指针，该数据结构体定义如下： 
 /* 
 * structure for day data. 
 */
 typedef struct
 { 
    int date ; int time ; 
    LONGLONG amount ; 
    LONGLONG volume ; 
    int openprice ; 
    int closeprice ; 
    int highprice ; 
    int lowprice ; 
 }ROM_KLINE_DATA; 
 //int _rec_count ,K 线数据记录条数。
```
对该函数体进行修改，添加数据处理模块。例如保存到文件或者发送给窗体显示（数据通知 windows 窗体建议将 K 线数据写入本地缓存文件后使用 PostMessage 函数通知以避免界面刷新死 锁），或者喂数据给算法模块。
## 0.3. TICK 数据回调函数
```cpp
static void blueye_tick_call_back(ROM_TICK_DATA *_tick_data, int _rec_count); 
 //ROM_TICK_DATA *_tick_data， TICK 数据结构体指针，该数据结构体定义如下： 
 /* 
 * structure for strike report detail data. 
 */
 typedef struct 
 { 
     int time;
     short iDPrice;
     int iDVolume;
 }ROM_TICK_DATA; 
 //int _rec_count TICK 数据记录条数。
```

对该函数体进行修改，添加数据处理模块。例如保存到文件或者发送给窗体显示（数据通知 windows 窗体建议将 K 线数据写入本地缓存文件后使用 PostMessage 函数通知以避免界面刷新死 锁），或者喂数据给算法模块。
## 0.4 设置日志回调函数地址
```cpp
static void log(std::string _str_log); 
//std::string _str_log， 底层返回日志信息，通过回调函数地址可以显示及存档。
```
## 0.5 读取行情节点配置信息（IP 地址，端口号）
```cpp
void read_config_from_file(); 
```
从配置文件读取 ini 文件初始化，ini 是通过路由服务器下载而获取。
## 0.6 API 初始化
```cpp
bool init(HANDLE _shutdown_event, std::string _str_token); 
//HANDLE _shutdown_event 系统停止的时间句柄，可以为空。
//std::string _str_token, 用户身份 ID，是一个 40 位的字符串密钥。
```
# 1. 实时行情订阅接口 API

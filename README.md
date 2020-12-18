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
 typedef struct { int date ; int time ; LONGLONG amount ; LONGLONG volume ; int openprice ; int closeprice ; int highprice ; int lowprice ; }ROM_KLINE_DATA; int _rec_count ,K 线数据记录条数。
```

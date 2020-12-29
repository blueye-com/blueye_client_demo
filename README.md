# 概述
行情支持两种行情协议。  
一种是基于小内存块(小于 20K)的数据推送和点播方式，传输使用自定义二进制数据流， 典型的数据类型包括：单笔实时行情、批量实时行情、各类排行数据、K 线数据（512 条之 内）、交易明细数据(1024 条之内)，等等。服务程序名称为 fix_qengin。  
另外一种是基于文件方式的方式下载，支持大文件下载，传输使用 FIX 的扩展文件协议， 典型的数据类型包括：除权数据、财务数据、资金数据、K 线数据（大于 512 条，按品种、 按日期下载）、交易明细（大于 1024 条，按品种、按日期下载）。服务程序名称为 fix_tengin。    
行情接口 API 的库名称分别为 blueye_client.dll（32 位）和 blueye_client64.dll（64 位），以及 blueye_fix_api.dll（32 位）。    
使用类 CBlueyeQuote 对 blueye_client.dll 或者 blueye_client64.dll 进行封装。    
使用 data_fix 类 blueye_fix_api.dll 进行封装。
# 1.请求类型定义
```cpp
#define		REQ_TYPE_QUOTE_BROADCAST     10			//请求全量数据广播(只针对有权限的账户)
#define		REQ_TYPE_QUOTE_SUBSCRIBE     11			//请求行情订阅(订阅数量上限100只品种)
#define		REQ_TYPE_QUOTE_DEPTH         12			//请求L2深度数据（暂时未开放）
#define		REQ_TYPE_QUOTE_1M_DATA       20			//请求1分钟数据K线
#define		REQ_TYPE_QUOTE_3M_DATA       21			//请求3分钟数据K线
#define		REQ_TYPE_QUOTE_5M_DATA       22			//请求5分钟数据K线
#define		REQ_TYPE_QUOTE_10M_DATA      23			//请求10分钟数据K线
#define		REQ_TYPE_QUOTE_15M_DATA      24			//请求15分钟数据K线
#define		REQ_TYPE_QUOTE_30M_DATA      25			//请求30分钟数据K线
#define		REQ_TYPE_QUOTE_60M_DATA      26			//请求60分钟数据K线
#define		REQ_TYPE_QUOTE_DAILY_DATA    27			//请求日数据K线
#define		REQ_TYPE_QUOTE_WEEKLY_DATA   28			//请求周数据K线
#define		REQ_TYPE_QUOTE_MONTHLY_DATA  29			//请求月数据K线
#define		REQ_TYPE_QUOTE_QUARTER_DATA  30			//请求季数据K线
#define		REQ_TYPE_QUOTE_ANNUAL_DATA   31			//请求年数据K线
#define		REQ_TYPE_QUOTE_TICK_LIST     40			//请求明细数据K线
```
# 2.数据格式定义
## 2.1 实时行情数据（点播或者推送）
```cpp
#define		STKLABEL_LEN			10			// 股号数据长度,国内市场股号编码兼容钱龙
#define		STKNAME_LEN				32			// 股名长度
typedef struct tagRCV_REPORT_STRUCTEx
{
	WORD	m_cbSize;									// 结构大小
	int		m_time;										// 交易时间
	WORD	m_wMarket;									// 股票市场类型
	char	m_szLabel[STKLABEL_LEN];					// 股票代码,以'\0'结尾
	char	m_szName[STKNAME_LEN - 15];					// 股票名称,以'\0'结尾
	BYTE	m_Dec : 4;
	BYTE	m_Enabled : 1;
	BYTE	m_Other : 3;
	WORD	m_ID;
	float	m_fOpenInterest;
	float	m_fLastOpenInterest;
	float	m_fLastSettle;

	float	m_fLastClose;								// 昨收
	float	m_fOpen;									// 今开
	float	m_fHigh;									// 最高
	float	m_fLow;										// 最低
	float	m_fNewPrice;								// 最新
	float	m_fVolume;									// 成交量
	float	m_fAmount;									// 成交额

	float	m_fBuyPrice[3];								// 申买价1,2,3
	float	m_fBuyVolume[3];							// 申买量1,2,3
	float	m_fSellPrice[3];							// 申卖价1,2,3
	float	m_fSellVolume[3];							// 申卖量1,2,3

	float	m_fBuyPrice4;								// 申买价4
	float	m_fBuyVolume4;								// 申买量4
	float	m_fSellPrice4;								// 申卖价4
	float	m_fSellVolume4;								// 申卖量4

	float	m_fBuyPrice5;								// 申买价5
	float	m_fBuyVolume5;								// 申买量5
	float	m_fSellPrice5;								// 申卖价5
	float	m_fSellVolume5;								// 申卖量5
	void print()
	{
		std::cout << m_cbSize << "," << m_time << "," << m_wMarket << "," << m_szLabel << "," << m_szName << ",,," << m_fLastClose << "," << m_fOpen << "," << m_fHigh << "," << m_fLow << "," << m_fNewPrice << "," << m_fVolume << "," << m_fAmount << std::endl;
	}
	std::string to_string()
	{
		std::ostringstream _2019_msg;
		_2019_msg << m_cbSize << "," << m_time << "," << m_wMarket << "," << m_szLabel << "," << m_szName << ",,," << m_fLastClose << "," << m_fOpen << "," << m_fHigh << "," << m_fLow << "," << m_fNewPrice << "," << m_fVolume << "," << m_fAmount << std::endl;
		return _2019_msg.str();
	}
} RCV_REPORT_STRUCTEx, *PRCV_REPORT_STRUCTEx;
```
## 2.2 1分钟K线数据
```cpp
typedef struct ROM_TINY_KLINE_DATA
{
	__time32_t		t;					//time stamp of a record.
	short		iDOpen;				//difference value from close.
	short		iDHigh;				//difference value from close.
	short		iDLow;				//difference value from close.
	int			iClose;				//real close price.
	LONGLONG	llVolume;			//real volume value.
	LONGLONG	iDAmount;			//real_amount
	LONGLONG	iDVBAll;			//
	LONGLONG	iDVSAll;			//
};
```
## 2.3 历史日K线数据
```cpp
typedef struct
{
	int date;
	int time;
	LONGLONG amount;
	LONGLONG volume;
	int openprice;
	int closeprice;
	int highprice;
	int lowprice;
}ROM_KLINE_DATA;
```
## 2.4 明细数据
```cpp
//股票的明细数据格式
typedef struct
{
	int		time;
	short	iDPrice;
	int		iDVolume;
}ROM_TICK_DATA;

/*
//指数及板块的明细数据
typedef struct
{
	int		time;
	int		iPrice;
	int		iDVolume;
}ROM_TICK_DATA_EX;
```
# 3.blueye_client.dll(blueye_client64.dll)行情接口初始化
云端行情返回就绪后，会调用回调函数，通知上层应用处理数据，其中数据的结构体定义如下：
```cpp
blueye_call_back(CALL_BACK_DATA *_blueye_data, int _rec_count);
typedef struct
{
	int		type_;		//请求类型
	int		sub_type_;	//请求子类型，当类型type为REQ_TYPE_QUOTE_TICK_LIST时,sub_type=0代表返回数据是如下格式的明细数据 ROM_TICK_DATA， 
 sub_type=1代表返回数据是如下格式的明细数据 ROM_TICK_DATA_EX
	void*	p_data_;		//数据指针(地址)
}CALL_BACK_DATA;
```
回调函数的参数如下：
1, CALL_BACK_DATA *_blueye_data 数据指针；
2，int _rec_count 记录条数。
  对该函数体进行修改，添加数据处理模块。例如保存到文件或者发送给窗体显示（数据通知windows窗体建议将K线数据写入本地缓存文件后使用PostMessage函数通知以避免界面刷新死锁），或者喂数据给算法模块。
```cpp
void CBlueyeQuote::blueye_call_back(CALL_BACK_DATA *_blueye_data, int _rec_count)
{
	if (g_quote_for_window.h_wnd_ && _blueye_data && _blueye_data->p_data_)
	{
		//实时行情数据.
		//Live quote dealing.
		if (_blueye_data->type_ == REQ_TYPE_QUOTE_BROADCAST || _blueye_data->type_ == REQ_TYPE_QUOTE_SUBSCRIBE)
		{
			for (int i = 0; i < _rec_count; i++)
				g_quote_for_window.PushQuoteData(((RCV_REPORT_STRUCTEx*)_blueye_data->p_data_)[i]);
			::SendMessage(g_quote_for_window.h_wnd_, WM_BLUEYE_QUOTE_DATA, _rec_count, 0);
		}
		//1分时数据处理
		//Minutely quote processing.
		else if (_blueye_data->type_ == REQ_TYPE_QUOTE_1M_DATA)
		{
			;
		}
		//日线数据处理
		//Daily data processing.
		else if (_blueye_data->type_ == REQ_TYPE_QUOTE_DAILY_DATA)
		{
			;
		}
	}
	else
	{
		//实时行情数据.
		//Live quote dealing.
		if (_blueye_data->type_ == REQ_TYPE_QUOTE_BROADCAST || _blueye_data->type_ == REQ_TYPE_QUOTE_SUBSCRIBE)
		{
			for (int i = 0; i < _rec_count; i++)
			{
				std::cout << ((RCV_REPORT_STRUCTEx*)_blueye_data->p_data_)[i].to_string() << std::endl;
			}
		}
		//1分时数据处理
		//Minutely quote processing.
		else if(_blueye_data->type_ == REQ_TYPE_QUOTE_1M_DATA)
		{
			for (int i = 0; i < _rec_count; i++)
			{
				std::cout << ((ROM_TINY_KLINE_DATA*)_blueye_data->p_data_)[i].t<<","
					<< ((ROM_TINY_KLINE_DATA*)_blueye_data->p_data_)[i].iClose<<","
					<< ((ROM_TINY_KLINE_DATA*)_blueye_data->p_data_)[i].llVolume << ","
					<< ((ROM_TINY_KLINE_DATA*)_blueye_data->p_data_)[i].iDAmount <<	std::endl;
			}
			std::cout << "total records count is " << _rec_count << std::endl;
		}
		//日线数据处理
		//Daily data processing.
		else if (_blueye_data->type_ == REQ_TYPE_QUOTE_DAILY_DATA)
		{
			for (int i = 0; i < _rec_count; i++)
			{
				std::cout << ((ROM_KLINE_DATA*)_blueye_data->p_data_)[i].date << ","
					<< ((ROM_KLINE_DATA*)_blueye_data->p_data_)[i].openprice << ","
					<< ((ROM_KLINE_DATA*)_blueye_data->p_data_)[i].highprice << ","
					<< ((ROM_KLINE_DATA*)_blueye_data->p_data_)[i].lowprice << "," 
					<< ((ROM_KLINE_DATA*)_blueye_data->p_data_)[i].closeprice << ","
					<< ((ROM_KLINE_DATA*)_blueye_data->p_data_)[i].volume << "," 
					<< ((ROM_KLINE_DATA*)_blueye_data->p_data_)[i].amount  <<std::endl;
			}
			std::cout << "total records count is " << _rec_count << std::endl;
		}

		//日线数据处理
		//Tick data processing.
		else if (_blueye_data->type_ == REQ_TYPE_QUOTE_TICK_LIST)
		{
			if (_blueye_data->sub_type_ == 0)
			{
				for (int i = 0; i < _rec_count; i++)
				{
					std::cout << ((ROM_TICK_DATA*)_blueye_data->p_data_)[i].time << ","
						<< ((ROM_TICK_DATA*)_blueye_data->p_data_)[i].iDPrice << ","
						<< ((ROM_TICK_DATA*)_blueye_data->p_data_)[i].iDVolume << ","<< std::endl;
				}
			}
			else
			{

				for (int i = 0; i < _rec_count; i++)
				{
					std::cout << ((ROM_TICK_DATA_EX*)_blueye_data->p_data_)[i].time << ","
						<< ((ROM_TICK_DATA_EX*)_blueye_data->p_data_)[i].iPrice << ","
						<< ((ROM_TICK_DATA_EX*)_blueye_data->p_data_)[i].iDVolume << "," << std::endl;
				}
			}
		}
	}
}
```
```cpp
//日志回调函数
	static void log(std::string _str_log);

//配置函数
	void read_config_from_file();
该函数将从ini文件中读取相关的配置信息，如版本号、订阅信息、ip、端口等，并将其保存于config_param_中

//初始化函数
	bool init_ex(HANDLE	_shutdown_event, std::string _str_token);
初始化函数负责加载动态链接库，并启动相应的io服务

	void release();
//该函数负责停用io服务

	void switch_window(HWND _h_wnd);
//该函数负责切换窗口句柄，并清除行情队列

	void pack_subscribe(std::vector<STRU_SYMBOL> &_symbols);
//该函数负责对实时行情数据进行打包处理

get_queue_size_ = (p_get_queue_size)GetProcAddress(instance_, "i_get_queue_size");
//获取待发送队列的长度

	void send_business(ROM_QUOTE_HEAD *_p_quote_head);
//将行情信息送入发送队列

bool is_login();
	//判断是否登入
static void blueye_call_back(PRCV_REPORT_STRUCTEx _report_data, int _rec_count);
参数：
PRCV_REPORT_STRUCTEx _report_data， 收到的行情推送数据包指针，该数据结构体详见附录。
int _rec_count ,行情记录条数。
```
# 4. 实时行情订阅接口 API
## CBlueyeQuote::subscribe_ 

参数：std::string _str_subscribing ，格式为交易所代码（2 位，'HS'代表上海，'ZS'代表深圳） +品种代码（6 位），多个品种之间使用逗号隔开。 例如 “HS600000,HS600035,ZS000001,ZS300125”。  

说明：A, 根据此接口订阅指定的数据，最大支持 100 个品种。如果需要定制全市场推送，则 需要通过特定的 token 账号来申请。 B,如果取消订阅，则发送一个不存在的字符，例如“BSDFSDF”。 C,收到的推送数据会通过回调函数 blueye_call_back 发送到上层。

# 5. K 线数据点播下载
## CBlueyeQuote::request_kline_ 

参数：byte _exchange ，交易所代码,0-深圳，1-上海。  
char* _symbol 品种代码。  
int _kline_type,K 线类型, 0-1 分钟，7-日 K 线。  
int _offset,起始位置偏移量。   
int _count,请求数量。  

返回：true,发送成功，false 失败。  

说明：  
A, 收到数据会系统会调用 K 线回调函数 blueye_kline_call_back 将数据传入，用户通 过修改回调函数体处理数据。  
B, 目前支持支持日内 1 分钟和日 K 两类数据的下载，其余数据可以调用 FIX 接口通过 文件方式下载，或者通过 1 分钟和日 K 数据在本地合成其他格式的数据。  

# 6. 明细数据点播下载
## CBlueyeQuote::request_tick_

参数：byte _exchange ，交易所代码,0-深圳，1-上海。  
char* _symbol 品种代码。   
int _count, 请求数量。  

返回：true,发送成功，false 失败。  

说明：  
A, 收到数据会系统会调用 K 线回调函数 blueye_tick_call_back 将数据传入，用户通 过修改回调函数体处理数据。   
B, 如果一个品种的请求数据条数大于最大限制条数（1024），则会以多包的形式返回 数据。最后通过时间序号进行排序合并。   

# 7. 排行榜数据下载
## CBlueyeQuote::request_top_list_

参数： _exchange - 交易所,  
_type - 排行类别,  
_offset - 请求起始偏移量（0 开始）,  
_count - 请求记录条数。   

返回：true,发送成功，false 失败。  

说明：  
A, 收到数据会系统会调用 K 线回调函数 blueye_call_back 将数据传入，用户通过修改 回调函数体处理数据。   
B, 如果一个品种的请求数据条数大于最大限制条数（1024），则会以多包的形式返回 数据。

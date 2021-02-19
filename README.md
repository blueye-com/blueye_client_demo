# 概述
行情支持两种行情协议。  
一种是基于小内存块(小于 20K)的数据推送和点播方式，传输使用自定义二进制数据流， 典型的数据类型包括：单笔实时行情、批量实时行情、各类排行数据、K 线数据（512 条之 内）、交易明细数据(1024 条之内)，等等。服务程序名称为 fix_qengin。  
另外一种是基于文件方式的方式下载，支持大文件下载，传输使用 FIX 的扩展文件协议， 典型的数据类型包括：除权数据、财务数据、资金数据、K 线数据（大于 512 条，按品种、 按日期下载）、交易明细（大于 1024 条，按品种、按日期下载）。服务程序名称为 fix_tengin。    
行情接口 API 的库名称分别为 blueye_client.dll（32 位）和 blueye_client64.dll（64 位），以及 blueye_fix_api.dll（32 位）。    
使用类 CBlueyeQuote 对 blueye_client.dll 或者 blueye_client64.dll 进行封装。    
使用 data_fix 类 blueye_fix_api.dll 进行封装。  
ps: 程序运行在32位环境下，blueye_client32.dll默认为DEBUG，release已压缩为blueye_client32.zip存放于默认路径下  
该工程需安装boost库1.71版本至D盘下，并在工程中设置库目录
boost安装教程https://blog.csdn.net/u010117867/article/details/83118831
# 1.请求类型定义
```cpp
#define		REQ_TYPE_QUOTE_BROADCAST			10		//请求全量数据广播(只针对有权限的账户)
#define		REQ_TYPE_QUOTE_SUBSCRIBE			11		//请求行情订阅(订阅数量上限100只品种)
#define		REQ_TYPE_QUOTE_DEPTH				12		//请求L2深度数据（暂时未开放）
#define		REQ_TYPE_QUOTE_KLINE_DATA			13		//请求K线数据
#define		REQ_TYPE_QUOTE_TICK_LIST			14		//请求明细数据
#define		REQ_TYPE_QUOTE_BATCH_KLINE_DATA	15		//批量请求K线数据
#define		REQ_TYPE_QUOTE_BATCH_TICK_LIST		16		//批量请求明细数据

#define		REQ_TYPE_TEXT						97	
#define		REQ_TYPE_LOGIN					98
#define		REQ_TYPE_HEART_BEAT				99
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
	__time32_t		t;					//时间戳/time stamp of a record.
	short		iDOpen;				//开盘价/与昨日收盘价的差值/difference value from close.
	short		iDHigh;				//最高价/difference value from close.
	short		iDLow;				//最低价/difference value from close.
	int			iClose;			//收盘价/real close price.
	LONGLONG	llVolume;			//成交额/real volume value.
	LONGLONG	iDAmount;			//成交量/real_amount
	LONGLONG	iDVBAll;			//
	LONGLONG	iDVSAll;			//
};
```
## 2.3 历史日K线数据
```cpp
typedef struct
{
	int date;//日期
	int time;//时间
	LONGLONG amount;//成交量
	LONGLONG volume;//成交额
	int openprice;//开盘价
	int closeprice;//收盘价
	int highprice;//最高价
	int lowprice;//最低价
}ROM_KLINE_DATA;
```
## 2.4 明细数据
```cpp
//股票的明细数据格式
typedef struct
{
	int		time;
	short		iDPrice;
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
```cpp
bool CBlueyeQuote::init_ex(HANDLE	_shutdown_event, std::string _str_token)
{
read_config_from_file();

	shutdown_event_ = NULL;
	start_io_service_ = NULL;
	pack_subscribe_ = NULL;
	instance_ = NULL;
	instance_ = LoadLibrary("blueye_client32.dll");
	if (instance_)
	{
		start_io_service_ex_ = (p_start_io_service_ex)GetProcAddress(instance_, "i_start_io_service_ex");
		stop_io_service_ = (p_stop_io_service)GetProcAddress(instance_, "i_stop_io_service");
		pack_subscribe_ = (p_pack_subscribe)GetProcAddress(instance_, "i_pack_subscribe");
		send_business_ = (p_send_business)GetProcAddress(instance_, "i_send_business");
		send_batch_business_ = (p_send_batch_business)GetProcAddress(instance_, "i_send_batch_business");
		is_login_ = (p_is_client_login)GetProcAddress(instance_, "i_is_login");
		clear_business_ = (p_clear_business)GetProcAddress(instance_, "i_clear_business");
		get_queue_size_ = (p_get_client_queue_size)GetProcAddress(instance_, "i_get_queue_size");

		if (start_io_service_ex_)
		{
			start_io_service_ex_(std::string(config_param_.sIp_), std::to_string(config_param_.remote_port_), _shutdown_event, blueye_call_back, log, _str_token);
			return true;
		}
		else
			return false;
	}
	else
return false;
}
```
初始化函数，从配置文件（route.ini）中读取启动io服务的相关参数，加载动态链接库（blueye_client32.dll），并启动相应的io服务
# 4.回调函数接口API
云端行情返回就绪后，会调用回调函数，通知上层应用处理数据，其中数据的结构体定义如下：

```cpp
blueye_call_back(CALL_BACK_DATA *_blueye_data, int _rec_count);
typedef struct
{
	int		type_;		//请求类型
	int		sub_type_;	//请求子类型，当类型type为REQ_TYPE_QUOTE_TICK_LIST时，							sub_type=0代表返回数据是如下格式的明细数据 ROM_TICK_DATA， 
sub_type=1代表返回数据是如下格式的明细数据 ROM_TICK_DATA_EX
	void*	p_data_;		//数据指针(地址)
}CALL_BACK_DATA;

回调函数的参数如下：
1, CALL_BACK_DATA *_blueye_data 数据指针；
2，int _rec_count 记录条数。
```
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
# 5.fix协议接口（历史多分钟K线数据下载）
```cpp
void request_fix_data(int fix_thread_id, byte exchange, std::string symbol,std::string data_type)
{
	std::string _str_file_name = data_type + std::to_string(_exchange) + _symbol + ".dat.zip";
	g_algo_data_fix_.push_file_download_queue(fix_thread_id, data_type, _str_file_name);
	g_algo_data_fix_.download_files(fix_thread_id);
}
request_fix_data(fix_thread_id, SHA, "600036", "w1m_data_by_symbol");
//request_fix_data(fix_thread_id, SHA, "600036", "w3m_data_by_symbol");
//request_fix_data(fix_thread_id, SHA, "600036", "w5m_data_by_symbol");
//request_fix_data(fix_thread_id, SHA, "600036", "w10m_data_by_symbol");
//request_fix_data(fix_thread_id, SHA, "600036", "w15m_data_by_symbol");
//request_fix_data(fix_thread_id, SHA, "600036", "w30m_data_by_symbol");
//request_fix_data(fix_thread_id, SHA, "600036", "w60m_data_by_symbol");
```
**fix_thread_id**:fix线程id  
**exchange**:交易所代码  
**symbol**:股票代码  
**data_type**:获取数据类型  如获取5分钟K线数据："w1m_data_by_symbol"  
**基于文件方式的方式下载，支持大文件下载，传输使用 FIX 的扩展文件协议， 典型的数据类型包括：除权数据、财务数据、资金数据、K 线数据（大于 512 条，按品种、 按日期下载）、交易明细（大于 1024 条，按品种、按日期下载）**

# 6.实时行情接口API
```cpp
void subscribe_live_quote(CBlueyeQuote *_p_blueye_quote)
{
	if (_p_blueye_quote)
	{
		std::vector<STRU_SYMBOL> _symbols;
		STRU_SYMBOL _symbol;
		_symbol.nExchange = SHA;
		strcpy(_symbol.symbol, "600000");
		_symbols.push_back(_symbol);
		strcpy(_symbol.symbol, "600036");
		_symbols.push_back(_symbol);
		_p_blueye_quote->pack_subscribe(_symbols);
	}
}
```
说明：  
A, 根据此接口订阅指定的数据，最大支持100个品种。如果需要定制全市场推送，则需要通过特定的token账号来申请。
B,如果取消订阅，则发送一个不存在的字符，例如“BSDFSDF”。
C,收到的推送数据会通过回调函数blueye_call_back发送到上层。
# 7.K线数据点播下载
```cpp
void request_1m_data(CBlueyeQuote *_p_blueye_quote,byte _exchange,char *_symbol)
{
	ROM_QUOTE_HEAD _quote_head;
	memset(&_quote_head, 0, sizeof(_quote_head));
	_quote_head.reset(REQ_TYPE_QUOTE_KLINE_DATA, "sdfsdfsd");
	_quote_head.exchange = _exchange;
	memcpy(_quote_head.symbol, _symbol, ROM_SYMBOL_LEN);
	_quote_head.t1 = ROM_QUOTE_HEAD::P1M;
	_p_blueye_quote->send_business(&_quote_head);
}


void request_daily_data(CBlueyeQuote *_p_blueye_quote, byte _exchange, char *_symbol)
{
	ROM_QUOTE_HEAD _quote_head;
	memset(&_quote_head, 0, sizeof(_quote_head));
	_quote_head.reset(REQ_TYPE_QUOTE_KLINE_DATA, "E7GH8ZH81E5T3Y6K86l0J3H6M9ML9JZH9HPH6B8H");
	_quote_head.exchange = _exchange;
	memcpy(_quote_head.symbol, _symbol, ROM_SYMBOL_LEN);
	_quote_head.d1 = 0;
	_quote_head.d2 = 200;
	_quote_head.t1 = ROM_QUOTE_HEAD::P_DAILY;
	_p_blueye_quote->send_business(&_quote_head);
}
```
说明：  
A, 收到数据会系统会调用回调函数blueye_call_back将数据传入，用户通过修改回调函数体处理数据。
B, 目前支持日内1分钟和日K两类数据的下载，其余数据可以调用FIX接口通过文件方式下载，或者通过1分钟和日K数据在本地合成其他格式的数据。
# 8.明细数据点播下载  
```cpp
void request_tick_data(CBlueyeQuote *_p_blueye_quote, byte _exchange, char *_symbol)
{
	ROM_QUOTE_HEAD _quote_head;
	memset(&_quote_head, 0, sizeof(_quote_head));
	_quote_head.reset(REQ_TYPE_QUOTE_TICK_LIST, "sdfsdfsd");
	_quote_head.exchange = _exchange;
	memcpy(_quote_head.symbol, _symbol, ROM_SYMBOL_LEN);
	_quote_head.d1 = 0;
	_quote_head.d2 = 400;
	_p_blueye_quote->send_business(&_quote_head);
}
```
说明：  
A, 收到数据会系统会调用K线回调函数blueye_call_back将数据传入，用户通过修改回调函数体处理数据。
B, 如果一个品种的请求数据条数大于最大限制条数（1024），则会以多包的形式返回数据。最后通过时间序号进行排序合并。
# 9.批量数据下载接口API
```cpp
void request_batch_data(CBlueyeQuote *_p_blueye_quote)
{
	ROM_QUOTE_HEAD _quote_head;
	memset(&_quote_head, 0, sizeof(_quote_head));
	_quote_head.reset(REQ_TYPE_QUOTE_BATCH_KLINE_DATA, "E7GH8ZH81E5T3Y6K86l0J3H6M9ML9JZH9HPH6B8H");
	//_quote_head.exchange = _exchange;
	//memcpy(_quote_head.symbol, _symbol, ROM_SYMBOL_LEN);
	_quote_head.d1 = 0;
	_quote_head.d2 = 200;
	_quote_head.t1 = ROM_QUOTE_HEAD::P_DAILY;

	std::vector<STRU_SYMBOL> _symbols;
	STRU_SYMBOL _symbol;
	_symbol.nExchange = SHA;
	strcpy(_symbol.symbol, "600000");
	_symbols.push_back(_symbol);
	strcpy(_symbol.symbol, "600036");
	_symbols.push_back(_symbol);

	_symbol.nExchange = SHA;
	strcpy(_symbol.symbol, "600050");
	_symbols.push_back(_symbol);

	_symbol.nExchange = SZN;
	strcpy(_symbol.symbol, "002236");
	_symbols.push_back(_symbol);
	std::string _str_symbols_list = _p_blueye_quote->pack_batch_request_symbols(_symbols);

	if(_str_symbols_list != "")
		_p_blueye_quote->send_batch_business(&_quote_head, _str_symbols_list);
}
```
说明：  
A, 通过调用CBlueyeQuote类提供的send_batch_business接口实现行情数据请求的批量发送。  
B, 行情头的reset函数可设置请求的行情数据类型及token密钥，通过设置行情头的t1变量可获取不同的分时数据。  
# 10.分钟数据转换接口API  
```cpp
void ManyMinutesDataConversion(int rec_count,int exchange , std::string symbol, ROM_KLINE_DATA* kline,ROM_KLINE_DATA* outcome_kine,int expected_type)
```
**rec_count**:转化的记录条数  
**exchange**:交易所代码  
**symbol**:股票代码  
**kline**:原始数据  
**outcome_kline**:经过转化的数据  
**expected_type**:转化的数据类型：  

#define KLINE_TYPE_MINUTE1_DATA                                  1                                                //1分钟K线  
#define KLINE_TYPE_MINUTE5_DATA                                  5                                                //5分钟K线  
#define KLINE_TYPE_MINUTE15_DATA                                15                                                //15分钟K线  
#define KLINE_TYPE_MINUTE30_DATA                                30                                                //30分钟K线  
#define KLINE_TYPE_MINUTE60_DATA                                60                                                //60分钟K线  
# 行情接口调用示例
```cpp
	CBlueyeQuote blueye_quote_;
	HANDLE _m_shutdown_event = CreateEvent(NULL, TRUE, FALSE, "");
	int fix_thread_id;

	if (blueye_quote_.init_ex(_m_shutdown_event, "E7GH8ZH81E5T3Y6K86l0J3H6M9ML9JZH9HPH6B8H") 
		&& (fix_thread_id = p_this_instance_->start_fix("119.23.219.55", "6001", NULL, NULL)))
	{
		blueye_quote_.switch_window(NULL);
		while (!blueye_quote_.is_login() || !g_algo_data_fix_.is_login(fix_thread_id))
		{
			Sleep(1000);
		}
	
		//subscribe_live_quote(&blueye_quote_);
		//request_1m_data(&blueye_quote_, SHA, (char *)"600036");
		//request_daily_data(&blueye_quote_, SHA, (char *)"600200");
		//request_tick_data(&blueye_quote_, SHA, (char *)"600036");
		//request_batch_data(&blueye_quote_); //批量下载股票
		
		//request_fix_data(fix_thread_id, SHA, "600036", "w1m_data_by_symbol");
		//request_fix_data(fix_thread_id, SHA, "600036", "w3m_data_by_symbol");
		request_fix_data(fix_thread_id, SHA, "600036", "w5m_data_by_symbol");
		//request_fix_data(fix_thread_id, SHA, "600036", "w10m_data_by_symbol");
		//request_fix_data(fix_thread_id, SHA, "600036", "w15m_data_by_symbol");
		//request_fix_data(fix_thread_id, SHA, "600036", "w30m_data_by_symbol");
		//request_fix_data(fix_thread_id, SHA, "600036", "w60m_data_by_symbol");
		
		while (1)
		{
			Sleep(10000);
		}
	}
	else
	{
		std::cout << "Failed in blueye_quote_.init_ex" << std::endl;
	}
	return 0;

```
示例为向fix服务器请求股票代码为600036的5分钟K线数据，启动fix服务后调用request_fix_data向服务器请求指定数据，在回调函数中可对返回的数据进行解压及其他处理
```cpp
	if ( _str_json.find("w1m_data_by_symbol") != std::string::npos)
	{
		int npos = _str_json.find(".zip");
		std::string _file_name = _str_json.substr(0, npos);
		std::string _fix_download_file_name = ipfs.file_root_path_ + "w1m_data_by_symbol" + "\\" + _file_name;
		FILE* _fp = fopen(_fix_download_file_name.c_str(), "rb+");
		fseek(_fp, 0, SEEK_END);
		int _file_len = ftell(_fp);
		fseek(_fp, 0, SEEK_SET);
		int _rec_count = _file_len / sizeof(ROM_KLINE_DATA);
		if (_file_len % sizeof(ROM_KLINE_DATA) == 0)
		{
			for (int i = 0; i < _rec_count; i++)
			{
				ROM_KLINE_DATA* _single_record = new ROM_KLINE_DATA;
				fread(_single_record, 1, sizeof(ROM_KLINE_DATA), _fp);
				std::cout << _single_record->volume << ","
					<< _single_record->amount << std::endl;
			}
		}
	}
```
示例中对返回的数据做了打印处理

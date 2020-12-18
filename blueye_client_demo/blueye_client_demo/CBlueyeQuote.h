#pragma once
#pragma pack(push,1)

#include <string>
#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <set>
#include "blueye_client_common.h"

#include "stdafx.h"
//#include "..\..\common\lambda\lambda_common.h"

// 证券市场
#define		SH_MARKET_EX			'HS'		// 上海
#define		SZ_MARKET_EX			'ZS'		// 深圳
#define		HK_MARKET_EX			'KH'		// 香港
#define		HK_FUTURE_MARKET_EX		23112
#define		USA1_FUTURE_MARKET_EX	17481
#define		USA2_FUTURE_MARKET_EX	20823
#define		SH_FUTRE_MARKET_EX		20814
#define		CN_FUTRE_MARKET_EX		18003
#define		FOREX_MARKET_EX			18519
#define		GLOBAL_MARKET_EX		22601		//全球指数

#define		NUMBER_MULTIPLY_FOR_STOCK			1000
#define		NUMBER_MULTIPLY_FOR_FUND			1000
#define		NUMBER_MULTIPLY_FOR_BOND			1000
#define		NUMBER_MULTIPLY_FOR_OPTION			10000
#define		NUMBER_MULTIPLY_FOR_FUTURE			10000
#define		NUMBER_MULTIPLY_FOR_FOREX			10000

//如下是行情接收库需要的一些头文件和函数指针定义////////////////////////
#define		STKLABEL_LEN			10			// 股号数据长度,国内市场股号编码兼容钱龙
#define		STKNAME_LEN				32			// 股名长度

typedef struct tagRCV_FILE_HEADEx
{
	DWORD	m_dwAttrib;							// 文件子类型
	DWORD	m_dwLen;							// 文件长度
	union
	{
		DWORD	m_dwSerialNo;					// 序列号,对股评
		time_t	m_FileTime;						// 文件创建时间
	};
	char	m_szFileName[MAX_PATH];				// 文件名 or URL
} RCV_FILE_HEADEx, *PRCV_FILE_HEADEx;


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
typedef struct tagRCV_DATA
{
	int					m_wDataType;			// 文件类型
	int					m_nPacketNum;			// 记录数,参见注一
	RCV_FILE_HEADEx		m_File;					// 文件接口
	BOOL				m_bDISK;				// 文件是否已存盘的文件
	union
	{
		RCV_REPORT_STRUCTEx  *	m_pReport;
		//RCV_REPORT_STRUCTEx1 *	m_pReport1;
		//RCV_HISTORY_STRUCTEx *	m_pDay;
		//RCV_MINUTE_STRUCTEx  *	m_pMinute;
		//RCV_POWER_STRUCTEx	 *	m_pPower;
		void				 *	m_pData;		// 参见注二
	};
} RCV_DATA, *PRCV_DATA;

typedef void(*p_report_data_call_back)(PRCV_REPORT_STRUCTEx _report_data, int _rec_count);
typedef void(*p_log)(std::string _str_log);
typedef void(*p_start_io_service)(std::string _str_ip, std::string _str_port, HANDLE	_shutdown_event, p_report_data_call_back _p_report_data_call_back, p_log _log, std::string _str_token);
typedef void(*p_stop_io_service)();
typedef void(*p_pack_subscribe)(std::string str_symbols_list_);

#define WM_BLUEYE_QUOTE_DATA WM_USER+5

typedef struct
{
	int			exchange_;
	std::string symbol_;
	std::string algo_name_;
	std::string param_name_;
	int			order_type_;	// 1-buy, 2-sell
	int			price_;
	int			volume_;
	int			price_type_;
	int			period_type_;
	int			open_order_point_;
	time_t		open_order_time_;
	char		status_;
	ROM_REAL_TIME_DATA_BS5 rom_quote_;
	enum { PriceTypeLimit = 0, PrieTypeAny };
	enum {StatusPending=0,StatusProcessing,StatusExpired};
	void set(std::string _symbol)
	{
		exchange_ = SZN;
		symbol_ = _symbol;
		algo_name_ = "";
		order_type_ = 1;
		period_type_ = 7;
		price_ = 0;
		volume_ = 0;
		price_type_ = 1;	//market type
		memset(&rom_quote_, 0, sizeof(ROM_REAL_TIME_DATA_BS5));
	}
	std::string gen_key() { return std::to_string(exchange_) + symbol_; }
	std::string get_string()
	{
		std::string _str_me = std::to_string(exchange_) + "," + symbol_ + "," + algo_name_ + "," + param_name_ + "," + std::to_string(order_type_) + "," + std::to_string(price_) + "," +
			std::to_string(volume_) + "," + std::to_string(price_type_) + "," + std::to_string(period_type_) + "," + std::to_string(open_order_point_) + "," + std::to_string(open_order_time_) + "," + std::to_string(status_);
		return _str_me;
	}
}tag_ORDER;

class CQuoteForWindow
{
protected:
	//CRITICAL_SECTION					quote_queue_section_;
	std::deque<RCV_REPORT_STRUCTEx>		new_quote_queue_;
public:
	enum {MaxQueueSize = 1024};
	HWND								h_wnd_;
public:
	CQuoteForWindow()
	{
		h_wnd_ = NULL;
		//InitializeCriticalSection(&quote_queue_section_);
	}
	~CQuoteForWindow()
	{
		//DeleteCriticalSection(&quote_queue_section_);
		new_quote_queue_.clear();
	}
	int PushQuoteData(RCV_REPORT_STRUCTEx &_rom_quote)
	{
		//EnterCriticalSection(&quote_queue_section_);
		if(new_quote_queue_.size()<MaxQueueSize)
			new_quote_queue_.push_back(_rom_quote);
		//LeaveCriticalSection(&quote_queue_section_);
		return 1;
	}
	int PopQuoteData(RCV_REPORT_STRUCTEx &_rom_quote)
	{
		//EnterCriticalSection(&quote_queue_section_);
		int _ret = 0;
		if (new_quote_queue_.size() > 0)
		{
			_rom_quote = new_quote_queue_.front();
			new_quote_queue_.pop_front();
			_ret = 1;
		}
		//LeaveCriticalSection(&quote_queue_section_);
		return _ret;
	}

	void Clear()
	{
		//EnterCriticalSection(&quote_queue_section_);
		new_quote_queue_.clear();
		//LeaveCriticalSection(&quote_queue_section_);
	}
};

#define IP_ADDRESS_LEN		15
#define TOKEN_LENGTH		40
typedef struct
{
	char version_[TOKEN_LENGTH];
	int full_data_;
	int subscribe_;
	int use_local_dll_;
	char sIp_[IP_ADDRESS_LEN + 1];
	int remote_port_;
	int listen_port_;
	char token_[TOKEN_LENGTH];	//token as an communication account to logon.
}tag_CONFIG_PARAM;


#pragma pack(pop)

class CBlueyeQuote
{
protected:
	p_start_io_service		start_io_service_;
	p_stop_io_service		stop_io_service_;
	p_pack_subscribe		pack_subscribe_;
	HINSTANCE				instance_;
	HANDLE					shutdown_event_;
	tag_CONFIG_PARAM		config_param_;
public:
	CBlueyeQuote();
	~CBlueyeQuote();
	static void blueye_call_back(PRCV_REPORT_STRUCTEx _report_data, int _rec_count);
	static void log(std::string _str_log);
	void read_config_from_file();
	bool init(HANDLE	_shutdown_event, std::string _str_token);
	void release();
	void switch_window(HWND _h_wnd);
	void pack_subscribe(std::vector<STRU_SYMBOL> &_symbols);
	int pop_quote(RCV_REPORT_STRUCTEx &_rom_quote);
	void stop_data();
	static int convert_data(RCV_REPORT_STRUCTEx *pSource, ROM_REAL_TIME_DATA_BS5 *pDest);
	static int get_number_multiply(int iSymbolType);
	static void code_wjf_2_rom(WORD wMarket, char *sWjf, char *sRom);
	static unsigned char get_stock_type(WORD dwExchange, char *sSymbol);
};


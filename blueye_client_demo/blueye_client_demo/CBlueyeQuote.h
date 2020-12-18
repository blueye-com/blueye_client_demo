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

// ֤ȯ�г�
#define		SH_MARKET_EX			'HS'		// �Ϻ�
#define		SZ_MARKET_EX			'ZS'		// ����
#define		HK_MARKET_EX			'KH'		// ���
#define		HK_FUTURE_MARKET_EX		23112
#define		USA1_FUTURE_MARKET_EX	17481
#define		USA2_FUTURE_MARKET_EX	20823
#define		SH_FUTRE_MARKET_EX		20814
#define		CN_FUTRE_MARKET_EX		18003
#define		FOREX_MARKET_EX			18519
#define		GLOBAL_MARKET_EX		22601		//ȫ��ָ��

#define		NUMBER_MULTIPLY_FOR_STOCK			1000
#define		NUMBER_MULTIPLY_FOR_FUND			1000
#define		NUMBER_MULTIPLY_FOR_BOND			1000
#define		NUMBER_MULTIPLY_FOR_OPTION			10000
#define		NUMBER_MULTIPLY_FOR_FUTURE			10000
#define		NUMBER_MULTIPLY_FOR_FOREX			10000

//������������տ���Ҫ��һЩͷ�ļ��ͺ���ָ�붨��////////////////////////
#define		STKLABEL_LEN			10			// �ɺ����ݳ���,�����г��ɺű������Ǯ��
#define		STKNAME_LEN				32			// ��������

typedef struct tagRCV_FILE_HEADEx
{
	DWORD	m_dwAttrib;							// �ļ�������
	DWORD	m_dwLen;							// �ļ�����
	union
	{
		DWORD	m_dwSerialNo;					// ���к�,�Թ���
		time_t	m_FileTime;						// �ļ�����ʱ��
	};
	char	m_szFileName[MAX_PATH];				// �ļ��� or URL
} RCV_FILE_HEADEx, *PRCV_FILE_HEADEx;


typedef struct tagRCV_REPORT_STRUCTEx
{
	WORD	m_cbSize;									// �ṹ��С
	int		m_time;										// ����ʱ��
	WORD	m_wMarket;									// ��Ʊ�г�����
	char	m_szLabel[STKLABEL_LEN];					// ��Ʊ����,��'\0'��β
	char	m_szName[STKNAME_LEN - 15];					// ��Ʊ����,��'\0'��β
	BYTE	m_Dec : 4;
	BYTE	m_Enabled : 1;
	BYTE	m_Other : 3;
	WORD	m_ID;
	float	m_fOpenInterest;
	float	m_fLastOpenInterest;
	float	m_fLastSettle;

	float	m_fLastClose;								// ����
	float	m_fOpen;									// ��
	float	m_fHigh;									// ���
	float	m_fLow;										// ���
	float	m_fNewPrice;								// ����
	float	m_fVolume;									// �ɽ���
	float	m_fAmount;									// �ɽ���

	float	m_fBuyPrice[3];								// �����1,2,3
	float	m_fBuyVolume[3];							// ������1,2,3
	float	m_fSellPrice[3];							// ������1,2,3
	float	m_fSellVolume[3];							// ������1,2,3

	float	m_fBuyPrice4;								// �����4
	float	m_fBuyVolume4;								// ������4
	float	m_fSellPrice4;								// ������4
	float	m_fSellVolume4;								// ������4

	float	m_fBuyPrice5;								// �����5
	float	m_fBuyVolume5;								// ������5
	float	m_fSellPrice5;								// ������5
	float	m_fSellVolume5;								// ������5
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
	int					m_wDataType;			// �ļ�����
	int					m_nPacketNum;			// ��¼��,�μ�עһ
	RCV_FILE_HEADEx		m_File;					// �ļ��ӿ�
	BOOL				m_bDISK;				// �ļ��Ƿ��Ѵ��̵��ļ�
	union
	{
		RCV_REPORT_STRUCTEx  *	m_pReport;
		//RCV_REPORT_STRUCTEx1 *	m_pReport1;
		//RCV_HISTORY_STRUCTEx *	m_pDay;
		//RCV_MINUTE_STRUCTEx  *	m_pMinute;
		//RCV_POWER_STRUCTEx	 *	m_pPower;
		void				 *	m_pData;		// �μ�ע��
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


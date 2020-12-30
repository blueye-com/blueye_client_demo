// blueye_client_demo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "CBlueyeQuote.h"
#include "data_fix.h"
#include "CBlueyeQuoteFile.h"
#include "ZipData.h"

extern CBlueyeQuoteFile *p_this_instance_;
extern data_fix	g_algo_data_fix_;
extern blueye_ipfs g_algo_blueye_ipfs_;

#define SYMBOL_LEN 6

void TestDownloadData()
{
	char _root_file[128] = {};
	char _str_ip[128] = "119.23.219.55",_str_port[128] = "6001";
	_getcwd(_root_file, MAX_PATH);
	/*strcat(_root_file, "\\blueye_algo.ini");
	GetPrivateProfileString("daily_tasks", "data_fix_ip", "fix3.blueye.info", _str_ip, sizeof(_str_ip), _root_file);
	GetPrivateProfileString("daily_tasks", "data_fix_port", "6001", _str_port, sizeof(_str_port), _root_file);*/
	int _thread_id = p_this_instance_->start_fix(_str_ip, _str_port, NULL,NULL);
	if (_thread_id > 0)
	{
		while (!g_algo_data_fix_.is_login(_thread_id))
		{
			Sleep(1000);
		}
		std::string _str_file_name = "day_data_by_symbol1600036.dat.zip";
		g_algo_data_fix_.push_file_download_queue(_thread_id, "day_data_by_symbol", _str_file_name);
		g_algo_data_fix_.download_files(_thread_id);
	}
}

CString GetModuleFilePath(HMODULE hModule)
{
	char sIniPath[MAX_PATH] = { 0 };
	::GetModuleFileName(hModule, sIniPath, MAX_PATH);

	CString strIniPath = sIniPath;
	int iPos = strIniPath.ReverseFind('\\');
	strIniPath = strIniPath.Left(iPos);

	return strIniPath;
}

/* 
   订阅行情的示例函数
   The exmple for live quote data subscribing.
   
   返回数据将通过回调函数来获取cm
   Data will be processed in the call back function.
 */
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

/*
   查询1分钟数据的示例函数
   The exmple for minutely quote data subscribing.

   返回数据将通过回调函数来获取
   Data will be processed in the call back function.
 */
void request_1m_data(CBlueyeQuote *_p_blueye_quote,byte _exchange,char *_symbol)
{
	ROM_QUOTE_HEAD _quote_head;
	memset(&_quote_head, 0, sizeof(_quote_head));
	_quote_head.reset(REQ_TYPE_QUOTE_KLINE_DATA, "E7GH8ZH81E5T3Y6K86l0J3H6M9ML9JZH9HPH6B8H");
	_quote_head.exchange = _exchange;
	memcpy(_quote_head.symbol, _symbol, ROM_SYMBOL_LEN);
	_quote_head.t1 = ROM_QUOTE_HEAD::P1M;
	_p_blueye_quote->send_business(&_quote_head);
}

/*
   查询日K线数据的示例函数
   The exmple for daily quote data subscribing.

   返回数据将通过回调函数来获取
   Data will be processed in the call back function.
 */
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

/*
   查询批量股票的K线数据的示例函数
   The exmple for batch kline quote data request.

   返回数据将通过回调函数来获取
   Data will be processed in the call back function.
 */
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

	if (_str_symbols_list != "")
		_p_blueye_quote->send_batch_business(&_quote_head, _str_symbols_list);
}
/*
   查询明细数据的示例函数
   The exmple for tick quote data subscribing.

   返回数据将通过回调函数来获取
   Data will be processed in the call back function.
 */
void request_tick_data(CBlueyeQuote *_p_blueye_quote, byte _exchange, char *_symbol)
{
	ROM_QUOTE_HEAD _quote_head;
	memset(&_quote_head, 0, sizeof(_quote_head));
	_quote_head.reset(REQ_TYPE_QUOTE_TICK_LIST, "E7GH8ZH81E5T3Y6K86l0J3H6M9ML9JZH9HPH6B8H");
	_quote_head.exchange = _exchange;
	memcpy(_quote_head.symbol, _symbol, ROM_SYMBOL_LEN);
	_quote_head.d1 = 0;
	_quote_head.d2 = 400;
	_p_blueye_quote->send_business(&_quote_head);
}


int main()
{
	CBlueyeQuote blueye_quote_;
	HANDLE _m_shutdown_event = CreateEvent(NULL, TRUE, FALSE, "");

	if (blueye_quote_.init_ex(_m_shutdown_event, "E7GH8ZH81E5T3Y6K86l0J3H6M9ML9JZH9HPH6B8H"))
	{
		blueye_quote_.switch_window(NULL);
		while (!blueye_quote_.is_login())
		{
			Sleep(1000);
		}
	
		subscribe_live_quote(&blueye_quote_);
		request_1m_data(&blueye_quote_, SHA, (char *)"600036");
		request_daily_data(&blueye_quote_, SHA, (char *)"600200");
		request_tick_data(&blueye_quote_, SHA, (char *)"600036");
		//request_batch_data(&blueye_quote_); //批量下载股票
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
}


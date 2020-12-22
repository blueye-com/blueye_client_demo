// blueye_client_demo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "CBlueyeQuote.h"

/* 
   订阅行情的示例函数
   The exmple for live quote data subscribing.
   
   返回数据将通过回调函数来获取
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
	_quote_head.reset(REQ_TYPE_QUOTE_1M_DATA, "sdfsdfsd");
	_quote_head.exchange = _exchange;
	memcpy(_quote_head.symbol, _symbol, ROM_SYMBOL_LEN);
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
	_quote_head.reset(REQ_TYPE_QUOTE_DAILY_DATA, "sdfsdfsd");
	_quote_head.exchange = _exchange;
	memcpy(_quote_head.symbol, _symbol, ROM_SYMBOL_LEN);
	_quote_head.d1 = 0;
	_quote_head.d2 = 200;
	_p_blueye_quote->send_business(&_quote_head);
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
	_quote_head.reset(REQ_TYPE_QUOTE_TICK_LIST, "sdfsdfsd");
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


	if (blueye_quote_.init_ex(_m_shutdown_event, "sdfsdfs"))
	{
		blueye_quote_.switch_window(NULL);

		Sleep(10000);
		subscribe_live_quote(&blueye_quote_);
		request_1m_data(&blueye_quote_, SHA, (char *)"600036");
		request_daily_data(&blueye_quote_, SHA, (char *)"600036");
		request_tick_data(&blueye_quote_, SHA, (char *)"600036");
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

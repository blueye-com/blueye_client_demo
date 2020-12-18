#pragma once

#include <sstream>

#include "stdafx.h"

#define WM_STOCK_DATA                WM_USER+2
#define WM_UPDATE_CODE_DATA WM_USER+4
#define STOCK_DOWNLOAD_DATA WM_USER+1

#define WM_TEST_NOTIFICATION WM_USER+4

#define STOCK_DOWNLOAD_PROCCESSING        -10000
#define STOCK_DOWNLOAD_FINISHED                10

#define SHORT_PULL_CONNECTION                997                        //short pulling connection.
#define LONG_PUSH_CONNECTION                998                        //long pushing connection.
#define LONG_PULL_CONNECTION                999                        //long pulling connection.
#define OTHER_CONNECTION                        1000                //短连接
#define HEART_BEAT_TEST                                1111                //心跳检测

#define SYMBOL_QCLIENT_LEN                        6                        //length of stock code.
#define SYMBOL_NAME_LEN                                16                        //length of stock name.
#define SYMBOL_SPELL_LEN                        8                        //chinese spelling string length of symbol.
#define USER_LEN                                        20                        //user name length.
#define PWD_LEN                                                20                        //password length.

#define SZN                0
#define SHA                1

#define ROM_SYMBOL_LEN 6

#define INPUT_HEADER_SIZE                        sizeof(ROM_QUOTE_HEAD)
#define OUTPUT_HEADER_SIZE                        8
#define DEFAULT_BODY_LENGTH                        (1024*1024)

#define MAX_QUEUE_ELEMENT_COUNT                10

#define CLIENT_ADDRESS_LEN                        18

/*
 * structure for real time quote data.
 */
typedef struct
{
	byte symbol[SYMBOL_QCLIENT_LEN];
	byte symbol_name[SYMBOL_NAME_LEN];
	byte exchange;                                                        //exchange place code,0-ShenZhen,1-ShangHai.  
	byte type;
	LONGLONG amount;
	LONGLONG volume;
	int openprice;
	int closeprice;
	int highprice;
	int lowprice;
	int newprice;
	LONGLONG vbuy1, vbuy2, vbuy3, vbuy4, vbuy5;
	int pbuy1, pbuy2, pbuy3, pbuy4, pbuy5;
	LONGLONG vsell1, vsell2, vsell3, vsell4, vsell5;
	int psell1, psell2, psell3, psell4, psell5;
	LONGLONG vbuyAll, vsellAll;
	int        reserved1, reserved2, reserved3;                //the filed(reserved1) saves the price changing rate,
																					//the field(reserved2) saves average volume per minute during the last five trading days.过去5日平均每分钟成交量
																					//the field(reserved3) unused.
	int time;
}ROM_REAL_TIME_DATA_BS5;


//symbol
typedef struct {
	int                        nExchange;                                                //market code.
	char                symbol[ROM_SYMBOL_LEN + 1];                //symbol
	std::string gen_keystring()
	{
		std::ostringstream _2019_msg;

		try
		{
			_2019_msg.str("");
			_2019_msg << nExchange << symbol;
			return _2019_msg.str();
		}
		catch (...)
		{
			return "";
		}
	}
} STRU_SYMBOL;
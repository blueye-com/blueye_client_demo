#pragma once
#include "stdafx.h"
#include "blueye_client_common.h"

#define ROM_SYMBOL_TYPE_SZN_INDEX		18
#define ROM_SYMBOL_TYPE_SZN_A			1
#define ROM_SYMBOL_TYPE_SZN_B			12
#define ROM_SYMBOL_TYPE_SZN_BOND		6
#define ROM_SYMBOL_TYPE_SZN_FUND		10
#define ROM_SYMBOL_TYPE_SZN_OPTION		11
#define ROM_SYMBOL_TYPE_SZN_CREATIVE	20

#define ROM_SYMBOL_TYPE_SHA_INDEX		30
#define ROM_SYMBOL_TYPE_SHA_A			40
#define ROM_SYMBOL_TYPE_SCI_CREATIVE	41			//科创版
#define ROM_SYMBOL_TYPE_SHA_B			50
#define ROM_SYMBOL_TYPE_SHA_BOND		60
#define ROM_SYMBOL_TYPE_SHA_FUND		70
#define ROM_SYMBOL_TYPE_SHA_OPTION		71

#define ROM_SYMBOL_TYPE_SH_FUTURE		81
#define ROM_SYMBOL_TYPE_ZZ_FUTURE		82
#define ROM_SYMBOL_TYPE_DL_FUTURE		83
#define ROM_SYMBOL_TYPE_ZJ_FUTURE		84
#define ROM_SYMBOL_TYPE_TJ_SILVER		85

#define KLINE_TYPE_MINUTE1_DATA                                        1                                                //1分钟K线
#define KLINE_TYPE_MINUTE5_DATA                                        5                                                //5分钟K线
#define KLINE_TYPE_MINUTE15_DATA                                15                                                //15分钟K线
#define KLINE_TYPE_MINUTE30_DATA                                30                                                //30分钟K线
#define KLINE_TYPE_MINUTE60_DATA                                60                                                //60分钟K线
#define KLINE_TYPE_MINUTE120_DATA				120						//120分钟K线
#define KLINE_TYPE_MINUTE240_DATA				241						//240分钟K线
#define KLINE_TYPE_DAY_DATA                                                240                                                //日K线
#define KLINE_TYPE_WEEK_DATA                                        1200                                        //周K线        
#define KLINE_TYPE_MONTH_DATA                                        4800                                        //月K线

#define ROM_SYMBOL_TYPE_UNKNOWN			99
#define ROM_SYMBOL_TYPE_BLOCK			100

int FilterUnusualData(float fDifference, ROM_KLINE_DATA * pRomKLine, int nKlineCount);
void GeneryMinuteKlinePath(CString &sFilePath, char *sSymbol, BYTE exchange, int iKLineType, int iMarkType);
int GenerateManyMinutlyKLine(const CString& sFilePath, ROM_KLINE_DATA *romMinKlineData, int realCount, ROM_KLINE_DATA *pManyMiniteKlineData, int nMaxCount, int iKLineType, int iMarkType);
void ManyMinutesDataConversion(int rec_count, int exchange, std::string symbol, ROM_KLINE_DATA* kline, ROM_KLINE_DATA* outcome_kine, int expected_type);
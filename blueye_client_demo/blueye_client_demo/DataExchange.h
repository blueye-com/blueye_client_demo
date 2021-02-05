#pragma once
#include "blueye_client_common.h"
#include "blueye_ipfs.h"

#define ROM_KLINE_DATA_LEN        40
#define CONTENT_CAPACITY		  5000000
class DataExchange
{
private:
	blueye_ipfs ipfs_min_;
	BLUEYE_MEM swap;
	BLUEYE_MEM swap_ex_;
public:
	DataExchange();
	~DataExchange();
	int insert_unique_sort_value2(ROM_KLINE_DATA *pKLineArray, int nCount, ROM_KLINE_DATA *pKLineSingle);
	void update_minutely_kline(byte _exchange, char *_symbol, std::string _str_data_type, ROM_KLINE_DATA *_pp_new_kline, int _new_rec_count, int _total_symbols_count);
};
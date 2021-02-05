#include <iostream>
#include "DataExchange.h"


// return UTC time via date and seconds
time_t GetUTCTime(int date, int seconds)
{
	struct tm t;
	t.tm_year = date / 10000 - 1900;
	t.tm_mon = date % 10000 / 100 - 1;
	t.tm_mday = date % 100;
	t.tm_hour = seconds / 3600;
	t.tm_min = seconds % 3600 / 60;
	t.tm_sec = seconds % 60;
	return mktime(&t);
}

DataExchange::DataExchange(void)
{
	swap_ex_.mem_ = new char[5000000];
	swap_ex_.len_ = CONTENT_CAPACITY;
}


DataExchange::~DataExchange(void)
{
	;
}

int DataExchange::insert_unique_sort_value2(ROM_KLINE_DATA *pKLineArray, int nCount, ROM_KLINE_DATA *pKLineSingle)
{
	if (pKLineArray == NULL || nCount < 0 || pKLineSingle == NULL) return nCount;
	int low = 0, high = nCount - 1, mid, pos, i;
	int found = 0;
	time_t        t1, t2;
	while (low <= high && !found)                    //采用二分查找法，查找pKLineSingle的插入位置
	{
		mid = (low + high) / 2;
		t1 = GetUTCTime(pKLineSingle->date, pKLineSingle->time);
		t2 = GetUTCTime(pKLineArray[mid].date, pKLineArray[mid].time);
		if (t1 < t2)
			high = mid - 1;
		else if (t1 > t2)
			low = mid + 1;
		else found = 1;
	}
	if (found)
	{
		pos = mid;
		memcpy(&pKLineArray[pos], (char *)pKLineSingle, ROM_KLINE_DATA_LEN);
		return nCount;
	}
	else
	{
		pos = low;                                       //插入位置是下标为pos的数组元素
		for (i = nCount - 1; i >= pos; i--)              //从下标为n-1~pos的元素依次往后移
			memcpy(&pKLineArray[i + 1], &pKLineArray[i], ROM_KLINE_DATA_LEN);
		memcpy(&pKLineArray[pos], (char *)pKLineSingle, ROM_KLINE_DATA_LEN);     //将pKLineSingle存入下标为pos的数组元素中
		return ++nCount;
	}
}

void DataExchange::update_minutely_kline(byte _exchange, char *_symbol, std::string _str_data_type, ROM_KLINE_DATA *_pp_new_kline, int _new_rec_count, int _total_symbols_count)
{
	if (_symbol == NULL || _str_data_type == "" || _pp_new_kline == NULL || _new_rec_count <= 0) return;

	int _total_rec_count = 0;
	int _old_data_len = ipfs_min_.read_big_content(_str_data_type,
		std::to_string((int)_exchange) + std::string(_symbol),
		swap_ex_.mem_, swap_ex_.len_);
	if (_old_data_len > 0)
	{
		ROM_KLINE_DATA *_pp_old_kline = (ROM_KLINE_DATA *)swap_ex_.mem_;
		_total_rec_count = _old_data_len / sizeof(ROM_KLINE_DATA);
		for (int x = 0; x < _new_rec_count; x++)
			_total_rec_count = insert_unique_sort_value2(_pp_old_kline, _total_rec_count, &_pp_new_kline[x]);
	}
	else
	{
		memcpy(swap_ex_.mem_, _pp_new_kline, _new_rec_count * sizeof(ROM_KLINE_DATA));
		_total_rec_count = _new_rec_count;
	}
	if (_total_rec_count > 0)
	{
		std::cout << _total_symbols_count << "," << _str_data_type << ":" << std::to_string(_exchange) << "," << _symbol << " OK!" << std::endl;
		ipfs_min_.write_big_content(_str_data_type,
			std::to_string((int)_exchange) + std::string(_symbol),
			swap_ex_.mem_, _total_rec_count*KLINE_DATA_SIZE);
	}
}
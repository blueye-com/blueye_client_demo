#pragma once

#include "stdafx.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <deque>
#include <map>
#include <stdlib.h>  
#include <stdio.h>  
#include <string>  
#include <sstream>  
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp>  
#include <boost/tokenizer.hpp>
#include <time.h>  
#include <locale>
#include <codecvt>
#include <fstream>
#include <direct.h>
#pragma push(pack,1)

/*
FIX_MSG_TYPE_INSTITUTION_REQUEST:
response:
FLD_ROWCOUNT,
->FIX_TAG_InstitutionId;
->FIX_TAG_InstitutionName;
->FIX_TAG_GatewayName;
->FIX_TAG_Ip;
->FIX_TAG_Port;
*/
#define FIX_MSG_TYPE_INSTITUTION_REQUEST	400

/*
FIX_MSG_TYPE_BUSINESS_DICT
response:
response:
FLD_ROWCOUNT,
FID_BROWINDEX
->FIX_TAG_DictName
->FIX_TAG_DictValue
*/
#define FIX_MSG_TYPE_BUSINESS_DICT			401

/*
FIX_MSG_TYPE_BUSINESS_MAP
request:
name_of_gateway
response:
FID_FILENAME;
FID_RawDataLength;
FID_RawData
*/
#define FIX_MSG_TYPE_BUSINESS_MAP			402

/*
FIX_MSG_TYPE_REQ_MY_ACC_LIST
request:
FIX_TAG_UserId
response:
FLD_ROWCOUNT,
FID_BROWINDEX
->FIX_TAG_AccountId
->FIX_TAG_AccountName
*/
#define FIX_MSG_TYPE_REQ_MY_ACC_LIST		403

/*
FIX_MSG_TYPE_SET_MY_ACC_LIST
request:
FIX_TAG_UserId
FLD_ROWCOUNT,
->FIX_TAG_AccountId
->FIX_TAG_AccountName
response:
error_code,error_msg
*/
#define FIX_MSG_TYPE_SET_MY_ACC_LIST		405


/*
FIX_MSG_TYPE_REQ_MY_SUB_ACC_LIST
request:
FIX_TAG_AccountId
response:
FLD_ROWCOUNT,
FID_BROWINDEX
->FIX_TAG_SubAccountId
->FIX_TAG_SubAccountName
*/
#define FIX_MSG_TYPE_REQ_MY_SUB_ACC_LIST	406

/*
FIX_MSG_TYPE_REQ_MY_SUB_ACC_LIST
request:
FIX_TAG_AccountId
FLD_ROWCOUNT,
FID_BROWINDEX
->FIX_TAG_SubAccountId
->FIX_TAG_SubAccountName
response:
error_code,error_msg
*/
#define FIX_MSG_TYPE_SET_MY_SUB_ACC_LIST	407

/*
FIX_MSG_TYPE_REQ_MY_JOBS
request:
FIX_TAG_AccountId
response:
FIX_TAG_AccountId
FLD_ROWCOUNT,
FID_BROWINDEX
->FIX_TAG_Exchange
->FIX_TAG_Symbol
->FIX_TAG_Pattern
->FIX_TAG_ParamFileName
->FIX_TAG_Period
->FIX_TAG_FloatAmplification
->FIX_TAG_SignalPushType
->FIX_TAG_GatewayType
->FIX_TAG_TriggerMethod
->FIX_TAG_JobStatus
->FIX_TAG_JobStatus
*/
#define FIX_MSG_TYPE_REQ_MY_JOBS			408

/*
FIX_MSG_TYPE_SET_MY_JOBS
request:
FIX_TAG_AccountId
FLD_ROWCOUNT,
FID_BROWINDEX
->FIX_TAG_Exchange
->FIX_TAG_Symbol
->FIX_TAG_Pattern
->FIX_TAG_ParamFileName
->FIX_TAG_Period
->FIX_TAG_FloatAmplification
->FIX_TAG_SignalPushType
->FIX_TAG_GatewayType
->FIX_TAG_TriggerMethod
->FIX_TAG_JobStatus
->FIX_TAG_JobStatus
response:
error_code,error_msg
*/
#define FIX_MSG_TYPE_SET_MY_JOBS			409

/*
FIX_MSG_TYPE_REQ_MY_TRANS
request:
FIX_TAG_AccountId
FIX_TAG_FileName
response:
FIX_TAG_FileName
RawDataLength		//If RawDataLength<=0 means not found
RawData
*/
#define FIX_MSG_TYPE_REQ_MY_TRANS			410

/*
FIX_MSG_TYPE_SET_MY_TRANS
request:
FIX_TAG_AccountId
FIX_TAG_FileName
RawDataLength
RawData
response:
error_code,error_msg
*/
#define FIX_MSG_TYPE_SET_MY_TRANS			411


#define FIX_MSG_TYPE_USER_TEXT				500
#define FIX_MSG_TYPE_USER_REQ_FILE			501
#define FIX_MSG_TYPE_USER_SET_FILE			502

/*
Big file transfer
Big file need to be transfered through multiple small rawdata with pos and length marked.
In order to save big file, previously every such a file will be saved as : file_name.idx, file_name.001,file_name.002,...
1,	file_name.idx will saved as
total_length
check_sum
->pos,temp file name (file_name.001)
->pos,size file name (file_name.001)
... ...
->pos,size file name (file_name.nnn)
2,To check if the full packages have been accomplished. If then, to save and delete the idx and temparory file
How to check the file completion,sum(length of file_name.nnn) = total_length,check sum is ok
*/
/*
FIX_MSG_TYPE_USER_REQ_BIG_FILE
request:
FIX_TAG_TransFileType
FIX_TAG_TransFileName
response:
FIX_TAG_TransFileType
FIX_TAG_TransFileName
FIX_TAG_TransFileCheckSum
FIX_TAG_TransFileTotalLength
FIX_TAG_TransFilePos
FIX_TAG_RawDataLength		//If RawDataLength<=0 means not found
FIX_TAG_RawData
*/
#define FIX_MSG_TYPE_USER_REQ_BIG_FILE			503

/*
FIX_MSG_TYPE_USER_SET_BIG_FILE
request:
FIX_TAG_TransFileType
FIX_TAG_TransFileName
FIX_TAG_TransFileCheckSum
FIX_TAG_TransFilePos
FIX_TAG_TransFileTotalLength
FIX_TAG_RawDataLength		//If RawDataLength<=0 means not found
FIX_TAG_RawData
response:
error_code,error_msg
*/
#define FIX_MSG_TYPE_USER_SET_BIG_FILE			504


typedef void(*p_fix_call_back)(int _thread_id, std::string _str_msg_type, std::string _str_json);
typedef void(*p_fix_log)(int _thread_id, std::string _str_log);

typedef int(*p_init_fix_api)(std::string _str_ip, std::string _str_port, std::string _str_business_map_name,HANDLE	_shutdown_event, p_fix_call_back _p_call_back, p_fix_log _log, std::string _str_token);

typedef bool(*p_is_login)(int _thread_id);

typedef bool(*p_is_released)(int _thread_id);

typedef void(*p_release_fix_api)(int _thread_id);

typedef bool (*p_send_business_ex)(int _thread_id, std::string _str_business_name, std::vector<std::map<std::string, std::string>> _map_src_data, int &_msg_seq_num);

typedef void (*p_download_file)(int _thread_id, std::string _str_file_type, std::string _str_file_name, int &_msg_seq_num);

typedef void (*p_upload_file)(int _thread_id, std::string _str_file_type, std::string _str_file_name, int &_msg_seq_num);

typedef void(*p_truncate_file)(int _thread_id, std::string _str_file_type, std::string _str_file_name, int &_msg_seq_num);

typedef void (*p_reload_router_list)(int _thread_id);
typedef void (*p_reload_all_map)(int _thread_id);

typedef std::map<std::string, std::map<std::string, std::string>> * (*p_get_institutions)(int _thread_id);

typedef std::string(*p_get_file_name)(int _thread_id, std::string _str_file_type, std::string _str_file_name);

typedef int(*p_get_queue_size)(int _thread_id);

typedef void(*p_clear_fix_msg)(int _thread_id);

typedef struct
{
	HINSTANCE			instance_;
	p_init_fix_api		_init_fix_api_;
	p_is_login			_is_login_;
	p_is_released		_is_released_;
	p_release_fix_api	_release_fix_api_;
	p_send_business_ex		_send_business_;
	p_download_file		_download_file_;
	p_upload_file		_upload_file_;
	p_truncate_file		_truncate_file_;
	p_reload_router_list _reload_router_list_;
	p_reload_all_map	_reload_all_map_;
	p_get_institutions	_get_institutions_;
	p_get_file_name		_get_file_name_;
	p_get_queue_size	_get_queue_size_;
	p_clear_fix_msg		_clear_fix_msg_;
	bool load(char *_file_name)
	{
		if (_file_name == NULL) return false;

		if (_file_name)
		{
			instance_ = LoadLibrary(_file_name);
			if (instance_)
			{
				_init_fix_api_ = (p_init_fix_api)GetProcAddress(instance_, "i_init_fix_api");
				_is_login_ = (p_is_login)GetProcAddress(instance_, "i_is_login");
				_is_released_ = (p_is_released)GetProcAddress(instance_, "i_is_released");
				_release_fix_api_ = (p_release_fix_api)GetProcAddress(instance_, "i_release_fix_api");
				_send_business_ = (p_send_business_ex)GetProcAddress(instance_, "i_send_business");
				_download_file_ = (p_download_file)GetProcAddress(instance_, "i_download_file");
				_upload_file_ = (p_upload_file)GetProcAddress(instance_, "i_upload_file");
				_truncate_file_ = (p_truncate_file)GetProcAddress(instance_, "i_truncate_file");
				_reload_router_list_ = (p_reload_router_list)GetProcAddress(instance_, "i_reload_router_list");
				_reload_all_map_ = (p_reload_all_map)GetProcAddress(instance_, "i_reload_all_map");
				_get_institutions_ = (p_get_institutions)GetProcAddress(instance_, "i_get_institutions");
				_get_file_name_ = (p_get_file_name)GetProcAddress(instance_, "i_get_file_name");
				_get_queue_size_ = (p_get_queue_size)GetProcAddress(instance_, "i_get_queue_size");
				_clear_fix_msg_ = (p_clear_fix_msg)GetProcAddress(instance_, "i_clear_fix_msg");

			}
			return (instance_ && _init_fix_api_ && _release_fix_api_ && _send_business_ && _download_file_ && _upload_file_ && _get_queue_size_) ? true:false;
		}
		else
		{
			memset(this, 0, sizeof(tag_BLUEYE_FIX_API));
			return false;
		}
	}
	void release()
	{
		if (instance_) FreeLibrary(instance_);
		memset(this, 0, sizeof(tag_BLUEYE_FIX_API));
	}
}tag_BLUEYE_FIX_API;

typedef struct
{
	static std::string utf8_to_gb2312(std::string const &strUtf8)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> cutf8;
		std::wstring wTemp = cutf8.from_bytes(strUtf8);
#ifdef _MSC_VER
		std::locale loc("zh-CN");
#else
		std::locale loc("zh_CN.GB18030");
#endif
		const wchar_t* pwszNext = nullptr;
		char* pszNext = nullptr;
		mbstate_t state = {};

		std::vector<char> buff(wTemp.size() * 2);
		int res = std::use_facet<std::codecvt<wchar_t, char, mbstate_t> >
			(loc).out(state,
				wTemp.data(), wTemp.data() + wTemp.size(), pwszNext,
				buff.data(), buff.data() + buff.size(), pszNext);

		if (std::codecvt_base::ok == res)
		{
			return std::string(buff.data(), pszNext);
		}
		return "";
	}

	static std::string gb2312_to_utf8(std::string const &strGb2312)
	{
		std::vector<wchar_t> buff(strGb2312.size());
#ifdef _MSC_VER
		std::locale loc("zh-CN");
#else
		std::locale loc("zh_CN.GB18030");
#endif
		wchar_t* pwszNext = nullptr;
		const char* pszNext = nullptr;
		mbstate_t state = {};
		int res = std::use_facet<std::codecvt<wchar_t, char, mbstate_t> >
			(loc).in(state,
				strGb2312.data(), strGb2312.data() + strGb2312.size(), pszNext,
				buff.data(), buff.data() + buff.size(), pwszNext);

		if (std::codecvt_base::ok == res)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> cutf8;
			return cutf8.to_bytes(std::wstring(buff.data(), pwszNext));
		}

		return "";

	}
	static std::string str_toupper(std::string s) {
		std::transform(s.begin(), s.end(), s.begin(),
			[](unsigned char c) { return std::toupper(c); }
		);
		return s;
	}

	static void read_grid_from_json(std::string _str_json_file, std::string _str_content_tag, std::vector<std::map<std::string, std::string>> &_grid)
	{
		if (_str_json_file == "") return;

		boost::property_tree::ptree root, data;

		//if file was not found, to return 
		FILE *_fp = fopen(_str_json_file.c_str(), "r+b");
		if (!_fp)
			return;
		else
			fclose(_fp);
		try { boost::property_tree::read_json<boost::property_tree::ptree>(_str_json_file, root); }
		catch (...) { return; }
		_grid.clear();

		for (boost::property_tree::ptree::iterator it = root.begin(); it != root.end(); it++)
		{
			std::cout << it->first << std::endl;
			if (it->first == _str_content_tag && it->second.data() == "")
			{
				data = it->second.get_child("");
				for (boost::property_tree::ptree::iterator itit = it->second.begin(); itit != it->second.end(); itit++)
				{
					if (itit->first != "")
					{
						std::string _str_key = utf8_to_gb2312(itit->first);
						std::string _str_value = utf8_to_gb2312(itit->second.get<std::string>(""));
						std::cout << "\t" << _str_key.c_str() << "," << _str_value.c_str() << std::endl;
						std::map<std::string, std::string> _one_rec;
						_one_rec.insert(std::make_pair(_str_key, _str_value));
					}
					else
					{
						std::map<std::string, std::string> _one_rec;
						for (boost::property_tree::ptree::iterator ititit = itit->second.begin(); ititit != itit->second.end(); ititit++)
						{
							if (ititit->first != "")
							{
								std::string _str_key = utf8_to_gb2312(ititit->first);
								std::string _str_value = utf8_to_gb2312(ititit->second.get<std::string>(""));
								std::cout << "\t" << _str_key.c_str() << "," << _str_value.c_str() << std::endl;
								_one_rec.insert(std::make_pair(_str_key, _str_value));
							}
						}
						std::cout << "*************" << std::endl;
						_grid.push_back(_one_rec);
					}
				}
			}
		}
	}

	static void write_json_to_file(std::string _str_file, std::string _str_json)
	{
		FILE *fp = fopen(_str_file.c_str(), "w+b");
		if (fp)
		{
			fwrite(_str_json.c_str(), _str_json.length(), 1, fp);
			fclose(fp);
		}
	}
	static void json_to_map(std::string _str_msg_type,std::string _str_msg_content, std::vector<std::map<std::string, std::string>> &_grid)
	{
		int _tick = GetTickCount();
		std::string _str_json_file = std::to_string(_tick) + ".json";
		write_json_to_file(_str_json_file, _str_msg_content);
		read_grid_from_json(_str_json_file, _str_msg_type, _grid);
		/*if (_grid.size() > 0)
		{
			std::string _str_tttt = "";
			for (int i = 0; i<_grid.size(); i++)
			{
				for (std::map<std::string, std::string>::iterator it = _grid[i].begin(); it != _grid[i].end(); it++)
				{
					_str_tttt += it->first + "," + it->second + ";";
				}
			}
		}*/
		remove(_str_json_file.c_str());
	}
}tag_BLUEYE_FIX_API_PARSER;

#pragma push(pop)
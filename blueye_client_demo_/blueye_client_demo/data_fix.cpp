#include "stdafx.h"
#include "data_fix.h"

// ..\..\..\server\blueye_algo\blueye_algo\blueye_fix_api.dll
//..\..\..\server\blueye_algo\blueye_algo\tactics.dll


std::map<int, tag_DATA_FIX>						g_map_fix_instance;
tag_BLUEYE_FIX_API								*g_p_fix_api_;

extern CString GetModuleFilePath(HMODULE hModule);

data_fix::data_fix()
{
	if (fix_api_.load((char *)"blueye_fix_api.dll"))
	{
		log_to_file((char *)"load blueye_fix_api.dll successfully.", 1, true);
		shutdown_event_ = CreateEvent(NULL, TRUE, FALSE, "");
		g_p_fix_api_ = &fix_api_;
	}
	else
	{
		log_to_file((char *)"failed load blueye_fix_api.dll.", 1, true);
		g_p_fix_api_ = NULL;
	}
}


data_fix::~data_fix()
{
	fix_api_.release();
	g_p_fix_api_ = NULL;
}

int data_fix::create_fix(std::string _str_ip, std::string _str_port, std::string _str_institution, p_fix_call_back	_fix_call_back_, p_fix_log _fix_log)
{
	int _thread_id = fix_api_._init_fix_api_(_str_ip, _str_port, _str_institution, shutdown_event_, data_fix::fix_call_back, data_fix::fix_log, "blueye");
	//int _thread_id = fix_api_._init_fix_api_("127.0.0.1", "6001", _str_institution, shutdown_event_, data_fix::fix_call_back, data_fix::fix_log, "blueye");
	if (_thread_id > 0)
	{
		tag_DATA_FIX _data_fix;
		_data_fix.thread_id_ = _thread_id;
		_data_fix.fix_msg_seq_num_ = 1;
		_data_fix.fix_call_back_ = _fix_call_back_;
		_data_fix.fix_log_ = _fix_log;
		time(&_data_fix.last_downloading_time_);
		g_map_fix_instance.insert(std::make_pair(_thread_id, _data_fix));
	}
	return _thread_id;
}

std::map<int, tag_DATA_FIX> * data_fix::get_fix_instance()
{
	return &g_map_fix_instance;
}
void data_fix::fix_call_back(int _thread_id, std::string _str_msg_type, std::string _str_json)
{
	std::string _str_callback_content = "";
	if (_str_msg_type == std::to_string(FIX_MSG_TYPE_USER_REQ_FILE) || _str_msg_type == std::to_string(FIX_MSG_TYPE_USER_REQ_BIG_FILE))
	{
		if (g_map_fix_instance.count(_thread_id) > 0 )
		{
			if(g_map_fix_instance[_thread_id].fix_call_back_)
				g_map_fix_instance[_thread_id].fix_call_back_(_thread_id, _str_msg_type, _str_json);
			time(&g_map_fix_instance[_thread_id].last_downloading_time_);
			g_map_fix_instance[_thread_id].map_files_downloading_.erase(_str_json);
		}
		_str_callback_content = _str_json;
	}
	
	//std::string _str_log_msg = "FIX call back: thread id=" + std::to_string(_thread_id) + ",msg_type=" + _str_msg_type + ",msg_content=" + _str_json;
	//data_fix::log_to_file((char *)_str_log_msg.c_str(), 1, true);
}
void data_fix::fix_log(int _thread_id, std::string _str_log)
{
	if (g_p_fix_api_ && g_p_fix_api_->_is_login_(_thread_id) && g_map_fix_instance.count(_thread_id) > 0 )
	{
		//data_fix::log_to_file((char *)"Fix login successfully", 1, true);
		g_map_fix_instance[_thread_id].fix_log_(_thread_id, _str_log);
	}
	//std::string _str_log_msg = "FIX log: thread id=" + std::to_string(_thread_id) + ",log=" + _str_log ;
	//data_fix::log_to_file((char *)_str_log_msg.c_str(), 1, true);
}


void data_fix::log_to_file(char *_s_msg, int _log_level, bool _echo)
{
	try
	{
		time_t t;
		time(&t);
		struct tm * ctm = localtime(&t);
		char _s_time[MAX_PATH], _log_file_name[MAX_PATH];
		memset(_s_time, 0, sizeof(_s_time));
		sprintf(_s_time, "[%d %02d:%02d:%02d] ",
			(ctm->tm_year + 1900) * 10000 + (ctm->tm_mon + 1) * 100 + ctm->tm_mday,
			ctm->tm_hour, ctm->tm_min, ctm->tm_sec);
		std::string _str_log = std::string(_s_time) + std::string(_s_msg);

		sprintf(_log_file_name, "algo_fix_log%d_%d.txt", _log_level, (ctm->tm_year + 1900) * 10000 + (ctm->tm_mon + 1) * 100 + ctm->tm_mday);
		FILE *_fp = fopen(_log_file_name, "a+");
		if (_fp)
		{
			fprintf(_fp, "%s\r\n", _str_log.c_str());
			fclose(_fp);
		}
		if (_echo)
			std::cout << _str_log << std::endl;
	}
	catch (...)
	{
		;
	}
}
void data_fix::push_file_download_queue(int _thread_id,std::string _str_file_type, std::string _str_file_name)
{
	if (g_map_fix_instance.count(_thread_id) > 0)
	{
		tag_DOWNLOAD_FILE_REC _file_rec;
		_file_rec.str_file_name_ = _str_file_name;
		_file_rec.str_file_type_ = _str_file_type;
		_file_rec.retry_times_ = 0;
		g_map_fix_instance[_thread_id].deque_files_need_downloaded_.push_back(_file_rec);
		//std::cout << _str_file_name.c_str() << "," << std::endl;
	}
}

void data_fix::download_files(int _thread_id)
{
	if (g_map_fix_instance.count(_thread_id) > 0 && fix_api_._is_login_(_thread_id) && g_map_fix_instance[_thread_id].deque_files_need_downloaded_.size() > 0)
	{
		std::string _str_echo_msg;
		int _downloading_count = 0;
		//for (std::map<std::string, tag_DOWNLOAD_FILE_REC>::iterator it = g_map_fix_instance[_thread_id].map_files_need_downloaded_.begin(); it != g_map_fix_instance[_thread_id].map_files_need_downloaded_.end(); it++)
		for (int x = 0; x<g_map_fix_instance[_thread_id].deque_files_need_downloaded_.size(); x++)
		{
			if (map_black_list_.count(g_map_fix_instance[_thread_id].deque_files_need_downloaded_[x].str_file_name_) <= 0 &&
				map_black_list2_.count(g_map_fix_instance[_thread_id].deque_files_need_downloaded_[x].str_file_name_) <= 0)
			{
				fix_api_._download_file_(_thread_id, g_map_fix_instance[_thread_id].deque_files_need_downloaded_[x].str_file_type_,
					g_map_fix_instance[_thread_id].deque_files_need_downloaded_[x].str_file_name_, g_map_fix_instance[_thread_id].fix_msg_seq_num_);
				g_map_fix_instance[_thread_id].map_files_downloading_.insert(std::make_pair(g_map_fix_instance[_thread_id].deque_files_need_downloaded_[x].str_file_name_, g_map_fix_instance[_thread_id].deque_files_need_downloaded_[x]));
				g_map_fix_instance[_thread_id].fix_msg_seq_num_++;
			}
			//_str_echo_msg = "下载文件,类型:" + it->second.str_file_type_ + "名称:" + it->second.str_file_name_;
			//std::cout << _str_echo_msg.c_str() << std::endl;
		}
		g_map_fix_instance[_thread_id].deque_files_need_downloaded_.clear();
	}
}


void data_fix::download_file(int _thread_id, std::string _str_file_type, std::string _str_file_name)
{
	fix_api_._download_file_(_thread_id, _str_file_type, _str_file_name, g_map_fix_instance[_thread_id].fix_msg_seq_num_);
	g_map_fix_instance[_thread_id].fix_msg_seq_num_++;
}

void data_fix::upload_file(int _thread_id, std::string _str_file_type, std::string _str_file_name)
{
	fix_api_._upload_file_(_thread_id, _str_file_type, _str_file_name, g_map_fix_instance[_thread_id].fix_msg_seq_num_);
	g_map_fix_instance[_thread_id].fix_msg_seq_num_++;
}
void data_fix::write_black_list(std::map<std::string, std::string> *_p_map_black_list, std::string _str_file_name)
{
	if (_p_map_black_list == NULL || _p_map_black_list->size() == 0)
		return;
	CString strPath = GetModuleFilePath(NULL);
	std::string _root_path_ = std::string(strPath) + "\\fix_data\\algo\\" + _str_file_name;
	FILE *_fp = fopen(_root_path_.c_str(), "w+b");
	if (_fp)
	{
		for (std::map<std::string, std::string>::iterator it = _p_map_black_list->begin(); it != _p_map_black_list->end(); it++)
		{
			fprintf(_fp, "%s,%s\r\n", it->first.c_str(), it->second.c_str());
		}
		fclose(_fp);
	}
}

void data_fix::read_black_list(std::map<std::string, std::string> *_p_map_black_list, std::string _str_file_name, bool _b_time_filter)
{
	CString strPath = GetModuleFilePath(NULL);
	std::string _root_path_ = std::string(strPath) + "\\fix_data\\algo\\" + _str_file_name;

	if (_b_time_filter)
	{
		struct stat _buf;
		stat(_root_path_.c_str(), &_buf);
		time_t _t_cur;
		time(&_t_cur);
		if ((_t_cur - _buf.st_mtime) > BLACK_LIST_FILE_EXPIRE)
		{
			remove(_root_path_.c_str());
			return;
		}
	}

	FILE *_fp = fopen(_root_path_.c_str(), "r+b");
	if (_fp && _p_map_black_list)
	{
		char _line[MAX_PATH];

		_p_map_black_list->clear();
		while (!feof(_fp))
		{
			memset(_line, 0, sizeof(_line));
			fgets(_line, MAX_PATH, _fp);
			std::string _str_line = std::string(_line);
			size_t _pos = _str_line.find(",");
			if (_pos != std::string::npos && _str_line.length()>(_pos + 1))
			{
				_p_map_black_list->insert(std::make_pair(_str_line.substr(0, _pos), _str_line.substr(_pos + 1, _str_line.length() - _pos)));
			}
		}

		fclose(_fp);
	}
}

void data_fix::clear_msg(int _thread_id)
{
	fix_api_._clear_fix_msg_(_thread_id);
}

void data_fix::truncate_file(int _thread_id, std::string _str_file_type, std::string _str_file_name)
{
	fix_api_._truncate_file_(_thread_id, _str_file_type, _str_file_name, g_map_fix_instance[_thread_id].fix_msg_seq_num_);
	g_map_fix_instance[_thread_id].fix_msg_seq_num_++;
}
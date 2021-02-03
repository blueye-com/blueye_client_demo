#pragma once
#include "blueye_fix_api.h"

#pragma push(pack,1)

typedef struct
{
	std::string str_msg_type_;
	std::string str_msg_content_;
}tag_CALL_BACK_MSG;

typedef struct
{
	int												thread_id_;
	tag_CALL_BACK_MSG								callback_msgs_;
	std::string										log_msgs_;
}tag_CALL_BACK_DATA;

typedef struct
{
	std::string					str_file_type_;
	std::string					str_file_name_;
	short						retry_times_;
	enum {MAX_RETRY_TIME=6};
}tag_DOWNLOAD_FILE_REC;

//typedef void (*p_fix_call_back)(int _thread_id, std::string _str_msg_type, std::string _str_json);
//typedef void (*p_fix_log)(int _thread_id, std::string _str_log);

typedef struct
{
	int												thread_id_;
	int												fix_msg_seq_num_;
	p_fix_call_back									fix_call_back_;
	p_fix_log										fix_log_;
	std::deque<tag_DOWNLOAD_FILE_REC>				deque_files_need_downloaded_;
	std::map<std::string, tag_DOWNLOAD_FILE_REC>	map_files_downloading_;
	time_t											last_downloading_time_;
}tag_DATA_FIX;


#define MAX_QUEUE_FILE_SIZE		10
#define BLACK_LIST_FILE_EXPIRE	36000

class data_fix
{
protected:
	tag_BLUEYE_FIX_API								fix_api_;
	
protected:
	
public:
	HANDLE											shutdown_event_;
	std::map<std::string, std::string>				map_black_list_;
	std::map<std::string, std::string>				map_black_list2_;

public:
	data_fix();
	~data_fix();


	int create_fix(std::string _str_ip, std::string _str_port, std::string _str_institution, p_fix_call_back _fix_call_back_, p_fix_log _fix_log);
	std::map<int, tag_DATA_FIX> * get_fix_instance();
	bool is_login(int _thread_id) { return fix_api_._is_login_(_thread_id); }
	void push_file_download_queue(int _thread_id,std::string _str_file_type, std::string _str_file_name);
	void download_files(int _thread_id);
	void download_file(int _thread_id, std::string _str_file_type, std::string _str_file_name);
	void upload_file(int _thread_id, std::string _str_file_type, std::string _str_file_name);
	void clear_msg(int _thread_id);
	void truncate_file(int _thread_id, std::string _str_file_type, std::string _str_file_name);
	int get_queue_size(int _thread_id)
	{
		if (fix_api_._get_queue_size_)
			return fix_api_._get_queue_size_(_thread_id);
		else
			return 0;
	}

	void write_black_list(std::map<std::string, std::string> *_p_map_black_list, std::string _str_file_name);
	void read_black_list(std::map<std::string, std::string> *_p_map_black_list, std::string _str_file_name, bool _b_time_filter);

	static void log_to_file(char *_s_msg, int _log_level, bool _echo);
	static void fix_call_back(int _thread_id, std::string _str_msg_type, std::string _str_json);
	static void fix_log(int _thread_id, std::string _str_log);

};

#pragma pop(pack)
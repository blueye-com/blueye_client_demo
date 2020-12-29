#pragma once
#include "CBlueyeQuote.h"
#include "blueye_ipfs.h"
#include "data_fix.h"


typedef struct
{
	std::string str_msg_type_;
	std::string str_msg_content_;
}tag_FIX_CALL_BACK_MSG;

typedef struct
{
	int												thread_id_;
	tag_FIX_CALL_BACK_MSG							callback_msgs_;
	std::string										log_msgs_;
}tag_FIX_CALL_BACK_DATA;

class CBlueyeQuoteFile
{
public:
	static void my_fix_call_back(int _thread_id, std::string _str_msg_type, std::string _str_json);
	static void my_fix_log(int _thread_id, std::string _str_log);
	int start_fix(std::string _str_ip, std::string _str_port, HWND _hwnd, int _msg);
	void stop_fix();
	bool is_fix_login(int _thread_id);
public:
	std::string														root_path_;
	int																fix_thread_id_;
	HWND															h_wnd_notification_;
	int																msg_;
};

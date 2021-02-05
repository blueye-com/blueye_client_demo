#include "CBlueyeQuoteFile.h"
#include "ZipData.h"

CBlueyeQuoteFile *p_this_instance_ = new CBlueyeQuoteFile;
data_fix	g_algo_data_fix_;
blueye_ipfs g_algo_blueye_ipfs_;

int CBlueyeQuoteFile::start_fix(std::string _str_ip, std::string _str_port, HWND _hwnd, int _msg)
{
	int _thread_id = g_algo_data_fix_.create_fix(_str_ip, _str_port, "9999", CBlueyeQuoteFile::my_fix_call_back, CBlueyeQuoteFile::my_fix_log);
	p_this_instance_ = this;
	p_this_instance_->h_wnd_notification_ = _hwnd;
	p_this_instance_->msg_ = _msg;
	return _thread_id;
}

void CBlueyeQuoteFile::stop_fix()
{
	SetEvent(g_algo_data_fix_.shutdown_event_);
	Sleep(1000);
}

bool CBlueyeQuoteFile::is_fix_login(int _thread_id)
{
	return g_algo_data_fix_.is_login(_thread_id);
}

void CBlueyeQuoteFile::my_fix_call_back(int _thread_id, std::string _str_msg_type, std::string _str_json)
{
	//std::string _str_callback_content = "";
	CZipData _zip;
	_zip.do_unzip_and_remove((char *)g_algo_blueye_ipfs_.get_content_path_by_type(_str_json).c_str(), (char *)"zip");
	if (p_this_instance_ && _str_msg_type == std::to_string(FIX_MSG_TYPE_USER_REQ_FILE) || _str_msg_type == std::to_string(FIX_MSG_TYPE_USER_REQ_BIG_FILE))
	{
		if ( _str_json.find("w1m_data_by_symbol") != std::string::npos)
		{
			
		}
		else if ( _str_json.find("w3m_data_by_symbol") != std::string::npos)
		{
			
		}
		else if ( _str_json.find("w5m_data_by_symbol") != std::string::npos)
		{

		}
		else if ( _str_json.find("w10m_data_by_symbol") != std::string::npos)
		{

		}
		else if ( _str_json.find("w15m_data_by_symbol") != std::string::npos)
		{

		}
		else if ( _str_json.find("w30m_data_by_symbol") != std::string::npos)
		{

		}
		else if ( _str_json.find("w60m_data_by_symbol") != std::string::npos)
		{

		}
		else if ( _str_json.find("day_data_by_symbol") != std::string::npos)
		{

		}
		else if ( _str_json.find("week_data_by_symbol") != std::string::npos)
		{

		}
		else if ( _str_json.find("rom_share") != std::string::npos)
		{

		}
		else if ( _str_json.find("finance") != std::string::npos)
		{

		}
		if (p_this_instance_->h_wnd_notification_ && p_this_instance_->msg_ > 0)
		{ }
		/*if (p_this_instance_->str_sys_acc_id_ != "" && _str_json.find(p_this_instance_->str_sys_algo_list_downloading_) != std::string::npos)
		{
			p_this_instance_->async_download_algo_related_files(p_this_instance_->str_sys_acc_id_);
		}*/
	}

	/*tag_CALL_BACK_DATA _callback_data;
	_callback_data.thread_id_ = _thread_id;
	_callback_data.callback_msgs_.str_msg_type_ = _str_msg_type;
	_callback_data.callback_msgs_.str_msg_content_ = _str_json;
	g_fix_callback_data.push_back(_callback_data);*/

}
void CBlueyeQuoteFile::my_fix_log(int _thread_id, std::string _str_log)
{
	/*tag_CALL_BACK_DATA _callback_data;
	_callback_data.thread_id_ = _thread_id;
	_callback_data.log_msgs_ = _str_log;
	g_fix_callback_data.push_back(_callback_data);*/
	/*if (p_this_instance_->is_fix_login(_thread_id) && p_this_instance_->h_wnd_notification_)
	{
		::PostMessage(p_this_instance_->h_wnd_notification_, p_this_instance_->msg_, MSG_FIX_LOGIN_OK, NULL);
	}
	std::cout << _str_log.c_str() << std::endl;*/
}
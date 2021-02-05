#include "stdafx.h"
#include <direct.h>
#include <boost/filesystem.hpp>
#include <cctype>
#include <clocale>

#include "blueye_ipfs.h"
#include "json_util.hpp"
#include "blueye_client_common.h"

#define SYMBOL_LEN 6

//extern void log_to_file(char *_s_msg, int _log_level, bool _echo);

extern CString GetModuleFilePath(HMODULE hModule);


BOOL CreateMultiDir(TCHAR* szPathDir)
{
	TCHAR szPath[MAX_PATH] = { 0 };
	TCHAR szbuf[MAX_PATH] = { 0 };
	BOOL bFlag = FALSE;

	_tcscpy(szPath, szPathDir);
	int nLen = _tcslen(szPath);
	if (nLen <= 3)
		return FALSE;
	if (szPath[nLen - 1] != _T('\\'))
	{
		_tcscat(szPath, _T("\\"));
		szPath[nLen + 1] = _T('\0');
		nLen++;
	}
	for (int i = 0; i < nLen; i++)
	{
		if (szPath[i] == _T('\\'))
		{
			if (bFlag == TRUE)
			{
				if (!PathFileExists(szbuf))
				{
					if (!CreateDirectory(szbuf, NULL))
						return FALSE;
				}
			}
			szbuf[i] = szPath[i];
			bFlag = TRUE;
		}
		else
		{
			szbuf[i] = szPath[i];
		}
	}
	return TRUE;
}

// 核查目录，若目录不存在，创建目录
bool FindOrCreateDirectory(const char* pszPath)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(pszPath, &fd);
	while (hFind != INVALID_HANDLE_VALUE)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		return true;
	}
	if (!CreateMultiDir((char*)pszPath))
	{
		std::cout << pszPath << std::endl;
		std::cout << "创建目录失败，请检查权限！" << std::endl;
		return false;
	}
	return true;
}

blueye_ipfs::blueye_ipfs()
{
	char strPath[MAX_PATH];
	_getcwd(strPath, MAX_PATH);
	file_root_path_ = std::string(strPath) + "\\fix_data\\";
	box_file_root_path_ = std::string(strPath) + "\\box_data\\";
	if (!boost::filesystem::is_directory(file_root_path_))
		boost::filesystem::create_directories(file_root_path_);
	if (!boost::filesystem::is_directory(box_file_root_path_))
		boost::filesystem::create_directories(box_file_root_path_);

	byte_ipfs_chip_size_ = 10240;
}


blueye_ipfs::~blueye_ipfs()
{
}

std::string blueye_ipfs::gen_hash_key_for_content(std::string _str_content_type, std::string _str_content_name)
{
	return _str_content_type + _str_content_name;
}
void blueye_ipfs::gen_file_list(std::string _str_content_type, std::string _str_content_name, char *_p_content, int _content_len, std::vector<tag_FILE_INFO> &_file_list)
{
	if (_str_content_type == "" || _str_content_name == "" || _p_content == NULL || _content_len <= 0 ) return;

	_file_list.clear();
	std::string _str_content_type_path = file_root_path_ + "\\" + _str_content_type+"\\";
	if (!boost::filesystem::is_directory(_str_content_type_path))
		boost::filesystem::create_directories(_str_content_type_path);

	if (_content_len < byte_ipfs_chip_size_)
	{
		std::string _str_full_file_name = _str_content_type_path + _str_content_type + _str_content_name+".dat";
		FILE *fp = fopen(_str_full_file_name.c_str(), "w+b");
		if (fp)
		{
			fwrite(_p_content, _content_len, sizeof(char), fp);
			fclose(fp);
			tag_FILE_INFO _file_info;
			_file_info.str_file_name_ = _str_content_type + _str_content_name + ".dat";
			_file_info.file_len_ = _content_len;
			time(&_file_info.tm_file_update_);
			_file_list.push_back(_file_info);
		}
		else
		{
			str_error_msg_ = "Error in create file[" + _str_full_file_name + "]";
			//log_to_file((char *)str_error_msg_.c_str(), 1, false);
		}
	}
	else
	{
		std::string _str_full_file_name_old = _str_content_type_path + _str_content_type + _str_content_name + ".dat";
		remove(_str_full_file_name_old.c_str());
		int _file_split_count = _content_len / byte_ipfs_chip_size_;
		int _file_rest_size = _content_len % byte_ipfs_chip_size_;

		for (int i = 0; i < _file_split_count; i++)
		{
			std::string _str_full_file_name = _str_content_type_path + _str_content_type + _str_content_name + std::to_string(i) + ".dat";
			FILE *fp = fopen(_str_full_file_name.c_str(), "w+b");
			if (fp)
			{
				fwrite(_p_content + i * byte_ipfs_chip_size_, byte_ipfs_chip_size_, sizeof(char), fp);
				fclose(fp);
				tag_FILE_INFO _file_info;
				_file_info.str_file_name_ = _str_content_type + _str_content_name + std::to_string(i) + ".dat";
				_file_info.file_len_ = byte_ipfs_chip_size_;
				time(&_file_info.tm_file_update_);
				_file_list.push_back(_file_info);
			}
			else
			{
				str_error_msg_ = "Error in create file[" + _str_full_file_name + "]";
				//log_to_file((char *)str_error_msg_.c_str(), 1, false);
			}
		}
		if (_file_rest_size > 0)
		{
			std::string _str_full_file_name = _str_content_type_path + _str_content_type + _str_content_name + std::to_string(_file_split_count) + ".dat";
			FILE *fp = fopen(_str_full_file_name.c_str(), "w+b");
			if (fp)
			{
				fwrite(_p_content + _file_split_count * byte_ipfs_chip_size_, _file_rest_size, sizeof(char), fp);
				fclose(fp);
				tag_FILE_INFO _file_info;
				_file_info.str_file_name_ = _str_content_type + _str_content_name + std::to_string(_file_split_count) + ".dat";
				_file_info.file_len_ = _file_rest_size;
				time(&_file_info.tm_file_update_);
				_file_list.push_back(_file_info);
			}
			else
			{
				str_error_msg_ = "Error in create file[" + _str_full_file_name + "]";
				//log_to_file((char *)str_error_msg_.c_str(), 1, false);
			}
		}
	}
}
void blueye_ipfs::write_content( std::string _str_content_type, std::string _str_content_name, char *_p_content, int _content_len)
{
	if (_str_content_type == "" || _str_content_name == "") return;

	std::string _str_key = gen_hash_key_for_content(_str_content_type, _str_content_name);
	tag_CONTENT_INDEX _content_index;
	_content_index.str_name_ = _str_content_name;
	_content_index.str_type_ = _str_content_type;
	_content_index.str_node_ = str_cur_node_;
	//_content_index.str_crc_ = check_sum(_p_content, _content_len);

	gen_file_list(_str_content_type,_str_content_name,_p_content,_content_len, _content_index.file_list_);
	if (_content_index.file_list_.size() > 0)
	{
		if (map_content_index_.count(_str_key) > 0)
			map_content_index_.erase(_str_key);
		map_content_index_.insert(std::make_pair(_str_key, _content_index));	
	}
	
}
void blueye_ipfs::write_big_content(std::string _str_content_type, std::string _str_content_name, char *_p_content, int _content_len)
{
	if (_str_content_type == "" || _str_content_name == "" || _p_content == NULL || _content_len <=0) return;

	std::string _str_content_type_path = file_root_path_ + "\\" + _str_content_type + "\\";
	if (!boost::filesystem::is_directory(_str_content_type_path))
		boost::filesystem::create_directories(_str_content_type_path);
	std::string _str_full_file_name = _str_content_type_path + _str_content_type + _str_content_name + ".dat";
	FILE *fp = fopen(_str_full_file_name.c_str(), "w+b");
	if (fp)
	{
		fwrite(_p_content, _content_len, sizeof(char), fp);
		fclose(fp);
	}
	else
	{
		str_error_msg_ = "Error in create file[" + _str_full_file_name + "]";
		//log_to_file((char *)str_error_msg_.c_str(), 1, false);
	}
}
std::string blueye_ipfs::check_sum(char *_p_data, int _data_len)
{
	//to check sum
	char sCheckSum[3 + 1];
	unsigned int cks = 0;
	memset(sCheckSum, 0, sizeof(sCheckSum));
	for (int idx = 0; idx < _data_len; idx++)
	{
		cks += (unsigned int)_p_data[idx];
		sprintf(sCheckSum, "%03d", (unsigned int)(cks % 256));
	}
	//end of check sum
	return std::string(sCheckSum);
}

std::string blueye_ipfs::gen_json_from_map_content_index(std::string _str_type)
{
	std::map<std::string, tag_CONTENT_INDEX> _my_index;
	for (std::map<std::string, tag_CONTENT_INDEX>::iterator it = map_content_index_.begin(); it != map_content_index_.end();it++)
	{
		if (it->second.str_type_ == _str_type)
			_my_index.insert(std::make_pair(it->first,it->second));
	}

	std::string _str_json = "";

	_str_json += "{\r\n";
	//for (std::map<std::string, tag_CONTENT_INDEX>::iterator it = map_content_index_.begin(); it != map_content_index_.end();)
	for (std::map<std::string, tag_CONTENT_INDEX>::iterator it = _my_index.begin(); it != _my_index.end();)
	{
		if (it->second.str_type_ == _str_type)
		{
			_str_json += ("\t\"" + it->first + "\":\r\n");
			_str_json += ("\t{\r\n");
			_str_json += ("\t\t\"name\":\"" + it->second.str_name_ + "\",\r\n");
			_str_json += ("\t\t\"type\":\"" + it->second.str_type_ + "\",\r\n");
			_str_json += ("\t\t\"node\":\"" + it->second.str_node_ + "\",\r\n");
			_str_json += ("\t\t\"crc\":\"" + it->second.str_crc_ + "\",\r\n");
			_str_json += ("\t\t\"file_list\":");
			_str_json += ("[");
			for (int x = 0; x < it->second.file_list_.size();)
			{
				_str_json += ("\"");
				_str_json += (it->second.file_list_[x].str_file_name_ + ","
					+ std::to_string(it->second.file_list_[x].file_len_) + ","
					+ std::to_string(it->second.file_list_[x].tm_file_update_) );
				_str_json += "\"";
				x++;
				if (x < it->second.file_list_.size())
					_str_json += ",";
			}
			_str_json += "]\r\n";
			it++;
			if (it != _my_index.end())
				_str_json += "\t},\r\n";
			else
				_str_json += "\t}\r\n";
		}
		else
			it++;
	}
	_str_json += "}\r\n";
	_my_index.clear();
	return _str_json;
}

int blueye_ipfs::get_content_from_file_list(std::string _str_content_type, std::string _str_content_name, char *_p_content, int _content_capacity, std::vector<tag_FILE_INFO>	&_file_list)
{
	if (_str_content_type == "" || _str_content_name == "" || _p_content == NULL || _content_capacity <= 0)
	{
		str_error_msg_ = "Failed in get_content_from_file_list, invalid parameters.";
		//log_to_file((char *)str_error_msg_.c_str(), 1, false);
		return 0;
	}
	int _total_length = 0;
	for (int x = 0; x < _file_list.size(); x++)
	{
		std::string _str_content_type_path = file_root_path_ + "\\" + _str_content_type + "\\";
		std::string _str_full_file_name = _str_content_type_path + _file_list[x].str_file_name_;
		FILE *_fp = fopen(_str_full_file_name.c_str(), "r+b");
		if (_fp)
		{
			fseek(_fp, 0l, SEEK_END);
			int _file_len = ftell(_fp);
			if (_file_len == _file_list[x].file_len_)
			{
				fseek(_fp, 0l, SEEK_SET);
				
				if( (_total_length+_file_len) <= _content_capacity)
				{
					fread(_p_content + _total_length , 1, _file_len, _fp);
					fclose(_fp);
					_total_length += _file_len;
				}
				else
				{
					str_error_msg_ = "Failed in get_content_from_file_list, insufficient memory for read content";
					//log_to_file((char *)str_error_msg_.c_str(), 1, false);
					fclose(_fp);
					return 0;
				}
			}
			else
			{
				str_error_msg_ = "Failed in get_content_from_file_list,the length of file[" + _str_full_file_name + "] does not match the length in json-index, file conversion failed.";
				//log_to_file((char *)str_error_msg_.c_str(), 1, false);
				fclose(_fp);
				return 0;
			}
		}
		else
		{
			str_error_msg_ = "Failed in get_content_from_file_list,file not found ["+_str_full_file_name+"]";
			//log_to_file((char *)str_error_msg_.c_str(), 1, false);
			return 0;
		}
	}
	return _total_length;
}
int blueye_ipfs::read_content(std::string _str_content_type, std::string _str_content_name, char *_p_content, int _content_capacity)
{
	if (_str_content_type == "" || _str_content_name == "") return 0 ;

	std::string _str_key = gen_hash_key_for_content(_str_content_type, _str_content_name);
	if (map_content_index_.count(_str_key) > 0)
	{
		return get_content_from_file_list(_str_content_type, _str_content_name, _p_content, _content_capacity, map_content_index_[_str_key].file_list_);
		/*if (check_sum(_p_content, _content_length) == map_content_index_[_str_key].str_crc_)
		{
			return _content_length;
		}
		else
		{
			str_error_msg_ = "Failed in get_content_from_file_list,CRC error";
			log_to_file((char *)str_error_msg_.c_str(), 1, false);
			return 0;
		}*/
	}
	else
		return 0;
}

int blueye_ipfs::read_big_content(std::string _str_content_type, std::string _str_content_name, char *_p_content, int _content_capacity)
{
	if (_str_content_type == "" || _str_content_name == "") return 0;

	std::string _str_content_type_path = file_root_path_ + _str_content_type + "\\";
	std::string _str_full_file_name = _str_content_type_path + _str_content_type +_str_content_name+".dat";
	FILE *_fp = fopen(_str_full_file_name.c_str(), "r+b");
	if (_fp)
	{
		fseek(_fp, 0l, SEEK_END);
		int _file_len = ftell(_fp);
		if (_file_len > 0 && _file_len <= _content_capacity)
		{
			fseek(_fp, 0l, SEEK_SET);
			fread(_p_content, 1, _file_len, _fp);
			fclose(_fp);
			return _file_len;
		}
		else
		{
			str_error_msg_ = "Failed in read_big_content,the length of file[" + _str_full_file_name + "] is invalid:"+std::to_string(_file_len);
			//log_to_file((char *)str_error_msg_.c_str(), 1, false);
			fclose(_fp);
			return 0;
		}
	}
	else
	{
		str_error_msg_ = "Failed in get_content_from_file_list,file not found [" + _str_full_file_name + "]";
		//log_to_file((char *)str_error_msg_.c_str(), 1, false);
		return 0;
	}
}

bool blueye_ipfs::is_big_content_exists(std::string _str_content_type, std::string _str_content_name)
{
	std::string _str_content_type_path = file_root_path_  + _str_content_type + "\\";
	std::string _str_full_file_name = _str_content_type_path + _str_content_type + _str_content_name + ".dat";
	FILE *_fp = fopen(_str_full_file_name.c_str(), "r+b");
	if (_fp)
	{
		fclose(_fp);
		return true;
	}
	else
	{
		return false;
	}
}

bool blueye_ipfs::is_file_exists(std::string _str_file_path, std::string _str_file_name)
{
	std::string _str_content_type_path = file_root_path_ + "\\" + _str_file_path + "\\";
	std::string _str_full_file_name = _str_content_type_path + _str_file_name;
	FILE *_fp = fopen(_str_full_file_name.c_str(), "r+b");
	if (_fp)
	{
		fclose(_fp);
		return true;
	}
	else
	{
		return false;
	}
}

time_t blueye_ipfs::get_file_mtime(std::string _str_file_type, std::string _str_file_name)
{
	std::string _str_content_type_path = file_root_path_ + "\\" + _str_file_type + "\\";
	std::string _str_full_file_name = _str_content_type_path + _str_file_name;
	struct stat buf;
	stat(_str_full_file_name.c_str(), &buf);
	return buf.st_mtime;
}
void blueye_ipfs::read_map_content_index(std::string _str_content_type)
{
	boost::property_tree::ptree root, data;

	//if file was not found, to return 
	std::string _str_content_type_path = file_root_path_ + "\\" + _str_content_type + "\\";
	std::string _str_full_file_name = _str_content_type_path + _str_content_type + ".json";
	FILE *_fp = fopen(_str_full_file_name.c_str(), "r+b");
	if (!_fp)
		return;
	else
		fclose(_fp);

	std::cout << "read_map_content_index("<< _str_content_type<<") ..." << std::endl;
	int _tick1, _tick2;
	_tick1 = GetTickCount();

	try { boost::property_tree::read_json<boost::property_tree::ptree>(_str_full_file_name, root); }
	catch (...) { return; }
	map_content_index_.clear();

	for (boost::property_tree::ptree::iterator it = root.begin(); it != root.end(); it++)
	{
		if (it->second.data() == "")
		{
			tag_CONTENT_INDEX _content_index;
			_content_index.reset();
			for (boost::property_tree::ptree::iterator itit = it->second.begin(); itit != it->second.end(); itit++)
			{
				if (itit->first != "")
				{
					if (itit->second.get<std::string>("") != "")
					{
						std::string _str_key = json_util::utf8_to_gb2312(itit->first);
						std::string _str_value = json_util::utf8_to_gb2312(itit->second.get<std::string>(""));
						if (_str_key == "name")
							_content_index.str_name_ = _str_value;
						else if (_str_key == "type")
							_content_index.str_type_ = _str_value;
						else if (_str_key == "node")
							_content_index.str_node_ = _str_value;
						else if (_str_key == "crc")
							_content_index.str_crc_ = _str_value;
					}
					else
					{
						if (itit->first == "file_list")
						{
							for (boost::property_tree::ptree::iterator ititit = itit->second.begin(); ititit != itit->second.end(); ititit++)
							{
								std::string _str_value = json_util::utf8_to_gb2312(ititit->second.get<std::string>(""));
								tag_FILE_INFO _file_info;
								_file_info.from_string(_str_value);
								_content_index.file_list_.push_back(_file_info);
							}
						}
					}
				}
			}
			map_content_index_.insert(std::make_pair(it->first, _content_index));
		}
	}
	_tick2 = GetTickCount();
	std::cout << "read_map_content_index("<< _str_content_type <<") map_content_index_ size " << map_content_index_.size() << ",time cost="<<_tick2-_tick1<<std::endl;
}

void blueye_ipfs::write_box_data_p1m(CALL_BACK_DATA *_blueye_data, int rec_count)
{
	std::string _downloadpath;
	if (SHA == _blueye_data->quote_head_.exchange)
		_downloadpath = box_file_root_path_ + "sha\\min\\";
	else if (SZN == _blueye_data->quote_head_.exchange)
		_downloadpath = box_file_root_path_ + "szn\\min\\";
	else
		return;
	FindOrCreateDirectory(_downloadpath.c_str());

	std::string _filename;
	char _str_temp[SYMBOL_LEN + 1] = {0};
	memcpy(_str_temp, (char*)_blueye_data->quote_head_.symbol, SYMBOL_LEN);
	_filename = _downloadpath + std::string(_str_temp) + ".tinymin";
	FILE * fp = fopen(_filename.c_str(), "wb+");
	if (fp)
	{
		for (int i = 0; i < rec_count; i++)
		{
			fwrite(&((ROM_TINY_KLINE_DATA*)_blueye_data->p_data_)[i], sizeof(ROM_TINY_KLINE_DATA), 1, fp);
		}
	}
	fclose(fp);
}

void blueye_ipfs::write_box_data_daily(CALL_BACK_DATA *_blueye_data, int rec_count)
{
	std::string _downloadpath;
	if (SHA == _blueye_data->quote_head_.exchange)
		_downloadpath = box_file_root_path_ + "sha\\k\\";
	else if (SZN == _blueye_data->quote_head_.exchange)
		_downloadpath = box_file_root_path_ + "szn\\k\\";
	else
		return;
	FindOrCreateDirectory(_downloadpath.c_str());

	std::string _filename;
	char _str_temp[SYMBOL_LEN + 1] = {0};
	memcpy(_str_temp, (char*)_blueye_data->quote_head_.symbol, SYMBOL_LEN);
	_filename = _downloadpath + std::string(_str_temp) + ".day";
	FILE * fp = fopen(_filename.c_str(), "wb+");
	if (fp)
	{
		for (int i = 0; i < rec_count; i++)
		{
			fwrite(&((ROM_KLINE_DATA*)_blueye_data->p_data_)[i], sizeof(ROM_KLINE_DATA), 1, fp);
		}
	}
	fclose(fp);
}

void blueye_ipfs::write_map_content_index(std::string _str_content_type)
{
	std::string _str_json = gen_json_from_map_content_index(_str_content_type);
	if (_str_json !="")
	{
		std::string _str_content_type_path = file_root_path_ + "\\" + _str_content_type + "\\";
		if (!boost::filesystem::is_directory(_str_content_type_path))
			boost::filesystem::create_directories(_str_content_type_path);
		std::string _str_full_file_name = _str_content_type_path + _str_content_type + ".json";
		FILE *_fp = fopen(_str_full_file_name.c_str(), "w+b");
		if (_fp)
		{
			fwrite(_str_json.c_str(), 1, _str_json.length(), _fp);
			fclose(_fp);
		}
		else
		{
			str_error_msg_ = "Error in create file[" + _str_full_file_name + "]";
			//log_to_file((char *)str_error_msg_.c_str(), 1, false);
		}
	}
}

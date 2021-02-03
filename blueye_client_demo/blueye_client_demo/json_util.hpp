#pragma once

#pragma once
#include <iostream>  
#include <stdlib.h>  
#include <stdio.h>  
#include <string>  
#include <sstream>  
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp>  
#include <boost/tokenizer.hpp>
#include <time.h>  
#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <fstream>
#include <direct.h>

class json_util
{	
public:
	json_util()
	{
		;
	}
	~json_util()
	{
		;
	}
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

	static std::string gen_json_from_grid(std::string _str_json_file, std::string _str_content_tag, std::vector<std::map<std::string, std::string>> &_grid)
	{
		if (_str_json_file == "" || _str_content_tag == "" || _grid.size() <= 0) return "";

		std::string _str_json = "";

		_str_json += "{\r\n";
		_str_json += ("\t\"" + _str_content_tag + "\":[\r\n");
		for (int i = 0; i < _grid.size(); i++)
		{
			_str_json += ("\t\t{\r\n");
			for (std::map<std::string, std::string>::iterator itit = _grid[i].begin(); itit != _grid[i].end(); )
			{
				_str_json += ("\t\t\t \"" + itit->first + "\":\"" + itit->second + "\"");
				itit++;
				if (itit != _grid[i].end())
					_str_json += (", \r\n");
				else
					_str_json += ("\r\n");
			}
			_str_json += "\t\t}";
			if (i >= 0 && i < (_grid.size() - 1))
				_str_json += ",";
			_str_json += ("\r\n");
		}
		_str_json += ("\t]\r\n");
		_str_json += "}\r\n";
		_str_json = gb2312_to_utf8(_str_json);
		std::cout << _str_json << std::endl;
		return _str_json;
	}

	static void write_grid_to_json(std::string _str_json_file, std::string _str_content_tag, std::vector<std::map<std::string, std::string>> &_grid)
	{
		std::string _str_json = gen_json_from_grid(_str_json_file, _str_content_tag, _grid);
		FILE *fp = fopen(_str_json_file.c_str(), "w+b");
		if (fp)
		{
			fwrite(_str_json.c_str(), 1, _str_json.length(), fp);
			fclose(fp);
		}
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
};
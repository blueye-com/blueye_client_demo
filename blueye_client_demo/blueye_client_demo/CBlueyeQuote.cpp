#include "stdafx.h"
#include "CBlueyeQuote.h"
#include <direct.h>

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

#define ROM_SYMBOL_TYPE_HK_STOCK		90
#define ROM_SYMBOL_TYPE_HK_FUTURE		91
#define ROM_SYMBOL_TYPE_USA_FUTURE		92
#define ROM_SYMBOL_TYPE_FOREX			93

#define ROM_SYMBOL_TYPE_UNKNOWN			99
#define ROM_SYMBOL_TYPE_BLOCK			100

CQuoteForWindow g_quote_for_window;

CBlueyeQuote::CBlueyeQuote()
{
	;
}


CBlueyeQuote::~CBlueyeQuote()
{
	;
}

void CBlueyeQuote::blueye_call_back(PRCV_REPORT_STRUCTEx _report_data, int _rec_count)
{
	if (g_quote_for_window.h_wnd_)
	{
		for (int i = 0; i < _rec_count; i++)
			g_quote_for_window.PushQuoteData(_report_data[i]);
		::SendMessage(g_quote_for_window.h_wnd_, WM_BLUEYE_QUOTE_DATA, _rec_count, 0);
	}
	else
	{
		for (int i = 0; i < _rec_count; i++)
		{
			ROM_REAL_TIME_DATA_BS5 _rom_quote;
			//CBlueyeQuote::convert_data(&_report_data[i], &_rom_quote);
			std::cout<< _report_data[i].to_string() << std::endl;
		}
	}
}

void CBlueyeQuote::log(std::string _str_log)
{
	if (_str_log !="")
	{
		std::cout << _str_log << std::endl;
	}
}
void CBlueyeQuote::read_config_from_file()
{
	char sIniFileName[MAX_PATH];

	_getcwd(sIniFileName, MAX_PATH);
	strcat(sIniFileName, "\\Route.ini");

	GetPrivateProfileString("QUOTE_CONFIG", "version", "2019", config_param_.version_, sizeof(config_param_.version_), sIniFileName);
	config_param_.full_data_ = GetPrivateProfileInt("QUOTE_CONFIG", "full_data", 0, sIniFileName);
	config_param_.subscribe_ = GetPrivateProfileInt("QUOTE_CONFIG", "subscribe", 0, sIniFileName);
	config_param_.use_local_dll_ = GetPrivateProfileInt("QUOTE_CONFIG", "use_local_dll", 0, sIniFileName);
	memset(config_param_.sIp_, 0, sizeof(config_param_.sIp_));
	GetPrivateProfileString("QUOTE_CONFIG", "remote_ip", "127.0.0.1", config_param_.sIp_, sizeof(config_param_.sIp_), sIniFileName);
	config_param_.remote_port_ = GetPrivateProfileInt("QUOTE_CONFIG", "remote_port", 6000, sIniFileName);
	config_param_.listen_port_ = GetPrivateProfileInt("QUOTE_CONFIG", "listen_port", 6000, sIniFileName);
	GetPrivateProfileString("QUOTE_CONFIG", "token", "K8H7A08A", config_param_.token_, sizeof(config_param_.token_), sIniFileName);
}

bool CBlueyeQuote::init(HANDLE	_shutdown_event, std::string _str_token)
{
	read_config_from_file();

	shutdown_event_ = NULL;
	start_io_service_ = NULL;
	pack_subscribe_ = NULL;
	instance_ = NULL;
	instance_ = LoadLibrary("blueye_client32.dll");
	if (instance_)
	{
		start_io_service_ = (p_start_io_service)GetProcAddress(instance_, "i_start_io_service");
		stop_io_service_ = (p_stop_io_service)GetProcAddress(instance_, "i_stop_io_service");
		pack_subscribe_ = (p_pack_subscribe)GetProcAddress(instance_, "i_pack_subscribe");
		if (start_io_service_)
		{
			start_io_service_(std::string(config_param_.sIp_), std::to_string(config_param_.remote_port_), _shutdown_event, blueye_call_back, log, _str_token);
			return true;
		}
		else
			return false;
	}
	else
		return false;
}
void CBlueyeQuote::release()
{
	if (stop_io_service_)
	{
		stop_io_service_();
	}
}

void CBlueyeQuote::switch_window(HWND _h_wnd)
{
	g_quote_for_window.h_wnd_ = _h_wnd;
	g_quote_for_window.Clear();
}

void CBlueyeQuote::pack_subscribe(std::vector<STRU_SYMBOL> &_symbols)
{
	if (pack_subscribe_ == NULL || _symbols.size() <= 0) return;

	std::string _str_symbols_list = "";
	for (int i=0;i<_symbols.size();i++)
	{
		if (_symbols[i].nExchange == SZN)
			_str_symbols_list += std::to_string(SZ_MARKET_EX) + std::string(_symbols[i].symbol);
		else
			_str_symbols_list += std::to_string(SH_MARKET_EX) + std::string(_symbols[i].symbol);
		if (i < _symbols.size())
			_str_symbols_list += ",";
	}

	pack_subscribe_(_str_symbols_list);
	//log(_str_symbols_list.c_str());
}

int CBlueyeQuote::pop_quote(RCV_REPORT_STRUCTEx &_rom_quote)
{
	return g_quote_for_window.PopQuoteData(_rom_quote);
}

void CBlueyeQuote::stop_data()
{
	if (pack_subscribe_)
	{
		pack_subscribe_(std::to_string(SH_MARKET_EX) + "123456");
	}
	g_quote_for_window.h_wnd_ = NULL;
	g_quote_for_window.Clear();
}


unsigned char CBlueyeQuote::get_stock_type(WORD dwExchange, char *sSymbol)
{
	unsigned char type = ROM_SYMBOL_TYPE_UNKNOWN;
	if ((dwExchange != SH_MARKET_EX && dwExchange != SZ_MARKET_EX) || sSymbol == NULL)
		return type;

	if (dwExchange == SZ_MARKET_EX)  // exchange of SZN
	{
		if (sSymbol[0] == '0' && sSymbol[1] == '0' && sSymbol[2] == '0')
			type = ROM_SYMBOL_TYPE_SZN_A;
		else if (sSymbol[0] == '0' && sSymbol[1] == '0' && sSymbol[2] == '1')
			type = ROM_SYMBOL_TYPE_SZN_A;
		else if (sSymbol[0] == '0' && sSymbol[1] == '0' && sSymbol[2] == '2')
			type = ROM_SYMBOL_TYPE_SZN_A;
		else if (sSymbol[0] == '2' && sSymbol[1] == '0' && sSymbol[2] == '0')
			type = ROM_SYMBOL_TYPE_SZN_B;
		else if (sSymbol[0] == '1' && sSymbol[1] == '5' ||
			sSymbol[0] == '1' && sSymbol[1] == '6' ||
			sSymbol[0] == '1' && sSymbol[1] == '7' ||
			sSymbol[0] == '1' && sSymbol[1] == '8')
			type = ROM_SYMBOL_TYPE_SZN_FUND;
		else if (sSymbol[0] == '1' && sSymbol[1] == '0' ||
			sSymbol[0] == '1' && sSymbol[1] == '1' ||
			sSymbol[0] == '1' && sSymbol[1] == '2')
			type = ROM_SYMBOL_TYPE_SZN_BOND;
		else if (sSymbol[0] == '3' && sSymbol[1] == '9' && sSymbol[2] == '9')
			type = ROM_SYMBOL_TYPE_SZN_INDEX;
		else if (sSymbol[0] == '3' && sSymbol[1] == '9' && sSymbol[2] == '5')
			type = ROM_SYMBOL_TYPE_SZN_INDEX;
		else if (sSymbol[0] == '0' && sSymbol[1] == '3')
			type = ROM_SYMBOL_TYPE_SZN_OPTION;
		else if (sSymbol[0] == '3' && sSymbol[1] == '0')
			type = ROM_SYMBOL_TYPE_SZN_CREATIVE;
		else
			type = ROM_SYMBOL_TYPE_UNKNOWN;
	}
	else
	{
		if (sSymbol[0] == '0' && sSymbol[1] == '0' && sSymbol[2] == '0')
			type = ROM_SYMBOL_TYPE_SHA_INDEX;
		else if (sSymbol[0] == '6' && sSymbol[1] == '0' && sSymbol[2] == '0')
			type = ROM_SYMBOL_TYPE_SHA_A;
		else if (sSymbol[0] == '6' && sSymbol[1] == '0' && sSymbol[2] == '1')
			type = ROM_SYMBOL_TYPE_SHA_A;
		else if (sSymbol[0] == '6' && sSymbol[1] == '8' && sSymbol[2] == '8')
			type = ROM_SYMBOL_TYPE_SCI_CREATIVE;
		else if (sSymbol[0] == '9' && sSymbol[1] == '0' && sSymbol[2] == '0')
			type = ROM_SYMBOL_TYPE_SHA_B;
		else if (sSymbol[0] == '5' && sSymbol[1] == '0' && sSymbol[2] == '0' ||
			sSymbol[0] == '5' && sSymbol[1] == '1' && sSymbol[2] == '0' ||
			sSymbol[0] == '5' && sSymbol[1] == '1' && sSymbol[2] == '1' ||
			sSymbol[0] == '5' && sSymbol[1] == '1' && sSymbol[2] == '2' ||
			sSymbol[0] == '5' && sSymbol[1] == '1' && sSymbol[2] == '3')
			type = ROM_SYMBOL_TYPE_SHA_FUND;
		else if (sSymbol[0] == '0' && sSymbol[1] == '1')
			type = ROM_SYMBOL_TYPE_SHA_BOND;
		else if (sSymbol[0] == '1' && sSymbol[1] == '0')
			type = ROM_SYMBOL_TYPE_SHA_BOND;
		else if (sSymbol[0] == '1' && sSymbol[1] == '1')
			type = ROM_SYMBOL_TYPE_SHA_BOND;
		else if (sSymbol[0] == '1' && sSymbol[1] == '2')
			type = ROM_SYMBOL_TYPE_SHA_BOND;
		else if (sSymbol[0] == '2' && sSymbol[1] == '0')
			type = ROM_SYMBOL_TYPE_SHA_BOND;
		else if (sSymbol[0] == '0' && sSymbol[1] == '0' && sSymbol[2] == '9'&&
			sSymbol[3] == '9' && sSymbol[4] == '8' && sSymbol[5] == '8')
			type = ROM_SYMBOL_TYPE_SHA_BOND;
		else if (sSymbol[0] == '8')
			type = ROM_SYMBOL_TYPE_SHA_OPTION;			//ROM added on 2019.06.08.
		else if (sSymbol[0] == 'O' && sSymbol[1] == 'P')
			type = ROM_SYMBOL_TYPE_SHA_OPTION;
		else
			type = ROM_SYMBOL_TYPE_UNKNOWN;
	}

	return type;
}
int	CBlueyeQuote::get_number_multiply(int iSymbolType)
{
	switch (iSymbolType)
	{
	case ROM_SYMBOL_TYPE_SZN_INDEX:
	case ROM_SYMBOL_TYPE_SZN_A:
	case ROM_SYMBOL_TYPE_SZN_B:
		return NUMBER_MULTIPLY_FOR_STOCK;
	case ROM_SYMBOL_TYPE_SZN_BOND:
		return NUMBER_MULTIPLY_FOR_BOND;
	case ROM_SYMBOL_TYPE_SZN_FUND:
		return NUMBER_MULTIPLY_FOR_FUND;
	case ROM_SYMBOL_TYPE_SZN_OPTION:
		return NUMBER_MULTIPLY_FOR_OPTION;
	case ROM_SYMBOL_TYPE_SZN_CREATIVE:
		return NUMBER_MULTIPLY_FOR_STOCK;
	case ROM_SYMBOL_TYPE_SHA_INDEX:
		return NUMBER_MULTIPLY_FOR_STOCK;
	case ROM_SYMBOL_TYPE_SCI_CREATIVE:
		return NUMBER_MULTIPLY_FOR_STOCK;
	case ROM_SYMBOL_TYPE_SHA_A:
		return NUMBER_MULTIPLY_FOR_STOCK;
	case ROM_SYMBOL_TYPE_SHA_B:
		return NUMBER_MULTIPLY_FOR_STOCK;
	case ROM_SYMBOL_TYPE_SHA_BOND:
		return NUMBER_MULTIPLY_FOR_BOND;
	case ROM_SYMBOL_TYPE_SHA_FUND:
		return NUMBER_MULTIPLY_FOR_FUND;
	case ROM_SYMBOL_TYPE_SHA_OPTION:
		return NUMBER_MULTIPLY_FOR_OPTION;
	case ROM_SYMBOL_TYPE_HK_STOCK:
		return NUMBER_MULTIPLY_FOR_STOCK;
	default:
		return NUMBER_MULTIPLY_FOR_STOCK;
	}
}

int CBlueyeQuote::convert_data(RCV_REPORT_STRUCTEx *pSource, ROM_REAL_TIME_DATA_BS5 *pDest)
{
	if (pSource == NULL || pDest == NULL) return 0;

	int iMultipleVol;


	memset(pDest, 0, sizeof(ROM_REAL_TIME_DATA_BS5));

	switch (pSource->m_wMarket)
	{
	case SH_MARKET_EX:
		pDest->exchange = SHA;
		pDest->type = CBlueyeQuote::get_stock_type(pSource->m_wMarket, pSource->m_szLabel);
		break;
	case SZ_MARKET_EX:
		pDest->exchange = SZN;
		pDest->type = CBlueyeQuote::get_stock_type(pSource->m_wMarket, pSource->m_szLabel);
		break;
	case HK_MARKET_EX:
		pDest->exchange = SHA;
		pDest->type = ROM_SYMBOL_TYPE_HK_STOCK;
		break;
	case HK_FUTURE_MARKET_EX:
		pDest->exchange = SHA;
		pDest->type = ROM_SYMBOL_TYPE_HK_FUTURE;
		break;
	case USA1_FUTURE_MARKET_EX:
	case USA2_FUTURE_MARKET_EX:
		pDest->exchange = SHA;
		pDest->type = ROM_SYMBOL_TYPE_USA_FUTURE;
		break;
	default:
		return 0;
	}

	CBlueyeQuote::code_wjf_2_rom(pSource->m_wMarket, pSource->m_szLabel, (char *)pDest->symbol);
	memset(pDest->symbol_name, 0, sizeof(pDest->symbol_name));
	if (strlen(pSource->m_szName) < sizeof(pDest->symbol_name))
		memcpy(pDest->symbol_name, pSource->m_szName, strlen(pSource->m_szName));
	else
		memcpy(pDest->symbol_name, pSource->m_szName, sizeof(pDest->symbol_name) - 2);

	//if(pDest->type == ROM_SYMBOL_TYPE_SHA_INDEX)
	iMultipleVol = 1;
	//else
	//	iMultipleVol = 100;

	pDest->amount = (LONGLONG)(pSource->m_fAmount);
	pDest->volume = (LONGLONG)pSource->m_fVolume*iMultipleVol;

	int iNumberMultiply = CBlueyeQuote::get_number_multiply(pDest->type);
	float fLoss = 0.5;
	pDest->openprice = (int)(pSource->m_fOpen*iNumberMultiply + fLoss);
	pDest->closeprice = (int)(pSource->m_fLastClose*iNumberMultiply + fLoss);
	pDest->highprice = (int)(pSource->m_fHigh*iNumberMultiply + fLoss);
	pDest->lowprice = (int)(pSource->m_fLow*iNumberMultiply + fLoss);
	pDest->newprice = (int)(pSource->m_fNewPrice*iNumberMultiply + fLoss);
	pDest->vbuy1 = (LONGLONG)((pSource->m_fBuyVolume[0])*iMultipleVol + fLoss);
	pDest->vbuy2 = (LONGLONG)((pSource->m_fBuyVolume[1])*iMultipleVol + fLoss);
	pDest->vbuy3 = (LONGLONG)((pSource->m_fBuyVolume[2])*iMultipleVol + fLoss);
	pDest->vbuy4 = (LONGLONG)((pSource->m_fBuyVolume4)*iMultipleVol + fLoss);
	pDest->vbuy5 = (LONGLONG)((pSource->m_fBuyVolume5)*iMultipleVol + fLoss);

	pDest->pbuy1 = (int)(pSource->m_fBuyPrice[0] * iNumberMultiply + fLoss);
	pDest->pbuy2 = (int)(pSource->m_fBuyPrice[1] * iNumberMultiply + fLoss);
	pDest->pbuy3 = (int)(pSource->m_fBuyPrice[2] * iNumberMultiply + fLoss);
	pDest->pbuy4 = (int)(pSource->m_fBuyPrice4*iNumberMultiply + fLoss);
	pDest->pbuy5 = (int)(pSource->m_fBuyPrice5*iNumberMultiply + fLoss);

	pDest->vsell1 = (LONGLONG)(pSource->m_fSellVolume[0])*iMultipleVol;
	pDest->vsell2 = (LONGLONG)(pSource->m_fSellVolume[1])*iMultipleVol;
	pDest->vsell3 = (LONGLONG)(pSource->m_fSellVolume[2])*iMultipleVol;
	pDest->vsell4 = (LONGLONG)(pSource->m_fSellVolume4*iMultipleVol);
	pDest->vsell5 = (LONGLONG)(pSource->m_fSellVolume5*iMultipleVol);

	pDest->psell1 = (int)(pSource->m_fSellPrice[0] * iNumberMultiply + fLoss);
	pDest->psell2 = (int)(pSource->m_fSellPrice[1] * iNumberMultiply + fLoss);
	pDest->psell3 = (int)(pSource->m_fSellPrice[2] * iNumberMultiply + fLoss);
	pDest->psell4 = (int)(pSource->m_fSellPrice4*iNumberMultiply + fLoss);
	pDest->psell5 = (int)(pSource->m_fSellPrice5*iNumberMultiply + fLoss);

	pDest->time = pSource->m_time;
	//if(pDest->type == ROM_SYMBOL_TYPE_SHA_OPTION)
	//	ShowConsole("收到期权数据");
	return 1;
}

void CBlueyeQuote::code_wjf_2_rom(WORD wMarket, char *sWjf, char *sRom)
{
	if (sWjf == NULL || strlen(sWjf) > ROM_SYMBOL_LEN || strlen(sWjf)<3 || sRom == NULL) return;

	int iWjfLen = strlen(sWjf);
	try {

		switch (wMarket)
		{
		case HK_MARKET_EX:
			if (sWjf[0] >= '0' && sWjf[0] <= '9')
			{
				memcpy(sRom, "HK", 2);
				memcpy(sRom + 2, sWjf, ROM_SYMBOL_LEN - 2);
			}
			else
			{
				memcpy(sRom, sWjf, iWjfLen);
				if (iWjfLen < ROM_SYMBOL_LEN)
				{
					for (int i = iWjfLen; i<ROM_SYMBOL_LEN; i++)
						sRom[i] = '0';
				}
			}
			break;
		case HK_FUTURE_MARKET_EX:
			memcpy(sRom, "HZ", 2);
			memcpy(sRom + 2, sWjf, ROM_SYMBOL_LEN - 2);
			break;
		case USA1_FUTURE_MARKET_EX:
			memcpy(sRom, "ID", 2);
			memcpy(sRom + 2, sWjf, ROM_SYMBOL_LEN - 2);
			break;
		case USA2_FUTURE_MARKET_EX:
			memcpy(sRom, sWjf, iWjfLen);
			if (iWjfLen < ROM_SYMBOL_LEN)
			{
				for (int i = iWjfLen; i<ROM_SYMBOL_LEN; i++)
					sRom[i] = '0';
			}
			break;
		case SH_FUTRE_MARKET_EX:
			if (sWjf[0] == '0')
			{
				memcpy(sRom, "N", 1);
				memcpy(sRom + 1, sWjf + 1, ROM_SYMBOL_LEN - 1);
			}
			else
			{
				memcpy(sRom, sWjf, iWjfLen);
				if (iWjfLen < ROM_SYMBOL_LEN)
				{
					for (int i = iWjfLen; i<ROM_SYMBOL_LEN; i++)
						sRom[i] = '0';
				}
			}
			break;
		case CN_FUTRE_MARKET_EX:
			memcpy(sRom, sWjf, iWjfLen);
			if (iWjfLen < ROM_SYMBOL_LEN)
			{
				for (int i = iWjfLen; i<ROM_SYMBOL_LEN; i++)
					sRom[i] = '0';
			}
			break;
		case FOREX_MARKET_EX:
			memcpy(sRom, sWjf, iWjfLen);
			if (iWjfLen < ROM_SYMBOL_LEN)
			{
				for (int i = iWjfLen; i<ROM_SYMBOL_LEN; i++)
					sRom[i] = '0';
			}
			break;
		case GLOBAL_MARKET_EX:
			memcpy(sRom, sWjf, iWjfLen);
			if (iWjfLen < ROM_SYMBOL_LEN)
			{
				for (int i = iWjfLen; i<ROM_SYMBOL_LEN; i++)
					sRom[i] = '0';
			}
			break;
		default:
			memcpy(sRom, sWjf, ROM_SYMBOL_LEN);
			break;
		}
	}
	catch (...)
	{
		sRom[0] = '\0';
	}
}

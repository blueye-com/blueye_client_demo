#include <iostream>
#include "ManyMinutesDataConversion.h"

extern CString GetModuleFilePath(HMODULE hModule);

BOOL IsFuture(int nType)
{
	if (nType == ROM_SYMBOL_TYPE_SH_FUTURE ||
		nType == ROM_SYMBOL_TYPE_ZZ_FUTURE ||
		nType == ROM_SYMBOL_TYPE_DL_FUTURE ||
		nType == ROM_SYMBOL_TYPE_ZJ_FUTURE)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL IsIndex(int nType)
{
	if (nType == ROM_SYMBOL_TYPE_SHA_INDEX ||
		nType == ROM_SYMBOL_TYPE_SZN_INDEX)
		return TRUE;

	return FALSE;
}

BOOL IsIndexOrBlock(int nType)
{
	if (IsIndex(nType) ||
		nType == ROM_SYMBOL_TYPE_BLOCK)
		return TRUE;
	return FALSE;
}

BOOL IsTJSpot(int nType)
{
	if (nType == ROM_SYMBOL_TYPE_TJ_SILVER)
		return TRUE;
	return FALSE;
}

BOOL IsStock(int nType)
{
	if (!IsIndexOrBlock(nType) && !IsFuture(nType) && !IsTJSpot(nType))
		return TRUE;

	return FALSE;
}

BOOL IsIndexOrStock(int nType)
{
	if (IsIndexOrBlock(nType) || IsStock(nType))
		return TRUE;

	return FALSE;
}

//从文件读取分钟K线（包括，1分钟， 5分钟，15分钟，30分钟，60分钟）
//参数：CString sFilePath,  文件的路径
//		ROM_KLINE_DATA *pRomKlineData 一个指针，
//返回：分钟K线的条数
int ReadMiniteKline(const CString & sFilePath, ROM_KLINE_DATA *&pRomKlineData)
{
	if (pRomKlineData)
		delete[] pRomKlineData;

	CFile file;
	if (file.Open(sFilePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite))
	{
		int nFileLength = file.GetLength();
		int nRomKlineCount = nFileLength / sizeof(ROM_KLINE_DATA);			//记录条数
		if (nRomKlineCount > 0)
		{
			pRomKlineData = new ROM_KLINE_DATA[nFileLength / sizeof(ROM_KLINE_DATA)];
			if (!pRomKlineData)
			{
				return -1;
			}
			ZeroMemory(pRomKlineData, nRomKlineCount * sizeof(ROM_KLINE_DATA));
			file.Read(pRomKlineData, nRomKlineCount * sizeof(ROM_KLINE_DATA));
		}
		else
		{
			nRomKlineCount = 0;
		}
		file.Close();

		return nRomKlineCount;
	}

	return 0;
}

time_t ConvertTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec)
{
	struct tm tt;
	tt.tm_year = nYear - 1900;
	tt.tm_mon = nMonth - 1;
	tt.tm_mday = nDay;
	tt.tm_hour = nHour;
	tt.tm_min = nMin;
	tt.tm_sec = nSec;
	return mktime(&tt);
}

int InsertUniqueSortValueEx(ROM_KLINE_DATA *pKLineArray, int nCount, ROM_KLINE_DATA *pKLineSingle)
{
	if (pKLineArray == NULL || nCount < 0 || pKLineSingle == NULL || pKLineSingle->date == 0 || pKLineSingle->time == 0) return nCount;

	int low = 0, high = nCount - 1, mid, pos, i;
	int found = 0;
	time_t t1, t2;

	while (low <= high && !found)					//采用二分查找法，查找pKLineSingle的插入位置
	{
		mid = (low + high) / 2;
		t1 = ConvertTime(pKLineSingle->date / 10000,
			pKLineSingle->date % 10000 / 100,
			pKLineSingle->date % 100,
			pKLineSingle->time / 3600,
			pKLineSingle->time % 3600 / 60,
			0);
		t2 = ConvertTime(pKLineArray[mid].date / 10000,
			pKLineArray[mid].date % 10000 / 100,
			pKLineArray[mid].date % 100,
			pKLineArray[mid].time / 3600,
			pKLineArray[mid].time % 3600 / 60,
			0);
		if (t1 < t2)
			high = mid - 1;
		else if (t1 > t2)
			low = mid + 1;
		else found = 1;
	}
	if (found)
	{
		pos = mid;
		memcpy(&pKLineArray[pos], (char *)pKLineSingle, sizeof(ROM_KLINE_DATA));
		return nCount;
	}
	else
	{
		pos = low;							//插入位置是下标为pos的数组元素
		for (i = nCount - 1; i >= pos; i--)			//从下标为n-1~pos的元素依次往后移
			memcpy(&pKLineArray[i + 1], &pKLineArray[i], sizeof(ROM_KLINE_DATA));
		memcpy(&pKLineArray[pos], (char *)pKLineSingle, sizeof(ROM_KLINE_DATA));     //将pKLineSingle存入下标为pos的数组元素中
		return ++nCount;
	}
}

//插入分钟K线数据
//参数：ROM_KLINE_DATA *&pRomKlineData		已有的分钟K线数组
//		int nArrayCount						已有数组的长度
//		ROM_KLINE_DATA * pNeedRomKlineData	需要插入的分钟K线数组
//		int nKLineCount						需要插入的数量
//返回：插入完后，分钟K线的长度。
int InsertMinuteKlineData(ROM_KLINE_DATA *&pRomKlineData, int nArrayCount, ROM_KLINE_DATA *pNeedRomKlineData, int nKLineCount)
{
	if (nKLineCount <= 0 || nArrayCount < 0) return 0;

	int nAllMiniteKlineCount = nArrayCount + nKLineCount;

	ROM_KLINE_DATA *pNewRomKlineDate = new ROM_KLINE_DATA[nAllMiniteKlineCount];
	if (!pNewRomKlineDate) return 0;

	ZeroMemory(pNewRomKlineDate, sizeof(ROM_KLINE_DATA) * nAllMiniteKlineCount);
	if (pRomKlineData != NULL && nArrayCount > 0)
	{
		memcpy(pNewRomKlineDate, pRomKlineData, sizeof(ROM_KLINE_DATA)*nArrayCount);
		delete[] pRomKlineData;
	}

	pRomKlineData = pNewRomKlineDate;
	pNewRomKlineDate = NULL;

	int i;
	for (i = 0; i < nKLineCount; i++)
	{
		nArrayCount = InsertUniqueSortValueEx(pRomKlineData, nArrayCount, &pNeedRomKlineData[i]);

		if (nArrayCount > nAllMiniteKlineCount)
		{
			break;
		}
	}

	return nArrayCount;
}

void GET_CURRENT_DIRECTORY(int iSize, LPTSTR szText)
{
	CString strPath = GetModuleFilePath(NULL);

	memset(szText, 0, iSize);
	if (iSize > strPath.GetLength())
		strcpy(szText, strPath.GetBuffer(strPath.GetLength()));
	else
		strncpy(szText, strPath.GetBuffer(strPath.GetLength()), iSize - 1);
}

//根据周期，获取周期存储路径
//参数： CString &sCycleFolder		out 周期文件夹名称
//		 int iKlineType				in k线周期
void GeneryCycleFoloderName(CString &sCycleFolder, int iKlineType)
{
	CString sFolderName;
	switch (iKlineType)
	{
	case KLINE_TYPE_MINUTE5_DATA:
	case KLINE_TYPE_MINUTE15_DATA:
	case KLINE_TYPE_MINUTE30_DATA:
	case KLINE_TYPE_MINUTE60_DATA:
	case KLINE_TYPE_MINUTE120_DATA:
	case KLINE_TYPE_MINUTE240_DATA:
		sFolderName = _T("minuteKline\\");
		break;
	case KLINE_TYPE_DAY_DATA:
		sFolderName.Empty();
		break;
	case KLINE_TYPE_WEEK_DATA:
		sFolderName = _T("weekKline\\");
		break;
	case KLINE_TYPE_MONTH_DATA:
		sFolderName = _T("monthKline\\");
		break;
	}
	sCycleFolder = sFolderName;
}


//获取分钟扩展名
//参数：CString &sNameExtend	out 文件扩展名
//		int iKlineType			in 文件周期
void GeneryMinuteExtend(CString &sNameExtend, char *sSymbol, int iKlineType)
{
	CString sNameEx;
	switch (iKlineType)
	{
	case KLINE_TYPE_MINUTE5_DATA:
	case KLINE_TYPE_MINUTE15_DATA:
	case KLINE_TYPE_MINUTE30_DATA:
	case KLINE_TYPE_MINUTE60_DATA:
	case KLINE_TYPE_MINUTE120_DATA:
		sNameEx.Format(_T("%s.%dm"), sSymbol, iKlineType);
		break;
	case KLINE_TYPE_MINUTE240_DATA:
		sNameEx.Format(_T("%s.%dm"), sSymbol, 240);
		break;
	case KLINE_TYPE_DAY_DATA:
		sNameEx.Empty();
		break;
	case KLINE_TYPE_WEEK_DATA:
		sNameEx.Format(_T("%s.week"), sSymbol);
		break;
	case KLINE_TYPE_MONTH_DATA:
		sNameEx.Format(_T("%s.mon"), sSymbol);
		break;
	}

	sNameExtend = sNameEx;
}


//根据市场类型（股票，现货，期货）获取存储的文件夹名称
//参数： CString &sFolderName     out 返回的文件夹名称
//		 BYTE exchange			  in  交易所代码，在市场类型为股票的时候区分沪市和深市， 0表示深市， 1表示沪市
//		 int iMarkType			  in  市场类型  （股票，现货，期货）
void GeneryMinuteFolderName(CString &sFolderName, BYTE exchange, int iMarkType)
{
	CString sMarkFolder;
	if (IsIndexOrStock(iMarkType))
	{
		if (exchange == SHA)
			sMarkFolder = _T("sha\\");
		else
			sMarkFolder = _T("szn\\");
	}
	else if (IsFuture(iMarkType))
	{
		sMarkFolder = _T("futures\\");
	}
	else if (IsTJSpot(iMarkType))
	{
		sMarkFolder = _T("spot\\");
	}
	else
	{
		sMarkFolder = _T("other\\");
	}

	sFolderName = sMarkFolder;
}

BOOL AutoCreateDir(const CString& sFilePath)
{
	if (sFilePath.IsEmpty()) return 0;

	CString pathname = sFilePath;
	int nDotIndex = pathname.Find("\.");

	if (nDotIndex < 0 && pathname.Right(1) != '\\')
		pathname += "\\";

	int end = pathname.ReverseFind('\\');

	int pt = pathname.Find("\\");

	if (pt > 1 && pathname[pt - 1] == ':')
		pt = pathname.Find("\\", pt + 1);

	CString path;
	while (pt != -1 && pt <= end)
	{
		path = pathname.Left(pt + 1);
		if (CreateDirectory(path, NULL) == FALSE)
		{
			if (ERROR_ALREADY_EXISTS != GetLastError())
				return FALSE;
		}
		pt = pathname.Find("\\", pt + 1);
	}

	return TRUE;
}

void WriteWeekOrMonthOrMiniteData(const CString &sFilePath, ROM_KLINE_DATA *pWeeklyOrMonthKLine, UINT nWeekOrMonthCount)
{
	if (pWeeklyOrMonthKLine == NULL || nWeekOrMonthCount <= 0) return;

	CFile file;
	if (file.Open(sFilePath, CFile::modeCreate | CFile::modeReadWrite))
	{
		file.Write((void *)pWeeklyOrMonthKLine, sizeof(ROM_KLINE_DATA) * nWeekOrMonthCount);
		file.Close();
	}

#ifdef TEST_WEEK_MONTH_DATA
	CFile infoFile;
	CString sLogPath = sFilePath;
	int endIndex = sLogPath.ReverseFind('\\');
	CString sOldPath = sLogPath.Left(endIndex + 1);
	sLogPath = sLogPath.Right(sLogPath.GetLength() - endIndex - 1);
	endIndex = sLogPath.Find('\.');
	if (endIndex > 0)
	{
		sLogPath = sLogPath.Left(endIndex);
		sLogPath += ".txt";
		sOldPath += sLogPath;
	}
	if (infoFile.Open(sOldPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite))
	{
		infoFile.Seek(0l, CFile::end);
		int i;
		ROM_KLINE_DATA romKlineData;
		CString romKlineInfo;
		for (i = 0; i < nWeekOrMonthCount; i++)
		{
			memcpy(&romKlineData, pWeeklyOrMonthKLine + i, sizeof(ROM_KLINE_DATA));
			romKlineInfo.Format("date = %d, time = %d, amount = %u,", romKlineData.date, romKlineData.time, romKlineData.amount);
			CString romKlineVolume;
			romKlineVolume.Format("volume = %u,", romKlineData.volume);
			romKlineInfo += romKlineVolume;
			romKlineVolume.Format("openprice = %u,", romKlineData.openprice);
			romKlineInfo += romKlineVolume;
			romKlineVolume.Format("closeprice = %u,", romKlineData.closeprice);
			romKlineInfo += romKlineVolume;
			romKlineVolume.Format("highprice = %u,", romKlineData.highprice);
			romKlineInfo += romKlineVolume;
			romKlineVolume.Format("lowprice = %u\r\n", romKlineData.lowprice);
			romKlineInfo += romKlineVolume;
			infoFile.Write(romKlineInfo.GetBuffer(romKlineInfo.GetLength()), romKlineInfo.GetLength());
			romKlineInfo.ReleaseBuffer();
		}

		infoFile.Write("\r\n", sizeof("\r\n"));
		infoFile.Close();
	}
#endif
}

int FilterUnusualData(float fDifference, ROM_KLINE_DATA * pRomKLine, int nKlineCount)
{
	if (!pRomKLine || nKlineCount <= 0) return -1;

	//先就算k线close平均价格
	int i;
	LONGLONG llClose = 0;
	for (i = 0; i < nKlineCount; i++)
	{
		llClose += pRomKLine[i].closeprice;
	}
	llClose /= (LONGLONG)nKlineCount;

	ROM_KLINE_DATA * pTempKline = new ROM_KLINE_DATA[nKlineCount];
	if (!pTempKline)
	{
		return -1;
	}
	memset(pTempKline, 0, sizeof(ROM_KLINE_DATA) * nKlineCount);

	//过滤处理
	int nIndex = 0;
	for (i = 0; i < nKlineCount; i++)
	{
		float fOpen = (float)llClose / (float)pRomKLine[i].openprice;
		float fClose = (float)llClose / (float)pRomKLine[i].closeprice;
		float fHigh = (float)llClose / (float)pRomKLine[i].highprice;
		float fLow = (float)llClose / (float)pRomKLine[i].lowprice;

		if ((fOpen >= fDifference || fOpen <= (float)(1 / fDifference)) ||
			(fClose >= fDifference || fClose <= (float)(1 / fDifference)) ||
			(fHigh >= fDifference || fHigh <= (float)(1 / fDifference)) ||
			(fLow >= fDifference || fLow <= (float)(1 / fDifference)) ||
			(i > 0 && pRomKLine[i].amount < pRomKLine[i - 1].amount) ||
			(i > 0 && pRomKLine[i].volume < pRomKLine[i - 1].volume) ||
			pRomKLine[i].date <= 0 || pRomKLine[i].time < 0 ||
			(i > 0 && pRomKLine[i].date == pRomKLine[i - 1].date && pRomKLine[i].time == pRomKLine[i - 1].time) ||
			(i > 0 && pRomKLine[i].time == 46800)	//zdx add 过滤掉13点整的这条分钟线
			//(i > 0 && i < nKlineCount-1 && pRomKLine[i].date == pRomKLine[i+1].date && pRomKLine[i].time > pRomKLine[i+1].time)  //zdx add 2014.7.31过滤掉时间乱序的
			)
		{
			continue;
		}

		memcpy(pTempKline + nIndex, &pRomKLine[i], sizeof(ROM_KLINE_DATA));
		nIndex++;
	}
	memcpy(pRomKLine, pTempKline, sizeof(ROM_KLINE_DATA)*nKlineCount);

	if (pTempKline)
		delete[] pTempKline;

	return nIndex;
}

int CorrectMinIndex(int iOriginalIndex)
{
	if (iOriginalIndex < (9 * 60 + 30))
		iOriginalIndex = 9 * 60 + 30;
	else if (iOriginalIndex > (11 * 60 + 30) && iOriginalIndex < 13 * 60)
		iOriginalIndex = 11 * 60 + 30;
	else if (iOriginalIndex >= 15 * 60)
		iOriginalIndex = 15 * 60 - 1;

	return iOriginalIndex;
}

time_t GetMinuteKLineFormatTimeForStock(time_t t, int iKLineType)
{
	int nYear, nMonth, nDay, iHour, iMin, iGap = 0, iMinIndex;
	CTime tt = CTime(t);
	nYear = tt.GetYear();
	nMonth = tt.GetMonth();
	nDay = tt.GetDay();
	iHour = tt.GetHour();
	iMin = tt.GetMinute();
	iMinIndex = iHour * 60 + iMin;
	iMinIndex = CorrectMinIndex(iMinIndex);

	iHour = iMinIndex / 60;
	iMin = iMinIndex % 60;

	switch (iKLineType)
	{
	case KLINE_TYPE_MINUTE5_DATA:
		iGap = 5 - iMinIndex % 5;
		break;
	case KLINE_TYPE_MINUTE15_DATA:
		iGap = 15 - iMinIndex % 15;
		break;
	case KLINE_TYPE_MINUTE30_DATA:
		iGap = 30 - iMinIndex % 30;
		break;
	case KLINE_TYPE_MINUTE60_DATA:
	case KLINE_TYPE_MINUTE120_DATA:
	case KLINE_TYPE_MINUTE240_DATA:
		//iGap = 60 - iMinIndex%60;
		break;
	default:
		return -1;
	}

	if (iGap == iKLineType && iMinIndex != 570 && iMinIndex != 780) //9点30和13点没有点
		iGap = 0;
	if (iKLineType == KLINE_TYPE_MINUTE60_DATA)
	{
		if (iMinIndex < 10 * 60 + 30)
		{
			iHour = 10;
			iMin = 30;
		}
		else if (iMinIndex >= 10 * 60 + 30 && iMinIndex <= 11 * 60 + 30)
		{
			iHour = 11;
			iMin = 30;
		}
		else if (iMinIndex >= 13 * 60 && iMinIndex <= 14 * 60)
		{
			iHour = 14;
			iMin = 0;
		}
		else
		{
			iHour = 15;
			iMin = 0;
		}
	}
	else if (iKLineType == KLINE_TYPE_MINUTE120_DATA)
	{
		if (iMinIndex < 13 * 60)
		{
			iHour = 11;
			iMin = 30;
		}
		else
		{
			iHour = 15;
			iMin = 0;
		}
	}
	else if (iKLineType == KLINE_TYPE_MINUTE240_DATA)
	{
		iHour = 15;
		iMin = 0;
	}
	else
	{
		if (iHour == 11 && iMin == 30)
		{
			iGap = 0;
		}

		iMin += iGap;
		if (iMin >= 60)		//进位
		{
			iHour++;
			iMin = 0;
		}
	}

	CTime ttRet(nYear, nMonth, nDay, iHour, iMin, 0);

	return ttRet.GetTime();
}

BOOL GetMinuteIndexFormat(int iMinIndex, int& iHour, int& iMin, int iKLineType, int iMarketId)
{
	BOOL	bIsNextDay = FALSE;
	switch (iKLineType)
	{
	case KLINE_TYPE_MINUTE5_DATA:
	case KLINE_TYPE_MINUTE15_DATA:
	case KLINE_TYPE_MINUTE30_DATA:
		switch (iMarketId)
		{
		case ROM_SYMBOL_TYPE_SH_FUTURE:
		case ROM_SYMBOL_TYPE_ZZ_FUTURE:
		case ROM_SYMBOL_TYPE_DL_FUTURE:
			if (iHour * 60 + iMin > 7 * 60 && iHour * 60 + iMin < 9 * 60)
			{
				iHour = 9;
				iMin = 0;
			}
			else if (iHour * 60 + iMin >= 15 * 60 && iHour * 60 + iMin < 16 * 60)
			{
				iHour = 14;
				iMin = 60 - iKLineType;
			}
			else if (iHour * 60 + iMin >= 11 * 60 + 30 && iHour * 60 + iMin < 13 * 60 + 30)
			{
				iHour = 13;
				iMin = 30;
			}
			else if (iHour * 60 + iMin >= 20 * 60 && iHour * 60 + iMin < 21 * 60)
			{
				iHour = 21;
				iMin = 0;
			}
			else if (iHour * 60 + iMin >= 2 * 60 + 30 && iHour * 60 + iMin < 5 * 60)
			{
				iHour = 2;
				iMin = 30 - iKLineType;
			}
			break;

		case ROM_SYMBOL_TYPE_ZJ_FUTURE:
			if (iHour * 60 + iMin < 9 * 60 + 15)
			{
				iHour = 9;
				iMin = 15;
			}
			else if (iHour * 60 + iMin >= 15 * 60 + 15)
			{
				iHour = 15; iMin = 15;
				iMin -= iKLineType;
				if (iMin < 0)
				{
					iHour--;
					iMin += 60;
				}
			}
			else if (iHour * 60 + iMin >= 11 * 60 + 30 && iHour * 60 + iMin < 13 * 60)
			{
				if (iKLineType == KLINE_TYPE_MINUTE30_DATA)
				{
					iHour = 11;
					iMin = 15;
				}
				else
				{
					iHour = 13;
					iMin = 0;
				}
			}
			break;
		}

		break;
	case KLINE_TYPE_MINUTE60_DATA:
		switch (iMarketId)
		{
		case ROM_SYMBOL_TYPE_SH_FUTURE:
		case ROM_SYMBOL_TYPE_ZZ_FUTURE:
		case ROM_SYMBOL_TYPE_DL_FUTURE:
			if (iMinIndex > 6 * 60 + 40 && iMinIndex < 10 * 60)
			{
				iHour = 9;
				iMin = 0;
			}
			else if (iMinIndex >= 10 * 60 && iMinIndex < 11 * 60)
			{
				iHour = 10;
				iMin = 0;
			}
			else if (iMinIndex >= 11 * 60 && iMinIndex < 14 * 60)
			{
				iHour = 11;
				iMin = 0;
			}
			else if (iMinIndex >= 14 * 60 && iMinIndex < 15 * 60 + 20)
			{
				iHour = 14;
				iMin = 0;
			}
			else if (iMinIndex > 20 * 60 + 30 && iMinIndex < 22 * 60)
			{
				iHour = 21;
				iMin = 0;
			}
			else if (iMinIndex >= 22 * 60 && iMinIndex < 23 * 60)
			{
				iHour = 22;
				iMin = 0;
			}
			else if (iMinIndex >= 23 * 60)
			{
				iHour = 23;
				iMin = 0;
			}
			else if (iMinIndex >= 0 * 60 && iMinIndex < 1 * 60)
			{
				iHour = 0;
				iMin = 0;
				//				bIsNextDay = TRUE;
								//夜盘，跨天
				/*				tt += CTimeSpan( 1, 0, 0, 0 );
								nYear = tt.GetYear();
								nMonth = tt.GetMonth();
								nDay = tt.GetDay();
				 */
			}
			else if (iMinIndex >= 1 * 60 && iMinIndex < 2 * 60)
			{
				iHour = 1;
				iMin = 0;
			}
			else if (iMinIndex >= 2 * 60 && iMinIndex <= 3 * 60)
			{
				iHour = 2;
				iMin = 0;
			}
			else
			{
				iHour = iMinIndex / 60;
				iMin = iMinIndex % 60;
			}
			break;

		case ROM_SYMBOL_TYPE_ZJ_FUTURE:
			if (iMinIndex > 6 * 60 + 40 && iMinIndex < 10 * 60 + 15)
			{
				iHour = 9;
				iMin = 15;
			}
			else if (iMinIndex >= 10 * 60 + 15 && iMinIndex < 11 * 60 + 15)
			{
				iHour = 10;
				iMin = 15;
			}
			else if (iMinIndex >= 11 * 60 + 15 && iMinIndex < 13 * 60 + 45)
			{
				iHour = 11;
				iMin = 15;
			}
			else if (iMinIndex >= 13 * 60 + 45 && iMinIndex < 14 * 60 + 45)
			{
				iHour = 13;
				iMin = 45;
			}
			else if (iMinIndex >= 14 * 60 + 45 && iMinIndex < 15 * 60 + 45)
			{
				iHour = 14;
				iMin = 45;
			}
			else
			{
				//异常数据，维持原样
				iHour = iMinIndex / 60;
				iMin = iMinIndex % 60;
			}
			break;

		default:
			break;
		}
		break;

	case KLINE_TYPE_MINUTE120_DATA:
		switch (iMarketId)
		{
		case ROM_SYMBOL_TYPE_SH_FUTURE:
		case ROM_SYMBOL_TYPE_ZZ_FUTURE:
		case ROM_SYMBOL_TYPE_DL_FUTURE:

			if (iMinIndex > 7 * 60 && iMinIndex <= 11 * 60 + 40)			//按半日线处理
			{
				iHour = 9;
				iMin = 0;
			}
			else if (iMinIndex > 11 * 60 + 40 && iMinIndex < 15 * 0 + 20)	//股指是15分收盘，允许20分钟延迟收市
			{
				iHour = 13;
				iMin = 30;
			}
			else if (iMinIndex > 20 * 60 + 50 && iMinIndex < 23 * 60)
			{
				iHour = 21;
				iMin = 0;
			}
			else if (iMinIndex >= 23 * 60 || iMinIndex < 1 * 60)
			{
				iHour = 23;
				iMin = 0;
				if (iMinIndex < 1 * 60)
					bIsNextDay = TRUE;
			}
			else if (iMinIndex >= 1 * 60 || iMinIndex <= 3 * 60)
			{
				iHour = 1;
				iMin = 0;
				//				bIsNextDay = TRUE;
								//夜盘，跨天
				/*				tt += CTimeSpan( 1, 0, 0, 0 );
								nYear = tt.GetYear();
								nMonth = tt.GetMonth();
								nDay = tt.GetDay();
				 */
			}
			else
			{
				//异常数据，维持原样
				iHour = iMinIndex / 60;
				iMin = iMinIndex % 60;
			}
			break;

		case ROM_SYMBOL_TYPE_ZJ_FUTURE:						//开市时间 上午9:15—11:30；    下午13:00—15:15 
			if (iMinIndex > 7 * 60 && iMinIndex <= 11 * 60 + 45)//按半日线处理
			{
				iHour = 9;
				iMin = 15;
			}
			else if (iMinIndex > 12 * 60 + 45 && iMinIndex <= 15 * 60 + 45)
			{
				iHour = 13;
				iMin = 0;
			}
			else
			{
				//异常数据，维持原样
				iHour = iMinIndex / 60;
				iMin = iMinIndex % 60;
			}
			break;

		}

		break;

	case KLINE_TYPE_MINUTE240_DATA:
		switch (iMarketId)
		{
		case ROM_SYMBOL_TYPE_SH_FUTURE:
		case ROM_SYMBOL_TYPE_ZZ_FUTURE:
		case ROM_SYMBOL_TYPE_DL_FUTURE:
			if (iMinIndex > 7 * 60 && iMinIndex <= 15 * 60 + 20)
			{
				iHour = 9;
				iMin = 0;
			}
			else if (iMinIndex > 20 * 60 + 40 || iMinIndex < 1 * 60)
			{
				iHour = 21;
				iMin = 0;
				if (iMinIndex < 1 * 60)
					bIsNextDay = TRUE;
			}
			else if (iMinIndex >= 1 * 60 && iMinIndex <= 3 * 60)
			{
				iHour = 1;
				iMin = 0;
				//				bIsNextDay = TRUE;
								//夜盘，跨天
				/*				tt += CTimeSpan( 1, 0, 0, 0 );
								nYear = tt.GetYear();
								nMonth = tt.GetMonth();
								nDay = tt.GetDay();
				 */
			}
			else
			{
				//异常数据，维持原样
				iHour = iMinIndex / 60;
				iMin = iMinIndex % 60;
			}
			break;

		case ROM_SYMBOL_TYPE_ZJ_FUTURE:		//开市时间 上午9:15—11:30；    下午13:00—15:15 
			if (iMinIndex > 7 * 60 && iMinIndex <= 15 * 60 + 20)
			{
				iHour = 9;
				iMin = 15;
			}
			else
			{
				//异常数据，维持原样
				iHour = iMinIndex / 60;
				iMin = iMinIndex % 60;
			}
			break;
		}

	}

	return bIsNextDay;
}

time_t GetMinuteKLineFormatTimeForFuture(time_t t, int iKLineType, int iMarkType)
{
	int nYear, nMonth, nDay, iHour, iMin, iGap, iMinIndex;
	int nMinIndexFormat = 0;
	CTime tt = CTime(t);
	nYear = tt.GetYear();
	nMonth = tt.GetMonth();
	nDay = tt.GetDay();
	iHour = tt.GetHour();
	iMin = tt.GetMinute();
	iMinIndex = iHour * 60 + iMin;

	if (iKLineType == KLINE_TYPE_MINUTE30_DATA && iMarkType == ROM_SYMBOL_TYPE_ZJ_FUTURE && iMinIndex >= 7 * 60 && iMinIndex < 16 * 60)
	{
		iMin += 15;
		int nGap = iMin % iKLineType;
		iMin -= iMin % iKLineType;

		iMin -= 15;
		if (iMin < 0)
		{
			iHour--;
			iMin += 60;
		}
		GetMinuteIndexFormat(iMinIndex, iHour, iMin, iKLineType, iMarkType);
	}
	else if (iKLineType != KLINE_TYPE_MINUTE60_DATA && iKLineType != KLINE_TYPE_MINUTE120_DATA &&
		iKLineType != KLINE_TYPE_MINUTE240_DATA)
	{
		int nGap = iMin % iKLineType;
		iMin -= iMin % iKLineType;
		GetMinuteIndexFormat(iMinIndex, iHour, iMin, iKLineType, iMarkType);
	}
	else if (iKLineType == KLINE_TYPE_MINUTE60_DATA || iKLineType == KLINE_TYPE_MINUTE120_DATA || iKLineType == KLINE_TYPE_MINUTE240_DATA)
	{
		if (GetMinuteIndexFormat(iMinIndex, iHour, iMin, iKLineType, iMarkType))
		{
			//夜盘，跨天
			tt -= CTimeSpan(1, 0, 0, 0);
			nYear = tt.GetYear();
			nMonth = tt.GetMonth();
			nDay = tt.GetDay();
		}
	}
	CTime ttRet(nYear, nMonth, nDay, iHour, iMin, 0);

	return ttRet.GetTime();
}



time_t GetMinuteKLineFormatTime(time_t t, int iKLineType, int iMarkType)
{
	if (IsFuture(iMarkType))
		return GetMinuteKLineFormatTimeForFuture(t, iKLineType, iMarkType);
	else
		return GetMinuteKLineFormatTimeForStock(t, iKLineType);
}

int GenerateMinutelyKLineBy1MinKLine(ROM_KLINE_DATA *p1MinKLine, int n1MinKLineCount, ROM_KLINE_DATA *pMinutelyKLine, int nMaxMinutelyKLineCount, int iKLineType, int iMarkType)
{
	if (p1MinKLine == NULL ||
		n1MinKLineCount <= 0 ||
		pMinutelyKLine == NULL ||
		nMaxMinutelyKLineCount <= 0)
		return 0;

	try
	{
		int i, nMinutelyKLineCount = 0;
		time_t tmOriginal, tmFormat, tmSaved;
		struct tm tt, *ptt;

		for (i = 0; i < n1MinKLineCount; i++)
		{
			tt.tm_year = p1MinKLine[i].date / 10000 - 1900;
			tt.tm_mon = p1MinKLine[i].date % 10000 / 100 - 1;
			tt.tm_mday = p1MinKLine[i].date % 100;
			tt.tm_hour = p1MinKLine[i].time / 3600;
			tt.tm_min = p1MinKLine[i].time % 3600 / 60;
			tt.tm_sec = 0;
			tmOriginal = mktime(&tt);

			if (nMinutelyKLineCount == 0)
			{
				memcpy(&pMinutelyKLine[nMinutelyKLineCount], &p1MinKLine[i], sizeof(ROM_KLINE_DATA));
				tmFormat = GetMinuteKLineFormatTime(tmOriginal, iKLineType, iMarkType);
				ptt = localtime(&tmFormat);
				pMinutelyKLine[nMinutelyKLineCount].time = ptt->tm_hour * 60 * 60 + ptt->tm_min * 60;
				nMinutelyKLineCount++;
			}
			else
			{
				//during the same minutes.
				tmFormat = GetMinuteKLineFormatTime(tmOriginal, iKLineType, iMarkType);
				tt.tm_year = pMinutelyKLine[nMinutelyKLineCount - 1].date / 10000 - 1900;
				tt.tm_mon = pMinutelyKLine[nMinutelyKLineCount - 1].date % 10000 / 100 - 1;
				tt.tm_mday = pMinutelyKLine[nMinutelyKLineCount - 1].date % 100;
				tt.tm_hour = pMinutelyKLine[nMinutelyKLineCount - 1].time / 3600;
				tt.tm_min = pMinutelyKLine[nMinutelyKLineCount - 1].time % 3600 / 60;
				tt.tm_sec = 0;
				tmSaved = mktime(&tt);
				if (tmSaved > 0)
				{
					if (tmFormat == tmSaved)
					{
						if (p1MinKLine[i].highprice > pMinutelyKLine[nMinutelyKLineCount - 1].highprice)
							pMinutelyKLine[nMinutelyKLineCount - 1].highprice = p1MinKLine[i].highprice;
						if (p1MinKLine[i].lowprice < pMinutelyKLine[nMinutelyKLineCount - 1].lowprice)
							pMinutelyKLine[nMinutelyKLineCount - 1].lowprice = p1MinKLine[i].lowprice;
						pMinutelyKLine[nMinutelyKLineCount - 1].closeprice = p1MinKLine[i].closeprice;
						pMinutelyKLine[nMinutelyKLineCount - 1].volume += p1MinKLine[i].volume;
						//						pMinutelyKLine[nMinutelyKLineCount-1].amount += p1MinKLine[i].amount;
						pMinutelyKLine[nMinutelyKLineCount - 1].amount = p1MinKLine[i].amount;		//此处是持仓量，不需要叠加，使用最新的值
						pMinutelyKLine[nMinutelyKLineCount - 1].date = p1MinKLine[i].date;
					}
					else //new minutes period is comming
					{
						nMinutelyKLineCount++;
						memcpy(&pMinutelyKLine[nMinutelyKLineCount - 1], &p1MinKLine[i], sizeof(ROM_KLINE_DATA));
						ptt = localtime(&tmFormat);
						pMinutelyKLine[nMinutelyKLineCount - 1].time = ptt->tm_hour * 60 * 60 + ptt->tm_min * 60;
					}
				}
			}

			////
			if (nMinutelyKLineCount >= nMaxMinutelyKLineCount) break;
		}
		// 		for (i = 0; i < nMinutelyKLineCount; i++)
		// 		{
		// 			TRACE("i = %d, volume = %I64d, amount = %I64d\r\n", i, pMinutelyKLine[i].volume, pMinutelyKLine[i].amount);
		// 		}
		return nMinutelyKLineCount;
	}
	catch (...)
	{
		return 0;
	}
}

//根据市场类型（股票，现货，期货）k线周期获取存储路径
//参数：CString &sFilePath			out返回的路径
//		char *sSymbol				in 代码		
//		BYTE exchange				in 交易所代码
//		int iKLineType				in k线周期
//		int iMarkType				in 市场代码
void GeneryMinuteKlinePath(CString &sFilePath, char *sSymbol, BYTE exchange, int iKLineType, int iMarkType)
{
	try
	{
		if (sSymbol[0] <= 0 || (exchange != 0 && exchange != 1)) return;

		GET_CURRENT_DIRECTORY(MAX_PATH, sFilePath.GetBuffer(MAX_PATH));
		sFilePath.ReleaseBuffer();
		sFilePath += "\\box_data\\";

		CString sKlineCycle, sMarkFolder, sNameExtend;
		GeneryCycleFoloderName(sKlineCycle, iKLineType);
		GeneryMinuteFolderName(sMarkFolder, exchange, iMarkType);
		GeneryMinuteExtend(sNameExtend, sSymbol, iKLineType);

		sFilePath += sKlineCycle;
		sFilePath += sMarkFolder;
		AutoCreateDir(sFilePath);
		sFilePath += sNameExtend;
	}
	catch (...)
	{
		sFilePath.Empty();
	}
}

//创建多分钟数据
//先读取老数据，再一个一个插入，若有数据则跳过
int GenerateManyMinutlyKLine(const CString& sFilePath, ROM_KLINE_DATA *romMinKlineData, int realCount, ROM_KLINE_DATA *pManyMiniteKlineData, int nMaxCount, int iKLineType, int iMarkType)
{
	int nAllHisMinKlineCount = 0;
	int nNeedWriteMinKlineCount = 0;
	ROM_KLINE_DATA * pKlineData = NULL;

	ZeroMemory(pManyMiniteKlineData, realCount * sizeof(ROM_KLINE_DATA));
	int nMinKlineCount = GenerateMinutelyKLineBy1MinKLine(romMinKlineData, realCount, pManyMiniteKlineData, nMaxCount, iKLineType, iMarkType);
	nAllHisMinKlineCount = ReadMiniteKline(sFilePath, pKlineData);
	//将当日的多分钟线插入对应的股票的历史多分钟线中。
	nNeedWriteMinKlineCount = InsertMinuteKlineData(pKlineData, nAllHisMinKlineCount, pManyMiniteKlineData, nMinKlineCount);
	if (nNeedWriteMinKlineCount > 0)
	{
		WriteWeekOrMonthOrMiniteData(sFilePath, pKlineData, nNeedWriteMinKlineCount);
	}

	//最后删除内存
	if (pKlineData)
	{
		delete[] pKlineData;
		pKlineData = NULL;
	}

	switch (iKLineType)
	{
	case KLINE_TYPE_MINUTE5_DATA:
		printf("5M=%2d, ", nMinKlineCount);
		break;
	case KLINE_TYPE_MINUTE15_DATA:
		printf("15M=%2d, ", nMinKlineCount);
		break;
	case KLINE_TYPE_MINUTE30_DATA:
		printf("30M=%2d, ", nMinKlineCount);
		break;
	case KLINE_TYPE_MINUTE60_DATA:
		printf("60M=%2d\n", nMinKlineCount);
		break;
	}

	return 0;
}

void ManyMinutesDataConversion(int rec_count,int exchange , std::string symbol, ROM_KLINE_DATA* kline,ROM_KLINE_DATA* outcome_kine,int expected_type)
{
	float n = 2;
	rec_count = FilterUnusualData(n, kline, rec_count);
	if (rec_count < 0)
		return;
	//在这里先把1分钟的成交量递减，再计算下面
	for (int i = rec_count - 1; i >= 1; i--)
	{
		kline[i].volume = kline[i].volume - kline[i - 1].volume;
		kline[i].amount = kline[i].amount - kline[i - 1].amount;
	}
	CString sFilePath;
	//根据1分钟K线生成5分钟K线
	GeneryMinuteKlinePath(sFilePath, (char*)symbol.c_str(), exchange, KLINE_TYPE_MINUTE5_DATA, ROM_SYMBOL_TYPE_BLOCK);
	GenerateManyMinutlyKLine(sFilePath, kline, rec_count, outcome_kine, rec_count, KLINE_TYPE_MINUTE5_DATA, ROM_SYMBOL_TYPE_BLOCK);

	for (int i = 0; i < rec_count; i++)
	{
		std::cout << kline[i].date << ","
			<< kline[i].time << ","
			<< kline[i].amount << ","
			<< kline[i].volume << ","
			<< kline[i].openprice << ","
			<< kline[i].closeprice << ","
			<< kline[i].highprice << ","
			<< kline[i].lowprice << std::endl;
	}
}
// ZipData.cpp: implementation of the CZipData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ZipData.h"
#include "zlib\zlib.h"

#include <string>
#include <iostream>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CZipData::CZipData()
{
	m_pZipMem = NULL;
	m_nZipLen = 0;
	m_nCapacity = 0;
}

CZipData::~CZipData()
{
	if(m_pZipMem && m_nCapacity > 0)
	{
		free(m_pZipMem);
		m_pZipMem = NULL;
		m_nCapacity = 0;
	}
}

BOOL CZipData::Init(int nDefLen)
{
	if(nDefLen <= 0)  return FALSE; 
	try
	{
		if(m_pZipMem && m_nCapacity > 0)
		{
			free(m_pZipMem);
			m_pZipMem = NULL;
			m_nCapacity = 0;
		}
		m_nCapacity = nDefLen;
		m_pZipMem = (byte *)malloc(m_nCapacity);
		return m_pZipMem != NULL;
	}	
	catch(...)
	{
		return FALSE;
	}
}

int CZipData::DoCompress(byte *buffer,int iLen)
{
	if(buffer == NULL || iLen <= 0) return 0;

	int iRetLen = 0;
	try
	{
		byte *dest = new byte[iLen];
		memset(dest, 0,iLen);
		uLongf destLen = (uLongf)iLen;
		// Ñ¹Ëõ
		if( compress(dest, &destLen, buffer, (uLongf)iLen) == Z_OK && destLen > 0)
		{
			if((sizeof(TransZipData) - sizeof(char) + destLen) <= iLen  )
			{
				TransZipData *pZip = (TransZipData *)buffer;
				pZip->m_nType = 0x01001000;
				pZip->m_lOrigLen = iLen;
				pZip->m_lZipLen  = (long)destLen;
				pZip->m_nAlignment = 0;
				memcpy(pZip->m_cData,(char*)dest,destLen);
				iRetLen = sizeof(TransZipData) - sizeof(char) + destLen;
			}
			else
				iRetLen = 0;
		}
		else
			iRetLen = 0;
		delete[] dest;
		return iRetLen;
	}
	catch(...)
	{
		return 0;
	}
}


int CZipData::OnUnCompress(byte *btOriginalBuf, int nOritinalLen, byte *&btDestBuf)
{
	if(btOriginalBuf == NULL || nOritinalLen <= sizeof(TransZipData)) return 0;

	TransZipData *pZip = (TransZipData *)btOriginalBuf;
	
	if(pZip->m_lOrigLen<=0 || pZip->m_nType != 0x1000 || pZip->m_lOrigLen > MAX_ZIP_MEM_LEN) return 0;

	if(btDestBuf) delete[] btDestBuf;
	btDestBuf = new byte[pZip->m_lOrigLen];	

	uLongf iRealLen=(uLong)pZip->m_lOrigLen;
	if( uncompress(btDestBuf, &iRealLen,(Bytef*)pZip->m_cData, (uLongf)pZip->m_lZipLen) == Z_OK &&
		iRealLen == pZip->m_lOrigLen)		
		return iRealLen;
	else
	{
		delete[] btDestBuf;
		btDestBuf = NULL;
		return 0;
	}
}

void CZipData::compress_file(char *sFileName)
{
	char sNewFileName[MAX_PATH];
	sprintf(sNewFileName, "%s.zip", sFileName);

	printf("compress file %s ...\n", sFileName);

	FILE *fpSource = fopen(sFileName, "rb");
	int iLen = 0, nRead = 0, nSum, nWroten;
	if (fpSource)	//file has been found.
	{
		fseek(fpSource, 0l, SEEK_END);
		iLen = ftell(fpSource);
		if (iLen > 0)
		{
			char *buffer = new char[iLen];
			fseek(fpSource, 0l, SEEK_SET);
			for (nSum = 0; nSum < iLen; nSum += nRead)
				nRead = fread(buffer + nSum, 1, iLen - nSum, fpSource);
			fclose(fpSource);

			int nZipLen = DoCompress((byte *)buffer, iLen);

			if (nZipLen > 0)
			{
				FILE *fpDest = fopen(sNewFileName, "wb");
				for (nSum = 0; nSum < nZipLen; nSum += nWroten)
					nWroten = fwrite(buffer + nSum, 1, nZipLen - nSum, fpDest);

				fclose(fpDest);
			}

			if (buffer) delete[] buffer;
		}
		else
			fclose(fpSource);
	}
	else
		std::cout << "File not found:" << sFileName <<std::endl;
}

void CZipData::uncompress_file(char *sFileName)
{
	if (sFileName == NULL) return;
	
	char sRawFileName[MAX_PATH];
	std::string _str_zip_file_name = std::string(sFileName);
	std::size_t _size_found = _str_zip_file_name.find(".zip");
	if (_size_found !=std::string::npos && _size_found > 0)
	{
		strcpy(sRawFileName,_str_zip_file_name.substr(0,_size_found).c_str());
		//printf("Uncompress file from %s to %s...\n", sFileName, sRawFileName);
		FILE *_fp_zip = fopen(sFileName, "rb");
		int _zip_len = 0, nRead = 0, nSum, nWroten;
		if (_fp_zip)	//file has been found.
		{
			fseek(_fp_zip, 0l, SEEK_END);
			_zip_len = ftell(_fp_zip);
			if (_zip_len > 0)
			{
				char *buffer = new char[_zip_len];
				fseek(_fp_zip, 0l, SEEK_SET);
				for (nSum = 0; nSum < _zip_len; nSum += nRead)
					nRead = fread(buffer + nSum, 1, _zip_len - nSum, _fp_zip);
				fclose(_fp_zip);

				byte *_raw_buffer = NULL;
				int _raw_len = OnUnCompress((byte *)buffer, _zip_len, _raw_buffer);
				if (_raw_len > 0 && _raw_buffer)
				{
					FILE *_fp_raw = fopen(sRawFileName, "wb");
					fwrite(_raw_buffer, 1, _raw_len, _fp_raw);
					fclose(_fp_raw);
					delete _raw_buffer;
					//std::cout << sRawFileName << " uncompressed OK." << std::endl;
				}
				else
					std::cout << sRawFileName << " uncompressed failed." << std::endl;
				if (buffer) delete[] buffer;
			}
			else
			{
				fclose(_fp_zip);
				std::cout << sRawFileName << " generated failed." << std::endl;
			}
		}
		else
			std::cout << sRawFileName << " generated failed." << std::endl;
	}
	else
	{
		std::cout << sFileName << " open failed." <<  std::endl;
	}
}

void CZipData::do_zip_file_in_dir(char *sPath, char *sFileExt,bool bCompress)
{
	if (sPath == NULL || sFileExt == NULL) return;

	int _file_count = 0 ,_tick1;
	CString strMatch;
	strMatch.Format("%s\\*.%s", sPath, sFileExt);
	CFileFind find;
	BOOL bFound = find.FindFile(strMatch);
	_tick1 = GetTickCount();
	while (bFound)
	{
		bFound = find.FindNextFile();
		if (!find.IsDirectory())
		{
			CString strFile = find.GetFilePath();
			if (bCompress)
				compress_file(strFile.GetBuffer(MAX_PATH));
			else
				uncompress_file(strFile.GetBuffer(MAX_PATH));
			strFile.ReleaseBuffer();
			_file_count++;
			std::cout << _file_count << ",";
		}
	}
	//std::cout << "Finished do_zip_file_in_dir(),time cost " << (GetTickCount() - _tick1) << std::endl;
}

void CZipData::do_unzip_and_remove(char *sPath, char *sFileExt)
{
	if (sPath == NULL || sFileExt == NULL) return;

	int _file_count = 0, _tick1;
	CString strMatch;
	strMatch.Format("%s\\*.%s", sPath, sFileExt);
	CFileFind find;
	BOOL bFound = find.FindFile(strMatch);
	_tick1 = GetTickCount();
	while (bFound)
	{
		bFound = find.FindNextFile();
		if (!find.IsDirectory())
		{
			CString strFile = find.GetFilePath();
			uncompress_file(strFile.GetBuffer(MAX_PATH));
			remove(strFile.GetBuffer(MAX_PATH));
			strFile.ReleaseBuffer();
			_file_count++;
			std::cout << ".";
		}
	}
	//std::cout << "Finished do_zip_file_in_dir(),time cost " << (GetTickCount() - _tick1) << std::endl;
}
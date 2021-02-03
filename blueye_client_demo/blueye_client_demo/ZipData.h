// ZipData.h: interface for the CZipData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZIPDATA_H__50777029_75CD_422E_B1C5_0D0021CECAA7__INCLUDED_)
#define AFX_ZIPDATA_H__50777029_75CD_422E_B1C5_0D0021CECAA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma pack(push, 1)

#define DEFAULT_ZIP_MEM_LEN			1024*1024	//1M
#define MAX_ZIP_MEM_LEN				10*DEFAULT_ZIP_MEM_LEN

//ѹ����ͷ��ʽ
struct TransZipData
{
	unsigned short	m_nType;		// ��������,��ΪRT_ZIPDATA

	short 			m_nAlignment;	// Ϊ��4�ֽڶ������ӵ��ֶ�

	long			m_lZipLen;		// ѹ����ĳ���
	long			m_lOrigLen;		// ѹ��ǰ�ĳ���
	char		    m_cData[1];		// ѹ���������
};

typedef struct  {
	int nId;
	int DataType;
	int nRecordCount;
	int nRecordDataLenght;
}ZIP_DATA_HEADER;

#pragma pack(pop)

class CZipData  
{
public:
	int OnUnCompress(byte *btOriginalBuf, int nOritinalLen, byte *&btDestBuf);
	int DoCompress(byte *buffer,int iLen);
	BOOL Init(int nDefLen = DEFAULT_ZIP_MEM_LEN);
	int			m_nCapacity;
	byte		*m_pZipMem;
	int			m_nZipLen;
	time_t		m_tmStamp;		//ѹ����ʱ���
	CZipData();
	virtual ~CZipData();

	//For compression and uncompression of file, the compressed file has a extension name appended.
	void compress_file(char *sFileName);
	void uncompress_file(char *sFileName);
	void do_zip_file_in_dir(char *sPath, char *sFileExt, bool bCompress);
	void do_unzip_and_remove(char *sPath, char *sFileExt);
};

#endif // !defined(AFX_ZIPDATA_H__50777029_75CD_422E_B1C5_0D0021CECAA7__INCLUDED_)

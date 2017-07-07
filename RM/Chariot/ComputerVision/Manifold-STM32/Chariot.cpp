// Chariot.cpp
// Version 1.0
// Started by Deyou Kong, 2017-06-02
// Checked by Deyou Kong, 2017-06-25

#include <iostream>
#include <fstream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>
#include <errno.h>
using namespace std;

#include "CConfig.h"

#include "Chariot.h"

CChariot::CChariot()
{
	iError = 0;

	// 加载配置文件

	CConfig cCfg("Run.cfg");

	if (cCfg.LoadFile())
	{
		string szFoutFile("./Runlog/Chariot");
		szFoutFile += cCfg.GetString("default", "iLogCounter") + ".log";

		cCfg.SetInteger("default", "iLogCounter", cCfg.GetInteger("default", "iLogCounter") + 1);
		cCfg.SaveFile();

		bTeam = cCfg.GetBoolean("default", "bIsRed", true);

		bFoutMore = cCfg.GetBoolean("Chariot", "bFoutMore");
		bCoutMore = cCfg.GetBoolean("Chariot", "bCoutMore");

		fout.open(szFoutFile);
	}
	else
	{
		bTeam = true; // 注意此处，默认情况下我方为红方
		//bTeam = false; // 注意此处，默认情况下我方为蓝方

		bFoutMore = false;
		bCoutMore =	false;

		fout.open("./Runlog/Chariot.log");
		fout << "Load config file error! " << endl;
		fout << "Program use default param. Default output in Chariot.log." << endl;

		iError = -1;
	}

	// 准备串口通讯

	fdSerial = open("/dev/ttyTHS1", O_RDWR | O_NOCTTY | O_NONBLOCK);
	perror("Open serial of \"/dev/ttyTHS1\"");
	fout << "Open serial of \"/dev/ttyTHS1\"" << endl;

	if (fdSerial == -1)
	{
		iError = 1;
		fout << "Open serial failed." << endl;
		return;
	}

	termios tOption;

	tcgetattr(fdSerial, &tOption);
	cfmakeraw(&tOption);
	cfsetispeed(&tOption, B9600); // 接收波特率
	cfsetospeed(&tOption, B9600); // 发送波特率
	tcsetattr(fdSerial, TCSANOW, &tOption);

	tOption.c_cflag &= ~PARENB;
	tOption.c_cflag &= ~CSTOPB;
	tOption.c_cflag &= ~CSIZE;
	tOption.c_cflag |= CS8;
	tOption.c_cflag &= ~INPCK;
	tOption.c_cflag |= (CLOCAL | CREAD);
	tOption.c_cflag &= ~(INLCR | ICRNL);
	tOption.c_cflag &= ~(IXON);
	tOption.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	tOption.c_oflag &= ~OPOST;
	tOption.c_oflag &= ~(ONLCR | OCRNL);
	tOption.c_iflag &= ~(ICRNL | INLCR);
	tOption.c_iflag &= ~(IXON | IXOFF | IXANY);
	tOption.c_cc[VTIME] = 0;
	tOption.c_cc[VMIN]  = 0;

	tcflush(fdSerial, TCIOFLUSH);

	// 初始化通讯帧结构体通用项

	tFrm.bySOF = 0x66; // 帧起始标识，固定为0x66
	tFrm.byEOF = 0x88; // 帧结束标识，固定为0x88
	tFrm.ui8Version = 1; // 当前协议版本号设置为1

	// 完成

	cout << "Preparation is successful." << endl;
	fout << "Preparation is successful." << endl;
}

CChariot::~CChariot()
{
	iError = 0;

	tcflush(fdSerial, TCIOFLUSH);

	if (-1 == close(fdSerial))
	{
		iError = -1;

		cout << "Close serial failed." << endl;
		fout << "Close serial failed." << endl;
	}

	fout.close();
}

int CChariot::GetLastError()
{
	return iError;
}

bool CChariot::IsError()
{
	// iError = 0: 正常
	// iError > 0: 发生严重错误，必须立即解决或异常退出
	// iError < 0: 发生不影响核心功能的错误

	if (iError > 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

int CChariot::GetSerial()
{
	return fdSerial;
}

bool CChariot::GetTeam()
{
	return bTeam;
}

void CChariot::ErrRst()
{
	iError = 0;
}

bool CChariot::SetLastError(int iError)
{
	if (abs(iError) <= 1000)
	{
		return false;
	}
	else
	{
		this->iError = iError;
	}

	return true;
}

void CChariot::ErrorOutput(string sz)
{
	cout << sz << endl;
	fout << sz << endl;
}

double CChariot::Timer()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return ((double(tv.tv_sec) * 1000) + (double(tv.tv_usec) / 1000));
}

bool CChariot::Send()
{
	iError = 0;

	static unsigned int uiFrmSeqSend = 0; // 当前发送帧序列号，该类所有对象会共享该值
	uiFrmSeqSend++;
	
	tFrm.ui16FrmSeq = uiFrmSeqSend;
	
	int iWrited = write(fdSerial, &tFrm, sizeof(TFrame));

	if (iWrited == -1)
	{
		cout << "Write data to serial failed. FrmSeq is " << uiFrmSeqSend << endl;
		fout << "Write data to serial failed. FrmSeq is " << uiFrmSeqSend << endl;

		iError = -2;

		return false;
	}
	else if (iWrited < sizeof(TFrame))
	{
		cout << "Occur error in writing data to serial. Need write: " << sizeof(TFrame)
			 << ", Real write: " << iWrited <<". FrmSeq is " << uiFrmSeqSend
			 << "\nIt may be serial output buffer has fulled." << endl;
		fout << "Occur error in writing data to serial. Need write: " << sizeof(TFrame)
			 << ", Real write: " << iWrited << ". FrmSeq is " << uiFrmSeqSend
			 << "\nIt may be serial output buffer has fulled." << endl;

		iError = -3;

		return false;
	}

	if (bFoutMore)
	{
		fout << "Success in writing data to serial. Writed: " << sizeof(TFrame)
			 << ". FrmSeq is " << uiFrmSeqSend << endl;
	}
	if (bCoutMore)
	{
		cout << "Success in writing data to serial. Writed: " << sizeof(TFrame)
			 << ". FrmSeq is " << uiFrmSeqSend << endl;
	}

	return true;
}

bool CChariot::Recv()
{
	iError = 0;

	static unsigned int uiFrmSeqRecv = 0; // 当前接收帧序列号，该类所有对象会共享该值

	int iReaded = 0;

	double dTimerStart = Timer();

	while (iReaded < sizeof(TFrame))
	{
		double dUsedTime = Timer() - dTimerStart;
		
		//cout << "Used time:" << dUsedTime << endl;
		
		if (dUsedTime > 100) // 超时限度100ms
		{
			cout << "Read data from serial timeout. Last FrmSeq is " << uiFrmSeqRecv << endl;
			fout << "Read data from serial timeout. Last FrmSeq is " << uiFrmSeqRecv << endl;

			iError = -5;

			return false;
		}

		int iOnceReaded = read(fdSerial, ((unsigned char *)(&tFrm)) + iReaded, sizeof(TFrame) - iReaded);

		if (iOnceReaded == -1)
		{
			if (errno == EAGAIN)
			{
				continue;
			}

			cout << "Read data from serial failed. Last FrmSeq is " << uiFrmSeqRecv << endl;
			fout << "Read data from serial failed. Last FrmSeq is " << uiFrmSeqRecv << endl;

			iError = -4;

			return false;
		}

		iReaded += iOnceReaded;
	}

	uiFrmSeqRecv = tFrm.ui16FrmSeq;
	
	if (bFoutMore)
	{
		fout << "Success in reading data to serial. Readed: " << sizeof(TFrame)
			 << ". FrmSeq is " << uiFrmSeqRecv << endl;
	}
	if (bCoutMore)
	{
		cout << "Success in reading data to serial. Readed: " << sizeof(TFrame)
			 << ". FrmSeq is " << uiFrmSeqRecv << endl;
	}

	return true;
}

void CChariot::SetData(uint8_t ui8Direction, uint16_t ui16Type, int iBytes, void * pData)
{
	// 确保pData数据长度小于56字节

	tFrm.tData.ui8Direction = ui8Direction;
	tFrm.tData.ui16Type     = ui16Type;

	memset(tFrm.tData.byData, 0, sizeof(tFrm.tData.byData));

	if ((iBytes > 0) && pData)
	{
		memcpy(tFrm.tData.byData, pData, iBytes);
	}
}

void CChariot::GetData(uint8_t * pui8Direction, uint16_t * pui16Type, int iBytes, void * pData)
{
	// 确保pData数据长度小于56字节

	if (pui8Direction)
	{
		(*pui8Direction) = tFrm.tData.ui8Direction;
	}

	if (pui16Type)
	{
		(*pui16Type) = tFrm.tData.ui16Type;
	}
	
	if ((iBytes > 0) && pData)
	{
		memcpy(pData, tFrm.tData.byData, iBytes);
	}
}

bool CChariot::CtrlMove(float fSpeedX, float fSpeedY, float fSpeedYaw)
{
	tCtrlChassis.ui8CtrlType = 0x00;
	tCtrlChassis.fX = fSpeedX;
	tCtrlChassis.fY = fSpeedY;
	tCtrlChassis.fYaw = fSpeedYaw;

	SetData(FD_DIR_MANI2STM, FD_TYP_CTRL_CHASSIS, sizeof(tCtrlChassis), &tCtrlChassis);
	
	return Send();
}

bool CChariot::CtrlHead(float fSpeedYaw, float fSpeedPitch)
{
	tCtrlHead.ui8CtrlType = 0x00;
	tCtrlHead.fYaw   = fSpeedYaw;
	tCtrlHead.fPitch = fSpeedPitch;

	SetData(FD_DIR_MANI2STM, FD_TYP_CTRL_HEAD, sizeof(tCtrlHead), &tCtrlHead);

	return Send();
}

bool CChariot::CtrlShoot(int iShoot, bool bIsStop)
{
	tCtrlShoot.ui8CtrlType = iShoot;
	tCtrlShoot.iStop = bIsStop;

	SetData(FD_DIR_MANI2STM, FD_TYP_CTRL_SHOOT, sizeof(tCtrlShoot), &tCtrlShoot);

	return Send();
}

bool CChariot::InfoRShoot(int iShoot, float * pfSpeed, float * pfFreq)
{
	SetData(FD_DIR_STM2MANI, FD_TYP_INFO_RSHOOT, 0, nullptr);

	tcflush(fdSerial, TCIFLUSH);

	if (!Send())
	{
		return false;
	}

	if (!Recv())
	{
		return false;
	}

	GetData(nullptr, nullptr, sizeof(tInfoRealShoot), &tInfoRealShoot);

	if (0 == iShoot) // 小子弹
	{
		if (pfSpeed)
		{
			(*pfSpeed) = tInfoRealShoot.fBulletSpeed;
		}

		if (pfFreq)
		{
			(*pfFreq) = tInfoRealShoot.fBulletFreq;
		}
	}
	else if (1 == iShoot) // 高尔夫
	{
		if (pfSpeed)
		{
			(*pfSpeed) = tInfoRealShoot.fGolfSpeed;
		}

		if (pfFreq)
		{
			(*pfFreq) = tInfoRealShoot.fGolfFreq;
		}
	}

	return true;
}

bool CChariot::SetCustdata(float f1, float f2, float f3)
{
	tCustdata.fCustomData1 = f1;
	tCustdata.fCustomData2 = f2;
	tCustdata.fCustomData3 = f3;

	SetData(FD_DIR_MANI2STM, FD_TYP_CUSTDATA, sizeof(tCustdata), &tCustdata);

	return Send();
}

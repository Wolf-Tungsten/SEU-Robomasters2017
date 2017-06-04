// Chariot.cpp
// Version 1.0
// Started by Deyou Kong, 2017-06-02
// Checked by Deyou Kong, 2017-06-04

#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>
using namespace std;

#include "CConfig.h"

#include "Chariot.h"

CChariot::CChariot()
{
	ErrRst();

	// 加载配置文件

	CConfig cCfg("Run.cfg");

	if (!cCfg.LoadFile())
	{
		string szFoutFile("./Runlog/Chariot");
		szFoutFile += cCfg.GetString("default", "iLogCounter") + ".log";

		cCfg.SetInteger("default", "iLogCounter", cCfg.GetInteger("default", "iLogCounter") + 1);
		cCfg.SaveFile();

		bFoutMore = cCfg.GetBoolean("Chariot", "bFoutMore");
		bCoutMore = cCfg.GetBoolean("Chariot", "bCoutMore");

		fout.open(szFoutFile);
	}
	else
	{
		bFoutMore = false;
		bCoutMore =	false;

		fout.open("./Runlog/Chariot.log");
		fout << "Load config file error! " << endl;
		fout << "Program use default param. Default output in Chariot.log." << endl;

		iError = -1;
	}

	// 准备串口通讯

	fdSerial = open("/dev/ttyTHS1", O_RDWR | O_NOCTTY | O_NDELAY);
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
	cfsetispeed(&tOption, B9600);
	cfsetospeed(&tOption, B9600);
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

	tFrm.bySOF = 0x66;
	tFrm.byEOF = 0x88;
	tFrm.ui8Version = 1;

	// 完成

	cout << "Preparation is successful." << endl;
	fout << "Preparation is successful." << endl;
}

CChariot::~CChariot()
{
	ErrRst();

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

bool CChariot::Send()
{
	static unsigned int uiFrmSeqSend = 0; // 当前发送帧序列号，该类所有对象会共享该值
	uiFrmSeqSend++;
	
	tFrm.ui16FrmSeq = uiFrmSeqSend;
	
	int iWrited = write(fdSerial, &tFrm, sizeof(TFrame));

	if (iWrited == -1)
	{
		cout << "Write data to serial failed. FrmSeq is " << uiFrmSeqSend << endl;
		fout << "Write data to serial failed. FrmSeq is " << uiFrmSeqSend << endl;

		return false;
	}
	else if (iWrited < sizeof(TFrame))
	{
		cout << "Occur error in writing data to serial. Need write: " << sizeof(TFrame)
			 << ", Real write: " iWrited <<". FrmSeq is " << uiFrmSeqSend
			 << "\nIt may be serial output buffer has fulled." << endl;
		fout << "Occur error in writing data to serial. Need write: " << sizeof(TFrame)
			 << ", Real write: " iWrited << ". FrmSeq is " << uiFrmSeqSend
			 << "\nIt may be serial output buffer has fulled." << endl;

		return false;
	}

	if (bFoutMore)
	{
		fout << "Success in writing data to serial. FrmSeq is " << uiFrmSeqSend << endl;
	}
	if (bCoutMore)
	{
		cout << "Success in writing data to serial. FrmSeq is " << uiFrmSeqSend << endl;
	}

	return true;
}

bool CChariot::Recv()
{
	static unsigned int uiFrmSeqRecv = 0; // 当前接收帧序列号，该类所有对象会共享该值

	int iReaded = 0;

	double dTimerStart = Timer();

	while (iReaded < sizeof(TFrame))
	{
		if (Timer() - dTimerStart > 100) // 超时限度100ms
		{
			cout << "Read data from serial timeout. Last FrmSeq is " << uiFrmSeqRecv << endl;
			fout << "Read data from serial timeout. Last FrmSeq is " << uiFrmSeqRecv << endl;

			return false;
		}

		int iOnceReaded = read(fdSerial, ((unsigned char *)(&tFrm)) + iReaded, sizeof(TFrame) - iReaded);

		if (iOnceReaded == -1)
		{
			cout << "Read data from serial failed. Last FrmSeq is " << uiFrmSeqRecv << endl;
			fout << "Read data from serial failed. Last FrmSeq is " << uiFrmSeqRecv << endl;

			return false;
		}

		iReaded += iOnceReaded;

		usleep(1); // 轻微延时
	}

	uiFrmSeqRecv = tFrm.ui16FrmSeq;

	return true;
}

double CChariot::Timer()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return ((double(tv.tv_sec) * 1000) + (double(tv.tv_usec) / 1000));
}


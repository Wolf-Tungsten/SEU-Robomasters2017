// Chariot.h
// Version 1.0
// Started by Deyou Kong, 2017-05-29
// Checked by Deyou Kong, 2017-06-25

#pragma once

#include <fstream>
using namespace std;

class CChariot
{
#define TEAM_BLUE false
#define TEAM_RED  true

#define FD_DIR_MANI2STM 0
#define FD_DIR_STM2MANI 1

#define FD_TYP_CTRL_CHASSIS 0x00
#define FD_TYP_CTRL_HEAD    0x01
#define FD_TYP_CTRL_SHOOT   0x02
#define FD_TYP_INFO_GAME    0x10
#define FD_TYP_INFO_RBLOOD  0x11
#define FD_TYP_INFO_RSHOOT  0x12
#define FD_TYP_CUSTDATA     0x90

public:

	CChariot();
	~CChariot();

	int GetLastError();
	bool IsError();

	// 功能函数
	
	// 底盘控制
	bool CtrlMove(float fSpeedX, float fSpeedY, float fSpeedYaw = 0);

	// 云台控制
	bool CtrlHead(float fSpeedYaw, float fSpeedPitch);

	// 射击
	bool CtrlShoot(int iShoot, bool bIsStop); // 0：单发，1：连发

	// 实时射击信息
	bool InfoRShoot(int iShoot, float * pfSpeed, float * pfFreq); // 0：小子弹，1：高尔夫

	// 设置自定义数据
	bool SetCustdata(float f1, float f2, float f3);


protected:

	int GetSerial();

	bool GetTeam();

	void ErrRst();
	bool SetLastError(int iError);

	void ErrorOutput(string sz);

	double Timer();

	bool Send();
	bool Recv();

	void SetData(uint8_t ui8Direction, uint16_t ui16Type, int iBytes, void * pData);
	void GetData(uint8_t * pui8Direction, uint16_t * pui16Type, int iBytes, void * pData);

private:

	int fdSerial;
	int iError;

	bool bTeam;

	ofstream fout;
	
	bool bFoutMore; // 为true时输出详细的记录至文件
	bool bCoutMore; // 为true时输出详细的记录至控制台

#pragma pack(push, 1)

	// 战车Manifold-STM32传输通用帧结构体
	struct TFrame
	{
		typedef unsigned char byte;

		byte bySOF; // [SOF] 帧起始标识（Start of Frame），固定为0x66

		uint8_t  ui8Version; // [VER] 协议版本号
		uint16_t ui16FrmSeq; // [SEQ] 当前帧序列号

		struct TData
		{
			uint8_t  ui8Direction; // [DIR] 数据流向
			uint16_t ui16Type;     // [TYP] 数据类别

			byte byData[56]; // [DATA] 详细数据内容，长度固定为56，有效长度由TData*决定
		} tData;

		byte byEOF;// [EOF] 帧结束标识（End of Frame），固定为0x88
	} tFrm;
	// 战车Manifold-STM32传输协议：
	// | <-- 帧头段 ---> | <-- 帧数据段 --> | <- 帧尾段 -> |
	// | SOF | VER | SEQ | DIR | TYP | DATA |      EOF     |
	// |   8 |   8 |  16 |   8 |  16 |  448 |        8     | Bits: 512
	// |   1 |   1 |   2 |   1 |   2 |   56 |        1     | Bytes: 64

	// 底盘控制 <DIR: FD_DIR_MANI2STM(=0), TYP: FD_TYP_CTRL_CHASSIS(=0x00)>
	struct TDataControlChassis
	{
		// 0x00：速度控制
		uint8_t ui8CtrlType;
		float fX;
		float fY;
		float fYaw;
	} tCtrlChassis;

	// 云台控制 <DIR: FD_DIR_MANI2STM(=0), TYP: FD_TYP_CTRL_HEAD(=0x01)>
	struct TDataControlHead
	{
		// 0x00：速度控制
		uint8_t ui8CtrlType;
		float fYaw;   // 云台绕Z轴左右偏角
		float fPitch; // 云台俯仰角
	} tCtrlHead;

	// 发射控制 <DIR: FD_DIR_MANI2STM(=0), TYP: FD_TYP_CTRL_SHOOT(=0x02)>
	struct TDataControlShoot
	{
		// 0x00：单发
		// 0x01：连发
		uint8_t ui8CtrlType;
		int iStop; // 仅用于连发，开始连发为0，终止连发为1
	} tCtrlShoot;

	// 比赛进程信息 <DIR: FD_DIR_STM2MANI(=1), TYP: FD_TYP_INFO_GAME(=0x10)> [保留]
	struct TDataInformationGame
	{
		uint32_t ui32RemainTime;
		uint16_t ui16RemainLife;
		float fRealChassisOutV;
		float fRealChassisOutA;

		struct TDataInformationLocation
		{
			uint8_t ui8Valid;
			uint32_t ui32X;
			uint32_t ui32Y;
			uint32_t ui32Z;
			uint32_t ui32Compass;
		} tLocation; // 当前位置信息

		float fRemainPower;
	} tInfoGame;

	// 实时血量信息 <DIR: FD_DIR_STM2MANI(=1), TYP: FD_TYP_INFO_RBLOOD(=0x11)> [保留]
	struct TDataInformationRealBlood
	{
		uint8_t ui8ArmorWeakID : 4;
		uint8_t ui8FuncBlood   : 4;
		uint16_t ui16BloodChangeValue;
	} tInfoRealBlood;

	// 实时射击信息 <DIR: FD_DIR_STM2MANI(=1), TYP: FD_TYP_INFO_RSHOOT(=0x12)>
	struct TDataInformationRealShoot
	{
		float fBulletSpeed;
		float fBulletFreq;
		float fGolfSpeed;
		float fGolfFreq;
	} tInfoRealShoot;

	// 自定义数据 <DIR: FD_DIR_MANI2STM(=0), TYP: FD_TYP_CUSTDATA(=0x90)>
	struct TDataCustom
	{
		float fCustomData1;
		float fCustomData2;
		float fCustomData3;
	} tCustdata;

#pragma pack(pop)
};

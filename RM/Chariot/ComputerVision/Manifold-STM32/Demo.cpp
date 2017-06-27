// Demo.cpp
// Version 1.0
// Started by Deyou Kong, 2017-06-26
// Checked by Deyou Kong, 2017-06-26

#include <iostream>
#include <unistd.h>
using namespace std;

#include "ChariotEx.h"

#define DBG_BASIC   0
#define DBG_BASE    1
//#define DBG_HERO    2
//#define DBG_SOLDIER 3
//#define DBG_ENGINER 4

int main()
{
	#ifdef DBG_BASE
	CChariotBase cChariot;
	#endif
	
	#ifdef DBG_HERO
	CChariotHero cChariot;
	#endif
	
	#ifdef DBG_SOLDIER
	CChariotSoldier cChariot;
	#endif
	
	#ifdef DBG_ENGINER
	CChariotEnginer cChariot;
	#endif
	
	#ifdef DBG_BASIC
	cout << "CChariot:" << endl;
	// bool CtrlMove(float fSpeedX, float fSpeedY, float fSpeedYaw = 0);
	// bool CtrlHead(float fSpeedYaw, float fSpeedPitch);
	// bool CtrlShoot(int iShoot, bool bIsStop);
	// bool InfoRShoot(int iShoot, float * pfSpeed, float * pfFreq);
	// bool SetCustdata(float f1, float f2, float f3);
	cout << "CtrlMove ret:" << cChariot.CtrlMove(12372.4597213, -2.0, 20) << endl;
	sleep(2);
	cout << "CtrlHead ret:" << cChariot.CtrlHead(20, -5) << endl;
	sleep(2);
	cout << "CtrlShoot ret:" << cChariot.CtrlShoot(0, true) << endl;
	sleep(2);
	float f1 = -1, f2 = -1;
	cout << "InfoRShoot ret:" << cChariot.InfoRShoot(1, &f1, &f2) << ", fSpeed:" << f1 << ", fFreq:" << f2 << endl;
	sleep(2);
	cout << "SetCustdata ret:" << cChariot.SetCustdata(1.0, -1.0, 3.1) << endl;
	sleep(2);
	#endif
	
	#ifdef DBG_BASE
	cout << "CChariotBase:" << endl;
	// bool Scanning();
	cout << "Scanning ret:" << cChariot.Scanning() << endl;
	#endif
	
	#ifdef DBG_HERO
	cout << "CChariotHero:" << endl;
	#endif
	
	#ifdef DBG_SOLDIER
	cout << "CChariotSoldier:" << endl;
	#endif
	
	#ifdef DBG_ENGINER
	cout << "CChariotEnginer:" << endl;
	#endif
}

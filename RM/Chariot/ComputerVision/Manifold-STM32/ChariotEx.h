// ChariotEx.h
// Version 1.0
// Started by Deyou Kong, 2017-06-25
// Checked by Deyou Kong, 2017-06-25

#pragma once

#include "Chariot.h"

class CChariotBase : public CChariot
{
public:
	bool Scanning();
};

class CChariotHero : public CChariot
{
};

class CChariotSoldier : public CChariot
{
};

class CChariotEnginer : public CChariot
{
};

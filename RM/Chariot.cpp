// Chariot.cpp
// Version 1.0
// Started by Deyou Kong, 2017-06-02
// Checked by Deyou Kong, 2017-06-02

#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
using namespace std;

#include "Chariot.h"

CChariot::CChariot()
{
	fout.open();
}
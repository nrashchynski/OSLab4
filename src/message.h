#ifndef MESSAGE_H
#define MESSAGE_H

#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <limits>

const int MAX_MESSAGE_LENGTH = 20;

struct Message {
	char text[MAX_MESSAGE_LENGTH];
};

#endif  // MESSAGE_H
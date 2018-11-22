/*
CSE 109: Fall 2018
Elliot Scribner
ejs320
This program uses a server and client to solve a map using an algorithm and potentially some items
Program #7
*/

#pragma once

#include<cstdio>
#include<cstdlib>
#include<iostream>
#include<string>
#include<cerrno>
#include<cstring>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<signal.h>
#include<algorithm>
#include<netdb.h>
#include<thread>
#include<random>

using namespace std;


class client {
public:
	int readPortFile(const string& fileName);
	int safeRead(int fd, unsigned char* buffer, int toRead);



private:

};
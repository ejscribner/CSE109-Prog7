/*
CSE 109: Fall 2018
Elliot Scribner
ejs320
This program uses a server and client to solve a map using an algorithm and potentially some items
Program #7
*/

#include "Client.h"
#include <fcntl.h>
//#include <string.h>
#include <unistd.h>
#include <iostream>


using namespace std;

int main(int argc, char** argv) {
	cout << "Program Starting" << endl;
	int result = readPortFile("connection.txt");
	cout << "file read: " <<  result << endl;
	return 0;
}


int safeRead(int fd, unsigned char* buffer, int toRead)
{
	ssize_t readResult = 0;
	ssize_t haveRead = 0;
	while ((readResult = read(fd, buffer + haveRead, toRead)) != -1)
	{
		haveRead += readResult;
		toRead -= readResult;
		if (toRead == 0)
		{
			break;
		}
	}
	return haveRead;
}

int readPortFile(const string& fileName) {
	int fd = open(fileName.c_str(), O_RDONLY);
	if(fd == -1) {
		cerr << "**Error: could not open file" << endl;
		return -1;
	}
	ssize_t toRead = 2;
	short port = 0;
	unsigned char* buffer = (unsigned char*) &port;

	ssize_t readResult = 0;
	readResult = safeRead(fd, buffer, toRead);
//	cout << "Have Read: " << haveRead << endl;
	cout << "Port: " << port << endl;
	cout << "Bytes Read: " << readResult << endl;

	if(readResult < toRead) {
		cerr << "**Error: could not read port number";
		return -1;
	}
	//read length of host ** outputting 10000, still starting at beginning
	toRead = 8;
	size_t hostLength = 0;
	buffer = (unsigned char*) &hostLength;
	readResult = safeRead(fd, buffer, toRead);
//	cout << "Have Read: " << haveRead << endl;
	cout << "Host Length: " << hostLength << endl;
	cout << "Bytes Read: " << readResult << endl;

	if(readResult < toRead) {
		close(fd);
		cerr << "**Error: could not read hostname length";
		return -1;
	}

	//read hostname
	toRead = hostLength;
	char* name = NULL;
	name = (char*) malloc((hostLength + 1) * sizeof(char));
	buffer = (unsigned char*) name;
	readResult = safeRead(fd, buffer, toRead);
	cout << "Host Name: " << name << endl;
	cout << "Bytes Read: " << readResult << endl;

	if(readResult < toRead) {
		close(fd);
		cerr << "**Error: could not read hostname";
		return -1;
	}


	return readResult; //** Change later
}
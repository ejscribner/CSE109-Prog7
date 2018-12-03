/*
CSE 109: Fall 2018
Elliot Scribner
ejs320
This program uses a server and client to solve a map using an algorithm and potentially some items
Program #7
*/

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <stdint.h>
#include <vector>
#include <string>
#include "Client.h"

using namespace std;
//int readPortFile(const string& fileName);
int main(int argc, char** argv) {
	serverConnection* myConnection = new serverConnection();
	cout << "Program Starting" << endl;
	int result = myConnection->readPortFile("connection.txt");
	cout << "file read: " <<  result << endl;

	cout << "Port: " << myConnection->port << endl;
	cout << "Host Length: " << myConnection->hostLength << endl;
	cout << "Host Name: " << myConnection->name << endl;


	//socket/connection stuff
	int mySocket;
	if((mySocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Client failed to create socket");
		return 1;
	}

	sockaddr_in clientAddr;
	memset(&clientAddr, 0, sizeof(sockaddr_in));
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(myConnection->port);

	cout << "Trying to connect to " << myConnection->name << endl;
	hostent* host = gethostbyname(myConnection->name); //may need c_str but who knows
	if(host == NULL) {
		cerr << "**Error: host not found" << endl;
		return 1;
	}

	memmove(&(clientAddr.sin_addr.s_addr), host->h_addr_list[0], 4);

	int connection = connect(mySocket, (sockaddr*)&clientAddr, sizeof(sockaddr_in));
	if(connection == -1) {
		perror("Could not connect");
		return 1;
	}

	cout << "Successfully Connected to " << myConnection->name << endl;

	string map = printMap(connection, mySocket);
	char* command = (char*)malloc(64);//what size should this be?
	//loading
	sprintf(command, "L %d %d %d", 10,10,10);
	size_t commandLength = strlen(command);
	unsigned char* buffer = (unsigned char*)&commandLength;

	safeWrite(mySocket, buffer, 8);
	buffer = (unsigned char*)command;
	safeWrite(mySocket, buffer, commandLength);

	map = printMap(connection, mySocket);
	cout <<  map << endl;


	//can be used for user input

	while(true) {
		string myInput;
		cin >> myInput;
		move(myInput, command, commandLength, buffer, mySocket, map, connection);
	}



	bool running = true;
	char direction = 'E';
	while(running) {
		direction = turnLeft(direction);
		vector<char> possDirections = vector<char>(4);
		ssize_t parseIndex;
		parseIndex = map.find("You can go ");
		while (parseIndex != -1) {
			char parseDirection = map[parseIndex+12];
			parseIndex+=12;
			possDirections.push_back(parseDirection);
			cout << "possible direction: " << parseDirection << endl;
			parseIndex = map.find("You can go ", parseIndex);
		}
		running = false;

	}

	return 0;
}

void move(string s, char* command, size_t commandLength, unsigned char* buffer, int mySocket, string map, int connection) {
	sprintf(command, "%s", s.c_str());
	commandLength = strlen(command);
	buffer = (unsigned char*)&commandLength;

	safeWrite(mySocket, buffer, 8);
	buffer = (unsigned char*)command;
	safeWrite(mySocket, buffer, commandLength);

	map = printMap(connection, mySocket);
	cout <<  map << endl;
}

char turnLeft(char direction) {
	if(direction == 'N') {
		return 'W';
	} else if(direction == 'E') {
		return 'N';
	} else if(direction == 'S') {
		return 'E';
	} else if(direction == 'W') {
		return 'S';
	} else {
		return 'Z';
	}
}

char turnRight(char direction) {
	if(direction == 'N') {
		return 'E';
	} else if(direction == 'E') {
		return 'S';
	} else if(direction == 'S') {
		return 'W';
	} else if(direction == 'W') {
		return 'N';
	} else {
		return 'Z';
	}
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

int safeWrite(int fd, const unsigned char* buffer, int toWrite)
{
	ssize_t writeResult = 0;
	ssize_t haveWritten = 0;
	while ((writeResult = write(fd, buffer + haveWritten, toWrite)) != -1)
	{
		haveWritten += writeResult;
		toWrite -= writeResult;
		if (toWrite == 0)
		{
			break;
		}
	}

	return haveWritten;
}

serverConnection::serverConnection()
: port(0), hostLength(0)
{
	this->name = NULL;
	this->name = (char*) malloc((hostLength + 1) * sizeof(char));
}

int serverConnection::readPortFile(const string& fileName) {
	int fd = open(fileName.c_str(), O_RDONLY);
	if(fd == -1) {
		cerr << "**Error: could not open file" << endl;
		return -1;
	}
	ssize_t toRead = 2;
	unsigned char* buffer = (unsigned char*) &this->port;

	ssize_t readResult = 0;
	readResult = safeRead(fd, buffer, toRead);

	if(readResult < toRead) {
		cerr << "**Error: could not read port number";
		return -1;
	}
	toRead = 8;
	buffer = (unsigned char*) &this->hostLength;
	readResult = safeRead(fd, buffer, toRead);

	if(readResult < toRead) {
		close(fd);
		cerr << "**Error: could not read hostname length";
		return -1;
	}

	//read hostname
	toRead = hostLength;

	buffer = (unsigned char*) this->name;
	readResult = safeRead(fd, buffer, toRead);

	if(readResult < toRead) {
		close(fd);
		cerr << "**Error: could not read hostname";
		return -1;
	}


	return readResult;
}


int readAll(int socket, void* buffer, size_t buffLength) {
	unsigned char* pbuff = reinterpret_cast<unsigned char*>(buffer);
	while(buffLength > 0) {
		int numRead = read(socket, pbuff, buffLength);
		if(numRead < 0) {
			return -1;
		}
		if(numRead == 0) {
			return 0; //figure out error case return ##s
		}
		pbuff += numRead;
		buffLength -= numRead;
	}
	return 1;
}
int readAll2(int socket, void* buffer, size_t buffLength) {
	cout << "1" << endl;
	unsigned char* pbuff = reinterpret_cast<unsigned char*>(buffer);
	cout << "2" << endl;
	while(buffLength > 0) {
		cout << "3" << endl;
		cout << "	--socket: " << socket << endl;
		cout << "	--pbuff: " << pbuff << endl;
		cout << "	--buffLength: " << buffLength<< endl;
		int numRead = read(socket, pbuff, buffLength);//hangup on call w/ buffLength 8?
		cout << "4" << endl;
		if(numRead < 0) {
			return -1;
		}
		if(numRead == 0) {
			return 0; //figure out error case return ##s
		}
		pbuff += numRead;
		buffLength -= numRead;
	}
	return 1;
}

string printMap(int connection, int mySocket) {
	char* mapData = NULL;
	size_t dataSize = 0; //change type??
	const string command = "command> ";
	bool keepLooping = true;
	string map = "";
	do{
		if(readAll(mySocket, &dataSize, sizeof(dataSize)) <= 0) { //using sizeof(dataSize)?
			cerr << "**Error: could not read text size" << endl;
			return NULL;
		}

		if(dataSize == 0) {
			continue;
		}

		mapData = (char*)malloc(dataSize+1);

		if(readAll(mySocket, mapData, dataSize) <= 0) {
			cerr << "**Error: could not read test" << endl;
			return NULL;
		}
		mapData[dataSize] = '\0';
		//cout.write(mapData, dataSize);
		keepLooping = (dataSize != command.size()) || (strncmp(mapData, command.c_str(), command.size()) != 0);
		map += mapData;
		free(mapData);

	} while(keepLooping);
	//cout << flush
	return map;
}



int printMap2(int connection, int mySocket) {
	char* mapData = NULL;
	size_t dataSize = 0; //change type??
	const string command = "command> ";
	bool keepLooping = true;

	do{

		//read size?
		int res = readAll2(mySocket, &dataSize, sizeof(size_t));
		cout << "res: " << res << endl;
		if(res <= 0) { //using sizeof(dataSize)?
			cerr << "**Error: could not read text size" << endl;
			return 1;
		}

		cout << "here " << endl;

		if(dataSize == 0) {
			continue;
		}

		mapData = new char[dataSize];
		//read data?
		if(readAll2(mySocket, mapData, dataSize) <= 0) {
			cerr << "**Error: could not read text" << endl;
			delete[] mapData;
			return 1;
		}

		cout.write(mapData, dataSize);
		keepLooping = (dataSize != command.size()) || (strncmp(mapData, command.c_str(), command.size()) != 0);

		delete[] mapData;

	} while(keepLooping);
	cout << flush;
	return 0;
}


//int printMap(int connection, int mySocket) {
//	char* mapData = NULL;
//	string command = "command> ";
//	size_t dataSize = 0;
//	while(mapData != command.c_str()) {
////		cout << "	##DATA SIZE: " << dataSize << endl;
//		unsigned char* buffer = (unsigned char*) &dataSize;
//		connection = read(mySocket, buffer, 8);
//		if(connection == -1 || connection < 0) {
//			cerr << "**Error: could not read text size" << endl;
//			return 1;
//		}
//
//
//		cout << "	--DATA SIZE: " << dataSize << endl;
////		cout << "We read: " << connection << endl;
//
//		mapData = (char*)malloc(dataSize);
//		memset(mapData, 0, dataSize);
//		buffer = (unsigned char*) mapData;
////		cout << "printing map" << endl;
//
//		while((connection = read(mySocket, buffer, dataSize)) != -1) {
//			if(connection == -1 || connection < 0) {
//				cerr << "**Error: could not read text size" << endl;
//				return 1;
//			}
//			if(dataSize != 1) {
//				cout << buffer;
//			}
////			memset(&mapData, 0, dataSize);
//			free(buffer);
//			buffer = NULL;
//		}
//	}
//	return 0;
//}


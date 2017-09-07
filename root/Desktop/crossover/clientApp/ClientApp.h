#include <iostream>
#include <string>
#include <thread>
#include <map>
#include <vector>
#include <time.h>
#include "binn.h"
#include "PerformanceData.h"
#include <ctime>
#include <thread>
#include <map>
#include <vector>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <cstdarg>

#include <thread>
#define INET6_ADDRSTRLEN 65

#define CHUNK_SIZE 512

typedef struct data
{
	std::string utctime;
	std::string time;
	std::string attribute;
	std::string value;

} PerfData;

void SendDataHandler(int sockfd);
void sendObject(int sockfd);
std::string getAck(int sockfd);
// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

int PrintMessage(std::string format, ...)
{
	int done = 0;
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%d-%m-%Y %I:%M:%S", timeinfo);

	va_list arg;
	std::string strTime(buffer);
	std::string newFormat = "[CLIENT : " + strTime + "]\t" + format + "\n";
	va_start(arg, format.c_str());
	//done = vfprintf(stdout, format.c_str(), arg);
	done = vfprintf(stdout, newFormat.c_str(), arg);
	va_end(arg);

	return done;
}
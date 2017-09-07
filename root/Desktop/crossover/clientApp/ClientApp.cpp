// ClientApp.cpp : Defines the entry point for the console application.
//

#include "ClientApp.h"


int my_get_socket_error(int fd)
{
	char err_code;
	socklen_t len = sizeof(err_code);

	if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &err_code, &len) != 0)
		err_code = errno;
	else
		errno = err_code;
	return err_code;
}

void SendMemoryPerformanceCounterData(int sockfd)
{
	

	// add values to it
	char chUTC[] = "utctime";
	char chTime[] = "time";
	char chAttribute[] = "attribute";
	char chValue[] = "value";
    
	std::string strTime(currentDateTime());
    char timeBuf[512] = { 0 };
    snprintf(timeBuf,512,"%s",strTime.c_str());

	std::time_t utctime = std::time(0);
	char bufutc[11] = { 0 };
	snprintf(bufutc, 11,"%ld", utctime);
	std::string strUTC(bufutc);

	PerfData pMemory;
	double memoryUsage = PerformanceData::get().GetMemoryUsage();
	char chusage[24] = { 0 };
	snprintf(chusage, 24, "%3.2f", memoryUsage);
	std::string strUsage(chusage);
	
	pMemory.utctime = strUTC;
	pMemory.time = strTime;
	pMemory.attribute = "memory";
	pMemory.value = strUsage + "%";

	binn *objMemory;

	char chUsageVal[255] = { 0 };
	snprintf(chUsageVal, 255, "%s", pMemory.value.c_str());

	// create a new object
	objMemory = binn_object();
	binn_object_set_str(objMemory, chUTC, bufutc);
	binn_object_set_str(objMemory, chTime, timeBuf);
	binn_object_set_str(objMemory, chAttribute, "memory");
	binn_object_set_str(objMemory, chValue, chUsageVal);

	int size = binn_size(objMemory);
	char ibuff[24] = { 0 };
	snprintf(ibuff, 24, "%d", size);
	PrintMessage("object size %d",size);
	int n = send(sockfd, ibuff, 24, 0);
	PrintMessage("sent to server %d", n);
	std::string ack = getAck(sockfd);
	PrintMessage("ack -> %s",ack.c_str());
	n = send(sockfd, (char *)binn_ptr(objMemory), binn_size(objMemory), 0);
	PrintMessage("sent to server %d", n);
	ack = getAck(sockfd);
	PrintMessage("ack -> %s",ack.c_str());
	// release the buffer
	binn_free(objMemory);
}

void SendCpuPerformanceCounterData(int sockfd) {
	
	// add values to it
	char chUTC[] = "utctime";
	char chTime[] = "time";
	char chAttribute[] = "attribute";
	char chValue[] = "value";

	std::string strTime(currentDateTime());
    char timeBuf[512] = { 0 };
    snprintf(timeBuf,512,"%s",strTime.c_str());

	std::time_t utctime = std::time(0);
	char bufutc[11] = { 0 };
	snprintf(bufutc, 11, "%ld", utctime);
	std::string strUTC(bufutc);

	PerfData pMemory;
	double cpuusage = PerformanceData::get().GetCpuUsage();
	char chusage[24] = { 0 };
	snprintf(chusage, 24, "%3.2f", cpuusage);
	std::string strUsage(chusage);

	pMemory.utctime = strUTC;
	pMemory.time = strTime;
	pMemory.attribute = "cpu";
	pMemory.value = strUsage + "%";

	binn *objMemory;

	char chUsageVal[255] = { 0 };
	snprintf(chUsageVal, 255, "%s", pMemory.value.c_str());

	// create a new object
	objMemory = binn_object();
	binn_object_set_str(objMemory, chUTC, bufutc);
	binn_object_set_str(objMemory, chTime, timeBuf);
	binn_object_set_str(objMemory, chAttribute, "cpu");
	binn_object_set_str(objMemory, chValue, chUsageVal);

	int size = binn_size(objMemory);
	char ibuff[24] = { 0 };
	snprintf(ibuff, 24, "%d", size);
	PrintMessage("object size %d",size);
	int n = send(sockfd, ibuff, 24, 0);
	PrintMessage("sent to server %d", n);
	std::string ack = getAck(sockfd);
	PrintMessage("ack -> %s", ack.c_str());
	n = send(sockfd, (char *)binn_ptr(objMemory), binn_size(objMemory), 0);
	PrintMessage("sent to server %d", n);
	ack = getAck(sockfd);
	PrintMessage("ack -> %s", ack.c_str());
	// release the buffer
	binn_free(objMemory);
	

}

void SendProcessPerformanceCounterData(int sockfd) {
	
	// add values to it
	char chUTC[] = "utctime";
	char chTime[] = "time";
	char chAttribute[] = "attribute";
	char chValue[] = "value";

	std::string strTime(currentDateTime());
    char timeBuf[512] = { 0 };
    snprintf(timeBuf,512,"%s",strTime.c_str());

	std::time_t utctime = std::time(0);
	char bufutc[11] = { 0 };
	snprintf(bufutc, 11, "%ld", utctime);
	std::string strUTC(bufutc);

	PerfData pMemory;
	int processCount= PerformanceData::get().GetProcessCount();
	char chusage[24] = { 0 };
	snprintf(chusage, 24, "%d", processCount);
	std::string strUsage(chusage);

	pMemory.utctime = strUTC;
	pMemory.time = strTime;
	pMemory.attribute = "process";
	pMemory.value = strUsage;

	binn *objMemory;

	char chUsageVal[255] = { 0 };
	snprintf(chUsageVal, 255, "%s", pMemory.value.c_str());

	// create a new object
	objMemory = binn_object();
	binn_object_set_str(objMemory, chUTC, bufutc);
	binn_object_set_str(objMemory, chTime, timeBuf);
	binn_object_set_str(objMemory, chAttribute, "process");
	binn_object_set_str(objMemory, chValue, chUsageVal);

	int size = binn_size(objMemory);
	char ibuff[24] = { 0 };
	snprintf(ibuff, 24, "%d", size);
	PrintMessage( "object size %d" , size);
	int n = send(sockfd, ibuff, 24, 0);
	PrintMessage("sent to server %d", n);
	std::string ack = getAck(sockfd);
	PrintMessage("ack -> %s" , ack.c_str());
	n = send(sockfd, (char *)binn_ptr(objMemory), binn_size(objMemory), 0);
	PrintMessage("sent to server %d", n);
	ack = getAck(sockfd);
	// release the buffer
	binn_free(objMemory);
}

std::string getSuccess(int sockfd) {
	PrintMessage("receiving success");
	std::string retStr;
	char buff[8] = { 0 };
	int n = recv(sockfd, buff, 7, 0);
	PrintMessage("received ");
	if (n > 0) {
		std::string message(buff);
		retStr = message;
	}
	else {
		PrintMessage("error in receiving ack..");
		retStr = "error in recv";
	}
	return retStr;
}

std::string getAck(int sockfd) {
	PrintMessage("receiving ack"); 
	std::string retStr;
	char buff[3] = { 0 };
	int n = recv(sockfd, buff, 2, 0);
	PrintMessage("received %s" , buff);
	if (n > 0) {
		std::string message(buff);
		retStr = message;
	}
	else {
		PrintMessage("error in receiving ack..");
		retStr = "error in recv";
	}
	return retStr;
}

void sendObject(int sockfd) {
	PrintMessage("in sendobject function ");
	binn *obj;

	// create a new object
	obj = binn_object();

	// add values to it
	char chId[] = "id";
	char chName[] = "name";
	char chPrice[] = "price";

	char chNameVal[] = "memory";

	binn_object_set_int32(obj, chId, 1000);
	binn_object_set_str(obj, chName, chNameVal);
	binn_object_set_double(obj, chPrice, 256.55);

	int objsize = binn_size(obj);
	char ibuff[24] = { 0 };
	snprintf(ibuff, 24, "%d", objsize);

	int n = send(sockfd, ibuff, 24, 0);
	PrintMessage("sent to server %d", n);

	std::string ack = getAck(sockfd);
	if (ack == "OK") {
		PrintMessage("now sending object");
		// send over the network or save to a file...
		n = send(sockfd, (char *)binn_ptr(obj), binn_size(obj), 0);
		PrintMessage("sent to server %d", n);
		ack = getAck(sockfd);
	}
	// release the buffer
	binn_free(obj);
}

void SendDataHandler(int sockfd)
{
	int n = 0;
	std::string sCommand = "object";
	int length = sCommand.size();
	char ibuff[24] = { 0 };
	snprintf(ibuff, 24, "%d", length);
	std::string ack = "";
	while (1)
	{
		n = send(sockfd, ibuff, sizeof(ibuff), 0);
		PrintMessage("sent %s",ibuff);
		ack = getAck(sockfd);
		
		n = send(sockfd, sCommand.c_str(), sCommand.size(), 0);
		 ack = getAck(sockfd);
		std::cout << ack.c_str() << std::endl;
		if (ack == "ok" || ack == "OK") {
			//sendObject(sockfd);
			PrintMessage("sending memory performance counter");
			SendMemoryPerformanceCounterData(sockfd);
		}

		n = send(sockfd, ibuff, sizeof(ibuff), 0);
		PrintMessage("sent %s" ,ibuff );
		ack = getAck(sockfd);
		n = send(sockfd, sCommand.c_str(), sCommand.size(), 0);
		ack = getAck(sockfd);
		if (ack == "ok" || ack == "OK") {
			PrintMessage("sending cpu performance counter");
			SendCpuPerformanceCounterData(sockfd);
		}

		n = send(sockfd, ibuff, sizeof(ibuff), 0);
		PrintMessage("sent %s",ibuff);
		ack = getAck(sockfd);
		n = send(sockfd, sCommand.c_str(), sCommand.size(), 0);
		ack = getAck(sockfd);
		if (ack == "ok" || ack == "OK") {
			PrintMessage("sending process performance counter");
			SendProcessPerformanceCounterData(sockfd);
		}

		//sleeping for 20 seconds
		sleep(20);
	}
}


int main(int argc, char *argv[])
{
	PrintMessage("argument count %d",argc);
	if(argc < 3){
		printf("USAGE : client <server ip> <server port>\n");
		return 1;
	}
	
	std::string ip(argv[1]);
	std::string strport(argv[2]);

	int nport = atoi(strport.c_str());

	int len,sockfd,result;
	struct sockaddr_in address;
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ip.c_str());
	address.sin_port = htons(nport);
	len = sizeof(address);
	
	result = connect(sockfd, (struct sockaddr *)&address, len);

	if (result == -1)
	{
		PrintMessage("error client connect");
		return -1;
	}

	std::string clientKey;
	std::cout<<"Please enter client key to login :";
	std::cin>>clientKey;
	int n = 0;
	int length = clientKey.size();
	char ibuff[24] = { 0 };
	snprintf(ibuff, 24, "%d", length);
	n = send(sockfd, ibuff, sizeof(ibuff), 0);
	std::string ack = getAck(sockfd);
	std::cout << ack.c_str() << std::endl;
	if (ack == "OK")
	{
		n = send(sockfd, clientKey.c_str(), clientKey.size(), 0);
		ack = getSuccess(sockfd);
		std::cout << ack.c_str() << std::endl;

		if (ack.compare("success") == 0) 
		{
			PrintMessage("login success");
			std::string strready = "ready";
			int len = strready.size();
			char buff[24] = { 0 };
			snprintf(buff, 24, "%d", len);
			n = send(sockfd, buff, sizeof(buff), 0);

			n = send(sockfd, strready.c_str(), len, 0);
			PrintMessage("sent ready, getting ack ");
			ack = getAck(sockfd);
			std::cout << ack << std::endl;
		}
		else 
		{
			PrintMessage("login failed");
			PrintMessage("server closed connection");
			return 1;
		}

	}
	//checking if socket is connected
	socklen_t slen;
	struct sockaddr_storage addr;
	slen = sizeof(addr);
	int nCode = getpeername(sockfd, (struct sockaddr *)&addr, &slen);

	PrintMessage("Value returned by getpeername %d", nCode);

	char ipstr[INET6_ADDRSTRLEN];
	int port;
	// deal with both IPv4 and IPv6:
	if (addr.ss_family == AF_INET) {
		struct sockaddr_in *s = (struct sockaddr_in *)&addr;
		port = ntohs(s->sin_port);
		inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
	}
	else { // AF_INET6
		struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
		port = ntohs(s->sin6_port);
		inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
	}

	PrintMessage("Peer IP address: %s\n", ipstr);
	PrintMessage("Peer port      : %d\n", port);

	std::cin.clear();
	fflush(stdin);
	PrintMessage("starting performance data transfer thread\n");
	std::thread t(SendDataHandler,sockfd);
	t.detach();
	//SendDataHandler(sockfd);

	while (1)
	{
		

		PrintMessage("Please enter a command : ");
		std::string sCommand;
		std::getline(std::cin, sCommand);

		int n = 0;
		int length = sCommand.size();
		char ibuff[24] = { 0 };
		snprintf(ibuff, 24, "%d", length);
		n = send(sockfd, ibuff, sizeof(ibuff), 0);
		std::cout << "sent " << ibuff << std::endl;
		std::string ack = getAck(sockfd);
		std::cout << ack.c_str() << std::endl;
		if (ack == "OK")
		{
			n = send(sockfd, sCommand.c_str(), sCommand.size(), 0);
			ack = getAck(sockfd);
			PrintMessage(ack.c_str());
			if (sCommand == "object" || sCommand == "OBJECT") {
				SendCpuPerformanceCounterData(sockfd);
			}
		}

		PrintMessage("sent to server %d\n", n);
		if (n == -1) {
			PrintMessage("error with server exiting");
			break;

		}

	}
	close(sockfd);
    return 0;
}


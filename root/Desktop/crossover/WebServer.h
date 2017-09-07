#ifndef WEBSERVER_H_INCLUDED
#define WEBSERVER_H_INCLUDED

#pragma once
#include <iostream>
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
#include <string.h>
#include <stdlib.h>
#include <cstdarg>
#include <future>

#include "binn.h"
#include "Database.h"
#include "Email.h"
#include "ConfigStore.h"
#include <signal.h>

/* Catch Signal Handler function */
void signal_callback_handler(int signum){
	
	PrintMessage("Caught signal SIGPIPE %d",signum);
}

#define SOCKET_ERROR    (-1)
std::vector<std::string> parseCommand(const char *buffer);
int PrintMessage(std::string format, ...);

void CheckforAlert(std::string clientkey,record r){
	PrintMessage("client key *s, in check alert",clientkey.c_str());
	int size  = ConfigStore::get().listClientConfig.size();
	int i=0;
	while(i < size){
		ClientConfig c = ConfigStore::get().listClientConfig[i];
		if(c.clientKey == clientkey){ 
			PrintMessage("clientkey %s, c.listAlert.size() -> %d \n",clientkey.c_str(),c.listAlert.size());
			for(int j=0;j < c.listAlert.size();j++){
				if(c.listAlert[j].alertType == r.attribute && 
					c.listAlert[j].limitVal == r.value){
						PrintMessage("attribute %s\n",r.attribute.c_str());
						PrintMessage("criteria met  sending alert email\n");
						emailParams ep;
						std::string sMessage = "Alert criteria met. Client "+r.clientkey;
						//+" "+r.attribute+" : "+r.value;
						PrintMessage("Message for email is %s",sMessage.c_str());
						ep.mailServerName = "gmail.com";
						ep.smtpServer = "smtp.gmail.com";
						ep.smtpPort = "465";
						ep.username = "webmail2manish@gmail.com";
						ep.password = "Zqk42upl";
						ep.recipient = "webmail2manish@gmail.com";
						ep.message = sMessage;
						ep.subject = "message from server";
						ep.from = "Server";
						ep.to = "Client";
	
						CEmail::get().updateEmailParams(ep);
				}
			}
		}
		i++;
	}
}

typedef struct client_record {

	std::string clientip;
	std::string clientport;
	int sockfd;
	bool clientloggedin;
	std::string key;

	void clientHandler(int client_sock, struct sockaddr_in client_name)
	{
		std::string ip = inet_ntoa(client_name.sin_addr);
		int port = (int)ntohs(client_name.sin_port);
		char buff[20] = { 0 };
		
		
		sprintf(buff,"%d",port);
		std::string str_port(buff);
		std::string mapkey = ip + ":" + str_port;
		clientip = ip;
		clientport = str_port;
		sockfd = client_sock;

		

		customprint("client details : %s\n", mapkey.c_str());
		customprint("Inside Thread Function\n");

		sendOk(client_sock);

		while (1)
		{
			long status;
			char buffer[24] = { 0 };
			status = recv(client_sock, buffer, sizeof(buffer), 0);
			if(status < 0){
				customprint("client disconnected the connection.");	
				break;
			}
			customprint("in while loop status -> %d , buffer -> %s ",status,buffer);
			int datalength = atoi(buffer);
			if (datalength == 0)
			{
				sendOk(client_sock);
				continue;
			}
			customprint("data length %d", datalength);
			sendOk(client_sock);
			char buff_command[512] = { 0 };
			status = recv(client_sock, buff_command, datalength, 0);
			std::string data(buff_command);
			customprint("data received %s", data.c_str());
			sendOk(client_sock);
			std::vector<std::string>commands = parseCommand(data.c_str());

			if (commands.size() > 0) {
				if (commands[0] == "logout" || commands[0] == "LOGOUT") {
					customprint("\nLogging out client %s\n", mapkey.c_str());
					close(client_sock);
					int s = shutdown(client_sock, 2);
					break;
				}
				if (commands[0] == "object" || commands[0] == "OBJECT") {
					customprint(" read Object %s", commands[0].c_str());
					receiveObject(client_sock);
				}
			}
			else
			{
				customprint("no command received");
			}

		}//END WHILE

	}
	void customprint(std::string format, ...)
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
		std::string newFormat = "[" + clientip + ":" + clientport + " " + strTime + "]\t" + format + "";
		va_start(arg, format.c_str());
		//done = vfprintf(stdout, format.c_str(), arg);
		done = vfprintf(stdout, newFormat.c_str(), arg);
		va_end(arg);
	}
	
	int printMessage(std::string format, ...)
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
		std::string newFormat = "[" + clientip + ":" + clientport + " " + strTime + "]\t" + format + "";
		va_start(arg, format.c_str());
		//done = vfprintf(stdout, format.c_str(), arg);
		done = vfprintf(stdout, newFormat.c_str(), arg);
		va_end(arg);

		return done;
	}
	
	void sendOk(int client_sock)
	{
		//printMessage("sending ok");
		char buff[24] = { 0 };
		snprintf(buff, 24, "%s", "OK");
		long n = send(client_sock, buff, strlen(buff), 0);
		if (n != SOCKET_ERROR) {
			//printMessage("Sent %d bytes ", "",n);
		}
		else
		{
			// int iError = errno();
			// if (iError == WSAEWOULDBLOCK)
			// 	//printMessage("recv failed with error: WSAEWOULDBLOCK\n");
			// 	printf("recv failed with error: WSAEWOULDBLOCK\n");
			// else
			// 	//printMessage("recv failed with error: %d", iError);
			// 	printf("recv failed with error: %d", iError);

			// //printMessage("error encountered %d returning... ", n);
			customprint("receive failed %ld \n",n);
		}
	}

	void receiveObject(int client_sock)
	{
		/* Catch Signal Handler SIGPIPE */
		signal(SIGPIPE, signal_callback_handler);

		//printMessage("inside receiveObject");
		int received_int = 0;
		char buf[24] = { 0 };

		//int return_status = recv(client_sock, (char*)&received_int, sizeof(received_int),0);
		long return_status = recv(client_sock, buf, sizeof(buf), 0);
		//printMessage("inside receiveObject return status %d", return_status);

		if (return_status != SOCKET_ERROR) {
			received_int = atoi(buf);
			customprint("\n OBJECT SIZE = %d\n", received_int);
		}
		else {
			// Handling erros here
			// int iError = WSAGetLastError();
			// if (iError == WSAEWOULDBLOCK)
			// 	//printMessage("recv failed with error: WSAEWOULDBLOCK\n");
			// 	printf("recv failed with error: WSAEWOULDBLOCK\n");
			// else
			// 	//printMessage("recv failed with error: %d\n", iError);
			// 	printf("recv failed with error: %d\n", iError);

			// //printMessage("\n error encountered %d\n returning... \n", return_status);

			return;
		}
		char buff[1024] = { 0 };
		sendOk(client_sock);
		//return_status = recv(client_sock, buff, sizeof(buff), 0);
		
		return_status = recv(client_sock, buff, received_int, 0);
		if (return_status != SOCKET_ERROR) {
			
			char *name;
			binn *obj;

			obj = binn_open(buff);
			if (obj == 0) {
				//printMessage("\n binn_open failed \n");
				return;
			}
			char  strUTC[]   = "utctime";
			char  strId[]    = "time";
			char  strName[]  = "attribute";
			char  strPrice[] = "value";

			char *utctime = binn_object_str(obj, strUTC);
			char *time = binn_object_str(obj, strId);
			name = binn_object_str(obj, strName);
			char *value = binn_object_str(obj, strPrice);

			//printMessage("id - %d, name - %s, price -  %f", id, name, price);
			customprint("receiveObject -> utc - %s, time - %s, attribute - %s, value -  %s\n",utctime, time, name, value);

			record r;
			r.clientkey = key;
			r.utc = utctime;
			r.time = time;
			r.attribute = name;
			r.value = value;
			
			customprint("inserting record in database\n");
			//std::future<void> dbFuture = std::async(Database::get().insertRecord,r);
			Database::get().insertRecord(r);
			customprint("calling checkforalert thread\n");

			//CheckforAlert(key,r);
			std::future<void> alertFuture = std::async(CheckforAlert, key, r);
			
			customprint("async results");
			//dbFuture.get();
			alertFuture.get();

			binn_free(obj);  /* releases the binn pointer but NOT the received buf */
		}
		else {
			//handle error
			
			customprint(" error encountered %ld  in receiving object ", return_status);
		}
		sendOk(client_sock);
	}

}client;



#endif // WEBSERVER_H_INCLUDED

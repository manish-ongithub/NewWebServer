#pragma once

#include <iostream>
#include <thread>
#include <map>
#include <vector>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <cstdarg>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>



#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")

extern int PrintMessage(std::string format, ...);


typedef struct
{
	int sock;
	const char* address;
	unsigned short port;
} sock_info;

typedef struct
{
	SSL* ssl;
	SSL_CTX* ctx;
} ssl_info;

typedef struct
{
	char b64username[256];
	char b64password[256];
} email;

typedef struct {

	std::string mailServerName;
	std::string smtpServer;
	std::string smtpPort;

	std::string username;
	std::string password;
	std::string recipient;
	std::string message;
	std::string subject;
	std::string from;
	std::string to;
	//unsigned int messagelen = strlen(message);

}emailParams;
class CEmail
{
public:
	static CEmail& get()
	{
		static CEmail instance;
		return instance;
	}
	bool connectsocket(sock_info* info);
	bool setssl(sock_info* sockinfo, ssl_info* sslinfo);
	void removessl(sock_info* sockinfo, ssl_info* sslinfo);
	void initssl();
	void freessl();
	void sslb64encode(const char* buffer, char* outbuffer);
	void initemail(const char* username, const char* password, email* outemail);
	bool printsocketbuffer(ssl_info* sslinfo);
	void sendemail(ssl_info* sslinfo, const char* username, const char* password, const char* recipient, const char* from, const char* to, const char* message, const char* subject, unsigned int messagelen, const char* mailServerName);
	void updateEmailParams(emailParams);
	static int ssl_init_count;
	
private:
	CEmail() {}
	CEmail(const CEmail&);
	
};



#include "Email.h"


bool CEmail::connectsocket(sock_info* info)
{

	struct sockaddr_in* sockaddr_ipv4 = NULL;
	struct addrinfo* it = NULL, *result = NULL;
	getaddrinfo(info->address, NULL, NULL, &result);

	for (it = result; it != NULL; it = it->ai_next)
	{
		sockaddr_ipv4 = (struct sockaddr_in*)it->ai_addr;
		info->address = inet_ntoa(sockaddr_ipv4->sin_addr);

		if (strncmp(info->address, "0.0.0.0", 7))
			break;
	}
	freeaddrinfo(result);

	if ((info->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		perror("Error creating socket..");
		return false;
	}

	struct sockaddr_in SockAddr;
	memset(&SockAddr, 0, sizeof(SockAddr));
	SockAddr.sin_port = htons(info->port);
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = inet_addr(info->address);

	if (connect(info->sock, (struct sockaddr*)&SockAddr, sizeof(SockAddr)) < 0)
	{
		perror("Error connecting socket..");
		return false;
	}

	return true;
}

bool CEmail::setssl(sock_info* sockinfo, ssl_info* sslinfo)
{
	sslinfo->ctx = SSL_CTX_new(SSLv23_client_method());

	if (sslinfo->ctx)
	{
		sslinfo->ssl = SSL_new(sslinfo->ctx);
		SSL_set_fd(sslinfo->ssl, sockinfo->sock);
		return SSL_connect(sslinfo->ssl) != -1;
	}
	return false;
}
void CEmail::removessl(sock_info* sockinfo, ssl_info* sslinfo)
{
	if (sslinfo->ctx)
	{
		SSL_CTX_free(sslinfo->ctx);
	}

	if (sslinfo->ssl)
	{
		SSL_shutdown(sslinfo->ssl);
		SSL_free(sslinfo->ssl);
	}

	sslinfo->ssl = NULL;
	sslinfo->ctx = NULL;

}
void CEmail::initssl()
{
	if (!ssl_init_count)
	{
		SSL_library_init();
		SSL_load_error_strings();
		OpenSSL_add_all_algorithms();
	}

	++ssl_init_count;

}
void CEmail::freessl()
{
	if (!--ssl_init_count)
	{
		ERR_free_strings();
		EVP_cleanup();
		CRYPTO_cleanup_all_ex_data();
	}
}
int CEmail::ssl_init_count = 0;
void CEmail::sslb64encode(const char* buffer, char* outbuffer)
{
	char* b64str = NULL;
	BIO* b64 = BIO_new(BIO_f_base64());
	BIO* mem = BIO_new(BIO_s_mem());

	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	b64 = BIO_push(b64, mem);
	BIO_write(b64, buffer, strlen(buffer));
	BIO_flush(b64);

	int len = BIO_get_mem_data(mem, &b64str);
	memcpy(outbuffer, b64str, len);
	outbuffer[len] = '\0';
	BIO_free_all(b64);
}
void CEmail::initemail(const char* username, const char* password, email* outemail)
{
	char ubuffer[256];
	char pbuffer[256];
	unsigned int bytes_written = 0;

	sslb64encode(username, &ubuffer[0]);
	sslb64encode(password, &pbuffer[0]);

	sprintf(outemail->b64username, "%s", ubuffer);
	sprintf(outemail->b64password, "%s", pbuffer);
	PrintMessage("b64 username %s\n", outemail->b64username);
	PrintMessage("b64 password %s\n", outemail->b64password);
}
bool CEmail::printsocketbuffer(ssl_info* sslinfo)
{
	char buffer[1024];
	unsigned int bytes_read = SSL_read(sslinfo->ssl, &buffer[0], sizeof(buffer));
	if (bytes_read > 0)
	{
		PrintMessage("%.*s", bytes_read, buffer);
		return true;
	}
	return false;
}
void CEmail::sendemail(ssl_info* sslinfo, const char* username, const char* password, const char* recipient, const char* from, const char* to, const char* message, const char* subject, unsigned int messagelen,const char* mailServerName)
{
    PrintMessage("in function sendemail.");
	email em;
	char buffer[512];
	unsigned int bufflen = sizeof(buffer);
	initemail(username, password, &em);

	char helloCommand[512] = {0};
	sprintf(helloCommand, "HELO %s\r\n", mailServerName);
	int cLen = strlen(helloCommand);

	//SSL_write(sslinfo->ssl, "HELO \r\n", 6);
	//SSL_write(sslinfo->ssl, "HELO gmail.com\r\n", 16);

	SSL_write(sslinfo->ssl, helloCommand, cLen);
	printsocketbuffer(sslinfo);

	SSL_write(sslinfo->ssl, "AUTH LOGIN\r\n", 12);
	printsocketbuffer(sslinfo);

	bufflen = sprintf(buffer, "%s\r\n", em.b64username);
	SSL_write(sslinfo->ssl, buffer, bufflen);
	printsocketbuffer(sslinfo);

	bufflen = sprintf(buffer, "%s\r\n", em.b64password);
	SSL_write(sslinfo->ssl, buffer, bufflen);
	printsocketbuffer(sslinfo);
	printsocketbuffer(sslinfo);

	bufflen = sprintf(buffer, "MAIL FROM: <%s>\r\n", username);
	SSL_write(sslinfo->ssl, buffer, bufflen);
	printsocketbuffer(sslinfo);

	bufflen = sprintf(buffer, "RCPT TO: <%s>\r\n", recipient);
	SSL_write(sslinfo->ssl, buffer, bufflen);
	printsocketbuffer(sslinfo);

	SSL_write(sslinfo->ssl, "DATA\r\n", 6);
	printsocketbuffer(sslinfo);

	bufflen = sprintf(buffer, "From: <%s><%s>\r\n", from, username);
	bufflen += sprintf(&buffer[bufflen], "To: <%s><%s>\r\n", to, recipient);
	bufflen += sprintf(&buffer[bufflen], "Subject: <%s>\r\n", subject);
	SSL_write(sslinfo->ssl, buffer, bufflen);

	bufflen = 0;
	while (bufflen < messagelen)
	{
		bufflen += SSL_write(sslinfo->ssl, &message[bufflen], messagelen - bufflen);
	}

	SSL_write(sslinfo->ssl, "\r\n.\r\n", 5);
	printsocketbuffer(sslinfo);

	SSL_write(sslinfo->ssl, "QUIT\r\n", 6);
	printsocketbuffer(sslinfo);
}

void CEmail::updateEmailParams(emailParams mailParams)
{
    PrintMessage("in updateemailparams");
	int smtpPort = atoi(mailParams.smtpPort.c_str());
	ssl_info sslinfo = { 0 };
	sock_info sockinfo = { 0, mailParams.smtpServer.c_str() , smtpPort };
	//sock_info sockinfo = { 0, "smtp.gmail.com", 587 };
	const char* mailserver = mailParams.mailServerName.c_str();
	const char* username  = mailParams.username.c_str();
	const char* password  = mailParams.password.c_str();
	const char* recipient = mailParams.recipient.c_str();
	const char* message   = mailParams.message.c_str();
	const char* subject	  = mailParams.subject.c_str();
	const char* from	  = mailParams.from.c_str();
	const char* to		  = mailParams.to.c_str();
	unsigned int messagelen = strlen(message);

	if (connectsocket(&sockinfo))
	{
        printf("conneted to socket \n");
		initssl();
        
		if (setssl(&sockinfo, &sslinfo))
		{
			sendemail(&sslinfo, username, password, recipient, from, to, message, subject, messagelen, mailserver);
		}
		else
		{
			ERR_print_errors_fp(stderr);
		}

		removessl(&sockinfo, &sslinfo);
		freessl();
	}	
}
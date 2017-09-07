// WebServer.cpp : Defines the entry point for the console application.
//

#include "WebServer.h"

//using namespace std;

#define CLIENT_CONFIG_FILE "client.xml"
#define GLOBAL_CONFIG_FILE "global.xml"

int readClientConfigFile(std::string strpath) {
	ConfigStore::get().parseFile(strpath);
	return 1;
}
int readGlobalConfigFile(std::string strpath) {
	ConfigStore::get().parserGlobalConfig(strpath.c_str());
	const char *uristr = ConfigStore::get().globalConfig.database_uri.c_str();
	const char *dbname = ConfigStore::get().globalConfig.dbname.c_str();
	const char *collection = ConfigStore::get().globalConfig.collection.c_str();
	Database::get().initDatabase(uristr, dbname, collection);
	return 1;
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
	std::string newFormat = "[SERVER : " + strTime + "]\t" + format + "\n";
	va_start(arg, format.c_str());
	
	done = vfprintf(stdout, newFormat.c_str(), arg);
	va_end(arg);

	return done;
}


std::vector<std::string> parseCommand(const char *buffer) {
	std::vector<std::string> command;
	char *token;
	char *rest = strdup(buffer);
	/* get the first token */
	token = strtok(rest," ");
	/* walk through other tokens */
	while (token != NULL)
	{
		command.push_back(token);
		PrintMessage("%s\n",token);
		token = strtok(NULL," ");
	}
	return command;
}

void sendOkToClient(int client_sock)
{
	
	char buff[24] = { 0 };
	snprintf(buff, 24, "%s", "OK");
	int n = send(client_sock, buff, strlen(buff), 0);
	if (n != SOCKET_ERROR) {
		//PrintMessage("Sent %d bytes ", n);
	}
	else
	{
       
		PrintMessage("error encountered %d returning... ", n);
	}
}


bool validateLogin(std::string clientKey) {
	bool bretval = false;
	int count = ConfigStore::get().listClientConfig.size();
	for (int i = 0; i < count; ++i) {
		if (ConfigStore::get().listClientConfig[i].clientKey == clientKey) {
			bretval = true;
			break;
		}
	}
	return bretval;
}

void client_connected(int client_sock, struct sockaddr_in client_name)
{
	/* Catch Signal Handler SIGPIPE */
	signal(SIGPIPE, signal_callback_handler);
	int status;
	char buff[24] = { 0 };
	status = recv(client_sock, buff, sizeof(buff), 0);
	int datalength = atoi(buff);

	PrintMessage("client key length %d", datalength);
	sendOkToClient(client_sock);
	char clientKey[512] = { 0 };
	status = recv(client_sock, clientKey, datalength, 0);
	std::string data(clientKey);
	PrintMessage("client key received %s", data.c_str());
	//sendOkToClient(client_sock);
	bool bValidClient = validateLogin(data);
	if (bValidClient)
	{
		char buff[24] = { 0 };
		snprintf(buff, 24, "%s", "success");
		int n = send(client_sock, buff, strlen(buff), 0);
		if (n == SOCKET_ERROR) {
        	PrintMessage("Send Failed");
		}
		char rbuff[24] = { 0 };
		status = recv(client_sock, rbuff, sizeof(rbuff), 0);
		datalength = atoi(rbuff);

		PrintMessage("login success, ready packet length %d", datalength);
		char ready[512] = { 0 };
		status = recv(client_sock, ready, datalength, 0);
		PrintMessage("%s\n", ready);
		sendOkToClient(client_sock);
	}
	else {
		PrintMessage("client key %s, not a valid client. closing connection.", data.c_str());
		close(client_sock);
		return;
	}

	struct client_record *pClient = new client();
	pClient->clientloggedin = true;
	pClient->key = data;
	pClient->clientHandler(client_sock, client_name);
	delete pClient;
}



int main()
{

	/* Catch Signal Handler SIGPIPE */
	signal(SIGPIPE, signal_callback_handler);

	int sock;
	u_short port = 54800;
	//sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		PrintMessage("\n Socket not created %d\n", sock);
	}
	else
		PrintMessage(" Socket created %d", sock);

	in_addr_t rec_addr = inet_addr("192.168.0.241");

	struct sockaddr_in name;
	memset(&name, 0, sizeof(name));
	name.sin_family = AF_INET;
	//name.sin_port = htons(port);
	//name.sin_addr.s_addr = htonl(INADDR_ANY);
	//name.sin_addr.s_addr = rec_addr;
	name.sin_addr.s_addr = INADDR_ANY;
    name.sin_port = htons( 54800 );

	int bretval = bind(sock, (struct sockaddr *)&name, sizeof(name));
	PrintMessage("bind retval -> %d",bretval);
	socklen_t  namelen = sizeof(name);
	if (getsockname(sock, (struct sockaddr *)&name, &namelen) == -1)
	{
		printf("error in  getsockname");
	}
	/* Print it. The IP address is often zero beacuase    */
      /* sockets are seldom bound to a specific local       */
      /* interface.                                         */
	  PrintMessage("Local IP address is: %s", inet_ntoa(name.sin_addr));
	  PrintMessage("Local port is: %d", (int) ntohs(name.sin_port));

	//port = ntohs(name.sin_port);

	if (listen(sock, 5) == 0)
		PrintMessage("listening on port %d, backlog is 5 ",(int) ntohs(name.sin_port));
	else
		PrintMessage("error listen\n");

	int client_sock = -1;
	struct sockaddr_in client_name;
	socklen_t  client_name_len = sizeof(client_name);

	

	PrintMessage("reading config file");
	
	readGlobalConfigFile(GLOBAL_CONFIG_FILE);
	readClientConfigFile(CLIENT_CONFIG_FILE);



	PrintMessage("waiting for client connection ... ");
	while (1)
	{
		client_sock = accept(sock, (struct sockaddr *)&client_name, &client_name_len);
		if (client_sock == -1)
		{
			PrintMessage("error while accept");
		}
		else
		{
			std::thread t1(client_connected, client_sock, client_name);
			t1.detach();

		}

	}
	close(sock);
	
	return 0;
}

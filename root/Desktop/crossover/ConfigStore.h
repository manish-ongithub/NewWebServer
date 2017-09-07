#pragma once
#include <fstream>
#include <map>
#include <string>
#include <vector>

typedef struct g_config {
	std::string database_uri;
	std::string dbname;
	std::string collection;

}
GlobalConfig;

typedef struct clientAlert {
	std::string alertType;
	std::string limitVal;

}cAlert;


typedef struct c_config {
	std::string clientKey;
	std::string email;
	std::vector<clientAlert> listAlert;

}ClientConfig;

extern int PrintMessage(std::string format, ...);

class ConfigStore
{
public:
	static ConfigStore& get()
	{
		static ConfigStore instance;
		return instance;
	}
	bool parseFile(std::string& inStream);
	bool parserGlobalConfig(const char *filepath);
	
	std::string getValue(std::string key);
	GlobalConfig globalConfig;
	std::vector<ClientConfig> listClientConfig;
private:
	ConfigStore() {}
	ConfigStore(const ConfigStore&);
	//ConfigStore& operator=(const ConfigStore&);
	std::map<std::string, std::string> storedConfig;
	
	
};
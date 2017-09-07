
#include "tinyxml2.h"
#include "ConfigStore.h"
#include <iostream>
bool ConfigStore::parseFile(std::string& inStream) {

	PrintMessage("inside parsefile");

	tinyxml2::XMLDocument  doc;
	doc.LoadFile(inStream.c_str());
	tinyxml2::XMLNode* node = doc.FirstChild();
	listClientConfig.clear();
	if(node == nullptr){
		PrintMessage("could not load file : %s",inStream.c_str());
	}
	while(node != nullptr)
	{
		ClientConfig cconfig;
		const char* key = node->ToElement()->Attribute("key");
		std::string strkey(key);
		cconfig.clientKey = strkey;
		
		const char* mail = node->ToElement()->Attribute("mail");
		std::string strmail(mail);
		cconfig.email = strmail;
		std::cout<<"email -> "<<strmail<<std::endl;

		tinyxml2::XMLNode* childNode = node->FirstChild();
		while (childNode != nullptr)
		{
			clientAlert calert;
			const char* alertType = childNode->ToElement()->Attribute("type");
			const char* alertLimit = childNode->ToElement()->Attribute("limit");
			
			calert.alertType = alertType;
			calert.limitVal = alertLimit;
			cconfig.listAlert.push_back(calert);

			childNode = childNode->NextSibling();

		}
		listClientConfig.push_back(cconfig);
		node = node->NextSibling();
	}
	PrintMessage("Records read %d",listClientConfig.size());
	PrintMessage(" parsefile complete.");
	return true;
}

bool ConfigStore::parserGlobalConfig(const char *filepath)
{
	PrintMessage("inside parserGlobalConfig");
	tinyxml2::XMLDocument  doc;
	doc.LoadFile(filepath);
	tinyxml2::XMLNode* node = doc.FirstChild();
	
	if(node == nullptr){
		PrintMessage("could not load file : %s",filepath);
	}

	while (node != nullptr)
	{
		
		const char* url = node->FirstChildElement("database")->FirstChildElement("url")->GetText();
		std::string strkey(url);
		globalConfig.database_uri = strkey;
		//printf("\n url %s",url);

		const char* db = node->FirstChildElement("database")->FirstChildElement("db")->GetText();
		std::string strdb(db);
		globalConfig.dbname = strdb;
		//std::cout<<"database name "<<strdb<<std::endl;

		const char* collection = node->FirstChildElement("database")->FirstChildElement("collection")->GetText();
		std::string strcollection(collection);
		globalConfig.collection = strcollection;

		node = node->NextSibling();
	}
	return true;
}

std::string ConfigStore::getValue(std::string key) {
	std::string strVal;
	return strVal;
}
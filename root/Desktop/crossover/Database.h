#pragma once

#include <bson.h>
#include <bcon.h>
#include <mongoc.h>

#include <fstream>
#include <map>
#include <string>
#include <vector>



#pragma comment(lib, "bson-1.0.lib")
#pragma comment(lib, "mongoc-1.0.lib")

typedef struct clientData {
	std::string  clientkey;
	std::string  utc;
	std::string  time;
	std::string  attribute;
	std::string  value;
	std::string getJSON()
	{
		std::string retval;
		return retval;
	}

}record;
class Database
{
	public:
		static Database& get()
		{
			static Database instance;
			return instance;
		}
		void initDatabase(std::string uri, std::string db, std::string collection);
		int insertRecord(record rc);
		void reset();
	private:
		
		Database() {};
		Database(const Database&) {};
		std::string m_uri;
		std::string m_dbname;
		std::string m_collection;

};


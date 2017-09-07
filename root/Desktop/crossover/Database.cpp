
#include "Database.h"

void Database::initDatabase(std::string uri, std::string db, std::string collection)
{

	m_uri = uri;
	m_dbname = db;
	m_collection = collection;

	mongoc_init();
}

void Database::reset()
{
	m_uri = "";
	m_dbname = "";
	m_collection = "";
	mongoc_cleanup();
}
int Database::insertRecord(record stRecord) {
	
	bson_t     *document;
	mongoc_client_t *client;
	mongoc_collection_t *collection;
	bson_error_t error;


	if (m_uri.size() == 0 || m_dbname.size() == 0 || m_collection.size() == 0)
	{
		fprintf(stderr, "PLEASE INIT DATABASE CLASS.\n");
		return EXIT_FAILURE;
	}

	//char       *str;
	document = bson_new();



	BSON_APPEND_UTF8(document, "clientkey", stRecord.clientkey.c_str());
	BSON_APPEND_UTF8(document, "utc", stRecord.utc.c_str());
	BSON_APPEND_UTF8(document, "time", stRecord.time.c_str());
	BSON_APPEND_UTF8(document, "attribute", stRecord.attribute.c_str());
	BSON_APPEND_UTF8(document, "value", stRecord.value.c_str());


	client = mongoc_client_new(m_uri.c_str());

	if (!client) {
		fprintf(stderr, "Failed to parse URI.\n");
		
		return EXIT_FAILURE;
	}
	mongoc_client_set_error_api(client, 2);
	mongoc_client_set_appname(client, "connect-example");
	collection = mongoc_client_get_collection(client, m_dbname.c_str(), m_collection.c_str());

	if (!mongoc_collection_insert(collection, MONGOC_INSERT_NONE, document, NULL, &error)) {
		fprintf(stderr, "%s\n", error.message);
		
		return EXIT_FAILURE;
	}

	/*
	* Print the document as a JSON string.
	*/
	//str = bson_as_canonical_extended_json(document, NULL);
	//printf("%s\n", str);
	//bson_free(str);

	/*
	* Clean up allocated bson documents.
	*/
	bson_destroy(document);
	mongoc_collection_destroy(collection);
	mongoc_client_destroy(client);
	
	return 0;
}
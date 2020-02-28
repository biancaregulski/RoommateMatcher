#include <sstream> 
#include <iostream>
#include <sqlite3.h>

static int createDB(const char* dir);
static int createTable(const char* dir);
static int insertData(const char* dir, std::string id, std::string nextId, std::string roommateId, std::string firstName,
	std::string lastName, int temperature, int cleanliness, int visitors, int smoker, int showers);
static int selectData(const char* dir, std::string selection);
static int updateData(const char* dir, std::string id, std::string attribute, std::string value);
static int deleteData(const char* dir, std::string id);
static int callback(void* notUsed, int argc, char** argv, char** azColName);
static std::string intToString(int num);
static int stringToInt(std::string string);

static int createDB(const char* dir) {
	sqlite3* DB;
	int exit = 0;

	exit = sqlite3_open(dir, &DB);

	sqlite3_close(DB);

	return 0;
}

static int createTable(const char* dir) {
	sqlite3* DB;

	std::string sql = "CREATE TABLE IF NOT EXISTS RESIDENTS("
		"ID				TEXT PRIMARY KEY, "
		"NEXTID			TEXT NULL, "
		"ROOMMATEID		TEXT NULL, "
		"FIRSTNAME		TEXT NOT NULL, "
		"LASTNAME		TEXT NOT NULL, "
		"TEMPERATURE	INT NOT NULL, "
		"CLEANLINESS	INT NOT NULL, "
		"VISITORS		INT NOT NULL, "
		"SHOWERS		INT NOT NULL, "
		"SMOKER			INT NOT NULL);";

	try {
		int exit = 0;
		exit = sqlite3_open(dir, &DB);

		char* errorMsg;
		exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &errorMsg);

		if (exit != SQLITE_OK) {
			std::cout << "Error: table could not be created\n";
			sqlite3_free(errorMsg);
		}
		sqlite3_close(DB);
	}
	catch (const std::exception & e) {
		std::cerr << e.what();
	}

	return 0;
}

static int insertData(const char* dir, std::string id, std::string nextId, std::string roommateId, std::string firstName, 
					  std::string lastName, int temperature,  int cleanliness, int visitors, int smoker, int showers) {
	sqlite3* DB;
	char* errorMsg;
	int exit = sqlite3_open(dir, &DB);

	std::string sql = "INSERT INTO USER (ID, NEXTID, ROOMMATEID, FIRSTNAME, LASTNAME, TEMPERATURE, CLEANLINESS, VISITORS, SHOWERS, SMOKER) VALUES ('"
			+ id + "', '" + nextId + "', " + roommateId + "', " + firstName + "', " + lastName + ", " + intToString(temperature) + ", " 
			+ intToString(cleanliness) + ", " + intToString(visitors) + ", " + intToString(smoker) + ", " + intToString(showers) + ");";

	exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &errorMsg);
	if (exit != SQLITE_OK) {
		std::cerr << "Error: could not insert\n";
		sqlite3_free(errorMsg);
	}
	return 0;
}

static int updateData(const char* dir, std::string id, std::string attribute, std::string value) {
	sqlite3 * DB;
	char * errorMsg;
	int exit = sqlite3_open(dir, &DB);
	char punct = ' ';
	// if attribute is a string, then we need to add apostrophes to the query
	if (attribute.compare("ID") == 0 || attribute.compare("NEXTID") == 0 || attribute.compare("ROOMMATEID") == 0
		|| attribute.compare("FIRSTNAME") == 0 || attribute.compare("LASTNAME") == 0) {
		punct = '\'';
	}
	std::string sql = "UPDATE RESIDENTS SET " + attribute + " = " + punct + value + punct + " WHERE ID = '" + id + "';";
	exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &errorMsg);
	if (exit != SQLITE_OK) {
		std::cerr << "Error: could not update\n";
		sqlite3_free(errorMsg);
	}
	return 0;
}

static int selectData(const char* dir, std::string selection) {
	sqlite3 * DB;
	int exit = sqlite3_open(dir, &DB);
	std::string sql = "SELECT " + selection + " FROM RESIDENTS;";
	sqlite3_exec(DB, sql.c_str(), callback, NULL, NULL);

	return 0;
}

static int deleteData(const char* dir, std::string id) {
	sqlite3 * DB;
	int exit = sqlite3_open(dir, &DB);
	std::string sql = "DELETE FROM RESIDENTS WHERE ID = '" + id + "';";
	sqlite3_exec(DB, sql.c_str(), callback, NULL, NULL);

	return 0;
}

// retrieves database contents
static int callback(void* notUsed, int argc, char** argv, char** azColName) {
	for (int i = 0; i < argc; i++) {
		std::cout << azColName[i] << ": " << argv[i] << std::endl;
	}
	std::cout << std::endl;

	return 0;
}

static std::string intToString(int num) {
	std::ostringstream ss;
	ss << num;                                   
	return ss.str();
}

static int stringToInt(std::string string) {
	std::cout << "test";
	int num = 0;
	std::istringstream ss(string);
	ss >> num;                                 
	return num;
}
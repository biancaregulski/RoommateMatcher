#include <sstream> 
#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <cstdlib>
#include <regex>
#include <sqlite3.h>
#include <iomanip>
#include <cmath>

using Record = std::vector<std::string>;
using Records = std::vector<Record>;

static int createDB(const char* dir);
static int createTable(const char* dir);
static int insertData(const char* dir, std::string id, std::string roommateId, std::string firstName,
					  std::string lastName, int temperature, int cleanliness, int visitors, int smoker, int showerTime);
static bool isEmpty(const char* dir);
static Records selectData(const char* dir, const std::string& id, const std::string& attribute);
//int select_callback(void* p_data, int num_fields, char** p_fields, char** p_col_names);
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
		"ID				VARCHAR(10) PRIMARY KEY, "
		"Roommate_ID	VARCHAR(10) NOT NULL, "
		"First_Name		VARCHAR(30) NOT NULL, "
		"Last_Name		VARCHAR(30) NOT NULL, "
		"Temperature	INT NOT NULL, "
		"Cleanliness	INT NOT NULL, "
		"Visitors		INT NOT NULL, "
		"Shower_Time	INT NOT NULL, "
		"Smoker			INT NOT NULL);";

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

static int insertData(const char* dir, std::string id, std::string roommateId, std::string firstName, std::string lastName,
					  int temperature, int cleanliness, int visitors, int smoker, int showerTime) {
	sqlite3* DB;
	char* errorMsg = 0;
	int exit = sqlite3_open(dir, &DB);

	std::string sql = "INSERT INTO RESIDENTS (ID, Roommate_ID, First_Name, Last_Name, Temperature, Cleanliness, Visitors, Shower_Time, Smoker) VALUES ('"
		+ id + "', '" + roommateId + "', '" + firstName + "', '" + lastName + "', " + intToString(temperature - 1) + ", "
		+ intToString(cleanliness - 1) + ", " + intToString(visitors - 1) + ", " + intToString(showerTime - 1) + ", " + intToString(smoker - 1) + ");";

	exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &errorMsg);
	if (exit != SQLITE_OK) {
		std::cerr << "Error: " <<  errorMsg << '\n';
		sqlite3_free(errorMsg);
	}
	else {
		std::cout << "\n" << firstName << ' ' << lastName << " added to the list of residents."
				  << "\nResident ID: " << id << "\n\n";
	}

	return 0;
}

static int updateData(const char* dir, std::string id, std::string attribute, std::string value) {
	sqlite3* DB;
	char* errorMsg;
	int exit = sqlite3_open(dir, &DB);
	char punct = ' ';
	// if attribute is a string, then we need to add apostrophes to the query
	if (attribute.compare("ID") == 0 || attribute.compare("Roommate_ID") == 0
		|| attribute.compare("First_Name") == 0 || attribute.compare("Last_Name") == 0) {
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

static bool isEmpty(const char* dir) {
	sqlite3* DB;
	int exit = sqlite3_open(dir, &DB);
	const char* sql = "SELECT COUNT(*) FROM RESIDENTS";
	struct sqlite3_stmt* selectstmt;
	int result = sqlite3_prepare_v2(DB, sql, -1, &selectstmt, NULL);
	if (result == SQLITE_OK)
	{
		if (sqlite3_step(selectstmt) == SQLITE_ROW)
		{
			return false;		// row found
		}
		else
		{
			return true;
		}
	}
	sqlite3_finalize(selectstmt);
}

static Records selectData(const char* dir, const std::string &id, const std::string &attribute) {
	Records records;  
	sqlite3* DB;
	char* errorMsg;
	std::string sql;
	int exit = sqlite3_open(dir, &DB);
	sql = "SELECT " + attribute + " FROM RESIDENTS";
	if (!id.empty()) {
		sql += " WHERE ID = '" + id + "'";
	}
	sql += ";";
	exit = sqlite3_exec(DB, sql.c_str(),callback, &records, &errorMsg);

	if (exit != SQLITE_OK) {
		std::cerr << "Error: " << errorMsg << '\n';
		sqlite3_free(errorMsg);
	}

	return records;
}

int callback(void* p_data, int num_fields, char** p_fields, char** p_col_names) {
	Records* records = static_cast<Records*>(p_data);
	try {
		records->emplace_back(p_fields, p_fields + num_fields);
	}
	catch (...) {
		return 1;
	}
	return 0;
}


static int deleteData(const char* dir, std::string id) {
	sqlite3* DB;
	int exit = sqlite3_open(dir, &DB);
	std::string sql = "DELETE FROM RESIDENTS WHERE ID = '" + id + "';";
	exit = sqlite3_exec(DB, sql.c_str(), callback, NULL, NULL);

	if (sqlite3_changes(DB) > 0) {
		std::cout << id << " has been deleted.";
	}
	else {
		std::cout << id << " not found.";
	}

	return 0;
}

static std::string intToString(int num) {
	std::ostringstream ss;
	ss << num;
	return ss.str();
}

static int stringToInt(std::string string) {
	int num = 0;
	std::istringstream ss(string);
	ss >> num;
	return num;
}
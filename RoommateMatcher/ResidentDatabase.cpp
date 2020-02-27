#include <iostream>
#include <sqlite3.h>

static int createDB(const char* s) {
	sqlite3* DB;
	int exit = 0;

	exit = sqlite3_open(s, &DB);

	sqlite3_close(DB);

	return 0;
}

static int createTable(const char* s) {
	sqlite3* DB;

	std::string sql = "CREATE TABLE IF NOT EXISTS RESIDENTS("
		"ID				TEXT PRIMARY KEY, "
		"NEXTID			TEXT NULL, "
		"ROOMMATE		TEXT NULL, "
		"FIRSTNAME		TEXT NOT NULL, "
		"LASTNAME		TEXT NOT NULL, "
		"TEMPERATURE	INT NOT NULL, "
		"CLEANLINESS	INT NOT NULL, "
		"VISITORS		INT NOT NULL, "
		"SHOWERS		INT NOT NULL, "
		"SMOKER			INT NOT NULL);";

	try {
		int exit = 0;
		exit = sqlite3_open(s, &DB);

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
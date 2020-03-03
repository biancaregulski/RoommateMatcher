// TODO: algorithm
#include "ResidentDatabase.cpp"
#include <boost/algorithm/string/trim.hpp>


const char* levelNames[3] = { "Low", "Moderate", "High" };
const char* timeNames[3] = { "Mornings", "Afternoons", "No preference" };
const char* yesNo[2] = { "Yes", "No" };

void showMenu();
std::string nameErrorChecking();
int intErrorChecking(int rangeMax);
void displayResidentInfo(Record row);
void displayEditMenu(Record row);
std::string getFirstName();
std::string getLastName();
int getTemperature();
int getCleanliness();
int getVisitors();
int getShowerTime();
int getSmoker();

int main() {

    const char* dir = "RESIDENTS.db";
    sqlite3* DB;
    createDB(dir);
    createTable(dir);

    bool exit = false;
    int selection;
    Records records;
    int idNum;
    std::string idNumString;
    std::string firstName, lastName, id, roommateId;
    int temperature, cleanliness, visitors, showerTime;
    int smoker;
    Records zrecords;
    showMenu();

    do {
        std::cout << "\nSelection: ";
        std::cin >> selection;
        switch (selection) {
        case 1:
            firstName = getFirstName();
            lastName = getLastName();
            temperature = getTemperature();
            cleanliness = getCleanliness();
            visitors = getVisitors();
            showerTime = getShowerTime();
            smoker = getSmoker();

            // build id by appending strings
            id = "";
            id += std::tolower(firstName[0]);                // id starts with first and last initial
            id += std::tolower(lastName[0]);
            idNum = 0;
            do {
                idNumString = intToString(++idNum);
            } while (!selectData(dir, id + idNumString, "*").empty());

            id += idNumString;                               // then append id number to initials

            std::cout << id << '\n';

            roommateId = "None";
            insertData(dir, id, roommateId, firstName, lastName, temperature, cleanliness, visitors, smoker, showerTime);

            showMenu();
            break;
        case 2:
            // TODO: check if list is empty
            std::cout << "\nResident ID: ";
            std::cin >> id;
            records = selectData(dir, id, "*");
            if (records.empty()) {
                std::cout << "Resident not found.\n\n";
            }
            else {
                for (Record & row : records) {
                    displayResidentInfo(row);
                }
            }
            showMenu();
            break;
        case 3:
            // TODO: check if list is empty
            std::cout << "\nResident ID: ";
            std::cin >> id;
            records = selectData(dir, id, "*");
            if (records.empty()) {
                std::cout << "Resident not found.\n\n";
            }
            else {
                std::cout << "\nWhich attribute would you like to edit?\n";
                for (auto& row : records) {
                    displayEditMenu(row);
                }
                int editSelection;
                do {
                    std::cout << "\n> ";
                    editSelection = intErrorChecking(8);                // input number to find out which attribute to change
                    switch (editSelection) {
                    case 1:
                        firstName = getFirstName();
                        updateData(dir, id, "First_Name", firstName);
                        break;
                    case 2:
                        lastName = getLastName();
                        updateData(dir, id, "Last_Name", lastName);
                        break;
                    case 3:
                        temperature = getTemperature();
                        updateData(dir, id, "Temperature", intToString(temperature));
                        break;
                    case 4:
                        cleanliness = getCleanliness();
                        updateData(dir, id, "Cleanliness", intToString(cleanliness));
                        break;
                    case 5:
                        visitors = getVisitors();
                        updateData(dir, id, "Visitors", intToString(visitors));
                        break;
                    case 6:
                        showerTime = getShowerTime();
                        updateData(dir, id, "Shower_Time", intToString(showerTime));
                        break;
                    case 7:
                        smoker = getSmoker();
                        updateData(dir, id, "Smoker", intToString(smoker));
                    case 8: 
                        break;
                    default:
                        std::cout << "Invalid selection. Try again.\n";
                    }
                } while (editSelection < 0 || editSelection > 8);
            }
            std::cout << '\n';
            showMenu();
            break;
        case 4:
            std::cout << "\nResident ID: ";
            std::cin >> id;
            deleteData(dir, id);
            // TODO: if resident has roommate, set roommate's roommate to null
            std::cout << "\n\n";
            showMenu();
            break;
        case 5:
            // pair algorithm
            break;
        case 6:
            exit = true;
            break;
        default:
            std::cout << "Invalid selection. Try again.\n";
        }
    } while (exit == false);
}

std::string getFirstName() {
    std::cout << "\nFirst Name: ";
    std::cin.ignore();                              // ignores newline before std::getline
    return nameErrorChecking();
}

std::string getLastName() {
    std::cout << "Last Name: ";
    return nameErrorChecking();
}

int getTemperature() {
    std::cout << "\nWhat temperature should your room be?\n"
        << "1. Cold\n2. Average\n3. Warm\n> ";
    return intErrorChecking(3);                          // checks that 1 =< input >= 3
}

int getCleanliness() {
    std::cout << "\nHow clean is your room?\n"
        << "1. Tidy\n2. Average\n3. Messy\n> ";
    return intErrorChecking(3);
}

int getVisitors() {


    std::cout << "\nHow often may visitors be over?\n"
        << "1. Infrequently\n2. Average\n3. Frequently\n> ";
    return intErrorChecking(3);
}

int getShowerTime() {

    std::cout << "\nWhen do you like to take showers?\n"
        << "1. Mornings\n2. Afternoons\n3. No preference\n> ";
    return intErrorChecking(3);
}

int getSmoker() {
    std::cout << "\nAre you a smoker?\n"
        << "1. Yes\n2. No\n> ";
    return intErrorChecking(2);
}

std::string nameErrorChecking() {
    std::string name;
    char c;
    bool charFound = false;

    INPUT: while (std::getline(std::cin, name)) {                
        for (size_t i = 0; i < name.length(); i++) {
            c = name[i];         
            if (!std::isalpha(c) && c != ' ') {
                std::cout << "Name can only consist of letters and spaces\n> ";
                goto INPUT;
            }
            else if (std::isalpha(c)) {
                charFound = true;
            }
        }
        if (charFound) {
            break;                          // if find string without errors, leave while loop
        }
        else {
            std::cout << "Names must have at least one letter\n> ";     // if string consists of only spaces, continue while loop
        }
    }
    name[0] = toupper(name[0]);         // make first letter of name uppercase
    boost::algorithm::trim(name);       // gets rid of trailing/pending spaces
    return name;
}

int intErrorChecking(int rangeMax) {
    int preference;
    std::cin >> preference;
    while (preference < 1 || preference > rangeMax) {
        std::cout << "Input must be an integer in between 1 and " << rangeMax << ". Try again.\n> ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');         // discards inappropriate input
        std::cin >> preference;
    }
    return preference;
}

void showMenu() {
    std::cout << "* * * * * * * * * * * * *" << '\n'
              << "*    Roomate Matcher    *" << '\n'
              << "*  -------------------  *" << '\n'
              << "*  1. Add Resident      *" << '\n'
              << "*  2. Display Resident  *" << '\n'
              << "*  3. Edit Resident     *" << '\n'
              << "*  4. Delete Resident   *" << '\n'
              << "*  5. Pair Residents    *" << '\n'
              << "*  6. Exit              *" << '\n'
              << "* * * * * * * * * * * * *" << '\n';
}

void displayResidentInfo(Record row) {

    std::cout << "\nName: " << row.at(2) << ' ' << row.at(3);           // output first and last name
    std::cout << "\nRoommate: ";
    //if (row.at(1).empty()) {
        std::cout << "Unassigned";
   /* }
    else {
        // TODO: retrieve assigned roommate
    }*/
    std::cout << "\nPreferences"
        << "\n-----------"
        << "\nTemperature: " << levelNames[stringToInt(row.at(4))]
        << "\nCleanliness: " << levelNames[stringToInt(row.at(5))]
        << "\nVisitors: " << levelNames[stringToInt(row.at(6))]
        << "\nShower Time: " << timeNames[stringToInt(row.at(7))]
        << "\nSmoker: " << yesNo[stringToInt(row.at(8))] << "\n\n";
}

void displayEditMenu(Record row) {
    std::cout << "\n1. First name: " << row.at(2)
        << "\n2. Last name: " << row.at(3)
        << "\n3. Temperature: " << levelNames[stringToInt(row.at(4))]
        << "\n4. Cleanliness: " << levelNames[stringToInt(row.at(5))]
        << "\n5. Visitors: " << levelNames[stringToInt(row.at(6))]
        << "\n6. Shower Time: " << timeNames[stringToInt(row.at(7))]
        << "\n7. Smoker: " << yesNo[stringToInt(row.at(8))]
        << "\n8. Cancel";
}
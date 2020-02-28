// TODO: make database where you can query by name, preferences, etc.
// TODO: algorithm
#include "ResidentList.cpp"
#include <boost/algorithm/string/trim.hpp>

void showMenu();
std::string nameErrorChecking();
int intErrorChecking(int rangeMax);

int main() {

    const char* dir = "STUDENTS.db";      // TODO: change?
    sqlite3* DB;
    createDB(dir);
    createTable(dir);

    ResidentList residents;
    ResidentNode* selectedResident;
    bool exit = false;
    int selection;
    std::string firstName, lastName, id;
    int temperature, cleanliness, visitors, showers;
    int smoker;

    showMenu();

    do {
        std::cout << "\nSelection: ";
        std::cin >> selection;
        switch (selection) {
            case 1:
                std::cout << "\nFirst Name: ";
                std::cin.ignore();                              // ignores newline before std::getline
                firstName = nameErrorChecking();
                std::cout << "Last Name: ";
                lastName = nameErrorChecking();

                std::cout << "\nWhat temperature should your room be?\n"
                          << "1. Cold\n2. Average\n3. Warm\n> ";
                temperature = intErrorChecking(3);                          // checks that 1 =< input >= 3

                std::cout << "\nHow clean is your room?\n"
                          << "1. Tidy\n2. Average\n3. Messy\n> ";
                cleanliness = intErrorChecking(3);

                std::cout << "\nHow often may visitors be over?\n"
                    << "1. Infrequently\n2. Average\n3. Frequently\n> ";
                visitors = intErrorChecking(3);

                std::cout << "\nWhen do you like to take showers?\n"
                          << "1. Mornings\n2. Afternoons\n3. No preference\n> ";
                showers = intErrorChecking(3);

                std::cout << "\nAre you a smoker?\n"
                    << "1. Yes\n2. No\n> ";
                smoker = intErrorChecking(2);

                id = residents.addResident(firstName, lastName, temperature,
                    cleanliness, visitors, showers, smoker);

                std::cout << "\n" << firstName << ' ' << lastName << " added to the list of residents."
                          << "\nResident ID: " << id << "\n\n";
                showMenu();
                break;
            case 2:
                if (residents.isEmpty()) {
                    std::cout << "List of residents is empty.\n";
                }
                else {
                    std::cout << "\nResident ID: ";
                    std::cin >> id;
                    selectedResident = residents.searchNode(id);
                    if (selectedResident == NULL) {
                        std::cout << "Resident not found.\n\n";
                    }
                    else {
                        residents.displayResidentInfo(selectedResident);
                        std::cout << "\n\n";
                    }
                    showMenu();
                }
                break;
            case 3:
                if (residents.isEmpty()) {
                    std::cout << "List of residents is empty.\n";
                }
                else {
                    std::cout << "\nResident ID: ";
                    std::cin >> id;
                    selectedResident = residents.searchNode(id);
                    if (selectedResident == NULL) {
                        std::cout << "Resident not found.\n\n";
                    }
                    else {
                        std::cout << "\nWhich attribute would you like to edit?\n";
                        residents.displayEditMenu(selectedResident);
                        std::cout << "\n> ";
                    }
                    showMenu();
                }
                break;
            case 4:
                std::cout << "\nResident ID: ";
                std::cin >> id;
                if (residents.deleteNode(id)) {
                    std::cout << "\nResident deleted from list.\n\n";
                    // TODO: if resident has roommate, set roommate's roommate to null
                }
                else {
                    std::cout << "Resident not found.\n";
                }
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


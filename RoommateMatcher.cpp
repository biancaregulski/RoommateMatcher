// TODO: make database where you can query by name, preferences, etc.

#include "ResidentList.cpp"

void showMenu();
int intErrorChecking(int rangeMax);
char charErrorChecking();

int main() {
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
                std::getline(std::cin, firstName);              // allows names to have spaces

                std::cout << "Last Name: ";
                std::getline(std::cin, lastName);

                std::cout << "\nWhat temperature should your room be?\n"
                          << "1. Cold\n2. Average\n3. Warm\n> ";
                temperature = intErrorChecking(3);

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
            case 6:
                exit = true;
                break;
            default:
                std::cout << "Invalid selection. Try again.\n";
        }
    } while (exit == false);
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


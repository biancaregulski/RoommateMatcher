#include "ResidentDatabase.cpp"
#include <boost/algorithm/string/trim.hpp>
#include <algorithm>

using Ranking = std::vector<int>;
using RankingList = std::vector<Ranking>;

const char* dir = "RESIDENTS.db";

const char* levelNames[3] = { "Low", "Moderate", "High" };
const char* timeNames[3] = { "Mornings", "Afternoons", "No preference" };
const char* yesNo[2] = { "Yes", "No" };

void showMenu();

std::string getFirstName();
std::string getLastName();
int getTemperature();
int getCleanliness();
int getVisitors();
int getShowerTime();
int getSmoker();

std::string nameErrorChecking();
int intErrorChecking(int rangeMax);
void displayResidentInfo(Record row);
void displayEditMenu(Record row);
void displayAllResidents(Records records);
void createRankingList(Records records);
void algorithmPhase1(RankingList& residentRankingList);
void algorithmPhase2(RankingList& residentRankingList);

int main() {
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

            roommateId = "Unassigned";
            insertData(dir, id, roommateId, firstName, lastName, temperature, cleanliness, visitors, smoker, showerTime);

            showMenu();
            break;
        case 2:
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
            // if resident has roommate, set roommate's roommateID to Unassigned
            records = selectData(dir, id, "Roommate_ID");
            if (!records.empty()) {
                for (auto& row : records) {
                    updateData(dir, row[0], "Roommate_ID", "Unassigned");
                }
            }
            deleteData(dir, id);
            std::cout << "\n\n";
            showMenu();
            break;
        case 5:
            // pair algorithm
            createRankingList(selectData(dir, "", "ID, Temperature, Cleanliness, Visitors, Shower_Time, Smoker"));
            break;
        case 6:
            records = selectData(dir, "", "ID, First_Name, Last_Name");
            if (records.empty()) {
                std::cout << "No residents found.\n";
            }
            else {
                displayAllResidents(records);
            }
            break;
        case 7:
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
    std::cout << "* * * * * * * * * * * * * *" << '\n'
        << "*     Roomate Matcher     *" << '\n'
        << "*  ---------------------  *" << '\n'
        << "*  1. Add Resident        *" << '\n'
        << "*  2. Show Resident Info  *" << '\n'
        << "*  3. Edit Resident       *" << '\n'
        << "*  4. Delete Resident     *" << '\n'
        << "*  5. Pair Residents      *" << '\n'
        << "*  6. Show All Residents  *" << '\n'
        << "*  7. Exit                *" << '\n'
        << "* * * * * * * * * * * * * *" << '\n';
}

void displayResidentInfo(Record row) {
    std::cout << "\nName: " << row[2] << ' ' << row[3];           // output first and last name
    std::cout << "\nRoommate: " << row[1];
    std::cout << "\nPreferences"
        << "\n-----------"
        << "\nTemperature: " << levelNames[stringToInt(row[4])]
        << "\nCleanliness: " << levelNames[stringToInt(row[5])]
        << "\nVisitors: " << levelNames[stringToInt(row[6])]
        << "\nShower Time: " << timeNames[stringToInt(row[7])]
        << "\nSmoker: " << yesNo[stringToInt(row[8])] << "\n\n";
}

void displayEditMenu(Record row) {
    std::cout << "\n1. First name: " << row[2]
        << "\n2. Last name: " << row[3]
        << "\n3. Temperature: " << levelNames[stringToInt(row[4])]
        << "\n4. Cleanliness: " << levelNames[stringToInt(row[5])]
        << "\n5. Visitors: " << levelNames[stringToInt(row[6])]
        << "\n6. Shower Time: " << timeNames[stringToInt(row[7])]
        << "\n7. Smoker: " << yesNo[stringToInt(row[8])]
        << "\n8. Cancel";
}

void displayAllResidents(Records records) {
    // display id, first name, and last name for all residents
    for (auto& row : records) {
        std::cout << "ID: " << std::left << std::setw(10) << row[0]
            << "Name: " << row[1] << ' ' << row[2] << "\n";
    }
}

// Records records consists of ID, temperature, cleanliness, visitors, showerTime, and smoker values
void createRankingList(Records records) {
    RankingList residentRankingList;
    int numResidents = records.size();

    if (numResidents == 0) {
        std::cout << "No residents, cannot match.\n";
        return;
    }
    else if (numResidents == 1) {
        std::cout << "Only one resident, cannot match.\n";
        return;
    }

    else if (numResidents % 2 == 1) {
        std::cout << "Odd number of residents.\n"
            << "Not including resident \"" << records[records.size() - 1][0] << "\" in matching.\n\n";
        numResidents--;
    }

    if (numResidents == 2) {
        // no need to do algorithm :^)
        residentRankingList = {{ 0, 1},
                               {1, 0}};
    }

    else {
        // fill preferenceRankingList 2d vector with -1s as default
        RankingList prefRankingList(numResidents, Ranking(numResidents, -1));

        // fill each idRankingList row with integers
        Ranking ids(numResidents);
        for (int i = 0; i < numResidents; i++) {
            ids[i] = i;
        }
        residentRankingList.resize(numResidents, ids);

        // now fill rankingList with difference values between each potential resident and roommate
        int differenceTotal;
        int showerTimeIndex = 4;
        for (int resident = 0; resident < numResidents; resident++) {
            for (int roommate = 0; roommate < numResidents; roommate++) {
                if (resident != roommate) {
                    // loop through preferences, e.g. temperature, which has a rating 1-3 
                    differenceTotal = 0;
                    for (int i = 1; i < 6; i++) {
                        if (i == showerTimeIndex) {
                            /* calculation is different for shower time
                               add 1 to differenceTotal if they chose the same shower time
                               if they chose different shower times or if either picked "no preference", then add nothing
                            */
                            if (stringToInt(records[resident][showerTimeIndex]) == stringToInt(records[roommate][showerTimeIndex])) {
                                differenceTotal++;
                            }
                        }
                        else {
                            // add all absolute values of difference values
                            differenceTotal += std::abs(stringToInt(records[resident][i]) - stringToInt(records[roommate][i]));
                        }
                    }
                    prefRankingList[resident][roommate] = differenceTotal;
                }
            }

            // sort preference values in ascending order for each resident and sort ids in the same order
            int prefTemp;
            int idTemp;
            for (int i = 0; i < numResidents; i++) {
                for (int j = i + 1; j < numResidents; j++) {
                    if (prefRankingList[resident][i] > prefRankingList[resident][j]) {
                        prefTemp = prefRankingList[resident][i];
                        prefRankingList[resident][i] = prefRankingList[resident][j];
                        prefRankingList[resident][j] = prefTemp;

                        idTemp = residentRankingList[resident][i];
                        residentRankingList[resident][i] = residentRankingList[resident][j];
                        residentRankingList[resident][j] = idTemp;
                    }
                }
            }
        }
        algorithmPhase1(residentRankingList);
    }
    
    int residentIndex, roommateIndex;
    std::cout << "Matches: \n";
    for (int i = 0; i < residentRankingList.size(); i++) {
        residentIndex = residentRankingList[i][0];
        roommateIndex = residentRankingList[i][1];
        std::cout << records[residentIndex][0] << " <--> " << records[roommateIndex][0] << '\n';
        updateData(dir, records[residentIndex][0], "Roommate_ID", records[roommateIndex][0]);
    }
}

void algorithmPhase1(RankingList &residentRankingList) {
    // ALGORITHM PHASE 1 

    int proposedResident;
    std::vector<int> acceptedProposals(residentRankingList.size(), -1);                   // -1 signifies there is no current match
    std::vector<int> offeredProposals(residentRankingList.size(), -1);                   // -1 signifies there is no current match
    // each proposer will propose to other residents in order of preference
    int proposer = 0;
    int proposerCounter = 0;
    std::vector<int> timesRejected(residentRankingList.size(), 0);

    while (proposerCounter < residentRankingList.size()) {
        bool exit = false;
        for (int j = timesRejected[proposer] + 1; !exit && j < residentRankingList.size(); j++) {                      // check all columns except first (first column is proposer)
            proposedResident = residentRankingList[proposer][j];
            int prevMatch = acceptedProposals[proposedResident];
            // proposedResident currently has no match, so match them together
            if (acceptedProposals[proposedResident] == -1) {
                acceptedProposals[proposedResident] = proposer;
                offeredProposals[proposer] = proposedResident;
                proposer = ++proposerCounter;               // next proposer is simply next in resident list
                break;
            }
            // otherwise check to see if match is better than proposedResident's current match
            else {
                for (int k = 1; k < residentRankingList.size(); k++) {
                    if (residentRankingList[proposedResident][k] == prevMatch) {
                        // make no changes if current match is better (earlier in list)
                        timesRejected[proposer]++;
                        // proposer stays the same
                        break;
                    }
                    else if (residentRankingList[proposedResident][k] == proposer) {
                        timesRejected[prevMatch]++;
                        acceptedProposals[proposedResident] = proposer;           // new match is better, replace
                        offeredProposals[proposer] = proposedResident;
                        proposer = prevMatch;                           // next proposer is the rejected resident
                        exit = true;                                    // no need to look at other columns, move on to next row
                        break;
                    }
                }
            }
        }
    }

    int stableMatches = 0;

    for (int i = 0; i < residentRankingList.size(); i++) {
        /* all the elements before offeredProposals[i] and after acceptedProposals[i] will be reduced (set to -1) without checking
           this doesn't include column 0 since that refers to the resident which is proposing
           for the elements in between, check if a better option is possible for the proposedResident (if not, reduce that element for the proposer)
        */
        bool checkToReduce = false;
        for (int j = 1; j < residentRankingList[i].size(); j++) {
            if (checkToReduce) {
                if (residentRankingList[i][j] == acceptedProposals[i]) {
                    checkToReduce = false;
                }
                else {
                    proposedResident = residentRankingList[i][j];
                    for (int k = 1; k < residentRankingList.size(); k++) {
                        // break from the loop if current proposer is found before acceptedProposal[proposedResident]
                        if (residentRankingList[proposedResident][k] == i) {
                            break;
                        }
                        // if acceptedProposal[proposedResident] is found first, reduce the proposed resident
                        if (residentRankingList[proposedResident][k] == acceptedProposals[proposedResident]) {
                            // found other element before proposedResident, better option avaliable
                            residentRankingList[i][j] = -1;
                            break;
                        }
                    }
                }
            }
            else if (residentRankingList[i][j] == offeredProposals[i]) {
                // if offeredProposal and acceptedProposal are the same, all the only element that isn't reduced so we don't need to check
                if (offeredProposals[i] != acceptedProposals[i]) {
                    checkToReduce = true;
                    stableMatches++;
                }
            }
            else {
                residentRankingList[i][j] = -1;         // reduce this element from list
            }
        }
    }

    // get rid of -1s
    for (int i = 0; i < residentRankingList.size(); i++) {
        residentRankingList[i].erase(std::remove(residentRankingList[i].begin(), residentRankingList[i].end(), -1), residentRankingList[i].end());
    }

    if (stableMatches != residentRankingList.size()) {
        // all matches are not yet stable, need phase 2
        algorithmPhase2(residentRankingList);
    }
}

void algorithmPhase2(RankingList& residentRankingList) {
    // ALGORITHM PHASE 2

    int secondPref, lastPref;

    /* get second preference of resident with more than one choice
       get last preference of that second preference
       remove second preference from last preference's list and vice versa
       continue until original resident found again 
    */

    for (int i = 0; i < residentRankingList.size(); i++) {
        // check if resident has more than 1 choice (residentRankingList[i].size() also includes original resident)
        if (residentRankingList[i].size() > 2) {
            lastPref = i + 1;
            std::cout << "i: " << lastPref << '\n';
            do {
                secondPref = residentRankingList[lastPref][1];
                lastPref = residentRankingList[secondPref].back();      // gets last element of row
                // remove secondPref and leastPref from each other's lists
                residentRankingList[secondPref].pop_back();                     // remove last element
                residentRankingList[lastPref].erase(std::remove(residentRankingList[lastPref].begin(), residentRankingList[lastPref].end(), secondPref), residentRankingList[lastPref].end());
            } while (i != lastPref);
            residentRankingList[i].erase(std::remove(residentRankingList[i].begin(), residentRankingList[i].end(), -1), residentRankingList[i].end());
        }
    }
}
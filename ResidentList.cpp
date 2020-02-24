#include <iostream>
#include <string>
#include <limits>

enum level {
    low = 0,
    moderate = 1,
    high = 2
};

enum time {
    morning = 0,
    evening = 1,
    noPreference = 2
};

struct ResidentNode {
    std::string id;
    ResidentNode* roommate = NULL;
    std::string firstName, lastName;
    level temperature, cleanliness, visitors;
    int smoker;
    time showers;

    ResidentNode* next;
};

class ResidentList {
private:
    ResidentNode * head, * tail;
    const char* levelNames[3] = { "Low", "Moderate", "High" };
    const char* timeNames[3] = { "Mornings", "Afternoons", "No preference" };
    const char* yesNo[2] = { "Yes", "No" };
public:
    ResidentList() {
        head = NULL;
        tail = NULL;
    }

    std::string addResident(std::string firstName, std::string lastName, int temperature,
        int cleanliness, int visitors, int showers, int smoker) {
        ResidentNode* tmp = new ResidentNode;
        // tmp->roommate = roommate;
        tmp->firstName = firstName;
        tmp->lastName = lastName;
        tmp->temperature = (level)(temperature - 1);
        tmp->cleanliness = (level)(cleanliness - 1);
        tmp->visitors = (level)(visitors - 1);
        tmp->showers = (time)(showers - 1);
        tmp->smoker = smoker - 1;
        tmp->id = "";
        tmp->id += std::tolower(firstName[0]);
        tmp->id += std::tolower(lastName[0]);
        tmp->id += "1";

        tmp->next = NULL;

        if (head == NULL) {
            head = tmp;
            tail = tmp;
        }
        else {
            tail->next = tmp;
            tail = tail->next;
        }
        return tmp->id;
    }

    struct ResidentNode* searchNode(std::string id) {         // returns node based on searching id
        ResidentNode* curr = head;
        while (curr) {
            if (curr->id == id) {
                return curr;
            }
            curr = curr->next;
        }
        return NULL;
    }

    bool deleteNode(std::string id) {
        struct ResidentNode * tmp = head, * prev = NULL;

        if (tmp != NULL && tmp->id == id) {        // if deleting head, change head
            head = tmp->next;
            free(tmp);
            return true;
        }

        // find node to delete
        while (tmp != NULL && tmp->id != id) {
            prev = tmp;
            tmp = tmp->next;
        }

       
        if (tmp == NULL) {
            return false;            // return false if delete was unsuccessful
        }

        prev->next = tmp->next;

        free(tmp);  
        return true;
    }

    void displayResidentInfo(ResidentNode * resident) {
        std::cout << "\nName: " << resident->firstName << ' ' <<  resident->lastName;
        std::cout << "\nRoommate: "; 
        if (resident->roommate == NULL) {
            std::cout << "Unassigned";
        }
        else {
            // TODO: retrieve assigned roommate
        }
        std::cout << "\nPreferences"
                  << "\n-----------"
                  << "\nTemperature: " << levelNames[resident->temperature]
                  << "\nCleanliness: " << levelNames[resident->cleanliness]
                  << "\nVisitors: " << levelNames[resident->visitors]
                  << "\nShowers: " << timeNames[resident->showers]
                  << "\nSmoker: " << yesNo[resident->smoker];
    }

    void displayEditMenu(ResidentNode* resident) {
        std::cout << "\n1. First name: " << resident->firstName
                  << "\n2. Last name: " << resident->lastName
                  << "\n3. Temperature: " << levelNames[resident->temperature]
                  << "\n4. Cleanliness: " << levelNames[resident->cleanliness]
                  << "\n5. Visitors: " << levelNames[resident->visitors]
                  << "\n6. Showers: " << timeNames[resident->showers]
                  << "\n7. Smoker: " << yesNo[resident->smoker]
                  << "\n8. Cancel";
    }

    bool isEmpty() {
        if (head == NULL) {
            return true;
        }
        return false;
    }
};
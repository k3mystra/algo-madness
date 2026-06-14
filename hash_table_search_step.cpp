#include <iostream>
#include <ostream>
#include <string>
#include <optional>
#include <vector>
#include <cstdint>
#include <fstream>


// Standardize input type
typedef uint64_t Number;


struct Node {
    std::string value;
    Node* next;
};

// Linked-list implementation
class HashTable {
public:
    HashTable(int size);
    ~HashTable();
    void insert(Number key, std::string &value);
    void printData();

private:
    std::vector<std::optional<Node*>> data;
    int hashValue(Number value);
};

HashTable::HashTable(int size) {
    // std::optional default-init to empty
    data.resize(size);
}

HashTable::~HashTable() {
    for (int i = 0; i < data.size(); i++) {
        if (!data[i].has_value())
            continue;

        Node* currNodePtr = data[i].value();
        while (currNodePtr->next != nullptr) {
            Node* nextNodePtr = currNodePtr->next;
            delete currNodePtr;
            currNodePtr = nextNodePtr;
        }

        data[i].reset();
    }
}

int HashTable::hashValue(Number numValue) {
    // Assume table is fixed size
    return numValue % data.capacity();
}

void HashTable::insert(Number key, std::string &value) {
    Node* nodePtr = new Node {
        .value = value,
        .next = nullptr
    };

    int hashedKey = hashValue(key);
    if (!data[hashedKey].has_value())
        data[hashedKey] = nodePtr;
    else {
        Node* currTraversedNodePtr = data[hashedKey].value();
        while (currTraversedNodePtr->next != nullptr)
            currTraversedNodePtr = currTraversedNodePtr->next;
        currTraversedNodePtr->next = nodePtr;
    }
};

void HashTable::printData() {
    for (int i = 0; i < data.size(); i++) {
        std::optional<Node *> node = data[i];
        std::cout << i << " ";
        if (!node.has_value())
            std::cout << "None" << std::endl;
        else {
            Node* currNodePtr = node.value();
            std::cout << currNodePtr->value;
            while (currNodePtr->next != nullptr) {
                std::cout << " -> " << currNodePtr->next->value;
                currNodePtr = currNodePtr->next;
            }

            std::cout << std::endl;
        }
    }
}


int extractN(const std::string& filename) {
    // Find the underscore and dot positions
    size_t underscore = filename.find('_');
    size_t dot = filename.find(".csv");

    if (underscore == std::string::npos || dot == std::string::npos)
        throw std::invalid_argument("Invalid filename format");

    std::string num_str = filename.substr(underscore + 1, dot - underscore - 1);
    return std::stoi(num_str);
}

int main (int argc, char *argv[]) {
    // Ask for target
    // Find target while listing
    // Write output in specified output file
    std::string datasetFilename = "dataset_1000.csv";

    int tableSize = extractN(datasetFilename);
    HashTable table(tableSize);

    std::ifstream datasetFile(datasetFilename);
    std::string line;
    while(std::getline(datasetFile, line)) {
        size_t comma = line.find(',');

        std::string keyStr = line.substr(0, comma);
        std::string word = line.substr(comma + 1);

        std::string value = keyStr + "/" + word;
        Number key = std::stol(keyStr);

        table.insert(key, value);
    }

    table.printData();

    Number target;
    std::cout << "Target to search: ";
    std::cin >> target;

    return 0;
}

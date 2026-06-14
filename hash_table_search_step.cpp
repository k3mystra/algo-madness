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
    Number valueNum;
    std::string valueStr;
    Node* next;
};

// Linked-list implementation
class HashTable {
public:
    HashTable(int size);
    ~HashTable();
    void insert(Number key, std::string &valueStr);
    void printData();
    std::string searchValue(Number key);

private:
    std::vector<Node*> data;
    int hashValue(Number value);
};

HashTable::HashTable(int size) {
    // std::optional default-init to empty
    data.resize(size);
}

HashTable::~HashTable() {
    for (int i = 0; i < data.size(); i++) {
        if (data[i] == nullptr)
            continue;

        Node* currNodePtr = data[i];
        while (currNodePtr->next != nullptr) {
            Node* nextNodePtr = currNodePtr->next;
            delete currNodePtr;
            currNodePtr = nextNodePtr;
        }

        data[i] = nullptr;
    }
}

int HashTable::hashValue(Number numValue) {
    // Assume table is fixed size
    return numValue % data.capacity();
}

void HashTable::insert(Number key, std::string &value) {
    Node* nodePtr = new Node {
        .valueNum = key,
        .valueStr = value,
        .next = nullptr
    };

    int hashedKey = hashValue(key);
    if (data[hashedKey] == nullptr)
        data[hashedKey] = nodePtr;
    else {
        Node* currTraversedNodePtr = data[hashedKey];
        while (currTraversedNodePtr->next != nullptr)
            currTraversedNodePtr = currTraversedNodePtr->next;
        currTraversedNodePtr->next = nodePtr;
    }
};

void HashTable::printData() {
    for (int i = 0; i < data.size(); i++) {
        Node* node = data[i];
        std::cout << i << " ";
        if (node == nullptr)
            std::cout << "None" << std::endl;
        else {
            Node* currNodePtr = node;
            std::cout << currNodePtr->valueNum << '/' << currNodePtr->valueStr;
            currNodePtr = currNodePtr->next;
            while (currNodePtr != nullptr) {
                std::cout << " -> " << currNodePtr->valueNum << '/' << currNodePtr->valueStr;
                currNodePtr = currNodePtr->next;
            }

            std::cout << std::endl;
        }
    }
}

std::string HashTable::searchValue(Number key) {
    Number hashedKey = this->hashValue(key);
    Node* node = data[hashedKey];

    if (node == nullptr)
        return "-1";

    do {
        if (key == node->valueNum)
            return std::to_string(node->valueNum) + "/" + node->valueStr;
        node = node->next;
    }
    while (node == nullptr);

    return "-1";
}


int extractDatasetSize(const std::string& filename) {
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

    int tableSize = extractDatasetSize(datasetFilename);
    HashTable table(tableSize);

    std::ifstream datasetFile(datasetFilename);
    std::string line;
    while(std::getline(datasetFile, line)) {
        size_t comma = line.find(',');

        Number key = std::stol(line.substr(0, comma));
        std::string word = line.substr(comma + 1);

        table.insert(key, word);
    }

    table.printData();

    Number target;
    std::cout << "Target to search: ";
    std::cin >> target;

    std::string result = table.searchValue(target);
    std::cout << "Result: " << result << std::endl;

    return 0;
}

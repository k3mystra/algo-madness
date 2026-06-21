// *******************
// Program: hash_table_search_step.cpp
// Course: CCP6214 Algorithm Design and Analysis
// Lecture Class: TC6L
// Tutorial Class: T21L
// Trimester: 2610
// Member_1: 242UC244SC | AMIRUL HILMAN BIN MOHD YAZID | AMIRUL.HILMAN.MOHD@STUDENT.MMU.EDU.MY | 0125029714
// Member_2: 242UC240JF | IZZMINHAL AKMAL BIN NORHISYAM| IZZMINHAL.AKMAL.NORHISYAM1@STUDENT.MMU.EDU.MY | 01140348650
// Member_3: 242UC244LB | TIMOTHY KEW WEN TZUN | TIMOTHY-KEW.WEN.TZUN@STUDENT.MMU.EDU.MY | 0123456789
// Member_4: 242UC244S7 | Sivasubramaniam A/L Venkateswaran| SIVASUBRAMANIAM.A/L.VENKATESWARAN@STUDENT.MMU.EDU.MY | 0123456789
// *******************
// Task Distribution
// Member_1:HANDLES THE RADIX SORT 
// Member_2:HANDLES THE HASH TABLE SEARCH 
// Member_3:HANDLES THE DATASET GENERATION
// Member_4:HANDLES THE HEAP SORT 
// **


#include <iostream>
#include <ostream>
#include <string>
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
    std::string datasetFilename = "dataset_1000.csv";
    // std::string datasetFilename = "ataset_5000.csv";
    // std::string datasetFilename = "ataset_10000.csv";
    // std::string datasetFilename = "ataset_50000.csv";
    // std::string datasetFilename = "ataset_100000.csv";
    // std::string datasetFilename = "ataset_200000.csv";
    // std::string datasetFilename = "ataset_400000.csv";
    // std::string datasetFilename = "ataset_600000.csv";
    // std::string datasetFilename = "ataset_800000.csv";
    // std::string datasetFilename = "ataset_1000000.csv";

    // Optional override for quick testing: radix_sort.exe <file>
    if (argc >= 2) datasetFilename = argv[1];

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
    datasetFile.close();

    table.printData();

    Number target;
    std::cout << "Target to search: ";
    std::cin >> target;

    std::string result = table.searchValue(target);
    std::cout << "Result: " << result << std::endl;

    std::ofstream outputFile("hash_table_search_step_" + std::to_string(target) + ".txt");
    if (result != "-1")
        outputFile << target << " = " << result << std::endl;
    else
        outputFile << "-1 != " << target << std::endl;

    outputFile.close();

    return 0;
}

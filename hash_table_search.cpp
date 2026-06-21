// *******************
// Program: hash_table_search.cpp
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

#include <ios>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <cstdint>
#include <fstream>
#include <chrono>
#include <random>


// Standardize input type
typedef uint64_t Number;

const Number MAX_VAL = 9999999999;
const Number MIN_VAL = 1000000000;

const float DATASET_SAMPLE_PERCENTAGE = 0.1;
const Number SAMPLING_COUNT = 100;

struct Node {
    Number valueNum;
    std::string valueStr;
    Node* next;
};

// Linked-list implementation
class HashTable {
public:
    HashTable(Number size);
    ~HashTable();
    void insert(Number key, std::string &valueStr);
    void printData();
    std::string searchValue(Number key);

private:
    std::vector<Node*> data;
    int hashValue(Number value);
};

HashTable::HashTable(Number size) {
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
    while (node != nullptr);

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
    // std::string datasetFilename = "dataset_1000.csv";
    // std::string datasetFilename = "ataset_5000.csv";
    // std::string datasetFilename = "ataset_10000.csv";
    // std::string datasetFilename = "ataset_50000.csv";
    // std::string datasetFilename = "ataset_100000.csv";
    // std::string datasetFilename = "ataset_200000.csv";
    // std::string datasetFilename = "ataset_400000.csv";
    // std::string datasetFilename = "ataset_600000.csv";
    // std::string datasetFilename = "ataset_800000.csv";
    // std::string datasetFilename = "ataset_1000000.csv";

    std::string datasetFilename = argv[1];

    Number tableSize = extractDatasetSize(datasetFilename);
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

    Number sampleSize = tableSize * DATASET_SAMPLE_PERCENTAGE;

    std::mt19937_64 rng(std::random_device{}());
    std::uniform_int_distribution<Number> randomTargetGenerator(MIN_VAL, MAX_VAL);

    double minSampleDuration, maxSampleDuration, totalDuration = 0;
    for (Number n = 0; n < SAMPLING_COUNT; n++) {
        long currentSampleDuration;
        for (Number i = 0; i < sampleSize; i++) {
            Number target = randomTargetGenerator(rng);
            
            // Timing for search
            auto startTime = std::chrono::high_resolution_clock::now();
            table.searchValue(target);
            auto endTime = std::chrono::high_resolution_clock::now();

            currentSampleDuration += std::chrono::duration<double, std::nano>(endTime - startTime).count();
        }

        if (n == 0) {
            minSampleDuration = currentSampleDuration;
            maxSampleDuration = currentSampleDuration;
        }

        if (currentSampleDuration > maxSampleDuration)
            maxSampleDuration = currentSampleDuration;

        if (currentSampleDuration < minSampleDuration)
            minSampleDuration = currentSampleDuration;

        totalDuration += currentSampleDuration;
        currentSampleDuration = 0;
    }

    std::ostringstream ss;

    ss << std::fixed << std::setprecision(2);
    ss << "=== Setup ===" << '\n';
    ss << "Sample size: " << sampleSize << " (" << DATASET_SAMPLE_PERCENTAGE * 100.0 << "% of total dataset)" << '\n';
    ss << "No. of samples: " << SAMPLING_COUNT << '\n';

    ss << '\n';

    ss << std::fixed << std::setprecision(4);
    ss << "=== Result (by samples) ===" << '\n';
    ss << "Best: " << minSampleDuration / 1000 << "µs" << '\n';
    ss << "Worst: " << maxSampleDuration / 1000 << "µs" << '\n';
    ss << "Avg: " << totalDuration / SAMPLING_COUNT / 1000 << "µs" << '\n';

    // Output to cout and file
    std::cout << ss.str();

    std::ofstream outputFile("hash_table_search_dataset_" + std::to_string(tableSize) + ".txt");
    outputFile << ss.str();
    outputFile.close();

    return 0;
}

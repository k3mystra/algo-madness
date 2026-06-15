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
    std::string datasetFilename = "dataset_1000.csv";

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

    // Get sample proportion
    // Get no. of sample to take
    // Total timing for each sample

    float sampleProportion;
    std::cout << "Dataset proportion for each sample [0,1]: ";
    std::cin >> sampleProportion;

    Number sampleSize = tableSize * sampleProportion;

    Number sampleNum;
    std::cout << "No. of samples to take: ";
    std::cin >> sampleNum;

    std::mt19937_64 rng(std::random_device{}());
    std::uniform_int_distribution<Number> randomTargetGenerator(MIN_VAL, MAX_VAL);

    double minSampleDuration, maxSampleDuration, totalDuration = 0;
    for (Number n = 0; n < sampleNum; n++) {
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

    ss << std::fixed << std::setprecision(4);

    ss << "=== Setup ===" << '\n';
    ss << "Sample size: " << sampleSize << " (" << sampleProportion * 100.0 << "% of total dataset)" << '\n';
    ss << "No. of samples: " << sampleNum << '\n';

    ss << '\n';

    ss << "=== Result (by samples) ===" << '\n';
    ss << "Best: " << minSampleDuration / 1000 << "ms" << '\n';
    ss << "Worst: " << maxSampleDuration / 1000 << "ms" << '\n';
    ss << "Avg: " << totalDuration / sampleNum / 1000 << "ms" << '\n';

    // Output to cout and file
    std::cout << ss.str();

    std::ofstream outputFile("hash_table_search_dataset_" + std::to_string(tableSize) + ".txt");
    outputFile << ss.str();
    outputFile.close();

    return 0;
}

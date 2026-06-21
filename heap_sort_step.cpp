// *********************************************************
// Program: heap_sort_step.cpp
// Course: CCP6214 Algorithm Design and Analysis
// Lecture Class: TC6L
// Tutorial Class: T21L
// Trimester: 2610
// Member_1: 242UC244SC | AMIRUL HILMAN BIN MOHD YAZID | AMIRUL.HILMAN.MOHD@STUDENT.MMU.EDU.MY | 0125029714
// Member_2: 242UC240JF | IZZMINHAL AKMAL BIN NORHISYAM | IZZMINHAL.AKMAL.NORHISYAM1@STUDENT.MMU.EDU.MY | 01140348650
// Member_3: 242UC244LB | TIMOTHY KEW WEN TZUN | TIMOTHY.KEW.WEN@STUDENT.MMU.EDU.MY| 01137336763
// Member_4: 242UC244S7 | SIVASUBRAMANIAM A/L VENKATESWARAN | SIVASUBRAMANIAM.VENKATESW@STUDENT.MMU.EDU.MY | 0164263158
// *********************************************************
// Task Distribution
// Member_1: Handles Radix Sort
// Member_2: Handles Hash Table Search
// Member_3: Handles Dataset Generation
// Member_4: Handles Heap Sort
// *********************************************************

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

struct Record {
    long long integer;
    string str;
};

void printArray(Record arr[], int n, ofstream &outFile, string label) {
    outFile << "[";
    for (int i = 0; i < n; i++) {
        outFile << arr[i].integer << "/" << arr[i].str;
        if (i < n - 1)
            outFile << ", ";
    }
    outFile << "] " << label << endl;
}

void heapify(Record arr[], int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left].integer > arr[largest].integer)
        largest = left;

    if (right < n && arr[right].integer > arr[largest].integer)
        largest = right;

    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

void heapSortStep(Record arr[], int n, ofstream &outFile) {
    // Build max heap
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // Print initial state after building max heap
    printArray(arr, n, outFile, "initial");

    // Extract elements one by one
    for (int i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
        printArray(arr, n, outFile, "i = " + to_string(i));
    }
}

int main() {
    // string filename = "dataset_1000.csv";
    // string filename = "dataset_5000.csv";
    // string filename = "dataset_10000.csv";
    // string filename = "dataset_50000.csv";
    // string filename = "dataset_100000.csv";
    // string filename = "dataset_200000.csv";
    // string filename = "dataset_400000.csv";
    // string filename = "dataset_600000.csv";
    // string filename = "dataset_800000.csv";
    string filename = "dataset_1000000.csv";

    
    // int startRow = 1, endRow = 10;
    int startRow = 1, endRow = 7;

    ifstream inFile(filename);
    if (!inFile) {
        cout << "Error: Cannot open file " << filename << endl;
        return 1;
    }

    // Skip to start row
    string line;
    int currentRow = 1;
    while (currentRow < startRow && getline(inFile, line))
        currentRow++;

    // Read from startRow to endRow
    int n = endRow - startRow + 1;
    Record* arr = new Record[n];

    for (int i = 0; i < n; i++) {
        if (!getline(inFile, line)) {
            cout << "Error: Not enough rows in file" << endl;
            delete[] arr;
            return 1;
        }
        int commaPos = line.find(',');
        arr[i].integer = stoll(line.substr(0, commaPos));
        arr[i].str = line.substr(commaPos + 1);
    }
    inFile.close();

    // Output filename
    string outputFilename = filename.substr(0, filename.find('.')) + 
                           "_heap_sorted_step_" + 
                           to_string(startRow) + "_" + 
                           to_string(endRow) + ".txt";

    ofstream outFile(outputFilename);
    if (!outFile) {
        cout << "Error: Cannot create output file" << endl;
        delete[] arr;
        return 1;
    }

    heapSortStep(arr, n, outFile);

    outFile.close();

    cout << "Input file: " << filename << endl;
    cout << "Start row: " << startRow << ", End row: " << endRow << endl;
    cout << "Output file: " << outputFilename << endl;

    delete[] arr;
    return 0;
}

// *********************************************************
// Program: heap_sort.cpp
// Course: CCP6214 Algorithm Design and Analysis
// Lecture Class: TC6L
// Tutorial Class: T21L
// Trimester: 2610
// Member_1: 242UC244SC | AMIRUL HILMAN BIN MOHD YAZID | AMIRUL.HILMAN.MOHD@STUDENT.MMU.EDU.MY | 0125029714
// Member_2: 242UC240JF | IZZMINHAL AKMAL BIN NORHISYAM | IZZMINHAL.AKMAL.NORHISYAM@STUDENT.MMU.EDU.MY | 01140348650
// Member_3: 242UC244LB | TIMOTHY KEW WEN TZUN | TIMOTHY-KEW.WEN.TZUN@STUDENT.MMU.EDU.MY | 01137336763
// Member_4: 242UC244S7 | SIVASUBRAMANIAM A/L VENKATESWARAN | SIVASUBRAMANIAM.A/L.VENKATESWARAN@STUDENT.MMU.EDU.MY | 0164263158
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
#include <chrono>
using namespace std;

struct Record {
    long long integer;
    string str;
};

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

void heapSort(Record arr[], int n) {
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    for (int i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
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

    ifstream inFile(filename);
    if (!inFile) {
        cout << "Error: Cannot open file " << filename << endl;
        return 1;
    }

    int n = 0;
    string line;
    while (getline(inFile, line))
        n++;
    inFile.close();

    Record* arr = new Record[n];

    inFile.open(filename);
    for (int i = 0; i < n; i++) {
        getline(inFile, line);
        int commaPos = line.find(',');
        arr[i].integer = stoll(line.substr(0, commaPos));
        arr[i].str = line.substr(commaPos + 1);
    }
    inFile.close();

    auto start = chrono::high_resolution_clock::now();
    heapSort(arr, n);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - start;
    double runningTime = elapsed.count();

    string outputFilename = "heap_sort_" + filename;

    ofstream outFile(outputFilename);
    if (!outFile) {
        cout << "Error: Cannot create output file" << endl;
        delete[] arr;
        return 1;
    }

    outFile << "Running time: " << runningTime << " seconds" << endl;
    outFile << endl;

    for (int i = 0; i < n; i++)
        outFile << arr[i].integer << "/" << arr[i].str << endl;

    outFile.close();

    cout << "Input file: " << filename << endl;
    cout << "Number of elements: " << n << endl;
    cout << "Running time: " << runningTime << " seconds" << endl;
    cout << "Output file: " << outputFilename << endl;

    delete[] arr;
    return 0;
}

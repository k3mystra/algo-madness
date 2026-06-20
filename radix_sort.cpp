// *********************************************************
// Program: radix_sort.cpp
// Course: CCP6214 Algorithm Design and Analysis
// Lecture Class: TC4L
// Tutorial Class: T13L
// Trimester: 2610
// Member_1: ID | NAME | EMAIL | PHONE
// Member_2: ID | NAME | EMAIL | PHONE
// Member_3: ID | NAME | EMAIL | PHONE
// Member_4: ID | NAME | EMAIL | PHONE
// *********************************************************
// Radix Sort (LSD, base 10) processing from the rightmost digit.
// Sorts the records by their 10-digit integer field and writes the
// full sorted dataset to an output csv. Timing covers the sort ONLY
// (file reading and writing are excluded), as required by the brief.
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

// Stable LSD radix sort, base 10. Processes the least significant
// (rightmost) digit first up to the most significant digit.
void radixSort(Record arr[], int n) {
    if (n <= 1) return;

    long long maxVal = arr[0].integer;
    for (int i = 1; i < n; i++)
        if (arr[i].integer > maxVal) maxVal = arr[i].integer;

    Record* buffer = new Record[n];

    for (long long exp = 1; maxVal / exp > 0; exp *= 10) {
        int count[10] = {0};

        // Count occurrences of each digit at the current position.
        for (int i = 0; i < n; i++)
            count[(arr[i].integer / exp) % 10]++;

        // Prefix sums -> ending positions for each digit bucket.
        for (int d = 1; d < 10; d++)
            count[d] += count[d - 1];

        // Build the output array; iterate from the back to stay stable.
        for (int i = n - 1; i >= 0; i--) {
            int digit = (arr[i].integer / exp) % 10;
            buffer[--count[digit]] = arr[i];
        }

        // Copy this pass back into the original array.
        for (int i = 0; i < n; i++)
            arr[i] = buffer[i];
    }

    delete[] buffer;
}

int main(int argc, char* argv[]) {
    // ----- Dataset selection (uncomment ONE line) -----
    string filename = "dataset_1000.csv";
    // string filename = "dataset_5000.csv";
    // string filename = "dataset_10000.csv";
    // string filename = "dataset_50000.csv";
    // string filename = "dataset_100000.csv";
    // string filename = "dataset_200000.csv";
    // string filename = "dataset_400000.csv";
    // string filename = "dataset_600000.csv";
    // string filename = "dataset_800000.csv";
    // string filename = "dataset_1000000.csv";

    // Optional override for quick testing: radix_sort.exe <file>
    if (argc >= 2) filename = argv[1];

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
        if (!line.empty() && line.back() == '\r') line.pop_back(); // strip CRLF
        int commaPos = line.find(',');
        arr[i].integer = stoll(line.substr(0, commaPos));
        arr[i].str = line.substr(commaPos + 1);
    }
    inFile.close();

    // ----- Timed region: sorting only (no I/O) -----
    auto start = chrono::high_resolution_clock::now();
    radixSort(arr, n);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - start;
    double runningTime = elapsed.count();

    string outputFilename = "radix_sort_" + filename;

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

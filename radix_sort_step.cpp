// *********************************************************
// Program: radix_sort_step.cpp
// Course: CCP6214 Algorithm Design and Analysis
// Lecture Class: TC4L
// Tutorial Class: T13L
// Trimester: 2610
// Member_1: ID | NAME | EMAIL | PHONE
// Member_2: ID | NAME | EMAIL | PHONE
// Member_3: ID | NAME | EMAIL | PHONE
// Member_4: ID | NAME | EMAIL | PHONE
// *********************************************************
// Radix Sort STEP tracer (LSD, base 10, rightmost digit first).
// Reads rows [START_ROW .. END_ROW] (1-based, inclusive) from the
// dataset and prints the array after every digit pass, labelled
// d=<position> where position 1 = leftmost digit and the largest
// number = rightmost digit (e.g. d=10 for 10-digit integers).
// *********************************************************

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

struct Record {
    long long integer;
    string str;
};

// Print the array as [int/str, int/str, ...] <tag> to the given stream.
void printArray(ostream& os, Record arr[], int n, const string& tag) {
    os << "[";
    for (int i = 0; i < n; i++) {
        os << arr[i].integer << "/" << arr[i].str;
        if (i + 1 < n) os << ", ";
    }
    os << "] " << tag << "\n";
}

int main(int argc, char* argv[]) {
    // ----- Dataset selection (uncomment ONE line) -----
    string filename = "dataset_1000.csv";
    // string filename = "dataset_5000.csv";
    // string filename = "dataset_10000.csv";
    // string filename = "dataset_100000.csv";

    // ----- Row range to trace (1-based, inclusive) -----
    int startRow = 1;
    int endRow   = 7;

    // Optional overrides for testing: radix_sort_step.exe <file> <start> <end>
    if (argc >= 2) filename = argv[1];
    if (argc >= 4) { startRow = stoi(argv[2]); endRow = stoi(argv[3]); }

    if (startRow < 1 || endRow < startRow) {
        cout << "Error: invalid row range (need 1 <= start <= end)." << endl;
        return 1;
    }

    ifstream inFile(filename);
    if (!inFile) {
        cout << "Error: Cannot open file " << filename << endl;
        return 1;
    }

    // Read only the requested rows; count total rows (N) for the filename.
    int m = endRow - startRow + 1;
    Record* arr = new Record[m];
    int stored = 0;
    int totalRows = 0;
    string line;
    while (getline(inFile, line)) {
        totalRows++;
        if (totalRows >= startRow && totalRows <= endRow) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            int commaPos = line.find(',');
            arr[stored].integer = stoll(line.substr(0, commaPos));
            arr[stored].str = line.substr(commaPos + 1);
            stored++;
        }
    }
    inFile.close();

    if (endRow > totalRows) {
        cout << "Error: end row " << endRow << " exceeds dataset rows (" << totalRows << ")." << endl;
        delete[] arr;
        return 1;
    }

    // Number of digits in the largest value -> number of passes.
    long long maxVal = arr[0].integer;
    for (int i = 1; i < m; i++)
        if (arr[i].integer > maxVal) maxVal = arr[i].integer;
    int digits = 0;
    for (long long t = maxVal; t > 0; t /= 10) digits++;
    if (digits == 0) digits = 1;

    string outputFilename = "dataset_" + to_string(totalRows) +
                            "_radix_sorted_step_" + to_string(startRow) +
                            "_" + to_string(endRow) + ".txt";
    ofstream outFile(outputFilename);
    if (!outFile) {
        cout << "Error: Cannot create output file" << endl;
        delete[] arr;
        return 1;
    }

    // Show the original (unsorted) subset first.
    printArray(outFile, arr, m, "original");
    printArray(cout,    arr, m, "original");

    Record* buffer = new Record[m];
    long long exp = 1;
    for (int k = 0; k < digits; k++) {
        int count[10] = {0};

        for (int i = 0; i < m; i++)
            count[(arr[i].integer / exp) % 10]++;

        for (int d = 1; d < 10; d++)
            count[d] += count[d - 1];

        for (int i = m - 1; i >= 0; i--) {
            int digit = (arr[i].integer / exp) % 10;
            buffer[--count[digit]] = arr[i];
        }

        for (int i = 0; i < m; i++)
            arr[i] = buffer[i];

        // Label: rightmost pass = highest position number (e.g. d=10).
        string tag = "d=" + to_string(digits - k);
        printArray(outFile, arr, m, tag);
        printArray(cout,    arr, m, tag);

        exp *= 10;
    }

    outFile.close();
    cout << "Output file: " << outputFilename << endl;

    delete[] buffer;
    delete[] arr;
    return 0;
}

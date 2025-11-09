
#include <vector>
#include <string>
#include <unordered_set>
#include <iostream>
#include <algorithm>
#include <map>
#include <iomanip>

using namespace std;
vector<string> boolexpr(int numVar, vector<string> primes_Dec) {
    vector<string> primes_bool;
    
    // Helper function to convert a decimal number to its binary representation
    auto to_binary = [numVar](int decimal) {
        string binary;
        for (int i = 0; i < numVar; i++) {
            binary = ((decimal & 1) ? '1' : '0') + binary;
            decimal >>= 1;
        }
        return binary;
    };
    
    // Process each prime implicant
    for (const string& prime_dec : primes_Dec) {
        // Parse the decimal values
        vector<int> decimals;
        size_t start = 0, end = 0;
        while ((end = prime_dec.find(',', start)) != string::npos) {
            decimals.push_back(stoi(prime_dec.substr(start, end - start)));
            start = end + 1;
        }
        decimals.push_back(stoi(prime_dec.substr(start))); // Add the last number
        
        // Convert first decimal to binary
        string binary_pattern = to_binary(decimals[0]);
        
        // For each additional decimal, mark differing bits with '-'
        for (size_t i = 1; i < decimals.size(); i++) {
            string next_binary = to_binary(decimals[i]);
            for (int j = 0; j < numVar; j++) {
                if (binary_pattern[j] != next_binary[j]) {
                    binary_pattern[j] = '-';
                }
            }
        }
        
        // Convert binary pattern to boolean expression
        string boolean_expr = "";
        for (int i = 0; i < numVar; i++) {
            char var = 'a' + i; // Variables start with 'a'
            
            if (binary_pattern[i] == '0') {
                // If bit is 0, add variable with complement
                boolean_expr += var;
                boolean_expr += '\'';
            } else if (binary_pattern[i] == '1') {
                // If bit is 1, add variable without complement
                boolean_expr += var;
            }
            // If bit is '-', skip this variable
        }
        
        primes_bool.push_back(boolean_expr);
    }
    
    return primes_bool;
}

void printImplicantChart(int numVar, vector<int> minterms, vector<string> primes)
{
    vector<string> primes_bool = boolexpr(numVar, primes);
    
    // Sort minterms for better display
    sort(minterms.begin(), minterms.end());
    
    cout << "=====================" << endl;
    cout << "Prime Implicant Chart" << endl;
    cout << "=====================" << endl;
    
    // Find the maximum width needed for each prime implicant
    int max_prime_width = 0;
    for (const string& prime : primes_bool) {
        max_prime_width = max(max_prime_width, static_cast<int>(prime.length()));
    }
    max_prime_width += 2;  // Add some padding
    
    // Print header row with minterm numbers
    cout << string(max_prime_width, ' ') << " |";
    for (int minterm : minterms) {
        cout << setw(4) << minterm << "|";
    }
    cout << endl;
    
    // Print divider line
    cout << " " << string(max_prime_width-1, '-') << "+";
    for (size_t i = 0; i < minterms.size(); i++) {
        cout << "----+";
    }
    cout << endl;
    
    // Helper function to determine if a prime implicant covers a minterm
    auto covers_minterm = [numVar](const string& prime_str, int minterm, const string& prime_decimal) {
        // Parse the decimal values from the prime implicant
        vector<int> prime_minterms;
        size_t start = 0, end = 0;
        while ((end = prime_decimal.find(',', start)) != string::npos) {
            prime_minterms.push_back(stoi(prime_decimal.substr(start, end - start)));
            start = end + 1;
        }
        prime_minterms.push_back(stoi(prime_decimal.substr(start)));
        
        // Check if the minterm is in the list of minterms covered by this prime
        return find(prime_minterms.begin(), prime_minterms.end(), minterm) != prime_minterms.end();
    };
    
    // Print each prime implicant row
    for (size_t i = 0; i < primes_bool.size(); i++) {
        cout << " " << left << setw(max_prime_width-1) << primes_bool[i] << "|";
        
        // Print X or space for each minterm
        for (int minterm : minterms) {
            if (covers_minterm(primes_bool[i], minterm, primes[i])) {
                cout << "   x|";
            } else {
                cout << "    |";
            }
        }
        cout << endl;
    }
    
    // Print bottom divider line
    cout << " " << string(max_prime_width-1, '-') << "+";
    for (size_t i = 0; i < minterms.size(); i++) {
        cout << "----+";
    }
    cout << endl;
}

int main() {
    // Test case 1: Simple example
    cout << "Test Case 1:" << endl;
    int numVar1 = 4;
    vector<int> minterms1 = {0,1,2,5,6,7,8,9,10,14};
    vector<string> primes1 = { "1,5",
                                "5,7",
                                "6,7",
                                "0,1,8,9",
                                "0,2,8,10",
                                "2,6,10,14"};
    printImplicantChart(numVar1, minterms1, primes1);
    cout << endl;

    
    return 0;
}
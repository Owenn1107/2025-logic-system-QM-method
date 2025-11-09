#include <vector>
#include <string>
#include <unordered_set>
#include <iostream>
#include <algorithm>
#include <map>

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
int main() {
    // Test case 1: Example from the problem statement
    cout << "=== Test Case 1 ===" << endl;
    int numVar1 = 8;
    vector<string> primes_Dec1 = {"6,7,14,15","12,13,14,15"," 0,2,8,10"," 5,7,13,15"};
    
    cout << "Input decimals:" << endl;
    for (const auto& prime : primes_Dec1) {
        cout << "  " << prime << endl;
    }
    
    vector<string> result1 = boolexpr(numVar1, primes_Dec1);
    
    cout << "Output boolean expressions:" << endl;
    for (const auto& expr : result1) {
        cout << "  " << expr << endl;
    }
    cout << endl;
    
  
    
    return 0;
}

#include <vector>
#include <string>
#include <unordered_set>
#include <iostream>
#include <algorithm>
#include <map>

using namespace std;


int main() { 
    int numVar = 8; // Example number of variables
    vector<int> minterms = {0, 1, 2, 3, 4, 5, 6, 7,128,20,65,64,63,52,57,98,300,330}; // Example minterms
    vector<int> dontcares = {8, 9, 10, 11,200,100,101}; // Example don't cares
     //fill in the first column by of QM method
    //grouping by counting 1's number
    int column_num = 1;
    vector<vector<string>> cuurrent_Dec;
    vector<vector<string>> cuurrent_Bin;
    
    // Initialize vectors to hold numVar+1 groups (0 to numVar ones)
    cuurrent_Dec.resize(numVar + 1);
    cuurrent_Bin.resize(numVar + 1);
    
    // Helper function to count 1s in binary representation
    auto count_ones = [](int n) {
        int count = 0;
        while (n) {
            count += n & 1;
            n >>= 1;
        }
        return count;
    };
    
    // Helper function to convert to binary string with prefix
    auto to_binary = [numVar](int n, char prefix) {
        string result(1, prefix);  // First character is the prefix
        
        // Generate binary representation with leading zeros
        for (int i = numVar - 1; i >= 0; i--) {
            result += ((n & (1 << i)) ? '1' : '0');
        }
        
        return result;
    };
    
    // Process minterms
    for (int m : minterms) {
        int ones_count = count_ones(m);
        
        // Add decimal form with space prefix
        cuurrent_Dec[ones_count].push_back(to_string(m));
        
        // Add binary form with space prefix
        cuurrent_Bin[ones_count].push_back(to_binary(m, ' '));
    }
    
    // Process don't cares
    for (int d : dontcares) {
        int ones_count = count_ones(d);
        
        // Add decimal form with 'd' prefix
        cuurrent_Dec[ones_count].push_back(to_string(d));
        
        // Add binary form with 'd' prefix
        cuurrent_Bin[ones_count].push_back(to_binary(d, 'd'));
    }
    
    // Print groups for debugging
    cout << "Column " << column_num << ":" << endl;
    for (int i = 0; i <= numVar; i++) {
        if (!cuurrent_Dec[i].empty()) {
            cout << "Group " << i << " (terms with " << i << " ones):" << endl;
            for (size_t j = 0; j < cuurrent_Dec[i].size(); j++) {
                cout << "  " << cuurrent_Bin[i][j] << " (" << cuurrent_Dec[i][j] << ")" << endl;
            }
        }
    }








	return 0;

}
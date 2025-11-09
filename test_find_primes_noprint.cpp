
#include <vector>
#include <string>
#include <unordered_set>
#include <iostream>
#include <algorithm>
#include <map>

using namespace std;


int main() { 
    int numVar = 9; // Example number of variables
    vector<int> minterms = {1, 3, 7, 15, 31, 63, 127, 255}; // Example minterms
    vector<int> dontcares = {0, 2, 4, 8, 16, 32, 64, 128};
    //grouping by counting 1's number
    //fill in the first column by of QM method
    //grouping by counting 1's number
    int column_num = 1;
    vector<vector<string>> current_Dec;  // Store decimal representations
    vector<vector<string>> current_Bin;  // Store binary representations
    
    // Initialize vectors with numVar+1 groups (for 0 to numVar 1's)
    current_Dec.resize(numVar + 1);
    current_Bin.resize(numVar + 1);
    
    // Helper function to count number of 1's in binary representation
    auto count_ones = [](int n) {
        int count = 0;
        while (n) {
            count += n & 1;
            n >>= 1;
        }
        return count;
    };
    
    // Helper function to convert int to binary string with leading zeros
    auto to_binary = [numVar](int n, char prefix) {
        string result(1, prefix);  // First bit is ' ' for minterms or 'd' for don't cares
        
        // Add binary digits
        for (int i = numVar - 1; i >= 0; i--) {
            result += ((n & (1 << i)) ? '1' : '0');
        }
        
        return result;
    };
    
    // Process minterms
    for (int m : minterms) {
        int ones_count = count_ones(m);
        
        // Add decimal form (as string)
        current_Dec[ones_count].push_back(to_string(m));
        
        // Add binary form with ' ' prefix
        current_Bin[ones_count].push_back(to_binary(m, ' '));
    }
    
    // Process dontcares
    for (int d : dontcares) {
        int ones_count = count_ones(d);
        
        // Add decimal form with 'd' prefix
        current_Dec[ones_count].push_back(to_string(d));
        
        // Add binary form with 'd' prefix
        current_Bin[ones_count].push_back(to_binary(d, 'd'));
    }

   

    //now we have the first column of QM method
    vector<vector<string>> next_Dec;  // Store decimal representations
    vector<vector<string>> next_Bin;  // Store binary representations
    vector<string> primes;
    
    bool found_match = true;
    
    // Continue combining terms while matches are found
    while (found_match) {
        found_match = false;
        vector<vector<bool>> current_used(numVar + 1, vector<bool>(numVar + 1, false));

        // Clear next column structures
        next_Dec.clear();
        next_Bin.clear();
        next_Dec.resize(numVar + 1);
        next_Bin.resize(numVar + 1);
        
        // Compare adjacent groups
        for (int i = 0; i < numVar; i++) {
            // Skip if either group is empty
            if (current_Bin[i].empty() || current_Bin[i+1].empty()) continue;
            
            // Compare each term in group i with each term in group i+1
            for (size_t j = 0; j < current_Bin[i].size(); j++) {
                // Skip terms marked with 'x'
                if (current_Bin[i][j][0] == 'x') continue;
                
                for (size_t k = 0; k < current_Bin[i+1].size(); k++) {
                    // Skip terms marked with 'x'
                    if (current_Bin[i+1][k][0] == 'x') continue;
                    
                    // Check if they have the same pattern of '-' characters
                    bool same_dash_pattern = true;
                    for (int pos = 1; pos <= numVar; pos++) {
                        if ((current_Bin[i][j][pos] == '-') != (current_Bin[i+1][k][pos] == '-')) {
                            same_dash_pattern = false;
                            break;
                        }
                    }
                    
                    // If dash patterns match, check for exactly one bit difference
                    if (same_dash_pattern) {
                        int diff_pos = -1;
                        int diff_count = 0;
                        
                        for (int pos = 1; pos <= numVar; pos++) {
                            // Skip positions with '-'
                            if (current_Bin[i][j][pos] == '-') continue;
                            
                            if (current_Bin[i][j][pos] != current_Bin[i+1][k][pos]) {
                                diff_pos = pos;
                                diff_count++;
                            }
                        }
                        
                        // If exactly one bit differs, combine terms
                        if (diff_count == 1) {
                            found_match = true;
                            current_used[i][j] = true;
                            current_used[i+1][k] = true;

                            // Create the combined term
                            string new_bin = current_Bin[i][j];
                            new_bin[diff_pos] = '-';
                            
                            // Set the first bit based on whether both terms are don't cares
                            if (current_Bin[i][j][0] == 'd' && current_Bin[i+1][k][0] == 'd') {
                                new_bin[0] = 'd';  // If both terms are don't cares, the combined term is also a don't care
                            } else {
                                new_bin[0] = ' ';  // Otherwise, it's a regular term
                            }
                             
                            
                            
                            // Create the combined decimal representation
                            string new_dec = current_Dec[i][j] + "," + current_Dec[i+1][k];
                            
                            // Count ones in new term to place in correct group
                            int ones_count = 0;
                            for (int pos = 1; pos <= numVar; pos++) {
                                if (new_bin[pos] == '1') ones_count++;
                            }
                            next_Bin[ones_count].push_back(new_bin);
                            next_Dec[ones_count].push_back(new_dec);
                        }
                    }
                }
            }
        }
        //add'v'to the current_Bin here
        for (int i = 0; i <= numVar; i++) {
            for (size_t j = 0; j < current_Bin[i].size(); j++) {
                if (current_used[i][j]) {
                    current_Bin[i][j][0] = 'v';  // Mark as checked
                }
            }
        }


        
                // Collect prime implicants
        for (int i = 0; i <= numVar; i++) {
            for (size_t j = 0; j < current_Bin[i].size(); j++) {
                if (current_Bin[i][j][0] == ' ') {
                    // Store the decimal representation directly
                    string decimal_prime = current_Dec[i][j];
                    
                    // Add to primes if not already there
                    if (find(primes.begin(), primes.end(), decimal_prime) == primes.end()) {
                        primes.push_back(decimal_prime);
                    }
                }
            }
        }
        
        // If no new matches were found, exit the loop
        if (!found_match) break;
        
       
        // Update for next iteration
        current_Bin = next_Bin;
        current_Dec = next_Dec;
        column_num++;
        
        // Check for duplicates in the updated current_Bin
        for (int i = 0; i <= numVar; i++) {
            for (size_t j = 0; j < current_Bin[i].size(); j++) {
                // Skip already marked terms
                if (current_Bin[i][j][0] == 'x' || current_Bin[i][j][0] == 'v') continue;
                
                // Check for duplicates
                for (int i2 = i; i2 <= numVar; i2++) {
                    size_t start = (i2 == i) ? j + 1 : 0;
                    for (size_t j2 = start; j2 < current_Bin[i2].size(); j2++) {
                        if (current_Bin[i2][j2][0] != 'x' && current_Bin[i2][j2][0] != 'v' && 
                            current_Bin[i][j].substr(1) == current_Bin[i2][j2].substr(1)) {
                            current_Bin[i2][j2][0] = 'x';  // Mark as duplicate
                        }
                    }
                }
            }
        }
    }


	return 0;

}
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <map>
#include <unordered_set>

using namespace std;

struct QmSolution {
    int numVar;
    vector<string> primes;
    vector<vector<string> > minimumSops;
};


vector<string> find_primes(int numVar, vector<int> minterms,vector<int> dontcares){
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

        
        // Print current column after marking
        cout <<"========================="<< endl;
        cout << "Column " << column_num  << endl;
        cout <<"========================="<< endl;
        
        for (int i = 0; i <= numVar; i++) {
            if (!current_Bin[i].empty()) {
                for (size_t j = 0; j < current_Bin[i].size(); j++) {
                    cout << "  " << current_Bin[i][j] << " (" << current_Dec[i][j] << ")" << endl;
                }
                cout <<"------------------"<< endl;
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
    
    cout << "Final prime implicants:" << endl;
    for (const auto& prime : primes) {
        cout << "  " << prime << endl;
    }
    return primes;

}

vector<string> find_primes_noprint(int numVar, vector<int> minterms,vector<int> dontcares){
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
    return primes;

}


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

// Helper function to get minterms from a prime's decimal representation
vector<int> get_minterms_from_prime(const string& prime, const vector<int>& valid_minterms) {
    vector<int> result;
    size_t start = 0, end = 0;
    
    while ((end = prime.find(',', start)) != string::npos) {
        int minterm = stoi(prime.substr(start, end - start));
        if (find(valid_minterms.begin(), valid_minterms.end(), minterm) != valid_minterms.end()) {
            result.push_back(minterm);
        }
        start = end + 1;
    }
    
    // Add the last minterm
    int last_minterm = stoi(prime.substr(start));
    if (find(valid_minterms.begin(), valid_minterms.end(), last_minterm) != valid_minterms.end()) {
        result.push_back(last_minterm);
    }
    
    return result;
}

// Petrick's method implementation
vector<vector<int>> petrick_solve(const vector<vector<int>>& P) {
    // If no clauses, return empty result
    if (P.empty()) {
        return {{}};
    }
    
    // Start with the first clause
    vector<vector<int>> result = {};
    for (int prime_idx : P[0]) {
        result.push_back({prime_idx});
    }
    
    // Multiply all other clauses
    for (size_t i = 1; i < P.size(); i++) {
        vector<vector<int>> new_result;
        for (const auto& term : result) {
            for (int prime_idx : P[i]) {
                vector<int> new_term = term;
                // Only add the prime if it's not already in the term
                if (find(new_term.begin(), new_term.end(), prime_idx) == new_term.end()) {
                    new_term.push_back(prime_idx);
                }
                new_result.push_back(new_term);
            }
        }
        
        // Remove duplicates
        for (size_t j = 0; j < new_result.size(); j++) {
            sort(new_result[j].begin(), new_result[j].end());
        }
        sort(new_result.begin(), new_result.end());
        new_result.erase(unique(new_result.begin(), new_result.end()), new_result.end());
        
        result = new_result;
    }
    
    // Find solutions with minimal number of terms
    vector<vector<int>> minimal_solutions;
    if (!result.empty()) {
        size_t min_terms = result[0].size();
        for (const auto& solution : result) {
            min_terms = min(min_terms, solution.size());
        }
        
        // Keep only minimal solutions
        for (const auto& solution : result) {
            if (solution.size() == min_terms) 
                minimal_solutions.push_back(solution);
        }
    }
return minimal_solutions;

}

vector<vector<string>> find_minimumSops(int numVar, vector<int> minterms, vector<string> primes) {
    // Step 1: Create the prime implicant chart
    map<int, vector<int>> minterm_to_primes; // Maps minterms to the indices of primes that cover them
    
    // For each prime, determine which minterms it covers
    for (size_t i = 0; i < primes.size(); i++) {
        const string& prime = primes[i];
        vector<int> covered_minterms;
        
        // Parse the comma-separated decimal values
        size_t start = 0, end = 0;
        while ((end = prime.find(',', start)) != string::npos) {
            int minterm = stoi(prime.substr(start, end - start));
            if (find(minterms.begin(), minterms.end(), minterm) != minterms.end()) {
                covered_minterms.push_back(minterm);
            }
            start = end + 1;
        }
        // Handle the last minterm
        int last_minterm = stoi(prime.substr(start));
        if (find(minterms.begin(), minterms.end(), last_minterm) != minterms.end()) {
            covered_minterms.push_back(last_minterm);
        }
        
        // Add this prime to the list for each minterm it covers
        for (int minterm : covered_minterms) {
            minterm_to_primes[minterm].push_back(i);
        }
    }
    
    // Step 2: Identify essential prime implicants
    vector<bool> is_essential(primes.size(), false);
    unordered_set<int> covered_minterms;
    
    for (const auto& entry : minterm_to_primes) {
        int minterm = entry.first;
        const vector<int>& covering_primes = entry.second;
        
        // If a minterm is covered by only one prime, that prime is essential
        if (covering_primes.size() == 1) {
            int prime_idx = covering_primes[0];
            is_essential[prime_idx] = true;
            
            // Mark minterms covered by this essential prime
            for (int m : get_minterms_from_prime(primes[prime_idx], minterms)) {
                covered_minterms.insert(m);
            }
        }
    }
    
    // Step 3: Set up Petrick's method for remaining minterms
    vector<vector<int>> P; // Product of Sums form
    
    for (int minterm : minterms) {
        // Skip if already covered by essential primes
        if (covered_minterms.find(minterm) != covered_minterms.end()) {
            continue;
        }
        
        // Find all primes that cover this minterm
        vector<int> covering_primes = minterm_to_primes[minterm];
        if (!covering_primes.empty()) {
            P.push_back(covering_primes);
        }
    }
    
    // Step 4: Apply Petrick's method to find all solutions
    vector<vector<int>> solutions = petrick_solve(P);
    
    // Step 5: Add essential primes to each solution
    for (size_t i = 0; i < is_essential.size(); i++) {
        if (is_essential[i]) {
            for (auto& solution : solutions) {
                if (find(solution.begin(), solution.end(), i) == solution.end()) {
                    solution.push_back(i);
                }
            }
        }
    }
    
    // Step 6: Convert solutions to minimal SOP expressions in decimal form
    vector<vector<string>> minimumSops;
    for (const auto& solution : solutions) {
        vector<string> sop;
        for (int prime_idx : solution) {
            sop.push_back(primes[prime_idx]);
        }
        minimumSops.push_back(sop);
    }
    
    return minimumSops;
}

//return the answer of the type of QmSolution
QmSolution *solveQm(int numVar, vector<int> minterms,
                    vector<int> dontcares, bool verbose)
{
    QmSolution *sol = new QmSolution;
    sol->numVar = numVar;
    vector<string> primes_Dec = find_primes_noprint(numVar, minterms, dontcares);
    vector<vector<string>> find_minimumSops_Dec = find_minimumSops(numVar, minterms, primes_Dec);
    //trun the decimal to binary
    vector<string> primes_Bool = boolexpr(numVar, primes_Dec);
    vector<vector<string>> find_minimumSops_Bool;

    for (const auto& sop : find_minimumSops_Dec) {
        vector<string> bool_expr = boolexpr(numVar, sop);
        find_minimumSops_Bool.push_back(bool_expr);
    }
    sol->primes = primes_Bool;
    sol->minimumSops = find_minimumSops_Bool;
    if(verbose){
        //just for print
        vector<string> primes_forprint= find_primes(numVar, minterms, dontcares);
    }

    if (verbose) {
         printImplicantChart(numVar, minterms, primes_Dec);
    }

    return sol;


}
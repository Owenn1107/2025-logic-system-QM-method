
#include <vector>
#include <string>
#include <unordered_set>
#include <iostream>
#include <algorithm>
#include <map>
#include <iomanip>
using namespace std;


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

int main() {
    // Example 1: Simple case
    cout << "=== Example 1 ===" << endl;
    int numVar1 = 8;
    vector<int> minterms1 = {0, 2, 5, 6, 7, 8, 10, 12, 13, 14, 15};
    vector<string> primes1 = { "0,2,8,10",
                                "2,6,10,14",
                                "8,10,12,14",
                                "5,7,13,15",
                                "6,7,14,15",
                                "12,13,14,15"

};
    vector<vector<string>> sops1 = find_minimumSops(numVar1, minterms1, primes1);

    cout << "Minimal SOPs:" << endl;
    for (size_t i = 0; i < sops1.size(); i++) {
        cout << "Solution " << (i + 1) << ": ";
        for (size_t j = 0; j < sops1[i].size(); j++) {
            cout << sops1[i][j];
            if (j < sops1[i].size() - 1) cout << " + ";
        }
        cout << endl;
    }
    
  
    return 0;
}
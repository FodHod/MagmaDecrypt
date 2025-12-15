#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <bitset>
using namespace std;

// Error messages
const string BAD_SYMBOL_NB("Error: the symbol set size must be in the range ]2, 64]");
const string WRONG_SYMBOL_NB("Error: the number of symbols must match the symbol set size");
                             
const string WRONG_SYMBOL_VALUE("Error: at least one symbol value is not allowed");
const string DUPLICATED_SYMBOL_VALUE("Error: all symbols must be different");

const string WRONG_COMPL_SYMBOL_NB("Error: the number of complementary symbols is incorrect");
                                   
const string WRONG_COMPL_SYMBOL_VALUE("Error: complementary symbols must be part of the symbol set");

const string BAD_GRID_SIZE("Error: the grid size must be in the range [2, 64]");
const string WRONG_GRID_LINE_SIZE("Error: the number of symbols in each line must match the number of columns");
                                  
const string WRONG_GRID_SYMBOL_VALUE("Error: all grid symbols must be part of the symbol set");

const string BAD_KEY_VALUE("Error: the key value is invalid");

// Final message
const string THE_END("The end");


// prototype de la fonction d'affichage de message d'error
void print_error(string message);

void print_error(string message)
{
    cout << message;
    cout << endl;
    exit(0);
}

//
// Début de mon code (je vais écrire mes commentaires en anglais)
//

// For simplicity I created a struct to hold all data - on of the advanteges of c++
struct InputData {
    int nbS;
    vector<char> symboles;
    int nbTSymboles;
    vector<char> tsd;
    int nbL, nbC;
    vector<vector<char>> grid;
    int keyValue;
};

// Get Input Data according to the given format
InputData read_input() {
    InputData data;
    
    // Get nbS and symbols
    cin >> data.nbS;

    data.symboles.resize(data.nbS);
    for (int i = 0; i < data.nbS; i++) {
        cin >> data.symboles[i];
    }

    // Calculate complementary symbols needed
    data.nbTSymboles = (data.nbS * data.nbS - (data.nbS + 2 * (data.nbS - 1))) / 2;

    // Read symbols to complete solution table
    data.tsd.resize(data.nbTSymboles);
    for (int i = 0; i < data.nbTSymboles; i++) {
        cin >> data.tsd[i];
    }
    
    // Get size of the grid and read it
    cin >> data.nbL >> data.nbC;

    data.grid.assign(data.nbL, vector<char>(data.nbC));
    for (int r = 0; r < data.nbL; ++r) {
        for (int c = 0; c < data.nbC; ++c) {
            cin >> data.grid[r][c];
        }
    }

    // Get Key Value
    cin >> data.keyValue;
    return data;
}

// Validation of parameters according to the error messages
int parameters_validation(const InputData &data) {
    // Validate nbS and symboles
    if (data.nbS <= 2 || data.nbS > 64) {
        print_error(BAD_SYMBOL_NB);
    }

    if (data.symboles.size() != static_cast<size_t>(data.nbS)) {
        print_error(WRONG_SYMBOL_NB);
    }

    // Check symbol values (must be in [33, 125])
    for (char symbole: data.symboles) {
        if (symbole < 33 || symbole > 125) {
            print_error(WRONG_SYMBOL_VALUE);
        }
    }

    // Check for duplicates
    for (size_t i = 0; i < data.symboles.size(); ++i) {
        for (size_t j = i + 1; j < data.symboles.size(); ++j) {
            if (data.symboles[i] == data.symboles[j]) {
                print_error(DUPLICATED_SYMBOL_VALUE);
            }
        }
    }

    // Check complementary symbols
    if (data.tsd.size() != static_cast<size_t>(data.nbTSymboles)) {
        print_error(WRONG_COMPL_SYMBOL_NB);
    }

    for (char symbole: data.tsd) {
        if (find(data.symboles.begin(), data.symboles.end(), symbole) == data.symboles.end()) {
            print_error(WRONG_COMPL_SYMBOL_VALUE);
        }
    }

    // Validate grid dimensions
    if (data.nbL < 2 || data.nbL > 64 || data.nbC < 2 || data.nbC > 64) {
        print_error(BAD_GRID_SIZE);
    }

    // Check grid lines and symbols
    for (const auto &line : data.grid) {
        if (line.size() != static_cast<size_t>(data.nbC)) {
            print_error(WRONG_GRID_LINE_SIZE);
        }
        for (char symbole : line) {
            if (find(data.symboles.begin(), data.symboles.end(), symbole) == data.symboles.end()) {
                print_error(WRONG_GRID_SYMBOL_VALUE);
            }
        }
    }

    // Validate key value
    if (data.keyValue < 0 || data.keyValue > 255) {
        print_error(BAD_KEY_VALUE);
    }
    
    bitset<8> binary(data.keyValue);
    
    // Check most significant bit is 0
    if (binary[7] != 0) {
        print_error(BAD_KEY_VALUE);
    }
    
    // Check bit 6 for sequence/hierarchical
    bool isSequence = binary[6] == 1;
    
    if (isSequence) {
        // For sequence: check the 3 pairs are different (00, 01, 10, 11)
        vector<int> codes;
        for (int i = 0; i < 6; i += 2) {
            int code = (binary[i+1] << 1) | binary[i];
            codes.push_back(code);
        }
        
        // Check all codes are different
        for (size_t i = 0; i < codes.size(); ++i) {
            for (size_t j = i + 1; j < codes.size(); ++j) {
                if (codes[i] == codes[j]) {
                    print_error(BAD_KEY_VALUE);
                }
            }
        }
    } else {
        // For hierarchical: check value is 0, 1, or 2
        int value = data.keyValue & 0x7F; // Clear bit 7
        if (value != '0' && value != '1' && value != '2') {
            print_error(BAD_KEY_VALUE);
        }
    }

    return 0;
}

// Print the established solution table with given symboles
int print_solution_table(const InputData &data) {
    // Print header
    cout << "  ";
    for (char symbole : data.symboles) {
        cout << symbole << " ";
    }
    cout << endl;

    // Print first line (neutral element row)
    cout << data.symboles[0] << " ";
    for (size_t i = 0; i < data.symboles.size(); ++i) {
        cout << data.symboles[i] << " ";
    }
    cout << endl;

    // Print other lines
    int tsd_index = 0;
    for (size_t i = 1; i < data.symboles.size(); ++i) {
        cout << data.symboles[i] << " " << data.symboles[i] << " ";
        
        for (size_t j = 1; j < data.symboles.size(); ++j) {
            if (i == j) {
                // Diagonal: idempotence
                cout << data.symboles[i] << " ";
            } else if (j < i) {
                // Lower triangle: use commutative property
                // Find index for (j, i) in tsd
                size_t n = data.symboles.size();
                size_t idx = (j - 1) * (n - 1) - ((j - 1) * j) / 2 + (i - j - 1);
                cout << data.tsd[idx] << " ";
            } else {
                // Upper triangle: use tsd in order
                cout << data.tsd[tsd_index++] << " ";
            }
        }
        cout << endl;
    }

    cout << endl; // Empty line after table
    return 0;
}

// Print current grid
int print_grid(const InputData &data) {
    for (const auto &line : data.grid) {
        for (char symbole : line ) {
            cout << symbole << " ";
        }
        cout << endl;
    }
    cout << endl; // Empty line after grid
    return 0;
}

// Simple Function to get the index of a symbol in the solution table
static int find_symbol_index(const vector<char> &symbols, char s) {
    auto it = find(symbols.begin(), symbols.end(), s);
    if (it == symbols.end()) return -1;
    return static_cast<int>(it - symbols.begin());
}

// Get the "sum" of two symboles according to the solution table using find_symbol_index()
char get_symbol(const InputData &data, char sym1, char sym2) {
    int i = find_symbol_index(data.symboles, sym1);
    int j = find_symbol_index(data.symboles, sym2);
    if (i < 0 || j < 0) print_error(WRONG_GRID_SYMBOL_VALUE);

    // same symbol -> diagonal
    if (i == j) return data.symboles[i];

    // first row/column behaviour (neutral element)
    if (i == 0) return data.symboles[j];
    if (j == 0) return data.symboles[i];

    // ensure i < j for indexing tsd
    if (i > j) swap(i, j);

    size_t n = data.symboles.size();
    // Calculate index in tsd for pair (i, j)
    // Formula: (i-1)*(n-1) - ((i-1)*i)/2 + (j-i-1)
    size_t idx = (i - 1) * (n - 1) - ((i - 1) * i) / 2 + (j - i - 1);
    
    if (idx >= data.tsd.size()) {
        print_error(WRONG_COMPL_SYMBOL_NB);
    }

    return data.tsd[idx];
}

// Function to process a 2x2 block according to the order
char process_block(const InputData &data, char a, char b, char c, char d, const vector<int>& order, bool isSequence) {
    if (isSequence) {
        // Sequential combination: ((first second) third) fourth
        char temp = get_symbol(data, a, b); // First combination
        temp = get_symbol(data, temp, c);   // Second combination
        return get_symbol(data, temp, d);   // Final combination
    } else {
        // Hierarchical combination: (pair1)(pair2) then combine results
        char pair1 = get_symbol(data, a, b);
        char pair2 = get_symbol(data, c, d);
        return get_symbol(data, pair1, pair2);
    }
}

// Decryption algorithm
int decrypt(InputData data) {
    // Print solution table
    print_solution_table(data);

    // Print initial grid
    print_grid(data);

    // Analyze key to determine order
    bitset<8> binary(data.keyValue);
    bool isSequence = binary[6] == 1;
    
    vector<int> order(4);
    
    if (isSequence) {
        // Extract order from bits 0-5 (3 pairs of 2 bits)
        for (int i = 0; i < 6; i += 2) {
            int code = (binary[i+1] << 1) | binary[i];
            order[i/2] = code;
        }
        
        // Find the missing code (0, 1, 2, or 3)
        vector<bool> used(4, false);
        for (int i = 0; i < 3; ++i) {
            used[order[i]] = true;
        }
        for (int i = 0; i < 4; ++i) {
            if (!used[i]) {
                order[3] = i;
                break;
            }
        }
    } else {
        // Hierarchical: order depends on the ASCII value
        int value = data.keyValue & 0x7F;
        if (value == '0') {
            // (ab)(cd)
            order = {0, 1, 2, 3}; // a,b then c,d
        } else if (value == '1') {
            // (ac)(bd)
            order = {0, 2, 1, 3}; // a,c then b,d
        } else { // value == '2'
            // (ad)(bc)
            order = {0, 3, 1, 2}; // a,d then b,c
        }
    }

    // Continue decryption while both dimensions are even
    while (data.nbL % 2 == 0 && data.nbC % 2 == 0 && data.nbL > 1 && data.nbC > 1) {
        // Create new grid
        int newL = data.nbL / 2;
        int newC = data.nbC / 2;
        vector<vector<char>> newGrid(newL, vector<char>(newC));
        
        // Map order indices to block positions
        vector<vector<char>> positions = {
            {data.grid[0][0], data.grid[0][1], data.grid[1][0], data.grid[1][1]}  // a,b,c,d
        };
        
        // Process each 2x2 block
        for (int y = 0; y < data.nbL; y += 2) {
            for (int x = 0; x < data.nbC; x += 2) {
                // Get block elements
                char block[4] = {
                    data.grid[y][x],       // a: top-left
                    data.grid[y][x + 1],   // b: top-right
                    data.grid[y + 1][x],   // c: bottom-left
                    data.grid[y + 1][x + 1] // d: bottom-right
                };
                
                // Reorder according to sequence or use hierarchical order
                char result;
                if (isSequence) {
                    // For sequential: apply in specified order
                    char temp = get_symbol(data, block[order[0]], block[order[1]]);
                    temp = get_symbol(data, temp, block[order[2]]);
                    result = get_symbol(data, temp, block[order[3]]);
                } else {
                    // For hierarchical: use pairs
                    char pair1 = get_symbol(data, block[order[0]], block[order[1]]);
                    char pair2 = get_symbol(data, block[order[2]], block[order[3]]);
                    result = get_symbol(data, pair1, pair2);
                }
                
                newGrid[y / 2][x / 2] = result;
            }
        }
        
        // Update grid
        data.grid = move(newGrid);
        data.nbL = newL;
        data.nbC = newC;
        
        // Print decrypted grid
        print_grid(data);
    }

    return 0;
}

int main() {
    // Get Values - Tache 1 & 2
    InputData data = read_input();
    parameters_validation(data);

    // Decryption - tache 3
    decrypt(data);

    print_error(THE_END);
    return 0;
}
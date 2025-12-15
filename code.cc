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
    data.nbTSymboles = (data.nbS - 2) * (data.nbS - 1) / 2;

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

    for (char symbole: data.symboles) {
        if (symbole > 32 || symbole < 126) {
            print_error(WRONG_SYMBOL_VALUE);
        }
    }

    if (count(data.symboles.begin(), data.symboles.end(), data.symboles[0]) > 1) {
        print_error(DUPLICATED_SYMBOL_VALUE);
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

    // Last but not least, validate grid
    if (data.nbL < 2 || data.nbL > 64 || data.nbC < 2 || data.nbC > 64) {
        print_error(BAD_GRID_SIZE);
    }

    else if (data.nbL % 2 == 0 || data.nbC % 2 == 0){
        print_error(BAD_GRID_SIZE);
    }
    
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
    if (data.keyValue != 0) {
    bitset<8> binary(data.keyValue);
    string binaryStr = binary.to_string();
    vector<string> binary2Bit;
    
    for (int i = 0; i < 8; i += 2) {
        binary2Bit.push_back(binaryStr.substr(i, 2));
    }

    // Check first 2 bit
    if (binary2Bit[0] != "01") {
        print_error(BAD_KEY_VALUE);
    }
    binary2Bit.erase(binary2Bit.begin());

    // Check for duplicates in resting 3 bits
    // as the combinations of a, b, c and d are 00, 01, 10, 11 we just need to check for duplicates
    for (size_t i = 0; i < binary2Bit.size(); ++i) {
            for (size_t j = i + 1; j < binary2Bit.size(); ++j) {
                if (binary2Bit[i] == binary2Bit[j]) {
                    print_error(BAD_KEY_VALUE);
                }
            }
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

    // Print first ligne
    cout << data.symboles[0] << " ";
    for (size_t i = 0; i < data.symboles.size(); ++i) {
        cout << data.symboles[i] << " ";
    }
    cout << endl;

    // Print each ligne
    int i1 = 0;
    int i2 = 0;
    for (size_t i = 1; i < data.symboles.size(); ++i) {
        cout << data.symboles[i] << " " << data.symboles[i] << " ";
        for (size_t j = 0; j < data.symboles.size()-1; ++j) {
            if (i-1 == j) {
                cout << data.symboles[i] << " ";
            } else if (i > j) {
                cout << data.tsd[i1++] << " ";
            } else {
                cout << data.tsd[i2++] << " ";
            }
        }
        cout << endl;
    }

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
    return 0;
}

// Simple Function to get the index of a symbol in the solution table
static int find_symbol_index(const vector<char> &symbols, char s) {
    auto it = find(symbols.begin(), symbols.end(), s);
    if (it == symbols.end()) return -1;
    return static_cast<int>(it - symbols.begin());
}

// Get the "sum" of two symboles according to the solution table using find_symbol_index()
int get_symbol(const InputData &data, char sym1, char sym2) {
    int i = find_symbol_index(data.symboles, sym1);
    int j = find_symbol_index(data.symboles, sym2);
    if (i < 0 || j < 0) print_error(WRONG_GRID_SYMBOL_VALUE);

    // same symbol -> diagonal
    if (i == j) return data.symboles[i];

    // first row/column behaviour
    if (i == 0) return data.symboles[j];
    if (j == 0) return data.symboles[i];

    // ensure i < j so we can index tsd without mirrioring it cuz of symmetry
    if (i > j) swap(i, j);

    size_t n = data.symboles.size();
    // get index from tsd for (i, j)
    size_t idx = static_cast<size_t>((i - 1) * (n - 1) - ((i - 1) * i) / 2 + (j - i - 1));

    return data.tsd[idx];
}

// Decryption algorithm using get_symbol() - (using find_symbol_index()), print_grid() and print_solution_table()
int decrypt(InputData data) {
    // Print solution table
    print_solution_table(data);

    // Defining relevant variables for decryption - Notable: Key Value and Order
    int order[] = {0, 1, 2, 3};
    if (data.keyValue != 0) {
        // Reorder according to key value
        bitset<8> binary(data.keyValue);
        string binaryStr = binary.to_string();
        vector<string> binary2Bit;
        for (int i = 0; i < 8; i += 2) {
            binary2Bit.push_back(binaryStr.substr(i, 2));
        }
        binary2Bit.erase(binary2Bit.begin());

        int inputOrder[3];
        int sum = 0;
        for (size_t i = 0; i < binary2Bit.size(); ++i) {
            inputOrder[i] = stoi(binary2Bit[i], nullptr, 2);
            sum += inputOrder[i];
        }

        int missing = 6 - sum; // as 0+1+2+3 = 6 -> 6 - sum of the others = missing one
        order[0] = inputOrder[0];
        order[1] = inputOrder[1];
        order[2] = inputOrder[2];
        order[3] = missing;
    }

    // for the decryption I just repeat the process until nbL = 1
    while (data.nbL > 1) {
        // Print current grid
        print_grid(data);

        // Reset new grid
        vector<vector<char>> newGrid(data.nbL / 2, vector<char>(data.nbC / 2));

        for (size_t y = 0; y < data.nbL; y += 2) {
            for (size_t x = 0; x < data.nbC; x += 2) {
                // Get current 2x2 block
                char a = data.grid[y][x];
                char b = data.grid[y][x + 1];
                char c = data.grid[y + 1][x];
                char d = data.grid[y + 1][x + 1];
                char block[4] = {a, b, c, d};

                char sum;

                // Apply compositions according to order
                sum = get_symbol(data, block[order[0]], block[order[1]]);
                sum = get_symbol(data, sum, block[order[2]]);
                sum = get_symbol(data, sum, block[order[3]]);

                // Place result in new grid
                newGrid[y / 2][x / 2] = sum;
            }
        }

        data.grid = move(newGrid);
        data.nbL /= 2;
        data.nbC /= 2;
    }
    return 0;
}

int main() {
    // Get Values - Tache 1 & 2 ig
    InputData data = read_input();
    parameters_validation(data);

    // Decryption - tache 3
    decrypt(data);

    print_error(THE_END);
    return 0;
}
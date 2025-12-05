#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
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

struct InputData {
    int nbS;
    vector<char> symboles;
    int nbTSymboles;
    vector<char> tsd;
    int nbL, nbC;
    vector<vector<char>> grid;
    int keyValue;
};

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

    // Validate key value - idk how to do that tho :(
    
    return 0;
}

int main() {
    // Get Values - Tache 1 & 2 ig
    InputData data = read_input();
    parameters_validation(data);

    // Decryption

    print_error(THE_END);
    return 0;
}
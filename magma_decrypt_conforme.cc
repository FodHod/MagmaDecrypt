/*
 File: magma_decrypt.cc
 Author: Étudiant (votre nom)
 Version: 1.1
 Description:
   Programme de résolution pour le projet MagmaDecrypt.
   Le code est structuré pour respecter les "Conventions de programmation en C++"
   (noms, constexpr, pas de variables globales, fonctions courtes, documentation D1).
*/

#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::invalid_argument;
using std::istringstream;
using std::string;
using std::vector;

constexpr int MAX_ORDERS = 15;

static vector<string> split_line(const string &line) {
    istringstream iss(line);
    vector<string> tokens;
    string tok;
    while (iss >> tok) tokens.push_back(tok);
    return tokens;
}

static vector<string> read_all_tokens() {
    vector<string> tokens;
    string line;
    while (getline(cin, line)) {
        // remove possible trailing carriage return
        if (!line.empty() && line.back() == '\r') line.pop_back();
        auto parts = split_line(line);
        for (const auto &p : parts) tokens.push_back(p);
    }
    return tokens;
}

static int find_symbol_index(const vector<string> &symbols, const string &s) {
    for (size_t i = 0; i < symbols.size(); ++i) {
        if (symbols[i] == s) return static_cast<int>(i);
    }
    return -1;
}

static void print_composition_table(const vector<string> &symbols,
                                    const vector<vector<int>> &table) {
    cout << "Symbols (first is neutral):";
    for (const auto &s : symbols) cout << ' ' << s;
    cout << "\nnbS = " << symbols.size() << "\n\nComposition table:\n";
    for (size_t i = 0; i < table.size(); ++i) {
        for (size_t j = 0; j < table[i].size(); ++j) {
            cout << symbols[table[i][j]];
            if (j + 1 != table[i].size()) cout << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

static void print_grid(const vector<vector<int>> &grid,
                       const vector<string> &symbols) {
    for (const auto &row : grid) {
        for (size_t j = 0; j < row.size(); ++j) {
            cout << symbols[row[j]];
            if (j + 1 != row.size()) cout << " ";
        }
        cout << "\n";
    }
}

static int compose(const vector<vector<int>> &table, int x, int y) {
    return table[x][y];
}

// Define the 15 orders as functions that take a,b,c,d and return composed index
// The orders vector will contain lambdas capturing the composition table.
static vector<std::function<int(int,int,int,int)>> make_orders(
        const vector<vector<int>> &table) {
    vector<std::function<int(int,int,int,int)>> orders;
    orders.reserve(MAX_ORDERS);

    // Helper to compose two
    auto comp = [&](int x, int y) { return compose(table, x, y); };

    // 12 left-associated variants and 3 pairings
    orders.push_back([&](int a,int b,int c,int d){ return comp(comp(comp(a,b),c),d); }); // ((ab)c)d
    orders.push_back([&](int a,int b,int c,int d){ return comp(comp(comp(a,b),d),c); }); // ((ab)d)c
    orders.push_back([&](int a,int b,int c,int d){ return comp(comp(comp(a,c),b),d); }); // ((ac)b)d
    orders.push_back([&](int a,int b,int c,int d){ return comp(comp(comp(a,c),d),b); }); // ((ac)d)b
    orders.push_back([&](int a,int b,int c,int d){ return comp(comp(comp(a,d),b),c); }); // ((ad)b)c
    orders.push_back([&](int a,int b,int c,int d){ return comp(comp(comp(a,d),c),b); }); // ((ad)c)b
    orders.push_back([&](int a,int b,int c,int d){ return comp(comp(comp(b,c),a),d); }); // ((bc)a)d
    orders.push_back([&](int a,int b,int c,int d){ return comp(comp(comp(b,c),d),a); }); // ((bc)d)a
    orders.push_back([&](int a,int b,int c,int d){ return comp(comp(comp(b,d),a),c); }); // ((bd)a)c
    orders.push_back([&](int a,int b,int c,int d){ return comp(comp(comp(b,d),c),a); }); // ((bd)c)a
    orders.push_back([&](int a,int b,int c,int d){ return comp(comp(comp(c,d),a),b); }); // ((cd)a)b
    orders.push_back([&](int a,int b,int c,int d){ return comp(comp(comp(c,d),b),a); }); // ((cd)b)a

    // 3 hierarchical pairings
    orders.push_back([&](int a,int b,int c,int d){ return comp(comp(a,b), comp(c,d)); }); // (ab)(cd)
    orders.push_back([&](int a,int b,int c,int d){ return comp(comp(a,c), comp(b,d)); }); // (ac)(bd)
    orders.push_back([&](int a,int b,int c,int d){ return comp(comp(a,d), comp(b,c)); }); // (ad)(bc)

    return orders;
}

int main() {
    // Read tokens from stdin
    const auto tokens = read_all_tokens();
    if (tokens.empty()) {
        cout << "No input provided.\n";
        return 1;
    }

    size_t pos = 0;
    int nbS = 0;
    vector<string> symbols;

    // Try to parse nbS as integer
    bool has_nbS = false;
    try {
        nbS = std::stoi(tokens[pos]);
        has_nbS = true;
        ++pos;
    } catch (const std::exception &) {
        has_nbS = false;
    }

    if (has_nbS) {
        if (pos + static_cast<size_t>(nbS) > tokens.size()) {
            cout << "Not enough tokens for symbol list.\n";
            return 1;
        }
        for (int i = 0; i < nbS; ++i) symbols.push_back(tokens[pos++]);
    } else {
        // fallback: use first line's tokens as symbol list
        // tokens[0] was not an integer; take leading tokens until we've consumed a reasonable set.
        // For simplicity, treat the first token as a single symbol set entry and infer nbS from table later.
        symbols.push_back(tokens[pos++]);
        // We'll infer nbS from remaining tokens when reading table.
    }

    // At this point we expect nbS to be known; if not, we guess using remaining tokens.
    if (symbols.size() == 1 && !has_nbS) {
        // We need to deduce nbS: the table requires nbS*nbS tokens ahead.
        // Heuristic: try values from 2..16 and find one where enough tokens remain to parse table.
        bool found = false;
        for (int cand = 2; cand <= 20 && !found; ++cand) {
            if (pos + cand * cand <= tokens.size()) {
                nbS = cand;
                // Fill symbols by taking next cand-1 tokens (we already have one)
                for (int k = 1; k < nbS; ++k) {
                    symbols.push_back(tokens[pos++]);
                }
                found = true;
            }
        }
        if (!found) {
            cout << "Unable to infer number of symbols (nbS).\n";
            return 1;
        }
    }

    // Read composition table
    if (pos + static_cast<size_t>(nbS) * static_cast<size_t>(nbS) > tokens.size()) {
        cout << "Not enough tokens for composition table.\n";
        return 1;
    }
    vector<vector<int>> table(static_cast<size_t>(nbS),
                              vector<int>(static_cast<size_t>(nbS), -1));
    for (int i = 0; i < nbS; ++i) {
        for (int j = 0; j < nbS; ++j) {
            const string &tok = tokens[pos++];
            const int idx = find_symbol_index(symbols, tok);
            if (idx < 0) {
                cout << "Unknown symbol in composition table: " << tok << "\n";
                return 1;
            }
            table[i][j] = idx;
        }
    }

    // Try to read grid dimensions R C
    if (pos >= tokens.size()) {
        cout << "Missing grid and code tokens.\n";
        return 1;
    }
    int R = -1;
    int C = -1;
    bool have_rc = false;
    if (pos + 1 < tokens.size()) {
        try {
            R = std::stoi(tokens[pos]);
            C = std::stoi(tokens[pos + 1]);
            have_rc = true;
            pos += 2;
        } catch (const std::exception &) {
            have_rc = false;
        }
    }

    vector<vector<int>> grid;
    if (have_rc) {
        if (pos + static_cast<size_t>(R) * static_cast<size_t>(C) > tokens.size()) {
            cout << "Not enough tokens for grid of size " << R << "x" << C << "\n";
            return 1;
        }
        grid.assign(static_cast<size_t>(R), vector<int>(static_cast<size_t>(C), -1));
        for (int i = 0; i < R; ++i) {
            for (int j = 0; j < C; ++j) {
                const string &tok = tokens[pos++];
                const int idx = find_symbol_index(symbols, tok);
                if (idx < 0) {
                    cout << "Unknown symbol in grid: " << tok << "\n";
                    return 1;
                }
                grid[i][j] = idx;
            }
        }
    } else {
        // Fallback: assume last token is code, use remaining-1 tokens for grid.
        if (tokens.size() - pos < 2) {
            cout << "Not enough tokens to deduce grid and code.\n";
            return 1;
        }
        size_t remaining = tokens.size() - pos;
        size_t grid_tokens = remaining - 1;
        // Heuristic: set columns to first original row length if possible, else  make single row
        // Here, choose columns as divisor of grid_tokens close to sqrt
        size_t best_c = 1;
        for (size_t c = 1; c * c <= grid_tokens; ++c) {
            if (grid_tokens % c == 0) best_c = c;
        }
        C = static_cast<int>(best_c);
        R = static_cast<int>(grid_tokens / best_c);
        grid.assign(static_cast<size_t>(R), vector<int>(static_cast<size_t>(C), -1));
        for (int i = 0; i < R; ++i) {
            for (int j = 0; j < C; ++j) {
                const string &tok = tokens[pos++];
                const int idx = find_symbol_index(symbols, tok);
                if (idx < 0) {
                    cout << "Unknown symbol in grid: " << tok << "\n";
                    return 1;
                }
                grid[i][j] = idx;
            }
        }
    }

    // Next token is code
    if (pos >= tokens.size()) {
        cout << "Missing code token.\n";
        return 1;
    }
    const string code_token = tokens[pos++];
    const unsigned char code_char = static_cast<unsigned char>(code_token[0]);
    const int code_val = static_cast<int>(code_char);

    // Print composition table and initial grid
    print_composition_table(symbols, table);
    cout << "Initial grid (" << grid.size() << "x" << (grid.empty() ? 0 : grid[0].size())
         << "):\n";
    print_grid(grid, symbols);

    // Build orders and pick one deterministically
    const auto orders = make_orders(table);
    const int order_index = code_val % static_cast<int>(orders.size());
    cout << "\nUsing code '" << code_token << "' (ASCII " << code_val << "), selected order index = "
         << order_index << ".\n\n";

    int step = 0;
    auto current_grid = grid;
    while (!current_grid.empty() && !current_grid[0].empty() &&
           (current_grid.size() % 2 == 0) && (current_grid[0].size() % 2 == 0)) {
        const int rows = static_cast<int>(current_grid.size());
        const int cols = static_cast<int>(current_grid[0].size());
        const int new_r = rows / 2;
        const int new_c = cols / 2;
        vector<vector<int>> new_grid(static_cast<size_t>(new_r),
                                     vector<int>(static_cast<size_t>(new_c), -1));
        for (int i = 0; i < rows; i += 2) {
            for (int j = 0; j < cols; j += 2) {
                const int a = current_grid[static_cast<size_t>(i)][static_cast<size_t>(j)];
                const int b = current_grid[static_cast<size_t>(i)][static_cast<size_t>(j+1)];
                const int c = current_grid[static_cast<size_t>(i+1)][static_cast<size_t>(j)];
                const int d = current_grid[static_cast<size_t>(i+1)][static_cast<size_t>(j+1)];
                const int res = orders[order_index](a,b,c,d);
                new_grid[static_cast<size_t>(i/2)][static_cast<size_t>(j/2)] = res;
            }
        }
        current_grid.swap(new_grid);
        ++step;
        cout << "After step " << step << " (" << current_grid.size() << "x"
             << (current_grid.empty() ? 0 : current_grid[0].size()) << "):\n";
        print_grid(current_grid, symbols);
        cout << "\n";
    }

    cout << "--- End of MagmaDecrypt: reduction terminated ---\n";
    return 0;
}

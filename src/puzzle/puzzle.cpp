#include <iostream>
using std::cout;
#include <fstream>
using std::ifstream;
using std::ios;
#include <string>
using std::string;
#include <vector>
using std::vector;
#include <cstdlib>
using std::rand;
#include <ctime>
using std::time;

/******************************************************************
 * This will read in puzzles from the lichess database, in FEN,
 * provided that you download and unzip it to the same directory.
 * It reads puzzles sequentially from a randomly selected offset,
 * which uses the current time as a seed, so will differ between
 * executions.
 * 
 * Be warned this may make it hard to reproduce bugs/errors if you
 * do not see or store the FEN somewhere.
 *
 * The puzzle database is available here: 
 *       https://database.lichess.org/#puzzles
 ******************************************************************/   

bool read_lines(std::vector<std::string> & vec) {

    ifstream file;

    file.open("src/puzzle/lichess_db_puzzle.csv", ios::in);
    if (!file) {
        cout << "WARNING: could not read puzzle csv\n";
        return false;
    }

    string s;

    // before reading the required number of puzzles
    while (getline(file, s)) {
        vec.push_back(s);
    }

    return true;

}

/* fill the given vector with [num] randomly selected puzzles 
    these puzzles will always differ since they are selected using a random seed */
bool read_fens(unsigned num, vector<string> & vec) {
       
    ifstream file;
    
    file.open("src/puzzle/lichess_db_puzzle.csv", ios::in);
    if (!file) {
        cout << "WARNING: could not read puzzle csv\n";
        return false;
    }
    
    string s;
    
    // initialise the seed with current time
    srand(time(nullptr));
    
    // and skip a random number of puzzles (different every time)
    unsigned x = rand() % 10000;
    while (x--)
        getline(file, s);
    
    // before reading the required number of puzzles
    for (; getline(file, s) && num > 0; --num) {
        
        // FEN is second field of CSV
        s = s.substr(s.find_first_of(',') + 1);
        s = s.substr(0, s.find_first_of(','));
        vec.push_back(s);
        
    } 

    return true;

}

bool read_all_fens(vector<string> & vec) {

    ifstream file;
    
    file.open("src/puzzle/lichess_db_puzzle.csv", ios::in);
    if (!file) {
        cout << "WARNING: could not read puzzle csv\n";
        return false;
    }
    
    string s;
    
    // before reading the required number of puzzles
    while (getline(file, s)) {
        
        // FEN is second field of CSV
        s = s.substr(s.find_first_of(',') + 1);
        s = s.substr(0, s.find_first_of(','));
        vec.push_back(s);
        
    }

    return true;

}

std::string random_fen() {
    ifstream file;

    file.open("src/puzzle/lichess_db_puzzle.csv", ios::in);
    if (!file) {
        cout << "WARNING: could not read puzzle csv\n";
        return "";
    }

    string s;

    // initialise the seed with current time
    srand(time(nullptr));

    // and skip a random number of puzzles (different every time)
    unsigned x = rand() % 1000000;
    while (x--)
        getline(file, s);

    // FEN is second field of CSV
    s = s.substr(s.find_first_of(',') + 1);
    s = s.substr(0, s.find_first_of(','));
    return s;

}

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <map>
#include <chrono> 

using namespace std;

/*

    Using simulated annealing technique to solve the 8-queens problem.

*/

/*

    State --> An instantiation of this class represents a board position, and provides ability to randomly move
                one of the queens from it's position, yielding a new state.
                
                This class also provides static functions to generate a random starting
                position, as well as calculate the 'cost' or 'number of queens that could be taken'
                of a given state.

*/

class State {
    private:
        vector< vector<int> > board;
        vector< pair<int,int> > queenLocations;
    public:
        static State * generateRandom();
        static int cost(State * s);
        State * makeRandomMove();
        void printBoard();
};

State * State::generateRandom() {
    State * s = new State();
    s->board = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };
    srand((unsigned) time(0));
    int i = rand() % 8;
    int j = rand() % 8;
    vector< pair<int,int> > usedSquares;
    for(int k=0; k<8; ++k) {
        while(count(usedSquares.begin(), usedSquares.end(), make_pair(i, j))) {
            i = rand() % 8;
            j = rand() % 8;
        }
        s->board[i][j] = 1;
        usedSquares.push_back(make_pair(i, j));
    }
    s->queenLocations = usedSquares;
    return s;
}

// Cost will be sum of the amount of queens that each queen could take
int State::cost(State * s) {
    int cost = 0;
    // First we check if any queens are on the same row or column
    vector<int> columnCount(8);
    vector<int> rowCount(8);
    for(pair<int,int> p : s->queenLocations) {
        rowCount[p.first]++;
        columnCount[p.second]++;
    }
    // We add the number of queens that could be taken on each row and column to cost
    for(int x : rowCount) {
        if(x > 1) {
            cost += x;
        }
    }
    for(int x : columnCount) {
        if(x > 1) {
            cost += x;
        }
    }

    // Now we consider the diagonal lines
    vector<int> negDiag(16);
    vector<int> posDiag(16);
    // We can identify each diagonal line by the difference between i and j
    // found on it. We use i-j to distinguish the negative diagonals (top right to bottom left)
    // and j-i to distinguish the positive diagonals. We add +8 to this result to eliminate negative
    // values, in order to be able to use this result as an index of the appropriate vector.
    for(pair<int,int> p : s->queenLocations) {
        negDiag[(p.first - p.second) + 8]++;
        posDiag[(p.second - p.first) + 8]++;
    }

    for(int x : negDiag) {
        if(x > 1) {
            cost += x;
        }
    }
    for(int x : posDiag) {
        if(x > 1) {
            cost += x;
        }
    }

    return cost;
}

// Move one randomly selected queen to a randomly selected
// position on the board
State * State::makeRandomMove() {
    // Copy this object's members over to a new state (so we can compare costs later)
    State * s = new State();
    s->board = board;
    s->queenLocations = queenLocations;

    int i = rand() % 8;
    int j = rand() % 8;
    while(count(s->queenLocations.begin(), s->queenLocations.end(), make_pair(i, j))) {
        i = rand() % 8;
        j = rand() % 8;
    }

    // Choosing a queen randomly and moving them to i,j on the board
    int r = rand() % 8;
    s->board[queenLocations[r].first][queenLocations[r].second] = 0; // old position = 0
    s->queenLocations[r] = make_pair(i, j);
    s->board[i][j] = 1; // new position = 1

    return s;
}

void State::printBoard() {
    for(vector<int> v : board) {
        for(int i : v) {
            cout << i << " ";
        }
        cout << endl;
    }
}

/*

    Simulated Annealing Algorithm:

    Some variable definitions:
    T, deltaT --> this is the temperature and rate of change of temperature. 
                    Together, these variables decide how many iterations of the algorithm will be run.
                    The more iterations, the more likely we are to finish in a solution state.
                    
                    These can be tweaked to optimise the algorithm.
    
    current --> The current board state we are on. This changes as the algorithm progresses, and should over
                time trend towards a global minimum for cost(). 


    Algorithm:
        1. Randomly generate a board state. Let current state equal this randomly generated state.
        2. Move one of the queens randomly, making a new 'neighbour' state.
        3. Assess whether or not this new board state is better than the current one, using cost function.
         - If so, let current state equal neighbour state, go to step 2.
         - If not, stay at the same state and go to step 2 OR there's a small probability
            that we progress to this new state even though it's cost is not better.
        4. Update temperature. If temperature is very low, we go to step 5. Otherwise, go to step 2.
        5. return the current state.

*/


State * SA(State * startingState, float Tparam, float deltaTparam) {
    State * current = startingState;
    float T = Tparam;
    float deltaT = deltaTparam;
    bool finalStateFound = false;
    while(T > 2 && !finalStateFound) {
        State * neighbour = current->makeRandomMove();
        if(State::cost(neighbour) < State::cost(current)) {
            current = neighbour;
        } else {
            int c = State::cost(current) - State::cost(neighbour);
            int p = exp(c / T);
            if(p > rand() / double(RAND_MAX)) {
                current = neighbour;
            }
        }
        T *= deltaT;
        if(State::cost(current) == 0) {
            finalStateFound = true;
        }
    }
    return current;
}

using namespace std::chrono;

int main() {

    // Example for T=10000, deltaT=0.999
    State * s = State::generateRandom();

    cout << "Starting state:" << endl;
    s->printBoard();
    cout << "Cost of s: "  << State::cost(s) << endl << endl;

    State * t = SA(s, 10000, 0.999);
    cout << "Finishing state:" << endl;
    t->printBoard();
    cout << "Cost of t: " << State::cost(t) << endl << endl;

    /*

        Experiment to measure efficiency for different parameters

    */

    // These values will all be tested with one another to compare avg. cost and time.
    vector<float> temps = { 100, 1000, 10000 };
    vector<float> deltas = { 0.9, 0.99, 0.999 };
    
    // The algorithm will be run N times in the experiment
    int N = 100;

    map<pair<float,float>, int> costMap;
    map<pair<float,float>, float> durationMap;

    for(int i=0; i<N; ++i) {
        for(float T : temps) {
            for(float d : deltas) {
                auto start = high_resolution_clock::now();
                State * q = State::generateRandom();
                State * r = SA(s, T, d);
                auto stop = high_resolution_clock::now();
                auto duration = duration_cast<milliseconds>(stop - start);
                if(costMap.find(make_pair(T, d)) == costMap.end()) {
                    costMap.emplace(make_pair(T, d), State::cost(r));
                } else {
                    costMap.emplace(make_pair(T, d), ((costMap[make_pair(T, d)] * i-1) + State::cost(r)) / i);
                }
                if(durationMap.find(make_pair(T, d)) == durationMap.end()) {
                    durationMap.emplace(make_pair(T, d), duration.count());
                } else {
                    durationMap.emplace(make_pair(T, d), ((durationMap[make_pair(T, d)] * i-1) + duration.count()) / i);
                }
            }
        }
    }

    printf("%-8s%-10s%-12s%-10s%", "T", "deltaT", "Avg. Cost", "Avg. Time (ms)");
    cout << endl;

    for (auto const& x : costMap) {
        printf("%-8.0f%-10.3f%-12d%-10.0f%", x.first.first, x.first.second, x.second, durationMap[make_pair(x.first.first, x.first.second)]);
        cout << endl;
    }

    /*

    Results with:
        N = 10000
        temps = { 100, 1000, 10000};
        deltas = { 0.9, 0.99, 0.999};

    --> a clear performance improvement for deltaT = 0.999 in terms of finding solutions.
    --> The time is similar for T=1000 and T=10000 but clearly slower for T=100

    T       deltaT    Avg. Cost   Avg. Time (ms)
    100     0.900     6           1         
    100     0.990     2           22        
    100     0.999     0           27        
    1000    0.900     6           2         
    1000    0.990     2           20        
    1000    0.999     0           16        
    10000   0.900     6           1         
    10000   0.990     0           17        
    10000   0.999     0           16 

    */

    return 0;
}
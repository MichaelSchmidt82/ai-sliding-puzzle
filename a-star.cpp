/*
Author:         Michael Schmidt
Class:			CSCI 4350 - Intro to Artificial Intelligence
Instructor:	    Dr. Joshua Phillips
Due:            28 Sept 2017

Description:    An a-star implementation

Assumptions:    Board is N x N.
*/

#include "globals.h"
#include "state.h"
#include "pair.h"

/* Prototypes */
void expand_state (StatePtr parent,
    set<StatePtr, ClosedPtrCompare> & closed_list,
    set<StatePtr, FrontierPtrCompare> & frontier);
void init_board(Board & board, size_t & side);

//////////////
/* INT MAIN */
//////////////

int main (int argc, char *argv[]) {
	/* Statistics */
	size_t v = 0;
	size_t n;
	size_t d;
	double b;

	/* Variables */
    size_t side; // side length of puzzle
    Path path;
    Board board;
	StatePtr curr_state;
	set<StatePtr, ClosedPtrCompare> closed_list;
	set<StatePtr, FrontierPtrCompare> frontier;

    /* Create initial state */
    init_board(board, side);
    try {
        curr_state = new State(board, side, Mode(atoi(argv[1])));
    } catch (bad_alloc & ba) {
        cerr << "bad allocation: " << ba.what() << endl;
    }
    /* Perform a-star search */
	while (!curr_state->is_goal()) {
		closed_list.insert(curr_state);
		expand_state(curr_state, closed_list, frontier);

		curr_state = *frontier.begin();
		frontier.erase(frontier.begin());
		v++;
	}

	/* Compute Stats */
	n = frontier.size() + closed_list.size();
	d = curr_state->get_depth();
	b = pow(n, 1 / double(d));

	/* Print results */
	cout << "V=" << v << endl;
	cout << "N=" << n << endl;
	cout << "d=" << d << endl;
	cout << "b=" << b << endl;
	cout << endl;

    /* Push states onto a stack for path */
	while (curr_state != nullptr) {
		path.push(curr_state);
		curr_state = curr_state->get_parent();
	}

    /* Print path/state history */
    while (!path.empty()) {
		curr_state = path.top();
		curr_state->print_board();
		cout << endl;
		path.pop();
	}

	for (StatePtr p : frontier)
		if (p != nullptr) {
			delete p;
			p = nullptr;
		}

	for (StatePtr p : closed_list)
		if (p != nullptr) {
			delete p;
			p = nullptr;
		}

  /* Exit */
	return 0;
}

///////////////
/* Functions */
///////////////

void expand_state (StatePtr parent, set<StatePtr, ClosedPtrCompare> & closed_list, set<StatePtr, FrontierPtrCompare> & frontier) {
	static int id = 0;
	StatePtr expand[4];

	/* Generate new states */
	for (int i = 0; i < 4; i++) {
        try {
            expand[i] = new State(parent);
        } catch (bad_alloc& ba) {
            cerr << "bad allocation: " << ba.what() << endl;
        }
    }

    /* Create new states, remove ones that fail */
	if (!expand[0]->move_down()) {
		delete expand[0];
		expand[0] = nullptr;
	}

	if (!expand[1]->move_left()) {
		delete expand[1];
		expand[1] = nullptr;
	}

	if (!expand[2]->move_up()) {
		delete expand[2];
		expand[2] = nullptr;
	}

	if (!expand[3]->move_right()) {
		delete expand[3];
		expand[3] = nullptr;
	}

	/* check created states against known (closed) states */
	for (int i = 0; i < 4; i++)
		if (expand[i] != nullptr) {
			auto find = closed_list.find(expand[i]);
			if (find == closed_list.end()) {
				expand[i]->attach_parent(parent);
				frontier.insert(expand[i]);
			}
		}
}

/* Create board using stdin */
void init_board (Board & board, size_t & side) {
    Tile tile;
    vector<Tile> parse;

    while (cin >> tile)
        parse.push_back(tile);

    assert(ceil(sqrt(parse.size())) == sqrt(parse.size()) && "Assumes NxN puzzle.");
    side = sqrt(parse.size());

    try {
        board = new TilePtr[side];

    	for (int r = 0, i = 0; r < side; r++) {
            board[r] = new Tile[side];
            for (int c = 0; c < side; c++, i++)
                board[r][c] = parse[i];
        }
    } catch (bad_alloc & ba) {
        cerr << "bad allocation: " << ba.what() << endl;
    }
}

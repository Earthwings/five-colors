// Copyright 2018 Dennis Nienhüser
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include "puzzle.h"

#include <cmath>
#include <iostream>
#include <iterator>
#include <algorithm>

using namespace std;

Stone::Stone(const std::string &value)
{
    fields.resize(value.size());
    for (size_t i = 0; i < value.size(); ++i) {
        fields[i] = value[i];
    }
}

inline bool Stone::operator==(const Stone &other) const
{
    return fields == other.fields;
}

inline bool Position::operator==(const Position &other) const
{
    return size == other.size && row == other.row && col == other.col &&
           horizontal == other.horizontal && reverse == other.reverse;
}

inline bool Position::operator<(const Position &b) const
{
    if (row != b.row) {
        return row < b.row;
    }
    if (col != b.col) {
        return col < b.col;
    }
    if (horizontal != b.horizontal) {
        return horizontal < b.horizontal;
    }
    if (size != b.size) {
        return size < b.size;
    }
    return reverse < b.reverse;
}

Board::Board(size_t size) : data_(size, Row(size, empty_)), size_(size)
{
    // does nothing
}

inline size_t Board::size() const
{
    return size_;
}

inline bool Board::isEmpty(size_t row, size_t col) const
{
    assert(row < size_);
    assert(col < size_);
    return data_[row][col] == empty_;
}

inline bool Board::canAssign(const Position &position, const Stone &stone) const
{
    size_t row = position.row;
    size_t col = position.col;
    for (size_t i = 0, n = stone.fields.size(); i < n; ++i) {
        if (row >= size_ || col >= size_ || data_[row][col] != empty_) {
            return false;
        }
        row += position.horizontal ? 0 : 1;
        col += position.horizontal ? 1 : 0;
    }
    return true;
}

inline void Board::unassign(const Position &position, const Stone &stone)
{
    Stone empty = stone;
    empty.fields.assign(empty.fields.size(), empty_);
    assign(position, empty);
}

void Board::print() const
{
    printf("%s", string(4 * size_, '-').c_str());
    printf("\n");
    for (size_t row = 0; row < size_; ++row) {
        cout << '|';
        for (size_t col = 0; col < size_; ++col) {
            printf(" %c |", data_[row][col]);
        }
        printf("\n");
    }
    printf("%s", string(4 * size_, '-').c_str());
    printf("\n");

    if (isValid()) {
        cout << "Board is valid";
        if (isFull()) {
            cout << " and full." << endl;
        } else {
            cout << "." << endl;
        }
    } else {
        cout << "BOARD IS NOT VALID." << endl;
    }
}

string Board::signature() const
{
    string result;
    for (auto const &row : data_) {
        result += string(row.begin(), row.end());
    }
    return result;
}

ostream &operator<<(ostream &stream, const Board &board)
{
    stream << board.signature();
    return stream;
}

Layout::Layout(size_t boardSize) : size_(boardSize)
{
    // nothing to do
}

inline size_t Layout::boardSize() const
{
    return size_;
}

void Layout::add(const Position &position)
{
    fill_ += position.size;
    positions_.push_back(position);
    if (isFull()) {
        normalize();
    }
}

bool Layout::isFull() const
{
    return size_ * size_ == fill_;
}

inline std::vector<Position> const & Layout::positions() const
{
    return positions_;
}

string Layout::signature() const
{
    Board board(size_);
    char counter = 'A';
    for (auto const &position : positions_) {
        string value;
        value.reserve(position.size);
        for (size_t i = 0; i < position.size; ++i) {
            value.push_back(counter);
        }
        Stone stone(value);
        board.assign(position, stone);
        ++counter;
    }
    return board.signature();
}

void Layout::rotate90()
{
    for (auto &position : positions_) {
        if (position.horizontal) {
            rotate90(position.row, position.col);
        } else {
            position.row += position.size - 1;
            rotate90(position.row, position.col);
        }
        position.horizontal = !position.horizontal;
    }
    normalize();
}

void Layout::flipHorizontal()
{
    for (auto &position : positions_) {
        size_t const shift = position.horizontal ? 1 : position.size;
        position.row = size_ - (position.row + shift);
    }
    normalize();
}

void Layout::flipVertical()
{
    for (auto &position : positions_) {
        size_t const shift = position.horizontal ? position.size : 1;
        position.col = size_ - (position.col + shift);
    }
    normalize();
}

void Layout::rotate90(size_t &row, size_t &col) const
{
    auto const original_col = col;
    col = size_ - 1 - row;
    row = original_col;
}

void Layout::normalize()
{
    sort(positions_.begin(), positions_.end());
}

Layouts Layout::unify(const Layouts &layouts)
{
    struct LayoutVariants {
        Layout layout;
        list<string> signatures;

        LayoutVariants(const Layout &layout_) : layout(layout_)
        {
            Layout worker = layout;
            for (int k = 0; k < 2; ++k) {
                for (int j = 0; j < 2; ++j) {
                    for (int i = 0; i < 4; ++i) {
                        signatures.push_back(worker.signature());
                        worker.rotate90();
                    }
                    worker.flipHorizontal();
                }
                worker.flipVertical();
            }
            assert(worker == layout);
        }
    };

    Layouts unique;
    list<LayoutVariants> variants;

    for (auto const &layout : layouts) {
        auto const signature = layout.signature();
        bool is_unique = true;
        for (auto const &variant : variants) {
            if (find(variant.signatures.begin(), variant.signatures.end(), signature) != variant.signatures.end()) {
                is_unique = false;
            }
            if (!is_unique) {
                break;
            }
        }
        if (is_unique) {
            variants.push_back({layout});
        }
    }

    transform(variants.begin(), variants.end(), back_inserter(unique),
              [](LayoutVariants const & variants) -> Layout { return variants.layout; });

    return unique;
}

bool Layout::operator==(const Layout &other) const
{
    return other.positions_ == positions_;
}

ostream &operator<<(ostream &stream, const Layout &layout)
{
    for (auto const & position : layout.positions_) {
        stream << "At (" << position.row << "," << position.col << ": ";
        stream << string(position.size, position.horizontal ? 'H' : 'V');
        stream << "\n";
    }

    Board board(layout.size_);
    for (auto const &position : layout.positions_) {
        char const value = (position.horizontal ? 'A' : 'a') + char(position.size);
        Stone stone(string(position.size, value));
        board.assign(position, stone);
    }
    stream << board;
    return stream;
}

inline bool Layout::operator<(const Layout &other) const
{
    return positions_ < other.positions_;
}

Solver::Solver(const Layout &layout, const Stones &stones)
    : layout_(layout), stones_(stones)
{
    // nothing to do
}

Solutions Solver::findAssignment() const
{
    Board board(layout_.boardSize());
    Stones stones = stones_;
    size_t layoutIndex = 0;
    Solution solution;
    Solutions solutions;
    findAssignment(solutions, board, stones, layoutIndex, solution);
    return solutions;
}

void Solver::printSolution(const Solution &solution) const
{
    map<char, string> names;
    names['B'] = "blue";
    names['C'] = "cyan";
    names['D'] = "black";
    names['F'] = "fuchsia";
    names['G'] = "green";
    names['I'] = "indigo";
    names['K'] = "khaki";
    names['L'] = "lime";
    names['M'] = "magenta";
    names['O'] = "orange";
    names['P'] = "pink";
    names['R'] = "red";
    names['S'] = "silver";
    names['T'] = "teal";
    names['V'] = "violet";
    names['W'] = "white";
    names['Y'] = "yellow";
    cout << "Solution:" << endl;
    for (auto const &assignment : solution) {
        Position const &position = assignment.first;
        Stone const &stone = assignment.second;
        cout << "(" << position.row + 1 << "," << position.col + 1;
        auto value = stone.fields;
        if (position.reverse) {
            reverse(value.begin(), value.end());
        }
        cout << (position.horizontal ? ", horizontal) " : ", vertical)   ");
        for (auto v : value) {
            cout << names[v] << ' ';
        }
        cout << '\n';
    }
    cout << "Rotate and mirror this solution to produce variants of it.\n";
}

void Solver::findAssignment(Solutions &solutions, Board &board, Stones &stones, size_t layoutIndex,
                            Solution &solution) const
{
    auto const boardValid = board.isValid();
    if (!boardValid) {
        // No more solutions possible, stop recursion
        return;
    }

    if (boardValid && stones.empty()) {
        // Solution found, stop recursion
        solutions.push_back(solution);
        printSolution(solution);
        board.print();
        return;
    }

    auto const & positions = layout_.positions();
    for (size_t i = 0, n = stones.size(); i < n; ++i) {
        Stone stone = stones.front();
        stones.pop_front();
        if (stone.fields.size() != positions[layoutIndex].size) {
            // Stone in queue does not fit with position's stone type (size). Queue it.
            stones.push_back(stone);
            continue;
        }
        {
            // Try to fit the stone in forward direction. If it works, move on. Later on clean up.
            board.assign(positions[layoutIndex], stone);
            if (board.isValid()) {
                solution.push_back({positions[layoutIndex], stone});
                findAssignment(solutions, board, stones, layoutIndex + 1, solution);
                solution.pop_back();
            }
            board.unassign(positions[layoutIndex], stone);
        }

        {
            // Try to fit the stone in backward direction. If it works, move on. Later on clean up.
            Position reversed = positions[layoutIndex];
            reversed.reverse = !reversed.reverse;
            board.assign(reversed, stone);
            if (board.isValid()) {
                solution.push_back({reversed, stone});
                findAssignment(solutions, board, stones, layoutIndex + 1, solution);
                solution.pop_back();
            }
            board.unassign(reversed, stone);
        }
        stones.push_back(stone);
    }
}

Layouts LayoutGenerator::findAll(const Stones &stones)
{
    // Determine board size from stones
    size_t all = 0;
    for (auto const &stone : stones) {
        all += stone.fields.size();
    }
    size_t const board_size = size_t(sqrt(all));
    if (board_size * board_size != all) {
        cerr << "Stones do not fit into a squared board." << endl;
        return Layouts();
    }

    vector<Store> store(board_size + 1);
    for (auto const &stone : stones) {
        if (stone.fields.size() > board_size) {
            cerr << "Stone ";
            copy(stone.fields.begin(), stone.fields.end(), ostream_iterator<char>(cerr, ""));
            cerr << " does not fit into the board." << endl;
            return Layouts();
        }
        ++store[stone.fields.size()].count;
    }

    for (size_t i = 1; i <= board_size; ++i) {
        store[i].stone = Stone(string(i, 'A'));
    }

    Layouts layouts;
    vector<Position> layout;
    Board board(board_size);
    findAll(layouts, layout, board, store, 0);
    return layouts;
}

void LayoutGenerator::findAll(Layouts &layouts, vector<Position> &layout, Board &board,
                              vector<Store> &store, size_t step)
{
    auto const board_size = board.size();
    if (step >= board_size * board_size) {
        // Everything tried, stop recursion
        return;
    }
    size_t const row = step / board_size;
    size_t const col = step % board_size;
    if (!board.isEmpty(row, col)) {
        // Cannot assign anything here, but a later position might still work
        findAll(layouts, layout, board, store, step + 1);
    }
    for (size_t k = 1; k <= board_size; ++k) {
        auto & reserve = store[k];
        if (reserve.count == 0) {
            // No more stones of this size
            continue;
        }
        // Recurse into all possible assignments
        for (size_t horizontal = 0; horizontal < 2; ++horizontal) {
            Position const position({k, row, col, bool(horizontal), false});
            if (board.canAssign(position, reserve.stone)) {
                board.assign(position, reserve.stone);
                layout.push_back(position);
                if (board.isFull()) {
                    // Layout is valid, store it
                    Layout result(board.size());
                    for (auto const &pos : layout) {
                        result.add(pos);
                    }
                    layouts.push_back(result);
                }
                // For horizontal stones some steps can be skipped directly
                size_t const next_step = step + (horizontal ? k : 1);
                --reserve.count;
                findAll(layouts, layout, board, store, next_step);
                // Clean up
                ++reserve.count;
                board.unassign(position, store[k].stone);
                layout.pop_back();
            }
        }
    }
}

Stones &operator<<(Stones &stones, const string &value)
{
    stones.push_back({value});
    return stones;
}

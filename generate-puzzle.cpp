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

#include <iostream>
#include <set>
#include <random>
#include <functional>
#include <algorithm>

using namespace std;

void swapRows(Board &board, size_t rowA, size_t rowB)
{
    for (size_t col = 0, n = board.size(); col < n; ++col) {
        swap(board.at(rowA, col), board.at(rowB, col));
    }
}

void swapCols(Board &board, size_t colA, size_t colB)
{
    for (size_t row = 0, n = board.size(); row < n; ++row) {
        swap(board.at(row, colA), board.at(row, colB));
    }
}

bool isNice(Solution const &solution, Layouts const & layouts, size_t &num_solutions)
{
    Stones stones;
    set<string> values;
    for (auto const &value : solution) {
        stones.push_back(value.second);
        values.insert(value.second.value());
    }
    if (values.size() != solution.size()) {
        return false;
    }

    num_solutions = 0;
    for (auto const &layout : layouts) {
        num_solutions += Solver(layout, stones).findAssignment().size();
    }

    return num_solutions > 0;
}

int main(int argc, char* argv[])
{
    vector<size_t> stones(size_t(argc), 0);
    for (size_t i = 1; i < size_t(argc); ++i) {
        stones[i] = stoull(argv[i]);
    }
    if (stones.empty()) {
        cout << "Usage: " << argv[0] << " STONE1 STONE2 STONE3 ...\n";
        cout << "A STONE is a string where each character represents a certain color, e.g. GRB for green red blue.\n";
        cout << "Pass e.g. GRB BGR RBG for a 3x3 board." << endl;
        return 0;
    }

    auto const layouts = LayoutGenerator::findAll(stones);
    string const colors = "BDGYRVOMPSTWCFIKL";
    for (auto const &layout : layouts) {
        auto const size = layout.boardSize();
        if (size >= colors.size()) {
            cerr << "Board is too large: " << layout.boardSize() << " exceeds maximum board size " << colors.size() << "." << endl;
            return 1;
        }
        Board board(size);
        size_t index = 0;
        for (size_t row = 0; row < size; ++row) {
            for (size_t col = 0; col < size; ++col) {
                index = (index + 1) % size;
                board.assign(row, col, colors[index]);
            }
            ++index;
        }
        std::default_random_engine generator;
        std::uniform_int_distribution<size_t> distribution(0, size - 1);
        auto dice = std::bind(distribution, generator);
        for (int i = 0; i < 10; ++i) {
            if (dice() > size / 2) {
                swapRows(board, dice(), dice());
            } else {
                swapCols(board, dice(), dice());
            }
        }

        Solution solution;
        for (auto const &position : layout.positions()) {
            size_t row = position.row;
            size_t col = position.col;
            string fields;
            fields.reserve(position.size);
            Counter counter;
            for (size_t i = 0, n = position.size; i < n; ++i) {
                auto const value = board.at(row, col);
                fields.push_back(value);
                counter.add(value);
                row += position.horizontal ? 0 : 1;
                col += position.horizontal ? 1 : 0;
            }
            if (position.reverse) {
                reverse(fields.begin(), fields.end());
            }
            Stone const stone(fields);
            solution.push_back(make_pair(position, stone));
        }

        size_t solutions = 0;
        if (isNice(solution, layouts, solutions)) {
            cout << "Found " << solutions << " solutions, among them this one:" << endl;
            Solver::printSolution(solution);
            cout << "The board looks like this:" << endl;
            board.print();
        }
    }
}

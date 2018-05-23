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

#ifndef PUZZLE_H
#define PUZZLE_H

#include <cassert>
#include <list>
#include <map>
#include <vector>
#include <array>
#include <iostream>

// A stone that can be set in the game board
struct Stone {
    Stone(const std::string &value);
    bool operator==(const Stone &other) const;
    std::vector<char> fields;
    std::string value() const;
};
using Stones = std::list<Stone>;
Stones& operator<<(Stones& stones, const std::string &value);

// Encodes the position of a stone in the board
struct Position {
    size_t size = 1;
    size_t row = 0;
    size_t col = 0;
    bool horizontal = true;
    bool reverse = false;

    bool operator==(const Position &other) const;
    bool operator<(const Position &b) const;
};
using Solution = std::list<std::pair<Position, Stone>>;
using Solutions = std::list<Solution>;

// Determines if a string consists of unique characters. Extra data structure for performance reasons.
class Counter
{
public:
    Counter() = default;

    void add(char value)
    {
        if (value < min_) {
            std::cerr << "Character " << value << " is too small, please decrease Counter::min_ field from now " << min_ << " to (at least) " << value << std::endl;
            exit(3);
        }
        size_t const val = size_t(value - min_);
        if (val >= seen_.size()) {
            std::cerr << "Character " << value << " is too large, please increase Counter::seen_ field from now " << seen_.size() << " to (at least) " << val + 1 << std::endl;
            exit(5);
        }

        if (valid_) {
            if (seen_[val] == true) {
                valid_ = false;
            } else {
                seen_[val] = true;
            }
        }
    }
    bool isValid() const
    {
        return valid_;
    }

private:
    bool valid_ = true;
    static constexpr char const min_ = '0';
    std::vector<bool> seen_ = std::vector<bool>(80, false);
};

// Game board with a matrix-like data structure
class Board
{
public:
    using Cell = char;
    using Row = std::vector<Cell>;
    using Matrix = std::vector<Row>;

    explicit Board(size_t size);
    explicit Board(size_t size, const Solution &solution);

    char & at(size_t row, size_t col)
    {
        return data_[row][col];
    }

    char at(size_t row, size_t col) const
    {
        return data_.at(row).at(col);
    }

    size_t size() const;
    void assign(size_t row, size_t col, char value)
    {
        assert(row < size_);
        assert(col < size_);
        assert(value == empty_ || data_[row][col] == empty_);
        data_[row][col] = value;
        fill_ = (value == empty_ ? fill_ - 1 : fill_ + 1);
    }
    bool isEmpty(size_t row, size_t col) const;
    bool canAssign(const Position &position, const Stone &stone) const;
    void assign(const Position &position, const Stone &stone)
    {
        size_t row = position.row;
        size_t col = position.col;
        if (position.reverse) {
            for (int i = int(stone.fields.size()) - 1; i >= 0; --i) {
                assign(row, col, stone.fields[size_t(i)]);
                row += position.horizontal ? 0 : 1;
                col += position.horizontal ? 1 : 0;
            }
        } else {
            for (size_t i = 0, n = stone.fields.size(); i < n; ++i) {
                assign(row, col, stone.fields[i]);
                data_[row][col] = stone.fields[i];
                row += position.horizontal ? 0 : 1;
                col += position.horizontal ? 1 : 0;
            }
        }
    }

    void unassign(const Position &position, const Stone &stone);
    bool isValid() const
    {
        for (size_t row = 0; row < size_; ++row) {
            Counter counter;
            for (size_t col = 0; col < size_; ++col) {
                if (data_[row][col] != empty_) {
                    counter.add(data_[row][col]);
                }
            }
            if (!counter.isValid()) {
                return false;
            }
        }

        for (size_t col = 0; col < size_; ++col) {
            Counter counter;
            for (size_t row = 0; row < size_; ++row) {
                if (data_[row][col] != empty_) {
                    counter.add(data_[row][col]);
                }
            }
            if (!counter.isValid()) {
                return false;
            }
        }

        return true;
    }
    bool isFull() const
    {
        return size_ * size_ == fill_;
    }
    void print() const;
    std::string signature() const;
    friend std::ostream& operator<< (std::ostream& stream, const Board& board);

private:
    Cell const empty_ = ' ';
    Matrix data_;
    size_t size_;
    size_t fill_ = 0;
};

// A set of possible assigments of stones to the board, where stone colors are ignored
class Layout
{
public:
    explicit Layout(size_t boardSize);
    size_t boardSize() const;
    void add(const Position & position);
    bool isFull() const;
    std::vector<Position> const & positions() const;
    static std::list<Layout> unify(const std::list<Layout> &layouts);
    std::string signature() const;
    void rotate90();
    void flipHorizontal();
    void flipVertical();
    bool operator==(const Layout & other) const;
    friend std::ostream& operator<< (std::ostream& stream, const Layout& layout);

private:
    void rotate90(size_t &row, size_t &col) const;
    void normalize();
    bool operator<(const Layout & other) const;

    std::vector<Position> positions_;
    size_t size_ = 0;
    size_t fill_ = 0;
};
using Layouts = std::list<Layout>;

// Brute force solution search for a given layout and a given set of stones
class Solver
{
public:
    Solver(const Layout & layout, const Stones & stones);
    Solutions findAssignment() const;
    static void printSolution(const Solution & solution);

private:
    void findAssignment(Solutions &solutions, Board & board, Stones & stones, size_t layoutIndex,
                        Solution & solution) const;

    Layout layout_;
    Stones stones_;
};

// Brute force layout search
class LayoutGenerator
{
public:
    static Layouts findAll(const std::vector<size_t> &stones);
    static Layouts findAll(const Stones & stones);

private:
    struct Store {
        Stone stone = Stone(std::string());
        size_t count = 0;
    };

    static void findAll(Layouts & layouts, std::vector<Position> & layout, Board & board,
                        std::vector<Store> & store, size_t step);
};

#endif

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

#include <iostream>
#include <cassert>

#include "puzzle.h"

#define VERIFY(cond) if (!(cond)) {std::cout << "unit test failed at " << __FILE__ << ":" << __LINE__ << std::endl; assert(false); exit(127); }
#define VERIFY_EQUAL(valA, valB) if (!(valA == valB)) {std::cout << "unit test failed at " << __FILE__ << ":" << __LINE__ << ". Failure: " << valA << " != " << valB << std::endl; assert(false); exit(127); }

using namespace std;

class SmallGame
{
public:
    SmallGame()
    {
        Board board(3);
        VERIFY(board.isValid());
        VERIFY(!board.isFull());
        board.print();

        Stone stoneA("RGB");
        board.assign({stoneA.fields.size(), 0, 0, true, false}, stoneA);
        VERIFY(board.isValid());
        VERIFY(!board.isFull());
        board.print();

        Stone stoneB("GBR");
        board.assign({stoneB.fields.size(), 1, 0, true, false}, stoneB);
        VERIFY(board.isValid());
        VERIFY(!board.isFull());
        board.print();

        Stone stoneC("BRG");
        board.assign({stoneC.fields.size(), 2, 0, true, false}, stoneC);
        VERIFY(board.isValid());
        VERIFY(board.isFull());
        board.print();
    }
};

class MediumGame
{
public:
    MediumGame()
    {
        Layout layout(4);
        layout.add({3, 0, 1, true, false});
        layout.add({3, 1, 1, true, false});
        layout.add({3, 2, 1, true, false});
        layout.add({3, 0, 0, false, false});
        layout.add({2, 3, 0, true, false});
        layout.add({2, 3, 2, true, false});
        VERIFY(layout.isFull());

        Stones stones;
        stones.push_back({"GBD"});
        stones.push_back({"RGB"});
        stones.push_back({"DRG"});
        stones.push_back({"RDB"});
        stones.push_back({"GB"});
        stones.push_back({"DR"});

        Solver solver(layout, stones);
        auto const solutions = solver.findAssignment();
        VERIFY_EQUAL(16, solutions.size());
    }
};

class LargeGame
{
public:
    LargeGame()
    {
        Layouts layouts;

        {
            // - - - | |
            // - - - | |
            // - - - | |
            // - - - ^ ^
            // - - - v v
            Layout layout(5);
            // Five 3-stones horizontal on the left
            layout.add({3, 0, 0, true, false});
            layout.add({3, 1, 0, true, false});
            layout.add({3, 2, 0, true, false});
            layout.add({3, 3, 0, true, false});
            layout.add({3, 4, 0, true, false});
            // Two 3-stones vertical on the top right
            layout.add({3, 0, 3, false, false});
            layout.add({3, 0, 4, false, false});
            // Two 2-stones vertical on the bottom right
            layout.add({2, 3, 3, false, false});
            layout.add({2, 3, 4, false, false});
            VERIFY(layout.isFull());
            layouts.push_back(layout);
        }

        {
            // - - - | |
            // - - - | |
            // - - - | |
            // - - - < >
            // - - - < >
            Layout layout(5);
            // Five 3-stones horizontal on the left
            layout.add({3, 0, 0, true, false});
            layout.add({3, 1, 0, true, false});
            layout.add({3, 2, 0, true, false});
            layout.add({3, 3, 0, true, false});
            layout.add({3, 4, 0, true, false});
            // Two 3-stones vertical on the top right
            layout.add({3, 0, 3, false, false});
            layout.add({3, 0, 4, false, false});
            // Two 2-stones horizontal on the bottom right
            layout.add({2, 3, 3, true, false});
            layout.add({2, 4, 3, true, false});
            VERIFY(layout.isFull());
            layouts.push_back(layout);
        }

        {
            // - - - | |
            // - - - | |
            // | | | | |
            // | | | < >
            // | | | < >
            Layout layout(5);
            // Two 3-stones horizontal on the left
            layout.add({3, 0, 0, true, false});
            layout.add({3, 1, 0, true, false});
            // Three 3-stones vertical on the left
            layout.add({3, 2, 0, false, false});
            layout.add({3, 2, 1, false, false});
            layout.add({3, 2, 2, false, false});
            // Two 3-stones vertical on the top right
            layout.add({3, 0, 3, false, false});
            layout.add({3, 0, 4, false, false});
            // Two 2-stones horizontal on the bottom right
            layout.add({2, 3, 3, true, false});
            layout.add({2, 4, 3, true, false});
            VERIFY(layout.isFull());
            layouts.push_back(layout);
        }

        {
            // - - - | |
            // - - - | |
            // | | ^ | |
            // | | v < >
            // | | - - -
            Layout layout(5);
            // Two 3-stones horizontal on the left
            layout.add({3, 0, 0, true, false});
            layout.add({3, 1, 0, true, false});
            // Two 3-stones vertical on the left
            layout.add({3, 2, 0, false, false});
            layout.add({3, 2, 1, false, false});
            // Two 3-stones vertical on the top right
            layout.add({3, 0, 3, false, false});
            layout.add({3, 0, 4, false, false});
            // One 3-stone horizontal on the bottom right
            layout.add({3, 4, 2, true, false});
            // Two 2-stones at remaining places
            layout.add({2, 2, 2, false, false});
            layout.add({2, 3, 3, true, false});
            VERIFY(layout.isFull());
            layouts.push_back(layout);
        }

        Stones stones;
        stones.push_back({"DRB"});
        stones.push_back({"RDG"});
        stones.push_back({"GYR"});
        stones.push_back({"YBD"});
        stones.push_back({"BGY"});
        stones.push_back({"BGD"});
        stones.push_back({"RDY"});
        stones.push_back({"YR"});
        stones.push_back({"GB"});

        for (auto const &layout : layouts) {
            Solver solver(layout, stones);
            solver.findAssignment();
        }
    }
};

class Variants
{
public:
    Variants()
    {
        // - - - | |
        // - - - | |
        // | | ^ | |
        // | | v < >
        // | | - - -
        Layout layout(5);
        // Two 3-stones horizontal on the left
        layout.add({3, 0, 0, true, false});
        layout.add({3, 1, 0, true, false});
        // Two 3-stones vertical on the left
        layout.add({3, 2, 0, false, false});
        layout.add({3, 2, 1, false, false});
        // Two 3-stones vertical on the top right
        layout.add({3, 0, 3, false, false});
        layout.add({3, 0, 4, false, false});
        // One 3-stone horizontal on the bottom right
        layout.add({3, 4, 2, true, false});
        // Two 2-stones at remaining places
        layout.add({2, 2, 2, false, false});
        layout.add({2, 3, 3, true, false});
        VERIFY(layout.isFull());

        auto const original_layout = layout;
        auto const signature_0 = layout.signature();
        for (int i=0; i<4; ++i) {
            layout.rotate90();
        }
        VERIFY_EQUAL(original_layout, layout);

        for (int i=0; i<2; ++i) {
            layout.flipHorizontal();
        }
        VERIFY_EQUAL(signature_0, layout.signature());

        for (int i=0; i<2; ++i) {
            layout.flipVertical();
        }
        VERIFY_EQUAL(signature_0, layout.signature());
    }
};

int main()
{
    SmallGame small_game;
    MediumGame medium_game;
    LargeGame large_game;
    Variants variants;
}

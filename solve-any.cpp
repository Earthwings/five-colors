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

using namespace std;

int main(int argc, char* argv[])
{
    Stones stones;
    for (int i=1; i<argc; ++i) {
        stones << argv[i];
    }
    if (stones.empty()) {
        cout << "Usage: " << argv[0] << " STONE1 STONE2 STONE3 ...\n";
        cout << "A STONE is a string where each character represents a certain color, e.g. GRB for green red blue.\n";
        cout << "Pass e.g. GRB BGR RBG for a 3x3 board." << endl;
        return 0;
    }

    auto const all_layouts = LayoutGenerator::findAll(stones);
    std::cout << "Found " << all_layouts.size() << " layouts of which ";
    auto const layouts = Layout::unify(all_layouts);
    std::cout << layouts.size() << " are unique layouts." << endl;
    size_t solution_count = 0;
    for (auto const &layout : layouts) {
        auto const solutions = Solver(layout, stones).findAssignment();
        solution_count += solutions.size();
    }
    cout << "Found " << solution_count << " solution(s) in total." << endl;

}
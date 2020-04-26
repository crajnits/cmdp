/* Copyright 2020 Chanchal Raj <craj.nits@gmail.com>
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
       http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <iostream>
#include <string>
#include "cmdp.h"

//---------------------------------------------------------------------------
// Main test function
//---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    // input: --filename test.jpg --ratio 15.6

    // init and parse input args
    cmdp::CmdParser parser;
    parser.parse(argc, argv);

    // read required parameters (availabe in args)
    std::string filename = parser.get<std::string>("--filename");       // test.jpg

    // read optional parameters with default values (available in args)
    float expRatio = parser.get<float>("--ratio", 100.56);              // 15.6

    // read optional parameters with default values (missing in args)
    int enable = parser.get<int>("--enable", 10);                       // 10

    // read required parameters (missing in args)
    int value = parser.get<int>("--value");                             // 0 and set error flag

    // check if any required paramters are missing.
    if (parser.printInput()) {                                          // print all params with status
        std::cout << "Input args are missing" << std::endl;
        return 0;
    }

    //...

    return 0;
}

//---------------------------------------------------------------------------
// End of File
//---------------------------------------------------------------------------

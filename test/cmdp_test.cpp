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
#include <sstream>
#include <string>
#include <vector>
#include <cmdp/cmdp.h>

//---------------------------------------------------------------------------
// Minimal test harness
//---------------------------------------------------------------------------
static int g_failures = 0;

#define CHECK(cond)                                                     \
    do {                                                                \
        if (!(cond)) {                                                  \
            ++g_failures;                                               \
            std::cout << "FAIL " << __FILE__ << ":" << __LINE__         \
                      << ": " << #cond << std::endl;                    \
        }                                                               \
    } while (0)

#define CHECK_EQ(actual, expected)                                      \
    do {                                                                \
        const auto a_ = (actual);                                       \
        const auto e_ = (expected);                                     \
        if (!(a_ == e_)) {                                              \
            ++g_failures;                                               \
            std::cout << "FAIL " << __FILE__ << ":" << __LINE__         \
                      << ": " << #actual << " == " << a_                \
                      << ", expected " << e_ << std::endl;              \
        }                                                               \
    } while (0)

//! \breif  build a parser from a literal argument list (argv[0] is added).
static cmdp::CmdParser makeParser(const std::vector<const char*>& args)
{
    std::vector<const char*> argv;
    argv.push_back("cmdp_test");
    argv.insert(argv.end(), args.begin(), args.end());
    return cmdp::CmdParser(static_cast<int>(argv.size()), argv.data());
}

//! defined in cmdp_test_tu2.cpp, links only if the header is ODR clean.
std::string secondTranslationUnitTypeName();

//---------------------------------------------------------------------------
// Tests
//---------------------------------------------------------------------------
static void testReadmeExample()
{
    cmdp::CmdParser parser = makeParser({"--filename", "test.jpg", "--ratio", "15.6"});

    CHECK_EQ(parser.get<std::string>("--filename"), std::string("test.jpg"));
    CHECK_EQ(parser.get<float>("--ratio", 100.56F), 15.6F);
    CHECK_EQ(parser.get<int>("--enable", 10), 10);      // missing, default used
    CHECK(!parser.getStatus());                         // still no error so far

    CHECK_EQ(parser.get<int>("--value"), 0);            // missing and required
    CHECK(parser.getStatus());
}

static void testMissingAndDefaults()
{
    cmdp::CmdParser parser = makeParser({"--a", "1"});

    CHECK(parser.has("--a"));
    CHECK(!parser.has("--b"));
    CHECK_EQ(parser.size(), static_cast<std::size_t>(1));

    CHECK_EQ(parser.get<int>("--a", 7), 1);
    CHECK_EQ(parser.get<int>("--b", 7), 7);
    CHECK(!parser.getStatus());     // a default is not an error

    CHECK_EQ(parser.get<int>("--b"), 0);
    CHECK(parser.getStatus());      // a missing required tag is
}

static void testStringWithSpacesIsNotTruncated()
{
    cmdp::CmdParser parser = makeParser({"--name", "John Smith"});

    CHECK_EQ(parser.get<std::string>("--name"), std::string("John Smith"));
    CHECK(!parser.getStatus());
}

static void testTrailingGarbageIsRejected()
{
    cmdp::CmdParser parser = makeParser({"--count", "12abc"});

    CHECK_EQ(parser.get<int>("--count", -1), -1);   // falls back to the default
    CHECK(parser.getStatus());
}

static void testInvalidValueSetsErrorEvenWithDefault()
{
    cmdp::CmdParser parser = makeParser({"--ratio", "not-a-number"});

    CHECK_EQ(parser.get<double>("--ratio", 2.5), 2.5);
    CHECK(parser.getStatus());
}

static void testNegativeNumbersAreValues()
{
    cmdp::CmdParser parser = makeParser({"--offset", "-5", "--scale", "-1.25", "--verbose"});

    CHECK_EQ(parser.get<int>("--offset"), -5);
    CHECK_EQ(parser.get<double>("--scale"), -1.25);
    CHECK(parser.has("--verbose"));
    CHECK(!parser.getStatus());
}

static void testUnsignedRejectsNegative()
{
    cmdp::CmdParser parser = makeParser({"--size", "-1"});

    CHECK_EQ(parser.get<unsigned int>("--size", 3U), 3U);
    CHECK(parser.getStatus());
}

static void testBoolForms()
{
    cmdp::CmdParser parser = makeParser({
        "--t", "true", "--f", "FALSE", "--y", "yes", "--n", "0", "--flag", "--bad", "maybe"
    });

    CHECK_EQ(parser.get<bool>("--t"), true);
    CHECK_EQ(parser.get<bool>("--f"), false);
    CHECK_EQ(parser.get<bool>("--y"), true);
    CHECK_EQ(parser.get<bool>("--n"), false);
    CHECK_EQ(parser.get<bool>("--flag", false), true);      // valueless tag is a flag
    CHECK(!parser.getStatus());

    CHECK_EQ(parser.get<bool>("--bad", false), false);
    CHECK(parser.getStatus());
}

static void testEqualsSyntax()
{
    cmdp::CmdParser parser = makeParser({"--filename=test.jpg", "--ratio=15.6", "--empty="});

    CHECK_EQ(parser.get<std::string>("--filename"), std::string("test.jpg"));
    CHECK_EQ(parser.get<float>("--ratio"), 15.6F);
    CHECK_EQ(parser.get<std::string>("--empty"), std::string(""));
    CHECK(!parser.getStatus());
}

static void testStrayPositionalDoesNotShiftPairs()
{
    // the old strictly pairwise loop turned this into {--a: 1, junk: --b}
    cmdp::CmdParser parser = makeParser({"--a", "1", "junk", "--b", "2"});

    CHECK_EQ(parser.get<int>("--a"), 1);
    CHECK_EQ(parser.get<int>("--b"), 2);
    CHECK(!parser.getStatus());
}

static void testEmptyAndMalformedArgv()
{
    cmdp::CmdParser empty = makeParser({});
    CHECK_EQ(empty.size(), static_cast<std::size_t>(0));
    CHECK(!empty.getStatus());

    CHECK_EQ(cmdp::CmdParser(0, nullptr).size(), static_cast<std::size_t>(0));

    cmdp::CmdParser trailing = makeParser({"--a", "1", "--b"});
    CHECK_EQ(trailing.get<int>("--a"), 1);
    CHECK(trailing.has("--b"));
}

static void testTypeNames()
{
    CHECK_EQ(cmdp::getTypeName<int>(), std::string("int"));
    CHECK_EQ(cmdp::getTypeName<float>(), std::string("float"));
    CHECK_EQ(cmdp::getTypeName<bool>(), std::string("bool"));
    CHECK_EQ(cmdp::getTypeName<std::string>(), std::string("string"));

    // linking at all proves the header is safe to include in several sources
    CHECK_EQ(secondTranslationUnitTypeName(), std::string("string"));
}

static void testPrintStatus()
{
    cmdp::CmdParser parser = makeParser({"--filename", "test.jpg", "--ratio", "15.6"});
    parser.get<std::string>("--filename");
    parser.get<float>("--ratio", 100.56F);
    parser.get<int>("--enable", 10);
    parser.get<int>("--value");

    std::ostringstream os;
    CHECK(parser.printStatus(os));      // returns the error state

    const std::string out = os.str();
    CHECK(out.find("[string] --filename") != std::string::npos);
    CHECK(out.find("[float ] --ratio") != std::string::npos);
    CHECK(out.find("Default") != std::string::npos);
    CHECK(out.find("Missing") != std::string::npos);

    std::cout << out;
}

//---------------------------------------------------------------------------
// Main test function
//---------------------------------------------------------------------------
int main()
{
    testReadmeExample();
    testMissingAndDefaults();
    testStringWithSpacesIsNotTruncated();
    testTrailingGarbageIsRejected();
    testInvalidValueSetsErrorEvenWithDefault();
    testNegativeNumbersAreValues();
    testUnsignedRejectsNegative();
    testBoolForms();
    testEqualsSyntax();
    testStrayPositionalDoesNotShiftPairs();
    testEmptyAndMalformedArgv();
    testTypeNames();
    testPrintStatus();

    if (g_failures != 0) {
        std::cout << g_failures << " check(s) failed" << std::endl;
        return 1;
    }
    std::cout << "all checks passed" << std::endl;
    return 0;
}

//---------------------------------------------------------------------------
// End of File
//---------------------------------------------------------------------------

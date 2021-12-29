# cmdp
`Simple C++ command line arguments parser`

This is a simple, header only C++ command line arguments parser library.

Options can be given as a tag value pair, in either form:

    --tag value
    --tag=value

A token is treated as a tag when it starts with `-` and is not a number, so
negative values such as `--offset -5` work as expected. A tag given without a
value (`--verbose`) is recorded as a flag.

## Uses

```cpp
#include <cmdp/cmdp.h>
```

Create a `cmdp::CmdParser` instance.

```cpp
cmdp::CmdParser parser(argc, argv);
```

Then read the required or optional parameters.

```cpp
// read required parameters
std::string filename = parser.get<std::string>("--filename");
```
```cpp
// read optional parameters (just pass the default values)
float expRatio = parser.get<float>("--ratio", 100.56F);
```

The error flag is set when a required tag is missing, or when a value cannot be
converted to the requested type. A value that only partly converts, such as
`--count 12abc`, is rejected rather than silently read as `12`. Check the flag
with `getStatus()`, or with `printStatus()` which also prints a summary.

### API

| Member | Description |
| --- | --- |
| `get<T>(tag)` | required parameter, sets the error flag if missing |
| `get<T>(tag, def)` | optional parameter, `def` is used if missing |
| `has(tag)` | true if the tag was given on the command line |
| `size()` | number of tags given on the command line |
| `printStatus(os = std::cout)` | print every queried parameter, returns the error state |
| `getStatus()` | returns the error state |

`bool` values accept `1/0`, `true/false`, `yes/no` and `on/off`, case
insensitively. A valueless tag reads as `true`. Any other type works as long as
it is default constructible and supports `operator>>`.

## Example

cpp main file:
```cpp
int main(int argc, char* argv[])
{
    // input: --filename test.jpg --ratio 15.6

    // init and parse input args
    cmdp::CmdParser parser(argc, argv);

    // read required parameters (availabe in args)
    std::string filename = parser.get<std::string>("--filename");       // test.jpg

    // read optional parameters with default values (available in args)
    float ratio = parser.get<float>("--ratio", 100.56F);                // 15.6

    // read optional parameters with default values (missing in args)
    int enable = parser.get<int>("--enable", 10);                       // 10

    // read required parameters (missing in args)
    int value = parser.get<int>("--value");                             // 0 and set error flag

    // check if any required paramters are missing.
    if (parser.printStatus()) {                                          // print all params with status
        std::cout << "Invalid Input args" << std::endl;
        return 0;
    }
    //...

    return 0;
}
```

Output Result `parser.printStatus()`:
```
[Type  ]   Tag           Value           Status
[string] --filename      test.jpg
[float ] --ratio         15.6
[int   ] --enable        10              Default
[int   ] --value         0               Missing

Invalid Input args
```

## Linking

This is a header only library. Add the repository root to your include path, or
use CMake:

```cmake
add_subdirectory(cmdp)
target_link_libraries(my_app PRIVATE cmdp::cmdp)
```

## Building the tests

```sh
./test/build.bash          # configure, build and run the tests
```

or directly with CMake:

```sh
cmake -S . -B build
cmake --build build
cd build && ctest --output-on-failure
```

## Requirements

The only build requirement is a C++ compiler that supports C++11.

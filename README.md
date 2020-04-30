# cmdp
`Simple C++ command line arguments parser` 

This is a simple C++ command line arguments parser library

Options can be given as tag value pair

    --tag value

## Uses

```cpp
#include "cmdp.h"
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
    float expRatio = parser.get<float>("--ratio", 100.56F);             // 15.6

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

This is a header only library.

## Requirements

The only build requirement is a C++ compiler that supports C++11 features such as:

* typeid
* default constructors

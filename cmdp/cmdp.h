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
#ifndef CMDP_CMDP_H_
#define CMDP_CMDP_H_
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <vector>

//---------------------------------------------------------------------------
// Namespace
//---------------------------------------------------------------------------
namespace cmdp
{
//---------------------------------------------------------------------------
// Type names
//---------------------------------------------------------------------------
//! \breif  returns type name as string.
//!
//! \note   the primary template falls back to the implementation defined
//!         typeid name, which is mangled on gcc/clang. specialize for any
//!         additional type that should print nicely.
template<typename T>
std::string getTypeName() {
    return typeid(T).name();
}

//! \breif  readable names for the builtin types.
//!
//! \note   explicit specializations of a function template are not implicitly
//!         inline, so every one of them must say so or the header breaks the
//!         one definition rule when included in more than one source file.
#define CMDP_DECLARE_TYPE_NAME(type, name)  \
    template<> inline std::string getTypeName<type>() { return name; }

CMDP_DECLARE_TYPE_NAME(bool,               "bool")
CMDP_DECLARE_TYPE_NAME(char,               "char")
CMDP_DECLARE_TYPE_NAME(signed char,        "schar")
CMDP_DECLARE_TYPE_NAME(unsigned char,      "uchar")
CMDP_DECLARE_TYPE_NAME(short,              "short")
CMDP_DECLARE_TYPE_NAME(unsigned short,     "ushort")
CMDP_DECLARE_TYPE_NAME(int,                "int")
CMDP_DECLARE_TYPE_NAME(unsigned int,       "uint")
CMDP_DECLARE_TYPE_NAME(long,               "long")
CMDP_DECLARE_TYPE_NAME(unsigned long,      "ulong")
CMDP_DECLARE_TYPE_NAME(long long,          "llong")
CMDP_DECLARE_TYPE_NAME(unsigned long long, "ullong")
CMDP_DECLARE_TYPE_NAME(float,              "float")
CMDP_DECLARE_TYPE_NAME(double,             "double")
CMDP_DECLARE_TYPE_NAME(long double,        "ldouble")
CMDP_DECLARE_TYPE_NAME(std::string,        "string")

#undef CMDP_DECLARE_TYPE_NAME

//---------------------------------------------------------------------------
// Conversions
//---------------------------------------------------------------------------
//! \breif  any streamable type to string conversion.
//!
//! \return string representation of value
template <typename T>
std::string toString(const T& value) {
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

//! \breif  string to string conversion (no stream round trip).
template <>
inline std::string toString<std::string>(const std::string& value) {
    return value;
}

//! \breif  bool to string conversion (readable instead of 1/0).
template <>
inline std::string toString<bool>(const bool& value) {
    return value ? "true" : "false";
}

//! \breif  string to any streamable type conversion.
//!
//! \note   the whole string must be consumed, so partially numeric input
//!         such as "12abc" is rejected instead of silently yielding 12.
//!
//! \return true on success, out is left untouched on failure
template <typename T>
bool fromString(
    const std::string& str,     //!< [in] : string to convert
    T& out                      //!< [out]: converted value
) {
    // istream extraction of an unsigned type accepts a leading minus sign and
    // wraps it around, so "-1" would quietly become a huge number.
    if (std::is_unsigned<T>::value) {
        const std::size_t pos = str.find_first_not_of(" \t");
        if (pos != std::string::npos && str[pos] == '-') {
            return false;
        }
    }
    std::istringstream ss(str);
    T value;
    ss >> value;
    if (ss.fail()) {
        return false;
    }
    // reject trailing garbage
    ss >> std::ws;
    if (!ss.eof()) {
        return false;
    }
    out = value;
    return true;
}

//! \breif  string to string conversion.
//!
//! \note   the primary template would stop at the first whitespace and
//!         silently truncate values such as --name "John Smith".
template <>
inline bool fromString<std::string>(const std::string& str, std::string& out) {
    out = str;
    return true;
}

//! \breif  string to bool conversion.
//!
//! \note   accepts 1/0, true/false, yes/no and on/off, case insensitively.
//!         an empty value means the tag was given without one, i.e. a flag.
template <>
inline bool fromString<bool>(const std::string& str, bool& out) {
    std::string s;
    for (std::size_t i = 0; i < str.size(); ++i) {
        const unsigned char c = static_cast<unsigned char>(str[i]);
        if (!std::isspace(c)) {
            s += static_cast<char>(std::tolower(c));
        }
    }
    if (s.empty() || s == "1" || s == "true" || s == "yes" || s == "on") {
        out = true;
        return true;
    }
    if (s == "0" || s == "false" || s == "no" || s == "off") {
        out = false;
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
//! \breif  header only command line parser class.
//!
//! \note   arguments are given as "--tag value" or "--tag=value" pairs. a tag
//!         is any token starting with '-' that is not a number, so negative
//!         values such as "--offset -5" are handled, and a tag with no value
//!         is recorded as a valueless flag.
class CmdParser
{
public:
    //---------------------------------------------------------------------------
    //! \breif  default ctor. parse args into the map.
    CmdParser(
        int argc,                   //!< [in]: number of args
        const char* const argv[]    //!< [in]: args
    ) {
        if (argc < 1 || argv == nullptr) {
            return;
        }
        for (int i = 1; i < argc; ++i) {
            if (argv[i] == nullptr) {
                break;
            }
            const std::string arg(argv[i]);
            if (!isTag(arg)) {
                continue;   // stray positional argument, nothing to pair it with
            }
            // --tag=value
            const std::size_t eq = arg.find('=');
            if (eq != std::string::npos) {
                data_[arg.substr(0, eq)] = arg.substr(eq + 1);
                continue;
            }
            // --tag value, unless the next token is a tag of its own
            if (i + 1 < argc && argv[i + 1] != nullptr && !isTag(argv[i + 1])) {
                data_[arg] = argv[i + 1];
                ++i;
            }
            else {
                data_[arg] = "";    // valueless flag
            }
        }
    }

    //---------------------------------------------------------------------------
    //! \breif  get value corresponding to tag id.
    //!
    //! \note   function will set error flag (Missing) if tag not found, or
    //!         (Invalid) if the value cannot be converted to T.
    //!
    //! \return value of type T
    template <typename T>
    T get(
        const std::string& tag      //!< [in]: tag id
    ) {
        return getImpl<T>(tag, T(), true);
    }

    //---------------------------------------------------------------------------
    //! \breif  get value corresponding to tag id.
    //!
    //! \note   default value will be used if tag not found. an unconvertible
    //!         value still sets the error flag (Invalid).
    //!
    //! \return value of type T
    template <typename T>
    T get(
        const std::string& tag,     //!< [in]: tag id
        const T& def                //!< [in]: default value
    ) {
        return getImpl<T>(tag, def, false);
    }

    //---------------------------------------------------------------------------
    //! \breif  check whether a tag was given on the command line.
    //!
    //! \return true if present
    bool has(
        const std::string& tag      //!< [in]: tag id
    ) const {
        return data_.find(tag) != data_.end();
    }

    //---------------------------------------------------------------------------
    //! \breif  number of tags given on the command line.
    //!
    //! \return tag count
    std::size_t size() const {
        return data_.size();
    }

    //---------------------------------------------------------------------------
    //! \breif  print all params, must be called after reading all args.
    //!
    //! \return error state
    bool printStatus(
        std::ostream& os = std::cout    //!< [in]: output stream
    ) const {
        std::size_t tagWidth = 13;      // "  Tag" column, wide enough for the header
        std::size_t valWidth = 15;      // "Value" column
        for (std::size_t i = 0; i < params_.size(); ++i) {
            tagWidth = std::max(tagWidth, params_[i].tag.size());
            valWidth = std::max(valWidth, params_[i].value.size());
        }

        const std::ios_base::fmtflags flags = os.flags();
        os << std::left;
        os << "[" << std::setw(6) << "Type" << "] "
           << std::setw(static_cast<int>(tagWidth) + 2) << "  Tag" << ' '
           << std::setw(static_cast<int>(valWidth)) << "Value" << " Status\n";
        for (std::size_t i = 0; i < params_.size(); ++i) {
            const Param& p = params_[i];
            os << "[" << std::setw(6) << p.type << "] "
               << std::setw(static_cast<int>(tagWidth) + 2) << p.tag << ' '
               << std::setw(static_cast<int>(valWidth)) << p.value << ' '
               << p.status << '\n';
        }
        os.flags(flags);
        return error_;
    }

    //---------------------------------------------------------------------------
    //! \breif  check parser error state
    //!
    //! \return error state
    bool getStatus() const {
        return error_;
    }

private:
    //---------------------------------------------------------------------------
    //! \breif  a single queried parameter, kept for printStatus only.
    struct Param
    {
        std::string type;       //!< type name
        std::string tag;        //!< args tag name
        std::string value;      //!< args value
        std::string status;     //!< empty, Default, Missing or Invalid
    };

    //---------------------------------------------------------------------------
    //! \breif  a token is a tag if it starts with '-' and is not a number.
    //!
    //! \return true if the token is a tag
    static bool isTag(
        const std::string& arg      //!< [in]: command line token
    ) {
        if (arg.size() < 2 || arg[0] != '-') {
            return false;
        }
        // "-5" and "-.5" are values, not tags
        if (std::isdigit(static_cast<unsigned char>(arg[1]))) {
            return false;
        }
        if (arg[1] == '.' && arg.size() > 2 &&
            std::isdigit(static_cast<unsigned char>(arg[2]))) {
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------------------
    //! \breif  internal function, returns value corresponding to tag id.
    //!
    //! \return value of type T
    template <typename T>
    T getImpl(
        const std::string& tag,     //!< [in]: command id tag
        const T& def,               //!< [in]: default value
        bool required               //!< [in]: true, if default value is not given
    ) {
        Param param;
        param.type = getTypeName<T>();
        param.tag = tag;

        auto it = data_.find(tag);
        if (it == data_.end()) {
            param.value = toString(def);
            param.status = required ? "Missing" : "Default";
            error_ = error_ || required;
            params_.push_back(param);
            return def;
        }

        param.value = it->second;
        T value = def;
        if (!fromString(it->second, value)) {
            param.status = "Invalid";
            error_ = true;
            params_.push_back(param);
            return def;
        }
        params_.push_back(param);
        return value;
    }

    bool error_ = false;                                    //!< parser error state
    std::unordered_map<std::string, std::string> data_;     //!< input args map
    std::vector<Param> params_;                             //!< queried params (printing only)
};
//---------------------------------------------------------------------------
} // end namespace
//---------------------------------------------------------------------------

#endif  // CMDP_CMDP_H_

//---------------------------------------------------------------------------
// End of File
//---------------------------------------------------------------------------

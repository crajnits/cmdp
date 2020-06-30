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
#ifndef _cmdp_h_
#define _cmdp_h_
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

//---------------------------------------------------------------------------
// Namespace
//---------------------------------------------------------------------------
namespace cmdp
{
//! returns type name as string
template<typename T>
std::string getTypeName() {
    return typeid(T).name();
}

//! typeid for string class is not readable.
//! all supported type can be specialized.
template<>
std::string getTypeName<std::string>() {
    return "string";
}

//! any type to string conversion
template <typename T>
std::string toString(const T& value) {
    return std::to_string(value);
}

//! string to string conversion (hack)
template <>
std::string toString<std::string>(const std::string& value) {
    return value;
}

//---------------------------------------------------------------------------
//! \breif  header only command line parser class.
class CmdParser
{
public:
    //---------------------------------------------------------------------------
    //! \breif  default ctor. parse args into the map.
    CmdParser(
        int argc,       //!< [in]: number of args
        char* argv[]    //!< [in]: args
    ) {
        error_ = false;
        for (int i = 1; i < argc - 1; i += 2) {
            data_[argv[i]] = argv[i + 1];
        }
    }

    //---------------------------------------------------------------------------
    //! \breif  get value corresponding to tag id.
    //!
    //! \note   function will set error flag (Missing) if tag not found.
    //!
    //! \return value of type T
    template <typename T>
    T get(
        const std::string& tag     //!< [in]: tag id
    ) {
        return getImpl<T>(tag, T(), true);
    }

    //---------------------------------------------------------------------------
    //! \breif  get value corresponding to tag id.
    //!
    //! \note   default value will be used if tag not found.
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
    //! \breif  print all params, must be called after reading all args.
    //!
    //! \return error state
    bool printStatus() const {
        printf("[%-6s] %-15s %-15s %s\n", "Type", "  Tag", "Value", "Status");
        for (int i = 0; i < tags_.size(); i++) {
            printf("[%-6s] %-15s %-15s %s\n", types_[i].c_str(),
                   tags_[i].c_str(), values_[i].c_str(), status_[i].c_str());
        }
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
    //! \breif  internal function, returns value corresponding to tag id.
    //!
    //! \return value of type T
    template <typename T>
    T getImpl(
        const std::string& tag,     //!< [in]: command id tag
        const T& def,               //!< [in]: default value
        bool missing                //!< [in]: true, if default value is not given
    ) {
        types_.push_back(getTypeName<T>());
        tags_.push_back(tag);
        auto it = data_.find(tag);
        if (it == data_.end()) {
            values_.push_back(toString(def));
            if (missing) {
                error_ = true;
                status_.push_back("Missing");
            }
            else {
                status_.push_back("Default");
            }
            return def;
        }
        values_.push_back(it->second);
        // string to type conversion
        std::istringstream ss(it->second);
        T value;
        ss >> value;
        if (ss.fail()) {
            error_ = true;
            status_.push_back("Invalid");
        }
        else {
            status_.push_back("");
        }
        return value;
    }

    bool error_;                                            //!< parser error state
    std::unordered_map<std::string, std::string> data_;     //!< input args map

    // debug value (only used for printing)
    std::vector<std::string> types_;                         //!< type name
    std::vector<std::string> tags_;                          //!< args tag name
    std::vector<std::string> values_;                        //!< args value
    std::vector<std::string> status_;                        //!< input, default, missing
};
//---------------------------------------------------------------------------
} // end namespace
//---------------------------------------------------------------------------

#endif  // _cmdp_h_

//---------------------------------------------------------------------------
// End of File
//---------------------------------------------------------------------------

#include <getopt.h>
#include <sstream>
#include <cstring>
#include <memory>
#include <iostream>
#include <algorithm>

#include "ParserArgs.h"

/*--- implementation of class methods ---*/
/*--- ParserOpt ---*/
ParserOpt::ParserOpt(int argc, char **argv)
        : mArgc(argc)
        , mArgv(argv)
{
    /* empty */
}

void ParserOpt::print_help(FILE *f) const
{
    std::stringstream stream;
    std::string argv(mArgv[0]);
    std::string required, unrequired;

    auto t = argv.rfind('/', argv.size()) + 1;

    stream << "Use: " << argv.substr(t).c_str();
    for(const auto &it : mMapOpt) {
        const auto &el = it.second.get();
        if (el.mRequired)
            required += std::string(1, el.mShortName);
        else
            unrequired += std::string(1, el.mShortName);
    }
    if (!required.empty()) {
        stream << " -" << required;
    }

    if (!unrequired.empty()) {
        stream << "[" << unrequired << "]";
    }
    stream << std::endl;
    stream << mHelp << std::endl;
    for(const auto &it : mMapOpt) {
        stream << it.second.get().getHelpString() << std::endl;
    }
    fprintf(f, "%s\n", stream.str().c_str());
}

bool ParserOpt::parsing() {
    int i = 0, res = 0;
    std::string short_opt;
    std::stringstream stream;

    if (mMapOpt.empty())
        return true;

    auto long_opt = std::unique_ptr<struct option[]>(new struct option[mMapOpt.size() + 1]);
    for(auto it = mMapOpt.begin(); it != mMapOpt.end(); it++, i++) {
        const auto &el = it->second.get();
        long_opt[i].name = el.mLongName.c_str();
        long_opt[i].has_arg = el.mType == typeid(bool).name() ? no_argument : required_argument;
        long_opt[i].flag = nullptr;
        long_opt[i].val = el.mShortName;
        short_opt += it->second.get().mType != typeid(bool).name() ?
                     std::string(1, el.mShortName) + ":" :
                     std::string(1, el.mShortName);
    }
    long_opt[i].name = nullptr;
    long_opt[i].has_arg = 0;
    long_opt[i].flag = nullptr;
    long_opt[i].val = 0;

    opterr = 0;
    while ((res = getopt_long(mArgc, mArgv, short_opt.c_str(), long_opt.get(), nullptr)) != EOF) {
        if (res == '?') {
            stream << mArgv[0] << ": found unknown option — «" << mArgv[optind - 1] << "»";
            mError= stream.str();
            return false;
        }

        auto it = mMapOpt.find(res);
        if (it != mMapOpt.end()) {
            auto &el = it->second.get();
            if (el.mType == typeid(int).name()) {
                el.mResult = optarg;
                el.mValid = !el.mResult.empty() && std::find_if(el.mResult.begin(), el.mResult.end(),
                                                                [](unsigned char c) { return !std::isdigit(c); }) == el.mResult.end();
            } else if (el.mType == typeid(std::string).name()) {
                el.mValid = true;
                el.mResult = optarg;
            } else if (el.mType == typeid(char).name() && strlen(optarg) == 1) {
                el.mValid = true;
                el.mResult = optarg;
            } else if (el.mType == typeid(bool).name()) {
                el.mValid = true;
            }
        }
    }

    for(auto it = mMapOpt.begin(); it != mMapOpt.end(); it++, i++) {
        const auto &el = it->second.get();
        if (!el.mRequired) continue;
        if (el.mValid) continue;
        stream << mArgv[0] << ": missed required option — «-" << el.mShortName << "»";
        mError = stream.str();
        return false;
    }
    return true;
}

std::string ParserOpt::getError() const {
    return mError;
}

/*--- Element ---*/
ParserOpt::Element::Element(const std::string &type, char shortName, const std::string &longName, const std::string &helpString, const std::string &helpValue, bool required)
        : mType(type)
        , mShortName(shortName)
        , mLongName(longName)
        , mHelpValue(helpValue)
        , mHelpString(helpString)
        , mRequired(required)
        , mValid(false)
{
    /* empty */
}

bool ParserOpt::Element::isValid() const
{
    return mValid;
}

std::string ParserOpt::Element::getHelpString() const
{
    std::stringstream stream;
    const int max = 30;
    if (mShortName == 0) stream << std::string(6, ' ');
    else stream << "  -" << mShortName;
    if (mLongName.empty()) stream << std::string(max - stream.str().size(), ' ');
    else stream << ", --" << mLongName;
    if (!mHelpValue.empty()) stream << "=" << mHelpValue;
    auto indent = max - stream.str().size();
    if (indent <= 0 && !mLongName.empty()) stream << "\n" << std::string(max, ' ');
    else stream << std::string(indent, ' ');
    stream << mHelpString;
    return stream.str();
}

ParserOpt::Element::operator int() const
{
    return static_cast<int>(strtol(mResult.c_str(), nullptr, 10));
}

ParserOpt::Element::operator bool() const
{
    return mValid;
}

ParserOpt::Element::operator std::string() const
{
    return mResult;
}

ParserOpt::Element::operator char() const {
    return mResult[0];
}
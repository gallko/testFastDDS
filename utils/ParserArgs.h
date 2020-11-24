#pragma once
#include <string>
#include <map>
#include <type_traits>
#include <functional>

class ParserOpt {
public:
    template <typename T> class ParserElement;

    ParserOpt(int argc, char **argv);
    ~ParserOpt() = default;

    ParserOpt operator=(const ParserOpt &f) = delete;
    ParserOpt(ParserOpt &f) = delete;
    ParserOpt operator=(ParserOpt &&f) = delete;
    ParserOpt(ParserOpt &&f) = delete;

    template <typename T> bool add_option(ParserElement<T> &element);
    bool parsing();
    std::string getError() const;
    void print_help(FILE *f) const;

private:
    class Element;

    using container_t = std::map<char, std::reference_wrapper<ParserOpt::Element>>;
    char **mArgv;
    int mArgc;
    container_t mMapOpt;
    std::string mError;
    std::string mHelp;
};

class ParserOpt::Element {
public:
    bool isValid() const;
    std::string getHelpString() const;
    virtual ~Element() = default;

protected:
    Element(const std::string &type, char shortName, const std::string &longName, const std::string &help, const std::string &helpValue, bool required);
    explicit operator int() const;
    explicit operator bool() const;
    explicit operator std::string() const;
    explicit operator char() const;

private:
    friend ParserOpt;
    const std::string mType;
    const char mShortName;
    const std::string mLongName;
    const std::string mHelpValue;
    const std::string mHelpString;
    std::string mResult;
    const bool mRequired;
    bool mValid;
};

/**
 * @class ParserElement
 * @brief Describe one parameter option
 * @tparam T
 * <int>, <std::string> return value of the key,
 * <bool> return true if parameter be set else false.
 * @example ParserElement('i', "integer", "Set number.", "NUMBER", true);
 */
template <typename T>
class ParserOpt::ParserElement : public ParserOpt::Element {
public:
    /**
     * @param[in] arg - the character-name option
     * @param[in] name - the long-name option
     * @param[in] help - the string for description of the option
     * @param[in] required - whether the option is mandatory
     */
    ParserElement(char arg, const std::string &name, const std::string &help, const std::string &helpValue, bool required = false);
    ~ParserElement() override = default;
    T getValue() const;
};


/*--- implementation of class methods ---*/

template<typename T>
bool ParserOpt::add_option(ParserElement<T> &element)
{
    return mMapOpt.insert(container_t::value_type { element.mShortName, std::ref(element) }).second;
}

template <typename T>
ParserOpt::ParserElement<T>::ParserElement(char arg, const std::string &name, const std::string &help, const std::string &helpValue, bool required)
        : Element(typeid(T).name(), arg, name, help, helpValue, required)
{
    /* empty */
}

template<typename T>
T ParserOpt::ParserElement<T>::getValue() const {
    return static_cast<T>(*this);
}
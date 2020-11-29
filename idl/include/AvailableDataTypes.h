#pragma once
#include <map>
#include <string>

namespace eprosima::fastdds::dds {
    class TypeSupport;
    class DomainParticipant;
}

class AvailableDataTypes final {
public:
    static AvailableDataTypes &instance();
    ~AvailableDataTypes() = default;
    bool isType(const std::string &typeName) const;
    eprosima::fastdds::dds::TypeSupport &getType(const std::string &typeName) const;

    void registerType(eprosima::fastdds::dds::DomainParticipant *domainParticipant, const std::string &typeName);
    void registerAllType(eprosima::fastdds::dds::DomainParticipant *domainParticipant);

private:
    AvailableDataTypes();
    const std::map<std::string, eprosima::fastdds::dds::TypeSupport> mContainer;
};


#include <fastdds/dds/topic/TypeSupport.hpp>
#include "include/AvailableDataTypes.h"

#include <Message_1PubSubTypes.h>
#include <Message_2PubSubTypes.h>
#include <ReportPubSubTypes.h>

using namespace eprosima::fastdds::dds;

AvailableDataTypes &AvailableDataTypes::instance() {
    static AvailableDataTypes types;
    return types;
}

AvailableDataTypes::AvailableDataTypes()
    : mContainer({
                         {"msg1", TypeSupport(new Message_1PubSubType())},
                         {"msg2", TypeSupport(new Message_2PubSubType())},
                         {"report", TypeSupport(new ReportPubSubType())}
                 })
{}

bool AvailableDataTypes::isType(const std::string &string) const {
    return mContainer.find(string) != mContainer.cend();
}

TypeSupport &AvailableDataTypes::getType(const std::string &string) const {
    return const_cast<TypeSupport &>(mContainer.find(string)->second);
}

void AvailableDataTypes::registerType(eprosima::fastdds::dds::DomainParticipant *domainParticipant, const std::string &typeName) {
    getType(typeName).register_type(domainParticipant, typeName);
}

void AvailableDataTypes::registerAllType(eprosima::fastdds::dds::DomainParticipant *domainParticipant) {
    for (auto &it: mContainer) {
        const_cast<TypeSupport &>(it.second).register_type(domainParticipant, it.first);
    }
}


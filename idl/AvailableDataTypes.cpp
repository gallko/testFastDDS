#include <fastdds/dds/topic/TypeSupport.hpp>
#include "include/AvailableDataTypes.h"

#include <Message_1PubSubTypes.h>
#include <Message_2PubSubTypes.h>
#include <ReportsPubSubTypes.h>

using namespace eprosima::fastdds::dds;

AvailableDataTypes &AvailableDataTypes::instance() {
    static AvailableDataTypes types;
    return types;
}

AvailableDataTypes::AvailableDataTypes()
    : mContainer({
                         {"msg1", TypeSupport(new Message_1PubSubType())},
                         {"msg2", TypeSupport(new Message_2PubSubType())},
                         {"report", TypeSupport(new ReportsPubSubType())}
                 })
{}

bool AvailableDataTypes::isType(const std::string &typeName) const {
    return mContainer.find(typeName) != mContainer.cend();
}

TypeSupport &AvailableDataTypes::getType(const std::string &typeName) const {
    return const_cast<TypeSupport &>(mContainer.find(typeName)->second);
}

void AvailableDataTypes::registerType(eprosima::fastdds::dds::DomainParticipant *domainParticipant, const std::string &typeName) {
    getType(typeName).register_type(domainParticipant, typeName);
}

void AvailableDataTypes::registerAllType(eprosima::fastdds::dds::DomainParticipant *domainParticipant) {
    for (auto &it: mContainer) {
        const_cast<TypeSupport &>(it.second).register_type(domainParticipant, it.first);
    }
}


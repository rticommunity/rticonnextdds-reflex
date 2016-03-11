#ifndef REFLEX_DATAREADER_PARAMS_H
#define REFLEX_DATAREADER_PARAMS_H

#include "ndds/ndds_cpp.h"
#include "reflex/dllexport.h"

#include <string>

#ifdef RTI_WIN32
#pragma warning(disable: 4251)
// std::string topic_name_ and type_name_ are private 
// so it's ok to disable this nasty warning.
#endif

namespace reflex { 
  namespace sub {

/**
 * @brief A valuetype to contain multiple DataReader parameters.
 */
class REFLEX_DLL_EXPORT DataReaderParams
{
public:
    explicit DataReaderParams (DDSDomainParticipant *);

    DataReaderParams & listener(DDSDataReaderListener *listener);
    DataReaderParams & subscriber(DDSSubscriber * subscriber);
    DataReaderParams & datareader_qos(const DDS_DataReaderQos & qos);
    DataReaderParams & topic_name (const std::string &name);
    DataReaderParams & type_name (const std::string &name);
    DataReaderParams & topic (DDSTopicDescription * topic);
    DataReaderParams & listener_statusmask(DDS_StatusMask mask);
    DataReaderParams & dynamicdata_type_property(DDS_DynamicDataTypeProperty_t);

    DDSDomainParticipant* domain_participant() const;
    DDSSubscriber* subscriber() const;
    const DDS_DataReaderQos & datareader_qos() const;
    DDSDataReaderListener * listener() const;
    DDSTopicDescription * topic() const;
    std::string topic_name() const;
    std::string type_name() const;
    DDS_StatusMask listener_statusmask() const;
    DDS_DynamicDataTypeProperty_t dynamicdata_type_property() const;

private:
    DDSDomainParticipant * participant_;
    DDSSubscriber * subscriber_;
    DDSTopicDescription * topic_;
    DDSDataReaderListener * listener_;
    DDS_DataReaderQos qos_;
    std::string topic_name_;
    std::string type_name_;
    DDS_StatusMask mask_;
    DDS_DynamicDataTypeProperty_t property_;
};

} // namespace sub
} // namespace reflex


#ifndef REFLEX_NO_HEADER_ONLY
#include "reflex/../../src/datareader_params.cxx"
#endif

#endif // REFLEX_DATAREADER_PARAMS_H


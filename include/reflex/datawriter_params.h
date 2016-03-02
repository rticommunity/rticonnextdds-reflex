#ifndef REFLEX_DATAWRITER_PARAMS_H
#define REFLEX_DATAWRITER_PARAMS_H

#include "ndds/ndds_cpp.h"
#include "reflex/dllexport.h"

#include <string>

#ifdef RTI_WIN32
#pragma warning(disable: 4251)
// std::string topic_name_ and type_name_ are private 
// so it's ok to disable this nasty warning.
#endif

namespace reflex { 
  namespace pub {

template <class T>
class DataWriterListener;

/**
 * @brief A valuetype to contain multiple DataWriter parameters.
 */
class REFLEX_DLL_EXPORT DataWriterParams
{
public:
    explicit DataWriterParams (DDSDomainParticipant *);

    DataWriterParams & listener(DDSDataWriterListener *listener);
    DataWriterParams & publisher(DDSPublisher * publisher);
    DataWriterParams & datawriter_qos(const DDS_DataWriterQos & qos);
    DataWriterParams & topic_name (const std::string &name);
    DataWriterParams & type_name (const std::string &name);
    DataWriterParams & topic (DDSTopic * topic);
    DataWriterParams & listener_statusmask(DDS_StatusMask mask);
    DataWriterParams & dynamicdata_type_property(DDS_DynamicDataTypeProperty_t);

    DDSDomainParticipant* domain_participant() const;
    DDSPublisher* publisher() const;
    const DDS_DataWriterQos & datawriter_qos() const;
    DDSDataWriterListener * listener() const;
    DDSTopic * topic() const;
    std::string topic_name() const;
    std::string type_name() const;
    DDS_StatusMask listener_statusmask() const;
    DDS_DynamicDataTypeProperty_t dynamicdata_type_property() const;

private:
    DDSDomainParticipant * participant_;
    DDSPublisher * publisher_;
    DDSTopic * topic_;
    DDSDataWriterListener * listener_;
    DDS_DataWriterQos qos_;
    std::string topic_name_;
    std::string type_name_;
    DDS_StatusMask mask_;
    DDS_DynamicDataTypeProperty_t property_;
};

} // namespace pub
} // namespace reflex

#ifndef REFLEX_NO_HEADER_ONLY
#include "reflex/../../src/datawriter_params.cxx"
#endif

#endif // REFLEX_DATAWRITER_PARAMS_H


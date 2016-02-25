#include "reflex/datareader_params.h"

namespace reflex {
  namespace sub {

    REFLEX_INLINE
    DataReaderParams::DataReaderParams(DDSDomainParticipant * participant)
      : participant_(participant),
        subscriber_(0),
        topic_(0),
        listener_(0),
        qos_(DDS_DATAREADER_QOS_DEFAULT),
        mask_(DDS_DATA_AVAILABLE_STATUS),
        property_(DDS_DYNAMIC_DATA_TYPE_PROPERTY_DEFAULT)
    {}

    REFLEX_INLINE
    DataReaderParams & DataReaderParams::listener(DataReaderListenerBase *listener)
    {
      listener_ = listener;
      return *this;
    }

    REFLEX_INLINE
    DataReaderParams & DataReaderParams::subscriber(DDSSubscriber * subscriber)
    {
      subscriber_ = subscriber;
      return *this;
    }

    REFLEX_INLINE
    DataReaderParams & DataReaderParams::datareader_qos(const DDS_DataReaderQos & qos)
    {
      qos_ = qos;
      return *this;
    }

    REFLEX_INLINE
    DataReaderParams & DataReaderParams::topic_name (const std::string &name)
    {
      topic_name_ = name;
      return *this;
    }

    REFLEX_INLINE
    DataReaderParams & DataReaderParams::type_name (const std::string &name)
    {
      type_name_ = name;
      return *this;
    }

    REFLEX_INLINE
    DataReaderParams & DataReaderParams::topic (DDSTopic * topic)
    {
      topic_ = topic;
      return *this;
    }
    
    REFLEX_INLINE
    DataReaderParams & DataReaderParams::listener_statusmask(DDS_StatusMask mask)
    {
      mask_ = mask;
      return *this;
    }

    REFLEX_INLINE
    DataReaderParams & DataReaderParams::dynamicdata_type_property(DDS_DynamicDataTypeProperty_t property)
    {
      property_ = property;
      return *this;
    }

    REFLEX_INLINE
    DDSDomainParticipant* DataReaderParams::domain_participant() const
    {
      return participant_;
    }

    REFLEX_INLINE
    DDSSubscriber* DataReaderParams::subscriber() const
    {
      return subscriber_;
    }

    REFLEX_INLINE
    const DDS_DataReaderQos & DataReaderParams::datareader_qos() const
    {
      return qos_;
    }

    REFLEX_INLINE
    DataReaderListenerBase * DataReaderParams::listener() const
    {
      return listener_;
    }

    REFLEX_INLINE
    DDSTopic * DataReaderParams::topic() const
    {
      return topic_;
    }

    REFLEX_INLINE
    std::string DataReaderParams::topic_name() const
    {
      return topic_name_;
    }

    REFLEX_INLINE
    std::string DataReaderParams::type_name() const
    {
      return type_name_;
    }

    REFLEX_INLINE
    DDS_StatusMask DataReaderParams::listener_statusmask() const
    {
      return mask_;
    }

    REFLEX_INLINE
    DDS_DynamicDataTypeProperty_t DataReaderParams::dynamicdata_type_property() const
    {
      return property_;
    }

} // sub
} // reflex


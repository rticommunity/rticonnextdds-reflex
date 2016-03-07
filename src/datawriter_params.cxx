#include "reflex/datawriter_params.h"

namespace reflex {
  namespace pub {

    REFLEX_INLINE
    DataWriterParams::DataWriterParams(DDSDomainParticipant * participant)
      : participant_(participant),
        publisher_(0),
        topic_(0),
        listener_(0),
        mask_(DDS_STATUS_MASK_NONE),
        property_(DDS_DYNAMIC_DATA_TYPE_PROPERTY_DEFAULT)
    {
      participant_->get_default_datawriter_qos(qos_);
    }

    REFLEX_INLINE
    DataWriterParams & DataWriterParams::listener(DDSDataWriterListener *listener)
    {
      listener_ = listener;
      return *this;
    }

    REFLEX_INLINE
    DataWriterParams & DataWriterParams::publisher(DDSPublisher * publisher)
    {
      publisher_ = publisher;
      return *this;
    }

    REFLEX_INLINE
    DataWriterParams & DataWriterParams::datawriter_qos(const DDS_DataWriterQos & qos)
    {
      qos_ = qos;
      return *this;
    }

    REFLEX_INLINE
    DataWriterParams & DataWriterParams::topic_name (const std::string &name)
    {
      topic_name_ = name;
      return *this;
    }

    REFLEX_INLINE
    DataWriterParams & DataWriterParams::type_name (const std::string &name)
    {
      type_name_ = name;
      return *this;
    }

    REFLEX_INLINE
    DataWriterParams & DataWriterParams::topic (DDSTopic * topic)
    {
      topic_ = topic;
      return *this;
    }
    
    REFLEX_INLINE
    DataWriterParams & DataWriterParams::listener_statusmask(DDS_StatusMask mask)
    {
      mask_ = mask;
      return *this;
    }

    REFLEX_INLINE
    DataWriterParams & DataWriterParams::dynamicdata_type_property(DDS_DynamicDataTypeProperty_t property)
    {
      property_ = property;
      return *this;
    }

    REFLEX_INLINE
    DDSDomainParticipant* DataWriterParams::domain_participant() const
    {
      return participant_;
    }

    REFLEX_INLINE
    DDSPublisher* DataWriterParams::publisher() const
    {
      return publisher_;
    }

    REFLEX_INLINE
    const DDS_DataWriterQos & DataWriterParams::datawriter_qos() const
    {
      return qos_;
    }

    REFLEX_INLINE
    DDSDataWriterListener * DataWriterParams::listener() const
    {
      return listener_;
    }

    REFLEX_INLINE
    DDSTopic * DataWriterParams::topic() const
    {
      return topic_;
    }

    REFLEX_INLINE
    std::string DataWriterParams::topic_name() const
    {
      return topic_name_;
    }

    REFLEX_INLINE
    std::string DataWriterParams::type_name() const
    {
      return type_name_;
    }

    REFLEX_INLINE
    DDS_StatusMask DataWriterParams::listener_statusmask() const
    {
      return mask_;
    }

    REFLEX_INLINE
    DDS_DynamicDataTypeProperty_t DataWriterParams::dynamicdata_type_property() const
    {
      return property_;
    }

} // pub
} // reflex


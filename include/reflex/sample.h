/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_SAMPLE_H
#define RTIREFLEX_SAMPLE_H

struct DDS_SampleInfo;

namespace reflex {

  namespace sub {

    /**
     * @brief A valuetype that combines an instance of type T (data) and DDS_SampleInfo
     */
    template <class T>
    class Sample
    {
      T val;
      DDS_SampleInfo val_info;

    public:

      Sample()
      { }

      Sample(T &t, DDS_SampleInfo &i)
        : val(t),
          val_info(i)
      { }

      T & data()
      {
        return val;
      }

      const T & data() const
      {
        return val;
      }

      const DDS_SampleInfo & info() const
      {
        return val_info;
      }

      DDS_SampleInfo & info()
      {
        return val_info;
      }

      T * operator -> ()
      {
        return &val;
      }

      const T * operator -> () const
      {
        return &val;
      }
    };

  } // namespace sub
} // namespace reflex


#endif // RTIREFLEX_SAMPLE_H

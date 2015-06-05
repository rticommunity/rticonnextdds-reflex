/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.    	                             
RTI grants Licensee a license to use, modify, compile, and create derivative works 
of the Software.  Licensee has the right to distribute object form only for use with RTI 
products.  The Software is provided "as is", with no warranty of any type, including 
any warranty for fitness for any purpose. RTI is under no obligation to maintain or 
support the Software.  RTI shall not be liable for any incidental or consequential 
damages arising out of the use or inability to use the software.
**********************************************************************************************/
/*
 * Based on: 
 * https://cdcvs.fnal.gov/redmine/projects/darkart/repository/revisions/development/show/darkart/Products
 * https://cdcvs.fnal.gov/redmine/projects/art/repository/revisions/develop/entry/art/Persistency/Common/Assns.h
 * https://cdcvs.fnal.gov/redmine/projects/art/repository/revisions/develop/entry/art/Persistency/Provenance/ProductID.h
 */

#ifndef RTIREFLEX_DARKART_H
#define RTIREFLEX_DARKART_H

#ifdef RTI_WIN32
#include <stdint.h>
#endif

#include <vector>
#include <list>
#include <set>
#include <string>
#include <tuple>
#include <stdint.h>

#include "reflex.h"

struct non_copyable_non_movable
{
  non_copyable_non_movable() {}

private:
  non_copyable_non_movable(const non_copyable_non_movable &);
  non_copyable_non_movable & operator = (const non_copyable_non_movable &);

  non_copyable_non_movable(non_copyable_non_movable &&);
  non_copyable_non_movable & operator = (non_copyable_non_movable &&);
};

namespace darkart
{
  // Holds useful information about the baseline for a single
  // channel as found by the baseline finder
  struct Baseline : non_copyable_non_movable
  {
    // Default constructor
    Baseline() { Clear(); } 
    // Reset all member data to default values
    void Clear() {}
    // Useful info about the baseline
    bool   found_baseline; //Was the baseline finder successful?
    double           mean; //the average baseline found
    double       variance; //variance of the samples in the baseline range
  };
  typedef std::vector<Baseline> BaselineData;

  // Holds identifier info for a single channel
  struct Channel : non_copyable_non_movable
  {
    static const int INVALID_CHANNEL_ID = -1;
    static const int SUMCH_ID = -2;
    // Default constructor
    Channel() { Clear(); }
    // Reset all member data to default values
    void Clear() {
      board_id = channel_num = channel_id = trigger_count = sample_bits = trigger_index = nsamps = 0;
      sample_rate = 0;  
    }

    // All the identifier info we want to store for each channel
    int           board_id; //id of the board the channel is on
    //int          board_num; //number board is on in this event
    int        channel_num; //number of the channel on the board
    int         channel_id; //global unique channel id
    std::string      label; //helpful label for each channel
    int      trigger_count; //used to check for ID_MISMATCH
    //uint32_t     timestamp; //trigger time for this channel (us since run start)
    int        sample_bits; //digitizer resolution
    double     sample_rate; //samples per microsecond
    int      trigger_index; //sample at which trigger occurred
    int             nsamps; //number of samples in the waveform
    bool         saturated; //did the signal hit the max or min range of the digitizer?
    uint64_t GetVerticalRange() const { return ((uint64_t) 1 << sample_bits) - 1; }
  };
  typedef std::vector<Channel> ChannelInfo;

  struct EventInfo : non_copyable_non_movable
  {
    enum STATUS_FLAGS { NORMAL=0, ID_MISMATCH=1, BAD_TIMESTAMP=2 /*enter more here*/};

    // Default constructor
    EventInfo() { Clear(); }
    // Reset all member data to default values
    void Clear() { status = NORMAL; }
    // All the identifier info we want to store for each event
    int              run_id; //id of the run this event is from
    int            event_id; //id of this event
    STATUS_FLAGS     status; //flag denoting event status
    //int       trigger_count; //number of triggers (including unaccepted triggers)
    uint64_t      timestamp; //unix timestamp for this event
    uint64_t             dt; //time since the last event in microseconds
    uint64_t     event_time; //time since run start in microseconds
    int              nchans; //physical channels that lit up
    bool          saturated; //true if any channel hit the limit of its digitizer range

  };

struct Pmt // : non_copyable_non_movable
  {
    // Default constructor
    Pmt() { Clear(); }
    Pmt(std::string s)
      : serial_id(std::move(s)) 
    {}

    // Reset all member data to default values
    void Clear() {}
    // All the relevant info we want to store for each PMT
    std::string     serial_id; //serial number
    int            channel_id; //channelID associated with this PMT
    double           spe_mean; //mean photoelectron response read from database
    double          spe_sigma; //sigma of photoelectron response read from database
    double     photocathode_x; //x position of the center of the photocathode
    double     photocathode_y; //y position of the center of the photocathode
    double     photocathode_z; //z position of the center of the photocathode
    double     photocathode_r; //r (cylindrical coord) position of the center of the photocathode
    double photocathode_theta; //theta (cylindrical coord) position of the center of the photocathode
    double  photocathode_area; //area [cm2] of the photocathode
    double                 qe; //quantum efficiency
    bool operator < (Pmt const & p) const
    {
      return serial_id < p.serial_id;
    }

  };

  // We store one Pmt object for each channel
  typedef std::set<Pmt> PmtInfo;

  struct VetoTDCHit : non_copyable_non_movable
  {
    VetoTDCHit(int p = 0, 
               int h = 0, 
               float l = 0, 
               float t = 0, 
               float theta = 0, 
               float phi = 0)
      : pmt_index(p),
      hit_index(h),
      leading_ns(l),
      trailing_ns(t),
      pmt_theta(theta),
      pmt_phi(phi) {}

    int   pmt_index;
    int   hit_index;
    float leading_ns;
    float trailing_ns;
    float pmt_theta;
    float pmt_phi;
  };
  typedef std::list<VetoTDCHit> VetoTDCHits;
  struct VetoTruth : non_copyable_non_movable
  {
    int         sim_event;
    VetoTDCHits hits;
  };

  class Waveform : non_copyable_non_movable
  {
  public:
    // Default constructor
    Waveform() { Clear(); }
    Waveform(std::size_t length);
    void Clear() {}
    // Set all elements in the wave to zero
    void zero();
    std::vector<double> wave;
    int trigger_index;
    double sample_rate; //in MHz
    double SampleToTime(int sample) const;
    int    TimeToSample(double time, bool checkrange=false) const;
  };
  typedef std::vector<Waveform> Waveforms;

  class TopLevel
  {
     BaselineData & baseline_data_;
     ChannelInfo  & channel_info_;
     EventInfo    & event_info_;
     PmtInfo      & pmt_info_;
     VetoTruth    & veto_truth_;
     Waveforms    & waveforms_;

  public:
     TopLevel(BaselineData & b,
              ChannelInfo  & c,
              EventInfo    & e,
              PmtInfo      & p,
              VetoTruth    & v,
              Waveforms    & w)
      : baseline_data_(b),
        channel_info_(c),
        event_info_(e),
        pmt_info_(p),
        veto_truth_(v),
        waveforms_(w)
     {}         
     
     const BaselineData & baseline_data() const { return baseline_data_; }
     const ChannelInfo  & channel_info()  const { return channel_info_;  }
     const EventInfo    & event_info()    const { return event_info_;    }
     const PmtInfo      & pmt_info()      const { return pmt_info_;      }
     const VetoTruth    & veto_truth()    const { return veto_truth_;    }
     const Waveforms    & waveforms()     const { return waveforms_;     }
     
     BaselineData & baseline_data() { return baseline_data_; }
     ChannelInfo  & channel_info()  { return channel_info_;  }
     EventInfo    & event_info()    { return event_info_;    }
     PmtInfo      & pmt_info()      { return pmt_info_;      }
     VetoTruth    & veto_truth()    { return veto_truth_;    }
     Waveforms    & waveforms()     { return waveforms_;     }
  };

  typedef std::tuple<BaselineData,
                     ChannelInfo,
                     EventInfo,
                     PmtInfo,
                     VetoTruth,
                     Waveforms> AllTuple;

  typedef std::vector<Waveform> Waveforms;
  typedef std::vector<Waveform> WaveformInfo;

  typedef std::vector<uint16_t> Fragment;
  typedef std::vector<Fragment> Fragments;

  typedef unsigned short ProcessIndex;
  typedef unsigned short ProductIndex;

  struct ProductID
  {
    ProcessIndex processIndex_;
    ProductIndex productIndex_;
  };

  struct RefCore
  {
    ProductID id_;
    // more transient state
  };

  template <class A, class B>
  struct Assns
  {
    typedef std::vector<std::pair<RefCore, size_t> > ptr_data_t;

    ptr_data_t ptr_data_1_;
    ptr_data_t ptr_data_2_;
    // more transient state
  };

  typedef Assns<ChannelInfo, BaselineData> BaselineDataAssns;
  typedef Assns<ChannelInfo, Waveform>     WaveformAssns;
  typedef Assns<ChannelInfo, WaveformInfo> WaveformInfoAssns;

  struct ProductKey
  {
    unsigned int event_id;
    std::string producer_label;
    std::string instance_name;
    std::string proc_step;

    ProductKey(unsigned int id,
      std::string label,
      std::string name,
      std::string step)
      : event_id(id),
      producer_label(std::move(label)),
      instance_name(std::move(name)),
      proc_step(std::move(step))
    {}
  };

  struct BaselineDataProduct : ProductKey
  {
    BaselineData & baseline_data;

    BaselineDataProduct(BaselineData & b,
      uint32_t id,
      std::string label,
      std::string name,
      std::string step)
      : ProductKey(id,
      std::move(label),
      std::move(name),
      std::move(step)),
      baseline_data(b)
    {}
  };

  struct WaveformsProduct : ProductKey
  {
    Waveforms & waveforms;

    WaveformsProduct(Waveforms & w,
      uint32_t id,
      std::string label,
      std::string name,
      std::string step)
      : ProductKey(id,
      std::move(label),
      std::move(name),
      std::move(step)),
      waveforms(w)
    {}
  };

  struct BaselineDataAssnsProduct : ProductKey
  {
    BaselineDataAssns & assns;

    BaselineDataAssnsProduct(BaselineDataAssns & a,
      unsigned int id,
      std::string label,
      std::string name,
      std::string step)
      : ProductKey(id,
      std::move(label),
      std::move(name),
      std::move(step)),
      assns(a)
    {}
  };

  struct ChannelInfoProduct : ProductKey
  {
    ChannelInfo & cinfo;

    ChannelInfoProduct(ChannelInfo & c,
      unsigned int id,
      std::string label,
      std::string name,
      std::string step)
      : ProductKey(id,
                   std::move(label),
                   std::move(name),
                   std::move(step)),
         cinfo(c)
    {}
  };

  struct EventInfoProduct : ProductKey
  {
    EventInfo & einfo;

    EventInfoProduct(EventInfo & e,
      unsigned int id,
      std::string label,
      std::string name,
      std::string step)
      : ProductKey(id,
                   std::move(label),
                   std::move(name),
                   std::move(step)),
         einfo(e)
    {}
  };


} // namespace darkart

RTI_ADAPT_ENUM(
  darkart::EventInfo::STATUS_FLAGS,
  (NORMAL,        0)
  (ID_MISMATCH,   1)
  (BAD_TIMESTAMP, 2))

RTI_ADAPT_STRUCT(
    darkart::Baseline,
    (bool,   found_baseline) 
    (double,           mean)
    (double,       variance))

RTI_ADAPT_STRUCT(
    darkart::Channel,
    (int,              board_id, RTI_KEY)
    (int,           channel_num)
    (int,            channel_id)
    (std::string,         label)
    (int,         trigger_count)
    (int,           sample_bits)
    (double,        sample_rate)
    (int,         trigger_index)
    (int,                nsamps)
    (bool,            saturated))

RTI_ADAPT_STRUCT(
  darkart::EventInfo,
  (int,                                 run_id)
  (int,                               event_id)
  (darkart::EventInfo::STATUS_FLAGS,    status)
  (uint64_t,                         timestamp)
  (uint64_t,                                dt)
  (uint64_t,                        event_time)
  (int,                                 nchans)
  (bool,                             saturated))

RTI_ADAPT_STRUCT(
  darkart::Pmt,
  (std::string,     serial_id)
  (int,            channel_id)
  (double,           spe_mean)
  (double,          spe_sigma)
  (double,     photocathode_x)
  (double,     photocathode_y)
  (double,     photocathode_z)
  (double,     photocathode_r)
  (double, photocathode_theta)
  (double,  photocathode_area)
  (double,                 qe))

RTI_ADAPT_STRUCT(
  darkart::VetoTDCHit,
  (int,      pmt_index)
  (int,      hit_index)
  (float,   leading_ns)
  (float,  trailing_ns)
  (float,    pmt_theta)
  (float,      pmt_phi))

RTI_ADAPT_STRUCT(
  darkart::VetoTruth,
  (int,                  sim_event)
  (darkart::VetoTDCHits,      hits))

RTI_ADAPT_STRUCT(
  darkart::Waveform,
  (std::vector<double>,          wave)
  (int,                 trigger_index)
  (double,                sample_rate))

RTI_ADAPT_STRUCT(
  darkart::TopLevel,  
  (darkart::BaselineData, baseline_data())
  (darkart::ChannelInfo ,  channel_info())
  (darkart::EventInfo   ,    event_info(), RTI_KEY)
  (darkart::PmtInfo     ,      pmt_info())
  (darkart::VetoTruth   ,    veto_truth())
  (darkart::Waveforms   ,     waveforms()))

RTI_ADAPT_STRUCT(
  darkart::BaselineDataProduct,
  (darkart::BaselineData &, baseline_data)
  (unsigned int, event_id, RTI_KEY)
  (std::string, producer_label, RTI_KEY)
  (std::string, instance_name, RTI_KEY)
  (std::string, proc_step, RTI_KEY))

RTI_ADAPT_STRUCT(
  darkart::WaveformsProduct,
  (darkart::Waveforms &, waveforms)
  (unsigned int, event_id, RTI_KEY)
  (std::string, producer_label, RTI_KEY)
  (std::string, instance_name, RTI_KEY)
  (std::string, proc_step, RTI_KEY))

RTI_ADAPT_STRUCT(
  darkart::ProductID,
  (darkart::ProcessIndex,  processIndex_)
  (darkart::ProductIndex,  productIndex_))

RTI_ADAPT_STRUCT(
  darkart::RefCore,
  (darkart::ProductID, id_))

RTI_ADAPT_STRUCT(
  darkart::BaselineDataAssns,
  (darkart::BaselineDataAssns::ptr_data_t, ptr_data_1_)
  (darkart::BaselineDataAssns::ptr_data_t, ptr_data_2_))

RTI_ADAPT_STRUCT(
  darkart::BaselineDataAssnsProduct,
  (darkart::BaselineDataAssns &, assns)
  (unsigned int, event_id, RTI_KEY)
  (std::string, producer_label, RTI_KEY)
  (std::string, instance_name, RTI_KEY)
  (std::string, proc_step, RTI_KEY))

RTI_ADAPT_STRUCT(
  darkart::ChannelInfoProduct,
  (darkart::ChannelInfo &, cinfo)
  (unsigned int, event_id, RTI_KEY)
  (std::string, producer_label, RTI_KEY)
  (std::string, instance_name, RTI_KEY)
  (std::string, proc_step, RTI_KEY))

RTI_ADAPT_STRUCT(
  darkart::EventInfoProduct,
  (darkart::EventInfo &, einfo)
  (unsigned int, event_id, RTI_KEY)
  (std::string, producer_label, RTI_KEY)
  (std::string, instance_name, RTI_KEY)
  (std::string, proc_step, RTI_KEY))


#endif // RTIREFLEX_DARKART_H


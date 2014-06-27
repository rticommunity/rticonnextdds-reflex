/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.    	                             
RTI grants Licensee a license to use, modify, compile, and create derivative works 
of the Software.  Licensee has the right to distribute object form only for use with RTI 
products.  The Software is provided “as is”, with no warranty of any type, including 
any warranty for fitness for any purpose. RTI is under no obligation to maintain or 
support the Software.  RTI shall not be liable for any incidental or consequential 
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#include "large_type.h"
#include <memory>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <array>
#include <tuple>

#define SIZE 4

void delete_entities(DDSDomainParticipant * participant);

std::tuple<DDSDynamicDataWriter *,
           DDSDomainParticipant *> 
create_ddwriter(const char *type_name, 
                const char *topic_name,
                DDSDynamicDataTypeSupport *type_support);

void write_large_type(int domain_id) 
{
    using reflex::MultiDimArray;
    using reflex::octet_t;
    using reflex::bounded;
    using reflex::BoundedRange;
    using reflex::Range;
    using reflex::Sparse;

    reflex::MAX_STRING_SIZE = 999;
    reflex::MAX_SEQ_SIZE = 9;

    using std::vector;
    DDS_ReturnCode_t         rc;
    DDSDomainParticipant *   participant = NULL;
    DDSTopic *               topic = NULL;
    DDSDataWriter *dataWriter         = NULL;
    DDSDynamicDataWriter *ddWriter    = NULL;
    bool                     returnValue = false;
    DDS_DynamicDataTypeProperty_t props;
    DDS_ExceptionCode_t ex;
    DDS_Duration_t period { 0, 1000*1000*1000 };

    int32_t int_var = 10;
    float float_var = 3;
    double double_var = 30; 
    bool bool_var = true;
    char char_var = 'Z';
    octet_t octet_var = 'A';
    long long ll = 0xFFFFFFF;
    std::string str("Hello World!");
    reflex::bounded<std::string, 99> bstr(str);

    custom::TupleIntString tis 
      = std::make_tuple(99, std::make_tuple("$99.99"));

    vector<int32_t> vl(0, 55555);
    reflex::bounded<vector<int32_t>, 77> bvl(vl);

    float float_arr[SIZE] = { 1.1f, 2.2f, 3.3f, 4.4f };
    std::list<float>       float_list(float_arr, float_arr + SIZE);
    std::set<double>       double_set(begin(float_list), end(float_list));
    vector<bool>        vb(SIZE, 0x1);
    vector<std::string> vs(SIZE, "Zz");
    vector<vector<std::list<std::tuple<char>>>> 
      vvltc(SIZE, std::vector<std::list<std::tuple<char>>>(0,std::list<std::tuple<char>>(2,
              std::make_tuple('M'))));
    
    vector<std::tuple<bool, std::string, std::string>> vtbss(SIZE);
    vtbss[0] = std::make_tuple(true,  "India",  "india");
    vtbss[1] = std::make_tuple(false, "USA",    "usa");
    vtbss[2] = std::make_tuple(true,  "Spain",  "spain");
    vtbss[3] = std::make_tuple(true,  "Canada", "canada");

    typedef vector<std::tuple<std::string, std::string>> VectorOfTuples;
    vector<VectorOfTuples>
      vvtss(1, VectorOfTuples(3, std::make_tuple("232 E. Java", "RTI")));
    
    std::string md_string_array[1][2][3] = { { { "00",  "01",  "01"  }, 
                                               { "02",  "03",  "05" }  }  };
    
    bool bool_array[2] = { true, false };
    char char_array[2][3] = { { '~', '!', '@' }, 
                              { '$', '%', '^' }  };

    std::tuple<std::string, octet_t>
       tuple_array[2] = { std::make_tuple("0x41",  65), 
                          std::make_tuple("0x42",  66)  };

    std::vector<int32_t> avi[1];

    std::tuple<int> ti_arr[2] = { std::make_tuple(1000), std::make_tuple(2000) };

    std::set<std::tuple<int32_t>> avt[2] 
      = { std::set<std::tuple<int32_t>>(ti_arr, ti_arr + 2), 
          std::set<std::tuple<int32_t>>(ti_arr, ti_arr + 2) };

    std::tuple<Color, Color> tcolor = std::make_tuple(green, blue);;
    Color palette[3] = { blue, green, red };
    std::list<std::list<Color>> llc(2, std::list<Color>(palette, palette+3));

#ifndef RTI_WIN32
    typedef Union<int16_t, Case<std::string, 20, 19, 18>, Case<char>> UBoolString;
    typedef Union<Color,   Case<char, red>,
                           Case<UBoolString, green>, 
                           Case<std::string, blue>> TestUnion;
    
    UBoolString ubs;
    std::string rti = "Real-Time Innovations, Inc.";
    ubs.set_caseptr_tuple_for_writing(make_caseptr_tuple(rti, char_array[0][0]), 1); 
    TestUnion tu;
    tu = Case<UBoolString, green>(ubs); 
#endif
    
    MultiDimArray<int16_t,2,3>::type int_array = { { {0,0,0}, {5,5,5} } };
    std::list<MultiDimArray<int16_t,2,3>::type> lmda(2, int_array);
    typedef reflex::Sparse<std::string, 
                           float, 
                           MultiDimArray<int16_t, 2, 3>::type
                          > SparseSFA;
    SparseSFA sparse;
    std::get<0>(sparse.get_opt_tuple()) = std::string("Real-Time Innovations, Inc.");
    std::get<1>(sparse.get_opt_tuple()) = 1.10f;
    std::get<2>(sparse.get_opt_tuple()) = int_array;
    std::list<std::tuple<float, bool>> ltfb1(SIZE, std::make_tuple(5.55f, true));
    std::list<std::tuple<float, bool>> ltfb2(SIZE);
    //auto range = make_range(ltfb1);
    auto range = reflex::make_range(ltfb1.begin(), ltfb1.end());
    auto bounded_range = reflex::make_bounded_range<5>(vvtss.begin(), vvtss.end());
    std::list<SparseSFA> lssfa(2, sparse);
    Zip::Map zip_map;
    zip_map.insert(std::make_pair("Fremont", 94555));
    zip_map.insert(std::make_pair("Sunnyvale", 94089));

#ifndef RTI_WIN32
    auto t1 = std::tie(int_var, float_var, double_var, bool_array, char_array,
                       octet_var, ll, bstr, tis, bvl, float_list, double_set, vb, vs, 
                       vvltc, bounded_range, vtbss, md_string_array, tuple_array,
                       avi, avt, tcolor, palette, llc, tu, int_array, lmda, 
                       sparse, range, lssfa, zip_map);
#else
    auto t1 = std::tie(int_var, float_var, double_var, bool_array, char_array,
                       octet_var, ll, bstr, tis, bvl, float_list, double_set, vb, vs,
                       vvltc, bounded_range, vtbss, md_string_array, tuple_array,
                       avi, avt, tcolor, palette, llc, /* tu, */ int_array, lmda, 
                       sparse, range, lssfa, zip_map);
#endif

    typedef std::tuple<
    /* 0 */   int32_t,
    /* 1 */   float,
    /* 2 */   double,
    /* 3 */   bool[2],
    /* 4 */   char[2][3],
    /* 5 */   octet_t,
    /* 6 */   long long,
    /* 7 */   bounded<std::string, 99>,
    /* 8 */   custom::TupleIntString, 
    /* 9 */   bounded<vector<int32_t>, 77>, 
    /* 10 */  std::list<float>,
    /* 11 */  std::set<double>,
    /* 12 */  vector<bool>,
    /* 13 */  vector<std::string>, 
    /* 14 */  vector<vector<std::list<std::tuple<char>>>>, 
    /* 15 */  BoundedRange<VectorOfTuples &, 5>,
    /* 16 */  vector<std::tuple<bool, std::string, std::string>>,
    /* 17 */  std::string[1][2][3], 
    /* 18 */  std::tuple<std::string, octet_t>[2],
    /* 19 */  std::vector<int32_t>[2],
    /* 20 */  std::set<std::tuple<int32_t>>[3],
    /* 21 */  std::tuple<Color, Color>,
    /* 22 */  Color[3],
    /* 23 */  std::list<std::list<Color>>,
#ifndef RTI_WIN32
    /* 24 */  TestUnion,
#endif
    /* 25 */  MultiDimArray<int16_t,2,3>::type,
    /* 26 */  std::list<MultiDimArray<int16_t,2,3>::type>,
    /* 27 */  Sparse<std::string, float, MultiDimArray<int16_t,2,3>::type>,
    /* 28 */  Range<std::tuple<float, bool> &>,
    /* 29 */  std::list<SparseSFA>,
    /* 30 */  std::map<std::string, unsigned> > TupleFull;
    
    reflex::SafeTypeCode<DDS_TypeCode> 
      stc(reflex::Tuple2Typecode<decltype(t1)>());
    
    typedef reflex::detail::remove_refs<decltype(t1)>::type Tuple;

    std::shared_ptr<DDSDynamicDataTypeSupport> 
      safe_typeSupport(new DDSDynamicDataTypeSupport(stc.get(), props));

    reflex::print_IDL(stc.get(), 0);
    
    reflex::SafeDynamicDataInstance ddi1(safe_typeSupport.get());
    reflex::SafeDynamicDataInstance ddi2(safe_typeSupport.get());

    Tuple2DD(t1, *ddi1.get());

#ifndef RTI_WIN32
    ddi1.get()->print(stdout, 2);
#endif

    Tuple t2; 
    std::string st_string;
    std::get<7>(t2) = st_string;
    std::get<9>(t2) = vl;
    std::get<15>(t2) = vvtss;

#ifndef RTI_WIN32
    char st_char;
    UBoolString ubs2;
    ubs2.set_caseptr_tuple_for_reading(make_caseptr_tuple(st_string, st_char));
    std::get<24>(t2).set_caseptr_tuple_for_reading(
        make_caseptr_tuple(st_char, ubs2, st_string));
#endif

#ifdef RTI_WIN32
    std::get<27>(t2) = ltfb2;
#else
    std::get<28>(t2) = ltfb2;
#endif

    DD2Tuple(*ddi1.get(), t2);
    std::cout << "size = " << std::get<29>(t2).size() << std::endl;
    Tuple2DD(t2, *ddi2.get());

#ifndef RTI_WIN32
    ddi2.get()->print(stdout, 2);
#endif

    bool is_equal = 
      ddi1.get()->equal(*ddi2.get()) ? true : false;
    
    std::cout << "DynamicData::equal = " 
              << std::boolalpha 
              << is_equal
              << std::endl;

    assert(ddi1.get()->equal(*ddi2.get()));

    std::tie(ddWriter, participant) = 
      create_ddwriter(stc.get()->name(ex), "N-Tuple", safe_typeSupport.get());

    if(ddWriter != NULL)
    {
      std::cout << "Writing " << stc.get()->name(ex)
                << " (domain = " << domain_id << ")...\n";
      for(;;)
      {
        rc = ddWriter->write(*ddi2.get(), DDS_HANDLE_NIL);
        if(rc != DDS_RETCODE_OK) {
          std::cerr << "Write error = " << reflex::get_readable_retcode(rc) << std::endl;
          break;
        }
        NDDSUtility::sleep(period);
      }
      
      delete_entities(participant);
   }
}


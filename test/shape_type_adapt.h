/*********************************************************************************************
(c) 2005-2013 Copyright, Real-Time Innovations, Inc.  All rights reserved.    	                             
RTI grants Licensee a license to use, modify, compile, and create derivative works 
of the Software.  Licensee has the right to distribute object form only for use with RTI 
products.  The Software is provided “as is”, with no warranty of any type, including 
any warranty for fitness for any purpose. RTI is under no obligation to maintain or 
support the Software.  RTI shall not be liable for any incidental or consequential 
damages arising out of the use or inability to use the software.
**********************************************************************************************/

/****************************************************************************
  This is what the RTI_ADPAT_STRUCT macro expands to.

  RTI_ADAPT_STRUCT(
    ShapeType,
    (std::string, color(), _KEY)
    (int,             x())
    (int,             y())
    (int,     shapesize()))
 ****************************************************************************/

template <> struct StructName<ShapeType>
{
  static std::string get() {
		return DefaultMemberNames::basename("ShapeType");
	}
}; 
	template<> MemberInfo MemberTraits<ShapeType, 0>::member_info() {
		return MemberInfo("color()", ((DDS_Octet)1));
	} 
	template<> MemberInfo MemberTraits<ShapeType, 1>::member_info()
	{
		return MemberInfo("x()", ((DDS_Octet)2));
	} 
	template<> MemberInfo MemberTraits<ShapeType, 2>::member_info() { 
		return MemberInfo("y()", ((DDS_Octet)2)); 
	} 
	template<> MemberInfo MemberTraits<ShapeType, 3>::member_info() { 
		return MemberInfo("shapesize()", ((DDS_Octet)2)); 
	} 
	namespace boost { namespace fusion { namespace traits { 
		template< > struct tag_of<ShapeType > { typedef struct_tag type; }; 
		template< > struct tag_of<ShapeType const> { typedef struct_tag type; }; } 
	namespace extension { 
		template< > struct access::struct_member< ShapeType, 0 > 
		{ 
			typedef std::string attribute_type; 
			typedef attribute_type type; 
			
			template<typename Seq> 
			struct apply { 
				typedef typename add_reference<typename mpl::eval_if< is_const<Seq>, 
				                                                      add_const<attribute_type>, 
				                                                      mpl::identity<attribute_type> >::type >::type type; 
	            static type call(Seq& seq) { return seq.color(); } 
			}; 
		}; 
	    template< > struct struct_member_name< ShapeType, 0 > 
		{ 
			typedef char const* type; 
			static type call() { return "color()"; } 
		}; 
		template< > struct access::struct_member< ShapeType, 1 > 
                { 
                  typedef int attribute_type; 
                  typedef attribute_type type; 
                  template<typename Seq> struct apply { 
                    typedef typename add_reference< typename mpl::eval_if< is_const<Seq>, 
                                                                           add_const<attribute_type>, 
                                                                           mpl::identity<attribute_type> >::type >::type type; 
                    static type call(Seq& seq) { 
                      return seq.x(); 
                    } 
                  }; 
                }; 
                template< > struct struct_member_name< ShapeType, 1 > { 
                  typedef char const* type; 
                  static type call() { return "x()"; } 
                }; 
                template< > struct access::struct_member< ShapeType, 2 > { 
                  typedef int attribute_type; 
                  typedef attribute_type type; 
                  template<typename Seq> struct apply { 
                    typedef typename add_reference< typename mpl::eval_if< is_const<Seq>, 
                                                                           add_const<attribute_type>, 
                                                                           mpl::identity<attribute_type> >::type >::type type; 
                    static type call(Seq& seq) { return seq.y(); } 
                  }; 
                }; 
                template< > struct struct_member_name< ShapeType, 2 > { 
                  typedef char const* type; 
                  static type call() { return "y()"; } 
                }; 
                template< > struct access::struct_member< ShapeType, 3 > { 
                  typedef int attribute_type; 
                  typedef attribute_type type; 
                  template<typename Seq> struct apply { 
                    typedef typename add_reference< typename mpl::eval_if< is_const<Seq>, 
                                                                           add_const<attribute_type>, 
                                                                           mpl::identity<attribute_type> >::type >::type type; 
                    static type call(Seq& seq) { return seq.shapesize(); } 
                  }; 
                }; 
                template< > struct struct_member_name< ShapeType, 3 > { 
                  typedef char const* type; 
                  static type call() { return "shapesize()"; } 
                }; 
                template< > struct struct_size<ShapeType> : mpl::int_<4>{}; 
                template< > struct struct_is_view< ShapeType > : mpl::false_{}; 
        } 
        } 
        namespace mpl { 
          template<typename> struct sequence_tag; 
          template< > struct sequence_tag<ShapeType> { 
            typedef fusion::fusion_sequence_tag type; 
          }; 
          template< > struct sequence_tag< ShapeType const > { 
            typedef fusion::fusion_sequence_tag type; }; 
          } 
        }

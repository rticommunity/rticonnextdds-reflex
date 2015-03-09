/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.    	                             
RTI grants Licensee a license to use, modify, compile, and create derivative works 
of the Software.  Licensee has the right to distribute object form only for use with RTI 
products.  The Software is provided "as is", with no warranty of any type, including 
any warranty for fitness for any purpose. RTI is under no obligation to maintain or 
support the Software.  RTI shall not be liable for any incidental or consequential 
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_SHAPE_TYPE_H
#define RTIREFLEX_SHAPE_TYPE_H

#ifdef RTI_WIN32
#pragma warning(disable: 4351) 
#endif

#include "reflex.h"

class ShapeType
{
  std::string color_;
  int x_, y_, shapesize_;
	
public:
  ShapeType() {}

  ShapeType(const std::string & color,
            int x,
            int y,
            int shapesize)
   : color_(color),
      x_(x),
      y_(y),
      shapesize_(shapesize)
  {}

  std::string & color() { return color_;     }
  int & x()             { return x_;         }
  int & y()             { return y_;         }
  int & shapesize()     { return shapesize_; }
  
  const std::string & color() const { return color_;     }
  const int & x()             const { return x_;         }
  const int & y()             const { return y_;         }
  const int & shapesize()     const { return shapesize_; }
};

RTI_ADAPT_STRUCT(
  ShapeType,
  (std::string, color(), RTI_KEY)
  (int,             x())
  (int,             y())
  (int,     shapesize()))

enum ShapeFillKind {
  SOLID_FILL,
  TRANSPARENT_FILL,
  HORIZONTAL_HATCH_FILL,
  VERTICAL_HATCH_FILL
};

class ShapeTypeExtended : public ShapeType 
{
  ShapeFillKind fillKind_;
  float angle_;

public:
  ShapeTypeExtended() {}

  ShapeTypeExtended(
    const std::string & color,
    int x,
    int y,
    int shapesize,
    ShapeFillKind fill,
    float ang)
    : ShapeType(color, x, y, shapesize),
      fillKind_(fill),
      angle_(ang)
  {}

  ShapeFillKind & fillKind() { return fillKind_; }
  float & angle() { return angle_; }

  const ShapeFillKind & fillKind() const { return fillKind_; }
  const float & angle() const { return angle_; }
};


ENUM_DEF(ShapeFillKind, "ShapeFillKind", 4)

ENUM_MEMBER_DEF(ShapeFillKind, 0, "SOLID_FILL",            0)
ENUM_MEMBER_DEF(ShapeFillKind, 1, "TRANSPARENT_FILL",      1)
ENUM_MEMBER_DEF(ShapeFillKind, 2, "HORIZONTAL_HATCH_FILL", 2)
ENUM_MEMBER_DEF(ShapeFillKind, 3, "VERTICAL_HATCH_FILL",   3)

RTI_ADAPT_VALUETYPE(
  ShapeTypeExtended, 
  BASE(ShapeType),
  (ShapeFillKind, fillKind())
  (float,         angle()))

/* Alternatively, you may use a tuple. */

typedef std::tuple<std::string, int, int, int> ShapeTypeTuple;

STRUCT_NAME_DEF_CUSTOM(ShapeTypeTuple, "ShapeType")

MEMBER_TRAITS_DEF_CUSTOM(ShapeTypeTuple, 0, "color",     RTI_KEY);
MEMBER_TRAITS_DEF_CUSTOM(ShapeTypeTuple, 1, "x",         RTI_REQUIRED);
MEMBER_TRAITS_DEF_CUSTOM(ShapeTypeTuple, 2, "y",         RTI_REQUIRED);
MEMBER_TRAITS_DEF_CUSTOM(ShapeTypeTuple, 3, "shapesize", RTI_REQUIRED);


#endif // RTIREFLEX_LARGE_TYPE_H


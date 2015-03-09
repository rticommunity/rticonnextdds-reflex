/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.    	                             
RTI grants Licensee a license to use, modify, compile, and create derivative works 
of the Software.  Licensee has the right to distribute object form only for use with RTI 
products.  The Software is provided "as is", with no warranty of any type, including 
any warranty for fitness for any purpose. RTI is under no obligation to maintain or 
support the Software.  RTI shall not be liable for any incidental or consequential 
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef MANY_MEMBERS_H
#define MANY_MEMBERS_H

#ifdef RTI_WIN32
#pragma warning(disable: 4351) 
#endif

#include "reflex.h"

struct many_members
{
  int m1,   m2,  m3,  m4,  m5,  m6,  m7,  m8,  m9, m10;
  int m11, m12, m13, m14, m15, m16, m17, m18, m19, m20;
  int m21, m22, m23, m24, m25, m26, m27, m28, m29, m30;
  int m31, m32, m33, m34, m35, m36, m37, m38, m39, m40;
  int m41, m42, m43, m44, m45, m46, m47, m48, m49, m50;
  int m51, m52, m53, m54, m55, m56, m57, m58, m59, m60;
  int m61, m62, m63, m64, m65, m66, m67, m68, m69, m70;
  int m71, m72, m73, m74, m75, m76, m77, m78, m79, m80;
  int m81, m82, m83, m84, m85, m86, m87, m88, m89, m90;
  int m91, m92, m93, m94, m95, m96, m97, m98, m99, m100;
  int m101;
};

RTI_ADAPT_STRUCT(many_members,
    (int,  m1)(int,  m2)(int,  m3)(int,  m4)(int,  m5)
    (int,  m6)(int,  m7)(int,  m8)(int,  m9)(int, m10)
    (int, m11)(int, m12)(int, m13)(int, m14)(int, m15)
    (int, m16)(int, m17)(int, m18)(int, m19)(int, m20)
    (int, m21)(int, m22)(int, m23)(int, m24)(int, m25)
    (int, m26)(int, m27)(int, m28)(int, m29)(int, m30)
    (int, m31)(int, m32)(int, m33)(int, m34)(int, m35)
    (int, m36)(int, m37)(int, m38)(int, m39)(int, m40)
    (int, m41)(int, m42)(int, m43)(int, m44)(int, m45)
    (int, m46)(int, m47)(int, m48)(int, m49)(int, m50)
    (int, m51)(int, m52)(int, m53)(int, m54)(int, m55)
    (int, m56)(int, m57)(int, m58)(int, m59)(int, m60)
    (int, m61)(int, m62)(int, m63)(int, m64)(int, m65)
    (int, m66)(int, m67)(int, m68)(int, m69)(int, m70)
    (int, m71)(int, m72)(int, m73)(int, m74)(int, m75)
    (int, m76)(int, m77)(int, m78)(int, m79)(int, m80)
    (int, m81)(int, m82)(int, m83)(int, m84)(int, m85)
    (int, m86)(int, m87)(int, m88)(int, m89)(int, m90)
    (int, m91)(int, m92)(int, m93)(int, m94)(int, m95)
    (int, m96)(int, m97)(int, m98)(int, m99)(int, m100)
    (int, m101))

#endif // MANY_MEMBERS_H


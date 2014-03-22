/*********************************************************************************************
(c) 2005-2013 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided “as is”, with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_DLLEXPORT_H
#define RTIREFLEX_DLLEXPORT_H

#ifdef RTI_WIN32
  #ifdef BUILD_DD2TUPLE_DLL
    #define DllExport   __declspec( dllexport ) 
    #define DECLSPEC    __cdecl 
    #define EXPIMP_TEMPLATE
  #else
    #define DllExport   __declspec( dllimport ) 
    #define DECLSPEC    __cdecl 
    #define EXPIMP_TEMPLATE extern
  #endif
#else
  #define DllExport  
  #define DECLSPEC
  #define EXPIMP_TEMPLATE
#endif

#endif // RTIREFLEX_DLLEXPORT_H

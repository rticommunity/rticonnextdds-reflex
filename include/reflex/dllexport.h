/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_DLLEXPORT_H
#define RTIREFLEX_DLLEXPORT_H

#ifdef REFLEX_NO_HEADER_ONLY

#ifdef RTI_WIN32
  #ifdef BUILD_REFLEX_DLL
    #define REFLEX_DLL_EXPORT   __declspec( dllexport ) 
    #define REFLEX_DECLSPEC    __cdecl 
    #define REFLEX_EXPIMP_TEMPLATE
  #else
    #define REFLEX_DLL_EXPORT   __declspec( dllimport ) 
    #define REFLEX_DECLSPEC    __cdecl 
    #define REFLEX_EXPIMP_TEMPLATE extern
  #endif
#else
  #define REFLEX_DLL_EXPORT  
  #define REFLEX_DECLSPEC
  #define REFLEX_EXPIMP_TEMPLATE
#endif

#define REFLEX_INLINE 

#else
  #define REFLEX_DLL_EXPORT 
  #define REFLEX_DECLSPEC
  #define REFLEX_EXPIMP_TEMPLATE 
  #define REFLEX_INLINE inline
#endif // REFLEX_NO_HEADER_ONLY

#endif // RTIREFLEX_DLLEXPORT_H

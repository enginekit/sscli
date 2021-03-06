// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// PInvokeMap is an enum that defines the PInvoke attributes.  These
//  values are defined in CorHdr.h.
//
// Date: Sep 99
//
namespace System.Runtime.InteropServices {
    using System.Runtime.InteropServices;
    using System;
    
    // This Enum matchs the CorPinvokeMap defined in CorHdr.h
    [Serializable()] 
    internal enum PInvokeMap
    {
        NoMangle            = 0x0001,   // Pinvoke is to use the member name as specified.
        CharSetMask         = 0x0006,   // Heuristic used in data type & name mapping.
        CharSetNotSpec      = 0x0000,
        CharSetAnsi         = 0x0002, 
        CharSetUnicode      = 0x0004,
        CharSetAuto         = 0x0006,
    
        PinvokeOLE          = 0x0020,   // Heuristic: pinvoke will return hresult, with return value becoming the retval param. Not relevant for fields. 
        SupportsLastError   = 0x0040,   // Information about target function. Not relevant for fields.

        BestFitMask         = 0x0030,
        BestFitEnabled      = 0x0010,
        BestFitDisabled     = 0x0020,
        BestFitUseAsm       = 0x0030,
    
        ThrowOnUnmappableCharMask       = 0x3000,
        ThrowOnUnmappableCharEnabled    = 0x1000,
        ThrowOnUnmappableCharDisabled   = 0x2000,
        ThrowOnUnmappableCharUseAsm     = 0x3000,
    
        // None of the calling convention flags is relevant for fields.
        CallConvMask        = 0x0700,
        CallConvWinapi      = 0x0100,   // Pinvoke will use native callconv appropriate to target windows platform.
        CallConvCdecl       = 0x0200,
        CallConvStdcall     = 0x0300,
        CallConvThiscall    = 0x0400,   // In M9, pinvoke will raise exception.
        CallConvFastcall    = 0x0500,
    }
}

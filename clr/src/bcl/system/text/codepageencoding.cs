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

// WARNING:
//
// This is just an IObjectReference proxy for the Code Page Encodings.
namespace System.Text
{
    using System;
    using System.Runtime.Serialization;
    using System.Security.Permissions;

    /*=================================CodePageEncoding==================================
    ** This class is here only to deserialize the Code Page classes from Everett (V1.1) into
    ** Appropriate Whidbey (V2.0) objects.  We also serialize the Whidbey classes
    ** using this proxy since we pretty much need one anyway and that solves Whidbey
    ** to Everett compatibility as well.
    ==============================================================================*/

    [Serializable()]
    internal sealed class CodePageEncoding : ISerializable, IObjectReference
    {
        // Temp stuff
        [NonSerialized]
        private int m_codePage;
        [NonSerialized]
        private bool m_isReadOnly;
        [NonSerialized]
        private bool m_deserializedFromEverett = false;

        [NonSerialized]
        private EncoderFallback encoderFallback = null;
        [NonSerialized]
        private DecoderFallback decoderFallback = null;

        // Might need this when GetRealObjecting
        [NonSerialized]
        private Encoding realEncoding = null;

        // Constructor called by serialization.
        internal CodePageEncoding(SerializationInfo info, StreamingContext context)
        {
            // Any info?
            if (info==null) throw new ArgumentNullException("info");

            // All versions have a code page
            this.m_codePage = (int)info.GetValue("m_codePage", typeof(int));

            // See if we have a code page
            try
            {
                this.m_isReadOnly = (bool)info.GetValue("m_isReadOnly", typeof(bool));

                this.encoderFallback = (EncoderFallback)info.GetValue("encoderFallback", typeof(EncoderFallback));
                this.decoderFallback = (DecoderFallback)info.GetValue("decoderFallback", typeof(DecoderFallback));
            }
            catch (SerializationException)
            {
                this.m_deserializedFromEverett = true;

                // May as well be read only
                this.m_isReadOnly = true;
            }
        }

        // Just get it from GetEncoding
        public Object GetRealObject(StreamingContext context)
        {
            this.realEncoding = Encoding.GetEncoding(this.m_codePage);

            // If its read only then it uses default fallbacks, otherwise pick up the new ones
            // Otherwise we want to leave the new one read only
            if (!this.m_deserializedFromEverett && !this.m_isReadOnly)
            {
                this.realEncoding = (Encoding)this.realEncoding.Clone();
                this.realEncoding.EncoderFallback = this.encoderFallback;
                this.realEncoding.DecoderFallback = this.decoderFallback;
            }

            return this.realEncoding;
        }

        // ISerializable implementation
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)]
        void ISerializable.GetObjectData(SerializationInfo info, StreamingContext context)
        {
            // We cannot ever call this.
            BCLDebug.Assert(false, "Didn't expect to make it to CodePageEncoding ISerializable.GetObjectData");
            throw new ArgumentException(Environment.GetResourceString("Arg_ExecutionEngineException"));
        }

        [Serializable]
        internal sealed class Decoder : ISerializable, IObjectReference
        {
            // Might need this when GetRealObjecting
            [NonSerialized]
            private Encoding realEncoding = null;

            internal Decoder(SerializationInfo info, StreamingContext context)
            {
                // Any info?
                if (info==null) throw new ArgumentNullException("info");

                this.realEncoding = (Encoding)info.GetValue("encoding", typeof(Encoding));
            }

            // Just get it from GetDecider
            public Object GetRealObject(StreamingContext context)
            {
                return this.realEncoding.GetDecoder();
            }

            // ISerializable implementation, get data for this object
            [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)]
            void ISerializable.GetObjectData(SerializationInfo info, StreamingContext context)
            {
                // We cannot ever call this.
                BCLDebug.Assert(false, "Didn't expect to make it to CodePageEncoding.Decoder.GetObjectData");
                throw new ArgumentException(Environment.GetResourceString("Arg_ExecutionEngineException"));
            }
        }
    }
}

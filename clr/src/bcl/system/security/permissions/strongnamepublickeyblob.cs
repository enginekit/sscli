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
// StrongNamePublicKeyBlob.cs
// 

namespace System.Security.Permissions
{
    using System;
    using SecurityElement = System.Security.SecurityElement;
    using System.Security.Util;

[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable] sealed public class StrongNamePublicKeyBlob
    {
        internal byte[] PublicKey;
        
        internal StrongNamePublicKeyBlob()
        {
        }
        
        public StrongNamePublicKeyBlob( byte[] publicKey )
        {
            if (publicKey == null)
                throw new ArgumentNullException( "PublicKey" );
        
            this.PublicKey = new byte[publicKey.Length];
            Array.Copy( publicKey, 0, this.PublicKey, 0, publicKey.Length );
        }
        
        internal StrongNamePublicKeyBlob( String publicKey )
        {
            this.PublicKey = Hex.DecodeHexString( publicKey );
        }        
        
        private static bool CompareArrays( byte[] first, byte[] second )
        {
            if (first.Length != second.Length)
            {
                return false;
            }
            
            int count = first.Length;
            for (int i = 0; i < count; ++i)
            {
                if (first[i] != second[i])
                    return false;
            }
            
            return true;
        }
                
        
        internal bool Equals( StrongNamePublicKeyBlob blob )
        {
            if (blob == null)
                return false;
            else 
                return CompareArrays( this.PublicKey, blob.PublicKey );
        }

        public override bool Equals( Object obj )
        {
            if (obj == null || !(obj is StrongNamePublicKeyBlob))
                return false;

            return this.Equals( (StrongNamePublicKeyBlob)obj );
        }

        static private int GetByteArrayHashCode( byte[] baData )
        {
            if (baData == null)
                return 0;

            int accumulator = 0;

            for (int i = 0; i < baData.Length; ++i)
            {
                accumulator = (accumulator << 8) ^ (int)baData[i] ^ (accumulator >> 24);
            }

            return accumulator;
        }

        public override int GetHashCode()
        {
            return GetByteArrayHashCode( PublicKey );
        }

        public override String ToString()
        {
            return Hex.EncodeHexString( PublicKey );
        }
    }
}

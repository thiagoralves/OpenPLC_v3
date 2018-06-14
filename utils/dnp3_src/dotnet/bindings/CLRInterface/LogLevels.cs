
//
// Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
// more contributor license agreements. See the NOTICE file distributed
// with this work for additional information regarding copyright ownership.
// Green Energy Corp licenses this file to you under the Apache License,
// Version 2.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This file was forked on 01/01/2013 by Automatak, LLC and modifications
// have been made to this file. Automatak, LLC licenses these modifications to
// you under the terms of the License.
//

using System.Text;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// Combinations of the log filters
    /// </summary>
    public static class LogLevels
    {
        public static readonly System.UInt32 NONE = 0;
        public static readonly System.UInt32 ALL = ~NONE;
        public static readonly System.UInt32 NORMAL = LogFilters.EVENT | LogFilters.ERROR | LogFilters.WARNING | LogFilters.INFO;
        public static readonly System.UInt32 APP_COMMS = LogFilters.APP_HEADER_RX | LogFilters.APP_HEADER_TX | LogFilters.APP_OBJECT_RX | LogFilters.APP_OBJECT_TX;
    }

    /// <summary>
    /// Strongly typed wrapper for log filters
    /// </summary>
    public struct LogFilter
    {
        public LogFilter(System.UInt32 flags)
        {
            this.flags = flags;
        }

        public LogFilter Add(System.UInt32 flags)
        {
            return new LogFilter(this.flags | flags);
        }

        public LogFilter Remove(System.UInt32 flags)
        {
            return new LogFilter(this.flags & (~flags));
        }

        public System.UInt32 Flags
        {
            get
            {
                return flags;
            }
            set
            {
                flags = value;
            }
        }

        private System.UInt32 flags;
    };

    /// <summary>
    /// Constant log filter bitfield
    /// </summary>
    public static class LogFilters
    {
        public const System.UInt32 EVENT = 1;
        public const System.UInt32 ERROR = 1 << 1;
        public const System.UInt32 WARNING = 1 << 2;
        public const System.UInt32 INFO = 1 << 3;
        public const System.UInt32 DEBUG = 1 << 4;

        public const System.UInt32 LINK_RX = DEBUG << 1;
        public const System.UInt32 LINK_RX_HEX = DEBUG << 2;

        public const System.UInt32 LINK_TX = DEBUG << 3;
        public const System.UInt32 LINK_TX_HEX = DEBUG << 4;

        public const System.UInt32 TRANSPORT_RX = DEBUG << 5;
        public const System.UInt32 TRANSPORT_TX = DEBUG << 6;

        public const System.UInt32 APP_HEADER_RX = DEBUG << 7;
        public const System.UInt32 APP_HEADER_TX = DEBUG << 8;

        public const System.UInt32 APP_OBJECT_RX = DEBUG << 9;
        public const System.UInt32 APP_OBJECT_TX = DEBUG << 10;

        public const System.UInt32 APP_HEX_RX = DEBUG << 11;
        public const System.UInt32 APP_HEX_TX = DEBUG << 12;
     
        public static string GetFilterString(System.UInt32 filters)
        {
            switch (filters)
            { 
                case(EVENT):
                    return "EVENT";
                case(ERROR):
                    return "ERROR";
                case(WARNING):
                    return "WARN";
                case(INFO):
                    return "INFO";
                case(DEBUG):
                    return "DEBUG";                
                case(LINK_RX):                    
                case (LINK_RX_HEX):
                    return "<-LL-";
                case (LINK_TX):                    
                case (LINK_TX_HEX):
                    return "-LL->";
                case (TRANSPORT_RX):
                    return "<-TL-";
                case (TRANSPORT_TX):
                    return "-TL->";

                case(APP_HEX_RX):
                case(APP_HEADER_RX):
                case(APP_OBJECT_RX):
                    return "<-AL-";

                case (APP_HEX_TX):
                case (APP_HEADER_TX):
                case (APP_OBJECT_TX):
                    return "-AL->";                
                default:
                    return "?";
            }           
        }

        static void OptionallyAppend(StringBuilder builder, System.UInt32 filters, System.UInt32 flag, string s)
        {
            if ((filters & flag) != 0)
            {
                var notFirst = builder.Length > 0;
                if (notFirst)
                {
                    builder.Append("/");
                }
                builder.Append(s);
            }
        }


    }


}

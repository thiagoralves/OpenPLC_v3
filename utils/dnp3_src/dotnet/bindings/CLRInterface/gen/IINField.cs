//
//  _   _         ______    _ _ _   _             _ _ _
// | \ | |       |  ____|  | (_) | (_)           | | | |
// |  \| | ___   | |__   __| |_| |_ _ _ __   __ _| | | |
// | . ` |/ _ \  |  __| / _` | | __| | '_ \ / _` | | | |
// | |\  | (_) | | |___| (_| | | |_| | | | | (_| |_|_|_|
// |_| \_|\___/  |______\__,_|_|\__|_|_| |_|\__, (_|_|_)
//                                           __/ |
//                                          |___/
// Copyright 2013 Automatak LLC
// 
// Automatak LLC (www.automatak.com) licenses this file
// to you under the the Apache License Version 2.0 (the "License"):
// 
// http://www.apache.org/licenses/LICENSE-2.0.html
//

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatak.DNP3.Interface
{
    public enum IINBit
    {
        ALL_STATIONS = 0,
        CLASS1_EVENTS,
        CLASS2_EVENTS,
        CLASS3_EVENTS,
        NEED_TIME,
        LOCAL_CONTROL,
        DEVICE_TROUBLE,
        DEVICE_RESTART,
        FUNC_NOT_SUPPORTED,
        OBJECT_UNKNOWN,
        PARAM_ERROR,
        EVENT_BUFFER_OVERFLOW,
        ALREADY_EXECUTING,
        CONFIG_CORRUPT,
        RESERVED1,
        RESERVED2 = 15
    };

    [Flags]
    public enum LSBMask : byte
    {
        ALL_STATIONS = 0x01,
        CLASS1_EVENTS = 0x02,
        CLASS2_EVENTS = 0x04,
        CLASS3_EVENTS = 0x08,
        NEED_TIME = 0x10,
        LOCAL_CONTROL = 0x20,
        DEVICE_TROUBLE = 0x40,
        DEVICE_RESTART = 0x80,
    };

    [Flags]
    public enum MSBMask : byte
    {
        FUNC_NOT_SUPPORTED = 0x01,
        OBJECT_UNKNOWN = 0x02,
        PARAM_ERROR = 0x04,
        EVENT_BUFFER_OVERFLOW = 0x08,
        ALREADY_EXECUTING = 0x10,
        CONFIG_CORRUPT = 0x20,
        RESERVED1 = 0x40,
        RESERVED2 = 0x80,
        //special mask that indicates if there was any error processing a request
        REQUEST_ERROR_MASK = FUNC_NOT_SUPPORTED | OBJECT_UNKNOWN | PARAM_ERROR
    };

    public class IINField
    {
        private readonly LSBMask _lsb;
        private readonly MSBMask _msb;

        public IINField(LSBMask lsb, MSBMask msb)
        {
            _lsb = lsb;
            _msb = msb;
        }

        public bool IsSet(IINBit iin) 
        {
	        switch (iin)
	        {
	            case IINBit.ALL_STATIONS:
		            return _lsb.HasFlag(LSBMask.ALL_STATIONS);
	            case IINBit.CLASS1_EVENTS:
		            return _lsb.HasFlag(LSBMask.CLASS1_EVENTS);
	            case IINBit.CLASS2_EVENTS:
		            return _lsb.HasFlag(LSBMask.CLASS2_EVENTS);
	            case IINBit.CLASS3_EVENTS:
		            return _lsb.HasFlag(LSBMask.CLASS3_EVENTS);
	            case IINBit.NEED_TIME:
		            return _lsb.HasFlag(LSBMask.NEED_TIME);
	            case IINBit.LOCAL_CONTROL:
		            return _lsb.HasFlag(LSBMask.LOCAL_CONTROL);
	            case IINBit.DEVICE_TROUBLE:
		            return _lsb.HasFlag(LSBMask.DEVICE_TROUBLE);
	            case IINBit.DEVICE_RESTART:
		            return _lsb.HasFlag(LSBMask.DEVICE_RESTART);
	            case IINBit.FUNC_NOT_SUPPORTED:
		            return _msb.HasFlag(MSBMask.FUNC_NOT_SUPPORTED);
	            case IINBit.OBJECT_UNKNOWN:
		            return _msb.HasFlag(MSBMask.OBJECT_UNKNOWN);
	            case IINBit.PARAM_ERROR:
		            return _msb.HasFlag(MSBMask.PARAM_ERROR);
	            case IINBit.EVENT_BUFFER_OVERFLOW:
		            return _msb.HasFlag(MSBMask.EVENT_BUFFER_OVERFLOW);
	            case IINBit.ALREADY_EXECUTING:
		            return _msb.HasFlag(MSBMask.ALREADY_EXECUTING);
	            case IINBit.CONFIG_CORRUPT:
		            return _msb.HasFlag(MSBMask.CONFIG_CORRUPT);
	            case IINBit.RESERVED1:
		            return _msb.HasFlag(MSBMask.RESERVED1);
	            case IINBit.RESERVED2:
		            return _msb.HasFlag(MSBMask.RESERVED2);
	            default:
		            return false;
	        }
        }
    }
}
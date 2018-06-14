/**
 * Copyright 2013-2016 Automatak, LLC
 *
 * Licensed to Automatak, LLC (www.automatak.com) under one or more
 * contributor license agreements. See the NOTICE file distributed with this
 * work for additional information regarding copyright ownership. Automatak, LLC
 * licenses this file to you under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may obtain
 * a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
package com.automatak.dnp3;

public class IINField
{
    public enum LSB
    {
        ALL_STATIONS(1 << 0),
        CLASS1_EVENTS(1 << 1),
        CLASS2_EVENTS(1 << 2),
        CLASS3_EVENTS(1 << 3),
        NEED_TIME(1 << 4),
        LOCAL_CONTROL(1 << 5),
        DEVICE_TROUBLE(1 << 6),
        DEVICE_RESTART(1 << 7);

        public final int id;

        public boolean isSet(byte value)
        {
            return (value & id) != 0;
        }

        LSB(int id)
        {
            this.id = id;
        }
    }

    public enum MSB
    {
        FUNC_NOT_SUPPORTED(1 << 0),
        OBJECT_UNKNOWN(1 << 1),
        PARAM_ERROR(1 << 2),
        EVENT_BUFFER_OVERFLOW(1 << 3),
        ALREADY_EXECUTING(1 << 4),
        CONFIG_CORRUPT(1 << 5),
        RESERVED1(1 << 6),
        RESERVED2(1 << 7);

        public final int id;

        public boolean isSet(byte value)
        {
            return (value & id) != 0;
        }

        MSB(int id)
        {
            this.id = id;
        }
    }

    public final byte lsb;
    public final byte msb;

    public IINField(byte lsb, byte msb)
    {
        this.lsb = lsb;
        this.msb = msb;
    }

    public boolean isSet(LSB bit)
    {
        return bit.isSet(lsb);
    }

    public boolean isSet(MSB bit)
    {
        return bit.isSet(msb);
    }


}

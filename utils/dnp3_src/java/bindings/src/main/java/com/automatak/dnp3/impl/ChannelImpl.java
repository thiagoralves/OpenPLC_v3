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
package com.automatak.dnp3.impl;


import com.automatak.dnp3.*;


class ChannelImpl implements Channel {

    private long nativePointer;

    public ChannelImpl(long nativePointer)
    {
        this.nativePointer = nativePointer;
    }

    @Override
    public void finalize()
    {
        this.destroy_native(this.nativePointer);
    }

    @Override
    public void setLogLevel(int levels)
    {
        this.set_log_level_native(this.nativePointer, levels);
    }

    @Override
    public LinkStatistics getStatistics()
    {
        return this.get_statistics_native(this.nativePointer);
    }

    @Override
    public synchronized Master addMaster(String id, SOEHandler handler, MasterApplication application, MasterStackConfig config) throws DNP3Exception
    {
        long ret = get_native_master(nativePointer, id, handler, application, config);

        if(ret == 0)
        {
            throw new DNP3Exception("Unable to create master");
        }

        return new MasterImpl(ret);
    }

    @Override
    public synchronized Outstation addOutstation(String id, CommandHandler commandHandler, OutstationApplication application, OutstationStackConfig config) throws DNP3Exception
    {
        long ret = get_native_outstation(nativePointer, id, commandHandler, application, config);

        if(ret == 0)
        {
            throw new DNP3Exception("Unable to create outstation");
        }

        return new OutstationImpl(ret);
    }

    @Override
    public synchronized void shutdown()
    {
        this.shutdown_native(nativePointer);
    }

    private native void set_log_level_native(long nativePointer, int levels);
    private native LinkStatistics get_statistics_native(long nativePointer);
    private native void shutdown_native(long nativePointer);
    private native void destroy_native(long nativePointer);
    private native long get_native_master(long nativePointer, String id, SOEHandler handler, MasterApplication application, MasterStackConfig config);
    private native long get_native_outstation(long nativePointer, String id, CommandHandler commandHandler, OutstationApplication application, OutstationStackConfig config);
}

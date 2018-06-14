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

import com.automatak.dnp3.ChangeSet;
import com.automatak.dnp3.Outstation;
import com.automatak.dnp3.StackStatistics;

class OutstationImpl implements Outstation {

    private long nativePointer;

    public OutstationImpl(long nativePointer) {
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
    public StackStatistics getStatistics() { return this.get_statistics_native(this.nativePointer); }

    @Override
    public void enable()
    {
        this.enable_native(nativePointer);
    }

    @Override
    public void disable()
    {
        this.disable_native(nativePointer);
    }

    @Override
    public void shutdown()
    {
        this.shutdown_native(nativePointer);
    }

    @Override
    public void apply(ChangeSet changeSet) {

        ChangeSetImpl impl = new ChangeSetImpl();
        changeSet.apply(impl);
        this.apply_native(this.nativePointer, impl.nativePointer);
    }

    private native void set_log_level_native(long nativePointer, int levels);
    private native StackStatistics get_statistics_native(long nativePointer);
    private native void enable_native(long nativePointer);
    private native void disable_native(long nativePointer);
    private native void shutdown_native(long nativePointer);
    private native void destroy_native(long nativePointer);
    private native void apply_native(long nativePointer, long nativeChangeSet);
}

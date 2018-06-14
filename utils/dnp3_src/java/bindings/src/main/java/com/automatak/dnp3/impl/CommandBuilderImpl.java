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

class CommandBuilderImpl implements CommandBuilder {

    public final long nativePointer;

    public CommandBuilderImpl()
    {
        this.nativePointer = create_native();
    }

    @Override
    public void finalize()
    {
        destroy_native(nativePointer);
    }

    @Override
    public void addCrob(Iterable<IndexedValue<ControlRelayOutputBlock>> commands)
    {
        this.add_crob_native(nativePointer, commands);
    }

    @Override
    public void addAO16(Iterable<IndexedValue<AnalogOutputInt16>> commands)
    {
        this.add_aoI16_native(nativePointer, commands);
    }

    @Override
    public void addAO32(Iterable<IndexedValue<AnalogOutputInt32>> commands)
    {
        this.add_aoI32_native(nativePointer, commands);
    }

    @Override
    public void addAOFloat32(Iterable<IndexedValue<AnalogOutputFloat32>> commands)
    {
        this.add_aoF32_native(nativePointer, commands);
    }

    @Override
    public void addAODouble64(Iterable<IndexedValue<AnalogOutputDouble64>> commands)
    {
        this.add_aoD64_native(nativePointer, commands);
    }

    private native long create_native();
    private native void destroy_native(long nativePointer);

    private native void add_crob_native(long nativePointer, Iterable<IndexedValue<ControlRelayOutputBlock>> commands);
    private native void add_aoI16_native(long nativePointer, Iterable<IndexedValue<AnalogOutputInt16>> commands);
    private native void add_aoI32_native(long nativePointer, Iterable<IndexedValue<AnalogOutputInt32>> commands);
    private native void add_aoF32_native(long nativePointer, Iterable<IndexedValue<AnalogOutputFloat32>> commands);
    private native void add_aoD64_native(long nativePointer, Iterable<IndexedValue<AnalogOutputDouble64>> commands);
}

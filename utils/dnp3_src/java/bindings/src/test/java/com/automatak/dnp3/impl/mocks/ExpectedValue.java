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
package com.automatak.dnp3.impl.mocks;

import com.automatak.dnp3.*;

public class ExpectedValue {

    enum Type {
        BinaryType,
        DoubleBinaryType,
        AnalogType,
        CounterType,
        FrozenCounterType,
        BOStatusType,
        AOStatusType
    }

    public static final ExpectedValue.Type[] ALL_TYPES = ExpectedValue.Type.values();

    final int index;
    final long value;
    final Type type;

    public ExpectedValue(BinaryInput measurement, int index)
    {
        this(measurement.value ? 1 : 0, index, Type.BinaryType);
    }

    public ExpectedValue(DoubleBitBinaryInput measurement, int index)
    {
        this(measurement.value.toType(), index, Type.DoubleBinaryType);
    }

    public ExpectedValue(AnalogInput measurement, int index)
    {
        this((long)measurement.value, index, Type.AnalogType);
    }

    public ExpectedValue(Counter measurement, int index)
    {
        this(measurement.value, index, Type.CounterType);
    }

    public ExpectedValue(FrozenCounter measurement, int index)
    {
        this(measurement.value, index, Type.FrozenCounterType);
    }

    public ExpectedValue(BinaryOutputStatus measurement, int index)
    {
        this(measurement.value ? 1 : 0, index, Type.BOStatusType);
    }

    public ExpectedValue(AnalogOutputStatus measurement, int index)
    {
        this((long)measurement.value, index, Type.AOStatusType);
    }


    ExpectedValue(long value, int index, Type type)
    {
        this.value = value;
        this.index = index;
        this.type = type;
    }

    public boolean isEqual(ExpectedValue other)
    {
        return (this.type == other.type) && (this.index == other.index) && (this.value == other.value);
    }

    @Override
    public String toString()
    {
        return String.format("[%d - %s(%d)]", index, this.type, this.value);
    }

}


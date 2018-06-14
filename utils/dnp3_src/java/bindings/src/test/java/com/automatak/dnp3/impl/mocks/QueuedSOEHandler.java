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

import java.time.Duration;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Queue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.function.Function;

public class QueuedSOEHandler implements SOEHandler {

    final LinkedBlockingQueue<List<ExpectedValue>> items = new LinkedBlockingQueue<>();
    List<ExpectedValue> temp = null;

    <T> void add(Iterable<T> values, Function<T, ExpectedValue> convert)
    {
        values.forEach(v -> this.temp.add(convert.apply(v)));
    }

    public List<ExpectedValue> waitForValues(Duration duration)
    {
        try
        {
            return items.poll(duration.toMillis(), TimeUnit.MILLISECONDS);
        }
        catch(InterruptedException ex)
        {
            throw new RuntimeException(ex);
        }
    }

    @Override
    public void start()
    {
        this.temp = new ArrayList<>();
    }

    @Override
    public void end()
    {
        this.items.add(this.temp);
        this.temp = null;
    }

    @Override
    public void processBI(HeaderInfo info, Iterable<IndexedValue<BinaryInput>> values)
    {
        this.add(values, v -> new ExpectedValue(v.value, v.index));
    }

    @Override
    public void processDBI(HeaderInfo info, Iterable<IndexedValue<DoubleBitBinaryInput>> values)
    {
        this.add(values, v -> new ExpectedValue(v.value, v.index));
    }

    @Override
    public void processAI(HeaderInfo info, Iterable<IndexedValue<AnalogInput>> values)
    {
        this.add(values, v -> new ExpectedValue(v.value, v.index));
    }

    @Override
    public void processC(HeaderInfo info, Iterable<IndexedValue<Counter>> values)
    {
        this.add(values, v -> new ExpectedValue(v.value, v.index));
    }

    @Override
    public void processFC(HeaderInfo info, Iterable<IndexedValue<FrozenCounter>> values)
    {
        this.add(values, v -> new ExpectedValue(v.value, v.index));
    }

    @Override
    public void processBOS(HeaderInfo info, Iterable<IndexedValue<BinaryOutputStatus>> values)
    {
        this.add(values, v -> new ExpectedValue(v.value, v.index));
    }

    @Override
    public void processAOS(HeaderInfo info, Iterable<IndexedValue<AnalogOutputStatus>> values)
    {
        this.add(values, v -> new ExpectedValue(v.value, v.index));
    }

    @Override
    public void processDNPTime(HeaderInfo info, Iterable<DNPTime> values) {}
}

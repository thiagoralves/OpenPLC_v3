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
package com.automatak.dnp3.mock;

import com.automatak.dnp3.*;

public class PrintingSOEHandler implements SOEHandler {

    private static PrintingSOEHandler instance = new PrintingSOEHandler();

    public static SOEHandler getInstance() {
        return instance;
    }

    private PrintingSOEHandler(){}

    @Override
    public void start()
    {
        System.out.println("start asdu");
    }

    @Override
    public void end()
    {
        System.out.println("end asdu");
    }

    @Override
    public void processBI(HeaderInfo info, Iterable<IndexedValue<BinaryInput>> values)
    {
        System.out.println(info);
        values.forEach((meas) -> System.out.println(meas));
    }

    @Override
    public void processDBI(HeaderInfo info, Iterable<IndexedValue<DoubleBitBinaryInput>> values)
    {
        System.out.println(info);
        values.forEach((meas) -> System.out.println(meas));
    }

    @Override
    public void processAI(HeaderInfo info, Iterable<IndexedValue<AnalogInput>> values)
    {
        System.out.println(info);
        values.forEach((meas) -> System.out.println(meas));
    }

    @Override
    public void processC(HeaderInfo info, Iterable<IndexedValue<Counter>> values)
    {
        System.out.println(info);
        values.forEach((meas) -> System.out.println(meas));
    }

    @Override
    public void processFC(HeaderInfo info, Iterable<IndexedValue<FrozenCounter>> values)
    {
        System.out.println(info);
        values.forEach((meas) -> System.out.println(meas));
    }

    @Override
    public void processBOS(HeaderInfo info, Iterable<IndexedValue<BinaryOutputStatus>> values)
    {
        System.out.println(info);
        values.forEach((meas) -> System.out.println(meas));
    }

    @Override
    public void processAOS(HeaderInfo info, Iterable<IndexedValue<AnalogOutputStatus>> values)
    {
        System.out.println(info);
        values.forEach((meas) -> System.out.println(meas));
    }

    @Override
    public void processDNPTime(HeaderInfo info, Iterable<DNPTime> values)
    {
        System.out.println(info);
        values.forEach((meas) -> System.out.println(meas));
    }
}

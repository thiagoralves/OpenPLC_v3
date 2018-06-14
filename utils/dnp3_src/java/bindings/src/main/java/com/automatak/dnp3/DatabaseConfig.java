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

import java.util.ArrayList;
import java.util.List;
import java.util.function.Function;

/**
 * Configuration class for the outstation database
 */
public class DatabaseConfig {

    public final List<BinaryConfig> binary;
    public final List<DoubleBinaryConfig> doubleBinary;
    public final List<AnalogConfig> analog;
    public final List<CounterConfig> counter;
    public final List<FrozenCounterConfig> frozenCounter;
    public final List<BinaryOutputStatusConfig> boStatus;
    public final List<AnalogOutputStatusConfig> aoStatus;

    private static <T> List<T> initialize(int num, Function<Integer, T> factory) {
        ArrayList<T> list = new ArrayList<>(num);
        for (int i = 0; i < num; ++i) list.add(factory.apply(i));
        return list;
    }

    public static DatabaseConfig allValues(int num) {
        return new DatabaseConfig(num, num, num, num, num, num, num);
    }

    public DatabaseConfig(int numBinary, int numDoubleBinary, int numAnalog, int numCounter, int numFrozenCounter, int numBOStatus, int numAOStatus) {
        this.binary = initialize(numBinary, BinaryConfig::new);
        this.doubleBinary = initialize(numDoubleBinary, DoubleBinaryConfig::new);
        this.analog = initialize(numAnalog, AnalogConfig::new);
        this.counter = initialize(numCounter, CounterConfig::new);
        this.frozenCounter = initialize(numFrozenCounter, FrozenCounterConfig::new);
        this.boStatus = initialize(numBOStatus, BinaryOutputStatusConfig::new);
        this.aoStatus = initialize(numAOStatus, AnalogOutputStatusConfig::new);
    }

}

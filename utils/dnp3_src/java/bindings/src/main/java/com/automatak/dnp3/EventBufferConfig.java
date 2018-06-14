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

/**
 * Configuration of maximum event counts per event type.
 * <p>
 * The underlying implementation uses a *preallocated* heap buffer to store events
 * until they are transmitted to the master. The size of this buffer is proportional
 * to the TotalEvents() method, i.e. the sum of the maximum events for each type.
 * <p>
 * Implementations should configure the buffers to store a reasonable number events
 * given the polling frequency and memory restrictions of the target platform.
 */
public class EventBufferConfig {

    /**
     * Construct the class using the same maximum for all types. This is mainly used for demo purposes.
     * You probably don't want to use this method unless your implementation actually reports every type.
     *
     * @param num size parameter for every type
     * @return a new EventBufferConfig instance
     */
    public static EventBufferConfig allTypes(int num) {
        return new EventBufferConfig(num, num, num, num, num, num, num);
    }

    /**
     * Construct the class specifying the maximum number of events for each type individually
     * @param maxBinaryEvents maximum number of event for this type
     * @param maxDoubleBinaryEvents maximum number of event for this type
     * @param maxAnalogEvents maximum number of event for this type
     * @param maxCounterEvents maximum number of event for this type
     * @param maxFrozenCounterEvents maximum number of event for this type
     * @param maxBinaryOutputStatusEvents maximum number of event for this type
     * @param maxAnalogOutputStatusEvents maximum number of event for this type
     */
    public EventBufferConfig(
            int maxBinaryEvents,
            int maxDoubleBinaryEvents,
            int maxAnalogEvents,
            int maxCounterEvents,
            int maxFrozenCounterEvents,
            int maxBinaryOutputStatusEvents,
            int maxAnalogOutputStatusEvents)
    {
        this.maxBinaryEvents = maxBinaryEvents;
        this.maxDoubleBinaryEvents = maxDoubleBinaryEvents;
        this.maxAnalogEvents = maxAnalogEvents;
        this.maxCounterEvents = maxCounterEvents;
        this.maxFrozenCounterEvents = maxFrozenCounterEvents;
        this.maxBinaryOutputStatusEvents = maxBinaryOutputStatusEvents;
        this.maxAnalogOutputStatusEvents = maxAnalogOutputStatusEvents;
    }

    /// The number of binary events the outstation will buffer before overflowing
    public int maxBinaryEvents;

    /// The number of double bit binary events the outstation will buffer before overflowing
    public int maxDoubleBinaryEvents;

    /// The number of analog events the outstation will buffer before overflowing
    public int maxAnalogEvents;

    /// The number of counter events the outstation will buffer before overflowing
    public int maxCounterEvents;

    /// The number of frozen counter events the outstation will buffer before overflowing
    public int maxFrozenCounterEvents;

    /// The number of binary output status events the outstation will buffer before overflowing
    public int maxBinaryOutputStatusEvents;

    /// The number of analog output status events the outstation will buffer before overflowing
    public int maxAnalogOutputStatusEvents;

};



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
 * A discrete or continuous analog value
 */
public class AnalogInput extends Measurement
{
    public final double value;

    /**
     * Primary constructor
     * @param value         double precision value type
     * @param quality       bit-field representing quality values
     * @param timestamp     milliseconds since unix epoch UTC
     */
    public AnalogInput(double value, byte quality, long timestamp)
    {
        super(quality, timestamp);
        this.value = value;
    }

    @Override
    public String toString() {
        return String.format("Analog(%f, 0x%x, %d)", value, quality, timestamp);
    }
}

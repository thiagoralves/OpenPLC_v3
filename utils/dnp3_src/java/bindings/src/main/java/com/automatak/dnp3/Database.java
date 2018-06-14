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

import com.automatak.dnp3.enums.EventMode;

/**
 * Interface used to load measurement changes into an outstation
 */
public interface Database
{
    /**
     * Update a value in the database
     * @param value measurement to update
     * @param index index of measurement
     */
    void update(BinaryInput value, int index);

    /**
     * Update a value in the database
     * @param value measurement to update
     * @param index index of measurement
     */
    void update(DoubleBitBinaryInput value, int index);

    /**
     * Update a value in the database
     * @param value measurement to update
     * @param index index of measurement
     */
    void update(AnalogInput value, int index);

    /**
     * Update a value in the database
     * @param value measurement to update
     * @param index index of measurement
     */
    void update(Counter value, int index);

    /**
     * Update a value in the database
     * @param value measurement to update
     * @param index index of measurement
     */
    void update(FrozenCounter value, int index);

    /**
     * Update a value in the database
     * @param value measurement to update
     * @param index index of measurement
     */
    void update(BinaryOutputStatus value, int index);

    /**
     * Update a value in the database
     * @param value measurement to update
     * @param index index of measurement
     */
    void update(AnalogOutputStatus value, int index);

    /**
     * Update a value in the database
     * @param value measurement to update
     * @param index index of measurement
     * @param mode EventMode to use
     */
    void update(BinaryInput value, int index, EventMode mode);

    /**
     * Update a value in the database
     * @param value measurement to update
     * @param index index of measurement
     * @param mode EventMode to use
     */
    void update(DoubleBitBinaryInput value, int index, EventMode mode);

    /**
     * Update a value in the database
     * @param value measurement to update
     * @param index index of measurement
     * @param mode EventMode to use
     */
    void update(AnalogInput value, int index, EventMode mode);

    /**
     * Update a value in the database
     * @param value measurement to update
     * @param index index of measurement
     * @param mode EventMode to use
     */
    void update(Counter value, int index, EventMode mode);

    /**
     * Update a value in the database
     * @param value measurement to update
     * @param index index of measurement
     * @param mode EventMode to use
     */
    void update(FrozenCounter value, int index, EventMode mode);

    /**
     * Update a value in the database
     * @param value measurement to update
     * @param index index of measurement
     * @param mode EventMode to use
     */
    void update(BinaryOutputStatus value, int index, EventMode mode);

    /**
     * Update a value in the database
     * @param value measurement to update
     * @param index index of measurement
     * @param mode EventMode to use
     */
    void update(AnalogOutputStatus value, int index, EventMode mode);

}

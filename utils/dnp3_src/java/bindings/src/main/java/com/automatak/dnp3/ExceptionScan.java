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
 * Defines a periodic class based scan (Class 0,1,2,3)
 */
public class ExceptionScan
{
    public int classMask;
    public long scanRateMs;

    /**
     * Scan all events every 5 seconds
     */
    public ExceptionScan()
    {
        this.classMask = PointClassMasks.ALL_EVENTS;
        this.scanRateMs = 5000;
    }

    /**
     *
     * Constructor that sets all values
     *
     * @param classMask Bit-field that determines which classes are scanned. Use PointClass.getMask to create a mask.
     * @param scanRateMs Periodic scan rate in milliseconds
     */
    public ExceptionScan(int classMask, long scanRateMs)
    {
        this.classMask = classMask;
        this.scanRateMs = scanRateMs;
    }
}

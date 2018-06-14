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

import com.automatak.dnp3.enums.TimeSyncMode;

import java.time.Duration;

/**
 * Configuration class for a master station
 */
public class MasterConfig {


    /**
     * Constructor with reasonable defaults
     */
    public MasterConfig()
    {

    }

    /**
     * Application layer response timeout
     */
    public Duration responseTimeout = Duration.ofSeconds(5);

    /**
     * Enumeration that specifies how/if the master does performs time synchronization
     */
    public TimeSyncMode timeSyncMode = TimeSyncMode.None;

    /**
     * If true, the master will disable unsol on startup for all 3 event classes
     */
    public boolean disableUnsolOnStartup = true;

    /**
     * If true, the master will not clear the restart IIN bit in response to detecting it set
     */
    public boolean ignoreRestartIIN = false;

    /**
     * If true, the master will enable/disable unsol on startup
     */
    public ClassField unsolClassMask = ClassField.allEventClasses();

    /**
     * Which classes should be requested in a startup integrity scan, defaults to 3/2/1/0
     * A mask equal to 0 means no startup integrity scan will be performed
     */
    public ClassField startupIntegrityClassMask = ClassField.allClasses();

    /**
     * Defines whether an integrity scan will be performed when the EventBufferOverflow IIN is detected
     */
    public boolean integrityOnEventOverflowIIN = true;

    /**
     * Which classes should be requested in an event scan when detecting corresponding events available IIN
     */
    public ClassField eventScanOnEventsAvailableClassMask = ClassField.none();

    /**
     * Time delay beforce retrying a failed task
     */
    public Duration taskRetryPeriod = Duration.ofSeconds(5);

    /**
     * Period after which tasks that haven't started are failed
     */
    public Duration taskStartTimeout = Duration.ofSeconds(10);

    /**
     * maximum APDU tx size in bytes
     */
    public int maxTxFragSize = 2048;

    /**
     * maximum APDU rx size in bytes
     */
    public int maxRxFragSize = 2048;
}

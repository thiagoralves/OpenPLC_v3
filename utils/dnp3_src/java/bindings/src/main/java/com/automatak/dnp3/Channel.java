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
 * A communication channel to which DNP3 masters / outstation can be attached
 */
public interface Channel {

    /**
     * Shutdown the channel and all stacks that have been added. Calling shutdown more than once or
     * continuing to use child objects (masters/outstations) after calling shutdown can cause a failure.
     */
    void shutdown();

    /**
     * Set the active log levels for the channel's logger
     * @param levels
     */
    void setLogLevel(int levels);

    /**
     * Get statistics for the channel
     * @return the statistics object
     */
    LinkStatistics getStatistics();

    /**
     * Adds a master to the channel
     *
     * @param loggerId name of the logger that will be assigned to this stack
     * @param handler where measurements will be sent as they are received from the outstation
     * @param application  master application instance
     * @param config configuration information for the master stack
     * @return reference to the created master
     * @throws DNP3Exception if any error occurs while creating the master
     */
    Master addMaster(String loggerId, SOEHandler handler, MasterApplication application, MasterStackConfig config) throws DNP3Exception;

    /**
     * Adds an outstation to the channel
     *
     * @param id name of the logger that will be assigned to this stack
     * @param commandHandler where command requests are sent to be handled in application code
     * @param application outstation application instance
     * @param config configuration information for the outstation stack
     * @return reference to the created outstation
     * @throws DNP3Exception if any error occurs while creating the outstation
     */
    Outstation addOutstation(String id, CommandHandler commandHandler, OutstationApplication application, OutstationStackConfig config) throws DNP3Exception;
}

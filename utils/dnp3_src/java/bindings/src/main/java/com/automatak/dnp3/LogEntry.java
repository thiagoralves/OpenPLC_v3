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
 * A logged event
 */
public class LogEntry {

    /**
     * Construct a new log entry
     * @param level the value of the associated log level
     * @param id the id of the logger
     * @param location location of the message in the underlying C++
     * @param message the log message
     */
    public LogEntry(int level, String id, String location, String message)
    {
        this.level = level;
        this.id = id;
        this.location = location;
        this.message = message;
    }

    /**
     * Log level of the entry
     */
    public final int level;

    /**
     * Name of the logger by which the event was recorded
     */
    public final String id;

    /**
     * Event location in the underlying C++
     */
    public final String location;

    /**
     * Event message
     */
    public final String message;

}

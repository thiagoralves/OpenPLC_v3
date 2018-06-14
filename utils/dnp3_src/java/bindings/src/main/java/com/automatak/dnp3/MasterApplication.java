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

import com.automatak.dnp3.enums.MasterTaskType;

/**
 * Master application code implements this interface to interface with stack
 */
public interface MasterApplication extends LinkStatusListener {

    /**
     * Get UTC time
     *
     * @return a count of milliseconds since Unix epoch for the purposes of time syncing
     */
    long getMillisecondsSinceEpoch();

    /**
     * Called whenever an IIN field is received in an ASDU from the outstation
     * @param iin the received IIN field
     */
    void onReceiveIIN(IINField iin);

    /**
     * Called whenever a task starts
     * @param type The type of the task
     * @param id The user assigned id
     */
    void onTaskStart(MasterTaskType type, TaskId id);

    /**
     * Task completion notification for built-in and user defined tasks
     * @param info information about the complete task
     */
    void onTaskComplete(TaskInfo info);

    /**
     * Called when the application layer is opened
     */
    void onOpen();

    /**
     * Called when the application layer is closed
     */
    void onClose();

    /**
     * Tells the master whether to assign class on startup
     * @return if true, the master will assign classes during startup
     */
    boolean assignClassDuringStartup();

    /**
     * Returns a list of class assignments to perform during startup or restart
     * Only called if AssignClassDuringStartup returns true
     * @return An enumerable collection of class assignments
     */
    Iterable<ClassAssignment> getClassAssignments();
}

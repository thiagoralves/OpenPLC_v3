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

import com.automatak.dnp3.enums.AssignClassType;
import com.automatak.dnp3.enums.PointClass;

/**
 * Outstation application code implements this interface to interface with the stack
 */
public interface OutstationApplication extends LinkStatusListener {

    /**
     * @return true of the outstation should allow absolute time to be written
     */
    boolean supportsWriteAbsoluteTime();

    /**
     * Called when the master writes the time
     * @param msSinceEpoch milliseconds since Unix epoch
     * @return true, if the wrote is successful
     */
    boolean writeAbsoluteTime(long msSinceEpoch);

    /**
     * @return true if the outstation supports assigning class
     */
    boolean supportsAssignClass();

    /**
     * Called when the master assigns class to a particular type of measurements
     * @param type the type of the assignment
     * @param clazz the class assigned
     * @param start the start of the measurement range
     * @param stop the end of the measurement range
     */
    void recordClassAssignment(AssignClassType type, PointClass clazz, int start, int stop);

    /**
     * @return Application controlled IIN bits
     */
    ApplicationIIN getApplicationIIN();
}

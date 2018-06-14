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

import com.automatak.dnp3.enums.CommandStatus;
import com.automatak.dnp3.enums.OperateType;

/**
 * Outstation application code implements this interface to handle command requests from a master
 */
public interface CommandHandler {

    /**
     * Called when an ASDU containing commands begins
     */
    void start();

    /**
     * Called when an ASDU containing commands begins
     */
    void end();

    /**
     * Select a ControlRelayOutputBlock (Group12Var1)
     * @param command command object
     * @param index request index
     * @return Enumeration representing the result of the request
     */
    CommandStatus selectCROB(ControlRelayOutputBlock command, int index);

    /**
     * Select a 32-bit integer AnalogOutput (Group41Var1)
     * @param command command object
     * @param index request index
     * @return Enumeration representing the result of the request
     */
    CommandStatus selectAOI32(AnalogOutputInt32 command, int index);

    /**
     * Select a 16-bit integer AnalogOutput (Group41Var2)
     * @param command command object
     * @param index request index
     * @return Enumeration representing the result of the request
     */
    CommandStatus selectAOI16(AnalogOutputInt16 command, int index);

    /**
     * Select a single precision AnalogOutput (Group41Var3)
     * @param command command object
     * @param index request index
     * @return Enumeration representing the result of the request
     */
    CommandStatus selectAOF32(AnalogOutputFloat32 command, int index);

    /**
     * Select a double precision AnalogOutput (Group41Var4)
     * @param command command object
     * @param index request index
     * @return Enumeration representing the result of the request
     */
    CommandStatus selectAOD64(AnalogOutputDouble64 command, int index);

    /**
     * Operate a ControlRelayOutputBlock (Group12Var1)
     * @param command command object
     * @param index request index
     * @param opType The type of the operation (SBO, DO, DONoAck)
     * @return Enumeration representing the result of the request
     */
    CommandStatus operateCROB(ControlRelayOutputBlock command, int index, OperateType opType);

    /**
     * Operate a 32-bit integer AnalogOutput (Group41Var1)
     * @param command command object
     * @param index request index
     * @param opType The type of the operation (SBO, DO, DONoAck)
     * @return Enumeration representing the result of the request
     */
    CommandStatus operateAOI32(AnalogOutputInt32 command, int index, OperateType opType);

    /**
     * Operate a 16-bit integer AnalogOutput (Group41Var2)
     * @param command command object
     * @param index request index
     * @param opType The type of the operation (SBO, DO, DONoAck)
     * @return Enumeration representing the result of the request
     */
    CommandStatus operateAOI16(AnalogOutputInt16 command, int index, OperateType opType);

    /**
     * Operate a single precision AnalogOutput (Group41Var3)
     * @param command command object
     * @param index request index
     * @param opType The type of the operation (SBO, DO, DONoAck)
     * @return Enumeration representing the result of the request
     */
    CommandStatus operateAOF32(AnalogOutputFloat32 command, int index, OperateType opType);

    /**
     * Operate a double precision AnalogOutput (Group41Var4)
     * @param command command object
     * @param index request index
     * @param opType The type of the operation (SBO, DO, DONoAck)
     * @return Enumeration representing the result of the request
     */
    CommandStatus operateAOD64(AnalogOutputDouble64 command, int index, OperateType opType);

}

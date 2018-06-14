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

import java.util.concurrent.CompletableFuture;

/**
 * Interface for dispatching command requests from a master to an outstation
 */
public interface CommandProcessor {

    /**
     * Select and operate a set of headers
     * @param headers A collection of command headers
     * @return value representing the future completion of the commands
     */
    CompletableFuture<CommandTaskResult> selectAndOperate(CommandHeaders headers);

    /**
     * Direct operate a set of headers
     * @param headers A collection of command headers
     * @return value representing the future completion of the commands
     */
    CompletableFuture<CommandTaskResult> directOperate(CommandHeaders headers);

    /**
     * Select and operate a ControlRelayOutputBlock (Group12Variation1)
     * @param command command data
     * @param index index of request
     * @return value representing the future completion of the command
     */
    CompletableFuture<CommandTaskResult> selectAndOperateCROB(ControlRelayOutputBlock command, int index);

    /**
     * Select and operate a 32-bit integer AnalogOutput (Group41Variation1)
     * @param command command data
     * @param index index of request
     * @return value representing the future completion of the command
     */
    CompletableFuture<CommandTaskResult> selectAndOperateAOInt32(AnalogOutputInt32 command, int index);

    /**
     * Select and operate a 16-bit integer AnalogOutput (Group41Variation2)
     * @param command command data
     * @param index index of request
     * @return value representing the future completion of the command
     */
    CompletableFuture<CommandTaskResult> selectAndOperateAOInt16(AnalogOutputInt16 command, int index);

    /**
     * Select and operate single precision AnalogOutput (Group41Variation3)
     * @param command command data
     * @param index index of request
     * @return value representing the future completion of the command
     */
    CompletableFuture<CommandTaskResult> selectAndOperateAOFloat32(AnalogOutputFloat32 command, int index);

    /**
     * Select and operate double precision AnalogOutput (Group41Variation4)
     * @param command command data
     * @param index index of request
     * @return value representing the future completion of the command
     */
    CompletableFuture<CommandTaskResult> selectAndOperateAODouble64(AnalogOutputDouble64 command, int index);

    /**
     * Direct operate a ControlRelayOutputBlock (Group12Variation1)
     * @param command command data
     * @param index index of request
     * @return future to the result of the operation
     */
    CompletableFuture<CommandTaskResult> directOperateCROB(ControlRelayOutputBlock command, int index);

    /**
     * Direct operate a 32-bit integer AnalogOutput (Group41Variation1)
     * @param command command data
     * @param index index of request
     * @return value representing the future completion of the command
     */
    CompletableFuture<CommandTaskResult> directOperateAOInt32(AnalogOutputInt32 command, int index);

    /**
     * Direct operate a 16-bit integer AnalogOutput (Group41Variation2)
     * @param command command data
     * @param index index of request
     * @return value representing the future completion of the command
     */
    CompletableFuture<CommandTaskResult> directOperateAOInt16(AnalogOutputInt16 command, int index);

    /**
     * Direct operate a single precision AnalogOutput (Group41Variation3)
     * @param command command data
     * @param index index of request
     * @return value representing the future completion of the command
     */
    CompletableFuture<CommandTaskResult> directOperateAOFloat32(AnalogOutputFloat32 command, int index);

    /**
     * Direct operate  a double precision AnalogOutput (Group41Variation4)
     * @param command command data
     * @param index index of request
     * @return value representing the future completion of the command
     */
    CompletableFuture<CommandTaskResult> directOperateAODouble64(AnalogOutputDouble64 command, int index);
}

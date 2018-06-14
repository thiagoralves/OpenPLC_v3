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
import com.automatak.dnp3.enums.ControlCode;

/**
 * Command request for control a relay output
 */
public class ControlRelayOutputBlock {

    public final ControlCode function;
    public final short count;
    public final long onTimeMs;
    public final long offTimeMs;
    public final CommandStatus status;

    /**
     * Primary constructor
     * @param function Enumeration that controls that behavior of the command
     * @param count How many times to repeat the operation
     * @param onTimeMs How long to set the output active
     * @param offTimeMs How long to set the output inactive
     * @param status Status code received from an outstation
     */
    public ControlRelayOutputBlock(ControlCode function, short count, long onTimeMs, long offTimeMs, CommandStatus status)
    {
        this.function = function;
        this.count = count;
        this.onTimeMs = onTimeMs;
        this.offTimeMs = offTimeMs;
        this.status = status;
    }

}

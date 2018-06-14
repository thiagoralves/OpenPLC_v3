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
package com.automatak.dnp3.mock;


import com.automatak.dnp3.*;
import com.automatak.dnp3.enums.CommandStatus;
import com.automatak.dnp3.enums.OperateType;

public class ConstantCommandHandler implements CommandHandler {

    private final CommandStatus status;

    public ConstantCommandHandler(CommandStatus status)
    {
        this.status = status;
    }

    @Override
    public void start() {}

    @Override
    public void end() {}
    
    public CommandStatus selectCROB(ControlRelayOutputBlock command, int index)
    {
        return status;
    }
    public CommandStatus selectAOI32(AnalogOutputInt32 command, int index)
    {
        return status;
    }
    public CommandStatus selectAOI16(AnalogOutputInt16 command, int index)
    {
        return status;
    }
    public CommandStatus selectAOF32(AnalogOutputFloat32 command, int index)
    {
        return status;
    }
    public CommandStatus selectAOD64(AnalogOutputDouble64 command, int index)
    {
        return status;
    }

    public CommandStatus operateCROB(ControlRelayOutputBlock command, int index, OperateType opType)
    {
        return status;
    }
    public CommandStatus operateAOI32(AnalogOutputInt32 command, int index, OperateType opType)
    {
        return status;
    }
    public CommandStatus operateAOI16(AnalogOutputInt16 command, int index, OperateType opType)
    {
        return status;
    }
    public CommandStatus operateAOF32(AnalogOutputFloat32 command, int index, OperateType opType)
    {
        return status;
    }
    public CommandStatus operateAOD64(AnalogOutputDouble64 command, int index, OperateType opType)
    {
        return status;
    }
    
}

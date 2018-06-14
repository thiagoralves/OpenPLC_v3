
//
// Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
// more contributor license agreements. See the NOTICE file distributed
// with this work for additional information regarding copyright ownership.
// Green Energy Corp licenses this file to you under the Apache License,
// Version 2.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This file was forked on 01/01/2013 by Automatak, LLC and modifications
// have been made to this file. Automatak, LLC licenses these modifications to
// you under the terms of the License.
//

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// Outstation application code implements this interface to handle command requests from a master
    /// </summary>
    public interface ICommandHandler
    {
        /// <summary>
        /// Called before any other method calls
        /// </summary>
        void Start();

        /// <summary>
        /// Called after all other method calls
        /// </summary>
        void End();

        /// <summary>
        /// Query application if it supports a ControlRelayOutputBlock (Group12Var1)
        /// </summary>
        /// <param name="command">Command to operate</param>
        /// <param name="index">index of the command</param>        
        /// <returns>result of the operation as a CommandStatus enumeration</returns>
        CommandStatus Select(ControlRelayOutputBlock command, System.UInt16 index);

        /// <summary>
        /// Select a 32-bit analog output (Group 41 Var1)
        /// </summary>
        /// <param name="command">Command to operate</param>
        /// <param name="index">index of the command</param>        
        /// <returns>result of the operation as a CommandStatus enumeration</returns>
        CommandStatus Select(AnalogOutputInt32 command, System.UInt16 index);
        
        /// <summary>
        /// Select a 16-bit analog output (Group 41 Var2)
        /// </summary>
        /// <param name="command">Command to operate</param>
        /// <param name="index">index of the command</param>        
        /// <returns>result of the operation as a CommandStatus enumeration</returns>
        CommandStatus Select(AnalogOutputInt16 command, System.UInt16 index);

        /// <summary>
        /// Select a single precision analog output (Group 41 Var3)
        /// </summary>
        /// <param name="command">Command to operate</param>
        /// <param name="index">index of the command</param>        
        /// <returns>result of the operation as a CommandStatus enumeration</returns>
        CommandStatus Select(AnalogOutputFloat32 command, System.UInt16 index);

        /// <summary>
        /// Select a double precision analog output (Group 41 Var4)
        /// </summary>
        /// <param name="command">Command to operate</param>
        /// <param name="index">index of the command</param>        
        /// <returns>result of the operation as a CommandStatus enumeration</returns>
        CommandStatus Select(AnalogOutputDouble64 command, System.UInt16 index);

        /// <summary>
        /// Operate a ControlRelayOutputBlock (Group12Var1)
        /// </summary>
        /// <param name="command">Command to operate</param>
        /// <param name="index">index of the command</param>
        /// <param name="opType">The type of the operation (SBO, DO, DONoAck) </param>
        /// <returns>result of the operation as a CommandStatus enumeration</returns>
        CommandStatus Operate(ControlRelayOutputBlock command, System.UInt16 index, OperateType opType);
        
        /// <summary>
        /// Operate a 32-bit analog output (Group 41 Var1)
        /// </summary>
        /// <param name="command">Command to operate</param>
        /// <param name="index">index of the command</param>        
        /// <param name="opType">The type of the operation (SBO, DO, DONoAck) </param>
        /// <returns>result of the operation as a CommandStatus enumeration</returns>
        CommandStatus Operate(AnalogOutputInt32 command, System.UInt16 index, OperateType opType);
        
        /// <summary>
        /// Operate a 16-bit analog output (Group 41 Var2)
        /// </summary>
        /// <param name="command">Command to operate</param>
        /// <param name="index">index of the command</param>        
        /// <param name="opType">The type of the operation (SBO, DO, DONoAck) </param>
        /// <returns>result of the operation as a CommandStatus enumeration</returns>
        CommandStatus Operate(AnalogOutputInt16 command, System.UInt16 index, OperateType opType);

        /// <summary>
        /// Operate a single precision analog output (Group 41 Var3)
        /// </summary>
        /// <param name="command">Command to operate</param>
        /// <param name="index">index of the command</param>
        /// <param name="opType">The type of the operation (SBO, DO, DONoAck) </param>
        /// <returns>result of the operation as a CommandStatus enumeration</returns>
        CommandStatus Operate(AnalogOutputFloat32 command, System.UInt16 index, OperateType opType);

        /// <summary>
        /// Operate a double precision analog output (Group 41 Var4)
        /// </summary>
        /// <param name="command">Command to operate</param>
        /// <param name="index">index of the command</param>        
        /// <param name="opType">The type of the operation (SBO, DO, DONoAck) </param>
        /// <returns>result of the operation as a CommandStatus enumeration</returns>
        CommandStatus Operate(AnalogOutputDouble64 command, System.UInt16 index, OperateType opType);
     
    }
}

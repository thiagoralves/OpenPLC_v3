
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
using System.Threading.Tasks;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// Singeton that rejects all commands
    /// </summary>
    public class RejectingCommandHandler : SimpleCommandHandler
    {
        private static readonly ICommandHandler instance = new RejectingCommandHandler();

        private RejectingCommandHandler() : base(() => CommandStatus.NOT_SUPPORTED)
        {}

        public static ICommandHandler Instance
        {
            get
            {
                return instance;
            }
        }
    }

    /// <summary>
    /// Handles all commands by calling a function
    /// </summary>
    public class SimpleCommandHandler : ICommandHandler
    {
        private readonly Func<CommandStatus> status;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="status">Function to call for return value</param>
        public SimpleCommandHandler(Func<CommandStatus> status)
        {
            this.status = status;
        }

        CommandStatus ICommandHandler.Select(ControlRelayOutputBlock command, ushort index)
        {
            return status();
        }

        CommandStatus ICommandHandler.Select(AnalogOutputInt32 command, ushort index)
        {
            return status();
        }

        CommandStatus ICommandHandler.Select(AnalogOutputInt16 command, ushort index)
        {
            return status();
        }

        CommandStatus ICommandHandler.Select(AnalogOutputFloat32 command, ushort index)
        {
            return status();
        }

        CommandStatus ICommandHandler.Select(AnalogOutputDouble64 command, ushort index)
        {
            return status();
        }

        CommandStatus ICommandHandler.Operate(ControlRelayOutputBlock command, ushort index, OperateType opType)
        {
            return status();
        }

        CommandStatus ICommandHandler.Operate(AnalogOutputInt32 command, ushort index, OperateType opType)
        {
            return status();
        }

        CommandStatus ICommandHandler.Operate(AnalogOutputInt16 command, ushort index, OperateType opType)
        {
            return status();
        }

        CommandStatus ICommandHandler.Operate(AnalogOutputFloat32 command, ushort index, OperateType opType)
        {
            return status();
        }

        CommandStatus ICommandHandler.Operate(AnalogOutputDouble64 command, ushort index, OperateType opType)
        {
            return status();
        }

        void ICommandHandler.Start()
        {
           
        }

        void ICommandHandler.End()
        {
           
        }
    }
}

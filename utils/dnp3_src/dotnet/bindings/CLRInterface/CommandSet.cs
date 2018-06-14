
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
using System.Threading.Tasks;
using System.Linq;
using System.Text;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// An interface that can have any command type collection added to it
    /// </summary>
    public interface ICommandBuilder
    {
        void Add(IEnumerable<IndexedValue<ControlRelayOutputBlock>> commands);
        void Add(IEnumerable<IndexedValue<AnalogOutputInt16>> commands);
        void Add(IEnumerable<IndexedValue<AnalogOutputInt32>> commands);
        void Add(IEnumerable<IndexedValue<AnalogOutputFloat32>> commands);
        void Add(IEnumerable<IndexedValue<AnalogOutputDouble64>> commands);
    };

    /// <summary>
    /// A header is something that can make a single callback to a builder
    /// </summary>
    public interface ICommandHeaders
    {
        void Build(ICommandBuilder builder);
    };

    /// <summary>
    /// Concrete implementation of ICommandHeader
    /// </summary>
    public class CommandHeader : ICommandHeaders
    {
        readonly Action<ICommandBuilder> action;

        private CommandHeader(Action<ICommandBuilder> action)
        {
            this.action = action;
        }

        public static ICommandHeaders From(params IndexedValue<ControlRelayOutputBlock>[] commands)
        {
            return new CommandHeader((ICommandBuilder builder) => builder.Add(commands));
        }

        public static ICommandHeaders From(params IndexedValue<AnalogOutputInt16>[] commands)
        {
            return new CommandHeader((ICommandBuilder builder) => builder.Add(commands));
        }

        public static ICommandHeaders From(params IndexedValue<AnalogOutputInt32>[] commands)
        {
            return new CommandHeader((ICommandBuilder builder) => builder.Add(commands));
        }

        public static ICommandHeaders From(params IndexedValue<AnalogOutputFloat32>[] commands)
        {
            return new CommandHeader((ICommandBuilder builder) => builder.Add(commands));
        }

        public static ICommandHeaders From(params IndexedValue<AnalogOutputDouble64>[] commands)
        {
            return new CommandHeader((ICommandBuilder builder) => builder.Add(commands));
        }

        void ICommandHeaders.Build(ICommandBuilder builder)
        {
            action(builder);
        }
    };

    /// <summary>
    /// A command set collects headers and can replay their values back to a builder
    /// </summary>
    public class CommandSet : ICommandHeaders
    {
        readonly ICommandHeaders[] headers;

        private CommandSet(params ICommandHeaders[] headers)
        {
            this.headers = headers;
        }

        public static ICommandHeaders From(params ICommandHeaders[] headers)
        {
            return new CommandSet(headers);
        }

        void ICommandHeaders.Build(ICommandBuilder builder)
        {
            foreach (var header in headers)
            {
                header.Build(builder);
            }
        }
    }
}

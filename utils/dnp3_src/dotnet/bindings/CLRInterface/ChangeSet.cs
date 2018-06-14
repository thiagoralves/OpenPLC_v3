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
    /// A set of measurement changes that can be applied to a database
    /// </summary>
    public interface IChangeSet
    {
        /// <summary>
        /// Apply the change set
        /// </summary>
        /// <param name="database">The database to which to apply the changes</param>
        void Apply(IDatabase database);        
    }

    /// <summary>
    /// Concrete implementation of IChangeSet
    /// </summary>
    public class ChangeSet : IDatabase, IChangeSet
    {
        private IList<Action<IDatabase>> updates = new List<Action<IDatabase>>();

        void IChangeSet.Apply(IDatabase database)
        {
            foreach(var action in updates) 
            {
                action(database);
            }
        }        

        public void Clear()
        {
            updates.Clear();
        }

        public bool IsEmpty()
        {
            return updates.Count == 0;
        }

        public int Count
        {
            get
            {
                return updates.Count;
            }
        }

        public void Update(Binary update, ushort index, EventMode mode = EventMode.Detect)
        {
            updates.Add((IDatabase db) => db.Update(update, index, mode));
        }

        public void Update(DoubleBitBinary update, ushort index, EventMode mode = EventMode.Detect)
        {
            updates.Add((IDatabase db) => db.Update(update, index, mode));
        }

        public void Update(Analog update, ushort index, EventMode mode = EventMode.Detect)
        {
            updates.Add((IDatabase db) => db.Update(update, index, mode));
        }

        public void Update(Counter update, ushort index, EventMode mode = EventMode.Detect)
        {
            updates.Add((IDatabase db) => db.Update(update, index, mode));
        }

        public void Update(FrozenCounter update, ushort index, EventMode mode = EventMode.Detect)
        {
            updates.Add((IDatabase db) => db.Update(update, index, mode));
        }

        public void Update(BinaryOutputStatus update, ushort index, EventMode mode = EventMode.Detect)
        {
            updates.Add((IDatabase db) => db.Update(update, index, mode));
        }

        public void Update(AnalogOutputStatus update, ushort index, EventMode mode = EventMode.Detect)
        {
            updates.Add((IDatabase db) => db.Update(update, index, mode));
        }

        public void Update(TimeAndInterval update, ushort index)
        {
            updates.Add((IDatabase db) => db.Update(update, index));
        }
        
    }
}

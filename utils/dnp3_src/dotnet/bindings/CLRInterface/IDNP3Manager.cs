
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
using System.Runtime.CompilerServices;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// Root interface used for all dnp3 applications
    /// </summary>
    public interface IDNP3Manager
    {
        /// <summary>
        /// Force a blocking shutdown. All channels and stacks are invalidated.
        /// </summary>
        void Shutdown();        

		/// <summary>
        /// Add a TCP client channel. The channel does not try to connect until you add a stack.
		/// </summary>
        /// <param name="id">An id used for logging purposes</param>
        /// <param name="filters">The starting level for logging output</param>
        /// <param name="retry">Retry configuration for the channel</param>
        /// <param name="address">The address of remote host as a INET address i.e. "127.0.0.1" or name "www.google.com"</param>
        /// <param name="port">The port to make the connection on</param>
        /// <param name="listener">Callback interface for the channel</param>
        /// <returns>A channel interface</returns>
        IChannel AddTCPClient(String id, UInt32 filters, ChannelRetry retry, String address, UInt16 port, IChannelListener listener);

        /// <summary>
        /// Add a TCP server channel. The channel does not try to listen until you add a stack.
        /// </summary>
        /// <param name="id">An id used for logging purposes</param>
        /// <param name="filters">The starting level for logging output</param>
        /// <param name="retry">Retry configuration for the channel</param>
        /// <param name="endpoint">The address that identifies the network adapter to bind i.e. "127.0.0.1" or "0.0.0.0"</param>
        /// <param name="port">The port to listen on</param>
        /// <param name="listener">Callback interface for the channel</param>
        /// <returns>A channel interface</returns>
        IChannel AddTCPServer(String id, UInt32 filters, ChannelRetry retry, String endpoint, UInt16 port, IChannelListener listener);

        /// <summary>
        /// Add a TLS client channel. The channel does not try to connect until you add a stack.
        /// </summary>
        /// 
        /// <exception cref="System.ArgumentException">If the TLS certificate/key information is invalid</exception>
        /// 
        /// <param name="id">An id used for logging purposes</param>
        /// <param name="filters">The starting level for logging output</param>
        /// <param name="retry">Retry configuration for the channel</param>
        /// <param name="address">The address of remote host as a INET address i.e. "127.0.0.1" or name "www.google.com"</param>
        /// <param name="port">The port to make the connection on</param>
        /// <param name="config">TLS configuration information</param>
        /// <param name="listener">Callback interface for the channel</param>
        /// <returns>A channel interface</returns>
        IChannel AddTLSClient(String id, UInt32 filters, ChannelRetry retry, String address, UInt16 port, TLSConfig config, IChannelListener listener);

        /// <summary>
        /// Add a TLS server channel. The channel does not try to listen until you add a stack.
        /// </summary>
        /// 
        /// <exception cref="System.ArgumentException">If the TLS certificate/key information is invalid</exception>
        /// 
        /// <param name="id">An id used for logging purposes</param>
        /// <param name="filters">The starting level for logging output</param>
        /// <param name="retry">Retry configuration for the channel</param>
        /// <param name="endpoint">The address that identifies the network adapter to bind i.e. "127.0.0.1" or "0.0.0.0"</param>
        /// <param name="port">The port to listen on</param>
        /// <param name="config">TLS configuration information</param>
        /// <param name="listener">Callback interface for the channel</param>
        /// <returns>A channel interface</returns>
        IChannel AddTLSServer(String id, UInt32 filters, ChannelRetry retry, String endpoint, UInt16 port, TLSConfig config, IChannelListener listener);

        /// <summary>
        /// Add a serial channel. The port does not try to open until you add a stack.
        /// </summary>
        /// <param name="id">An id used for logging purposes</param>
        /// <param name="filters">The starting level for logging output</param>
        /// <param name="retry">Retry configuration for the channel</param>
        /// <param name="settings">Configuration struct for the serial port</param>
        /// <param name="listener">Callback interface for the channel</param>
        /// <returns>A channel interface</returns>
        IChannel AddSerial(String id, UInt32 filters, ChannelRetry retry, SerialSettings settings, IChannelListener listener);

        /// <summary>
        /// Create a new TCP listener an bind it to the specified callback interface
        /// </summary>
        /// <param name="loggerid"></param>
        /// <param name="filters"></param>
        /// <param name="endpoint"></param>
        /// <param name="callbacks"></param>
        /// <returns>A listener which can be canceled</returns>        
        IListener CreateListener(string loggerid, UInt32 filters, IPEndpoint endpoint, IListenCallbacks callbacks);

        /// <summary>
        /// Create a new TLS listener an bind it to the specified callback interface
        /// </summary>
        /// <param name="loggerid"></param>
        /// <param name="filters"></param>
        /// <param name="endpoint"></param>
        /// <param name="config"></param>
        /// <param name="callbacks"></param>
        /// <returns>A listener which can be canceled</returns>        
        IListener CreateListener(string loggerid, UInt32 filters, IPEndpoint endpoint, TLSConfig config, IListenCallbacks callbacks);
    }
}

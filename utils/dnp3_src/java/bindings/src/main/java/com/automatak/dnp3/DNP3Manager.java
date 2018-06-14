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

/**
 * The main entry point for all dnp3 applications. Use this object to create communication channels to which
 * you can then add masters and outstations.
 *
 * To obtain a concrete implementation of this interface, use the DNP3ManagerFactory class in the impl package.
 *
 */
public interface DNP3Manager {

    /**
     * Add a TCP client channel. The channel does not try to connect until you add a stack.
     * @param id An id used for logging purposes
     * @param levels The starting level for logging output
     * @param retry Retry configuration for the channel
     * @param address The address of remote host as a INET address i.e. "127.0.0.1" or name "www.google.com"
     * @param adapter The local adapter to use. "0.0.0.0* means "any adapter'.
     * @param port The port to make the connection on. Note that only the range 0 to 65535 is valid
     * @param listener Optional listener (can be null) for monitoring the state of the channel
     * @return A channel interface
     * @throws DNP3Exception if an error occurs creating the channel
     */
    Channel addTCPClient(String id, int levels, ChannelRetry retry, String address, String adapter, int port, ChannelListener listener) throws DNP3Exception;

    /**
     * Add a TCP client channel. The channel does not try to connect until you add a stack.
     * @param id An id used for logging purposes
     * @param levels The starting level for logging output
     * @param retry Retry configuration for the channel
     * @param endpoint TThe address that identifies the network adapter to bind i.e. "127.0.0.1" or "0.0.0.0"
     * @param port The port to make the connection on. Note that only the range 0 to 65535 is valid
     * @param listener Optional listener (can be null) for monitoring the state of the channel
     * @return A channel interface
     * @throws DNP3Exception if an error occurs creating the channel
     */
    Channel addTCPServer(String id, int levels, ChannelRetry retry, String endpoint, int port, ChannelListener listener) throws DNP3Exception;

    /**
     * Add a TCP client channel. The channel does not try to connect until you add a stack.
     * @param id An id used for logging purposes
     * @param levels The starting level for logging output
     * @param retry Retry configuration for the channel
     * @param address The address of remote host as a INET address i.e. "127.0.0.1" or name "www.google.com"
     * @param adapter The local adapter to use. "0.0.0.0* means "any adapter'.
     * @param port The port to make the connection on. Note that only the range 0 to 65535 is valid
     * @param config TLS configuration
     * @param listener Optional listener (can be null) for monitoring the state of the channel
     * @return A channel interface
     * @throws DNP3Exception if an error occurs creating the channel
     */
    Channel addTLSClient(String id, int levels, ChannelRetry retry, String address, String adapter, int port, TLSConfig config, ChannelListener listener) throws DNP3Exception;

    /**
     * Add a TCP client channel. The channel does not try to connect until you add a stack.
     * @param id An id used for logging purposes
     * @param levels The starting level for logging output
     * @param retry Retry configuration for the channel
     * @param endpoint TThe address that identifies the network adapter to bind i.e. "127.0.0.1" or "0.0.0.0"
     * @param port The port to make the connection on. Note that only the range 0 to 65535 is valid
     * @param config TLS configuration
     * @param listener Optional listener (can be null) for monitoring the state of the channel
     * @return A channel interface
     * @throws DNP3Exception if an error occurs creating the channel
     */
    Channel addTLSServer(String id, int levels, ChannelRetry retry, String endpoint, int port, TLSConfig config, ChannelListener listener) throws DNP3Exception;

    /**
     * Add a serial channel. The port does not try to open until you add a stack.
     *
     * @param id An id used for logging purposes
     * @param levels The starting level for logging output
     * @param retry Retry configuration for the channel
     * @param settings Configuration for the serial port
     * @param listener Optional listener (can be null) for monitoring the state of the channel
     * @throws DNP3Exception if an error occurs creating the channel
     * @return a channel interface
     */
    Channel addSerial(String id, int levels, ChannelRetry retry, SerialSettings settings, ChannelListener listener) throws DNP3Exception;


    /**
     * Permanently shutdown all channels and any associated stacks
     */
    void shutdown();
}

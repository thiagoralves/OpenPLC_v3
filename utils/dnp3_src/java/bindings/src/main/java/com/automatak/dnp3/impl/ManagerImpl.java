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
package com.automatak.dnp3.impl;

import com.automatak.dnp3.*;

class ManagerImpl implements DNP3Manager {

    private long pointer;

    public ManagerImpl(int concurrency, LogHandler handler)
    {
        this.pointer = create_native_manager(concurrency, handler);
    }

    @Override
    public synchronized void finalize()
    {
        this.shutdown();
    }

    @Override
    public synchronized Channel addTCPClient(String id, int levels, ChannelRetry retry, String address, String adapter, int port, ChannelListener listener) throws DNP3Exception
    {
        if(this.pointer == 0)
        {
            throw new DNP3Exception("Manager has been shutdown");
        }

        long ptr = get_native_channel_tcp_client(this.pointer, id, levels, retry.minRetryDelay.toMillis(), retry.maxRetryDelay.toMillis(), address, adapter, port, listener);

        if(ptr == 0) {
           throw new DNP3Exception("Unable to create channel");
        }

       return new ChannelImpl(ptr);
    }

    @Override
    public synchronized Channel addTCPServer(String id, int levels, ChannelRetry retry, String endpoint, int port, ChannelListener listener) throws DNP3Exception
    {
        if(this.pointer == 0)
        {
            throw new DNP3Exception("Manager has been shutdown");
        }

        long ptr = get_native_channel_tcp_server(this.pointer, id, levels, retry.minRetryDelay.toMillis(), retry.maxRetryDelay.toMillis(), endpoint, port, listener);

        if(ptr == 0) {
            throw new DNP3Exception("Unable to create channel");
        }

        return new ChannelImpl(ptr);
    }

    @Override
    public synchronized Channel addTLSClient(String id, int levels, ChannelRetry retry, String address, String adapter, int port, TLSConfig config, ChannelListener listener) throws DNP3Exception
    {
        if(this.pointer == 0)
        {
            throw new DNP3Exception("Manager has been shutdown");
        }

        long ptr = get_native_channel_tls_client(this.pointer, id, levels, retry.minRetryDelay.toMillis(), retry.maxRetryDelay.toMillis(), address, adapter, port, config, listener);

        if(ptr == 0) {
            throw new DNP3Exception("Unable to create TLS client. Did you compile opendnp3 w/ TLS support?");
        }

        return new ChannelImpl(ptr);
    }

    @Override
    public synchronized Channel addTLSServer(String id, int levels, ChannelRetry retry, String endpoint, int port, TLSConfig config, ChannelListener listener) throws DNP3Exception
    {
        if(this.pointer == 0)
        {
            throw new DNP3Exception("Manager has been shutdown");
        }

        long ptr = get_native_channel_tls_server(this.pointer, id, levels, retry.minRetryDelay.toMillis(), retry.maxRetryDelay.toMillis(), endpoint, port, config, listener);

        if(ptr == 0) {
            throw new DNP3Exception("Unable to create TLS server. Did you compile opendnp3 w/ TLS support?");
        }

        return new ChannelImpl(ptr);
    }

    @Override
    public synchronized Channel addSerial(String id, int levels, ChannelRetry retry, SerialSettings settings, ChannelListener listener) throws DNP3Exception
    {
        if(this.pointer == 0)
        {
            throw new DNP3Exception("Manager has been shutdown");
        }

        long ptr = get_native_channel_serial(
                this.pointer,
                id,
                levels,
                retry.minRetryDelay.toMillis(),
                retry.maxRetryDelay.toMillis(),
                settings.port,
                settings.baudRate,
                settings.dataBits,
                settings.parity.toType(),
                settings.stopBits,
                settings.flowControl.toType(),
                listener
        );

        if(ptr == 0) {
            throw new DNP3Exception("Unable to create channel");
        }

        return new ChannelImpl(ptr);
    }

    @Override
    public synchronized void shutdown()
    {
        if(this.pointer != 0)
        {
            shutdown_native_manager(this.pointer);
            this.pointer = 0;
        }
    }

    private native long create_native_manager(int concurrency, LogHandler handler);
    private native void shutdown_native_manager(long nativePointer);

    private native long get_native_channel_tcp_client(long nativePointer, String id, int level, long minRetryMs, long maxRetryMs, String address, String adapter, int port, ChannelListener listener);
    private native long get_native_channel_tcp_server(long nativePointer, String id, int level, long minRetryMs, long maxRetryMs, String endpoint, int port, ChannelListener listener);
    private native long get_native_channel_tls_client(long nativePointer, String id, int level, long minRetryMs, long maxRetryMs, String address, String adapter, int port, TLSConfig config, ChannelListener listener);
    private native long get_native_channel_tls_server(long nativePointer, String id, int level, long minRetryMs, long maxRetryMs, String endpoint, int port, TLSConfig config, ChannelListener listener);
    private native long get_native_channel_serial(long nativePointer, String id, int level, long minRetryMs, long maxRetryMs, String port, int baudRate, int dataBits, int parity, int stopBits, int flowControl, ChannelListener listener);


}

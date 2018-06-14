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

import com.automatak.dnp3.enums.FlowControl;
import com.automatak.dnp3.enums.Parity;

/**
 * Settings class uses to configure a serial port
 */
public class SerialSettings {

    public final String port;
    public final int baudRate;
    public final int dataBits;
    public final int stopBits;
    public final Parity parity;
    public final FlowControl flowControl;

    /**
     * Constructs a SerialSettings class with a fully configurable set of options
     * @param port        i.e. COMX on windows or /dev/ttyX on Linux
     * @param baudRate    Baud rate for the port. Allowed values depend on the specific port
     * @param dataBits    Number of data bits. Typically 8.
     * @param parity      Parity, typically none
     * @param stopBits    Stop bits, typically 1
     * @param flowControl Flow control, typically none
     */
    public SerialSettings(String port, int baudRate, int dataBits, Parity parity, int stopBits, FlowControl flowControl)
    {
        this.port = port;
        this.baudRate = baudRate;
        this.dataBits = dataBits;
        this.stopBits = stopBits;
        this.parity = parity;
        this.flowControl = flowControl;
    }

    /**
     * Constructs the SerialSettings class with the typical 8/N/1 settings
     * @param port       i.e. COMX on windows or /dev/ttyX on Linux
     * @param baudRate   Baud rate for the port. Allowed values depend on the specific port
     */
    public SerialSettings(String port, int baudRate)
    {
        this(port, baudRate, 8, Parity.None, 1, FlowControl.None);
    }



}

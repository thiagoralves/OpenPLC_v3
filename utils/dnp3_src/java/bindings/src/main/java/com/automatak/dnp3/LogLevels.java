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

public final class LogLevels
{
    public static final int EVENT = 1;
    public static final int ERROR = 1 << 1;
    public static final int WARNING = 1 << 2;
    public static final int INFO = 1 << 3;
    public static final int DEBUG = 1 << 4;

    public static final int LINK_RX = DEBUG << 1;
    public static final int LINK_RX_HEX = DEBUG << 2;

    public static final int LINK_TX = DEBUG << 3;
    public static final int LINK_TX_HEX = DEBUG << 4;

    public static final int TRANSPORT_RX = DEBUG << 5;
    public static final int TRANSPORT_TX = DEBUG << 6;

    public static final int APP_HEADER_RX = DEBUG << 7;
    public static final int APP_HEADER_TX = DEBUG << 8;

    public static final int APP_OBJECT_RX = DEBUG << 9;
    public static final int APP_OBJECT_TX = DEBUG << 10;

    public static final int APP_HEX_RX = DEBUG << 11;
    public static final int APP_HEX_TX = DEBUG << 12;

    public static String getFilterString(int filters)
    {
        switch (filters)
        {
            case(EVENT):
                return "EVENT";
            case(ERROR):
                return "ERROR";
            case(WARNING):
                return "WARN";
            case(INFO):
                return "INFO";
            case(DEBUG):
                return "DEBUG";
            case(LINK_RX):
            case (LINK_RX_HEX):
                return "<-LL-";
            case (LINK_TX):
            case (LINK_TX_HEX):
                return "-LL->";
            case (TRANSPORT_RX):
                return "<-TL-";
            case (TRANSPORT_TX):
                return "-TL->";

            case(APP_HEX_RX):
            case(APP_HEADER_RX):
            case(APP_OBJECT_RX):
                return "<-AL-";

            case (APP_HEX_TX):
            case (APP_HEADER_TX):
            case (APP_OBJECT_TX):
                return "-AL->";
            default:
                return "?";
        }
    }
}


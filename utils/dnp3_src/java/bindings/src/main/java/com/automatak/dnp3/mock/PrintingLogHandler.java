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

import com.automatak.dnp3.LogEntry;
import com.automatak.dnp3.LogHandler;
import com.automatak.dnp3.LogLevels;

/**
 * Singleton that prints log information as it is received
 */
public class PrintingLogHandler implements LogHandler {

    private static PrintingLogHandler instance = new PrintingLogHandler();

    @Override
    public void log(LogEntry entry)
    {
         System.out.println(System.currentTimeMillis() + " - " + getLevelString(entry.level) + " - " + entry.id + " - " + entry.message);

    }

    public static PrintingLogHandler getInstance() {
        return instance;
    }

    private PrintingLogHandler() {
    }

    public String getLevelString(int level)
    {
        switch(level)
        {
            case(LogLevels.EVENT):
                return "EVENT";
            case(LogLevels.ERROR):
                return "ERROR";
            case(LogLevels.WARNING):
                return "WARN";
            case(LogLevels.INFO):
                return "INFO";
            case(LogLevels.DEBUG):
                return "DEBUG";
            case(LogLevels.LINK_RX):
            case(LogLevels.LINK_RX_HEX):
                return "<-LL-";
            case(LogLevels.LINK_TX):
            case(LogLevels.LINK_TX_HEX):
                return "-LL->";
            case(LogLevels.TRANSPORT_RX):
                return "<-TL-";
            case(LogLevels.TRANSPORT_TX):
                return "-TL->";

            case(LogLevels.APP_HEX_RX):
            case(LogLevels.APP_HEADER_RX):
            case(LogLevels.APP_OBJECT_RX):
                return "<-AL-";

            case (LogLevels.APP_HEX_TX):
            case (LogLevels.APP_HEADER_TX):
            case (LogLevels.APP_OBJECT_TX):
                return "-AL->";

            default:
                return "?";
        }
    }
}

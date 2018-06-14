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

import com.automatak.dnp3.*;
import com.automatak.dnp3.enums.AssignClassType;
import com.automatak.dnp3.enums.LinkStatus;
import com.automatak.dnp3.enums.MasterTaskType;
import com.automatak.dnp3.enums.PointClass;

import java.util.Collections;

public class DefaultOutstationApplication implements OutstationApplication
{
    private static DefaultOutstationApplication instance = new DefaultOutstationApplication();

    public static OutstationApplication getInstance() {
        return instance;
    }

    private DefaultOutstationApplication() {}

    @Override
    public boolean supportsWriteAbsoluteTime() { return false; }

    @Override
    public boolean writeAbsoluteTime(long msSinceEpoch) { return false; }

    @Override
    public boolean supportsAssignClass(){ return false; }

    @Override
    public void recordClassAssignment(AssignClassType type, PointClass clazz, int start, int stop) {}

    @Override
    public ApplicationIIN getApplicationIIN() { return ApplicationIIN.none(); }

    @Override
    public void onStateChange(LinkStatus value) {}

    @Override
    public void onKeepAliveInitiated() {}

    @Override
    public void onKeepAliveFailure(){}

    @Override
    public void onKeepAliveSuccess(){}


}

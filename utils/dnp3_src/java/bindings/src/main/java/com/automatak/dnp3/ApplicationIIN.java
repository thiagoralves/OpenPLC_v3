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
 * Represents the set of IIN bits that can be controlled by the application
 */
public class ApplicationIIN
{
    public ApplicationIIN(
            boolean needTime,
            boolean localControl,
            boolean deviceTrouble,
            boolean configCorrupt)
    {
        this.needTime = needTime;
        this.localControl = localControl;
        this.deviceTrouble = deviceTrouble;
        this.configCorrupt =  configCorrupt;
    }

    public static ApplicationIIN none(){
        return new ApplicationIIN(false, false, false, false);
    }

    public final boolean needTime;
    public final boolean localControl;
    public final boolean deviceTrouble;
    public final boolean configCorrupt;
}
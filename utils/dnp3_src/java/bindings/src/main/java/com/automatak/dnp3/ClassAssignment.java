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

import com.automatak.dnp3.enums.PointClass;

public class ClassAssignment {

    public ClassAssignment(byte group, byte variation, PointClass clazz, Range range) {
        this.group = group;
        this.variation = variation;
        this.clazz = clazz;
        this.range = range;
    }

    public final byte group;
    public final byte variation;
    public final PointClass clazz;

    // if range is full, this implies 0x06
    public final Range range;

}

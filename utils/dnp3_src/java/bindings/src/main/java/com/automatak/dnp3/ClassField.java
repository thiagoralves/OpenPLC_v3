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

/**
 * Structure that records which events are scanned / evented
 */
public class ClassField {

    public int bitfield;

    private ClassField(int mask) {
        this.bitfield = mask;
    }

    public static ClassField from(PointClass... classes) {
        byte mask = 0;
        for (PointClass pc : classes) {
            mask |= pc.toType();
        }
        return new ClassField(mask);
    }

    public boolean isSet(PointClass pc)
    {
        return (pc.toType() & bitfield) != 0;
    }

    public void set(PointClass pc, boolean value)
    {
        if(value)
        {
            this.bitfield |= pc.toType();
        }
        else
        {
            this.bitfield &= ~pc.toType();
        }
    }

    public static ClassField none() {
        return new ClassField(0);
    }

    public static ClassField allClasses() {
        return new ClassField(PointClassMasks.ALL_CLASSES);
    }

    public static ClassField allEventClasses() {
        return new ClassField(PointClassMasks.ALL_EVENTS);
    }

}

/*
 * Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
 * more contributor license agreements. See the NOTICE file distributed
 * with this work for additional information regarding copyright ownership.
 * Green Energy Corp licenses this file to you under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in
 * compliance with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project was forked on 01/01/2013 by Automatak, LLC and modifications
 * may have been made to this file. Automatak, LLC licenses these modifications
 * to you under the terms of the License.
 */
#ifndef OPENDNP3_CELL_H
#define OPENDNP3_CELL_H

namespace opendnp3
{

/**
* Type used to record whether a value is requested in a response
*/
template <class Spec>
struct SelectedValue
{
	SelectedValue() : selected(false), value(), variation(Spec::DefaultStaticVariation)
	{}

	bool selected;
	typename Spec::meas_t value;
	typename Spec::static_variation_t variation;
};

/**
* Holds particular measurement type in the database.
*/
template <class Spec>
struct Cell
{
	typename Spec::meas_t value;			// current value
	typename Spec::config_t config;			// configuration
	typename Spec::event_cell_t event;		// event cell
	SelectedValue<Spec> selection;			// selected value

};





}

#endif

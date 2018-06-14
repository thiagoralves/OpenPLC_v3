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
#ifndef OPENPAL_FINALLY_H
#define OPENPAL_FINALLY_H

#include "Uncopyable.h"

namespace openpal
{

template <typename Cleanup>
class RAII;

template <typename Cleanup>
RAII<Cleanup> Finally(const Cleanup&);

/** Finally is an RAII helper that takes a functor to run when it destructs
*/
template <class Cleanup>
class RAII
{
public:
	friend RAII Finally<Cleanup>(const Cleanup& fun);

	~RAII()
	{
		fun();
	}

private:

	RAII() = delete;

	RAII(const Cleanup& fun_) : fun(fun_) {}

	Cleanup fun;
};

template <class Cleanup>
RAII<Cleanup> Finally(const Cleanup& fun)
{
	return RAII<Cleanup>(fun);
}


}

#endif


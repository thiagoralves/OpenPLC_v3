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
#ifndef TESTLIB_RANDOM_H_
#define TESTLIB_RANDOM_H_

#include <random>

namespace testlib
{

template<class T>
class Random
{

public:
	Random(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max()) :
		rng(),
		dist(min, max)
	{

	}

	T Next()
	{
		return dist(rng);
	}

private:
	std::mt19937 rng;
	std::uniform_int_distribution<T> dist;
};

class RandomBool : private Random<uint32_t>
{
public:
	RandomBool() : Random<uint32_t>(0, 1)
	{}

	bool NextBool()
	{
		return Next() ? true : false;
	}
};


}

#endif


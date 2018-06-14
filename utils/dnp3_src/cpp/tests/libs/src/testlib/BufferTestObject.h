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
#ifndef __BUFFER_TEST_OBJECT_H_
#define __BUFFER_TEST_OBJECT_H_

#include <string>
#include <vector>
#include <cstdint>

#include <openpal/container/RSlice.h>

namespace testlib
{

class BufferTestObject
{
	static const size_t MAX_SIZE = 1024 * 1024;

public:
	BufferTestObject();
	~BufferTestObject();

	bool BufferEquals(const openpal::RSlice& arBuffer) const;
	bool BufferEquals(const uint8_t*, size_t) const;
	bool BufferEqualsHex(const std::string& arData) const;
	bool BufferEqualsString(const std::string& arData) const;
	bool BufferContains(const std::string& arData) const;

	std::string GetBufferAsHexString(bool spaced = true) const;

	bool IsBufferEmpty()
	{
		return mBuffer.size() == 0;
	}
	void ClearBuffer();
	size_t Size()
	{
		return mBuffer.size();
	}
	bool SizeEquals(size_t aNum)
	{
		return aNum == Size();
	}
	size_t NumWrites()
	{
		return mNumWrites;
	}

protected:
	void WriteToBuffer(const openpal::RSlice& arBuffer);

private:
	size_t mNumWrites;
	std::vector<uint8_t> mBuffer;
};

}

#endif


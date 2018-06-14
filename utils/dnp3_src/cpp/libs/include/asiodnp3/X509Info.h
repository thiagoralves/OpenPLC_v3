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
#ifndef ASIODNP3_X509_INFO_H
#define ASIODNP3_X509_INFO_H

#include <openpal/container/RSlice.h>
#include <openpal/util/Uncopyable.h>
#include <string>


namespace asiodnp3
{

/**
* Select information from a preverified x509 certificate
* that user can can inspect an optionally reject
*/
class X509Info : private openpal::Uncopyable
{
public:

	X509Info(int depth_, const openpal::RSlice sha1thumbprint_, std::string subjectName_) :
		depth(depth_),
		sha1thumbprint(sha1thumbprint_),
		subjectName(subjectName_)
	{}

	// the depth of the certificate in the chain
	int depth;

	// the sha1 thumbprint
	openpal::RSlice sha1thumbprint;

	// the extracted subject name
	std::string subjectName;

private:

	X509Info();
};

}

#endif

#ifndef __EVENT_CONVERTER_H_
#define __EVENT_CONVERTER_H_

using namespace System::Collections::ObjectModel;

#include <vector>
#include <functional>
#include <vcclr.h>

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{

			template <class T, class U>
			private class EventConverter
			{

			public:


				EventConverter(const std::function<U(T)>& convert_, System::Action<U>^ listener) :
					convert(convert_),
					rootListener(listener)
				{

				}

				std::function<void(T)> GetTrigger()
				{
					auto trigger = [this](T value) { this->OnEvent(value); };
					return trigger;
				}

				/// How to subscribe on the managed side
				void AddListener(System::Action<U>^ listener)
				{
					auto pRoot = new gcroot < System::Action<U> ^ >(listener);
					callbacks.push_back(pRoot);
				}

			private:

				void OnEvent(T value)
				{
					rootListener->Invoke(convert(value));
				}

				std::function<U(T)> convert;
				gcroot < System::Action<U> ^ > rootListener;
			};

		}
	}
}

#endif

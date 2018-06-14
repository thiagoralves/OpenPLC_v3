

#ifndef __OUTSTATION_APPLICATION_ADAPTER_H_
#define __OUTSTATION_APPLICATION_ADAPTER_H_

using namespace System::Collections::ObjectModel;

#include <vcclr.h>

#include <opendnp3/outstation/IOutstationApplication.h>

using namespace Automatak::DNP3::Interface;

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{

			private class OutstationApplicationAdapter : public opendnp3::IOutstationApplication
			{
			public:
				OutstationApplicationAdapter(Automatak::DNP3::Interface::IOutstationApplication^ proxy);

				virtual void OnStateChange(opendnp3::LinkStatus value) override final;

				virtual void OnKeepAliveInitiated() override final;

				virtual void OnKeepAliveFailure() override final;

				virtual void OnKeepAliveSuccess() override final;				

				virtual bool WriteAbsoluteTime(const openpal::UTCTimestamp& timestamp) override final;

				virtual bool SupportsWriteAbsoluteTime() override final;
				
				virtual bool SupportsWriteTimeAndInterval() override final;
				
				virtual bool WriteTimeAndInterval(const opendnp3::ICollection<opendnp3::Indexed<opendnp3::TimeAndInterval>>& values) override final;

				virtual bool SupportsAssignClass() override final;

				virtual void RecordClassAssignment(opendnp3::AssignClassType type, opendnp3::PointClass clazz, uint16_t start, uint16_t stop) override final;

				virtual opendnp3::ApplicationIIN GetApplicationIIN() const override final;

				virtual opendnp3::RestartMode ColdRestartSupport() const override final;

				virtual opendnp3::RestartMode WarmRestartSupport() const override final;

				virtual uint16_t ColdRestart();

				virtual uint16_t WarmRestart();

			private:
				gcroot < Automatak::DNP3::Interface::IOutstationApplication^ > proxy;
			};


		}
	}
}

#endif

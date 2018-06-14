
#include "Conversions.h"

#include <msclr\marshal_cppstd.h>

using namespace Automatak::DNP3::Interface;

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{

			std::string Conversions::ConvertString(System::String^ s)
			{
				return msclr::interop::marshal_as<std::string>(s);
			}

			System::String^ Conversions::ConvertString(const std::string& s)
			{
				return gcnew System::String(s.c_str());
			}

			ChannelState Conversions::ConvertChannelState(opendnp3::ChannelState aState)
			{
				return (ChannelState)aState;
			}

			LinkStatus Conversions::ConvertLinkStatus(opendnp3::LinkStatus aState)
			{
				return (LinkStatus)aState;
			}

			IChannelStatistics^ Conversions::ConvertChannelStats(const opendnp3::LinkStatistics& stats)
			{
				ChannelStatistics^ ret = gcnew ChannelStatistics();
				
				ret->numBytesRx = stats.channel.numBytesRx;
				ret->numBytesTx = stats.channel.numBytesTx;
				ret->numOpen = stats.channel.numOpen;
				ret->numOpenFail = stats.channel.numOpenFail;
				ret->numClose = stats.channel.numClose;
				ret->numLinkFrameRx = stats.parser.numLinkFrameRx;
				ret->numLinkFrameTx = stats.channel.numLinkFrameTx;
				ret->numBadLinkFrameRx = stats.parser.numBadFCB + stats.parser.numBadFCV + stats.parser.numBadFunctionCode + stats.parser.numBadLength;
				ret->numCrcError = stats.parser.numBodyCrcError + stats.parser.numHeaderCrcError;

				return ret;
			}

			IStackStatistics^ Conversions::ConvertStackStats(const opendnp3::StackStatistics& statistics)
			{
				StackStatistics^ ret = gcnew StackStatistics();
				
				ret->numTransportErrorRx = statistics.transport.rx.numTransportErrorRx;
				ret->numTransportRx = statistics.transport.rx.numTransportRx;
				ret->numTransportTx = statistics.transport.tx.numTransportTx;
				
				return ret;
			}

			CommandTaskResult^ Conversions::ConvertCommandTaskResult(const opendnp3::ICommandTaskResult& response)
			{
				auto convert = [](const opendnp3::CommandPointResult& value) -> CommandPointResult^
				{
					return gcnew CommandPointResult(value.headerIndex, value.index, (CommandPointState)value.state, (CommandStatus)value.status);
				};

				auto adapter = CreateAdapter<opendnp3::CommandPointResult, CommandPointResult^>(convert);

				response.Foreach(adapter);

				return gcnew CommandTaskResult((TaskCompletion)response.summary, adapter.GetValues());
			}

			CommandStatus Conversions::ConvertCommandStatus(opendnp3::CommandStatus status)
			{
				return (CommandStatus)status;
			}

			opendnp3::CommandStatus Conversions::ConvertCommandStatus(CommandStatus status)
			{
				return (opendnp3::CommandStatus) status;
			}

			ControlCode Conversions::ConvertControlCode(opendnp3::ControlCode code)
			{
				return (ControlCode)ControlCodeToType(code);
			}

			opendnp3::ControlCode Conversions::ConvertControlCode(ControlCode code)
			{
				return (opendnp3::ControlCode) code;
			}

			ControlRelayOutputBlock^ Conversions::ConvertCommand(const opendnp3::ControlRelayOutputBlock& bo)
			{
				return gcnew ControlRelayOutputBlock(ConvertControlCode(bo.functionCode), bo.count, bo.onTimeMS, bo.offTimeMS);
			}

			opendnp3::ControlRelayOutputBlock Conversions::ConvertCommand(ControlRelayOutputBlock^ bo)
			{
				return opendnp3::ControlRelayOutputBlock(ConvertControlCode(bo->code), bo->count, bo->onTime, bo->offTime);
			}

			opendnp3::AnalogOutputInt32 Conversions::ConvertCommand(AnalogOutputInt32^ sp)
			{
				return opendnp3::AnalogOutputInt32(sp->value);
			}

			AnalogOutputInt32^ Conversions::ConvertCommand(const opendnp3::AnalogOutputInt32& sp)
			{
				return gcnew AnalogOutputInt32(sp.value);
			}

			opendnp3::AnalogOutputInt16 Conversions::ConvertCommand(AnalogOutputInt16^ sp)
			{
				return opendnp3::AnalogOutputInt16(sp->value);
			}

			AnalogOutputInt16^ Conversions::ConvertCommand(const opendnp3::AnalogOutputInt16& sp)
			{
				return gcnew AnalogOutputInt16(sp.value);
			}

			opendnp3::AnalogOutputFloat32 Conversions::ConvertCommand(AnalogOutputFloat32^ sp)
			{
				return opendnp3::AnalogOutputFloat32(sp->value);
			}

			AnalogOutputFloat32^ Conversions::ConvertCommand(const opendnp3::AnalogOutputFloat32& sp)
			{
				return gcnew AnalogOutputFloat32(sp.value);
			}

			opendnp3::AnalogOutputDouble64 Conversions::ConvertCommand(AnalogOutputDouble64^ sp)
			{
				return opendnp3::AnalogOutputDouble64(sp->value);
			}

			AnalogOutputDouble64^ Conversions::ConvertCommand(const opendnp3::AnalogOutputDouble64& sp)
			{
				return gcnew AnalogOutputDouble64(sp.value);
			}

			Binary^ Conversions::ConvertMeas(opendnp3::Binary meas)
			{
				return gcnew Binary(meas.value, meas.flags.value, TimeStamp::Convert(meas.time));
			}

			DoubleBitBinary^ Conversions::ConvertMeas(opendnp3::DoubleBitBinary meas)
			{
				return gcnew DoubleBitBinary(static_cast<DoubleBit>(meas.value), meas.flags.value, TimeStamp::Convert(meas.time));
			}

			Analog^ Conversions::ConvertMeas(opendnp3::Analog meas)
			{
				return gcnew Analog(meas.value, meas.flags.value, TimeStamp::Convert(meas.time));
			}

			Counter^ Conversions::ConvertMeas(opendnp3::Counter meas)
			{
				return gcnew Counter(meas.value, meas.flags.value, TimeStamp::Convert(meas.time));
			}

			FrozenCounter^ Conversions::ConvertMeas(opendnp3::FrozenCounter meas)
			{
				return gcnew FrozenCounter(meas.value, meas.flags.value, TimeStamp::Convert(meas.time));
			}

			AnalogOutputStatus^ Conversions::ConvertMeas(opendnp3::AnalogOutputStatus meas)
			{
				return gcnew AnalogOutputStatus(meas.value, meas.flags.value, TimeStamp::Convert(meas.time));
			}

			BinaryOutputStatus^ Conversions::ConvertMeas(opendnp3::BinaryOutputStatus meas)
			{
				return gcnew BinaryOutputStatus(meas.value, meas.flags.value, TimeStamp::Convert(meas.time));
			}

			OctetString^ Conversions::ConvertMeas(const opendnp3::OctetString& meas)
			{
				return gcnew OctetString(Conversions::Convert(meas.ToRSlice()));
			}

			TimeAndInterval^ Conversions::ConvertMeas(const opendnp3::TimeAndInterval& meas)
			{
				return gcnew TimeAndInterval(meas.time, meas.interval, meas.units);
			}

			BinaryCommandEvent^ Conversions::ConvertMeas(const opendnp3::BinaryCommandEvent& meas)
			{
				return gcnew BinaryCommandEvent(meas.value, ConvertCommandStatus(meas.status), TimeStamp::Convert(meas.time));
			}

			AnalogCommandEvent^ Conversions::ConvertMeas(const opendnp3::AnalogCommandEvent& meas)
			{
				return gcnew AnalogCommandEvent(meas.value, ConvertCommandStatus(meas.status), TimeStamp::Convert(meas.time));
			}

			SecurityStat^ Conversions::ConvertMeas(const opendnp3::SecurityStat& meas)
			{
				return gcnew SecurityStat(meas.value.count, meas.value.assocId, meas.quality, TimeStamp::Convert(meas.time));
			}

			opendnp3::Binary Conversions::ConvertMeas(Binary^ meas)
			{
				return opendnp3::Binary(meas->Value, meas->Quality, opendnp3::DNPTime(TimeStamp::Convert(meas->Timestamp)));
			}

			opendnp3::DoubleBitBinary Conversions::ConvertMeas(DoubleBitBinary^ meas)
			{
				return opendnp3::DoubleBitBinary((opendnp3::DoubleBit) meas->Value, meas->Quality, opendnp3::DNPTime(TimeStamp::Convert(meas->Timestamp)));
			}

			opendnp3::Analog Conversions::ConvertMeas(Analog^ meas)
			{
				return opendnp3::Analog(meas->Value, meas->Quality, opendnp3::DNPTime(TimeStamp::Convert(meas->Timestamp)));
			}

			opendnp3::Counter Conversions::ConvertMeas(Counter^ meas)
			{
				return opendnp3::Counter(meas->Value, meas->Quality, opendnp3::DNPTime(TimeStamp::Convert(meas->Timestamp)));
			}

			opendnp3::FrozenCounter Conversions::ConvertMeas(FrozenCounter^ meas)
			{
				return opendnp3::FrozenCounter(meas->Value, meas->Quality, opendnp3::DNPTime(TimeStamp::Convert(meas->Timestamp)));
			}

			opendnp3::AnalogOutputStatus Conversions::ConvertMeas(AnalogOutputStatus^ meas)
			{
				return opendnp3::AnalogOutputStatus(meas->Value, meas->Quality, opendnp3::DNPTime(TimeStamp::Convert(meas->Timestamp)));
			}

			opendnp3::TimeAndInterval Conversions::ConvertMeas(TimeAndInterval^ meas)
			{
				return opendnp3::TimeAndInterval(opendnp3::DNPTime(meas->time), meas->interval, meas->units);
			}

			opendnp3::BinaryCommandEvent Conversions::ConvertMeas(BinaryCommandEvent^ meas)
			{
				return opendnp3::BinaryCommandEvent(meas->Value, ConvertCommandStatus(meas->Status), opendnp3::DNPTime(TimeStamp::Convert(meas->Timestamp)));
			}

			opendnp3::AnalogCommandEvent Conversions::ConvertMeas(AnalogCommandEvent^ meas)
			{
				return opendnp3::AnalogCommandEvent(meas->Value, ConvertCommandStatus(meas->Status), opendnp3::DNPTime(TimeStamp::Convert(meas->Timestamp)));
			}

			opendnp3::BinaryOutputStatus Conversions::ConvertMeas(BinaryOutputStatus^ meas)
			{
				return opendnp3::BinaryOutputStatus(meas->Value, meas->Quality, opendnp3::DNPTime(TimeStamp::Convert(meas->Timestamp)));
			}

			LinkHeader^ Conversions::Convert(const opendnp3::LinkHeaderFields& fields)
			{
				return gcnew LinkHeader((LinkFunction)fields.func, fields.isFromMaster, fields.fcb, fields.fcvdfc, fields.dest, fields.src);
			}

			asiopal::IPEndpoint Conversions::Convert(IPEndpoint^ endpoint)
			{
				return asiopal::IPEndpoint(ConvertString(endpoint->address), endpoint->port);
			}

			X509Info^ Conversions::Convert(const asiodnp3::X509Info& info)
			{				
				return gcnew X509Info(
					info.depth,
					Conversions::Convert(info.sha1thumbprint),
					Conversions::ConvertString(info.subjectName)
				);
			}

			asiopal::SerialSettings Conversions::ConvertSerialSettings(SerialSettings^ settings)
			{
				asiopal::SerialSettings s;
				s.deviceName = ConvertString(settings->port);
				s.baud = settings->baud;
				s.dataBits = settings->dataBits;
				s.stopBits = (opendnp3::StopBits) settings->stopBits;
				s.parity = (opendnp3::Parity) settings->parity;
				s.flowType = (opendnp3::FlowControl) settings->flowControl;
				return s;
			}

			openpal::TimeDuration Conversions::ConvertMilliseconds(System::Int64 ms)
			{
				return openpal::TimeDuration::Milliseconds(ms);
			}

			openpal::TimeDuration Conversions::ConvertTimespan(System::TimeSpan ts)
			{
				return ConvertMilliseconds(ts.Ticks / System::TimeSpan::TicksPerMillisecond);
			}		

			System::TimeSpan Conversions::ConvertTimeDuration(const openpal::TimeDuration& duration)
			{
				return System::TimeSpan::FromMilliseconds((double) duration.GetMilliseconds());
			}

			opendnp3::ClassField Conversions::ConvertClassField(ClassField classField)
			{
				return opendnp3::ClassField(classField.ClassMask);
			}
			
			asiopal::TLSConfig Conversions::Convert(TLSConfig^ config)
			{				
				return asiopal::TLSConfig(
					Conversions::ConvertString(config->peerCertFilePath),
					Conversions::ConvertString(config->localCertFilePath),
					Conversions::ConvertString(config->privateKeyFilePath),
					config->maxVerifyDepth,
					config->allowTLSv10,
					config->allowTLSv11,
					config->allowTLSv12,
					Conversions::ConvertString(config->cipherList)
				);
			}

			opendnp3::LinkConfig Conversions::ConvertConfig(LinkConfig^ config)
			{
				return opendnp3::LinkConfig(
					config->isMaster,
					config->useConfirms,
					config->numRetry,
					config->localAddr,
					config->remoteAddr,
					ConvertTimespan(config->responseTimeout),
					ConvertTimespan(config->keepAliveTimeout)
				);
			}

			opendnp3::EventBufferConfig Conversions::ConvertConfig(EventBufferConfig^ cm)
			{
				opendnp3::EventBufferConfig config;
				config.maxBinaryEvents = cm->maxBinaryEvents;
				config.maxDoubleBinaryEvents = cm->maxDoubleBinaryEvents;
				config.maxCounterEvents = cm->maxCounterEvents;
				config.maxFrozenCounterEvents = cm->maxFrozenCounterEvents;
				config.maxAnalogEvents = cm->maxAnalogEvents;
				config.maxBinaryOutputStatusEvents = cm->maxBinaryOutputStatusEvents;
				config.maxAnalogOutputStatusEvents = cm->maxAnalogOutputStatusEvents;
				return config;
			}

			opendnp3::OutstationConfig Conversions::ConvertConfig(OutstationConfig^ config, opendnp3::IndexMode indexMode)
			{
				opendnp3::OutstationConfig oc;				

				oc.params = ConvertConfig(config->config, indexMode);
				oc.eventBufferConfig = ConvertConfig(config->buffer);				

				return oc;
			}			

			opendnp3::OutstationParams Conversions::ConvertConfig(OutstationParams^ config, opendnp3::IndexMode indexMode)
			{
				opendnp3::OutstationParams params;

				params.indexMode = indexMode;
				params.allowUnsolicited = config->allowUnsolicited;
				params.typesAllowedInClass0 = opendnp3::StaticTypeBitField(config->typesAllowedInClass0.mask);
				params.maxControlsPerRequest = config->maxControlsPerRequest;
				params.maxTxFragSize = config->maxTxFragSize;
				params.selectTimeout = ConvertTimespan(config->selectTimeout);
				params.solConfirmTimeout = ConvertTimespan(config->solicitedConfirmTimeout);
				params.unsolClassMask = ConvertClassField(config->unsolClassMask);
				params.unsolConfirmTimeout = ConvertTimespan(config->unsolicitedConfirmTimeout);
				params.unsolRetryTimeout = ConvertTimespan(config->unsolicitedRetryPeriod);
				
				return params;
			}

			opendnp3::MasterParams Conversions::ConvertConfig(MasterConfig^ config)
			{
				opendnp3::MasterParams mp;

				mp.disableUnsolOnStartup = config->disableUnsolOnStartup;
				mp.integrityOnEventOverflowIIN = config->integrityOnEventOverflowIIN;
				mp.responseTimeout = ConvertTimespan(config->responseTimeout);
				mp.startupIntegrityClassMask = ConvertClassField(config->startupIntegrityClassMask);
				mp.eventScanOnEventsAvailableClassMask = ConvertClassField(config->eventScanOnEventsAvailableClassMask);
				mp.taskRetryPeriod = ConvertTimespan(config->taskRetryPeriod);
				mp.taskStartTimeout = ConvertTimespan(config->taskStartTimeout);
				mp.timeSyncMode = (opendnp3::TimeSyncMode) config->timeSyncMode;
				mp.unsolClassMask = ConvertClassField(config->unsolClassMask);

				return mp;
			}

			asiodnp3::MasterStackConfig Conversions::ConvertConfig(MasterStackConfig^ config)
			{
				asiodnp3::MasterStackConfig cfg;
				cfg.master = ConvertConfig(config->master);
				cfg.link = ConvertConfig(config->link);
				return cfg;
			}

			asiodnp3::OutstationStackConfig Conversions::ConvertConfig(OutstationStackConfig^ config)
			{										
				opendnp3::DatabaseSizes sizes(
					config->databaseTemplate->binaries->Count,
					config->databaseTemplate->doubleBinaries->Count,
					config->databaseTemplate->analogs->Count,
					config->databaseTemplate->counters->Count,
					config->databaseTemplate->frozenCounters->Count,
					config->databaseTemplate->binaryOutputStatii->Count,
					config->databaseTemplate->analogOutputStatii->Count,
					config->databaseTemplate->timeAndIntervals->Count
				);


				asiodnp3::OutstationStackConfig cfg(sizes);

				ApplyConfig(config->databaseTemplate, cfg.dbConfig);

				IndexMode indexMode = config->databaseTemplate->GetIndexMode();
				cfg.outstation = ConvertConfig(config->outstation, (opendnp3::IndexMode) indexMode);
				cfg.link = ConvertConfig(config->link);				

				return cfg;
			}

			void Conversions::ApplyConfig(DatabaseTemplate^ lhs, asiodnp3::DatabaseConfig& rhs)
			{
				ConvertEventConfig<opendnp3::BinaryInfo>(lhs->binaries, rhs.binary);
				ConvertEventConfig<opendnp3::DoubleBitBinaryInfo>(lhs->doubleBinaries, rhs.doubleBinary);
				ConvertDeadbandConfig<opendnp3::AnalogInfo>(lhs->analogs, rhs.analog);
				ConvertDeadbandConfig<opendnp3::CounterInfo>(lhs->counters, rhs.counter);
				ConvertDeadbandConfig<opendnp3::FrozenCounterInfo>(lhs->frozenCounters, rhs.frozenCounter);
				ConvertEventConfig<opendnp3::BinaryOutputStatusInfo>(lhs->binaryOutputStatii, rhs.boStatus);
				ConvertDeadbandConfig<opendnp3::AnalogOutputStatusInfo>(lhs->analogOutputStatii, rhs.aoStatus);
				ConvertStaticConfig<opendnp3::TimeAndIntervalInfo>(lhs->timeAndIntervals, rhs.timeAndInterval);
			}			

			opendnp3::GroupVariationID Conversions::Convert(PointClass clazz)
			{
				switch (clazz)
				{
				case(PointClass::Class1) :
					return opendnp3::GroupVariationID(60, 1);
				case(PointClass::Class2) :
					return opendnp3::GroupVariationID(60, 2);
				case(PointClass::Class3) :
					return opendnp3::GroupVariationID(60, 3);
				default:
					return opendnp3::GroupVariationID(60, 4);
				}
			}

			openpal::Buffer Conversions::Convert(array<System::Byte>^ bytes)
			{
				if (bytes == nullptr)
				{
					return openpal::Buffer(0);
				}

				openpal::Buffer buffer(bytes->Length);

				for (int i = 0; i < bytes->Length; ++i)
				{
					buffer[i] = bytes[i];
				}

				return buffer;
			}

			array<System::Byte>^ Conversions::Convert(const openpal::RSlice& bytes)
			{
				array<System::Byte>^ ret = gcnew array<System::Byte>(bytes.Size());

				for (int i = 0; i < ret->Length; ++i)
				{
					ret[i] = bytes[i];
				}

				return ret;
			}			

		}
	}
}

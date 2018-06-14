//
//  _   _         ______    _ _ _   _             _ _ _
// | \ | |       |  ____|  | (_) | (_)           | | | |
// |  \| | ___   | |__   __| |_| |_ _ _ __   __ _| | | |
// | . ` |/ _ \  |  __| / _` | | __| | '_ \ / _` | | | |
// | |\  | (_) | | |___| (_| | | |_| | | | | (_| |_|_|_|
// |_| \_|\___/  |______\__,_|_|\__|_|_| |_|\__, (_|_|_)
//                                           __/ |
//                                          |___/
// 
// This file is auto-generated. Do not edit manually
// 
// Copyright 2016 Automatak LLC
// 
// Automatak LLC (www.automatak.com) licenses this file
// to you under the the Apache License Version 2.0 (the "License"):
// 
// http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "JCache.h"

namespace jni
{
    cache::AnalogConfig JCache::AnalogConfig;
    cache::AnalogInput JCache::AnalogInput;
    cache::AnalogOutputDouble64 JCache::AnalogOutputDouble64;
    cache::AnalogOutputFloat32 JCache::AnalogOutputFloat32;
    cache::AnalogOutputInt16 JCache::AnalogOutputInt16;
    cache::AnalogOutputInt32 JCache::AnalogOutputInt32;
    cache::AnalogOutputStatus JCache::AnalogOutputStatus;
    cache::AnalogOutputStatusConfig JCache::AnalogOutputStatusConfig;
    cache::ApplicationIIN JCache::ApplicationIIN;
    cache::ArrayList JCache::ArrayList;
    cache::AssignClassType JCache::AssignClassType;
    cache::BinaryConfig JCache::BinaryConfig;
    cache::BinaryInput JCache::BinaryInput;
    cache::BinaryOutputStatus JCache::BinaryOutputStatus;
    cache::BinaryOutputStatusConfig JCache::BinaryOutputStatusConfig;
    cache::ChannelListener JCache::ChannelListener;
    cache::ChannelState JCache::ChannelState;
    cache::ChannelStatistics JCache::ChannelStatistics;
    cache::ClassAssignment JCache::ClassAssignment;
    cache::ClassField JCache::ClassField;
    cache::CommandHandler JCache::CommandHandler;
    cache::CommandPointResult JCache::CommandPointResult;
    cache::CommandPointState JCache::CommandPointState;
    cache::CommandProcessor JCache::CommandProcessor;
    cache::CommandStatus JCache::CommandStatus;
    cache::CommandTaskResult JCache::CommandTaskResult;
    cache::CompletableFuture JCache::CompletableFuture;
    cache::ControlCode JCache::ControlCode;
    cache::ControlRelayOutputBlock JCache::ControlRelayOutputBlock;
    cache::Counter JCache::Counter;
    cache::CounterConfig JCache::CounterConfig;
    cache::DNPTime JCache::DNPTime;
    cache::DatabaseConfig JCache::DatabaseConfig;
    cache::DoubleBinaryConfig JCache::DoubleBinaryConfig;
    cache::DoubleBit JCache::DoubleBit;
    cache::DoubleBitBinaryInput JCache::DoubleBitBinaryInput;
    cache::Duration JCache::Duration;
    cache::EventAnalogOutputStatusVariation JCache::EventAnalogOutputStatusVariation;
    cache::EventAnalogVariation JCache::EventAnalogVariation;
    cache::EventBinaryOutputStatusVariation JCache::EventBinaryOutputStatusVariation;
    cache::EventBinaryVariation JCache::EventBinaryVariation;
    cache::EventBufferConfig JCache::EventBufferConfig;
    cache::EventConfig JCache::EventConfig;
    cache::EventCounterVariation JCache::EventCounterVariation;
    cache::EventDoubleBinaryVariation JCache::EventDoubleBinaryVariation;
    cache::EventFrozenCounterVariation JCache::EventFrozenCounterVariation;
    cache::FrozenCounter JCache::FrozenCounter;
    cache::FrozenCounterConfig JCache::FrozenCounterConfig;
    cache::GroupVariation JCache::GroupVariation;
    cache::Header JCache::Header;
    cache::HeaderInfo JCache::HeaderInfo;
    cache::IINField JCache::IINField;
    cache::IndexMode JCache::IndexMode;
    cache::IndexedValue JCache::IndexedValue;
    cache::Iterable JCache::Iterable;
    cache::Iterator JCache::Iterator;
    cache::LinkLayerConfig JCache::LinkLayerConfig;
    cache::LinkLayerStatistics JCache::LinkLayerStatistics;
    cache::LinkStatistics JCache::LinkStatistics;
    cache::List JCache::List;
    cache::LogEntry JCache::LogEntry;
    cache::LogHandler JCache::LogHandler;
    cache::MasterApplication JCache::MasterApplication;
    cache::MasterConfig JCache::MasterConfig;
    cache::MasterStackConfig JCache::MasterStackConfig;
    cache::MasterTaskType JCache::MasterTaskType;
    cache::OperateType JCache::OperateType;
    cache::OutstationApplication JCache::OutstationApplication;
    cache::OutstationConfig JCache::OutstationConfig;
    cache::OutstationStackConfig JCache::OutstationStackConfig;
    cache::ParserStatistics JCache::ParserStatistics;
    cache::PointClass JCache::PointClass;
    cache::QualifierCode JCache::QualifierCode;
    cache::Range JCache::Range;
    cache::SOEHandler JCache::SOEHandler;
    cache::Stack JCache::Stack;
    cache::StackStatistics JCache::StackStatistics;
    cache::StaticAnalogOutputStatusVariation JCache::StaticAnalogOutputStatusVariation;
    cache::StaticAnalogVariation JCache::StaticAnalogVariation;
    cache::StaticBinaryOutputStatusVariation JCache::StaticBinaryOutputStatusVariation;
    cache::StaticBinaryVariation JCache::StaticBinaryVariation;
    cache::StaticCounterVariation JCache::StaticCounterVariation;
    cache::StaticDoubleBinaryVariation JCache::StaticDoubleBinaryVariation;
    cache::StaticFrozenCounterVariation JCache::StaticFrozenCounterVariation;
    cache::TLSConfig JCache::TLSConfig;
    cache::TaskCompletion JCache::TaskCompletion;
    cache::TaskId JCache::TaskId;
    cache::TaskInfo JCache::TaskInfo;
    cache::TimeSyncMode JCache::TimeSyncMode;
    cache::TimestampMode JCache::TimestampMode;
    cache::TransportStatistics JCache::TransportStatistics;

    bool JCache::init(JNIEnv* env)
    {
        return AnalogConfig.init(env)
        && AnalogInput.init(env)
        && AnalogOutputDouble64.init(env)
        && AnalogOutputFloat32.init(env)
        && AnalogOutputInt16.init(env)
        && AnalogOutputInt32.init(env)
        && AnalogOutputStatus.init(env)
        && AnalogOutputStatusConfig.init(env)
        && ApplicationIIN.init(env)
        && ArrayList.init(env)
        && AssignClassType.init(env)
        && BinaryConfig.init(env)
        && BinaryInput.init(env)
        && BinaryOutputStatus.init(env)
        && BinaryOutputStatusConfig.init(env)
        && ChannelListener.init(env)
        && ChannelState.init(env)
        && ChannelStatistics.init(env)
        && ClassAssignment.init(env)
        && ClassField.init(env)
        && CommandHandler.init(env)
        && CommandPointResult.init(env)
        && CommandPointState.init(env)
        && CommandProcessor.init(env)
        && CommandStatus.init(env)
        && CommandTaskResult.init(env)
        && CompletableFuture.init(env)
        && ControlCode.init(env)
        && ControlRelayOutputBlock.init(env)
        && Counter.init(env)
        && CounterConfig.init(env)
        && DNPTime.init(env)
        && DatabaseConfig.init(env)
        && DoubleBinaryConfig.init(env)
        && DoubleBit.init(env)
        && DoubleBitBinaryInput.init(env)
        && Duration.init(env)
        && EventAnalogOutputStatusVariation.init(env)
        && EventAnalogVariation.init(env)
        && EventBinaryOutputStatusVariation.init(env)
        && EventBinaryVariation.init(env)
        && EventBufferConfig.init(env)
        && EventConfig.init(env)
        && EventCounterVariation.init(env)
        && EventDoubleBinaryVariation.init(env)
        && EventFrozenCounterVariation.init(env)
        && FrozenCounter.init(env)
        && FrozenCounterConfig.init(env)
        && GroupVariation.init(env)
        && Header.init(env)
        && HeaderInfo.init(env)
        && IINField.init(env)
        && IndexMode.init(env)
        && IndexedValue.init(env)
        && Iterable.init(env)
        && Iterator.init(env)
        && LinkLayerConfig.init(env)
        && LinkLayerStatistics.init(env)
        && LinkStatistics.init(env)
        && List.init(env)
        && LogEntry.init(env)
        && LogHandler.init(env)
        && MasterApplication.init(env)
        && MasterConfig.init(env)
        && MasterStackConfig.init(env)
        && MasterTaskType.init(env)
        && OperateType.init(env)
        && OutstationApplication.init(env)
        && OutstationConfig.init(env)
        && OutstationStackConfig.init(env)
        && ParserStatistics.init(env)
        && PointClass.init(env)
        && QualifierCode.init(env)
        && Range.init(env)
        && SOEHandler.init(env)
        && Stack.init(env)
        && StackStatistics.init(env)
        && StaticAnalogOutputStatusVariation.init(env)
        && StaticAnalogVariation.init(env)
        && StaticBinaryOutputStatusVariation.init(env)
        && StaticBinaryVariation.init(env)
        && StaticCounterVariation.init(env)
        && StaticDoubleBinaryVariation.init(env)
        && StaticFrozenCounterVariation.init(env)
        && TLSConfig.init(env)
        && TaskCompletion.init(env)
        && TaskId.init(env)
        && TaskInfo.init(env)
        && TimeSyncMode.init(env)
        && TimestampMode.init(env)
        && TransportStatistics.init(env)
        ;
    }

    void JCache::cleanup(JNIEnv* env)
    {
        AnalogConfig.cleanup(env);
        AnalogInput.cleanup(env);
        AnalogOutputDouble64.cleanup(env);
        AnalogOutputFloat32.cleanup(env);
        AnalogOutputInt16.cleanup(env);
        AnalogOutputInt32.cleanup(env);
        AnalogOutputStatus.cleanup(env);
        AnalogOutputStatusConfig.cleanup(env);
        ApplicationIIN.cleanup(env);
        ArrayList.cleanup(env);
        AssignClassType.cleanup(env);
        BinaryConfig.cleanup(env);
        BinaryInput.cleanup(env);
        BinaryOutputStatus.cleanup(env);
        BinaryOutputStatusConfig.cleanup(env);
        ChannelListener.cleanup(env);
        ChannelState.cleanup(env);
        ChannelStatistics.cleanup(env);
        ClassAssignment.cleanup(env);
        ClassField.cleanup(env);
        CommandHandler.cleanup(env);
        CommandPointResult.cleanup(env);
        CommandPointState.cleanup(env);
        CommandProcessor.cleanup(env);
        CommandStatus.cleanup(env);
        CommandTaskResult.cleanup(env);
        CompletableFuture.cleanup(env);
        ControlCode.cleanup(env);
        ControlRelayOutputBlock.cleanup(env);
        Counter.cleanup(env);
        CounterConfig.cleanup(env);
        DNPTime.cleanup(env);
        DatabaseConfig.cleanup(env);
        DoubleBinaryConfig.cleanup(env);
        DoubleBit.cleanup(env);
        DoubleBitBinaryInput.cleanup(env);
        Duration.cleanup(env);
        EventAnalogOutputStatusVariation.cleanup(env);
        EventAnalogVariation.cleanup(env);
        EventBinaryOutputStatusVariation.cleanup(env);
        EventBinaryVariation.cleanup(env);
        EventBufferConfig.cleanup(env);
        EventConfig.cleanup(env);
        EventCounterVariation.cleanup(env);
        EventDoubleBinaryVariation.cleanup(env);
        EventFrozenCounterVariation.cleanup(env);
        FrozenCounter.cleanup(env);
        FrozenCounterConfig.cleanup(env);
        GroupVariation.cleanup(env);
        Header.cleanup(env);
        HeaderInfo.cleanup(env);
        IINField.cleanup(env);
        IndexMode.cleanup(env);
        IndexedValue.cleanup(env);
        Iterable.cleanup(env);
        Iterator.cleanup(env);
        LinkLayerConfig.cleanup(env);
        LinkLayerStatistics.cleanup(env);
        LinkStatistics.cleanup(env);
        List.cleanup(env);
        LogEntry.cleanup(env);
        LogHandler.cleanup(env);
        MasterApplication.cleanup(env);
        MasterConfig.cleanup(env);
        MasterStackConfig.cleanup(env);
        MasterTaskType.cleanup(env);
        OperateType.cleanup(env);
        OutstationApplication.cleanup(env);
        OutstationConfig.cleanup(env);
        OutstationStackConfig.cleanup(env);
        ParserStatistics.cleanup(env);
        PointClass.cleanup(env);
        QualifierCode.cleanup(env);
        Range.cleanup(env);
        SOEHandler.cleanup(env);
        Stack.cleanup(env);
        StackStatistics.cleanup(env);
        StaticAnalogOutputStatusVariation.cleanup(env);
        StaticAnalogVariation.cleanup(env);
        StaticBinaryOutputStatusVariation.cleanup(env);
        StaticBinaryVariation.cleanup(env);
        StaticCounterVariation.cleanup(env);
        StaticDoubleBinaryVariation.cleanup(env);
        StaticFrozenCounterVariation.cleanup(env);
        TLSConfig.cleanup(env);
        TaskCompletion.cleanup(env);
        TaskId.cleanup(env);
        TaskInfo.cleanup(env);
        TimeSyncMode.cleanup(env);
        TimestampMode.cleanup(env);
        TransportStatistics.cleanup(env);
    }
}

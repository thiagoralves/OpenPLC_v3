package com.automatak.dnp3.codegen

import java.util.concurrent.CompletableFuture

import com.automatak.dnp3._
import com.automatak.dnp3.enums._

/// a cache for class names
object Classes {

  val all : List[ClassConfig] = (enumerations ::: interfaces ::: javaTypes ::: custom).sortBy(_.clazz.getSimpleName)

  private def listMethods = MethodFilter.nameEquals("add", Some(1))
  private def listConstructors = ConstructorFilter.withParamTypes(List("int"))

  private def enumerations : List[ClassConfig] = List(
    classOf[GroupVariation],
    classOf[QualifierCode],
    classOf[TimestampMode],
    classOf[DoubleBit],
    classOf[TimeSyncMode],
    classOf[MasterTaskType],
    classOf[TaskCompletion],
    classOf[CommandPointState],
    classOf[CommandStatus],
    classOf[ControlCode],
    classOf[ChannelState],
    classOf[PointClass],
    classOf[OperateType],
    classOf[AssignClassType],

    classOf[StaticBinaryVariation],
    classOf[StaticDoubleBinaryVariation],
    classOf[StaticAnalogVariation],
    classOf[StaticCounterVariation],
    classOf[StaticFrozenCounterVariation],
    classOf[StaticAnalogOutputStatusVariation],
    classOf[StaticBinaryOutputStatusVariation],

    classOf[EventBinaryVariation],
    classOf[EventDoubleBinaryVariation],
    classOf[EventAnalogVariation],
    classOf[EventCounterVariation],
    classOf[EventFrozenCounterVariation],
    classOf[EventAnalogOutputStatusVariation],
    classOf[EventBinaryOutputStatusVariation]

  ).map(e => ClassConfig(
    e,
    Set(Features.Methods),
    MethodFilter.any(MethodFilter.nameEquals("fromType"), MethodFilter.nameEquals("toType"))
  ))

  private def interfaces : List[ClassConfig] = List(
    classOf[LogHandler],
    classOf[SOEHandler],
    classOf[MasterApplication],
    classOf[OutstationApplication],
    classOf[CommandProcessor],
    classOf[Stack],
    classOf[ChannelListener],
    classOf[CommandHandler],
    classOf[IndexMode]
  ).map(c => ClassConfig(c, Set(Features.Methods)))

  private def javaTypes : List[ClassConfig] = List(
    ClassConfig(classOf[java.time.Duration], Set(Features.Methods), MethodFilter.nameEquals("toMillis")),
    ClassConfig(classOf[java.util.List[_]], Set(Features.Methods), MethodFilter.nameEquals("size")),
    ClassConfig(classOf[java.util.ArrayList[_]], Set(Features.Constructors, Features.Methods), listMethods, listConstructors),
    ClassConfig(classOf[java.lang.Iterable[_]], Set(Features.Methods), MethodFilter.nameEquals("iterator")),
    ClassConfig(classOf[java.util.Iterator[_]], Set(Features.Methods), MethodFilter.equalsAny("hasNext", "next"))
  )

  // all the classes to generate C++ info on
  private def custom : List[ClassConfig] = List(
    ClassConfig(classOf[MasterStackConfig], Set(Features.Fields)),
    ClassConfig(classOf[OutstationStackConfig], Set(Features.Fields)),
    ClassConfig(classOf[MasterConfig], Set(Features.Fields)),
    ClassConfig(classOf[OutstationConfig], Set(Features.Fields)),
    ClassConfig(classOf[LinkLayerConfig], Set(Features.Fields)),
    ClassConfig(classOf[LogEntry], Set(Features.Constructors)),
    ClassConfig(classOf[ClassField], Set(Features.Fields)),
    ClassConfig(classOf[HeaderInfo], Set(Features.Constructors)),
    ClassConfig(classOf[IndexedValue[_]], Set(Features.Constructors, Features.Fields)),
    ClassConfig(classOf[BinaryInput], Set(Features.Constructors)),
    ClassConfig(classOf[DoubleBitBinaryInput], Set(Features.Constructors)),
    ClassConfig(classOf[AnalogInput], Set(Features.Constructors)),
    ClassConfig(classOf[Counter], Set(Features.Constructors)),
    ClassConfig(classOf[FrozenCounter], Set(Features.Constructors)),
    ClassConfig(classOf[BinaryOutputStatus], Set(Features.Constructors)),
    ClassConfig(classOf[AnalogOutputStatus], Set(Features.Constructors)),
    ClassConfig(classOf[IINField], Set(Features.Constructors)),
    ClassConfig(classOf[TaskId], Set(Features.Constructors)),
    ClassConfig(classOf[TaskInfo], Set(Features.Constructors)),
    ClassConfig(classOf[ClassAssignment], Set(Features.Fields)),
    ClassConfig(classOf[Range], Set(Features.Fields, Features.Methods), MethodFilter.nameEquals("isDefined")),
    ClassConfig(classOf[CommandTaskResult], Set(Features.Constructors)),
    ClassConfig(classOf[CommandPointResult], Set(Features.Constructors)),
    ClassConfig(classOf[CompletableFuture[_]], Set(Features.Methods), MethodFilter.nameEquals("complete")),
    ClassConfig(classOf[ControlRelayOutputBlock], Set(Features.Fields, Features.Constructors)),
    ClassConfig(classOf[AnalogOutputInt16], Set(Features.Fields, Features.Constructors)),
    ClassConfig(classOf[AnalogOutputInt32], Set(Features.Fields, Features.Constructors)),
    ClassConfig(classOf[AnalogOutputFloat32], Set(Features.Fields, Features.Constructors)),
    ClassConfig(classOf[AnalogOutputDouble64], Set(Features.Fields, Features.Constructors)),
    ClassConfig(classOf[Header], Set(Features.Fields)),
    ClassConfig(classOf[ApplicationIIN], Set(Features.Fields)),
    ClassConfig(classOf[EventBufferConfig], Set(Features.Fields)),
    ClassConfig(classOf[DatabaseConfig], Set(Features.Fields)),
    ClassConfig(classOf[EventConfig], Set(Features.Fields)),
    ClassConfig(classOf[BinaryConfig], Set(Features.Fields)),
    ClassConfig(classOf[DoubleBinaryConfig], Set(Features.Fields)),
    ClassConfig(classOf[AnalogConfig], Set(Features.Fields)),
    ClassConfig(classOf[CounterConfig], Set(Features.Fields)),
    ClassConfig(classOf[FrozenCounterConfig], Set(Features.Fields)),
    ClassConfig(classOf[BinaryOutputStatusConfig], Set(Features.Fields)),
    ClassConfig(classOf[AnalogOutputStatusConfig], Set(Features.Fields)),
    ClassConfig(classOf[TLSConfig], Set(Features.Fields)),
    ClassConfig(classOf[DNPTime], Set(Features.Constructors)),
    ClassConfig(classOf[ParserStatistics], Set(Features.Constructors)),
    ClassConfig(classOf[ChannelStatistics], Set(Features.Constructors)),
    ClassConfig(classOf[LinkStatistics], Set(Features.Constructors)),
    ClassConfig(classOf[LinkLayerStatistics], Set(Features.Constructors)),
    ClassConfig(classOf[TransportStatistics], Set(Features.Constructors)),
    ClassConfig(classOf[StackStatistics], Set(Features.Constructors))
  )


}

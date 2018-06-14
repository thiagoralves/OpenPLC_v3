package com.automatak.render.dnp3.enums.groups


import com.automatak.render.EnumModel
import com.automatak.render.dnp3.enums._


object CSharpEnumGroup {

  def enums : List[EnumModel] = List(
    AssignClassType(),
    MasterTaskType(),
    StaticTypeBitmask(),
    IntervalUnit(),
    DoubleBit(),
    PointClass(),
    CommandStatus(),
    TaskCompletion(),
    ControlCode(),
    ChannelState(),
    TimeSyncMode(),
    RestartMode(),
    TimestampMode(),
    QualifierCode(),
    GroupVariationEnum(),
    EventMode(),
    IndexMode(),
    UserOperation(),
    UserRole(),
    KeyWrapAlgorithm(),
    KeyChangeMethod(),
    CommandPointState(),
    RestartType(),
    OperateType(),
    LinkFunction(),
    LinkStatus(),
    Parity(),
    StopBits(),
    FlowControl(),
    FlagsType()
  ) ::: DefaultVariations.enums ::: QualityMasks.enums


}

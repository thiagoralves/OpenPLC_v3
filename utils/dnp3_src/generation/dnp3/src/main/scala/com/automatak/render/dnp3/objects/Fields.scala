package com.automatak.render.dnp3.objects

import com.automatak.render.EnumModel
import com.automatak.render.dnp3.enums._
import com.automatak.render.dnp3.objects.generators.FixedSizeHelpers

sealed abstract class FixedSizeFieldType(val numBytes: Int) {
  def defaultValue: String = "0"
}

case object UInt8Field extends FixedSizeFieldType(1)
case object UInt16Field extends FixedSizeFieldType(2)
case object UInt32Field extends FixedSizeFieldType(4)
case object UInt48Field extends FixedSizeFieldType(6)
case object SInt16Field extends FixedSizeFieldType(2)
case object SInt32Field extends FixedSizeFieldType(4)
case object Float32Field extends FixedSizeFieldType(4)
case object Float64Field extends FixedSizeFieldType(8)
case class EnumFieldType(model: EnumModel) extends FixedSizeFieldType(1) {
  override def defaultValue: String = "%s::%s".format(model.name, model.default.displayName)
}

object FieldAttribute extends Enumeration {
  type WeekDay = Value
  val IsTimeUTC, IsTimeRel, IsFlags = Value
}

object FixedSizeField {

  //common flags field
  val flags = FixedSizeField("flags", UInt8Field, Set(FieldAttribute.IsFlags))

  // SA stuff
  val csq = FixedSizeField("challengeSeqNum", UInt32Field)
  val ksq = FixedSizeField("keyChangeSeqNum", UInt32Field)
  val scsq = FixedSizeField("statusChangeSeqNum", UInt32Field)
  val userNum = FixedSizeField("userNum", UInt16Field)
  val assocId = FixedSizeField("assocId", UInt16Field)
  val macAlgo = FixedSizeField("hmacAlgo", EnumFieldType(HMACType()))
  val keyWrapAlgo = FixedSizeField("keyWrapAlgo", EnumFieldType(KeyWrapAlgorithm()))
  val keyStatus = FixedSizeField("keyStatus", EnumFieldType(KeyStatus()))
  val challengeReason = FixedSizeField("challengeReason", EnumFieldType(ChallengeReason()))
  val errorCode = FixedSizeField("errorCode", EnumFieldType(AuthErrorCode()))
  val keyChangeMethod = FixedSizeField("keyChangeMethod", EnumFieldType(KeyChangeMethod()))
  val certificateType = FixedSizeField("certificateType", EnumFieldType(CertificateType()))
  val userOperation = FixedSizeField("userOperation", EnumFieldType(UserOperation()))
  val userRole = FixedSizeField("userRole", UInt16Field)
  val userRoleExpDays = FixedSizeField("userRoleExpDays", UInt16Field)


  // timestamps
  val time16 = FixedSizeField("time", UInt16Field, Set(FieldAttribute.IsTimeRel))
  val time48 = FixedSizeField("time", UInt48Field, Set(FieldAttribute.IsTimeUTC))

  // counter values
  val count16 = FixedSizeField("value", UInt16Field)
  val count32 = FixedSizeField("value", UInt32Field)

  // analog values
  val value16 = FixedSizeField("value", SInt16Field)
  val value32 = FixedSizeField("value", SInt32Field)
  val float32 = FixedSizeField("value", Float32Field)
  val float64 = FixedSizeField("value", Float64Field)

  //enums
  val commandStatus = FixedSizeField("status", UInt8Field)
  val intervalUnit = FixedSizeField("intervalUnit", EnumFieldType(IntervalUnit()))

}

object VariableFields {
  val challengeData = VariableField("challengeData")
  val hmac = VariableField("hmacValue")
  val keyWrapData = VariableField("keyWrapData")
  val errorText = VariableField("errorText")
  val certificate = VariableField("certificate")
  val userName = VariableField("userName")
  val userPublicKey = VariableField("userPublicKey")
  val certificationData = VariableField("certificationData")
  val encryptedUpdateKey = VariableField("encryptedUpdateKey")
  val signature = VariableField("digitalSignature")
}

sealed trait Field {
  def name: String
  def cppType: String
  def cppArgument: String = cppType
}

sealed case class FixedSizeField(name: String, typ: FixedSizeFieldType, attributes: Set[FieldAttribute.Value] = Set.empty) extends Field {

  def isTimeUTC : Boolean = attributes.contains(FieldAttribute.IsTimeUTC)
  def isFlags : Boolean = attributes.contains(FieldAttribute.IsFlags)

  def cppType: String = typ match {
    case UInt8Field => "uint8_t"
    case UInt16Field => "uint16_t"
    case UInt32Field => "uint32_t"
    case UInt48Field => "DNPTime"
    case SInt16Field => "int16_t"
    case SInt32Field => "int32_t"
    case Float32Field => "float"
    case Float64Field => "double"
    case EnumFieldType(model: EnumModel) => model.name
    case _ => throw new Exception("Unknown field type")
  }

  def defaultValue: String = typ match {
    case UInt8Field => "0"
    case UInt16Field => "0"
    case UInt32Field => "0"
    case UInt48Field => "0"
    case SInt16Field => "0"
    case SInt32Field => "0"
    case Float32Field => "0.0"
    case Float64Field => "0.0"
    case EnumFieldType(model: EnumModel) => model.defaultValue.get.toString
    case _ => throw new Exception("Unknown field type")
  }

}
sealed case class VariableField(name: String) extends Field {
  def cppType: String = "openpal::RSlice"
  override def cppArgument = "const openpal::RSlice&"
}



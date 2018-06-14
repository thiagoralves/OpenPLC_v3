package com.automatak.render.dnp3.enums

import com.automatak.render._
import com.automatak.render.dnp3.objects.{GroupVariation, ObjectGroup}
import scala.Some

object GroupVariationEnum {

  private val defaultValue = EnumValue("UNKNOWN", 0xFFFF)

  private def values : List[EnumValue] = ObjectGroup.all.flatMap(_.objects).map { gv =>
    EnumValue(gv.name, gv.shortValue, None, Some(gv.fullDesc))
  }

  def apply() : EnumModel = EnumModel(
    "GroupVariation",
    List("Comprehensive list of supported groups and variations"),
    EnumModel.UInt16,
    values,
    Some(defaultValue),
    Hex
  )

}

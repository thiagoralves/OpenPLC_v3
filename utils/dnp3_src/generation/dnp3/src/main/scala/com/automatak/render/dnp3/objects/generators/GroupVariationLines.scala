package com.automatak.render.dnp3.objects.generators

import com.automatak.render._
import com.automatak.render.cpp._
import com.automatak.render.dnp3.objects.GroupVariation

object GroupVariationLines {

  def idDeclaration(gv: GroupVariation): Iterator[String] = Iterator(
    "static GroupVariationID ID() { return GroupVariationID(%s,%s); }".format(gv.group, gv.variation)
  )

  def implComments(gv: GroupVariation)(implicit i : Indentation) : Iterator[String] = comment("------- %s -------".format(gv.name)) ++ space

}

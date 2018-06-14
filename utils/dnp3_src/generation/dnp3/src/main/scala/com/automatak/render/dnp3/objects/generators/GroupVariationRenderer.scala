package com.automatak.render.dnp3.objects.generators

import com.automatak.render.dnp3.objects._

import com.automatak.render._
import com.automatak.render.cpp._

object GroupVariationHeaderRenderer extends ModelRenderer[GroupVariation]{

  def render(gv: GroupVariation)(implicit i: Indentation): Iterator[String] = comment(gv.fullDesc) ++ gv.declaration

}

object GroupVariationImplRenderer extends ModelRenderer[GroupVariation]{

  def render(gv: GroupVariation)(implicit i: Indentation): Iterator[String] = gv.implLines

}

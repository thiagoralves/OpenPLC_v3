package com.automatak.render.dnp3.enums

import com.automatak.render._


object IndexMode {

  private val comments = List(
    "Select contiguous or dis-contiguous index mode"
  )

  def apply(): EnumModel = EnumModel("IndexMode", comments, EnumModel.UInt8, codes, None, Hex)

  private val codes = List(
    EnumValue("Contiguous", 0, "Indices are contiguous. Most efficient as direct indexing is used."),
    EnumValue("Discontiguous", 1, "Indices are dis-contiguous. Resorts to binary search to find raw index.")
  )

}




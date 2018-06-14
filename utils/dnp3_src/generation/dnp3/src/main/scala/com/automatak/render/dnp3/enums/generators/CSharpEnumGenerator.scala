package com.automatak.render.dnp3.enums.generators

import _root_.java.nio.file.Path
import com.automatak.render._

import com.automatak.render.dnp3.enums._
import com.automatak.render.cpp.CppIndentation
import com.automatak.render.csharp._

object CSharpEnumGenerator {

  def apply(enums: List[EnumModel], ns: String, dir: Path): Unit = {

    def fileName(model: EnumModel) = model.name + ".cs"

    def filePath(model: EnumModel) = dir.resolve(fileName(model))

    implicit val indent = CppIndentation()

    def writeEnumToFile(model: EnumModel): Unit = {
        def license = commented(LicenseHeader())
        def enum = EnumModelRenderer.render(model)
        def lines = license ++ space ++ namespace(ns)(enum)
        writeTo(filePath(model))(lines)
        println("Wrote: " + filePath(model))
    }

    enums.foreach(writeEnumToFile)
  }
}

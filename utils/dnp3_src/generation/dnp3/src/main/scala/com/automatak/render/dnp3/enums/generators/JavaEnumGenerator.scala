package com.automatak.render.dnp3.enums.generators

import _root_.java.nio.file.Path
import com.automatak.render._

import com.automatak.render.cpp.CppIndentation
import com.automatak.render.java.EnumModelRenderer

object JavaEnumGenerator {

  def apply(enums: List[EnumModel], packageLine: String, dir: Path): Unit = {

    def fileName(model: EnumModel) = model.name + ".java"

    def filePath(model: EnumModel) = dir.resolve(fileName(model))

    implicit val indent = CppIndentation()

    def writeEnumToFile(model: EnumModel): Unit = {
      def license = commented(LicenseHeader())
      def enum = EnumModelRenderer.render(model)
      def lines = license ++ space ++ Iterator(packageLine) ++ (enum)
      writeTo(filePath(model))(lines)
      println("Wrote: " + filePath(model))
    }

    enums.foreach(writeEnumToFile)
  }
}

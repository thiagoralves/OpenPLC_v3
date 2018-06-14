package com.automatak.render.dnp3.enums.generators

import _root_.java.nio.file.Path

import com.automatak.render._
import com.automatak.render.cpp._

object CppEnumGenerator {

  def apply(enums: List[EnumConfig], cppNamespace : String, incFormatString: String, incDirectory: Path, implDirectory: Path): Unit = {

    implicit val indent = CppIndentation()

    def headerPath(model: EnumModel) = incDirectory.resolve(headerName(model))
    def implPath(model: EnumModel) = implDirectory.resolve(implName(model))

    def headerName(model: EnumModel) = model.name + ".h"
    def implName(model: EnumModel) = model.name + ".cpp"

    def writeEnumToFiles(cfg: EnumConfig): Unit = {

      val conversions = if(cfg.conversions) List(EnumToType, EnumFromType) else Nil
      val stringify = if(cfg.stringConv) List(EnumToString) else Nil

      val renders = conversions ::: stringify

      def writeHeader() {
        def license = commented(LicenseHeader())
        def enum = EnumModelRenderer.render(cfg.model)
        def signatures = renders.map(c => c.header.render(cfg.model)).flatten.toIterator
        def lines = license ++ space ++ includeGuards(cfg.model.name)(cstdint ++ space ++ namespace(cppNamespace)(enum ++ space ++ signatures))
        writeTo(headerPath(cfg.model))(lines)
        println("Wrote: " + headerPath(cfg.model))
      }

      def writeImpl() {
        def license = commented(LicenseHeader())
        def funcs = renders.map(r => r.impl.render(cfg.model)).flatten.toIterator
        def inc = quoted(String.format(incFormatString, headerName(cfg.model)))
        def lines = license ++ space ++ Iterator(include(inc)) ++ space ++ namespace(cppNamespace)(funcs)

        if(cfg.conversions || cfg.stringConv)
        {
          writeTo(implPath(cfg.model))(lines)
          println("Wrote: " + implPath(cfg.model))
        }
      }

      writeHeader()
      writeImpl()
    }

    enums.foreach { e =>
      writeEnumToFiles(e)
    }
  }
}

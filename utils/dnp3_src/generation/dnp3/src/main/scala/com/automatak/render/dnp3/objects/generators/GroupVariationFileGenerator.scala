package com.automatak.render.dnp3.objects.generators

import java.nio.file.Path
import com.automatak.render.dnp3.objects._
import com.automatak.render._
import com.automatak.render.cpp._
import com.automatak.render.LicenseHeader

object GroupVariationFileGenerator {

  def apply(path: Path) = {

    implicit val indent = CppIndentation()

    def headerPath(group: ObjectGroup): Path = path.resolve(group.name+".h")
    def implPath(group: ObjectGroup): Path = path.resolve(group.name+".cpp")

    def definitions(group: ObjectGroup): Iterator[String] = spaced(group.objects.iterator.map(o => comment(o.fullDesc) ++ o.declaration))

    def implementations(group: ObjectGroup): Iterator[String] = spaced(group.objects.iterator.map(o => o.implLines))

    def optionalIncludes(group: ObjectGroup) : Set[String] = {

      def getEnums(gv: GroupVariation):  List[String] = {

        def extract(typ: FixedSizeFieldType): Option[String] = typ match {
          case EnumFieldType(model) => Some(quoted("opendnp3/gen/"+model.name+".h"))
          case _ => None
        }

        gv match {
          case fs : FixedSize => fs.fields.flatMap(f => extract(f.typ))
          case _ => Nil
        }
      }


      def include(file: String): String = "#include " + file

      group.objects.flatMap(o => getEnums(o)).map(include).toSet
    }

    def includeHeader(group: ObjectGroup): Iterator[String] = Iterator("#include " + quoted(group.name+".h"))

    def headerFile(group: ObjectGroup): Iterator[String] = {
      commented(LicenseHeader()) ++ space ++
      includeGuards(group.name.toUpperCase) {
        headerIncludes(group) ++
        optionalIncludes(group) ++ space ++
        namespace("opendnp3") {
          definitions(group)
        }
      }
    }

    def implFile(group: ObjectGroup): Iterator[String] = {
      val defs = implementations(group)
      if(defs.isEmpty) Iterator.empty
      else
      {
        commented(LicenseHeader()) ++ space ++
          includeHeader(group) ++ space ++
          implIncludes(group) ++ space ++
          Iterator("using namespace openpal;") ++ space ++
          namespace("opendnp3") {
            defs
          }
      }
    }

    def headerIncludes(group: ObjectGroup): Iterator[String] = {
      group.objects.flatMap(_.headerIncludes).distinct.map(x => "#include %s".format(x)).toIterator
    }

    def implIncludes(group: ObjectGroup): Iterator[String] = {
      group.objects.flatMap(_.implIncludes).distinct.map(x => "#include %s".format(x)).toIterator
    }

    ObjectGroup.all.foreach { g =>
        writeTo(headerPath(g))(headerFile(g))
        (writeTo(implPath(g))(implFile(g)))
    }
  }

}

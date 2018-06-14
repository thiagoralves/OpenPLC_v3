package com.automatak.render.dnp3

import java.nio.file.FileSystems

import com.automatak.render.dnp3.enums.generators.{CSharpEnumGenerator, CppEnumGenerator, JavaEnumGenerator}
import com.automatak.render.dnp3.enums.groups.{CSharpEnumGroup, DNPCppEnumGroup}
import com.automatak.render.dnp3.objects.generators.{AttributeGenerator, GroupVariationFileGenerator}

object Generate {

  val dnp3GenHeaderPath = FileSystems.getDefault.getPath("../cpp/libs/include/opendnp3/gen")
  val dnp3GenImplPath = FileSystems.getDefault.getPath("../cpp/libs/src/opendnp3/gen")
  val csharpGenPath = FileSystems.getDefault.getPath("../dotnet/bindings/CLRInterface/gen")
  val dnp3ObjectPath = FileSystems.getDefault.getPath("../cpp/libs/src/opendnp3/objects")
  val javaGenPath = FileSystems.getDefault.getPath("../java/api/src/main/java/com/automatak/dnp3/enums")

  def main(args: Array[String]): Unit = {

    // generate the C++ dnp3 enums
    CppEnumGenerator(DNPCppEnumGroup.enums, "opendnp3", "opendnp3/gen/%s", dnp3GenHeaderPath, dnp3GenImplPath)

    // generate the C++ variation attribute lookups
    AttributeGenerator.writeAttributes("opendnp3", dnp3GenHeaderPath, dnp3GenImplPath)

    // generate the C# enums
    CSharpEnumGenerator(CSharpEnumGroup.enums, "Automatak.DNP3.Interface", csharpGenPath)


    // generate the Java enums
    JavaEnumGenerator(CSharpEnumGroup.enums, "package com.automatak.dnp3.enums;", javaGenPath)

    // generate all the group/variation parsers
    GroupVariationFileGenerator(dnp3ObjectPath)
  }

}

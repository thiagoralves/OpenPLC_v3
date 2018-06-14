package com.automatak.render.cpp

import com.automatak.render._

/*
object DefaultSwitchModelRenderer {

  def wrap[A](f: A => String): A => String = { (a: A) => "return " + f(a) + ";" }

}

class DefaultSwitchModelRenderer[A](fCase: A => String)(fAction: A => String) extends SwitchModelRenderer(fCase)(DefaultSwitchModelRenderer.wrap(fAction))
*/

class SwitchModelRenderer[A](fCase: A => String)(fAction: A => String) {

  def render(nonDefaults: List[A], default: A)(implicit indent: Indentation): Iterator[String] = {

    def switch = Iterator("switch(arg)")

    def nonDefaultCases: Iterator[String] = nonDefaults.toIterator.map { c =>
      Iterator(List("case(", fCase(c), "):").mkString) ++
        indent {
          Iterator("return " + fAction(c) + ";")
        }
    }.flatten.toIterator

    def defaultCase: Iterator[String] = {
      Iterator("default:") ++ indent { Iterator("return " + fAction(default) + ";") }
    }

    switch ++ bracket {
      nonDefaultCases ++
      defaultCase
    }
  }
}

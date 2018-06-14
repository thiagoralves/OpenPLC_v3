package com.automatak.render.cpp

import com.automatak.render.Indentation

object CppIndentation {
  def apply(): Indentation = IndentationImpl("  ")
}


private case class IndentationImpl(indent: String) extends Indentation {

  private var count = 0

  def apply(f: => Iterator[String]): Iterator[String] = {
    count += 1
    val padding = Iterator.fill(count)(indent).mkString("") //calculate padding NOW instead of lazily
    def transform(s: String): String = {
      if(s.isEmpty) s
      else padding + s
    }
    val iter = f.map(transform)
    count -= 1
    iter
  }

}

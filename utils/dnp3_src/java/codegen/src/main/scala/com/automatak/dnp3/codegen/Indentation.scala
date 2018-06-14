package com.automatak.dnp3.codegen

trait Indentation {

  def apply(f: => Iterator[String]): Iterator[String]

}

object CppIndentation extends Indentation {

  private var count = 0

  def apply(f: => Iterator[String]): Iterator[String] = {
    count += 1
    val padding = Iterator.fill(count)("    ").mkString("") //calculate padding NOW instead of lazily
    def transform(s: String): String = {
      if(s.isEmpty) s
      else padding + s
    }
    val iter = f.map(transform)
    count -= 1
    iter
  }

}
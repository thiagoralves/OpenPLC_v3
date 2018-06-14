package com.automatak.render

trait Indentation {

  def apply(f: => Iterator[String]): Iterator[String]

}

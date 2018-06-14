package com.automatak.render.cpp

import com.automatak.render.Indentation
import com.automatak.render.space

import scala.annotation.tailrec

object CppFunction {

  def withSpaces(items: List[Iterator[String]]): Iterator[String] = {
    withSpacesRec(items, Iterator.empty)
  }

  @tailrec
  def withSpacesRec(items: List[Iterator[String]], sum: Iterator[String]): Iterator[String] = items match {
    case x :: tail => if(sum.isEmpty) withSpacesRec(tail, x) else withSpacesRec(tail, sum ++ space ++ x)
    case Nil => sum
  }
}

trait CppFunction {
  def signature(implicit indent: Indentation) : Iterator[String]
  def implementation(implicit indent: Indentation) : Iterator[String]
}

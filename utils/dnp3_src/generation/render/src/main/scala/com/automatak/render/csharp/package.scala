package com.automatak.render

package object csharp {

  def namespace(namespace: String)(inner: => Iterator[String])(implicit indent: Indentation): Iterator[String] = {
    Iterator("namespace " + namespace) ++ bracket {
      inner
    }
  }
}

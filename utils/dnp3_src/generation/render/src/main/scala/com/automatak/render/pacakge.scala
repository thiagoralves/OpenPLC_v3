/**
 * Copyright 2013 Automatak, LLC
 *
 * Licensed to Automatak, LLC (www.automatak.com) under one or more
 * contributor license agreements. See the NOTICE file distributed with this
 * work for additional information regarding copyright ownership. Automatak, LLC
 * licenses this file to you under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may obtain
 * a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
package com.automatak

import java.nio.file.{Files, StandardOpenOption, Path}
import java.nio.charset.Charset

package object render {

    // a custom flatten that adds a blank line in between blocks
    def spaced(i: Iterator[Iterator[String]]): Iterator[String] = {

      def map(iter: Iterator[String]): Iterator[String] = if(iter.hasNext) iter ++ space else iter

      i.foldLeft(Iterator.apply[String]())((sum, i) => sum ++ map(i))
    }

    def space: Iterator[String] = Iterator.apply("")

    def externC(inner: => Iterator[String]): Iterator[String] = {
      Iterator("#ifdef __cplusplus", """extern "C" {""", "#endif") ++ space ++
      inner ++ space ++
      Iterator("#ifdef __cplusplus", """}""", "#endif")
    }

    def commented(lines: Iterator[String]): Iterator[String] = {
      Iterator("//") ++ lines.map(l => "// " + l) ++ Iterator("//")
    }

    def merge(a: Iterator[Option[Iterator[String]]], b: Iterator[String]): Iterator[String] = {

      val iter = new Iterator[Iterator[String]] {
        def hasNext = a.hasNext && b.hasNext

        def next(): Iterator[String] = {
          val d = b.next()
          a.next() match {
            case Some(c) => c ++ Iterator(d)
            case None => Iterator(d)
          }
        }
      }

      iter.flatten
    }

    def bracketWithCap[A](indent: Indentation, cap: String)(inner: => Iterator[String]): Iterator[String] = {
      Iterator("{") ++
        indent(
          inner
        ) ++
        Iterator("}"+cap)
    }

    def bracket[A](inner: => Iterator[String])(implicit indent: Indentation): Iterator[String] = bracketWithCap(indent,"")(inner)


    def bracketSemiColon[A](inner: => Iterator[String])(implicit indent: Indentation): Iterator[String] = bracketWithCap(indent,";")(inner)

  def bracketSemiColon[A](cap: String)(inner: => Iterator[String])(implicit indent: Indentation): Iterator[String] = bracketWithCap(indent," %s;".format(cap))(inner)

    implicit class RichStringList(list: List[String]) {

      def spaced : String = list.mkString(" ")

    }

    def delimited(delim: String, last: Option[String] = None)(s: Iterator[String]) : Iterator[String] = new Iterator[String] {
      def hasNext: Boolean = s.hasNext

      def next(): String = {
        val ret = s.next()
        if(s.hasNext) (ret + delim) else {
          last match {
            case Some(l) => ret + l
            case None => ret
          }
        }
      }
    }

    def commaDelimitedWithSemicolon(s: Iterator[String]) : Iterator[String] = delimited(",", Some(";"))(s)

    def commaDelimited(s: Iterator[String]) : Iterator[String] = delimited(",")(s)

    def writeTo(path: Path)(lines: Iterator[String]): Unit = {

      if(!lines.isEmpty)
      {
        if(!Files.exists(path.getParent)) Files.createDirectory(path.getParent)
        if(!Files.exists(path)) Files.createFile(path)

        val writer = Files.newBufferedWriter( path, Charset.defaultCharset, StandardOpenOption.TRUNCATE_EXISTING)

        def writeLine(s: String) = {
          writer.write(s)
          writer.write(System.lineSeparator)
        }

        try { lines.foreach(writeLine) }
        finally { writer.close() }
      }



    }
}

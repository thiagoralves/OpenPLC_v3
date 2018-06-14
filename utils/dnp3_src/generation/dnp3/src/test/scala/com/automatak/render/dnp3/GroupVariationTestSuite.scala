package com.automatak.render.dnp3

import org.scalatest.FunSuite
import org.scalatest.Matchers
import org.scalatest.junit.JUnitRunner
import org.junit.runner.RunWith

import com.automatak.render.dnp3.objects.GroupVariation.Id
import com.automatak.render.dnp3.objects.ObjectGroup
import com.automatak.render.dnp3.objects.groups.Group12Var1

@RunWith(classOf[JUnitRunner])
class GroupVariationTestSuite extends FunSuite with Matchers {

  test("Groups are all unique") {
    ObjectGroup.all.foldLeft(Set.empty[Byte]) { (set, group) =>
      if(set(group.group)) fail("collision at " + group.group)
      set + group.group
    }
  }

  test("Object ids are not repeated") {

    val ids = for {
      group <- ObjectGroup.all
      gv <- group.objects
    } yield gv.id

    ids.foldLeft(Set.empty[Id]) { (set, id) =>
      if(set(id)) fail("collision at " + id)
      set + id
    }
  }

  test("Fixed size calculated correctly") {
    Group12Var1.size should equal(11)
  }

}

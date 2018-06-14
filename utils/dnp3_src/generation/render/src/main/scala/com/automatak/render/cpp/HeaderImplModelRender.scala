package com.automatak.render.cpp

import com.automatak.render.ModelRenderer

trait HeaderImplModelRender[A] {
  def header: ModelRenderer[A]
  def impl: ModelRenderer[A]
}

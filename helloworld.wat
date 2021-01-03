;; Copyright (c) 2020 the Mondradiko contributors.
;; SPDX-License-Identifier: LGPL-3.0-or-later

;; Copy this into test-folder, then run cmake/bundler.py to update the assets in World.cpp
(module
 (func $getPosition (import "" "TransformComponent_getPosition") (param i32) (result f64 f64 f64))
 (func $setPosition (import "" "TransformComponent_setPosition") (param i32 f64 f64 f64))
 (memory $memory 1)
 (export "memory" (memory $memory))
 (func $update (export "update")
  (f64.store (i32.const 0)
    (f64.add
      (f64.load (i32.const 0))
      (f64.const 0.001)
    )
  )
  (call $setPosition
   (i32.const 0)
   (f64.load (i32.const 0))
   (f64.const 0)
   (f64.const 0)
  )
 )
)
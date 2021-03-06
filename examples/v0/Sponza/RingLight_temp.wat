(module
 (type $i32_=>_i32 (func (param i32) (result i32)))
 (type $i32_i32_=>_none (func (param i32 i32)))
 (type $i32_f64_=>_none (func (param i32 f64)))
 (type $i32_=>_none (func (param i32)))
 (type $i32_i32_i32_=>_none (func (param i32 i32 i32)))
 (type $none_=>_none (func))
 (type $i32_i32_=>_i32 (func (param i32 i32) (result i32)))
 (type $i32_=>_f64 (func (param i32) (result f64)))
 (type $f64_f64_f64_=>_i32 (func (param f64 f64 f64) (result i32)))
 (type $f64_=>_f64 (func (param f64) (result f64)))
 (type $i32_i32_i32_i32_=>_none (func (param i32 i32 i32 i32)))
 (type $i32_f64_f64_f64_=>_none (func (param i32 f64 f64 f64)))
 (type $i64_=>_i32 (func (param i64) (result i32)))
 (type $f64_=>_i32 (func (param f64) (result i32)))
 (type $i32_i32_i32_=>_i32 (func (param i32 i32 i32) (result i32)))
 (type $i32_f64_f64_=>_i32 (func (param i32 f64 f64) (result i32)))
 (import "World" "World_spawnEntityAt" (func $components/World/World.spawnEntityAt (param f64 f64 f64) (result i32)))
 (import "Entity" "Entity#add<components/PointLightComponent/PointLightComponent>" (func $components/Entity/Entity#add<components/PointLightComponent/PointLightComponent> (param i32) (result i32)))
 (import "PointLightComponent" "PointLightComponent_setIntensity" (func $components/PointLightComponent/PointLightComponent#setIntensity (param i32 f64 f64 f64)))
 (import "env" "abort" (func $~lib/builtins/abort (param i32 i32 i32 i32)))
 (import "Entity" "Entity#get<components/PointLightComponent/PointLightComponent>" (func $components/Entity/Entity#get<components/PointLightComponent/PointLightComponent> (param i32) (result i32)))
 (memory $0 1)
 (data (i32.const 1024) "n\83\f9\a2\00\00\00\00\d1W\'\fc)\15DN\99\95b\db\c0\dd4\f5\abcQ\feA\90C<:n$\b7a\c5\bb\de\ea.I\06\e0\d2MB\1c\eb\1d\fe\1c\92\d1\t\f55\82\e8>\a7)\b1&p\9c\e9\84D\bb.9\d6\919A~_\b4\8b_\84\9c\f49S\83\ff\97\f8\1f;(\f9\bd\8b\11/\ef\0f\98\05\de\cf~6m\1fm\nZf?FO\b7\t\cb\'\c7\ba\'u-\ea_\9e\f79\07={\f1\e5\eb\b1_\fbk\ea\92R\8aF0\03V\08]\8d\1f \bc\cf\f0\abk{\fca\91\e3\a9\1d6\f4\9a_\85\99e\08\1b\e6^\80\d8\ff\8d@h\a0\14W\15\06\061\'sM")
 (data (i32.const 1228) ",")
 (data (i32.const 1240) "\01\00\00\00\1c\00\00\00I\00n\00v\00a\00l\00i\00d\00 \00l\00e\00n\00g\00t\00h")
 (data (i32.const 1276) ",")
 (data (i32.const 1288) "\01\00\00\00\1a\00\00\00~\00l\00i\00b\00/\00a\00r\00r\00a\00y\00.\00t\00s")
 (data (i32.const 1324) "<")
 (data (i32.const 1336) "\01\00\00\00(\00\00\00A\00l\00l\00o\00c\00a\00t\00i\00o\00n\00 \00t\00o\00o\00 \00l\00a\00r\00g\00e")
 (data (i32.const 1388) "<")
 (data (i32.const 1400) "\01\00\00\00\1e\00\00\00~\00l\00i\00b\00/\00r\00t\00/\00t\00c\00m\00s\00.\00t\00s")
 (data (i32.const 1452) "<")
 (data (i32.const 1464) "\01\00\00\00\1e\00\00\00~\00l\00i\00b\00/\00r\00t\00/\00t\00l\00s\00f\00.\00t\00s")
 (data (i32.const 1548) "<")
 (data (i32.const 1560) "\01\00\00\00$\00\00\00I\00n\00d\00e\00x\00 \00o\00u\00t\00 \00o\00f\00 \00r\00a\00n\00g\00e")
 (data (i32.const 1612) "|")
 (data (i32.const 1624) "\01\00\00\00^\00\00\00E\00l\00e\00m\00e\00n\00t\00 \00t\00y\00p\00e\00 \00m\00u\00s\00t\00 \00b\00e\00 \00n\00u\00l\00l\00a\00b\00l\00e\00 \00i\00f\00 \00a\00r\00r\00a\00y\00 \00i\00s\00 \00h\00o\00l\00e\00y")
 (data (i32.const 1740) "<")
 (data (i32.const 1752) "\01\00\00\00*\00\00\00O\00b\00j\00e\00c\00t\00 \00a\00l\00r\00e\00a\00d\00y\00 \00p\00i\00n\00n\00e\00d")
 (data (i32.const 1836) "<")
 (data (i32.const 1848) "\01\00\00\00(\00\00\00O\00b\00j\00e\00c\00t\00 \00i\00s\00 \00n\00o\00t\00 \00p\00i\00n\00n\00e\00d")
 (data (i32.const 1936) "\06\00\00\00 \00\00\00\00\00\00\00 ")
 (data (i32.const 1972) "\02\01\00\00\00\00\00\00 ")
 (global $~lib/math/rempio2_y0 (mut f64) (f64.const 0))
 (global $~lib/math/rempio2_y1 (mut f64) (f64.const 0))
 (global $~lib/math/res128_hi (mut i64) (i64.const 0))
 (global $~lib/rt/tlsf/ROOT (mut i32) (i32.const 0))
 (global $~lib/rt/tcms/fromSpace (mut i32) (i32.const 0))
 (global $~lib/rt/tcms/white (mut i32) (i32.const 0))
 (global $~lib/rt/tcms/total (mut i32) (i32.const 0))
 (global $RingLight/ring_light (mut i32) (i32.const 0))
 (global $RingLight/RingLight i32 (i32.const 3))
 (global $~lib/rt/tcms/pinSpace (mut i32) (i32.const 0))
 (global $~lib/rt/tcms/toSpace (mut i32) (i32.const 0))
 (global $~lib/rt/__rtti_base i32 (i32.const 1936))
 (export "RingLight" (global $RingLight/RingLight))
 (export "RingLight#get:lights" (func $RingLight/RingLight#get:lights))
 (export "RingLight#set:lights" (func $~lib/array/Array<components/Entity/Entity>#set:buffer))
 (export "RingLight#get:hue_offset" (func $RingLight/RingLight#get:hue_offset))
 (export "RingLight#set:hue_offset" (func $RingLight/RingLight#set:hue_offset))
 (export "RingLight#get:radius" (func $RingLight/RingLight#get:radius))
 (export "RingLight#set:radius" (func $RingLight/RingLight#set:radius))
 (export "RingLight#get:delta" (func $RingLight/RingLight#get:delta))
 (export "RingLight#set:delta" (func $RingLight/RingLight#set:delta))
 (export "RingLight#constructor" (func $RingLight/RingLight#constructor))
 (export "RingLight#update" (func $RingLight/RingLight#update))
 (export "update" (func $RingLight/update))
 (export "__new" (func $~lib/rt/tcms/__new))
 (export "__pin" (func $~lib/rt/tcms/__pin))
 (export "__unpin" (func $~lib/rt/tcms/__unpin))
 (export "__collect" (func $~lib/rt/tcms/__collect))
 (export "__rtti_base" (global $~lib/rt/__rtti_base))
 (export "memory" (memory $0))
 (start $~start)
 (func $~lib/math/pio2_large_quot (param $0 i64) (result i32)
  (local $1 i64)
  (local $2 i64)
  (local $3 i64)
  (local $4 i64)
  (local $5 i64)
  (local $6 i64)
  (local $7 i32)
  (local $8 i64)
  (local $9 i64)
  (local $10 i64)
  (local $11 i64)
  (local $12 f64)
  local.get $0
  i64.const 9223372036854775807
  i64.and
  i64.const 52
  i64.shr_u
  i64.const 1045
  i64.sub
  local.tee $4
  i64.const 6
  i64.shr_s
  i32.wrap_i64
  i32.const 3
  i32.shl
  i32.const 1024
  i32.add
  local.tee $7
  i64.load
  local.set $6
  local.get $7
  i64.load offset=8
  local.set $3
  local.get $7
  i64.load offset=16
  local.set $1
  local.get $4
  i64.const 63
  i64.and
  local.tee $4
  i64.const 0
  i64.ne
  if
   local.get $6
   local.get $4
   i64.shl
   local.get $3
   i64.const 64
   local.get $4
   i64.sub
   local.tee $2
   i64.shr_u
   i64.or
   local.set $6
   local.get $3
   local.get $4
   i64.shl
   local.get $1
   local.get $2
   i64.shr_u
   i64.or
   local.set $3
   local.get $1
   local.get $4
   i64.shl
   local.get $7
   i64.load offset=24
   local.get $2
   i64.shr_u
   i64.or
   local.set $1
  end
  local.get $0
  i64.const 4503599627370495
  i64.and
  i64.const 4503599627370496
  i64.or
  local.tee $4
  i64.const 4294967295
  i64.and
  local.tee $2
  local.get $3
  i64.const 32
  i64.shr_u
  local.tee $8
  i64.mul
  local.get $3
  i64.const 4294967295
  i64.and
  local.tee $5
  local.get $2
  i64.mul
  local.tee $9
  i64.const 32
  i64.shr_u
  i64.add
  local.set $3
  local.get $5
  local.get $4
  i64.const 32
  i64.shr_u
  local.tee $5
  i64.mul
  local.get $3
  i64.const 4294967295
  i64.and
  i64.add
  local.set $2
  local.get $5
  local.get $8
  i64.mul
  local.get $3
  i64.const 32
  i64.shr_u
  i64.add
  local.get $2
  i64.const 32
  i64.shr_u
  i64.add
  global.set $~lib/math/res128_hi
  local.get $4
  i64.const 32
  i64.shr_s
  local.get $1
  i64.const 32
  i64.shr_u
  i64.mul
  local.tee $3
  local.get $9
  i64.const 4294967295
  i64.and
  local.get $2
  i64.const 32
  i64.shl
  i64.add
  i64.add
  local.set $1
  local.get $1
  local.get $3
  i64.lt_u
  i64.extend_i32_u
  global.get $~lib/math/res128_hi
  local.get $4
  local.get $6
  i64.mul
  i64.add
  i64.add
  local.tee $8
  i64.const 2
  i64.shl
  local.get $1
  i64.const 62
  i64.shr_u
  i64.or
  local.tee $6
  i64.const 63
  i64.shr_s
  local.tee $4
  i64.const 1
  i64.shr_s
  local.get $6
  i64.xor
  local.tee $2
  i64.clz
  local.set $3
  local.get $2
  local.get $3
  i64.shl
  local.get $4
  local.get $1
  i64.const 2
  i64.shl
  i64.xor
  local.tee $5
  i64.const 64
  local.get $3
  i64.sub
  i64.shr_u
  i64.or
  local.tee $1
  i64.const 4294967295
  i64.and
  local.set $2
  local.get $1
  i64.const 32
  i64.shr_u
  local.tee $9
  i64.const 560513588
  i64.mul
  local.get $2
  i64.const 3373259426
  i64.mul
  local.get $2
  i64.const 560513588
  i64.mul
  local.tee $10
  i64.const 32
  i64.shr_u
  i64.add
  local.tee $11
  i64.const 4294967295
  i64.and
  i64.add
  local.set $2
  local.get $9
  i64.const 3373259426
  i64.mul
  local.get $11
  i64.const 32
  i64.shr_u
  i64.add
  local.get $2
  i64.const 32
  i64.shr_u
  i64.add
  global.set $~lib/math/res128_hi
  local.get $10
  i64.const 4294967295
  i64.and
  local.get $2
  i64.const 32
  i64.shl
  i64.add
  local.tee $2
  local.get $1
  f64.convert_i64_u
  f64.const 3.753184150245214e-04
  f64.mul
  local.get $5
  local.get $3
  i64.shl
  f64.convert_i64_u
  f64.const 3.834951969714103e-04
  f64.mul
  f64.add
  i64.trunc_f64_u
  local.tee $1
  i64.lt_u
  i64.extend_i32_u
  global.get $~lib/math/res128_hi
  local.tee $5
  i64.const 11
  i64.shr_u
  i64.add
  f64.convert_i64_u
  global.set $~lib/math/rempio2_y0
  local.get $1
  local.get $5
  i64.const 53
  i64.shl
  local.get $2
  i64.const 11
  i64.shr_u
  i64.or
  i64.add
  f64.convert_i64_u
  f64.const 5.421010862427522e-20
  f64.mul
  global.set $~lib/math/rempio2_y1
  global.get $~lib/math/rempio2_y0
  i64.const 4372995238176751616
  local.get $3
  i64.const 52
  i64.shl
  i64.sub
  local.get $0
  local.get $6
  i64.xor
  i64.const -9223372036854775808
  i64.and
  i64.or
  f64.reinterpret_i64
  local.tee $12
  f64.mul
  global.set $~lib/math/rempio2_y0
  global.get $~lib/math/rempio2_y1
  local.get $12
  f64.mul
  global.set $~lib/math/rempio2_y1
  local.get $8
  i64.const 62
  i64.shr_s
  local.get $4
  i64.sub
  i32.wrap_i64
 )
 (func $~lib/math/NativeMath.sin (param $0 f64) (result f64)
  (local $1 f64)
  (local $2 i64)
  (local $3 f64)
  (local $4 f64)
  (local $5 i32)
  (local $6 i32)
  (local $7 i32)
  (local $8 f64)
  local.get $0
  i64.reinterpret_f64
  local.tee $2
  i64.const 32
  i64.shr_u
  i32.wrap_i64
  local.tee $5
  i32.const 31
  i32.shr_u
  local.set $6
  local.get $5
  i32.const 2147483647
  i32.and
  local.tee $5
  i32.const 1072243195
  i32.le_u
  if
   local.get $5
   i32.const 1045430272
   i32.lt_u
   if
    local.get $0
    return
   end
   local.get $0
   local.get $0
   local.get $0
   f64.mul
   local.tee $3
   local.get $0
   f64.mul
   local.get $3
   local.get $3
   local.get $3
   f64.const 2.7557313707070068e-06
   f64.mul
   f64.const -1.984126982985795e-04
   f64.add
   f64.mul
   f64.const 0.00833333333332249
   f64.add
   local.get $3
   local.get $3
   local.get $3
   f64.mul
   f64.mul
   local.get $3
   f64.const 1.58969099521155e-10
   f64.mul
   f64.const -2.5050760253406863e-08
   f64.add
   f64.mul
   f64.add
   f64.mul
   f64.const -0.16666666666666632
   f64.add
   f64.mul
   f64.add
   return
  end
  local.get $5
  i32.const 2146435072
  i32.ge_u
  if
   local.get $0
   local.get $0
   f64.sub
   return
  end
  block $~lib/math/rempio2|inlined.0 (result i32)
   local.get $2
   i64.const 32
   i64.shr_u
   i32.wrap_i64
   i32.const 2147483647
   i32.and
   local.tee $7
   i32.const 1073928572
   i32.lt_u
   if
    i32.const 1
    local.set $5
    local.get $6
    if (result f64)
     local.get $0
     f64.const 1.5707963267341256
     f64.add
     local.set $0
     i32.const -1
     local.set $5
     local.get $7
     i32.const 1073291771
     i32.ne
     if (result f64)
      local.get $0
      local.get $0
      f64.const 6.077100506506192e-11
      f64.add
      local.tee $0
      f64.sub
      f64.const 6.077100506506192e-11
      f64.add
     else
      local.get $0
      f64.const 6.077100506303966e-11
      f64.add
      local.tee $3
      f64.const 2.0222662487959506e-21
      f64.add
      local.set $0
      local.get $3
      local.get $0
      f64.sub
      f64.const 2.0222662487959506e-21
      f64.add
     end
    else
     local.get $0
     f64.const 1.5707963267341256
     f64.sub
     local.set $0
     local.get $7
     i32.const 1073291771
     i32.ne
     if (result f64)
      local.get $0
      local.get $0
      f64.const 6.077100506506192e-11
      f64.sub
      local.tee $0
      f64.sub
      f64.const 6.077100506506192e-11
      f64.sub
     else
      local.get $0
      f64.const 6.077100506303966e-11
      f64.sub
      local.tee $3
      f64.const 2.0222662487959506e-21
      f64.sub
      local.set $0
      local.get $3
      local.get $0
      f64.sub
      f64.const 2.0222662487959506e-21
      f64.sub
     end
    end
    local.get $0
    global.set $~lib/math/rempio2_y0
    global.set $~lib/math/rempio2_y1
    local.get $5
    br $~lib/math/rempio2|inlined.0
   end
   local.get $7
   i32.const 1094263291
   i32.lt_u
   if
    local.get $7
    i32.const 20
    i32.shr_u
    local.tee $6
    local.get $0
    local.get $0
    f64.const 0.6366197723675814
    f64.mul
    f64.nearest
    local.tee $3
    f64.const 1.5707963267341256
    f64.mul
    f64.sub
    local.tee $0
    local.get $3
    f64.const 6.077100506506192e-11
    f64.mul
    local.tee $4
    f64.sub
    local.tee $1
    i64.reinterpret_f64
    i64.const 32
    i64.shr_u
    i32.wrap_i64
    i32.const 20
    i32.shr_u
    i32.const 2047
    i32.and
    i32.sub
    i32.const 16
    i32.gt_u
    if
     local.get $3
     f64.const 2.0222662487959506e-21
     f64.mul
     local.get $0
     local.get $0
     local.get $3
     f64.const 6.077100506303966e-11
     f64.mul
     local.tee $4
     f64.sub
     local.tee $0
     f64.sub
     local.get $4
     f64.sub
     f64.sub
     local.set $4
     local.get $6
     local.get $0
     local.get $4
     f64.sub
     local.tee $1
     i64.reinterpret_f64
     i64.const 32
     i64.shr_u
     i32.wrap_i64
     i32.const 20
     i32.shr_u
     i32.const 2047
     i32.and
     i32.sub
     i32.const 49
     i32.gt_u
     if (result f64)
      local.get $3
      f64.const 8.4784276603689e-32
      f64.mul
      local.get $0
      local.get $0
      local.get $3
      f64.const 2.0222662487111665e-21
      f64.mul
      local.tee $4
      f64.sub
      local.tee $0
      f64.sub
      local.get $4
      f64.sub
      f64.sub
      local.set $4
      local.get $0
      local.get $4
      f64.sub
     else
      local.get $1
     end
     local.set $1
    end
    local.get $1
    global.set $~lib/math/rempio2_y0
    local.get $0
    local.get $1
    f64.sub
    local.get $4
    f64.sub
    global.set $~lib/math/rempio2_y1
    local.get $3
    i32.trunc_f64_s
    br $~lib/math/rempio2|inlined.0
   end
   i32.const 0
   local.get $2
   call $~lib/math/pio2_large_quot
   local.tee $5
   i32.sub
   local.get $5
   local.get $6
   select
  end
  local.set $6
  global.get $~lib/math/rempio2_y0
  local.set $3
  global.get $~lib/math/rempio2_y1
  local.set $4
  local.get $6
  i32.const 1
  i32.and
  if (result f64)
   f64.const 1
   local.get $3
   local.get $3
   f64.mul
   local.tee $0
   f64.const 0.5
   f64.mul
   local.tee $1
   f64.sub
   local.tee $8
   f64.const 1
   local.get $8
   f64.sub
   local.get $1
   f64.sub
   local.get $0
   local.get $0
   local.get $0
   local.get $0
   f64.const 2.480158728947673e-05
   f64.mul
   f64.const -0.001388888888887411
   f64.add
   f64.mul
   f64.const 0.0416666666666666
   f64.add
   f64.mul
   local.get $0
   local.get $0
   f64.mul
   local.tee $1
   local.get $1
   f64.mul
   local.get $0
   local.get $0
   f64.const -1.1359647557788195e-11
   f64.mul
   f64.const 2.087572321298175e-09
   f64.add
   f64.mul
   f64.const -2.7557314351390663e-07
   f64.add
   f64.mul
   f64.add
   f64.mul
   local.get $3
   local.get $4
   f64.mul
   f64.sub
   f64.add
   f64.add
  else
   local.get $3
   local.get $3
   f64.mul
   local.tee $0
   local.get $3
   f64.mul
   local.set $1
   local.get $3
   local.get $0
   local.get $4
   f64.const 0.5
   f64.mul
   local.get $1
   local.get $0
   local.get $0
   f64.const 2.7557313707070068e-06
   f64.mul
   f64.const -1.984126982985795e-04
   f64.add
   f64.mul
   f64.const 0.00833333333332249
   f64.add
   local.get $0
   local.get $0
   local.get $0
   f64.mul
   f64.mul
   local.get $0
   f64.const 1.58969099521155e-10
   f64.mul
   f64.const -2.5050760253406863e-08
   f64.add
   f64.mul
   f64.add
   f64.mul
   f64.sub
   f64.mul
   local.get $4
   f64.sub
   local.get $1
   f64.const -0.16666666666666632
   f64.mul
   f64.sub
   f64.sub
  end
  local.tee $0
  f64.neg
  local.get $0
  local.get $6
  i32.const 2
  i32.and
  select
 )
 (func $~lib/math/NativeMath.cos (param $0 f64) (result f64)
  (local $1 f64)
  (local $2 i64)
  (local $3 f64)
  (local $4 f64)
  (local $5 i32)
  (local $6 i32)
  (local $7 i32)
  (local $8 f64)
  local.get $0
  i64.reinterpret_f64
  local.tee $2
  i64.const 32
  i64.shr_u
  i32.wrap_i64
  local.tee $5
  i32.const 31
  i32.shr_u
  local.set $6
  local.get $5
  i32.const 2147483647
  i32.and
  local.tee $5
  i32.const 1072243195
  i32.le_u
  if
   local.get $5
   i32.const 1044816030
   i32.lt_u
   if
    f64.const 1
    return
   end
   f64.const 1
   local.get $0
   local.get $0
   f64.mul
   local.tee $3
   f64.const 0.5
   f64.mul
   local.tee $4
   f64.sub
   local.tee $1
   f64.const 1
   local.get $1
   f64.sub
   local.get $4
   f64.sub
   local.get $3
   local.get $3
   local.get $3
   local.get $3
   f64.const 2.480158728947673e-05
   f64.mul
   f64.const -0.001388888888887411
   f64.add
   f64.mul
   f64.const 0.0416666666666666
   f64.add
   f64.mul
   local.get $3
   local.get $3
   f64.mul
   local.tee $4
   local.get $4
   f64.mul
   local.get $3
   local.get $3
   f64.const -1.1359647557788195e-11
   f64.mul
   f64.const 2.087572321298175e-09
   f64.add
   f64.mul
   f64.const -2.7557314351390663e-07
   f64.add
   f64.mul
   f64.add
   f64.mul
   local.get $0
   f64.const 0
   f64.mul
   f64.sub
   f64.add
   f64.add
   return
  end
  local.get $5
  i32.const 2146435072
  i32.ge_u
  if
   local.get $0
   local.get $0
   f64.sub
   return
  end
  block $~lib/math/rempio2|inlined.1 (result i32)
   local.get $2
   i64.const 32
   i64.shr_u
   i32.wrap_i64
   i32.const 2147483647
   i32.and
   local.tee $7
   i32.const 1073928572
   i32.lt_u
   if
    i32.const 1
    local.set $5
    local.get $6
    if (result f64)
     local.get $0
     f64.const 1.5707963267341256
     f64.add
     local.set $0
     i32.const -1
     local.set $5
     local.get $7
     i32.const 1073291771
     i32.ne
     if (result f64)
      local.get $0
      local.get $0
      f64.const 6.077100506506192e-11
      f64.add
      local.tee $0
      f64.sub
      f64.const 6.077100506506192e-11
      f64.add
     else
      local.get $0
      f64.const 6.077100506303966e-11
      f64.add
      local.tee $3
      f64.const 2.0222662487959506e-21
      f64.add
      local.set $0
      local.get $3
      local.get $0
      f64.sub
      f64.const 2.0222662487959506e-21
      f64.add
     end
    else
     local.get $0
     f64.const 1.5707963267341256
     f64.sub
     local.set $0
     local.get $7
     i32.const 1073291771
     i32.ne
     if (result f64)
      local.get $0
      local.get $0
      f64.const 6.077100506506192e-11
      f64.sub
      local.tee $0
      f64.sub
      f64.const 6.077100506506192e-11
      f64.sub
     else
      local.get $0
      f64.const 6.077100506303966e-11
      f64.sub
      local.tee $3
      f64.const 2.0222662487959506e-21
      f64.sub
      local.set $0
      local.get $3
      local.get $0
      f64.sub
      f64.const 2.0222662487959506e-21
      f64.sub
     end
    end
    local.get $0
    global.set $~lib/math/rempio2_y0
    global.set $~lib/math/rempio2_y1
    local.get $5
    br $~lib/math/rempio2|inlined.1
   end
   local.get $7
   i32.const 1094263291
   i32.lt_u
   if
    local.get $7
    i32.const 20
    i32.shr_u
    local.tee $6
    local.get $0
    local.get $0
    f64.const 0.6366197723675814
    f64.mul
    f64.nearest
    local.tee $3
    f64.const 1.5707963267341256
    f64.mul
    f64.sub
    local.tee $0
    local.get $3
    f64.const 6.077100506506192e-11
    f64.mul
    local.tee $4
    f64.sub
    local.tee $1
    i64.reinterpret_f64
    i64.const 32
    i64.shr_u
    i32.wrap_i64
    i32.const 20
    i32.shr_u
    i32.const 2047
    i32.and
    i32.sub
    i32.const 16
    i32.gt_u
    if
     local.get $3
     f64.const 2.0222662487959506e-21
     f64.mul
     local.get $0
     local.get $0
     local.get $3
     f64.const 6.077100506303966e-11
     f64.mul
     local.tee $4
     f64.sub
     local.tee $0
     f64.sub
     local.get $4
     f64.sub
     f64.sub
     local.set $4
     local.get $6
     local.get $0
     local.get $4
     f64.sub
     local.tee $1
     i64.reinterpret_f64
     i64.const 32
     i64.shr_u
     i32.wrap_i64
     i32.const 20
     i32.shr_u
     i32.const 2047
     i32.and
     i32.sub
     i32.const 49
     i32.gt_u
     if (result f64)
      local.get $3
      f64.const 8.4784276603689e-32
      f64.mul
      local.get $0
      local.get $0
      local.get $3
      f64.const 2.0222662487111665e-21
      f64.mul
      local.tee $4
      f64.sub
      local.tee $0
      f64.sub
      local.get $4
      f64.sub
      f64.sub
      local.set $4
      local.get $0
      local.get $4
      f64.sub
     else
      local.get $1
     end
     local.set $1
    end
    local.get $1
    global.set $~lib/math/rempio2_y0
    local.get $0
    local.get $1
    f64.sub
    local.get $4
    f64.sub
    global.set $~lib/math/rempio2_y1
    local.get $3
    i32.trunc_f64_s
    br $~lib/math/rempio2|inlined.1
   end
   i32.const 0
   local.get $2
   call $~lib/math/pio2_large_quot
   local.tee $5
   i32.sub
   local.get $5
   local.get $6
   select
  end
  local.set $6
  global.get $~lib/math/rempio2_y0
  local.set $3
  global.get $~lib/math/rempio2_y1
  local.set $4
  local.get $6
  i32.const 1
  i32.and
  if (result f64)
   local.get $3
   local.get $3
   f64.mul
   local.tee $0
   local.get $3
   f64.mul
   local.set $1
   local.get $3
   local.get $0
   local.get $4
   f64.const 0.5
   f64.mul
   local.get $1
   local.get $0
   local.get $0
   f64.const 2.7557313707070068e-06
   f64.mul
   f64.const -1.984126982985795e-04
   f64.add
   f64.mul
   f64.const 0.00833333333332249
   f64.add
   local.get $0
   local.get $0
   local.get $0
   f64.mul
   f64.mul
   local.get $0
   f64.const 1.58969099521155e-10
   f64.mul
   f64.const -2.5050760253406863e-08
   f64.add
   f64.mul
   f64.add
   f64.mul
   f64.sub
   f64.mul
   local.get $4
   f64.sub
   local.get $1
   f64.const -0.16666666666666632
   f64.mul
   f64.sub
   f64.sub
  else
   f64.const 1
   local.get $3
   local.get $3
   f64.mul
   local.tee $0
   f64.const 0.5
   f64.mul
   local.tee $1
   f64.sub
   local.tee $8
   f64.const 1
   local.get $8
   f64.sub
   local.get $1
   f64.sub
   local.get $0
   local.get $0
   local.get $0
   local.get $0
   f64.const 2.480158728947673e-05
   f64.mul
   f64.const -0.001388888888887411
   f64.add
   f64.mul
   f64.const 0.0416666666666666
   f64.add
   f64.mul
   local.get $0
   local.get $0
   f64.mul
   local.tee $1
   local.get $1
   f64.mul
   local.get $0
   local.get $0
   f64.const -1.1359647557788195e-11
   f64.mul
   f64.const 2.087572321298175e-09
   f64.add
   f64.mul
   f64.const -2.7557314351390663e-07
   f64.add
   f64.mul
   f64.add
   f64.mul
   local.get $3
   local.get $4
   f64.mul
   f64.sub
   f64.add
   f64.add
  end
  local.tee $0
  f64.neg
  local.get $0
  local.get $6
  i32.const 1
  i32.add
  i32.const 2
  i32.and
  select
 )
 (func $~lib/rt/tlsf/removeBlock (param $0 i32) (param $1 i32)
  (local $2 i32)
  (local $3 i32)
  (local $4 i32)
  (local $5 i32)
  local.get $1
  i32.load
  local.tee $2
  i32.const 1
  i32.and
  i32.eqz
  if
   i32.const 0
   i32.const 1472
   i32.const 268
   i32.const 14
   call $~lib/builtins/abort
   unreachable
  end
  local.get $2
  i32.const -4
  i32.and
  local.tee $2
  i32.const 12
  i32.lt_u
  if
   i32.const 0
   i32.const 1472
   i32.const 270
   i32.const 14
   call $~lib/builtins/abort
   unreachable
  end
  local.get $2
  i32.const 256
  i32.lt_u
  if
   local.get $2
   i32.const 4
   i32.shr_u
   local.set $2
  else
   i32.const 31
   local.get $2
   i32.const 1073741820
   local.get $2
   i32.const 1073741820
   i32.lt_u
   select
   local.tee $2
   i32.clz
   i32.sub
   local.set $3
   local.get $2
   local.get $3
   i32.const 4
   i32.sub
   i32.shr_u
   i32.const 16
   i32.xor
   local.set $2
   local.get $3
   i32.const 7
   i32.sub
   local.set $3
  end
  local.get $2
  i32.const 16
  i32.lt_u
  i32.const 0
  local.get $3
  i32.const 23
  i32.lt_u
  select
  i32.eqz
  if
   i32.const 0
   i32.const 1472
   i32.const 284
   i32.const 14
   call $~lib/builtins/abort
   unreachable
  end
  local.get $1
  i32.load offset=8
  local.set $4
  local.get $1
  i32.load offset=4
  local.tee $5
  if
   local.get $5
   local.get $4
   i32.store offset=8
  end
  local.get $4
  if
   local.get $4
   local.get $5
   i32.store offset=4
  end
  local.get $1
  local.get $0
  local.get $2
  local.get $3
  i32.const 4
  i32.shl
  i32.add
  i32.const 2
  i32.shl
  i32.add
  i32.load offset=96
  i32.eq
  if
   local.get $0
   local.get $2
   local.get $3
   i32.const 4
   i32.shl
   i32.add
   i32.const 2
   i32.shl
   i32.add
   local.get $4
   i32.store offset=96
   local.get $4
   i32.eqz
   if
    local.get $0
    local.get $3
    i32.const 2
    i32.shl
    i32.add
    local.tee $4
    i32.load offset=4
    i32.const -2
    local.get $2
    i32.rotl
    i32.and
    local.set $1
    local.get $4
    local.get $1
    i32.store offset=4
    local.get $1
    i32.eqz
    if
     local.get $0
     local.get $0
     i32.load
     i32.const -2
     local.get $3
     i32.rotl
     i32.and
     i32.store
    end
   end
  end
 )
 (func $~lib/rt/tlsf/insertBlock (param $0 i32) (param $1 i32)
  (local $2 i32)
  (local $3 i32)
  (local $4 i32)
  (local $5 i32)
  (local $6 i32)
  local.get $1
  i32.eqz
  if
   i32.const 0
   i32.const 1472
   i32.const 201
   i32.const 14
   call $~lib/builtins/abort
   unreachable
  end
  local.get $1
  i32.load
  local.tee $3
  i32.const 1
  i32.and
  i32.eqz
  if
   i32.const 0
   i32.const 1472
   i32.const 203
   i32.const 14
   call $~lib/builtins/abort
   unreachable
  end
  local.get $1
  i32.const 4
  i32.add
  local.get $1
  i32.load
  i32.const -4
  i32.and
  i32.add
  local.tee $4
  i32.load
  local.tee $2
  i32.const 1
  i32.and
  if
   local.get $0
   local.get $4
   call $~lib/rt/tlsf/removeBlock
   local.get $1
   local.get $3
   i32.const 4
   i32.add
   local.get $2
   i32.const -4
   i32.and
   i32.add
   local.tee $3
   i32.store
   local.get $1
   i32.const 4
   i32.add
   local.get $1
   i32.load
   i32.const -4
   i32.and
   i32.add
   local.tee $4
   i32.load
   local.set $2
  end
  local.get $3
  i32.const 2
  i32.and
  if
   local.get $1
   i32.const 4
   i32.sub
   i32.load
   local.tee $1
   i32.load
   local.tee $6
   i32.const 1
   i32.and
   i32.eqz
   if
    i32.const 0
    i32.const 1472
    i32.const 221
    i32.const 16
    call $~lib/builtins/abort
    unreachable
   end
   local.get $0
   local.get $1
   call $~lib/rt/tlsf/removeBlock
   local.get $1
   local.get $6
   i32.const 4
   i32.add
   local.get $3
   i32.const -4
   i32.and
   i32.add
   local.tee $3
   i32.store
  end
  local.get $4
  local.get $2
  i32.const 2
  i32.or
  i32.store
  local.get $3
  i32.const -4
  i32.and
  local.tee $3
  i32.const 12
  i32.lt_u
  if
   i32.const 0
   i32.const 1472
   i32.const 233
   i32.const 14
   call $~lib/builtins/abort
   unreachable
  end
  local.get $3
  local.get $1
  i32.const 4
  i32.add
  i32.add
  local.get $4
  i32.ne
  if
   i32.const 0
   i32.const 1472
   i32.const 234
   i32.const 14
   call $~lib/builtins/abort
   unreachable
  end
  local.get $4
  i32.const 4
  i32.sub
  local.get $1
  i32.store
  local.get $3
  i32.const 256
  i32.lt_u
  if (result i32)
   local.get $3
   i32.const 4
   i32.shr_u
  else
   i32.const 31
   local.get $3
   i32.const 1073741820
   local.get $3
   i32.const 1073741820
   i32.lt_u
   select
   local.tee $3
   i32.clz
   i32.sub
   local.tee $4
   i32.const 7
   i32.sub
   local.set $5
   local.get $3
   local.get $4
   i32.const 4
   i32.sub
   i32.shr_u
   i32.const 16
   i32.xor
  end
  local.tee $3
  i32.const 16
  i32.lt_u
  i32.const 0
  local.get $5
  i32.const 23
  i32.lt_u
  select
  i32.eqz
  if
   i32.const 0
   i32.const 1472
   i32.const 251
   i32.const 14
   call $~lib/builtins/abort
   unreachable
  end
  local.get $0
  local.get $3
  local.get $5
  i32.const 4
  i32.shl
  i32.add
  i32.const 2
  i32.shl
  i32.add
  i32.load offset=96
  local.set $4
  local.get $1
  i32.const 0
  i32.store offset=4
  local.get $1
  local.get $4
  i32.store offset=8
  local.get $4
  if
   local.get $4
   local.get $1
   i32.store offset=4
  end
  local.get $0
  local.get $3
  local.get $5
  i32.const 4
  i32.shl
  i32.add
  i32.const 2
  i32.shl
  i32.add
  local.get $1
  i32.store offset=96
  local.get $0
  local.get $0
  i32.load
  i32.const 1
  local.get $5
  i32.shl
  i32.or
  i32.store
  local.get $0
  local.get $5
  i32.const 2
  i32.shl
  i32.add
  local.tee $0
  local.get $0
  i32.load offset=4
  i32.const 1
  local.get $3
  i32.shl
  i32.or
  i32.store offset=4
 )
 (func $~lib/rt/tlsf/addMemory (param $0 i32) (param $1 i32) (param $2 i32)
  (local $3 i32)
  (local $4 i32)
  local.get $1
  local.get $2
  i32.gt_u
  if
   i32.const 0
   i32.const 1472
   i32.const 377
   i32.const 14
   call $~lib/builtins/abort
   unreachable
  end
  local.get $1
  i32.const 19
  i32.add
  i32.const -16
  i32.and
  i32.const 4
  i32.sub
  local.set $1
  local.get $2
  i32.const -16
  i32.and
  local.get $0
  i32.load offset=1568
  local.tee $2
  if
   local.get $1
   local.get $2
   i32.const 4
   i32.add
   i32.lt_u
   if
    i32.const 0
    i32.const 1472
    i32.const 384
    i32.const 16
    call $~lib/builtins/abort
    unreachable
   end
   local.get $2
   local.get $1
   i32.const 16
   i32.sub
   i32.eq
   if
    local.get $2
    i32.load
    local.set $4
    local.get $1
    i32.const 16
    i32.sub
    local.set $1
   end
  else
   local.get $1
   local.get $0
   i32.const 1572
   i32.add
   i32.lt_u
   if
    i32.const 0
    i32.const 1472
    i32.const 397
    i32.const 5
    call $~lib/builtins/abort
    unreachable
   end
  end
  local.get $1
  i32.sub
  local.tee $2
  i32.const 20
  i32.lt_u
  if
   return
  end
  local.get $1
  local.get $4
  i32.const 2
  i32.and
  local.get $2
  i32.const 8
  i32.sub
  local.tee $2
  i32.const 1
  i32.or
  i32.or
  i32.store
  local.get $1
  i32.const 0
  i32.store offset=4
  local.get $1
  i32.const 0
  i32.store offset=8
  local.get $2
  local.get $1
  i32.const 4
  i32.add
  i32.add
  local.tee $2
  i32.const 2
  i32.store
  local.get $0
  local.get $2
  i32.store offset=1568
  local.get $0
  local.get $1
  call $~lib/rt/tlsf/insertBlock
 )
 (func $~lib/rt/tlsf/initialize
  (local $0 i32)
  (local $1 i32)
  memory.size
  local.tee $0
  i32.const 1
  i32.lt_s
  if (result i32)
   i32.const 1
   local.get $0
   i32.sub
   memory.grow
   i32.const 0
   i32.lt_s
  else
   i32.const 0
  end
  if
   unreachable
  end
  i32.const 2000
  i32.const 0
  i32.store
  i32.const 3568
  i32.const 0
  i32.store
  loop $for-loop|0
   local.get $1
   i32.const 23
   i32.lt_u
   if
    local.get $1
    i32.const 2
    i32.shl
    i32.const 2000
    i32.add
    i32.const 0
    i32.store offset=4
    i32.const 0
    local.set $0
    loop $for-loop|1
     local.get $0
     i32.const 16
     i32.lt_u
     if
      local.get $0
      local.get $1
      i32.const 4
      i32.shl
      i32.add
      i32.const 2
      i32.shl
      i32.const 2000
      i32.add
      i32.const 0
      i32.store offset=96
      local.get $0
      i32.const 1
      i32.add
      local.set $0
      br $for-loop|1
     end
    end
    local.get $1
    i32.const 1
    i32.add
    local.set $1
    br $for-loop|0
   end
  end
  i32.const 2000
  i32.const 3572
  memory.size
  i32.const 16
  i32.shl
  call $~lib/rt/tlsf/addMemory
  i32.const 2000
  global.set $~lib/rt/tlsf/ROOT
 )
 (func $~lib/rt/tlsf/prepareSize (param $0 i32) (result i32)
  local.get $0
  i32.const 1073741820
  i32.ge_u
  if
   i32.const 1344
   i32.const 1472
   i32.const 458
   i32.const 30
   call $~lib/builtins/abort
   unreachable
  end
  local.get $0
  i32.const 12
  i32.le_u
  if (result i32)
   i32.const 12
  else
   local.get $0
   i32.const 19
   i32.add
   i32.const -16
   i32.and
   i32.const 4
   i32.sub
  end
 )
 (func $~lib/rt/tlsf/searchBlock (param $0 i32) (param $1 i32) (result i32)
  (local $2 i32)
  local.get $1
  i32.const 256
  i32.lt_u
  if
   local.get $1
   i32.const 4
   i32.shr_u
   local.set $1
  else
   local.get $1
   i32.const 536870910
   i32.lt_u
   if
    local.get $1
    i32.const 1
    i32.const 27
    local.get $1
    i32.clz
    i32.sub
    i32.shl
    i32.add
    i32.const 1
    i32.sub
    local.set $1
   end
   local.get $1
   i32.const 31
   local.get $1
   i32.clz
   i32.sub
   local.tee $2
   i32.const 4
   i32.sub
   i32.shr_u
   i32.const 16
   i32.xor
   local.set $1
   local.get $2
   i32.const 7
   i32.sub
   local.set $2
  end
  local.get $1
  i32.const 16
  i32.lt_u
  i32.const 0
  local.get $2
  i32.const 23
  i32.lt_u
  select
  i32.eqz
  if
   i32.const 0
   i32.const 1472
   i32.const 330
   i32.const 14
   call $~lib/builtins/abort
   unreachable
  end
  local.get $0
  local.get $2
  i32.const 2
  i32.shl
  i32.add
  i32.load offset=4
  i32.const -1
  local.get $1
  i32.shl
  i32.and
  local.tee $1
  if (result i32)
   local.get $0
   local.get $1
   i32.ctz
   local.get $2
   i32.const 4
   i32.shl
   i32.add
   i32.const 2
   i32.shl
   i32.add
   i32.load offset=96
  else
   local.get $0
   i32.load
   i32.const -1
   local.get $2
   i32.const 1
   i32.add
   i32.shl
   i32.and
   local.tee $1
   if (result i32)
    local.get $0
    local.get $1
    i32.ctz
    local.tee $1
    i32.const 2
    i32.shl
    i32.add
    i32.load offset=4
    local.tee $2
    i32.eqz
    if
     i32.const 0
     i32.const 1472
     i32.const 343
     i32.const 18
     call $~lib/builtins/abort
     unreachable
    end
    local.get $0
    local.get $2
    i32.ctz
    local.get $1
    i32.const 4
    i32.shl
    i32.add
    i32.const 2
    i32.shl
    i32.add
    i32.load offset=96
   else
    i32.const 0
   end
  end
 )
 (func $~lib/rt/tlsf/prepareBlock (param $0 i32) (param $1 i32) (param $2 i32)
  (local $3 i32)
  (local $4 i32)
  local.get $1
  i32.load
  local.set $3
  local.get $2
  i32.const 4
  i32.add
  i32.const 15
  i32.and
  if
   i32.const 0
   i32.const 1472
   i32.const 357
   i32.const 14
   call $~lib/builtins/abort
   unreachable
  end
  local.get $3
  i32.const -4
  i32.and
  local.get $2
  i32.sub
  local.tee $4
  i32.const 16
  i32.ge_u
  if
   local.get $1
   local.get $2
   local.get $3
   i32.const 2
   i32.and
   i32.or
   i32.store
   local.get $2
   local.get $1
   i32.const 4
   i32.add
   i32.add
   local.tee $1
   local.get $4
   i32.const 4
   i32.sub
   i32.const 1
   i32.or
   i32.store
   local.get $0
   local.get $1
   call $~lib/rt/tlsf/insertBlock
  else
   local.get $1
   local.get $3
   i32.const -2
   i32.and
   i32.store
   local.get $1
   i32.const 4
   i32.add
   local.tee $0
   local.get $1
   i32.load
   i32.const -4
   i32.and
   i32.add
   local.get $0
   local.get $1
   i32.load
   i32.const -4
   i32.and
   i32.add
   i32.load
   i32.const -3
   i32.and
   i32.store
  end
 )
 (func $~lib/rt/tlsf/allocateBlock (param $0 i32) (param $1 i32) (result i32)
  (local $2 i32)
  (local $3 i32)
  local.get $0
  local.get $1
  call $~lib/rt/tlsf/prepareSize
  local.tee $1
  call $~lib/rt/tlsf/searchBlock
  local.tee $2
  i32.eqz
  if
   local.get $1
   i32.const 536870910
   i32.lt_u
   if (result i32)
    local.get $1
    i32.const 1
    i32.const 27
    local.get $1
    i32.clz
    i32.sub
    i32.shl
    i32.const 1
    i32.sub
    i32.add
   else
    local.get $1
   end
   i32.const 4
   memory.size
   local.tee $2
   i32.const 16
   i32.shl
   i32.const 4
   i32.sub
   local.get $0
   i32.load offset=1568
   i32.ne
   i32.shl
   i32.add
   i32.const 65535
   i32.add
   i32.const -65536
   i32.and
   i32.const 16
   i32.shr_u
   local.set $3
   local.get $2
   local.get $3
   local.get $2
   local.get $3
   i32.gt_s
   select
   memory.grow
   i32.const 0
   i32.lt_s
   if
    local.get $3
    memory.grow
    i32.const 0
    i32.lt_s
    if
     unreachable
    end
   end
   local.get $0
   local.get $2
   i32.const 16
   i32.shl
   memory.size
   i32.const 16
   i32.shl
   call $~lib/rt/tlsf/addMemory
   local.get $0
   local.get $1
   call $~lib/rt/tlsf/searchBlock
   local.tee $2
   i32.eqz
   if
    i32.const 0
    i32.const 1472
    i32.const 496
    i32.const 16
    call $~lib/builtins/abort
    unreachable
   end
  end
  local.get $2
  i32.load
  i32.const -4
  i32.and
  local.get $1
  i32.lt_u
  if
   i32.const 0
   i32.const 1472
   i32.const 498
   i32.const 14
   call $~lib/builtins/abort
   unreachable
  end
  local.get $0
  local.get $2
  call $~lib/rt/tlsf/removeBlock
  local.get $0
  local.get $2
  local.get $1
  call $~lib/rt/tlsf/prepareBlock
  local.get $2
 )
 (func $~lib/rt/tcms/__new (param $0 i32) (param $1 i32) (result i32)
  (local $2 i32)
  local.get $0
  i32.const 1073741804
  i32.ge_u
  if
   i32.const 1344
   i32.const 1408
   i32.const 125
   i32.const 31
   call $~lib/builtins/abort
   unreachable
  end
  local.get $0
  i32.const 16
  i32.add
  local.set $2
  global.get $~lib/rt/tlsf/ROOT
  i32.eqz
  if
   call $~lib/rt/tlsf/initialize
  end
  global.get $~lib/rt/tlsf/ROOT
  local.get $2
  call $~lib/rt/tlsf/allocateBlock
  local.tee $2
  local.get $1
  i32.store offset=12
  local.get $2
  local.get $0
  i32.store offset=16
  global.get $~lib/rt/tcms/fromSpace
  local.tee $1
  i32.load offset=8
  local.set $0
  local.get $2
  local.get $1
  global.get $~lib/rt/tcms/white
  i32.or
  i32.store offset=4
  local.get $2
  local.get $0
  i32.store offset=8
  local.get $0
  local.get $2
  local.get $0
  i32.load offset=4
  i32.const 3
  i32.and
  i32.or
  i32.store offset=4
  local.get $1
  local.get $2
  i32.store offset=8
  global.get $~lib/rt/tcms/total
  local.get $2
  i32.load
  i32.const -4
  i32.and
  i32.const 4
  i32.add
  i32.add
  global.set $~lib/rt/tcms/total
  local.get $2
  i32.const 20
  i32.add
 )
 (func $~lib/util/memory/memcpy (param $0 i32) (param $1 i32) (param $2 i32)
  (local $3 i32)
  (local $4 i32)
  (local $5 i32)
  loop $while-continue|0
   local.get $1
   i32.const 3
   i32.and
   i32.const 0
   local.get $2
   select
   if
    local.get $0
    local.tee $3
    i32.const 1
    i32.add
    local.set $0
    local.get $1
    local.tee $4
    i32.const 1
    i32.add
    local.set $1
    local.get $3
    local.get $4
    i32.load8_u
    i32.store8
    local.get $2
    i32.const 1
    i32.sub
    local.set $2
    br $while-continue|0
   end
  end
  local.get $0
  i32.const 3
  i32.and
  i32.eqz
  if
   loop $while-continue|1
    local.get $2
    i32.const 16
    i32.ge_u
    if
     local.get $0
     local.get $1
     i32.load
     i32.store
     local.get $0
     local.get $1
     i32.load offset=4
     i32.store offset=4
     local.get $0
     local.get $1
     i32.load offset=8
     i32.store offset=8
     local.get $0
     local.get $1
     i32.load offset=12
     i32.store offset=12
     local.get $1
     i32.const 16
     i32.add
     local.set $1
     local.get $0
     i32.const 16
     i32.add
     local.set $0
     local.get $2
     i32.const 16
     i32.sub
     local.set $2
     br $while-continue|1
    end
   end
   local.get $2
   i32.const 8
   i32.and
   if
    local.get $0
    local.get $1
    i32.load
    i32.store
    local.get $0
    local.get $1
    i32.load offset=4
    i32.store offset=4
    local.get $1
    i32.const 8
    i32.add
    local.set $1
    local.get $0
    i32.const 8
    i32.add
    local.set $0
   end
   local.get $2
   i32.const 4
   i32.and
   if
    local.get $0
    local.get $1
    i32.load
    i32.store
    local.get $1
    i32.const 4
    i32.add
    local.set $1
    local.get $0
    i32.const 4
    i32.add
    local.set $0
   end
   local.get $2
   i32.const 2
   i32.and
   if
    local.get $0
    local.get $1
    i32.load16_u
    i32.store16
    local.get $1
    i32.const 2
    i32.add
    local.set $1
    local.get $0
    i32.const 2
    i32.add
    local.set $0
   end
   local.get $2
   i32.const 1
   i32.and
   if
    local.get $0
    local.get $1
    i32.load8_u
    i32.store8
   end
   return
  end
  local.get $2
  i32.const 32
  i32.ge_u
  if
   block $break|2
    block $case2|2
     block $case1|2
      block $case0|2
       local.get $0
       i32.const 3
       i32.and
       i32.const 1
       i32.sub
       br_table $case0|2 $case1|2 $case2|2 $break|2
      end
      local.get $1
      i32.load
      local.set $5
      local.get $0
      local.get $1
      i32.load8_u
      i32.store8
      local.get $0
      i32.const 1
      i32.add
      local.tee $0
      local.get $1
      i32.const 1
      i32.add
      local.tee $1
      i32.load8_u
      i32.store8
      local.get $0
      local.tee $4
      i32.const 2
      i32.add
      local.set $0
      local.get $1
      local.tee $3
      i32.const 2
      i32.add
      local.set $1
      local.get $4
      local.get $3
      i32.load8_u offset=1
      i32.store8 offset=1
      local.get $2
      i32.const 3
      i32.sub
      local.set $2
      loop $while-continue|3
       local.get $2
       i32.const 17
       i32.ge_u
       if
        local.get $0
        local.get $1
        i32.load offset=1
        local.tee $3
        i32.const 8
        i32.shl
        local.get $5
        i32.const 24
        i32.shr_u
        i32.or
        i32.store
        local.get $0
        local.get $3
        i32.const 24
        i32.shr_u
        local.get $1
        i32.load offset=5
        local.tee $3
        i32.const 8
        i32.shl
        i32.or
        i32.store offset=4
        local.get $0
        local.get $3
        i32.const 24
        i32.shr_u
        local.get $1
        i32.load offset=9
        local.tee $3
        i32.const 8
        i32.shl
        i32.or
        i32.store offset=8
        local.get $0
        local.get $1
        i32.load offset=13
        local.tee $5
        i32.const 8
        i32.shl
        local.get $3
        i32.const 24
        i32.shr_u
        i32.or
        i32.store offset=12
        local.get $1
        i32.const 16
        i32.add
        local.set $1
        local.get $0
        i32.const 16
        i32.add
        local.set $0
        local.get $2
        i32.const 16
        i32.sub
        local.set $2
        br $while-continue|3
       end
      end
      br $break|2
     end
     local.get $1
     i32.load
     local.set $5
     local.get $0
     local.get $1
     i32.load8_u
     i32.store8
     local.get $0
     local.tee $4
     i32.const 2
     i32.add
     local.set $0
     local.get $1
     local.tee $3
     i32.const 2
     i32.add
     local.set $1
     local.get $4
     local.get $3
     i32.load8_u offset=1
     i32.store8 offset=1
     local.get $2
     i32.const 2
     i32.sub
     local.set $2
     loop $while-continue|4
      local.get $2
      i32.const 18
      i32.ge_u
      if
       local.get $0
       local.get $1
       i32.load offset=2
       local.tee $3
       i32.const 16
       i32.shl
       local.get $5
       i32.const 16
       i32.shr_u
       i32.or
       i32.store
       local.get $0
       local.get $3
       i32.const 16
       i32.shr_u
       local.get $1
       i32.load offset=6
       local.tee $3
       i32.const 16
       i32.shl
       i32.or
       i32.store offset=4
       local.get $0
       local.get $3
       i32.const 16
       i32.shr_u
       local.get $1
       i32.load offset=10
       local.tee $3
       i32.const 16
       i32.shl
       i32.or
       i32.store offset=8
       local.get $0
       local.get $1
       i32.load offset=14
       local.tee $5
       i32.const 16
       i32.shl
       local.get $3
       i32.const 16
       i32.shr_u
       i32.or
       i32.store offset=12
       local.get $1
       i32.const 16
       i32.add
       local.set $1
       local.get $0
       i32.const 16
       i32.add
       local.set $0
       local.get $2
       i32.const 16
       i32.sub
       local.set $2
       br $while-continue|4
      end
     end
     br $break|2
    end
    local.get $1
    i32.load
    local.set $5
    local.get $0
    local.tee $3
    i32.const 1
    i32.add
    local.set $0
    local.get $1
    local.tee $4
    i32.const 1
    i32.add
    local.set $1
    local.get $3
    local.get $4
    i32.load8_u
    i32.store8
    local.get $2
    i32.const 1
    i32.sub
    local.set $2
    loop $while-continue|5
     local.get $2
     i32.const 19
     i32.ge_u
     if
      local.get $0
      local.get $1
      i32.load offset=3
      local.tee $3
      i32.const 24
      i32.shl
      local.get $5
      i32.const 8
      i32.shr_u
      i32.or
      i32.store
      local.get $0
      local.get $3
      i32.const 8
      i32.shr_u
      local.get $1
      i32.load offset=7
      local.tee $3
      i32.const 24
      i32.shl
      i32.or
      i32.store offset=4
      local.get $0
      local.get $3
      i32.const 8
      i32.shr_u
      local.get $1
      i32.load offset=11
      local.tee $3
      i32.const 24
      i32.shl
      i32.or
      i32.store offset=8
      local.get $0
      local.get $1
      i32.load offset=15
      local.tee $5
      i32.const 24
      i32.shl
      local.get $3
      i32.const 8
      i32.shr_u
      i32.or
      i32.store offset=12
      local.get $1
      i32.const 16
      i32.add
      local.set $1
      local.get $0
      i32.const 16
      i32.add
      local.set $0
      local.get $2
      i32.const 16
      i32.sub
      local.set $2
      br $while-continue|5
     end
    end
   end
  end
  local.get $2
  i32.const 16
  i32.and
  if
   local.get $0
   local.get $1
   i32.load8_u
   i32.store8
   local.get $0
   i32.const 1
   i32.add
   local.tee $0
   local.get $1
   i32.const 1
   i32.add
   local.tee $1
   i32.load8_u
   i32.store8
   local.get $0
   i32.const 1
   i32.add
   local.tee $0
   local.get $1
   i32.const 1
   i32.add
   local.tee $1
   i32.load8_u
   i32.store8
   local.get $0
   i32.const 1
   i32.add
   local.tee $0
   local.get $1
   i32.const 1
   i32.add
   local.tee $1
   i32.load8_u
   i32.store8
   local.get $0
   i32.const 1
   i32.add
   local.tee $0
   local.get $1
   i32.const 1
   i32.add
   local.tee $1
   i32.load8_u
   i32.store8
   local.get $0
   i32.const 1
   i32.add
   local.tee $0
   local.get $1
   i32.const 1
   i32.add
   local.tee $1
   i32.load8_u
   i32.store8
   local.get $0
   i32.const 1
   i32.add
   local.tee $0
   local.get $1
   i32.const 1
   i32.add
   local.tee $1
   i32.load8_u
   i32.store8
   local.get $0
   i32.const 1
   i32.add
   local.tee $0
   local.get $1
   i32.const 1
   i32.add
   local.tee $1
   i32.load8_u
   i32.store8
   local.get $0
   i32.const 1
   i32.add
   local.tee $0
   local.get $1
   i32.const 1
   i32.add
   local.tee $1
   i32.load8_u
   i32.store8
   local.get $0
   i32.const 1
   i32.add
   local.tee $0
   local.get $1
   i32.const 1
   i32.add
   local.tee $1
   i32.load8_u
   i32.store8
   local.get $0
   i32.const 1
   i32.add
   local.tee $0
   local.get $1
   i32.const 1
   i32.add
   local.tee $1
   i32.load8_u
   i32.store8
   local.get $0
   i32.const 1
   i32.add
   local.tee $0
   local.get $1
   i32.const 1
   i32.add
   local.tee $1
   i32.load8_u
   i32.store8
   local.get $0
   i32.const 1
   i32.add
   local.tee $0
   local.get $1
   i32.const 1
   i32.add
   local.tee $1
   i32.load8_u
   i32.store8
   local.get $0
   i32.const 1
   i32.add
   local.tee $0
   local.get $1
   i32.const 1
   i32.add
   local.tee $1
   i32.load8_u
   i32.store8
   local.get $0
   i32.const 1
   i32.add
   local.tee $0
   local.get $1
   i32.const 1
   i32.add
   local.tee $1
   i32.load8_u
   i32.store8
   local.get $0
   local.tee $4
   i32.const 2
   i32.add
   local.set $0
   local.get $1
   local.tee $3
   i32.const 2
   i32.add
   local.set $1
   local.get $4
   local.get $3
   i32.load8_u offset=1
   i32.store8 offset=1
  end
  local.get $2
  i32.const 8
  i32.and
  if
   local.get $0
   local.get $1
   i32.load8_u
   i32.store8
   local.get $0
   i32.const 1
   i32.add
   local.tee $0
   local.get $1
   i32.const 1
   i32.add
   local.tee $1
   i32.load8_u
   i32.store8
   local.get $0
   i32.const 1
   i32.add
   local.tee $0
   local.get $1
   i32.const 1
   i32.add
   local.tee $1
   i32.load8_u
   i32.store8
   local.get $0
   i32.const 1
   i32.add
   local.tee $0
   local.get $1
   i32.const 1
   i32.add
   local.tee $1
   i32.load8_u
   i32.store8
   local.get $0
   i32.const 1
   i32.add
   local.tee $0
   local.get $1
   i32.const 1
   i32.add
   local.tee $1
   i32.load8_u
   i32.store8
   local.get $0
   i32.const 1
   i32.add
   local.tee $0
   local.get $1
   i32.const 1
   i32.add
   local.tee $1
   i32.load8_u
   i32.store8
   local.get $0
   i32.const 1
   i32.add
   local.tee $0
   local.get $1
   i32.const 1
   i32.add
   local.tee $1
   i32.load8_u
   i32.store8
   local.get $0
   local.tee $4
   i32.const 2
   i32.add
   local.set $0
   local.get $1
   local.tee $3
   i32.const 2
   i32.add
   local.set $1
   local.get $4
   local.get $3
   i32.load8_u offset=1
   i32.store8 offset=1
  end
  local.get $2
  i32.const 4
  i32.and
  if
   local.get $0
   local.get $1
   i32.load8_u
   i32.store8
   local.get $0
   i32.const 1
   i32.add
   local.tee $0
   local.get $1
   i32.const 1
   i32.add
   local.tee $1
   i32.load8_u
   i32.store8
   local.get $0
   i32.const 1
   i32.add
   local.tee $0
   local.get $1
   i32.const 1
   i32.add
   local.tee $1
   i32.load8_u
   i32.store8
   local.get $0
   local.tee $4
   i32.const 2
   i32.add
   local.set $0
   local.get $1
   local.tee $3
   i32.const 2
   i32.add
   local.set $1
   local.get $4
   local.get $3
   i32.load8_u offset=1
   i32.store8 offset=1
  end
  local.get $2
  i32.const 2
  i32.and
  if
   local.get $0
   local.get $1
   i32.load8_u
   i32.store8
   local.get $0
   local.tee $4
   i32.const 2
   i32.add
   local.set $0
   local.get $1
   local.tee $3
   i32.const 2
   i32.add
   local.set $1
   local.get $4
   local.get $3
   i32.load8_u offset=1
   i32.store8 offset=1
  end
  local.get $2
  i32.const 1
  i32.and
  if
   local.get $0
   local.get $1
   i32.load8_u
   i32.store8
  end
 )
 (func $~lib/memory/memory.copy (param $0 i32) (param $1 i32) (param $2 i32)
  (local $3 i32)
  (local $4 i32)
  block $~lib/util/memory/memmove|inlined.0
   local.get $2
   local.set $4
   local.get $0
   local.get $1
   i32.eq
   br_if $~lib/util/memory/memmove|inlined.0
   local.get $1
   local.get $0
   i32.sub
   local.get $4
   i32.sub
   i32.const 0
   local.get $4
   i32.const 1
   i32.shl
   i32.sub
   i32.le_u
   if
    local.get $0
    local.get $1
    local.get $4
    call $~lib/util/memory/memcpy
    br $~lib/util/memory/memmove|inlined.0
   end
   local.get $0
   local.get $1
   i32.lt_u
   if
    local.get $1
    i32.const 7
    i32.and
    local.get $0
    i32.const 7
    i32.and
    i32.eq
    if
     loop $while-continue|0
      local.get $0
      i32.const 7
      i32.and
      if
       local.get $4
       i32.eqz
       br_if $~lib/util/memory/memmove|inlined.0
       local.get $4
       i32.const 1
       i32.sub
       local.set $4
       local.get $0
       local.tee $2
       i32.const 1
       i32.add
       local.set $0
       local.get $1
       local.tee $3
       i32.const 1
       i32.add
       local.set $1
       local.get $2
       local.get $3
       i32.load8_u
       i32.store8
       br $while-continue|0
      end
     end
     loop $while-continue|1
      local.get $4
      i32.const 8
      i32.ge_u
      if
       local.get $0
       local.get $1
       i64.load
       i64.store
       local.get $4
       i32.const 8
       i32.sub
       local.set $4
       local.get $0
       i32.const 8
       i32.add
       local.set $0
       local.get $1
       i32.const 8
       i32.add
       local.set $1
       br $while-continue|1
      end
     end
    end
    loop $while-continue|2
     local.get $4
     if
      local.get $0
      local.tee $2
      i32.const 1
      i32.add
      local.set $0
      local.get $1
      local.tee $3
      i32.const 1
      i32.add
      local.set $1
      local.get $2
      local.get $3
      i32.load8_u
      i32.store8
      local.get $4
      i32.const 1
      i32.sub
      local.set $4
      br $while-continue|2
     end
    end
   else
    local.get $1
    i32.const 7
    i32.and
    local.get $0
    i32.const 7
    i32.and
    i32.eq
    if
     loop $while-continue|3
      local.get $0
      local.get $4
      i32.add
      i32.const 7
      i32.and
      if
       local.get $4
       i32.eqz
       br_if $~lib/util/memory/memmove|inlined.0
       local.get $4
       i32.const 1
       i32.sub
       local.tee $4
       local.get $0
       i32.add
       local.get $1
       local.get $4
       i32.add
       i32.load8_u
       i32.store8
       br $while-continue|3
      end
     end
     loop $while-continue|4
      local.get $4
      i32.const 8
      i32.ge_u
      if
       local.get $4
       i32.const 8
       i32.sub
       local.tee $4
       local.get $0
       i32.add
       local.get $1
       local.get $4
       i32.add
       i64.load
       i64.store
       br $while-continue|4
      end
     end
    end
    loop $while-continue|5
     local.get $4
     if
      local.get $4
      i32.const 1
      i32.sub
      local.tee $4
      local.get $0
      i32.add
      local.get $1
      local.get $4
      i32.add
      i32.load8_u
      i32.store8
      br $while-continue|5
     end
    end
   end
  end
 )
 (func $~lib/rt/tlsf/checkUsedBlock (param $0 i32) (result i32)
  (local $1 i32)
  local.get $0
  i32.const 4
  i32.sub
  local.set $1
  local.get $0
  i32.const 15
  i32.and
  i32.eqz
  i32.const 0
  local.get $0
  select
  if (result i32)
   local.get $1
   i32.load
   i32.const 1
   i32.and
   i32.eqz
  else
   i32.const 0
  end
  i32.eqz
  if
   i32.const 0
   i32.const 1472
   i32.const 559
   i32.const 3
   call $~lib/builtins/abort
   unreachable
  end
  local.get $1
 )
 (func $~lib/rt/tlsf/moveBlock (param $0 i32) (param $1 i32) (param $2 i32) (result i32)
  local.get $0
  local.get $2
  call $~lib/rt/tlsf/allocateBlock
  local.tee $2
  i32.const 4
  i32.add
  local.get $1
  i32.const 4
  i32.add
  local.get $1
  i32.load
  i32.const -4
  i32.and
  call $~lib/memory/memory.copy
  local.get $1
  i32.const 1988
  i32.ge_u
  if
   local.get $1
   local.get $1
   i32.load
   i32.const 1
   i32.or
   i32.store
   local.get $0
   local.get $1
   call $~lib/rt/tlsf/insertBlock
  end
  local.get $2
 )
 (func $~lib/memory/memory.fill (param $0 i32) (param $1 i32)
  (local $2 i32)
  block $~lib/util/memory/memset|inlined.0
   local.get $1
   i32.eqz
   br_if $~lib/util/memory/memset|inlined.0
   local.get $0
   i32.const 0
   i32.store8
   local.get $0
   local.get $1
   i32.add
   local.tee $2
   i32.const 1
   i32.sub
   i32.const 0
   i32.store8
   local.get $1
   i32.const 2
   i32.le_u
   br_if $~lib/util/memory/memset|inlined.0
   local.get $0
   i32.const 0
   i32.store8 offset=1
   local.get $0
   i32.const 0
   i32.store8 offset=2
   local.get $2
   i32.const 2
   i32.sub
   i32.const 0
   i32.store8
   local.get $2
   i32.const 3
   i32.sub
   i32.const 0
   i32.store8
   local.get $1
   i32.const 6
   i32.le_u
   br_if $~lib/util/memory/memset|inlined.0
   local.get $0
   i32.const 0
   i32.store8 offset=3
   local.get $2
   i32.const 4
   i32.sub
   i32.const 0
   i32.store8
   local.get $1
   i32.const 8
   i32.le_u
   br_if $~lib/util/memory/memset|inlined.0
   local.get $0
   i32.const 0
   local.get $0
   i32.sub
   i32.const 3
   i32.and
   local.tee $2
   i32.add
   local.tee $0
   i32.const 0
   i32.store
   local.get $0
   local.get $1
   local.get $2
   i32.sub
   i32.const -4
   i32.and
   local.tee $2
   i32.add
   local.tee $1
   i32.const 4
   i32.sub
   i32.const 0
   i32.store
   local.get $2
   i32.const 8
   i32.le_u
   br_if $~lib/util/memory/memset|inlined.0
   local.get $0
   i32.const 0
   i32.store offset=4
   local.get $0
   i32.const 0
   i32.store offset=8
   local.get $1
   i32.const 12
   i32.sub
   i32.const 0
   i32.store
   local.get $1
   i32.const 8
   i32.sub
   i32.const 0
   i32.store
   local.get $2
   i32.const 24
   i32.le_u
   br_if $~lib/util/memory/memset|inlined.0
   local.get $0
   i32.const 0
   i32.store offset=12
   local.get $0
   i32.const 0
   i32.store offset=16
   local.get $0
   i32.const 0
   i32.store offset=20
   local.get $0
   i32.const 0
   i32.store offset=24
   local.get $1
   i32.const 28
   i32.sub
   i32.const 0
   i32.store
   local.get $1
   i32.const 24
   i32.sub
   i32.const 0
   i32.store
   local.get $1
   i32.const 20
   i32.sub
   i32.const 0
   i32.store
   local.get $1
   i32.const 16
   i32.sub
   i32.const 0
   i32.store
   local.get $0
   local.get $0
   i32.const 4
   i32.and
   i32.const 24
   i32.add
   local.tee $1
   i32.add
   local.set $0
   local.get $2
   local.get $1
   i32.sub
   local.set $1
   loop $while-continue|0
    local.get $1
    i32.const 32
    i32.ge_u
    if
     local.get $0
     i64.const 0
     i64.store
     local.get $0
     i64.const 0
     i64.store offset=8
     local.get $0
     i64.const 0
     i64.store offset=16
     local.get $0
     i64.const 0
     i64.store offset=24
     local.get $1
     i32.const 32
     i32.sub
     local.set $1
     local.get $0
     i32.const 32
     i32.add
     local.set $0
     br $while-continue|0
    end
   end
  end
 )
 (func $~lib/array/Array<components/Entity/Entity>#push (param $0 i32) (param $1 i32)
  (local $2 i32)
  (local $3 i32)
  (local $4 i32)
  (local $5 i32)
  (local $6 i32)
  (local $7 i32)
  (local $8 i32)
  (local $9 i32)
  (local $10 i32)
  (local $11 i32)
  (local $12 i32)
  (local $13 i32)
  (local $14 i32)
  (local $15 i32)
  local.get $0
  i32.load offset=12
  local.tee $12
  i32.const 1
  i32.add
  local.tee $8
  local.set $2
  local.get $8
  local.get $0
  i32.load offset=8
  local.tee $9
  i32.const 2
  i32.shr_u
  i32.gt_u
  if
   local.get $2
   i32.const 268435455
   i32.gt_u
   if
    i32.const 1248
    i32.const 1296
    i32.const 14
    i32.const 48
    call $~lib/builtins/abort
    unreachable
   end
   local.get $0
   i32.load
   local.tee $6
   local.set $4
   local.get $2
   i32.const 2
   i32.shl
   local.tee $10
   local.set $5
   local.get $6
   i32.const 20
   i32.sub
   local.set $3
   block $__inlined_func$~lib/rt/tcms/__renew
    local.get $6
    i32.const 1988
    i32.lt_u
    if
     local.get $5
     local.get $3
     i32.load offset=12
     call $~lib/rt/tcms/__new
     local.tee $2
     local.get $4
     local.get $5
     local.get $3
     i32.load offset=16
     local.tee $3
     local.get $3
     local.get $5
     i32.gt_u
     select
     call $~lib/memory/memory.copy
     br $__inlined_func$~lib/rt/tcms/__renew
    end
    local.get $5
    i32.const 1073741804
    i32.ge_u
    if
     i32.const 1344
     i32.const 1408
     i32.const 143
     i32.const 31
     call $~lib/builtins/abort
     unreachable
    end
    global.get $~lib/rt/tcms/total
    local.get $3
    i32.load
    i32.const -4
    i32.and
    i32.const 4
    i32.add
    i32.sub
    global.set $~lib/rt/tcms/total
    local.get $4
    i32.const 16
    i32.sub
    local.set $2
    local.get $5
    i32.const 16
    i32.add
    local.set $3
    global.get $~lib/rt/tlsf/ROOT
    i32.eqz
    if
     call $~lib/rt/tlsf/initialize
    end
    local.get $2
    i32.const 1988
    i32.lt_u
    if
     global.get $~lib/rt/tlsf/ROOT
     local.get $2
     call $~lib/rt/tlsf/checkUsedBlock
     local.get $3
     call $~lib/rt/tlsf/moveBlock
     local.set $2
    else
     block $__inlined_func$~lib/rt/tlsf/reallocateBlock
      global.get $~lib/rt/tlsf/ROOT
      local.set $4
      local.get $2
      call $~lib/rt/tlsf/checkUsedBlock
      local.set $2
      block $folding-inner0
       local.get $3
       call $~lib/rt/tlsf/prepareSize
       local.tee $11
       local.get $2
       i32.load
       local.tee $13
       i32.const -4
       i32.and
       local.tee $7
       i32.le_u
       br_if $folding-inner0
       local.get $2
       i32.const 4
       i32.add
       local.get $2
       i32.load
       i32.const -4
       i32.and
       i32.add
       local.tee $14
       i32.load
       local.tee $15
       i32.const 1
       i32.and
       if
        local.get $11
        local.get $7
        i32.const 4
        i32.add
        local.get $15
        i32.const -4
        i32.and
        i32.add
        local.tee $7
        i32.le_u
        if
         local.get $4
         local.get $14
         call $~lib/rt/tlsf/removeBlock
         local.get $2
         local.get $7
         local.get $13
         i32.const 3
         i32.and
         i32.or
         i32.store
         br $folding-inner0
        end
       end
       local.get $4
       local.get $2
       local.get $3
       call $~lib/rt/tlsf/moveBlock
       local.set $2
       br $__inlined_func$~lib/rt/tlsf/reallocateBlock
      end
      local.get $4
      local.get $2
      local.get $11
      call $~lib/rt/tlsf/prepareBlock
     end
    end
    local.get $2
    i32.const 20
    i32.add
    local.tee $2
    i32.const 20
    i32.sub
    local.tee $3
    local.get $5
    i32.store offset=16
    local.get $3
    i32.load offset=4
    i32.const -4
    i32.and
    local.get $3
    i32.store offset=8
    local.get $3
    local.get $3
    i32.load offset=8
    local.tee $5
    i32.load offset=4
    i32.const 3
    i32.and
    i32.or
    local.set $4
    local.get $5
    local.get $4
    i32.store offset=4
    global.get $~lib/rt/tcms/total
    local.get $3
    i32.load
    i32.const -4
    i32.and
    i32.const 4
    i32.add
    i32.add
    global.set $~lib/rt/tcms/total
   end
   local.get $2
   local.get $9
   i32.add
   local.get $10
   local.get $9
   i32.sub
   call $~lib/memory/memory.fill
   local.get $2
   local.get $6
   i32.ne
   if
    local.get $0
    local.get $2
    i32.store
    local.get $0
    local.get $2
    i32.store offset=4
   end
   local.get $0
   local.get $10
   i32.store offset=8
  end
  local.get $0
  i32.load offset=4
  local.get $12
  i32.const 2
  i32.shl
  i32.add
  local.get $1
  i32.store
  local.get $0
  local.get $8
  i32.store offset=12
 )
 (func $~lib/array/Array<components/Entity/Entity>#set:buffer (param $0 i32) (param $1 i32)
  local.get $0
  local.get $1
  i32.store
 )
 (func $RingLight/RingLight#set:hue_offset (param $0 i32) (param $1 f64)
  local.get $0
  local.get $1
  f64.store offset=8
 )
 (func $RingLight/RingLight#set:radius (param $0 i32) (param $1 f64)
  local.get $0
  local.get $1
  f64.store offset=16
 )
 (func $RingLight/RingLight#set:delta (param $0 i32) (param $1 f64)
  local.get $0
  local.get $1
  f64.store offset=24
 )
 (func $RingLight/RingLight#constructor (param $0 i32) (param $1 f64) (param $2 f64) (result i32)
  (local $3 i32)
  (local $4 f64)
  (local $5 i32)
  local.get $0
  i32.eqz
  if
   i32.const 32
   i32.const 3
   call $~lib/rt/tcms/__new
   local.set $0
  end
  i32.const 16
  i32.const 4
  call $~lib/rt/tcms/__new
  local.tee $3
  i32.const 0
  i32.store
  local.get $3
  i32.const 0
  i32.store offset=4
  local.get $3
  i32.const 0
  i32.store offset=8
  local.get $3
  i32.const 0
  i32.store offset=12
  i32.const 0
  i32.const 0
  call $~lib/rt/tcms/__new
  local.tee $5
  i32.const 0
  call $~lib/memory/memory.fill
  local.get $3
  local.get $5
  i32.store
  local.get $3
  local.get $5
  i32.store offset=4
  local.get $3
  i32.const 0
  i32.store offset=8
  local.get $3
  i32.const 0
  i32.store offset=12
  local.get $0
  local.get $3
  i32.store
  local.get $0
  f64.const 0
  f64.store offset=8
  local.get $0
  local.get $1
  f64.store offset=16
  local.get $0
  local.get $2
  f64.store offset=24
  loop $while-continue|0
   local.get $4
   f64.const 6.283185307179586
   f64.lt
   if
    local.get $4
    call $~lib/math/NativeMath.sin
    local.get $1
    f64.mul
    f64.const 0.05
    local.get $4
    call $~lib/math/NativeMath.cos
    local.get $1
    f64.mul
    call $components/World/World.spawnEntityAt
    local.tee $3
    call $components/Entity/Entity#add<components/PointLightComponent/PointLightComponent>
    f64.const 1
    f64.const 0
    f64.const 1
    call $components/PointLightComponent/PointLightComponent#setIntensity
    local.get $0
    i32.load
    local.get $3
    call $~lib/array/Array<components/Entity/Entity>#push
    local.get $4
    local.get $2
    f64.add
    local.set $4
    br $while-continue|0
   end
  end
  local.get $0
 )
 (func $RingLight/RingLight#get:lights (param $0 i32) (result i32)
  local.get $0
  i32.load
 )
 (func $RingLight/RingLight#get:hue_offset (param $0 i32) (result f64)
  local.get $0
  f64.load offset=8
 )
 (func $RingLight/RingLight#get:radius (param $0 i32) (result f64)
  local.get $0
  f64.load offset=16
 )
 (func $RingLight/RingLight#get:delta (param $0 i32) (result f64)
  local.get $0
  f64.load offset=24
 )
 (func $RingLight/Color#constructor (param $0 f64) (param $1 f64) (param $2 f64) (result i32)
  (local $3 i32)
  i32.const 24
  i32.const 5
  call $~lib/rt/tcms/__new
  local.tee $3
  local.get $0
  f64.store
  local.get $3
  local.get $1
  f64.store offset=8
  local.get $3
  local.get $2
  f64.store offset=16
  local.get $3
 )
 (func $RingLight/HSVtoRGB (param $0 f64) (result i32)
  (local $1 i64)
  (local $2 i64)
  (local $3 i64)
  (local $4 f64)
  (local $5 f64)
  (local $6 i64)
  local.get $0
  f64.const 6
  f64.mul
  local.tee $4
  f64.floor
  local.set $0
  f64.const 1
  f64.const 1
  local.get $4
  local.get $0
  f64.sub
  f64.sub
  local.tee $4
  f64.sub
  local.set $5
  block $__inlined_func$~lib/math/NativeMath.mod (result f64)
   local.get $0
   i64.reinterpret_f64
   local.tee $2
   i64.const 63
   i64.shr_u
   local.set $6
   local.get $2
   i64.const 52
   i64.shr_u
   i64.const 2047
   i64.and
   local.tee $3
   i64.const 2047
   i64.eq
   if
    local.get $0
    f64.const 6
    f64.mul
    local.tee $0
    local.get $0
    f64.div
    br $__inlined_func$~lib/math/NativeMath.mod
   end
   local.get $2
   i64.const 1
   i64.shl
   local.tee $1
   i64.const -9209861237972664320
   i64.le_u
   if
    local.get $0
    local.get $1
    i64.const -9209861237972664320
    i64.ne
    f64.convert_i32_u
    f64.mul
    br $__inlined_func$~lib/math/NativeMath.mod
   end
   local.get $3
   i64.eqz
   if (result i64)
    local.get $2
    i64.const 1
    local.get $3
    local.get $2
    i64.const 12
    i64.shl
    i64.clz
    i64.sub
    local.tee $3
    i64.sub
    i64.shl
   else
    local.get $2
    i64.const 4503599627370495
    i64.and
    i64.const 4503599627370496
    i64.or
   end
   local.set $1
   block $folding-inner0
    loop $while-continue|0
     local.get $3
     i64.const 1025
     i64.gt_s
     if
      local.get $1
      i64.const 6755399441055744
      i64.ge_u
      if (result i64)
       local.get $1
       i64.const 6755399441055744
       i64.eq
       br_if $folding-inner0
       local.get $1
       i64.const 6755399441055744
       i64.sub
      else
       local.get $1
      end
      i64.const 1
      i64.shl
      local.set $1
      local.get $3
      i64.const 1
      i64.sub
      local.set $3
      br $while-continue|0
     end
    end
    local.get $1
    i64.const 6755399441055744
    i64.ge_u
    if
     local.get $1
     i64.const 6755399441055744
     i64.eq
     br_if $folding-inner0
     local.get $1
     i64.const 6755399441055744
     i64.sub
     local.set $1
    end
    local.get $1
    local.get $1
    i64.const 11
    i64.shl
    i64.clz
    local.tee $1
    i64.shl
    local.set $2
    local.get $3
    local.get $1
    i64.sub
    local.tee $1
    i64.const 0
    i64.gt_s
    if (result i64)
     local.get $2
     i64.const 4503599627370496
     i64.sub
     local.get $1
     i64.const 52
     i64.shl
     i64.or
    else
     local.get $2
     i64.const 1
     local.get $1
     i64.sub
     i64.shr_u
    end
    local.get $6
    i64.const 63
    i64.shl
    i64.or
    f64.reinterpret_i64
    br $__inlined_func$~lib/math/NativeMath.mod
   end
   local.get $0
   f64.const 0
   f64.mul
  end
  local.tee $0
  f64.const 0
  f64.eq
  if
   f64.const 1
   local.get $5
   f64.const 0
   call $RingLight/Color#constructor
   return
  end
  local.get $0
  f64.const 1
  f64.eq
  if
   local.get $4
   f64.const 1
   f64.const 0
   call $RingLight/Color#constructor
   return
  end
  local.get $0
  f64.const 2
  f64.eq
  if
   f64.const 0
   f64.const 1
   local.get $5
   call $RingLight/Color#constructor
   return
  end
  local.get $0
  f64.const 3
  f64.eq
  if
   f64.const 0
   local.get $4
   f64.const 1
   call $RingLight/Color#constructor
   return
  end
  local.get $0
  f64.const 4
  f64.eq
  if
   local.get $5
   f64.const 0
   f64.const 1
   call $RingLight/Color#constructor
   return
  end
  local.get $0
  f64.const 5
  f64.eq
  if
   f64.const 1
   f64.const 0
   f64.const 0
   call $RingLight/Color#constructor
   return
  end
  f64.const 1
  f64.const 0
  f64.const 1
  call $RingLight/Color#constructor
 )
 (func $RingLight/RingLight#update (param $0 i32) (param $1 f64)
  (local $2 i32)
  (local $3 i32)
  (local $4 f64)
  (local $5 i32)
  local.get $0
  local.get $0
  f64.load offset=8
  local.get $1
  f64.const 0.1
  f64.mul
  f64.add
  f64.store offset=8
  loop $for-loop|0
   local.get $3
   local.get $0
   i32.load
   i32.load offset=12
   i32.lt_s
   if
    local.get $3
    local.tee $2
    local.get $0
    i32.load
    local.tee $5
    i32.load offset=12
    i32.ge_u
    if
     i32.const 1568
     i32.const 1296
     i32.const 92
     i32.const 42
     call $~lib/builtins/abort
     unreachable
    end
    local.get $5
    i32.load offset=4
    local.get $2
    i32.const 2
    i32.shl
    i32.add
    i32.load
    local.tee $2
    i32.eqz
    if
     i32.const 1632
     i32.const 1296
     i32.const 96
     i32.const 40
     call $~lib/builtins/abort
     unreachable
    end
    local.get $2
    call $components/Entity/Entity#get<components/PointLightComponent/PointLightComponent>
    local.get $4
    f64.const 6.283185307179586
    f64.div
    local.get $0
    f64.load offset=8
    f64.add
    call $RingLight/HSVtoRGB
    local.tee $2
    f64.load
    local.get $2
    f64.load offset=8
    local.get $2
    f64.load offset=16
    call $components/PointLightComponent/PointLightComponent#setIntensity
    local.get $4
    local.get $0
    f64.load offset=24
    f64.add
    local.set $4
    local.get $3
    i32.const 1
    i32.add
    local.set $3
    br $for-loop|0
   end
  end
 )
 (func $RingLight/update (param $0 i32) (param $1 f64)
  global.get $RingLight/ring_light
  local.get $1
  call $RingLight/RingLight#update
 )
 (func $~lib/rt/tcms/Object#unlink (param $0 i32)
  (local $1 i32)
  local.get $0
  i32.load offset=4
  i32.const -4
  i32.and
  local.tee $1
  i32.eqz
  if
   i32.const 0
   local.get $0
   i32.const 1988
   i32.lt_u
   local.get $0
   i32.load offset=8
   select
   i32.eqz
   if
    i32.const 0
    i32.const 1408
    i32.const 101
    i32.const 18
    call $~lib/builtins/abort
    unreachable
   end
   return
  end
  local.get $0
  i32.load offset=8
  local.tee $0
  i32.eqz
  if
   i32.const 0
   i32.const 1408
   i32.const 105
   i32.const 16
   call $~lib/builtins/abort
   unreachable
  end
  local.get $1
  local.get $0
  i32.store offset=8
  local.get $0
  local.get $1
  local.get $0
  i32.load offset=4
  i32.const 3
  i32.and
  i32.or
  i32.store offset=4
 )
 (func $~lib/rt/tcms/__pin (param $0 i32) (result i32)
  (local $1 i32)
  (local $2 i32)
  (local $3 i32)
  local.get $0
  if
   local.get $0
   i32.const 20
   i32.sub
   local.tee $1
   i32.load offset=4
   i32.const 3
   i32.and
   i32.const 3
   i32.eq
   if
    i32.const 1760
    i32.const 1408
    i32.const 181
    i32.const 7
    call $~lib/builtins/abort
    unreachable
   end
   local.get $1
   call $~lib/rt/tcms/Object#unlink
   global.get $~lib/rt/tcms/pinSpace
   local.tee $3
   i32.load offset=8
   local.set $2
   local.get $1
   local.get $3
   i32.const 3
   i32.or
   i32.store offset=4
   local.get $1
   local.get $2
   i32.store offset=8
   local.get $2
   local.get $1
   local.get $2
   i32.load offset=4
   i32.const 3
   i32.and
   i32.or
   i32.store offset=4
   local.get $3
   local.get $1
   i32.store offset=8
  end
  local.get $0
 )
 (func $~lib/rt/tcms/__unpin (param $0 i32)
  (local $1 i32)
  (local $2 i32)
  local.get $0
  i32.eqz
  if
   return
  end
  local.get $0
  i32.const 20
  i32.sub
  local.tee $0
  i32.load offset=4
  i32.const 3
  i32.and
  i32.const 3
  i32.ne
  if
   i32.const 1856
   i32.const 1408
   i32.const 195
   i32.const 5
   call $~lib/builtins/abort
   unreachable
  end
  local.get $0
  call $~lib/rt/tcms/Object#unlink
  global.get $~lib/rt/tcms/fromSpace
  local.tee $2
  i32.load offset=8
  local.set $1
  local.get $0
  local.get $2
  global.get $~lib/rt/tcms/white
  i32.or
  i32.store offset=4
  local.get $0
  local.get $1
  i32.store offset=8
  local.get $1
  local.get $0
  local.get $1
  i32.load offset=4
  i32.const 3
  i32.and
  i32.or
  i32.store offset=4
  local.get $2
  local.get $0
  i32.store offset=8
 )
 (func $~lib/rt/tcms/__collect
  (local $0 i32)
  (local $1 i32)
  (local $2 i32)
  (local $3 i32)
  (local $4 i32)
  (local $5 i32)
  global.get $RingLight/ring_light
  local.tee $1
  if
   local.get $1
   call $~lib/rt/tcms/__visit
  end
  i32.const 1568
  call $~lib/rt/tcms/__visit
  i32.const 1248
  call $~lib/rt/tcms/__visit
  i32.const 1632
  call $~lib/rt/tcms/__visit
  i32.const 1344
  call $~lib/rt/tcms/__visit
  i32.const 1760
  call $~lib/rt/tcms/__visit
  i32.const 1856
  call $~lib/rt/tcms/__visit
  global.get $~lib/rt/tcms/pinSpace
  local.tee $1
  i32.load offset=4
  i32.const -4
  i32.and
  local.set $0
  loop $while-continue|0
   local.get $0
   local.get $1
   i32.ne
   if
    local.get $0
    i32.load offset=4
    i32.const 3
    i32.and
    i32.const 3
    i32.ne
    if
     i32.const 0
     i32.const 1408
     i32.const 213
     i32.const 16
     call $~lib/builtins/abort
     unreachable
    end
    local.get $0
    i32.const 20
    i32.add
    call $~lib/rt/__visit_members
    local.get $0
    i32.load offset=4
    i32.const -4
    i32.and
    local.set $0
    br $while-continue|0
   end
  end
  global.get $~lib/rt/tcms/white
  i32.eqz
  local.set $4
  global.get $~lib/rt/tcms/toSpace
  local.tee $5
  i32.load offset=4
  i32.const -4
  i32.and
  local.set $0
  loop $while-continue|1
   local.get $0
   local.get $5
   i32.ne
   if
    local.get $4
    local.get $0
    i32.load offset=4
    i32.const 3
    i32.and
    i32.ne
    if
     i32.const 0
     i32.const 1408
     i32.const 223
     i32.const 16
     call $~lib/builtins/abort
     unreachable
    end
    local.get $0
    i32.const 20
    i32.add
    call $~lib/rt/__visit_members
    local.get $0
    i32.load offset=4
    i32.const -4
    i32.and
    local.set $0
    br $while-continue|1
   end
  end
  global.get $~lib/rt/tcms/fromSpace
  local.tee $2
  i32.load offset=4
  i32.const -4
  i32.and
  local.set $0
  loop $while-continue|2
   local.get $0
   local.get $2
   i32.ne
   if
    global.get $~lib/rt/tcms/white
    local.get $0
    i32.load offset=4
    i32.const 3
    i32.and
    i32.ne
    if
     i32.const 0
     i32.const 1408
     i32.const 232
     i32.const 16
     call $~lib/builtins/abort
     unreachable
    end
    local.get $0
    i32.load offset=4
    i32.const -4
    i32.and
    local.get $0
    i32.const 1988
    i32.lt_u
    if
     local.get $0
     i32.const 0
     i32.store offset=4
     local.get $0
     i32.const 0
     i32.store offset=8
    else
     global.get $~lib/rt/tcms/total
     local.get $0
     i32.load
     i32.const -4
     i32.and
     i32.const 4
     i32.add
     i32.sub
     global.set $~lib/rt/tcms/total
     local.get $0
     i32.const 4
     i32.add
     local.tee $3
     i32.const 1988
     i32.ge_u
     if
      global.get $~lib/rt/tlsf/ROOT
      i32.eqz
      if
       call $~lib/rt/tlsf/initialize
      end
      global.get $~lib/rt/tlsf/ROOT
      local.get $3
      call $~lib/rt/tlsf/checkUsedBlock
      local.tee $3
      local.get $3
      i32.load
      i32.const 1
      i32.or
      i32.store
      local.get $3
      call $~lib/rt/tlsf/insertBlock
     end
    end
    local.set $0
    br $while-continue|2
   end
  end
  local.get $2
  local.get $2
  i32.store offset=4
  local.get $2
  local.get $2
  i32.store offset=8
  local.get $5
  global.set $~lib/rt/tcms/fromSpace
  local.get $2
  global.set $~lib/rt/tcms/toSpace
  local.get $4
  global.set $~lib/rt/tcms/white
 )
 (func $~lib/rt/tcms/__visit (param $0 i32)
  (local $1 i32)
  (local $2 i32)
  local.get $0
  i32.eqz
  if
   return
  end
  global.get $~lib/rt/tcms/white
  local.get $0
  i32.const 20
  i32.sub
  local.tee $0
  i32.load offset=4
  i32.const 3
  i32.and
  i32.eq
  if
   local.get $0
   call $~lib/rt/tcms/Object#unlink
   global.get $~lib/rt/tcms/toSpace
   local.tee $2
   i32.load offset=8
   local.set $1
   local.get $0
   local.get $2
   global.get $~lib/rt/tcms/white
   i32.eqz
   i32.or
   i32.store offset=4
   local.get $0
   local.get $1
   i32.store offset=8
   local.get $1
   local.get $0
   local.get $1
   i32.load offset=4
   i32.const 3
   i32.and
   i32.or
   i32.store offset=4
   local.get $2
   local.get $0
   i32.store offset=8
  end
 )
 (func $~lib/rt/__visit_members (param $0 i32)
  block $folding-inner0
   block $invalid
    block $RingLight/Color
     block $~lib/array/Array<components/Entity/Entity>
      block $~lib/string/String
       block $~lib/arraybuffer/ArrayBuffer
        local.get $0
        i32.const 8
        i32.sub
        i32.load
        br_table $~lib/arraybuffer/ArrayBuffer $~lib/string/String $folding-inner0 $folding-inner0 $~lib/array/Array<components/Entity/Entity> $RingLight/Color $invalid
       end
       return
      end
      return
     end
     local.get $0
     i32.load
     call $~lib/rt/tcms/__visit
     return
    end
    return
   end
   unreachable
  end
  local.get $0
  i32.load
  local.tee $0
  if
   local.get $0
   call $~lib/rt/tcms/__visit
  end
 )
 (func $~start
  i32.const 1524
  i32.const 1520
  i32.store
  i32.const 1528
  i32.const 1520
  i32.store
  i32.const 1520
  global.set $~lib/rt/tcms/fromSpace
  i32.const 0
  f64.const 1
  f64.const 0.3
  call $RingLight/RingLight#constructor
  global.set $RingLight/ring_light
  i32.const 1812
  i32.const 1808
  i32.store
  i32.const 1816
  i32.const 1808
  i32.store
  i32.const 1808
  global.set $~lib/rt/tcms/pinSpace
  i32.const 1908
  i32.const 1904
  i32.store
  i32.const 1912
  i32.const 1904
  i32.store
  i32.const 1904
  global.set $~lib/rt/tcms/toSpace
 )
)

; ModuleID = 'varuna_tmp_input_noopt-2b2855a9-ddef-498c-b65b-537ed0cf84dd.ll'
source_filename = "Varuna"

declare i32 @_Z11declarationi(i32)

define internal i32 @_Z10definitioni(i32 %arg) {
entry:
  %arg1 = alloca i32
  store i32 %arg, i32* %arg1
  %arg2 = load i32, i32* %arg1
  %calltmp = call i32 @_Z11declarationi(i32 %arg2)
  ret i32 %calltmp
}

define i32 @_Z4mainv() {
entry:
  %calltmp = call i32 @_Z10definitioni(i32 0)
  ret i32 %calltmp
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

; ModuleID = 'varuna_tmp_input_noopt-b0ce4808-b3e2-4bbd-8885-7b2747fab547.ll'
source_filename = "Varuna"

declare i32 @_Z4funci(i32)

define i32 @_Z4mainv() {
entry:
  %n = alloca i32
  store i32 10, i32* %n
  %n1 = load i32, i32* %n
  %calltmp = call i32 @_Z4funci(i32 %n1)
  ret i32 %calltmp
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

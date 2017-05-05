; ModuleID = 'varuna_tmp_input_noopt-1cf595f0-1a07-49a0-9a22-d3323e7b10b5.ll'
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

; ModuleID = 'varuna_tmp_input_noopt-f53b928a-58f8-4543-91e8-60dfea2e3e4a.ll'
source_filename = "Varuna"

declare i32 @_Z4funci(i32) local_unnamed_addr

define i32 @_Z4mainv() local_unnamed_addr {
entry:
  %calltmp = tail call i32 @_Z4funci(i32 10)
  ret i32 %calltmp
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

; ModuleID = 'varuna_tmp_input_noopt-5f721e82-00b1-453e-8a0c-abb13fabdd9f.ll'
source_filename = "Varuna"

declare i32 @_Z4funci(i32) local_unnamed_addr

define i32 @_Z4mainv() local_unnamed_addr {
entry:
  %calltmp = tail call i32 @_Z4funci(i32 10)
  ret i32 %calltmp
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

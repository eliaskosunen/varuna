; ModuleID = 'varuna_tmp_input_noopt-bdc41ad5-b835-4573-bf79-88128656df48.ll'
source_filename = "Varuna"

declare i32 @_Z11declarationi(i32) local_unnamed_addr

define i32 @_Z4mainv() local_unnamed_addr {
entry:
  %calltmp.i = tail call i32 @_Z11declarationi(i32 0)
  ret i32 %calltmp.i
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

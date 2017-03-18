; ModuleID = 'varuna_tmp_input_noopt-ed96e3fc-7b93-473d-9971-b34c725a1cb8.ll'
source_filename = "Varuna"

declare i32 @_Z11declarationi(i32) local_unnamed_addr

define i32 @_Z4mainv() local_unnamed_addr {
entry:
  %calltmp.i = tail call i32 @_Z11declarationi(i32 0)
  ret i32 %calltmp.i
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

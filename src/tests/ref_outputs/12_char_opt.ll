; ModuleID = 'varuna_tmp_input_noopt-a1544c37-6de6-47dc-8700-8658c0974e07.ll'
source_filename = "Varuna"

declare i32 @_Z3ascDi(i32) local_unnamed_addr

define i32 @_Z4mainv() local_unnamed_addr {
entry:
  %calltmp = tail call i32 @_Z3ascDi(i32 128512)
  %calltmp4 = tail call i32 @_Z3ascDi(i32 228)
  %divtmp = sdiv i32 %calltmp, %calltmp4
  ret i32 %divtmp
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

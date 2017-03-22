; ModuleID = 'varuna_tmp_input_noopt-cad6093b-c7ef-4321-a205-5a7f01f8973d.ll'
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

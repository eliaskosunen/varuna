; ModuleID = 'varuna_tmp_input_noopt-bd8d01ea-8c79-4914-8f0b-c92335b07d2b.ll'
source_filename = "Varuna"

declare i32 @_Z17external_functionv() local_unnamed_addr

declare i32 @_Z16another_functionv() local_unnamed_addr

define i32 @_Z4mainv() local_unnamed_addr {
entry:
  %calltmp = tail call i32 @_Z17external_functionv()
  %divtmp = sdiv i32 %calltmp, 2
  %calltmp2 = tail call i32 @_Z16another_functionv()
  %multmp = shl i32 %calltmp2, 1
  %addtmp = add nsw i32 %multmp, %divtmp
  ret i32 %addtmp
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

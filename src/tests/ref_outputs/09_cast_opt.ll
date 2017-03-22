; ModuleID = 'varuna_tmp_input_noopt-0bf18495-f098-4a19-9412-d640e6164a74.ll'
source_filename = "Varuna"

declare i32 @_Z3numv() local_unnamed_addr

define i32 @_Z4mainv() local_unnamed_addr {
entry:
  %calltmp = tail call i32 @_Z3numv()
  %sext = shl i32 %calltmp, 16
  %casttmp4 = ashr exact i32 %sext, 16
  ret i32 %casttmp4
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

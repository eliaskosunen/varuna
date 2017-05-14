; ModuleID = 'varuna_tmp_input_noopt-0159e0e1-3196-4c70-878a-6001c5e41e1a.ll'
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

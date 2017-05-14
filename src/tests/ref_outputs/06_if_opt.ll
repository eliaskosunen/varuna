; ModuleID = 'varuna_tmp_input_noopt-f5192acb-81b4-4efd-9007-4a29d3d2a3bb.ll'
source_filename = "Varuna"

declare i1 @_Z4condv() local_unnamed_addr

define i32 @_Z4mainv() local_unnamed_addr {
entry:
  %calltmp = tail call i1 @_Z4condv()
  %calltmp1 = tail call i1 @_Z4condv()
  %not.calltmp1 = xor i1 %calltmp1, true
  %. = zext i1 %not.calltmp1 to i32
  %.1 = select i1 %calltmp1, i32 2, i32 3
  %merge = select i1 %calltmp, i32 %., i32 %.1
  ret i32 %merge
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

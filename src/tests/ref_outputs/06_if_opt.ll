; ModuleID = 'test_06_if'
source_filename = "Varuna"

declare i1 @cond() local_unnamed_addr

define i32 @main() local_unnamed_addr {
entry:
  %calltmp = tail call i1 @cond()
  %calltmp1 = tail call i1 @cond()
  %0 = zext i1 %calltmp1 to i32
  %. = xor i32 %0, 1
  %.9 = select i1 %calltmp1, i32 2, i32 3
  %merge = select i1 %calltmp, i32 %., i32 %.9
  ret i32 %merge
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

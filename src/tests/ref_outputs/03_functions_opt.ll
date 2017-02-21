; ModuleID = 'test_03_functions'
source_filename = "Varuna"

declare i32 @declaration(i32) local_unnamed_addr

define i32 @main() local_unnamed_addr {
entry:
  %calltmp.i = tail call i32 @declaration(i32 0)
  ret i32 %calltmp.i
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

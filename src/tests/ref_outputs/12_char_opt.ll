; ModuleID = 'test_12_char'
source_filename = "Varuna"

declare i32 @asc(i32) local_unnamed_addr

define i32 @main() local_unnamed_addr {
entry:
  %calltmp = tail call i32 @asc(i32 128512)
  %calltmp4 = tail call i32 @asc(i32 228)
  %divtmp = sdiv i32 %calltmp, %calltmp4
  ret i32 %divtmp
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

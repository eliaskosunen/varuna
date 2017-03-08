; ModuleID = 'test_09_cast'
source_filename = "Varuna"

declare i32 @__va_num() local_unnamed_addr

define i32 @__va_main() local_unnamed_addr {
entry:
  %calltmp = tail call i32 @__va_num()
  %sext = shl i32 %calltmp, 16
  %casttmp4 = ashr exact i32 %sext, 16
  ret i32 %casttmp4
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

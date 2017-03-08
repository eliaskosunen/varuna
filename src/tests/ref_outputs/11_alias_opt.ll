; ModuleID = 'test_11_alias'
source_filename = "Varuna"

declare i32 @__va_func(i32) local_unnamed_addr

define i32 @__va_main() local_unnamed_addr {
entry:
  %calltmp = tail call i32 @__va_func(i32 10)
  ret i32 %calltmp
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

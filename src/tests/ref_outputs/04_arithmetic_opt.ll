; ModuleID = 'test_04_arithmetic'
source_filename = "Varuna"

declare i32 @__va_num(i32) local_unnamed_addr

define i32 @__va_main() local_unnamed_addr {
entry:
  %calltmp = tail call i32 @__va_num(i32 1)
  %calltmp1 = tail call i32 @__va_num(i32 0)
  %calltmp2 = tail call i32 @__va_num(i32 -5)
  %multmp = mul nsw i32 %calltmp2, %calltmp1
  %addtmp = add nsw i32 %multmp, %calltmp
  %calltmp3 = tail call i32 @__va_num(i32 42)
  %divtmp = sdiv i32 10, %calltmp3
  %calltmp4 = tail call i32 @__va_num(i32 2)
  %remtmp = srem i32 %divtmp, %calltmp4
  %subtmp = sub i32 %addtmp, %remtmp
  ret i32 %subtmp
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

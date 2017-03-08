; ModuleID = 'test_04_arithmetic'
source_filename = "Varuna"

declare i32 @__va_num(i32)

define i32 @__va_main() {
entry:
  %calltmp = call i32 @__va_num(i32 1)
  %calltmp1 = call i32 @__va_num(i32 0)
  %calltmp2 = call i32 @__va_num(i32 -5)
  %multmp = mul nsw i32 %calltmp1, %calltmp2
  %addtmp = add nsw i32 %calltmp, %multmp
  %calltmp3 = call i32 @__va_num(i32 42)
  %divtmp = sdiv i32 10, %calltmp3
  %calltmp4 = call i32 @__va_num(i32 2)
  %remtmp = srem i32 %divtmp, %calltmp4
  %subtmp = sub nsw i32 %addtmp, %remtmp
  ret i32 %subtmp
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

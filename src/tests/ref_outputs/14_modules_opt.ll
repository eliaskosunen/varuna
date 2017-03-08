; ModuleID = 'test_14_modules'
source_filename = "Varuna"

declare i32 @__va_external_function() local_unnamed_addr

declare i32 @__va_another_function() local_unnamed_addr

define i32 @__va_main() local_unnamed_addr {
entry:
  %calltmp = tail call i32 @__va_external_function()
  %divtmp = sdiv i32 %calltmp, 2
  %calltmp2 = tail call i32 @__va_another_function()
  %multmp = shl i32 %calltmp2, 1
  %addtmp = add nsw i32 %multmp, %divtmp
  ret i32 %addtmp
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

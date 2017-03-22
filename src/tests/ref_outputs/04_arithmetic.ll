; ModuleID = 'varuna_tmp_input_noopt-276eb58e-84a2-4d43-a821-649e0c96999d.ll'
source_filename = "Varuna"

declare i32 @_Z3numi(i32)

define i32 @_Z4mainv() {
entry:
  %calltmp = call i32 @_Z3numi(i32 1)
  %calltmp1 = call i32 @_Z3numi(i32 0)
  %calltmp2 = call i32 @_Z3numi(i32 -5)
  %multmp = mul nsw i32 %calltmp1, %calltmp2
  %addtmp = add nsw i32 %calltmp, %multmp
  %calltmp3 = call i32 @_Z3numi(i32 42)
  %divtmp = sdiv i32 10, %calltmp3
  %calltmp4 = call i32 @_Z3numi(i32 2)
  %remtmp = srem i32 %divtmp, %calltmp4
  %subtmp = sub nsw i32 %addtmp, %remtmp
  ret i32 %subtmp
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

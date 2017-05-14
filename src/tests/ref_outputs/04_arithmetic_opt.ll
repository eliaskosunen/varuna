; ModuleID = 'varuna_tmp_input_noopt-710eb25c-edc0-4eca-af72-df4ec0728c29.ll'
source_filename = "Varuna"

declare i32 @_Z3numi(i32) local_unnamed_addr

define i32 @_Z4mainv() local_unnamed_addr {
entry:
  %calltmp = tail call i32 @_Z3numi(i32 1)
  %calltmp1 = tail call i32 @_Z3numi(i32 0)
  %calltmp2 = tail call i32 @_Z3numi(i32 -5)
  %multmp = mul nsw i32 %calltmp2, %calltmp1
  %addtmp = add nsw i32 %multmp, %calltmp
  %calltmp3 = tail call i32 @_Z3numi(i32 42)
  %divtmp = sdiv i32 10, %calltmp3
  %calltmp4 = tail call i32 @_Z3numi(i32 2)
  %remtmp = srem i32 %divtmp, %calltmp4
  %subtmp = sub i32 %addtmp, %remtmp
  ret i32 %subtmp
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

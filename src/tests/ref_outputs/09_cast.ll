; ModuleID = 'varuna_tmp_input_noopt-35fd3f44-92e6-43be-a5bf-530df23df2e3.ll'
source_filename = "Varuna"

declare i32 @_Z3numv()

define i32 @_Z4mainv() {
entry:
  %b = alloca double
  %a = alloca i16
  %calltmp = call i32 @_Z3numv()
  %casttmp = trunc i32 %calltmp to i16
  store i16 %casttmp, i16* %a
  %a1 = load i16, i16* %a
  %casttmp2 = sitofp i16 %a1 to double
  store double %casttmp2, double* %b
  %b3 = load double, double* %b
  %casttmp4 = fptosi double %b3 to i32
  ret i32 %casttmp4
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

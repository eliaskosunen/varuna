; ModuleID = 'test_09_cast'
source_filename = "Varuna"

declare i32 @num()

define i32 @main() {
entry:
  %b = alloca double
  %a = alloca i16
  %calltmp = call i32 @num()
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

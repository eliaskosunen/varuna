; ModuleID = 'test_14_modules'
source_filename = "Varuna"

declare i32 @external_function()

declare i32 @another_function()

define i32 @main() {
entry:
  %b = alloca i32
  %a = alloca i32
  %calltmp = call i32 @external_function()
  store i32 %calltmp, i32* %a
  %a1 = load i32, i32* %a
  %divtmp = sdiv i32 %a1, 2
  store i32 %divtmp, i32* %a
  %calltmp2 = call i32 @another_function()
  store i32 %calltmp2, i32* %b
  %b3 = load i32, i32* %b
  %multmp = mul nsw i32 %b3, 2
  store i32 %multmp, i32* %b
  %a4 = load i32, i32* %a
  %b5 = load i32, i32* %b
  %addtmp = add nsw i32 %a4, %b5
  ret i32 %addtmp
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

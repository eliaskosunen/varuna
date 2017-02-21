; ModuleID = 'test_12_char'
source_filename = "Varuna"

declare i32 @asc(i32)

declare i32 @chr(i32)

define i32 @main() {
entry:
  %smile = alloca i32
  %c = alloca i32
  store i32 97, i32* %c
  %c1 = load i32, i32* %c
  store i32 228, i32* %c
  store i32 128512, i32* %smile
  %smile2 = load i32, i32* %smile
  %calltmp = call i32 @asc(i32 %smile2)
  %c3 = load i32, i32* %c
  %calltmp4 = call i32 @asc(i32 %c3)
  %divtmp = sdiv i32 %calltmp, %calltmp4
  ret i32 %divtmp
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

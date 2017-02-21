; ModuleID = 'test_11_alias'
source_filename = "Varuna"

declare i32 @func(i32)

define i32 @main() {
entry:
  %n = alloca i32
  store i32 10, i32* %n
  %n1 = load i32, i32* %n
  %calltmp = call i32 @func(i32 %n1)
  ret i32 %calltmp
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

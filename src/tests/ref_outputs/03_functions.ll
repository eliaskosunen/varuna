; ModuleID = 'test_03_functions'
source_filename = "Varuna"

declare i32 @__va_declaration(i32)

define internal i32 @__va_definition(i32 %arg) {
entry:
  %arg1 = alloca i32
  store i32 %arg, i32* %arg1
  %arg2 = load i32, i32* %arg1
  %calltmp = call i32 @__va_declaration(i32 %arg2)
  ret i32 %calltmp
}

define i32 @__va_main() {
entry:
  %calltmp = call i32 @__va_definition(i32 0)
  ret i32 %calltmp
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

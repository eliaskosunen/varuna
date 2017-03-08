; ModuleID = 'test_10_strings'
source_filename = "Varuna"

%string = type { i64, i8* }

@.str = internal constant [12 x i8] c"Hello world!"
@.str.1 = internal constant [12 x i8] c"Hei maailma!"

declare i32 @__va_func(%string)

define i32 @__va_main() {
entry:
  %s = alloca %string
  store %string { i64 12, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str, i32 0, i32 0) }, %string* %s
  %s1 = load %string, %string* %s
  store %string { i64 12, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.1, i32 0, i32 0) }, %string* %s
  %s2 = load %string, %string* %s
  %calltmp = call i32 @__va_func(%string %s2)
  ret i32 %calltmp
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

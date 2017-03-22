; ModuleID = 'varuna_tmp_input_noopt-ea216e5f-252a-4269-b05d-3ca8c31e3b45.ll'
source_filename = "Varuna"

%string = type { i64, i8* }

@.str = internal constant [12 x i8] c"Hello world!"
@.str.1 = internal constant [12 x i8] c"Hei maailma!"

declare i32 @_Z4func6string(%string)

define i32 @_Z4mainv() {
entry:
  %s = alloca %string
  store %string { i64 12, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str, i32 0, i32 0) }, %string* %s
  %s1 = load %string, %string* %s
  %calltmp = call i32 @_Z4func6string(%string %s1)
  %s2 = load %string, %string* %s
  store %string { i64 12, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.1, i32 0, i32 0) }, %string* %s
  %s3 = load %string, %string* %s
  %calltmp4 = call i32 @_Z4func6string(%string %s3)
  ret i32 %calltmp4
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

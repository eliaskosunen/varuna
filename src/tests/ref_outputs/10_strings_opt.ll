; ModuleID = 'varuna_tmp_input_noopt-442ddfca-5a6c-4fde-8a90-1b603011446b.ll'
source_filename = "Varuna"

%string = type { i64, i8* }

@.str = internal constant [12 x i8] c"Hello world!"
@.str.1 = internal constant [12 x i8] c"Hei maailma!"

declare i32 @_Z4func6string(%string) local_unnamed_addr

define i32 @_Z4mainv() local_unnamed_addr {
entry:
  %calltmp = tail call i32 @_Z4func6string(%string { i64 12, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str, i32 0, i32 0) })
  %calltmp4 = tail call i32 @_Z4func6string(%string { i64 12, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.1, i32 0, i32 0) })
  ret i32 %calltmp4
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

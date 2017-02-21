; ModuleID = 'test_10_strings'
source_filename = "Varuna"

%string = type { i64, i8* }

@.str.1 = internal constant [12 x i8] c"Hei maailma!"

declare i32 @func(%string) local_unnamed_addr

define i32 @main() local_unnamed_addr {
entry:
  %calltmp = tail call i32 @func(%string { i64 12, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.1, i32 0, i32 0) })
  ret i32 %calltmp
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

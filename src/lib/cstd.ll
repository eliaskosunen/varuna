; ModuleID = 'cstd'
source_filename = "/home/elias/code/projects/varuna/varuna/src/lib/cstd.va"

%string = type { i64, i8* }

declare i8* @__va_str_to_cstr(%string)

declare %string @__va_cstr_to_str(i8*)

declare %string @__va_stralloc(i64)

declare i8* @__va_cstralloc(i64)

declare void @__va_strfree(%string)

declare void @__va_cstrfree(i8*)

declare i32 @__va_getchar()

declare i1 @__va_gets(%string, i32)

declare i1 @__va_getsc(i8*, i32)

declare i32 @__va_putchar(i32)

declare i32 @__va_puts(%string)

declare i32 @__va_putsc(i8*)

declare void @__va_perror(%string)

declare void @__va_perrorc(i8*)

declare i32 @__va_eof()

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "Varuna (version 0.1.0-dev)", isOptimized: false, runtimeVersion: 1, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "/home/elias/code/projects/varuna/varuna/src/lib/cstd.va", directory: ".")
!2 = !{}
!3 = !{i32 1, !"Debug Info Version", i32 3}

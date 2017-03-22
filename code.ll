; ModuleID = 'varuna_tmp_input_noopt-7f7d9e36-c879-48cd-b23c-bff5204ec5c7.ll'
source_filename = "code.va"

%string = type { i64, i8* }

@.str = internal constant [12 x i8] c"Hello world!"
@.str.1 = internal constant [24 x i8] c"Please give some input: "
@.str.2 = internal constant [10 x i8] c"You gave: "
@.str.3 = internal constant [0 x i8] zeroinitializer

declare i1 @_Z6getstr6stringi(%string, i32)

declare void @_Z8cstrfreePc(i8*)

declare %string @_Z8strallocl(i64)

declare i8* @_Z11str_to_cstr6string(%string)

declare void @perror(i8*)

declare i8* @_Z9cstrallocl(i64)

declare i32 @_Z6putstr6string(%string)

declare %string @_Z11cstr_to_strPc(i8*)

declare i32 @_Z3eofv()

declare void @_Z7strfree6string(%string)

declare void @_Z9perrorstr6string(%string)

declare i8* @gets(i8*, i32)

declare i32 @puts(i8*)

define i32 @_Z4mainv() !dbg !4 {
entry:
  %buf = alloca %string, !dbg !5
  %calltmp = call i32 @_Z6putstr6string(%string { i64 12, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str, i32 0, i32 0) }), !dbg !5
  %calltmp1 = call %string @_Z8strallocl(i64 64), !dbg !7
  call void @llvm.dbg.declare(metadata %string* %buf, metadata !8, metadata !9), !dbg !10
  store %string %calltmp1, %string* %buf, !dbg !10
  %calltmp2 = call i32 @_Z6putstr6string(%string { i64 24, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.1, i32 0, i32 0) }), !dbg !11
  %buf3 = load %string, %string* %buf, !dbg !12
  %calltmp4 = call i1 @_Z6getstr6stringi(%string %buf3, i32 63), !dbg !13
  %calltmp5 = call i32 @_Z6putstr6string(%string { i64 10, i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.2, i32 0, i32 0) }), !dbg !14
  %buf6 = load %string, %string* %buf, !dbg !15
  %calltmp7 = call i32 @_Z6putstr6string(%string %buf6), !dbg !15
  %calltmp8 = call i32 @_Z3eofv(), !dbg !17
  %eqtmp = icmp eq i32 %calltmp7, %calltmp8, !dbg !17
  br i1 %eqtmp, label %if.then, label %if.merge, !dbg !17

if.then:                                          ; preds = %entry
  call void @_Z9perrorstr6string(%string { i64 0, i8* getelementptr inbounds ([0 x i8], [0 x i8]* @.str.3, i32 0, i32 0) }), !dbg !18
  %buf10 = load %string, %string* %buf, !dbg !20
  call void @_Z7strfree6string(%string %buf10), !dbg !20
  ret i32 1, !dbg !21

if.merge:                                         ; preds = %entry
  %buf12 = load %string, %string* %buf, !dbg !22
  call void @_Z7strfree6string(%string %buf12), !dbg !22
  ret i32 0, !dbg !23
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata, metadata) #0

attributes #0 = { nounwind readnone }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "Varuna (version 0.1.0-dev)", isOptimized: false, runtimeVersion: 1, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "code.va", directory: ".")
!2 = !{}
!3 = !{i32 1, !"Debug Info Version", i32 3}
!4 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 11, isLocal: false, isDefinition: true, scopeLine: 11, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!5 = !DILocation(line: 12, column: 27, scope: !6)
!6 = distinct !DILexicalBlock(scope: !4, file: !1, line: 12)
!7 = !DILocation(line: 13, column: 27, scope: !6)
!8 = !DILocalVariable(name: "buf", scope: !6, file: !1, line: 13)
!9 = !DIExpression()
!10 = !DILocation(line: 13, column: 9, scope: !6)
!11 = !DILocation(line: 14, column: 39, scope: !6)
!12 = !DILocation(line: 15, column: 16, scope: !6)
!13 = !DILocation(line: 15, column: 20, scope: !6)
!14 = !DILocation(line: 16, column: 25, scope: !6)
!15 = !DILocation(line: 17, column: 19, scope: !16)
!16 = distinct !DILexicalBlock(scope: !6, file: !1, line: 17)
!17 = !DILocation(line: 17, column: 27, scope: !16)
!18 = !DILocation(line: 18, column: 22, scope: !19)
!19 = distinct !DILexicalBlock(scope: !16, file: !1, line: 18)
!20 = !DILocation(line: 19, column: 21, scope: !19)
!21 = !DILocation(line: 20, column: 18, scope: !19)
!22 = !DILocation(line: 22, column: 17, scope: !6)
!23 = !DILocation(line: 23, column: 14, scope: !6)

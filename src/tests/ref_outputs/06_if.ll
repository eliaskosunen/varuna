; ModuleID = 'varuna_tmp_input_noopt-ef9dd412-38fe-49a0-aa76-dee57f6f1cca.ll'
source_filename = "Varuna"

declare i1 @_Z4condv()

define i32 @_Z4mainv() {
entry:
  %calltmp = call i1 @_Z4condv()
  br i1 %calltmp, label %if.then, label %if.else3

if.then:                                          ; preds = %entry
  %calltmp1 = call i1 @_Z4condv()
  br i1 %calltmp1, label %if.then2, label %if.else

if.then2:                                         ; preds = %if.then
  ret i32 0

if.else:                                          ; preds = %if.then
  ret i32 1

if.merge:                                         ; No predecessors!
  br label %if.merge8

if.else3:                                         ; preds = %entry
  %calltmp4 = call i1 @_Z4condv()
  br i1 %calltmp4, label %if.then5, label %if.else6

if.then5:                                         ; preds = %if.else3
  ret i32 2

if.else6:                                         ; preds = %if.else3
  ret i32 3

if.merge7:                                        ; No predecessors!
  br label %if.merge8

if.merge8:                                        ; preds = %if.merge7, %if.merge
  unreachable
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

; ModuleID = 'varuna_tmp_input_noopt-26ea67a3-7f82-4306-81eb-77f97d2f394c.ll'
source_filename = "Varuna"

declare i1 @_Z4condv()

define i32 @_Z4mainv() {
entry:
  %b = alloca i32
  %a = alloca i32
  store i32 1, i32* %a
  br label %while.cond

while.cond:                                       ; preds = %while.merge, %entry
  %calltmp = call i1 @_Z4condv()
  br i1 %calltmp, label %while.body, label %while.merge7

while.body:                                       ; preds = %while.cond
  store i32 2, i32* %b
  br label %while.cond1

while.cond1:                                      ; preds = %while.body3, %while.body
  %calltmp2 = call i1 @_Z4condv()
  %nottmp = xor i1 %calltmp2, true
  br i1 %nottmp, label %while.body3, label %while.merge

while.body3:                                      ; preds = %while.cond1
  %b4 = load i32, i32* %b
  %multmp = mul nsw i32 %b4, 2
  store i32 %multmp, i32* %b
  br label %while.cond1

while.merge:                                      ; preds = %while.cond1
  %a5 = load i32, i32* %a
  %b6 = load i32, i32* %b
  %addtmp = add nsw i32 %a5, %b6
  store i32 %addtmp, i32* %a
  br label %while.cond

while.merge7:                                     ; preds = %while.cond
  %a8 = load i32, i32* %a
  ret i32 %a8
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

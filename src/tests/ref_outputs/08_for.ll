; ModuleID = 'varuna_tmp_input_noopt-5195967b-63c9-4e5c-ac8c-e67c7a679b9b.ll'
source_filename = "Varuna"

declare i32 @_Z5countv()

define i32 @_Z4mainv() {
entry:
  %j = alloca i32
  %i = alloca i32
  %a = alloca i32
  store i32 1, i32* %a
  br label %for.init

for.init:                                         ; preds = %entry
  store i32 1, i32* %i
  br label %for.cond

for.cond:                                         ; preds = %for.step11, %for.init
  %i1 = load i32, i32* %i
  %calltmp = call i32 @_Z5countv()
  %letmp = icmp sle i32 %i1, %calltmp
  br i1 %letmp, label %for.body, label %for.merge14

for.body:                                         ; preds = %for.cond
  br label %for.init2

for.init2:                                        ; preds = %for.body
  store i32 1, i32* %j
  br label %for.cond3

for.cond3:                                        ; preds = %for.step, %for.init2
  %j4 = load i32, i32* %j
  %letmp5 = icmp sle i32 %j4, 5
  br i1 %letmp5, label %for.body6, label %for.merge

for.body6:                                        ; preds = %for.cond3
  %a7 = load i32, i32* %a
  %multmp = mul nsw i32 %a7, 2
  store i32 %multmp, i32* %a
  br label %for.step

for.step:                                         ; preds = %for.body6
  %j8 = load i32, i32* %j
  %addtmp = add nsw i32 %j8, 1
  store i32 %addtmp, i32* %j
  br label %for.cond3

for.merge:                                        ; preds = %for.cond3
  %a9 = load i32, i32* %a
  %addtmp10 = add nsw i32 %a9, 1
  store i32 %addtmp10, i32* %a
  br label %for.step11

for.step11:                                       ; preds = %for.merge
  %i12 = load i32, i32* %i
  %addtmp13 = add nsw i32 %i12, 1
  store i32 %addtmp13, i32* %i
  br label %for.cond

for.merge14:                                      ; preds = %for.cond
  %a15 = load i32, i32* %a
  ret i32 %a15
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

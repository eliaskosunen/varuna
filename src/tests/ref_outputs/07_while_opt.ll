; ModuleID = 'varuna_tmp_input_noopt-0ec1391a-0314-44e4-a597-8926042335e1.ll'
source_filename = "Varuna"

declare i1 @_Z4condv() local_unnamed_addr

define i32 @_Z4mainv() local_unnamed_addr {
entry:
  %calltmp3 = tail call i1 @_Z4condv()
  br i1 %calltmp3, label %while.cond1.preheader.preheader, label %while.merge7

while.cond1.preheader.preheader:                  ; preds = %entry
  br label %while.cond1.preheader

while.cond1.preheader:                            ; preds = %while.cond1.preheader.preheader, %while.merge
  %a.04 = phi i32 [ %addtmp, %while.merge ], [ 1, %while.cond1.preheader.preheader ]
  br label %while.cond1

while.cond1:                                      ; preds = %while.cond1, %while.cond1.preheader
  %b.0 = phi i32 [ %multmp, %while.cond1 ], [ 2, %while.cond1.preheader ]
  %calltmp2 = tail call i1 @_Z4condv()
  %multmp = shl nsw i32 %b.0, 1
  br i1 %calltmp2, label %while.merge, label %while.cond1

while.merge:                                      ; preds = %while.cond1
  %addtmp = add nsw i32 %b.0, %a.04
  %calltmp = tail call i1 @_Z4condv()
  br i1 %calltmp, label %while.cond1.preheader, label %while.merge7.loopexit

while.merge7.loopexit:                            ; preds = %while.merge
  br label %while.merge7

while.merge7:                                     ; preds = %while.merge7.loopexit, %entry
  %a.0.lcssa = phi i32 [ 1, %entry ], [ %addtmp, %while.merge7.loopexit ]
  ret i32 %a.0.lcssa
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

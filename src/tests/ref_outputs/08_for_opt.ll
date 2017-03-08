; ModuleID = 'test_08_for'
source_filename = "Varuna"

declare i32 @__va_count() local_unnamed_addr

define i32 @__va_main() local_unnamed_addr {
entry:
  %calltmp21 = tail call i32 @__va_count()
  %letmp22 = icmp slt i32 %calltmp21, 1
  br i1 %letmp22, label %for.merge14, label %for.cond3.preheader.preheader

for.cond3.preheader.preheader:                    ; preds = %entry
  br label %for.cond3.preheader

for.cond3.preheader:                              ; preds = %for.cond3.preheader.preheader, %for.cond3.preheader
  %a.024 = phi i32 [ %addtmp10, %for.cond3.preheader ], [ 1, %for.cond3.preheader.preheader ]
  %i.023 = phi i32 [ %addtmp13, %for.cond3.preheader ], [ 1, %for.cond3.preheader.preheader ]
  %multmp = shl i32 %a.024, 5
  %addtmp10 = or i32 %multmp, 1
  %addtmp13 = add nuw nsw i32 %i.023, 1
  %calltmp = tail call i32 @__va_count()
  %letmp = icmp slt i32 %i.023, %calltmp
  br i1 %letmp, label %for.cond3.preheader, label %for.merge14.loopexit

for.merge14.loopexit:                             ; preds = %for.cond3.preheader
  br label %for.merge14

for.merge14:                                      ; preds = %for.merge14.loopexit, %entry
  %a.0.lcssa = phi i32 [ 1, %entry ], [ %addtmp10, %for.merge14.loopexit ]
  ret i32 %a.0.lcssa
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

; ModuleID = 'test_14_modules_importee'
source_filename = "Varuna"

; Function Attrs: norecurse nounwind readnone
define i32 @external_function() local_unnamed_addr #0 {
entry:
  ret i32 10
}

; Function Attrs: norecurse nounwind readnone
define i32 @another_function() local_unnamed_addr #0 {
entry:
  ret i32 -89
}

attributes #0 = { norecurse nounwind readnone }

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

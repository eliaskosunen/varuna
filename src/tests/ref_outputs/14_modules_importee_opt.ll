; ModuleID = 'varuna_tmp_input_noopt-f5daa037-0cb4-423c-8b28-141827f418e2.ll'
source_filename = "Varuna"

; Function Attrs: norecurse nounwind readnone
define i32 @_Z17external_functionv() local_unnamed_addr #0 {
entry:
  ret i32 10
}

; Function Attrs: norecurse nounwind readnone
define i32 @_Z16another_functionv() local_unnamed_addr #0 {
entry:
  ret i32 -89
}

attributes #0 = { norecurse nounwind readnone }

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

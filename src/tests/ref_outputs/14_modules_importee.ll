; ModuleID = 'varuna_tmp_input_noopt-8afebbab-54a4-4699-91b8-03193d520b43.ll'
source_filename = "Varuna"

define i32 @_Z17external_functionv() {
entry:
  ret i32 10
}

define i32 @_Z16another_functionv() {
entry:
  ret i32 -89
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

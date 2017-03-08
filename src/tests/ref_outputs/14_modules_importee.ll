; ModuleID = 'test_14_modules_importee'
source_filename = "Varuna"

define i32 @external_function() {
entry:
  ret i32 10
}

define i32 @another_function() {
entry:
  ret i32 -89
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

; ModuleID = 'varuna_tmp_input_noopt-5a19a4a1-2d4b-4057-9038-5c2dab6d80ff.ll'
source_filename = "Varuna"

define i32 @_Z4mainv() {
entry:
  %mutable = alloca i32
  %immutable = alloca i32
  store i32 1, i32* %immutable
  store i32 2, i32* %mutable
  %mutable1 = load i32, i32* %mutable
  store i32 0, i32* %mutable
  %mutable2 = load i32, i32* %mutable
  store i32 1, i32* %mutable
  %immutable3 = load i32, i32* %immutable
  %mutable4 = load i32, i32* %mutable
  %subtmp = sub nsw i32 %immutable3, %mutable4
  ret i32 %subtmp
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

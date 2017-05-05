; ModuleID = 'varuna_tmp_input_noopt-60881899-f1ed-4303-8e89-6555f9da8531.ll'
source_filename = "Varuna"

declare i32 @_Z3ascDi(i32)

declare i32 @_Z3chri(i32)

define i32 @_Z4mainv() {
entry:
  %smile = alloca i32
  %c = alloca i32
  store i32 97, i32* %c
  %c1 = load i32, i32* %c
  store i32 228, i32* %c
  store i32 128512, i32* %smile
  %smile2 = load i32, i32* %smile
  %calltmp = call i32 @_Z3ascDi(i32 %smile2)
  %c3 = load i32, i32* %c
  %calltmp4 = call i32 @_Z3ascDi(i32 %c3)
  %divtmp = sdiv i32 %calltmp, %calltmp4
  ret i32 %divtmp
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

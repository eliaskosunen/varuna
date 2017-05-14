; ModuleID = 'varuna_tmp_input_noopt-a5b5cbcf-a309-482e-b0fc-73c914040faf.ll'
source_filename = "Varuna"

@constant = internal constant i32 1
@variable = internal global i32 2

define i32 @_Z4mainv() {
entry:
  %local_variable = alloca i32
  %local_constant = alloca i32
  store i32 3, i32* %local_constant
  store i32 4, i32* %local_variable
  %variable = load i32, i32* @variable
  store i32 5, i32* @variable
  %local_variable1 = load i32, i32* %local_variable
  store i32 6, i32* %local_variable
  %constant = load i32, i32* @constant
  %variable2 = load i32, i32* @variable
  %addtmp = add nsw i32 %constant, %variable2
  ret i32 %addtmp
}

!llvm.module.flags = !{!0}

!0 = !{i32 1, !"Debug Info Version", i32 3}

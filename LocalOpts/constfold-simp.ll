; ModuleID = 'constfold-simp.bc'
source_filename = "inputFiles/constfold.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @compute() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i32 2, i32* %2, align 4
  store i32 3, i32* %3, align 4
  %5 = load i32, i32* %2, align 4
  %6 = add nsw i32 4, %5
  %7 = load i32, i32* %3, align 4
  %8 = add nsw i32 %6, %7
  store i32 %8, i32* %4, align 4
  %9 = load i32, i32* %2, align 4
  %10 = load i32, i32* %1, align 4
  %11 = add nsw i32 %10, %9
  store i32 %11, i32* %1, align 4
  %12 = load i32, i32* %3, align 4
  %13 = load i32, i32* %1, align 4
  %14 = add nsw i32 %13, %12
  store i32 %14, i32* %1, align 4
  %15 = load i32, i32* %4, align 4
  %16 = load i32, i32* %1, align 4
  %17 = mul nsw i32 %16, %15
  store i32 %17, i32* %1, align 4
  %18 = load i32, i32* %1, align 4
  %19 = sdiv i32 %18, 2
  store i32 %19, i32* %1, align 4
  %20 = load i32, i32* %1, align 4
  ret i32 %20
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}

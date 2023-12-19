; ModuleID = 'stack-simp.bc'
source_filename = "inputFiles/stack.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.stack = type { i32, %struct.stack* }

@.str = private unnamed_addr constant [24 x i8] c"failed to assign memory\00", align 1
@head = common dso_local global %struct.stack* null, align 8
@.str.1 = private unnamed_addr constant [27 x i8] c"hello from list number %d\0A\00", align 1
@.str.2 = private unnamed_addr constant [29 x i8] c"poping value from the stack\0A\00", align 1
@.str.3 = private unnamed_addr constant [23 x i8] c"popped the value - %d\0A\00", align 1
@.str.4 = private unnamed_addr constant [30 x i8] c"stack after poping the value\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @push(i32 %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca %struct.stack*, align 8
  store i32 %0, i32* %2, align 4
  %4 = call noalias i8* @malloc(i64 16) #3
  %5 = bitcast i8* %4 to %struct.stack*
  store %struct.stack* %5, %struct.stack** %3, align 8
  %6 = load %struct.stack*, %struct.stack** %3, align 8
  %7 = icmp eq %struct.stack* %6, null
  br i1 %7, label %8, label %10

8:                                                ; preds = %1
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str, i64 0, i64 0))
  br label %10

10:                                               ; preds = %8, %1
  %11 = load %struct.stack*, %struct.stack** @head, align 8
  %12 = load %struct.stack*, %struct.stack** %3, align 8
  %13 = getelementptr inbounds %struct.stack, %struct.stack* %12, i32 0, i32 1
  store %struct.stack* %11, %struct.stack** %13, align 8
  %14 = load i32, i32* %2, align 4
  %15 = load %struct.stack*, %struct.stack** %3, align 8
  %16 = getelementptr inbounds %struct.stack, %struct.stack* %15, i32 0, i32 0
  store i32 %14, i32* %16, align 8
  %17 = load %struct.stack*, %struct.stack** %3, align 8
  store %struct.stack* %17, %struct.stack** @head, align 8
  ret void
}

; Function Attrs: nounwind
declare dso_local noalias i8* @malloc(i64) #1

declare dso_local i32 @printf(i8*, ...) #2

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @pop() #0 {
  %1 = alloca %struct.stack*, align 8
  %2 = alloca i32, align 4
  %3 = load %struct.stack*, %struct.stack** @head, align 8
  store %struct.stack* %3, %struct.stack** %1, align 8
  %4 = load %struct.stack*, %struct.stack** %1, align 8
  %5 = getelementptr inbounds %struct.stack, %struct.stack* %4, i32 0, i32 0
  %6 = load i32, i32* %5, align 8
  store i32 %6, i32* %2, align 4
  %7 = load %struct.stack*, %struct.stack** @head, align 8
  %8 = getelementptr inbounds %struct.stack, %struct.stack* %7, i32 0, i32 1
  %9 = load %struct.stack*, %struct.stack** %8, align 8
  store %struct.stack* %9, %struct.stack** @head, align 8
  %10 = load %struct.stack*, %struct.stack** %1, align 8
  %11 = bitcast %struct.stack* %10 to i8*
  call void @free(i8* %11) #3
  %12 = load i32, i32* %2, align 4
  ret i32 %12
}

; Function Attrs: nounwind
declare dso_local void @free(i8*) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @printLinkList() #0 {
  %1 = alloca %struct.stack*, align 8
  %2 = load %struct.stack*, %struct.stack** @head, align 8
  store %struct.stack* %2, %struct.stack** %1, align 8
  br label %3

3:                                                ; preds = %6, %0
  %4 = load %struct.stack*, %struct.stack** %1, align 8
  %5 = icmp ne %struct.stack* %4, null
  br i1 %5, label %6, label %14

6:                                                ; preds = %3
  %7 = load %struct.stack*, %struct.stack** %1, align 8
  %8 = getelementptr inbounds %struct.stack, %struct.stack* %7, i32 0, i32 0
  %9 = load i32, i32* %8, align 8
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([27 x i8], [27 x i8]* @.str.1, i64 0, i64 0), i32 %9)
  %11 = load %struct.stack*, %struct.stack** %1, align 8
  %12 = getelementptr inbounds %struct.stack, %struct.stack* %11, i32 0, i32 1
  %13 = load %struct.stack*, %struct.stack** %12, align 8
  store %struct.stack* %13, %struct.stack** %1, align 8
  br label %3

14:                                               ; preds = %3
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  %2 = call noalias i8* @malloc(i64 16) #3
  %3 = bitcast i8* %2 to %struct.stack*
  store %struct.stack* %3, %struct.stack** @head, align 8
  %4 = load %struct.stack*, %struct.stack** @head, align 8
  %5 = icmp eq %struct.stack* %4, null
  br i1 %5, label %6, label %8

6:                                                ; preds = %0
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str, i64 0, i64 0))
  br label %8

8:                                                ; preds = %6, %0
  %9 = load %struct.stack*, %struct.stack** @head, align 8
  %10 = getelementptr inbounds %struct.stack, %struct.stack* %9, i32 0, i32 0
  store i32 2, i32* %10, align 8
  %11 = load %struct.stack*, %struct.stack** @head, align 8
  %12 = getelementptr inbounds %struct.stack, %struct.stack* %11, i32 0, i32 1
  store %struct.stack* null, %struct.stack** %12, align 8
  call void @push(i32 3)
  call void @printLinkList()
  %13 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([29 x i8], [29 x i8]* @.str.2, i64 0, i64 0))
  %14 = call i32 @pop()
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([23 x i8], [23 x i8]* @.str.3, i64 0, i64 0), i32 %14)
  %16 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([30 x i8], [30 x i8]* @.str.4, i64 0, i64 0))
  call void @printLinkList()
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}

/* ===== Original LLVM IR ===== */
; ModuleID = 'cbe_demo'
source_filename = "cbe_demo"

@symbol_table = private unnamed_addr constant [2 x i64] [i64 1069547520, i64 1069547520], align 8
@llvm.used = appending global [1 x ptr] [ptr @symbol_table], section "llvm.metadata"

define float @demo_add() {
bb0:
  %r0 = alloca i64, align 8
  %r1 = alloca i64, align 8
  %r2 = alloca i64, align 8
  %r3 = alloca i64, align 8
  %r4 = alloca i64, align 8
  %r5 = alloca i64, align 8
  %r6 = alloca i64, align 8
  %r7 = alloca i64, align 8
  %r8 = alloca i64, align 8
  %r9 = alloca i64, align 8
  %r10 = alloca i64, align 8
  %r11 = alloca i64, align 8
  %r12 = alloca i64, align 8
  %r13 = alloca i64, align 8
  %r14 = alloca i64, align 8
  %r15 = alloca i64, align 8
  %r16 = alloca i64, align 8
  %r17 = alloca i64, align 8
  %r18 = alloca i64, align 8
  %r19 = alloca i64, align 8
  %r20 = alloca i64, align 8
  %r21 = alloca i64, align 8
  %r22 = alloca i64, align 8
  %r23 = alloca i64, align 8
  %r24 = alloca i64, align 8
  %r25 = alloca i64, align 8
  %r26 = alloca i64, align 8
  %r27 = alloca i64, align 8
  %r28 = alloca i64, align 8
  %r29 = alloca i64, align 8
  %r30 = alloca i64, align 8
  %r31 = alloca i64, align 8
  %r32 = alloca i64, align 8
  %r33 = alloca i64, align 8
  %r34 = alloca i64, align 8
  %r35 = alloca i64, align 8
  %r36 = alloca i64, align 8
  %r37 = alloca i64, align 8
  %r38 = alloca i64, align 8
  %r39 = alloca i64, align 8
  %r40 = alloca i64, align 8
  %r41 = alloca i64, align 8
  %r42 = alloca i64, align 8
  %r43 = alloca i64, align 8
  %r44 = alloca i64, align 8
  %r45 = alloca i64, align 8
  %r46 = alloca i64, align 8
  %r47 = alloca i64, align 8
  %r48 = alloca i64, align 8
  %r49 = alloca i64, align 8
  %r50 = alloca i64, align 8
  %r51 = alloca i64, align 8
  %r52 = alloca i64, align 8
  %r53 = alloca i64, align 8
  %r54 = alloca i64, align 8
  %r55 = alloca i64, align 8
  %r56 = alloca i64, align 8
  %r57 = alloca i64, align 8
  %r58 = alloca i64, align 8
  %r59 = alloca i64, align 8
  %r60 = alloca i64, align 8
  %r61 = alloca i64, align 8
  %r62 = alloca i64, align 8
  %r63 = alloca i64, align 8
  %r64 = alloca i64, align 8
  %r65 = alloca i64, align 8
  %r66 = alloca i64, align 8
  %r67 = alloca i64, align 8
  %r68 = alloca i64, align 8
  %r69 = alloca i64, align 8
  %r70 = alloca i64, align 8
  %r71 = alloca i64, align 8
  %r72 = alloca i64, align 8
  %r73 = alloca i64, align 8
  %r74 = alloca i64, align 8
  %r75 = alloca i64, align 8
  %r76 = alloca i64, align 8
  %r77 = alloca i64, align 8
  %r78 = alloca i64, align 8
  %r79 = alloca i64, align 8
  %r80 = alloca i64, align 8
  %r81 = alloca i64, align 8
  %r82 = alloca i64, align 8
  %r83 = alloca i64, align 8
  %r84 = alloca i64, align 8
  %r85 = alloca i64, align 8
  %r86 = alloca i64, align 8
  %r87 = alloca i64, align 8
  %r88 = alloca i64, align 8
  %r89 = alloca i64, align 8
  %r90 = alloca i64, align 8
  %r91 = alloca i64, align 8
  %r92 = alloca i64, align 8
  %r93 = alloca i64, align 8
  %r94 = alloca i64, align 8
  %r95 = alloca i64, align 8
  %r96 = alloca i64, align 8
  %r97 = alloca i64, align 8
  %__LoadStaticFloatImm_0___st_load = load float, ptr @symbol_table, align 4
  store float %__LoadStaticFloatImm_0___st_load, ptr %r0, align 4
  store i16 0, ptr %r9, align 2
  store i16 0, ptr %r1, align 2
  store i16 4, ptr %r2, align 2
  store i16 1, ptr %r5, align 2
  %__IAdd_5___load_lhs = load i16, ptr %r1, align 2
  %__IAdd_5___load_rhs = load i16, ptr %r9, align 2
  %__IAdd_5___op = add i16 %__IAdd_5___load_lhs, %__IAdd_5___load_rhs
  store i16 %__IAdd_5___op, ptr %r1, align 2
  %__IAdd_6___load_lhs = load i16, ptr %r2, align 2
  %__IAdd_6___load_rhs = load i16, ptr %r9, align 2
  %__IAdd_6___op = add i16 %__IAdd_6___load_lhs, %__IAdd_6___load_rhs
  store i16 %__IAdd_6___op, ptr %r2, align 2
  %__IAdd_7___load_lhs = load i16, ptr %r5, align 2
  %__IAdd_7___load_rhs = load i16, ptr %r9, align 2
  %__IAdd_7___op = add i16 %__IAdd_7___load_lhs, %__IAdd_7___load_rhs
  store i16 %__IAdd_7___op, ptr %r5, align 2
  %__IGreaterThan_8___load_lhs = load i64, ptr %r2, align 4
  %__IGreaterThan_8___load_rhs = load i64, ptr %r1, align 4
  %__IGreaterThan_8___op = icmp sgt i64 %__IGreaterThan_8___load_lhs, %__IGreaterThan_8___load_rhs
  store i1 %__IGreaterThan_8___op, ptr %r4, align 1
  %__BranchIfNot_9___cond = load i1, ptr %r4, align 1
  br i1 %__BranchIfNot_9___cond, label %bb1, label %bb3

bb1:                                              ; preds = %bb2, %bb0
  %__IGreaterThanEqual_10___load_lhs = load i64, ptr %r1, align 4
  %__IGreaterThanEqual_10___load_rhs = load i64, ptr %r2, align 4
  %__IGreaterThanEqual_10___op = icmp sge i64 %__IGreaterThanEqual_10___load_lhs, %__IGreaterThanEqual_10___load_rhs
  store i1 %__IGreaterThanEqual_10___op, ptr %r3, align 1
  %__BranchIf_11___cond = load i1, ptr %r3, align 1
  br i1 %__BranchIf_11___cond, label %bb3, label %bb2

bb2:                                              ; preds = %bb1
  %__LoadStaticFloatImm_12___st_load = load float, ptr getelementptr inbounds ([2 x i64], ptr @symbol_table, i64 0, i64 1), align 4
  store float %__LoadStaticFloatImm_12___st_load, ptr %r6, align 4
  %__FAdd_13___load_lhs = load float, ptr %r0, align 4
  %__FAdd_13___load_rhs = load float, ptr %r6, align 4
  %__FAdd_13___op = fadd float %__FAdd_13___load_lhs, %__FAdd_13___load_rhs
  store float %__FAdd_13___op, ptr %r0, align 4
  %__IAdd_14___load_lhs = load i64, ptr %r1, align 4
  %__IAdd_14___load_rhs = load i64, ptr %r5, align 4
  %__IAdd_14___op = add i64 %__IAdd_14___load_lhs, %__IAdd_14___load_rhs
  store i64 %__IAdd_14___op, ptr %r1, align 4
  br label %bb1

bb3:                                              ; preds = %bb1, %bb0
  %__Return_16___retval = load float, ptr %r0, align 4
  ret float %__Return_16___retval
}

/* ===== Optimized LLVM IR ===== */
; ModuleID = 'cbe_demo'
source_filename = "cbe_demo"
target triple = "x86_64-pc-windows-msvc"

@symbol_table = private unnamed_addr constant [2 x i64] [i64 1069547520, i64 1069547520], align 8
@llvm.used = appending global [1 x ptr] [ptr @symbol_table], section "llvm.metadata"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define noundef float @demo_add() local_unnamed_addr #0 {
bb0:
  ret float 7.500000e+00
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }

/* ===== C backend ===== */
/* Provide Declarations */
#include <stdint.h>
#ifndef __cplusplus
typedef unsigned char bool;
#endif

#if defined(__GNUC__)
#define  __ATTRIBUTELIST__(x) __attribute__(x)
#else
#define  __ATTRIBUTELIST__(x)  
#endif

#ifdef _MSC_VER  /* Can only support "linkonce" vars with GCC */
#define __attribute__(X)
#endif

#ifdef _MSC_VER
#define __PREFIXALIGN__(X) __declspec(align(X))
#define __POSTFIXALIGN__(X)
#else
#define __PREFIXALIGN__(X)
#define __POSTFIXALIGN__(X) __attribute__((aligned(X)))
#endif



/* Global Declarations */

/* Types Declarations */
struct l_array_2_uint64_t;

/* Function definitions */

/* Types Definitions */
struct l_array_2_uint64_t {
  uint64_t array[2];
};

/* Global Variable Declarations */
const static __PREFIXALIGN__(8) struct l_array_2_uint64_t symbol_table __POSTFIXALIGN__(8);

/* Function Declarations */
float demo_add(void) __ATTRIBUTELIST__((nothrow));


/* Global Variable Definitions and Initialization */
static const __PREFIXALIGN__(8) struct l_array_2_uint64_t symbol_table __POSTFIXALIGN__(8) = { { 1069547520, 1069547520 } };


/* LLVM Intrinsic Builtin Function Bodies */


/* Function Bodies */

float demo_add(void) {
  return 7.5;
}


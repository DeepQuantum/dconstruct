#include "base.h"
#include <windows.h>
#include "MinHook.h"
#include <format>
#include <print>
#include <fstream>
#include <string>
#include <iostream>
#include "disassembly/instructions.h"

namespace dconstruct {
// void parse_instruction(std::byte (*param_1) [16],u64 *param_2,int *param_3)
// {
//   i64 *plVar1;
//   u64 uVar2;
//   std::byte auVar3 [16];
//   u64 op2;
//   i64 lVar4;
//   u64 *puVar5;
//   std::byte (*pauVar6) [16];
//   i64 lVar7;
//   std::byte (*pauVar8) [16];
//   i64 lVar9;
//   i64 *plVar10;
//   i64 symbol_table_ptr;
//   u64 destination;
//   u64 op1;
//   i64 *stackFrame;
//   IMAGE_DOS_HEADER *pIVar11;
//   float fVar12;
//   float float_to_store;
//   std::byte auVar13 [64];
//   std::byte auVar14 [64];
//   std::byte auVar15 [64];
//   std::byte auStack_308 [32];
//   i64 local_2c8;
//   std::byte (*symbol_table_ptr2) [16];
//   i64 local_2b8;
//   u64 local_298 [2];
//   i64 local_288;
//   i64 alStack_280 [65];
//   u64 local_78;
//   undefined *func_ptr;
//   Opcode opcode;
//   i64 *callee;
//   byte destination_;
//   byte operand1;
//   byte operand2;
  
//   local_78 = DAT_14387e318 ^ (u64)auStack_308;
//   local_298[0] = 0;
//   FUN_14160f520(local_298,param_1);
//   op2 = (u64)_tls_index;
//   if ((*(int *)(*(i64 *)((i64)ThreadLocalStoragePointer + op2 * 8) + 0xf8) < DAT_1484ae1b8
//       ) && (_Init_thread_header(&DAT_1484ae1b8), DAT_1484ae1b8 == -1)) {
//     DAT_1484ae1b0 = 1;
//     _Init_thread_footer(&DAT_1484ae1b8);
//   }
//   if ((*(int *)(*(i64 *)((i64)ThreadLocalStoragePointer + op2 * 8) + 0xf8) < DAT_1484ae1c8
//       ) && (_Init_thread_header(&DAT_1484ae1c8), DAT_1484ae1c8 == -1)) {
//     DAT_1484ae1c0 = 0;
//     _Init_thread_footer(&DAT_1484ae1c8);
//   }
//   if ((*(int *)(*(i64 *)((i64)ThreadLocalStoragePointer + op2 * 8) + 0xf8) < DAT_1484ae1d8
//       ) && (_Init_thread_header(&DAT_1484ae1d8), DAT_1484ae1d8 == -1)) {
//     DAT_1484ae1d0 = DAT_1484ae1b0;
//     _Init_thread_footer(&DAT_1484ae1d8);
//   }
//   if ((*(int *)(*(i64 *)((i64)ThreadLocalStoragePointer + op2 * 8) + 0xf8) < DAT_1484ae1e8
//       ) && (_Init_thread_header(&DAT_1484ae1e8), DAT_1484ae1e8 == -1)) {
//     DAT_1484ae1e0 = DAT_1484ae1c0;
//     _Init_thread_footer(&DAT_1484ae1e8);
//   }
//   local_2b8 = (*(i64 **)(*param_1 + 8))[1];
//   local_2c8 = **(i64 **)(*param_1 + 8);
//   stackFrame = (i64 *)(param_1[1] + 8);
//   symbol_table_ptr2 = param_1 + 0x22;
//   auVar14 = ZEXT464(0x80000000);
//   auVar13 = ZEXT464(0x7fffffff);
//   auVar15 = ZEXT464(0x3f800000);
//   lVar9 = DAT_1484ae1d0;
//   lVar7 = DAT_1484ae1e0;
//   symbol_table_ptr = local_2b8;
// switchD_1414b2f81_caseD_5d:
//   while( true ) {
//     pIVar11 = &IMAGE_DOS_HEADER_140000000;
//     op2 = (u64)*(u32 *)*param_1;
//     opcode = *(Opcode *)(local_2c8 + op2 * 8);
//     destination_ = *(byte *)(local_2c8 + 1 + op2 * 8);
//     destination = (u64)destination_;
//     operand1 = *(byte *)(local_2c8 + 2 + op2 * 8);
//     op1 = (u64)operand1;
//     operand2 = *(byte *)(local_2c8 + 3 + op2 * 8);
//     op2 = (u64)operand2;
//     *(u32 *)*param_1 = *(u32 *)*param_1 + 1;
//     if (opcode < BreakFlag) break;
// switchD_1414b2f81_caseD_2b:
//     FUN_1414ba4a0();
// switchD_1414b2f81_caseD_0:
//     param_1 = (std::byte (*) [16])FUN_1414b8240((int *)param_1,param_2);
//     if (param_1 == (std::byte (*) [16])0x0) goto LAB_1414b3ad6;
//     local_2b8 = (*(i64 **)(*param_1 + 8))[1];
//     local_2c8 = **(i64 **)(*param_1 + 8);
//     stackFrame = (i64 *)(param_1[1] + 8);
//     symbol_table_ptr2 = param_1 + 0x22;
//     FUN_141234a50(6,0xc9e40b98fed629eb,param_1);
//     lVar9 = DAT_1484ae1d0;
//     lVar7 = DAT_1484ae1e0;
//     symbol_table_ptr = local_2b8;
//   }
//   switch(opcode) {
//   case Opcode::Return:
//     goto switchD_1414b2f81_caseD_0;
//   case Opcode::IAdd:
//     stackFrame[destination] = stackFrame[op2] + stackFrame[op1];
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::ISub:
//     stackFrame[destination] = stackFrame[op1] - stackFrame[op2];
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::IMul:
//     stackFrame[destination] = stackFrame[op2] * stackFrame[op1];
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::IDiv:
//     stackFrame[destination] = stackFrame[op1] / stackFrame[op2];
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::FAdd:
//     *(float *)(stackFrame + destination) =
//          *(float *)(stackFrame + op2) + *(float *)(stackFrame + op1);
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::FSub:
//     *(float *)(stackFrame + destination) =
//          *(float *)(stackFrame + op1) - *(float *)(stackFrame + op2);
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::FMul:
//     *(float *)(stackFrame + destination) =
//          *(float *)(stackFrame + op2) * *(float *)(stackFrame + op1);
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::FDiv:
//     *(float *)(stackFrame + destination) =
//          *(float *)(stackFrame + op1) / *(float *)(stackFrame + op2);
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::LoadStaticInt:
//   case Opcode::LoadStaticPointer:
//     stackFrame[destination] = *(i64 *)(symbol_table_ptr + stackFrame[op1] * 8);
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::LoadStaticFloat:
//     *(f32 *)(stackFrame + destination) =
//          *(f32 *)(symbol_table_ptr + stackFrame[op1] * 8);
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::LoadU16Imm:
//     stackFrame[destination] = (u64)CONCAT11(operand2,operand1);
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::LoadFloat:
//     *(f32 *)(stackFrame + destination) = *(f32 *)stackFrame[op1];
//     goto switchD_1414b2f81_caseD_5d;
//   default:
//     goto switchD_1414b2f81_caseD_f;
//   case Opcode::StoreInt:
//   case Opcode::StoreI64:
//   case Opcode::StoreU64:
//     *(i64 *)stackFrame[op1] = stackFrame[op2];
// switchD_1414b2f81_caseD_f:
//     stackFrame[destination] = *(i64 *)stackFrame[op1];
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::StoreFloat:
//     float_to_store = *(float *)(stackFrame + op2);
//     if (((u32)float_to_store & 0x7f800000) == 0x7f800000) {
//       pauVar6 = symbol_table_ptr2;
//       FUN_1414ba4a0();
//       error("Script set-float with NaN! (\'%s\')\n",&DAT_142af5450,pauVar6,pIVar11);
//       float_to_store = auVar15._0_4_;
//       lVar9 = DAT_1484ae1d0;
//       lVar7 = DAT_1484ae1e0;
//     }
//     *(float *)stackFrame[op1] = float_to_store;
//     *(float *)(stackFrame + destination) = float_to_store;
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::StorePointer:
//     *(i64 *)stackFrame[op1] = stackFrame[op2];
//     stackFrame[destination] = stackFrame[op2];
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::LookupInt:
//   case Opcode::LookupFloat:
//   case Opcode::LookupPointer:
//     puVar5 = lookup_sid(*(u64 *)(symbol_table_ptr + op1 * 8));
//     *(u64 *)(*symbol_table_ptr2 + destination * 8) =
//          *(u64 *)(symbol_table_ptr + op1 * 8);
//     if (puVar5 == (u64 *)0x0) {
//       stackFrame[destination] = 0;
//       lVar9 = DAT_1484ae1d0;
//       lVar7 = DAT_1484ae1e0;
//     }
//     else {
//       stackFrame[destination] = *puVar5;
//       lVar9 = DAT_1484ae1d0;
//       lVar7 = DAT_1484ae1e0;
//     }
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::MoveInt:
//     stackFrame[destination] = stackFrame[op1];
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::MoveFloat:
//     *(int *)(stackFrame + destination) = (int)stackFrame[op1];
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::MovePointer:
//   case Opcode::Move:
//     stackFrame[destination] = stackFrame[op1];
//     *(u64 *)(*symbol_table_ptr2 + destination * 8) =
//          *(u64 *)(*symbol_table_ptr2 + op1 * 8);
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::CastInteger:
//     stackFrame[destination] = (i64)*(float *)(stackFrame + destination);
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::CastFloat:
//     *(float *)(stackFrame + destination) = (float)stackFrame[destination];
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::Call:
//     uVar2 = *(u64 *)(*symbol_table_ptr2 + op1 * 8);
//     callee = (i64 *)stackFrame[op1];
//     if (callee == (i64 *)0x0) {
//       FUN_1414ba4a0();
//       stackFrame[destination] = 0;
//       symbol_table_ptr = local_2b8;
//     }
//     else {
//       FUN_14124b3c0(&DAT_1484adfa0);
//       pauVar6 = DAT_1484adf88;
//       if (DAT_1484adf88 == (std::byte (*) [16])0x0) {
//         FUN_14124b540(&DAT_1484adfa0);
//         lVar9 = FUN_14126cc80(0x13);
//         callee = (i64 *)FUN_1412408a0(1);
//         lVar7 = FUN_14126cc80(0x13);
//         FUN_141443cc0(lVar9,*(i64 **)(lVar7 + 0x1a0),callee,0,'\0');
//         FUN_141b854f0();
//         lVar9 = FUN_141d91f80();
//         FUN_141d92c20(lVar9);
//         goto LAB_1414b3ac7;
//       }
//       DAT_1484adf88 = *(std::byte (**) [16])*DAT_1484adf88;
//       FUN_14286ad10(pauVar6,0,0x448);
//       DAT_1484adf94 = DAT_1484adf94 + 1;
//       if (DAT_1484adf98 < DAT_1484adf94) {
//         DAT_1484adf98 = DAT_1484adf94;
//       }
//       FUN_14124b540(&DAT_1484adfa0);
//       *(std::byte (**) [16])pauVar6[1] = param_1;
//       if (operand2 != 0) {
//         pauVar8 = pauVar6 + 0x1a;
//         do {
//           *(u64 *)*pauVar8 =
//                *(u64 *)
//                 ((i64)stackFrame + (-0x18 - (i64)pauVar6) + (i64)pauVar8);
//           pauVar8 = (std::byte (*) [16])(*pauVar8 + 8);
//           op2 = op2 - 1;
//         } while (op2 != 0);
//       }
//       *(u32 *)(pauVar6[0x44] + 4) = (u32)destination_;
//       *(i64 **)(*pauVar6 + 8) = callee;
//       *(u32 *)(*pauVar6 + 4) = (u32)operand2;
//       *(undefined4 *)*pauVar6 = 0;
//       *(u64 *)(pauVar6[0x42] + 8) = uVar2;
//       *(u64 *)pauVar6[0x43] = 0;
//       local_2b8 = callee[1];
//       local_2c8 = *callee;
//       stackFrame = (i64 *)(pauVar6[1] + 8);
//       symbol_table_ptr2 = pauVar6 + 0x22;
//       FUN_141234a50(6,0xc9e40b98fed629eb,pauVar6);
//       lVar9 = DAT_1484ae1d0;
//       lVar7 = DAT_1484ae1e0;
//       param_1 = pauVar6;
//       symbol_table_ptr = local_2b8;
//     }
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::CallFf:
//     func_ptr = (undefined *)stackFrame[op1];
//     if (func_ptr == (undefined *)0x0) {
//       FUN_1414ba4a0();
//       stackFrame[destination] = 0;
//       goto switchD_1414b2f81_caseD_5d;
//     }
//     lVar9 = 4;
//     pauVar6 = (std::byte (*) [16])&local_288;
//     do {
//       *(u64 *)*pauVar6 = 0;
//       *(u64 *)(*pauVar6 + 8) = 0;
//       *(u64 *)pauVar6[1] = 0;
//       *(u64 *)(pauVar6[1] + 8) = 0;
//       *(u64 *)pauVar6[2] = 0;
//       *(u64 *)(pauVar6[2] + 8) = 0;
//       *(u64 *)pauVar6[3] = 0;
//       *(u64 *)(pauVar6[3] + 8) = 0;
//       *(u64 *)pauVar6[4] = 0;
//       *(u64 *)(pauVar6[4] + 8) = 0;
//       *(u64 *)pauVar6[5] = 0;
//       *(u64 *)(pauVar6[5] + 8) = 0;
//       pauVar6[6] = (std::byte  [16])0x0;
//       pauVar8 = pauVar6 + 8;
//       pauVar6[7] = (std::byte  [16])0x0;
//       lVar9 = lVar9 + -1;
//       pauVar6 = pauVar8;
//     } while (lVar9 != 0);
//     *pauVar8 = (std::byte  [16])0x0;
//     callee = stackFrame + 0x31;
//     lVar9 = 0x10;
//     plVar10 = alStack_280;
//     do {
//       *plVar10 = *callee;
//       plVar10[1] = callee[1];
//       plVar10[2] = callee[2];
//       plVar1 = plVar10 + 4;
//       plVar10[3] = callee[3];
//       callee = callee + 4;
//       lVar9 = lVar9 + -1;
//       plVar10 = plVar1;
//     } while (lVar9 != 0);
//     *plVar1 = *callee;
//     auVar13 = ZEXT1664(auVar13._0_16_);
//     auVar14 = ZEXT1664(auVar14._0_16_);
//     auVar15 = ZEXT1664(auVar15._0_16_);
//     lVar9 = measure_perf();
//     (*(code *)**(u64 **)func_ptr)(func_ptr,alStack_280,op2,stackFrame + destination);
//     lVar7 = measure_perf();
//     save_perf(lVar7 - lVar9);
//     if (((param_3 == (int *)0x0) || (*param_3 == 0)) && (local_288 != 2)) goto code_r0x0001414b3892;
//     do {
//       pauVar6 = *(std::byte (**) [16])param_1[1];
//       lVar9 = *(i64 *)(param_1[0x43] + 8);
//       if (lVar9 != 0) {
//         FUN_141241f70(&DAT_1484ade60,&DAT_1484ade60);
//         FUN_141254110((i64 *)&DAT_1484ade00,lVar9);
//         FUN_141242040(&DAT_1484ade60);
//       }
//       FUN_14124b3c0(&DAT_1484adfa0);
//       DAT_1484adf94 = DAT_1484adf94 + -1;
//       *(undefined4 *)(param_1[0x44] + 4) = 0xf8eef8ee;
//       *(u64 *)(*param_1 + 8) = 0;
//       *(std::byte (**) [16])*param_1 = DAT_1484adf88;
//       DAT_1484adf88 = param_1;
//       FUN_14124b540(&DAT_1484adfa0);
//       param_1 = pauVar6;
//     } while (pauVar6 != (std::byte (*) [16])0x0);
// LAB_1414b3ac7:
//     *param_2 = 0;
//     break;
//   case Opcode::IEqual:
//     lVar4 = lVar7;
//     if (stackFrame[op1] == stackFrame[op2]) {
//       lVar4 = lVar9;
//     }
//     stackFrame[destination] = lVar4;
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::IGreaterThan:
//     lVar4 = lVar7;
//     if (stackFrame[op2] < stackFrame[op1]) {
//       lVar4 = lVar9;
//     }
//     stackFrame[destination] = lVar4;
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::IGreaterThanEqual:
//     lVar4 = lVar7;
//     if (stackFrame[op2] <= stackFrame[op1]) {
//       lVar4 = lVar9;
//     }
//     stackFrame[destination] = lVar4;
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::ILessThan:
//     lVar4 = lVar7;
//     if (stackFrame[op1] < stackFrame[op2]) {
//       lVar4 = lVar9;
//     }
//     stackFrame[destination] = lVar4;
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::ILessThanEqual:
//     lVar4 = lVar7;
//     if (stackFrame[op1] <= stackFrame[op2]) {
//       lVar4 = lVar9;
//     }
//     stackFrame[destination] = lVar4;
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::FEqual:
//     if (*(float *)(stackFrame + op1) != *(float *)(stackFrame + op2)) {
// LAB_1414b316e:
//       lVar4 = lVar7;
//       goto LAB_1414b3171;
//     }
//     stackFrame[destination] = lVar9;
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::FGreaterThan:
//     lVar4 = lVar7;
//     if (*(float *)(stackFrame + op2) < *(float *)(stackFrame + op1)) {
//       lVar4 = lVar9;
//     }
//     stackFrame[destination] = lVar4;
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::FGreaterThanEqual:
//     lVar4 = lVar7;
//     if (*(float *)(stackFrame + op2) <= *(float *)(stackFrame + op1)) {
//       lVar4 = lVar9;
//     }
//     stackFrame[destination] = lVar4;
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::FLessThan:
//     lVar4 = lVar7;
//     if (*(float *)(stackFrame + op1) < *(float *)(stackFrame + op2)) {
//       lVar4 = lVar9;
//     }
//     stackFrame[destination] = lVar4;
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::FLessThanEqual:
//     lVar4 = lVar7;
//     if (*(float *)(stackFrame + op1) <= *(float *)(stackFrame + op2)) {
//       lVar4 = lVar9;
//     }
//     stackFrame[destination] = lVar4;
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::IMod:
//     stackFrame[destination] = stackFrame[op1] % stackFrame[op2];
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::FMod:
//     fVar12 = fmod(*(float *)(stackFrame + op1),*(float *)(stackFrame + op2));
//     *(float *)(stackFrame + destination) = fVar12;
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::IAbs:
//     op2 = stackFrame[op1] >> 0x3f;
//     stackFrame[destination] = (stackFrame[op1] ^ op2) - op2;
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::FAbs:
//     auVar3 = vandps_avx(ZEXT416(*(u32 *)(stackFrame + op1)),auVar13._0_16_);
//     *(int *)(stackFrame + destination) = auVar3._0_4_;
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::GoTo:
//   case Opcode::Label:
//     goto switchD_1414b2f81_caseD_2b;
//   case Opcode::Branch:
//     *(u32 *)*param_1 = (u32)CONCAT11(operand2,destination_);
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::BranchIf:
//     if (stackFrame[op1] != 0) {
//       *(u32 *)*param_1 = (u32)CONCAT11(operand2,destination_);
//     }
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::BranchIfNot:
//     if (stackFrame[op1] == 0) {
//       *(u32 *)*param_1 = (u32)CONCAT11(operand2,destination_);
//     }
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::OpLogNot:
//     lVar4 = lVar9;
//     if (stackFrame[op1] != 0) {
//       lVar4 = lVar7;
//     }
//     stackFrame[destination] = lVar4;
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::OpBitAnd:
//     stackFrame[destination] = stackFrame[op2] & stackFrame[op1];
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::OpBitNot:
//     stackFrame[destination] = ~stackFrame[op1];
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::OpBitOr:
//     stackFrame[destination] = stackFrame[op2] | stackFrame[op1];
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::OpBitXor:
//     stackFrame[destination] = stackFrame[op2] ^ stackFrame[op1];
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::OpBitNor:
//     stackFrame[destination] = ~(stackFrame[op2] | stackFrame[op1]);
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::OpLogAnd:
//     if ((stackFrame[op1] == 0) || (lVar4 = lVar9, stackFrame[op2] == 0)) goto LAB_1414b316e;
// LAB_1414b3171:
//     stackFrame[destination] = lVar4;
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::OpLogOr:
//     if ((stackFrame[op1] != 0) || (lVar4 = lVar7, stackFrame[op2] != 0)) goto LAB_1414b318b;
//     goto LAB_1414b318e;
//   case Opcode::INeg:
//     stackFrame[destination] = -stackFrame[op1];
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::FNeg:
//     *(f32 *)(stackFrame + destination) = *(f32 *)(stackFrame + op1) ^ auVar14._0_4_;
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::LoadParamCnt:
//     stackFrame[destination] = (i64)*(int *)(*param_1 + 4);
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::IAddImm:
//     stackFrame[destination] = stackFrame[op1] + op2;
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::ISubImm:
//     stackFrame[destination] = stackFrame[op1] - op2;
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::IMulImm:
//     stackFrame[destination] = op2 * stackFrame[op1];
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::IDivImm:
//     stackFrame[destination] = stackFrame[op1] / (i64)op2;
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::LoadStaticI32Imm:
//     stackFrame[destination] = (i64)*(int *)(symbol_table_ptr + op1 * 8);
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::LoadStaticFloatImm:
//     *(f32 *)(stackFrame + destination) = *(f32 *)(symbol_table_ptr + op1 * 8);
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::LoadStaticPointerImm:
//   case Opcode::LoadStaticI64Imm:
//   case Opcode::LoadStaticU64Imm:
//     stackFrame[destination] = *(i64 *)(symbol_table_ptr + op1 * 8);
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::IntAsh:
//     op2 = stackFrame[op2];
//     if ((i64)op2 < 0) {
//       stackFrame[destination] = stackFrame[op1] >> (-op2 & 0x3f);
//     }
//     else {
//       stackFrame[destination] = stackFrame[op1] << (op2 & 0x3f);
//     }
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::LoadStaticU32Imm:
//     stackFrame[destination] = (u64)*(u32 *)(symbol_table_ptr + op1 * 8);
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::LoadStaticI8Imm:
//     stackFrame[destination] = (i64)*(char *)(symbol_table_ptr + op1 * 8);
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::LoadStaticU8Imm:
//     stackFrame[destination] = (u64)*(byte *)(symbol_table_ptr + op1 * 8);
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::LoadStaticI16Imm:
//     stackFrame[destination] = (i64)*(short *)(symbol_table_ptr + op1 * 8);
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::LoadStaticU16Imm:
//     stackFrame[destination] = (u64)*(u16 *)(symbol_table_ptr + op1 * 8);
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::LoadI8:
//     goto switchD_1414b2f81_caseD_4b;
//   case Opcode::LoadU8:
//     goto switchD_1414b2f81_caseD_4c;
//   case Opcode::LoadI16:
//     goto switchD_1414b2f81_caseD_4d;
//   case Opcode::LoadU16:
//     goto switchD_1414b2f81_caseD_4e;
//   case Opcode::LoadI32:
//     goto switchD_1414b2f81_caseD_4f;
//   case Opcode::StoreI8:
//     *(char *)stackFrame[op1] = (char)stackFrame[op2];
// switchD_1414b2f81_caseD_4b:
//     stackFrame[destination] = (i64)*(char *)stackFrame[op1];
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::StoreU8:
//     *(char *)stackFrame[op1] = (char)stackFrame[op2];
// switchD_1414b2f81_caseD_4c:
//     stackFrame[destination] = (u64)*(byte *)stackFrame[op1];
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::StoreI16:
//     *(short *)stackFrame[op1] = (short)stackFrame[op2];
// switchD_1414b2f81_caseD_4d:
//     stackFrame[destination] = (i64)*(short *)stackFrame[op1];
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::StoreU16:
//     *(u16 *)stackFrame[op1] = (u16)stackFrame[op2];
// switchD_1414b2f81_caseD_4e:
//     stackFrame[destination] = (u64)*(u16 *)stackFrame[op1];
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::StoreI32:
//     *(int *)stackFrame[op1] = (int)stackFrame[op2];
// switchD_1414b2f81_caseD_4f:
//     stackFrame[destination] = (i64)*(int *)stackFrame[op1];
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::StoreU32:
//     *(u32 *)stackFrame[op1] = (u32)stackFrame[op2];
//   case Opcode::LoadU32:
//     stackFrame[destination] = (u64)*(u32 *)stackFrame[op1];
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::INotEqual:
//     lVar4 = lVar7;
//     if (stackFrame[op1] != stackFrame[op2]) {
//       lVar4 = lVar9;
//     }
//     stackFrame[destination] = lVar4;
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::FNotEqual:
//     if (*(float *)(stackFrame + op1) == *(float *)(stackFrame + op2)) {
//       stackFrame[destination] = lVar7;
//     }
//     else {
// LAB_1414b318b:
//       lVar4 = lVar9;
// LAB_1414b318e:
//       stackFrame[destination] = lVar4;
//     }
//     goto switchD_1414b2f81_caseD_5d;
//   case Opcode::StoreArray:
//     // *(std::byte (*) [16])stackFrame[op1] = *(std::byte (*) [16])stackFrame[op2];
//     // stackFrame[destination] = stackFrame[op2];
//   case Opcode::AssertPointer:
//     goto switchD_1414b2f81_caseD_5d;
//   }
// LAB_1414b3ad6:
//   FUN_14160f5d0(local_298);
//   FUN_142865e80(local_78 ^ (u64)auStack_308);
//   return;
// code_r0x0001414b3892:
//   lVar9 = DAT_1484ae1d0;
//   lVar7 = DAT_1484ae1e0;
//   symbol_table_ptr = local_2b8;
//   if (local_288 == 1) goto LAB_1414b39e3;
//   goto switchD_1414b2f81_caseD_5d;
// LAB_1414b39e3:
//   *param_2 = 0;
//   goto LAB_1414b3ad6;
// }
}



namespace dconstruct::hooking {
    using QWORD = DWORD64;

    using target_fn_t = QWORD*(__fastcall*)(QWORD* a1, QWORD* a2, i64* a3);

    target_fn_t original_fn = nullptr;

    QWORD* __fastcall hook(QWORD* start_ptr, QWORD* a2, i64* a3) {
        std::ofstream out("hook.log", std::ios::out | std::ios::app);
        
        const auto test = *start_ptr;
        
        if (p64(test) < 0x100) {
            std::println(out, "dead");
            return original_fn(start_ptr, a2, a3);
        }
        std::println(out, "*start_ptr: {:x}" , p64(test));

        // u64** v9 = (u64 **)(test + 8);

        // std::println(out, "v9: {:x}" , p64(v9));

        const u64 op2 = (u64)*(u32*)*start_ptr;

        //out <<  "here2" << "\n";

        //const Opcode opcode = *(Opcode *)(local_2c8 + op2 * 8);

        //const std::string log_msg = std::format("{}\n",(u8)opcode);
        //out << log_msg << "\n";
        return original_fn(start_ptr, a2, a3);
    }

    static constexpr p64 IMAGE_BASE               = 0x140000000;
    static constexpr p64 FUNCTION_RELATIVE_OFFSET = 0x1414B6940;

    static constexpr p64 FUNCTION_OFFSET          = FUNCTION_RELATIVE_OFFSET - IMAGE_BASE; 

    DWORD WINAPI init_thread(void*) {
        MH_Initialize();
        const p64 base = (p64)GetModuleHandle(nullptr);
        void* target = (void*)(base + FUNCTION_OFFSET);

        MH_CreateHook(target, &hook, reinterpret_cast<void**>(&original_fn));
        
        MH_EnableHook(target);
        return 0;
    }
}   

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, dconstruct::hooking::init_thread, nullptr, 0, nullptr);
    }
    return TRUE;
}
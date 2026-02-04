#include "disassembly/instructions.h"
#include <Windows.h>
#include <immintrin.h>

namespace dconstruct {
void __fastcall parse_instruction(unsigned int *a1, u64 *a2, i64 *a3)
{
  unsigned int *instruction_idx_ptr; // r14
  i64 v6; // rbx
  unsigned int v7; // eax
  bool garbage_flag; // cf
  i64 v9; // rax
  u64 *symbol_table_ptr; // rbx
  Instruction *instruction_ptr; // rcx
  unsigned int *v13; // r8
  i64 v17; // r12
  i64 v18; // r13
  u64 v19; // r9
  i64 current_instruction_idx; // rax
  Opcode opcode; // rdx
  bool garbage_flag2; // zf
  unsigned int next_instruction_index; // eax
  i64 v27; // rax
  u64 v28; // rax
  i64 v29; // rcx
  u64 v30; // rax
  u64 v31[2]; // rax
  i64 v32; // rax
  i64 v33; // rax
  i64 v41; // rax
  i64 v42; // rax
  i64 v43; // rax
  i64 v44; // rcx
  u64 v45; // rax
  i64 v46; // rcx
  u64 v47; // rax
  i64 v48; // rcx
  u64 v49; // rax
  i64 v50; // rcx
  u64 v51; // rax
  i64 v52; // rcx
  u64 v53; // rax
  i64 v54; // rcx
  u64 v55; // rax
  i64 v56; // rax
  i64 v57; // rax
  i64 v58; // rax
  i64 v59; // rax
  u64*v65; // rax
  i64 v66; // rbx
  u32*v67; // rsi
  int v68; // ecx
  int v69; // eax
  u64*v70; // rcx
  char *v71; // rdi
  i64 v72; // rdx
  void (__fastcall ***v73)(u64, std::byte *, u64, u64*, u64); // rsi
  i64 v75; // rcx
  u64*v76; // rdx
  u64*v77; // rcx
  i64 v78; // r8
  i64 v79; // rbx
  i64 v80; // rdx
  i64 v81; // rcx
  i64 v82; // r8
  i64 v83; // r9
  i64 v84; // rax
  i64 v85; // rax
  i64 v86; // rax
  i64 v87; // rdi
  i64 v88; // rbx
  i64 v89; // rax
  i64 v90; // rax
  unsigned int *v91; // rbx
  i64 v92; // rdi
  i64 v93; // r8
  int v98; // [rsp+20h] [rbp-2E8h]
  Instruction *v99; // [rsp+40h] [rbp-2C8h]
  unsigned int *v100; // [rsp+48h] [rbp-2C0h]
  u64 *v101; // [rsp+50h] [rbp-2B8h]
  int v104; // [rsp+68h] [rbp-2A0h]
  i64 v105; // [rsp+68h] [rbp-2A0h]
  u64 v106[2]; // [rsp+70h] [rbp-298h] BYREF
  i64 v107; // [rsp+80h] [rbp-288h] BYREF
  std::byte v108[520]; // [rsp+88h] [rbp-280h] BYREF
  char v109; // [rsp+2D0h] [rbp-38h] BYREF

  instruction_idx_ptr = a1;
  v106[0] = 0;

  reinterpret_cast<u64*(__fastcall*)(u64*, i64)>(0x1416130E0)(v106, (i64)a1);

  constexpr u32* TlsIndex_loc = reinterpret_cast<u32*>(0x14964A9B8);

  u32 TlsIndex = *TlsIndex_loc;

  v6 = (unsigned int)TlsIndex;

  i32* dword_1484B6438 = reinterpret_cast<i32*>(0x1484B6438);

  const void* value = TlsGetValue(TlsIndex);
  u32 teb_offset_value = *reinterpret_cast<const u32*>(reinterpret_cast<const u8*>(value) + 0xF8);

  const auto Init_thread_header = reinterpret_cast<void(*__fastcall)(i32*)>(0x14286C3C8);
  const auto Init_thread_footer = reinterpret_cast<void(*__fastcall)(i32*)>(0x14286C368);

  u64* qword_1484B6430 = reinterpret_cast<u64*>(0x1484B6430);
  i32* dword_1484B6438 = reinterpret_cast<i32*>(0x1484B6438);
  u64* qword_1484B6440 = reinterpret_cast<u64*>(0x1484B6440);
  i32* dword_1484B6448 = reinterpret_cast<i32*>(0x1484B6448);
  u64* qword_1484B6450 = reinterpret_cast<u64*>(0x1484B6450);
  i32* dword_1484B6458 = reinterpret_cast<i32*>(0x1484B6458);

  if ( *dword_1484B6438 > teb_offset_value)
  {
    Init_thread_header(dword_1484B6438);
    if ( *dword_1484B6438 == -1 )
    {
      *qword_1484B6430 = 1;
      Init_thread_footer(dword_1484B6438);
    }
  }
  if ( *dword_1484B6448 > teb_offset_value )
  {
    Init_thread_header(dword_1484B6448);
    if ( *dword_1484B6448 == -1 )
    {
      *qword_1484B6440 = 0;
      Init_thread_footer(dword_1484B6448);
    }
  }
  if ( *dword_1484B6458 > teb_offset_value )
  {
    Init_thread_header(dword_1484B6458);
    if ( *dword_1484B6458 == -1 )
    {
      qword_1484B6450 = qword_1484B6430;
      Init_thread_footer(dword_1484B6458);
    }
  }

  i64* qword_1484B6460 = reinterpret_cast<i64*>(0x1484B6460);
  i32* dword_1484B6468 = reinterpret_cast<i32*>(0x1484B6468);

  v7 = teb_offset_value;
  garbage_flag = *dword_1484B6468 < v7;
  if ( *dword_1484B6468 > (int)v7 )
  {
    Init_thread_header(dword_1484B6468);
    garbage_flag = *dword_1484B6468 != -1;
    if ( *dword_1484B6468 == -1 )
    {
      *qword_1484B6460 = *qword_1484B6440;
      Init_thread_footer(dword_1484B6468);
    }
  }
  v9 = *((u64*)instruction_idx_ptr + 1);
  symbol_table_ptr = *(u64 **)(v9 + 8);
  v101 = symbol_table_ptr;
  instruction_ptr = *(Instruction **)v9;
  v99 = *(Instruction **)v9;
  u32* _RDI = instruction_idx_ptr + 6;
  v13 = instruction_idx_ptr + 136;
  v100 = instruction_idx_ptr + 136;

  // f32* xmm7_ptr = reinterpret_cast<f32*>(0x142AF9720);
  // f32* xmm6_ptr = reinterpret_cast<f32*>(0x142B039A0);
  // f32* xmm8_ptr = reinterpret_cast<f32*>(0x142AF88E0);

  // __m128 xmm7 = _mm_load_ss(xmm7_ptr);
  // __m128 xmm6 = _mm_load_ss(xmm6_ptr);
  // __m128 xmm8 = _mm_load_ss(xmm8_ptr);

  u8 _RBP;
  u8 _RSI;
  u8 _R15;
  
  // __asm
  // {
  //   vmovss  xmm7, dword ptr cs:xmmword_142AF9720
  //   vmovss  xmm6, dword ptr cs:xmmword_142B039A0
  //   vmovss  xmm8, cs:flt_142AF88E0
  // }


  //auto error1 = reinterpret_cast<void(__fastcall*)(const char*)>(0x1414BE090);
  auto error2 = reinterpret_cast<void(*)(const char*, ...)>(0x141B8E720);

  const char* script_set_float_error = reinterpret_cast<const char*>(0x142E30700);
  const char* question_marks = reinterpret_cast<const char*>(0x142AFB490);

  auto lookup = reinterpret_cast<u64*(__fastcall*)(sid64)>(0x141356D60);

LABEL_14:
  v17 = *qword_1484B6450;
  v18 = *qword_1484B6460;
LABEL_15:
  v19 = 0x140000000uLL;
  while ( 2 )
  {
    current_instruction_idx = *instruction_idx_ptr;
    opcode = instruction_ptr[current_instruction_idx].opcode;
    _RBP = instruction_ptr[current_instruction_idx].destination;
    _RSI = instruction_ptr[current_instruction_idx].operand1;
    _R15 = instruction_ptr[current_instruction_idx].operand2;
    next_instruction_index = current_instruction_idx + 1;
    garbage_flag2 = next_instruction_index == 0;
    *instruction_idx_ptr = next_instruction_index;
    switch ( opcode )
    {
      case Opcode::Return:
        goto LABEL_158;
      case Opcode::IAdd:
        v27 = _RDI[_R15];
        garbage_flag = _RDI[_RSI] + v27;
        _RDI[_RBP] = _RDI[_RSI] + v27;
        continue;
      case Opcode::ISub:
        v28 = _RDI[_RSI];
        garbage_flag = v28 < _RDI[_R15];
        _RDI[_RBP] = v28 - _RDI[_R15];
        continue;
      case Opcode::IMul:
        _RDI[_RBP] = _RDI[_RSI] * _RDI[_R15];
        continue;
      case Opcode::IDiv:
        _RDI[_RBP] = _RDI[_RSI] / (i64)_RDI[_R15];
        continue;
      case Opcode::FAdd:
        *(f32*)(_RDI + _RBP) = *(f32*)(_RDI + _RSI) + *(f32*)(_RDI + _R15);
        continue;
      case Opcode::FSub:
        *(f32*)(_RDI + _RBP) = *(f32*)(_RDI + _RSI) - *(f32*)(_RDI + _R15);
        continue;
      case Opcode::FMul:
        *(f32*)(_RDI + _RBP) = *(f32*)(_RDI + _RSI) * *(f32*)(_RDI + _R15);
        continue;
      case Opcode::FDiv:
        *(f32*)(_RDI + _RBP) = *(f32*)(_RDI + _RSI) / *(f32*)(_RDI + _R15);
        continue;
      case Opcode::LoadStaticInt:
      case Opcode::LoadStaticPointer:
        _RDI[_RBP] = symbol_table_ptr[_RDI[_RSI]];
        instruction_ptr = v99;
        continue;
      case Opcode::LoadStaticFloat:
        *(f32*)(_RDI + _RBP) = symbol_table_ptr[_RDI[_RSI]];
        instruction_ptr = v99;
        continue;
      case Opcode::LoadU16Imm:
        _RDI[_RBP] = _RSI | (_R15 << 8);
        continue;
      case Opcode::LoadU32:
        goto LABEL_125;
      case Opcode::LoadFloat:
        *(f32*)(_RDI + _RBP) = *(f32*)_RDI[_RSI];
        instruction_ptr = v99;
        continue;
      case Opcode::LoadPointer:
      case Opcode::LoadI64:
      case Opcode::LoadU64:
        goto LABEL_110;
      case Opcode::StoreInt:
      case Opcode::StoreI64:
      case Opcode::StoreU64:
        *(u64*)_RDI[_RSI] = _RDI[_R15];
LABEL_110:
        _RDI[_RBP] = *(u64*)_RDI[_RSI];
        instruction_ptr = v99;
        continue;
      case Opcode::StoreFloat:
        v104 = *(f32*)(_RDI + _RBP);
        if ( (v104 & 0x7F800000) == 0x7F800000 )
        {
          error2(script_set_float_error, question_marks);
          v17 = *qword_1484B6450;
          v18 = *qword_1484B6460;
          instruction_ptr = v99;
          v13 = v100;
          v19 = 0x140000000uLL;
        }
        *(f32*)(_RDI + _RSI) = v104; 
        *(f32*)(_RDI + _R15) = v104; 
        continue;
      case Opcode::StorePointer:
        *(u64*)_RDI[_RSI] = _RDI[_R15];
        _RDI[_RBP] = _RDI[_R15];
        instruction_ptr = v99;
        continue;
      case Opcode::LookupInt:
      case Opcode::LookupFloat:
      case Opcode::LookupPointer:
        v65 = lookup(symbol_table_ptr[_RSI]);
        v13 = v100;
        *(u64*)&v100[2 * _RBP] = symbol_table_ptr[_RSI];
        garbage_flag = 0;
        if ( v65 )
          _RDI[_RBP] = *v65;
        else
          _RDI[_RBP] = 0;
        instruction_ptr = v99;
        goto LABEL_14;
      case Opcode::MoveInt:
        _RDI[_RBP] = _RDI[_RSI];
        continue;
      case Opcode::MoveFloat:
        LODWORD(_RDI[_RBP]) = _RDI[_RSI];
        continue;
      case Opcode::MovePointer:
      case Opcode::Move:
        _RDI[_RBP] = _RDI[_RSI];
        *(u64*)&v13[2 * _RBP] = *(u64*)&v13[2 * _RSI];
        continue;
      case Opcode::CastInteger:
        __asm { vcvttss2si rax, dword ptr [rdi+rbp*8]; jumptable 00000001414B6B41 case Opcode::25 }
        _RDI[_RBP] = _RAX;
        continue;
      case Opcode::CastFloat:
        __asm
        {
          vxorps  xmm0, xmm0, xmm0; jumptable 00000001414B6B41 case Opcode::26
          vcvtsi2ss xmm0, xmm0, qword ptr [rdi+rbp*8]
          vmovss  dword ptr [rdi+rbp*8], xmm0
        }
        continue;
      case Opcode::Call:
        v66 = *(u64*)&v13[2 * _RSI];
        v105 = _RDI[_RSI];
        if ( !v105 )
        {
          error(
            "Unable to call defun '%s' (check TTY for load symbol failed) - is it defined in a .dc file that is (include)"
            "d?  Use (load-relative) instead.\n");
          _RDI[_RBP] = 0;
          symbol_table_ptr = v101;
          v13 = v100;
          instruction_ptr = v99;
          goto LABEL_15;
        }
        sub_14124F1C0(
          &qword_1484B6220,
          "c:\\branches\\discbot03\\t2r-pc-1.6.0\\shared\\src\\ndlib\\script\\script-manager.cpp",
          850,
          "struct ScriptStackFrame *__cdecl ScriptStackFramePool::Alloc(void)");
        v67 = qword_1484B6208;
        if ( !qword_1484B6208 )
        {
          sub_14124F340(
            &qword_1484B6220,
            "c:\\branches\\discbot03\\t2r-pc-1.6.0\\shared\\src\\ndlib\\script\\script-manager.cpp",
            850,
            "struct ScriptStackFrame *__cdecl ScriptStackFramePool::Alloc(void)");
          v87 = sub_141270A80(19);
          v88 = sub_141244580(1u);
          v89 = sub_141270A80(19);
          LOBYTE(v98) = 0;
          sub_1414477C0(v87, *(u64*)(v89 + 416), v88, 0, v98, 1, 1);
          nullsub_599(&xmmword_14934C9C0);
          v90 = sub_141D98340(&unk_149450D20);
          sub_141D98FE0(v90);
          goto LABEL_166;
        }
        qword_1484B6208 = *(void **)qword_1484B6208;
        memset(v67, 0, 0x448u);
        v68 = qword_1484B6214 + 1;
        LODWORD(qword_1484B6214) = v68;
        v69 = HIDWORD(qword_1484B6214);
        if ( v68 > SHIDWORD(qword_1484B6214) )
          v69 = v68;
        HIDWORD(qword_1484B6214) = v69;
        sub_14124F340(
          &qword_1484B6220,
          "c:\\branches\\discbot03\\t2r-pc-1.6.0\\shared\\src\\ndlib\\script\\script-manager.cpp",
          850,
          "struct ScriptStackFrame *__cdecl ScriptStackFramePool::Alloc(void)");
        *((u64*)v67 + 2) = instruction_idx_ptr;
        if ( (_DWORD)_R15 )
        {
          v70 = v67 + 104;
          v71 = (char *)((char *)_RDI - (char *)v67);
          v72 = _R15;
          do
          {
            *v70 = *(u64*)((char *)v70 + (_QWORD)v71 - 24);
            ++v70;
            --v72;
          }
          while ( v72 );
        }
        v67[273] = _RBP;
        *((u64*)v67 + 1) = v105;
        v67[1] = _R15;
        *v67 = 0;
        *((u64*)v67 + 133) = v66;
        *((u64*)v67 + 134) = 0;
        instruction_idx_ptr = v67;
        symbol_table_ptr = *(u64 **)(v105 + 8);
        v101 = symbol_table_ptr;
        v99 = *(Instruction **)v105;
        _RDI = v67 + 6;
        v100 = v67 + 136;
        sub_141238750(6, 0xC9E40B98FED629EBuLL, v67);
        instruction_ptr = v99;
        v13 = v67 + 136;
        goto LABEL_14;
      case Opcode::CallFf:
        v73 = (void (__fastcall ***)(_QWORD, _BYTE *, _QWORD, u64*, _QWORD))_RDI[_RSI];
        if ( !v73 )
        {
          error("Unable to call func '%s'\n");
          _RDI[_RBP] = 0;
          instruction_ptr = v99;
          v13 = v100;
          goto LABEL_15;
        }
        _RAX = &v107;
        __asm { vpxor   xmm0, xmm0, xmm0 }
        v75 = 4;
        do
        {
          __asm
          {
            vmovups ymmword ptr [rax], ymm0
            vmovups ymmword ptr [rax+20h], ymm0
            vmovups ymmword ptr [rax+40h], ymm0
            vmovups xmmword ptr [rax+60h], xmm0
          }
          _RAX += 16;
          __asm { vmovups xmmword ptr [rax-10h], xmm0 }
          --v75;
        }
        while ( v75 );
        __asm { vmovups xmmword ptr [rax], xmm0 }
        v76 = _RDI + 49;
        v77 = v108;
        v78 = 16;
        do
        {
          *v77 = *v76;
          v77[1] = v76[1];
          v77[2] = v76[2];
          v77 += 4;
          *(v77 - 1) = v76[3];
          v76 += 4;
          --v78;
        }
        while ( v78 );
        *v77 = *v76;
        __asm { vzeroupper }
        v79 = sub_141245F70(v77, v76, 0, 0x140000000uLL);
        (**v73)(v73, v108, (unsigned int)_R15, &_RDI[_RBP], v73);
        v84 = sub_141245F70(v81, v80, v82, v83);
        sub_14029F6E0(v84 - v79);
        if ( (!a3 || !*(u32*)a3) && v107 != 2 )
        {
          garbage_flag = v107 == 0;
          if ( v107 == 1 )
          {
            *a2 = 0;
            goto LABEL_167;
          }
          symbol_table_ptr = v101;
          v13 = v100;
LABEL_160:
          v18 = qword_1484B6460;
          v17 = qword_1484B6450;
          instruction_ptr = v99;
          goto LABEL_15;
        }
        do
        {
          v91 = instruction_idx_ptr;
          instruction_idx_ptr = (unsigned int *)*((u64*)instruction_idx_ptr + 2);
          v92 = *((u64*)v91 + 135);
          if ( v92 )
          {
            sub_141245C50(
              &qword_1484B60E0,
              &qword_1484B60E0,
              "c:\\branches\\discbot03\\t2r-pc-1.6.0\\shared\\src\\ndlib\\script\\script-manager.cpp",
              998,
              "void __cdecl ScriptScopedAllocationPool::FreeBlock(unsigned char *)");
            LOBYTE(v93) = 1;
            sub_141257F10(&unk_1484B6080, v92, v93);
            sub_141245D20(&qword_1484B60E0);
          }
          sub_14124F1C0(
            &qword_1484B6220,
            "c:\\branches\\discbot03\\t2r-pc-1.6.0\\shared\\src\\ndlib\\script\\script-manager.cpp",
            896,
            "void __cdecl ScriptStackFramePool::Free(struct ScriptStackFrame *)");
          LODWORD(qword_1484B6214) = qword_1484B6214 - 1;
          v91[273] = -118556434;
          *((u64*)v91 + 1) = 0;
          *(u64*)v91 = qword_1484B6208;
          qword_1484B6208 = v91;
          sub_14124F340(
            &qword_1484B6220,
            "c:\\branches\\discbot03\\t2r-pc-1.6.0\\shared\\src\\ndlib\\script\\script-manager.cpp",
            896,
            "void __cdecl ScriptStackFramePool::Free(struct ScriptStackFrame *)");
        }
        while ( instruction_idx_ptr );
LABEL_166:
        *a2 = 0;
LABEL_167:
        sub_141613190(v106);
        _R11 = &v109;
        __asm
        {
          vmovaps xmm6, xmmword ptr [r11-10h]
          vmovaps xmm7, xmmword ptr [r11-20h]
          vmovaps xmm8, xmmword ptr [r11-30h]
        }
        return;
      case Opcode::IEqual:
        v44 = v18;
        v45 = _RDI[_R15];
        garbage_flag = _RDI[_RSI] < v45;
        if ( _RDI[_RSI] == v45 )
          v44 = v17;
        _RDI[_RBP] = v44;
        instruction_ptr = v99;
        continue;
      case Opcode::IGreaterThan:
        v48 = v18;
        v49 = _RDI[_R15];
        garbage_flag = _RDI[_RSI] < v49;
        if ( _RDI[_RSI] > (signed i64)v49 )
          v48 = v17;
        _RDI[_RBP] = v48;
        instruction_ptr = v99;
        continue;
      case Opcode::IGreaterThanEqual:
        v50 = v18;
        v51 = _RDI[_R15];
        garbage_flag = _RDI[_RSI] < v51;
        if ( _RDI[_RSI] >= (signed i64)v51 )
          v50 = v17;
        _RDI[_RBP] = v50;
        instruction_ptr = v99;
        continue;
      case Opcode::ILessThan:
        v52 = v18;
        v53 = _RDI[_R15];
        garbage_flag = _RDI[_RSI] < v53;
        if ( _RDI[_RSI] < (signed i64)v53 )
          v52 = v17;
        _RDI[_RBP] = v52;
        instruction_ptr = v99;
        continue;
      case Opcode::ILessThanEqual:
        v54 = v18;
        v55 = _RDI[_R15];
        garbage_flag = _RDI[_RSI] < v55;
        if ( _RDI[_RSI] <= (signed i64)v55 )
          v54 = v17;
        _RDI[_RBP] = v54;
        instruction_ptr = v99;
        continue;
      case Opcode::FEqual:
        __asm
        {
          vmovss  xmm0, dword ptr [rdi+rsi*8]; jumptable 00000001414B6B41 case Opcode::34
          vucomiss xmm0, dword ptr [rdi+r15*8]
        }
        if ( __SETP__(next_instruction_index, 0) || next_instruction_index )
          goto LABEL_49;
        _RDI[_RBP] = v17;
        continue;
      case Opcode::FGreaterThan:
        v56 = v18;
        __asm
        {
          vmovss  xmm0, dword ptr [rdi+rsi*8]
          vcomiss xmm0, dword ptr [rdi+r15*8]
        }
        if ( !garbage_flag && !garbage_flag2 )
          v56 = v17;
        _RDI[_RBP] = v56;
        continue;
      case Opcode::FGreaterThanEqual:
        v57 = v18;
        __asm
        {
          vmovss  xmm0, dword ptr [rdi+rsi*8]
          vcomiss xmm0, dword ptr [rdi+r15*8]
        }
        if ( !garbage_flag )
          v57 = v17;
        _RDI[_RBP] = v57;
        continue;
      case Opcode::FLessThan:
        v58 = v18;
        __asm
        {
          vmovss  xmm0, dword ptr [rdi+r15*8]
          vcomiss xmm0, dword ptr [rdi+rsi*8]
        }
        if ( !garbage_flag && !garbage_flag2 )
          v58 = v17;
        _RDI[_RBP] = v58;
        continue;
      case Opcode::FLessThanEqual:
        v59 = v18;
        __asm
        {
          vmovss  xmm0, dword ptr [rdi+r15*8]
          vcomiss xmm0, dword ptr [rdi+rsi*8]
        }
        if ( !garbage_flag )
          v59 = v17;
        _RDI[_RBP] = v59;
        continue;
      case Opcode::IMod:
        _RDI[_RBP] = (i64)_RDI[_RSI] % _RDI[_R15];
        continue;
      case Opcode::FMod:
        __asm
        {
          vmovss  xmm1, dword ptr [rdi+r15*8]; jumptable 00000001414B6B41 case Opcode::40
          vmovss  xmm0, dword ptr [rdi+rsi*8]; X
        }
        *(float *)&_XMM0 = fmodf(*(float *)&_XMM0, *(float *)&_XMM1);
        __asm { vmovss  dword ptr [rdi+rbp*8], xmm0 }
        instruction_ptr = v99;
        v13 = v100;
        goto LABEL_15;
      case Opcode::IAbs:
        v31 = (i64)_RDI[_RSI];
        *(u64*)&v31 = *((u64*)&v31 + 1) ^ v31;
        garbage_flag = (u64)v31 < *((u64*)&v31 + 1);
        _RDI[_RBP] = v31 - *((u64*)&v31 + 1);
        continue;
      case Opcode::FAbs:
        __asm
        {
          vmovss  xmm0, dword ptr [rdi+rsi*8]; jumptable 00000001414B6B41 case Opcode::42
          vandps  xmm0, xmm0, xmm6
          vmovss  dword ptr [rdi+rbp*8], xmm0
        }
        continue;
      case Opcode::Branch:
        garbage_flag = 0;
        *instruction_idx_ptr = _RBP | ((_DWORD)_R15 << 8);
        continue;
      case Opcode::BranchIf:
        garbage_flag = 0;
        if ( _RDI[_RSI] )
        {
          garbage_flag = 0;
          *instruction_idx_ptr = _RBP | ((_DWORD)_R15 << 8);
        }
        continue;
      case Opcode::BranchIfNot:
        garbage_flag = 0;
        if ( !_RDI[_RSI] )
        {
          garbage_flag = 0;
          *instruction_idx_ptr = _RBP | ((_DWORD)_R15 << 8);
        }
        continue;
      case Opcode::OpLogNot:
        v41 = v17;
        garbage_flag = 0;
        if ( _RDI[_RSI] )
          v41 = v18;
        _RDI[_RBP] = v41;
        continue;
      case Opcode::OpBitAnd:
        garbage_flag = 0;
        _RDI[_RBP] = _RDI[_RSI] & _RDI[_R15];
        continue;
      case Opcode::OpBitNot:
        _RDI[_RBP] = ~_RDI[_RSI];
        continue;
      case Opcode::OpBitOr:
        garbage_flag = 0;
        _RDI[_RBP] = _RDI[_RSI] | _RDI[_R15];
        continue;
      case Opcode::OpBitXor:
        garbage_flag = 0;
        _RDI[_RBP] = _RDI[_RSI] ^ _RDI[_R15];
        continue;
      case Opcode::OpBitNor:
        garbage_flag = 0;
        _RDI[_RBP] = ~(_RDI[_RSI] | _RDI[_R15]);
        continue;
      case Opcode::OpLogAnd:
        garbage_flag = 0;
        if ( !_RDI[_RSI] || (garbage_flag = 0, v42 = v17, !_RDI[_R15]) )
LABEL_49:
          v42 = v18;
        _RDI[_RBP] = v42;
        continue;
      case Opcode::OpLogOr:
        garbage_flag = 0;
        if ( _RDI[_RSI] )
          goto LABEL_53;
        garbage_flag = 0;
        v43 = v18;
        if ( _RDI[_R15] )
          goto LABEL_53;
        goto LABEL_54;
      case Opcode::INeg:
        garbage_flag = _RDI[_RSI] != 0;
        _RDI[_RBP] = -_RDI[_RSI];
        continue;
      case Opcode::FNeg:
        __asm
        {
          vmovss  xmm0, dword ptr [rdi+rsi*8]; jumptable 00000001414B6B41 case Opcode::57
          vxorps  xmm1, xmm0, xmm7
          vmovss  dword ptr [rdi+rbp*8], xmm1
        }
        continue;
      case Opcode::LoadParamCnt:
        _RDI[_RBP] = (int)instruction_idx_ptr[1];
        continue;
      case Opcode::IAddImm:
        v29 = _RDI[_RSI];
        garbage_flag = __CFADD__(_R15, v29);
        _RDI[_RBP] = _R15 + v29;
        instruction_ptr = v99;
        continue;
      case Opcode::ISubImm:
        v30 = _RDI[_RSI];
        garbage_flag = v30 < _R15;
        _RDI[_RBP] = v30 - _R15;
        continue;
      case Opcode::IMulImm:
        garbage_flag = (unsigned __int128)((i64)_RDI[_RSI] * (__int128)_R15) >> 64 != 0;
        _RDI[_RBP] = _RDI[_RSI] * _R15;
        continue;
      case Opcode::IDivImm:
        _RDI[_RBP] = _RDI[_RSI] / _R15;
        continue;
      case Opcode::LoadStaticI32Imm:
        _RDI[_RBP] = SLODWORD(symbol_table_ptr[_RSI]);
        continue;
      case Opcode::LoadStaticFloatImm:
        LODWORD(_RDI[_RBP]) = symbol_table_ptr[_RSI];
        continue;
      case Opcode::LoadStaticPointerImm:
      case Opcode::LoadStaticI64Imm:
      case Opcode::LoadStaticU64Imm:
        _RDI[_RBP] = symbol_table_ptr[_RSI];
        continue;
      case Opcode::IntAsh:
        v32 = _RDI[_R15];
        garbage_flag = 0;
        if ( v32 < 0 )
        {
          v33 = -v32;
          garbage_flag = v33 != 0;
          _RDI[_RBP] = (i64)_RDI[_RSI] >> v33;
        }
        else
        {
          _RDI[_RBP] = _RDI[_RSI] << v32;
        }
        instruction_ptr = v99;
        continue;
      case Opcode::LoadStaticU32Imm:
        _RDI[_RBP] = LODWORD(symbol_table_ptr[_RSI]);
        continue;
      case Opcode::LoadStaticI8Imm:
        _RDI[_RBP] = SLOBYTE(symbol_table_ptr[_RSI]);
        continue;
      case Opcode::LoadStaticU8Imm:
        _RDI[_RBP] = LOBYTE(symbol_table_ptr[_RSI]);
        continue;
      case Opcode::LoadStaticI16Imm:
        _RDI[_RBP] = SLOWORD(symbol_table_ptr[_RSI]);
        continue;
      case Opcode::LoadStaticU16Imm:
        _RDI[_RBP] = LOWORD(symbol_table_ptr[_RSI]);
        continue;
      case Opcode::LoadI8:
        goto LABEL_119;
      case Opcode::LoadU8:
        goto LABEL_117;
      case Opcode::LoadI16:
        goto LABEL_123;
      case Opcode::LoadU16:
        goto LABEL_121;
      case Opcode::LoadI32:
        goto LABEL_127;
      case Opcode::StoreI8:
        *(_BYTE *)_RDI[_RSI] = _RDI[_R15];
LABEL_119:
        _RDI[_RBP] = *(char *)_RDI[_RSI];
        instruction_ptr = v99;
        continue;
      case Opcode::StoreU8:
        *(_BYTE *)_RDI[_RSI] = _RDI[_R15];
LABEL_117:
        _RDI[_RBP] = *(unsigned __int8 *)_RDI[_RSI];
        instruction_ptr = v99;
        continue;
      case Opcode::StoreI16:
        *(_WORD *)_RDI[_RSI] = _RDI[_R15];
LABEL_123:
        _RDI[_RBP] = *(__int16 *)_RDI[_RSI];
        instruction_ptr = v99;
        continue;
      case Opcode::StoreU16:
        *(_WORD *)_RDI[_RSI] = _RDI[_R15];
LABEL_121:
        _RDI[_RBP] = *(unsigned __int16 *)_RDI[_RSI];
        instruction_ptr = v99;
        continue;
      case Opcode::StoreI32:
        *(u32*)_RDI[_RSI] = _RDI[_R15];
LABEL_127:
        _RDI[_RBP] = *(int *)_RDI[_RSI];
        instruction_ptr = v99;
        continue;
      case Opcode::StoreU32:
        *(u32*)_RDI[_RSI] = _RDI[_R15];
LABEL_125:
        _RDI[_RBP] = *(unsigned int *)_RDI[_RSI];
        instruction_ptr = v99;
        continue;
      case Opcode::INotEqual:
        v46 = v18;
        v47 = _RDI[_R15];
        garbage_flag = _RDI[_RSI] < v47;
        if ( _RDI[_RSI] != v47 )
          v46 = v17;
        _RDI[_RBP] = v46;
        instruction_ptr = v99;
        continue;
      case Opcode::FNotEqual:
        __asm
        {
          vmovss  xmm0, dword ptr [rdi+rsi*8]; jumptable 00000001414B6B41 case Opcode::91
          vucomiss xmm0, dword ptr [rdi+r15*8]
        }
        if ( __SETP__(next_instruction_index, 0) || next_instruction_index )
        {
LABEL_53:
          v43 = v17;
LABEL_54:
          _RDI[_RBP] = v43;
        }
        else
        {
          _RDI[_RBP] = v18;
        }
        continue;
      case Opcode::StoreArray:
        _RAX = _RDI[_R15];
        _RCX = _RDI[_RSI];
        __asm
        {
          vmovups xmm0, xmmword ptr [rax]
          vmovups xmmword ptr [rcx], xmm0
        }
        _RDI[_RBP] = _RDI[_R15];
        instruction_ptr = v99;
        continue;
      case Opcode::AssertPointer:
        continue;
      default:
        error("Eval: Unknown opcode '0x%x' encountered!\n");
LABEL_158:
        v85 = sub_1414BBE30(instruction_idx_ptr, a2, v13, v19);
        instruction_idx_ptr = (unsigned int *)v85;
        if ( !v85 )
          goto LABEL_167;
        v86 = *(u64*)(v85 + 8);
        symbol_table_ptr = *(u64 **)(v86 + 8);
        v101 = symbol_table_ptr;
        v99 = *(Instruction **)v86;
        _RDI = instruction_idx_ptr + 6;
        v100 = instruction_idx_ptr + 136;
        sub_141238750(6, 0xC9E40B98FED629EBuLL, instruction_idx_ptr);
        v13 = instruction_idx_ptr + 136;
        goto LABEL_160;
    }
  }
}
}
}
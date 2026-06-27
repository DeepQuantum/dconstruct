	.file	"wave-manager-funcs.bin"
	.text
	.globl	"is-final-build?"
	.type	"is-final-build?",@function
"is-final-build?":
	LoadU16Imm	r0, 1
	Return	r0
.Lfunc_end0:
	.size	"is-final-build?", .Lfunc_end0-"is-final-build?"

	.globl	"wave-manager-get-npc-alive-count-by-faction"
	.type	"wave-manager-get-npc-alive-count-by-faction",@function
"wave-manager-get-npc-alive-count-by-faction":
	Move	r1, r50
	LookupPointer	r0, 2
	Call	r0, r0, 1
	Move	r3, r0
	LookupPointer	r4, 1
	Move	r49, r3
	CallFf	r0, r4, 1
	Move	r2, r0
	CallFf	r0, r4, 1
	Move	r4, r0
	LookupPointer	r0, 0
	CallFf	r0, r0, 0
	LoadU16Imm	r0, 1
	ILessThan	r0, r4, r0
	BranchIf	.LBB1_6, r0
	LoadU16Imm	r5, 0
	LookupPointer	r7, 3
	LookupPointer	r8, 4
	LoadU16Imm	r9, 7
	LoadU16Imm	r10, 5
	LoadU16Imm	r11, 4
	Move	r6, r5
	Branch	.LBB1_2
.LBB1_4:
	LoadU64	r49, r0
.LBB1_5:
	Move	r50, r5
	Call	r0, r7, 2
	INotEqual	r0, r1, r0
	ISub	r2, r2, r0
	IAddImm	r6, r6, 1
	ILessThan	r0, r6, r4
	BranchIfNot	.LBB1_6, r0
.LBB1_2:
	Move	r49, r3
	Move	r50, r6
	CallFf	r0, r8, 2
	Move	r49, r5
	BranchIfNot	.LBB1_5, r0
	IAddImm	r12, r0, 12
	LoadU16	r12, r12
	INotEqual	r13, r12, r9
	BranchIfNot	.LBB1_4, r13
	IEqual	r13, r12, r10
	BranchIf	.LBB1_4, r13
	IEqual	r12, r12, r11
	Move	r49, r5
	BranchIf	.LBB1_4, r12
	Branch	.LBB1_5
.LBB1_6:
	LookupPointer	r0, 5
	CallFf	r0, r0, 0
	Move	r0, r2
	Return	r0
"wave-manager-get-npc-alive-count-by-faction_symbol_table":
	.quad	3423406688794319677
	.quad	4013010305452811590
	.quad	6237759117678430531
	.quad	6230461421097945948
	.quad	-2664920944689591574
	.quad	-5673227674328399655
"wave-manager-get-npc-alive-count-by-faction_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end1:
	.size	"wave-manager-get-npc-alive-count-by-faction", .Lfunc_end1-"wave-manager-get-npc-alive-count-by-faction"

	.globl	"wave-manager-get-total-npc-in-wave-count"
	.type	"wave-manager-get-total-npc-in-wave-count",@function
"wave-manager-get-total-npc-in-wave-count":
	LookupPointer	r0, 0
	Call	r0, r0, 1
	BranchIf	.LBB2_2, r0
	LookupPointer	r0, 3
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r49, 4
	LookupPointer	r1, 2
	Move	r50, r0
	CallFf	r0, r1, 2
	LookupPointer	r1, 1
	Move	r49, r0
	CallFf	r0, r1, 1
.LBB2_2:
	LoadStaticU64Imm	r49, 6
	LookupPointer	r0, 5
	CallFf	r0, r0, 1
	Return	r0
"wave-manager-get-total-npc-in-wave-count_symbol_table":
	.quad	998391088342721072
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	-3311351115081502130
	.quad	3
	.quad	4013010305452811590
	.quad	-7194759694070631083
"wave-manager-get-total-npc-in-wave-count_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	0
	.byte	0
.Lfunc_end2:
	.size	"wave-manager-get-total-npc-in-wave-count", .Lfunc_end2-"wave-manager-get-total-npc-in-wave-count"

	.globl	"wave-manager-is-ready?.1"
	.type	"wave-manager-is-ready?.1",@function
"wave-manager-is-ready?.1":
	Move	r1, r49
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	BranchIf	.LBB3_2, r0
	LookupPointer	r0, 3
	Move	r49, r1
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r49, 4
	LookupPointer	r2, 2
	Move	r50, r0
	CallFf	r0, r2, 2
	LookupPointer	r2, 1
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB3_2:
	LoadStaticU64Imm	r49, 6
	LookupPointer	r0, 5
	Move	r50, r1
	CallFf	r0, r0, 2
	Return	r0
"wave-manager-is-ready?.1_symbol_table":
	.quad	-6870362608876642110
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	-3311351115081502130
	.quad	3
	.quad	54185015286451590
	.quad	-4142738284031958410
"wave-manager-is-ready?.1_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	0
	.byte	0
.Lfunc_end3:
	.size	"wave-manager-is-ready?.1", .Lfunc_end3-"wave-manager-is-ready?.1"

	.globl	"wave-manager-get-encounter-intensity"
	.type	"wave-manager-get-encounter-intensity",@function
"wave-manager-get-encounter-intensity":
	Move	r1, r49
	LookupPointer	r0, 0
	Call	r0, r0, 1
	BranchIf	.LBB4_2, r0
	LookupPointer	r0, 3
	Move	r49, r1
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r49, 4
	LookupPointer	r2, 2
	Move	r50, r0
	CallFf	r0, r2, 2
	LookupPointer	r2, 1
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB4_2:
	LoadStaticU64Imm	r49, 6
	LookupPointer	r0, 5
	Move	r50, r1
	CallFf	r0, r0, 2
	Return	r0
"wave-manager-get-encounter-intensity_symbol_table":
	.quad	998391088342721072
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	-3311351115081502130
	.quad	3
	.quad	-8623127435633610462
	.quad	-3532724295824964118
"wave-manager-get-encounter-intensity_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	0
	.byte	0
.Lfunc_end4:
	.size	"wave-manager-get-encounter-intensity", .Lfunc_end4-"wave-manager-get-encounter-intensity"

	.globl	"wave-manager-get-next-spawning-wave-npc-schemas-group"
	.type	"wave-manager-get-next-spawning-wave-npc-schemas-group",@function
"wave-manager-get-next-spawning-wave-npc-schemas-group":
	Move	r4, r49
	LookupPointer	r0, 1
	Call	r0, r0, 1
	Move	r1, r0
	LookupPointer	r0, 0
	CallFf	r0, r0, 0
	Move	r2, r0
	BranchIfNot	.LBB5_1, r2
	BranchIfNot	.LBB5_26, r1
.LBB5_3:
	IAddImm	r0, r1, 32
	LoadU64	r5, r0
	IAddImm	r0, r5, 8
	LoadI32	r6, r0
	LoadStaticU64Imm	r49, 9
	LookupPointer	r0, 6
	Move	r50, r4
	CallFf	r0, r0, 2
	Move	r3, r0
	LoadStaticU64Imm	r49, 8
	LoadStaticU64Imm	r50, 7
	LoadU16Imm	r51, 0
	LookupPointer	r7, 5
	CallFf	r0, r7, 3
	Move	r1, r0
	ILessThan	r0, r3, r51
	BranchIf	.LBB5_27, r0
	IGreaterThanEqual	r0, r3, r6
	BranchIf	.LBB5_27, r0
	IMulImm	r0, r3, 24
	LoadU64	r3, r5
	IAdd	r0, r3, r0
	IAddImm	r0, r0, 8
	LoadU64	r9, r0
	LoadU16Imm	r3, 0
	Move	r51, r3
	CallFf	r0, r7, 3
	Move	r5, r0
	CallFf	r0, r7, 3
	Move	r6, r0
	CallFf	r0, r7, 3
	Move	r7, r0
	LoadStaticU64Imm	r49, 15
	LookupPointer	r10, 12
	Move	r50, r4
	CallFf	r0, r10, 2
	LookupPointer	r8, 11
	Move	r49, r5
	Move	r50, r0
	CallFf	r0, r8, 3
	LoadStaticU64Imm	r49, 14
	Move	r50, r4
	CallFf	r0, r10, 2
	Move	r49, r6
	Move	r50, r0
	CallFf	r0, r8, 3
	LoadStaticU64Imm	r49, 13
	Move	r50, r4
	CallFf	r0, r10, 2
	Move	r49, r7
	Move	r50, r0
	CallFf	r0, r8, 3
	LookupPointer	r10, 10
	Move	r49, r5
	CallFf	r0, r10, 1
	BranchIfNot	.LBB5_27, r0
	Move	r49, r6
	CallFf	r0, r10, 1
	BranchIf	.LBB5_8, r0
	LoadU16Imm	r51, 0
	Move	r49, r6
	Move	r50, r5
	CallFf	r0, r8, 3
.LBB5_8:
	LoadStaticU64Imm	r49, 19
	LoadU16Imm	r5, 1
	LookupPointer	r0, 17
	Move	r50, r5
	CallFf	r0, r0, 2
	Move	r7, r0
	Move	r49, r6
	CallFf	r0, r10, 1
	Move	r8, r0
	LoadStaticU64Imm	r49, 18
	LookupPointer	r0, 16
	Move	r50, r4
	CallFf	r0, r0, 2
	Move	r50, r3
	BranchIfNot	.LBB5_10, r0
	IMod	r50, r7, r8
.LBB5_10:
	LookupPointer	r0, 20
	Move	r49, r6
	CallFf	r0, r0, 2
	BranchIfNot	.LBB5_16, r0
	IAddImm	r4, r0, 12
	LoadU16	r4, r4
	INotEqual	r6, r4, r5
	BranchIf	.LBB5_28, r6
	LoadU8	r3, r0
	Branch	.LBB5_16
.LBB5_1:
	LoadStaticPointerImm	r49, 4
	LookupPointer	r0, 3
	CallFf	r0, r0, 1
	LoadU16Imm	r50, 19
	LookupPointer	r3, 2
	Move	r49, r0
	CallFf	r0, r3, 2
	BranchIf	.LBB5_3, r1
.LBB5_26:
	LoadStaticU64Imm	r49, 8
	LoadStaticU64Imm	r50, 7
	LoadU16Imm	r51, 0
	LookupPointer	r0, 5
	CallFf	r0, r0, 3
	Move	r1, r0
.LBB5_27:
	Move	r0, r1
	Return	r0
.LBB5_28:
	LoadU16Imm	r6, 2
	IEqual	r6, r4, r6
	BranchIfNot	.LBB5_29, r6
	LoadI32	r3, r0
	Branch	.LBB5_16
.LBB5_29:
	LoadU16Imm	r6, 3
	IEqual	r6, r4, r6
	BranchIfNot	.LBB5_30, r6
.LBB5_15:
	LoadU32	r3, r0
	Branch	.LBB5_16
.LBB5_30:
	LoadU16Imm	r6, 4
	IEqual	r6, r4, r6
	BranchIf	.LBB5_14, r6
	LoadU16Imm	r6, 5
	IEqual	r6, r4, r6
	BranchIfNot	.LBB5_32, r6
.LBB5_14:
	LoadU64	r3, r0
.LBB5_16:
	IMulImm	r0, r3, 8
	LoadU64	r3, r9
	IAdd	r0, r3, r0
	LoadU64	r6, r0
	IAddImm	r7, r6, 8
	LoadI32	r0, r7
	ILessThan	r0, r0, r5
	BranchIf	.LBB5_27, r0
	OpLogNot	r0, r2
	LookupPointer	r8, 21
	LookupPointer	r9, 22
	LookupPointer	r10, 23
	BranchIf	.LBB5_22, r0
	LoadU16Imm	r51, 0
	LoadU16Imm	r2, 7
	Move	r3, r51
	Branch	.LBB5_19
.LBB5_21:
	IAddImm	r3, r3, 1
	LoadI32	r0, r7
	ILessThan	r0, r3, r0
	BranchIfNot	.LBB5_27, r0
.LBB5_19:
	IMulImm	r0, r3, 16
	LoadU64	r4, r6
	IAdd	r49, r4, r0
	LoadU64	r50, r49
	Call	r0, r10, 1
	BranchIfNot	.LBB5_21, r0
	Move	r49, r2
	CallFf	r0, r9, 2
	Move	r49, r1
	Move	r50, r0
	CallFf	r0, r8, 3
	Branch	.LBB5_21
.LBB5_22:
	LoadStaticPointerImm	r11, 24
	LoadStaticPointerImm	r12, 25
	LoadU16Imm	r51, 0
	LookupPointer	r13, 2
	LookupPointer	r14, 3
	LookupPointer	r15, 26
	LoadU16Imm	r2, 19
	LoadU16Imm	r3, 7
	Move	r16, r51
	Branch	.LBB5_23
.LBB5_25:
	Move	r49, r4
	Call	r0, r15, 1
	Move	r49, r5
	Move	r50, r0
	Call	r0, r14, 2
	Move	r49, r0
	Move	r50, r2
	Call	r0, r13, 2
	IAddImm	r16, r16, 1
	LoadI32	r0, r7
	ILessThan	r0, r16, r0
	BranchIfNot	.LBB5_27, r0
.LBB5_23:
	IMulImm	r0, r16, 16
	LoadU64	r4, r6
	IAdd	r49, r4, r0
	LoadU64	r4, r49
	Call	r0, r10, 1
	Move	r5, r11
	BranchIfNot	.LBB5_25, r0
	Move	r49, r3
	Move	r50, r4
	CallFf	r0, r9, 2
	Move	r49, r1
	Move	r50, r0
	CallFf	r0, r8, 3
	Move	r5, r12
	Branch	.LBB5_25
.LBB5_32:
	LoadU16Imm	r6, 6
	IEqual	r4, r4, r6
	BranchIf	.LBB5_15, r4
	Branch	.LBB5_16
"wave-manager-get-next-spawning-wave-npc-schemas-group_symbol_table":
	.quad	-7966019873906053962
	.quad	-4561343946152313276
	.quad	-1851807429122828651
	.quad	6072285182812319085
	.quad	4
	.quad	-3193965096093911673
	.quad	-8623127435633610462
	.quad	-1
	.quad	8534565298598421705
	.quad	-6711785430563033859
	.quad	4013010305452811590
	.quad	7920520547792940799
	.quad	-1136051897790519866
	.quad	-7660363871649534493
	.quad	-160782944825580444
	.quad	4382432297032017585
	.quad	54185015286451590
	.quad	-1358566869505889023
	.quad	6884240918683042611
	.quad	9066152901236616051
	.quad	-2664920944689591574
	.quad	3321216610294482167
	.quad	3684810824690217768
	.quad	2009214271962714043
	.quad	26
	.quad	24
	.quad	-3311351115081502130
"wave-manager-get-next-spawning-wave-npc-schemas-group_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	1
	.byte	0
.Lfunc_end5:
	.size	"wave-manager-get-next-spawning-wave-npc-schemas-group", .Lfunc_end5-"wave-manager-get-next-spawning-wave-npc-schemas-group"

	.globl	"wave-manager-get-total-wave-count"
	.type	"wave-manager-get-total-wave-count",@function
"wave-manager-get-total-wave-count":
	Move	r1, r49
	LookupPointer	r0, 0
	Call	r0, r0, 1
	BranchIf	.LBB6_2, r0
	LookupPointer	r0, 3
	Move	r49, r1
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r49, 4
	LookupPointer	r2, 2
	Move	r50, r0
	CallFf	r0, r2, 2
	LookupPointer	r2, 1
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB6_2:
	LoadStaticU64Imm	r49, 6
	LookupPointer	r0, 5
	Move	r50, r1
	CallFf	r0, r0, 2
	Return	r0
"wave-manager-get-total-wave-count_symbol_table":
	.quad	998391088342721072
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	-3311351115081502130
	.quad	3
	.quad	-8623127435633610462
	.quad	126389912060929688
"wave-manager-get-total-wave-count_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	0
	.byte	0
.Lfunc_end6:
	.size	"wave-manager-get-total-wave-count", .Lfunc_end6-"wave-manager-get-total-wave-count"

	.globl	"wave-manager-get-num-waves-spawned"
	.type	"wave-manager-get-num-waves-spawned",@function
"wave-manager-get-num-waves-spawned":
	Move	r50, r49
	LoadStaticU64Imm	r49, 2
	LookupPointer	r0, 1
	CallFf	r0, r0, 2
	LookupPointer	r1, 0
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
"wave-manager-get-num-waves-spawned_symbol_table":
	.quad	4013010305452811590
	.quad	-1136051897790519866
	.quad	-7660363871649534493
"wave-manager-get-num-waves-spawned_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end7:
	.size	"wave-manager-get-num-waves-spawned", .Lfunc_end7-"wave-manager-get-num-waves-spawned"

	.globl	"wave-manager-get-next-spawning-wave-spawn-configs"
	.type	"wave-manager-get-next-spawning-wave-spawn-configs",@function
"wave-manager-get-next-spawning-wave-spawn-configs":
	Move	r50, r49
	LookupPointer	r0, 1
	Call	r0, r0, 1
	Move	r1, r0
	LoadStaticU64Imm	r49, 2
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	BranchIfNot	.LBB8_2, r0
	IAddImm	r0, r0, 24
	LoadU64	r0, r0
	BranchIfNot	.LBB8_2, r0
	Return	r0
.LBB8_2:
	BranchIfNot	.LBB8_3, r1
	IAddImm	r0, r1, 16
	LoadU64	r0, r0
	Return	r0
.LBB8_3:
	LoadU16Imm	r0, 0
	Return	r0
"wave-manager-get-next-spawning-wave-spawn-configs_symbol_table":
	.quad	-1136051897790519866
	.quad	-4561343946152313276
	.quad	-3630726564894351738
"wave-manager-get-next-spawning-wave-spawn-configs_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end8:
	.size	"wave-manager-get-next-spawning-wave-spawn-configs", .Lfunc_end8-"wave-manager-get-next-spawning-wave-spawn-configs"

	.globl	"wave-manager-get-next-spawning-wave-npc-count"
	.type	"wave-manager-get-next-spawning-wave-npc-count",@function
"wave-manager-get-next-spawning-wave-npc-count":
	LookupPointer	r0, 1
	Call	r0, r0, 1
	LookupPointer	r1, 0
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
"wave-manager-get-next-spawning-wave-npc-count_symbol_table":
	.quad	4013010305452811590
	.quad	1399776995102310962
"wave-manager-get-next-spawning-wave-npc-count_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end9:
	.size	"wave-manager-get-next-spawning-wave-npc-count", .Lfunc_end9-"wave-manager-get-next-spawning-wave-npc-count"

	.globl	"wave-manager-spawn-next-wave!"
	.type	"wave-manager-spawn-next-wave!",@function
"wave-manager-spawn-next-wave!":
	Move	r50, r49
	LookupPointer	r0, 0
	Call	r0, r0, 1
	BranchIfNot	.LBB10_2, r0
	LoadStaticU64Imm	r49, 2
	LookupPointer	r0, 1
	CallFf	r0, r0, 2
	Return	r0
.LBB10_2:
	LookupPointer	r0, 5
	Move	r49, r50
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r49, 6
	LookupPointer	r1, 4
	Move	r50, r0
	CallFf	r0, r1, 2
	LookupPointer	r1, 3
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
"wave-manager-spawn-next-wave!_symbol_table":
	.quad	998391088342721072
	.quad	4789555171397753330
	.quad	5189346610966935392
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	-3311351115081502130
	.quad	5
"wave-manager-spawn-next-wave!_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
.Lfunc_end10:
	.size	"wave-manager-spawn-next-wave!", .Lfunc_end10-"wave-manager-spawn-next-wave!"

	.globl	"wave-manager-get-npc-alive-group.2"
	.type	"wave-manager-get-npc-alive-group.2",@function
"wave-manager-get-npc-alive-group.2":
	LoadStaticU64Imm	r49, 5
	LoadStaticU64Imm	r50, 4
	LoadU16Imm	r1, 0
	LookupPointer	r0, 2
	Move	r51, r1
	CallFf	r0, r0, 3
	Move	r2, r0
	LoadStaticU64Imm	r49, 3
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	Move	r3, r0
	LookupPointer	r0, 0
	CallFf	r0, r0, 0
	LoadU16Imm	r0, 1
	ILessThan	r0, r3, r0
	BranchIf	.LBB11_8, r0
	LookupPointer	r7, 6
	LookupPointer	r8, 7
	LookupPointer	r9, 8
	LookupPointer	r10, 9
	LoadStaticU64Imm	r4, 3
	LoadU16Imm	r5, 7
	LoadU16Imm	r51, 1
	LoadU16Imm	r11, 5
	LoadU16Imm	r12, 4
	Move	r6, r1
	Branch	.LBB11_2
.LBB11_7:
	IAddImm	r6, r6, 1
	ILessThan	r0, r6, r3
	BranchIfNot	.LBB11_8, r0
.LBB11_2:
	Move	r49, r4
	Move	r50, r6
	CallFf	r0, r10, 2
	Move	r50, r1
	BranchIfNot	.LBB11_5, r0
	IAddImm	r13, r0, 12
	LoadU16	r13, r13
	INotEqual	r14, r13, r5
	BranchIfNot	.LBB11_4, r14
	IEqual	r14, r13, r11
	BranchIf	.LBB11_4, r14
	IEqual	r13, r13, r12
	Move	r50, r1
	BranchIfNot	.LBB11_5, r13
.LBB11_4:
	LoadU64	r50, r0
.LBB11_5:
	Move	r49, r50
	CallFf	r0, r9, 1
	BranchIfNot	.LBB11_7, r0
	Move	r49, r5
	CallFf	r0, r8, 2
	Move	r49, r2
	Move	r50, r0
	CallFf	r0, r7, 3
	Branch	.LBB11_7
.LBB11_8:
	LookupPointer	r0, 10
	CallFf	r0, r0, 0
	Move	r0, r2
	Return	r0
"wave-manager-get-npc-alive-group.2_symbol_table":
	.quad	3423406688794319677
	.quad	4013010305452811590
	.quad	-3193965096093911673
	.quad	-8578220443451849366
	.quad	-1
	.quad	8534565298598421705
	.quad	3321216610294482167
	.quad	3684810824690217768
	.quad	-4433807632111512251
	.quad	-2664920944689591574
	.quad	-5673227674328399655
"wave-manager-get-npc-alive-group.2_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end11:
	.size	"wave-manager-get-npc-alive-group.2", .Lfunc_end11-"wave-manager-get-npc-alive-group.2"

	.globl	"wave-manager-kill-all-npcs"
	.type	"wave-manager-kill-all-npcs",@function
"wave-manager-kill-all-npcs":
	Move	r1, r49
	LookupPointer	r0, 0
	Call	r0, r0, 1
	BranchIfNot	.LBB12_1, r0
	LookupPointer	r6, 3
	Move	r49, r1
	Call	r0, r6, 1
	Move	r2, r0
	LookupPointer	r0, 2
	Move	r49, r2
	CallFf	r0, r0, 1
	Move	r3, r0
	LookupPointer	r0, 1
	CallFf	r0, r0, 0
	LoadU16Imm	r0, 1
	ILessThan	r0, r3, r0
	BranchIf	.LBB12_8, r0
	LoadU16Imm	r51, 0
	LookupPointer	r7, 4
	LookupPointer	r8, 5
	LoadU16Imm	r4, 2
	LoadU16Imm	r9, 7
	LoadU16Imm	r10, 5
	LoadU16Imm	r11, 4
	Move	r5, r51
	Branch	.LBB12_4
.LBB12_6:
	LoadU64	r49, r0
.LBB12_7:
	Move	r50, r4
	Move	r52, r51
	Move	r53, r51
	CallFf	r0, r7, 5
	IAddImm	r5, r5, 1
	ILessThan	r0, r5, r3
	BranchIfNot	.LBB12_8, r0
.LBB12_4:
	Move	r49, r2
	Move	r50, r5
	CallFf	r0, r8, 2
	Move	r49, r51
	BranchIfNot	.LBB12_7, r0
	IAddImm	r12, r0, 12
	LoadU16	r12, r12
	INotEqual	r13, r12, r9
	BranchIfNot	.LBB12_6, r13
	IEqual	r13, r12, r10
	BranchIf	.LBB12_6, r13
	IEqual	r12, r12, r11
	Move	r49, r51
	BranchIf	.LBB12_6, r12
	Branch	.LBB12_7
.LBB12_8:
	LookupPointer	r0, 7
	CallFf	r0, r0, 0
	Move	r49, r1
	Call	r0, r6, 1
	LoadU16Imm	r50, 0
	LookupPointer	r1, 6
	Move	r49, r0
	Call	r0, r1, 2
	Return	r0
.LBB12_1:
	LoadU16Imm	r0, 0
	Return	r0
"wave-manager-kill-all-npcs_symbol_table":
	.quad	-8385702754139928849
	.quad	3423406688794319677
	.quad	4013010305452811590
	.quad	6237759117678430531
	.quad	-7738336759357028456
	.quad	-2664920944689591574
	.quad	-3835701475675913431
	.quad	-5673227674328399655
"wave-manager-kill-all-npcs_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end12:
	.size	"wave-manager-kill-all-npcs", .Lfunc_end12-"wave-manager-kill-all-npcs"

	.globl	"wave-manager-get-lost-player-npc-group"
	.type	"wave-manager-get-lost-player-npc-group",@function
"wave-manager-get-lost-player-npc-group":
	LookupPointer	r0, 3
	Call	r0, r0, 1
	Move	r1, r0
	LoadStaticU64Imm	r49, 5
	LoadStaticU64Imm	r50, 4
	LoadU16Imm	r2, 0
	LookupPointer	r0, 2
	Move	r51, r2
	CallFf	r0, r0, 3
	Move	r3, r0
	LookupPointer	r0, 1
	Move	r49, r1
	CallFf	r0, r0, 1
	Move	r4, r0
	LookupPointer	r0, 0
	CallFf	r0, r0, 0
	LoadU16Imm	r0, 1
	ILessThan	r0, r4, r0
	BranchIf	.LBB13_8, r0
	LookupPointer	r7, 6
	LookupPointer	r8, 7
	LookupPointer	r9, 8
	LookupPointer	r10, 9
	LoadU16Imm	r5, 7
	LoadU16Imm	r51, 1
	LoadU16Imm	r11, 5
	LoadU16Imm	r12, 4
	Move	r6, r2
	Branch	.LBB13_2
.LBB13_7:
	IAddImm	r6, r6, 1
	ILessThan	r0, r6, r4
	BranchIfNot	.LBB13_8, r0
.LBB13_2:
	Move	r49, r1
	Move	r50, r6
	CallFf	r0, r10, 2
	Move	r50, r2
	BranchIfNot	.LBB13_5, r0
	IAddImm	r13, r0, 12
	LoadU16	r13, r13
	INotEqual	r14, r13, r5
	BranchIfNot	.LBB13_4, r14
	IEqual	r14, r13, r11
	BranchIf	.LBB13_4, r14
	IEqual	r13, r13, r12
	Move	r50, r2
	BranchIfNot	.LBB13_5, r13
.LBB13_4:
	LoadU64	r50, r0
.LBB13_5:
	Move	r49, r50
	Call	r0, r9, 1
	BranchIfNot	.LBB13_7, r0
	Move	r49, r5
	CallFf	r0, r8, 2
	Move	r49, r3
	Move	r50, r0
	CallFf	r0, r7, 3
	Branch	.LBB13_7
.LBB13_8:
	LookupPointer	r0, 10
	CallFf	r0, r0, 0
	Move	r0, r3
	Return	r0
"wave-manager-get-lost-player-npc-group_symbol_table":
	.quad	3423406688794319677
	.quad	4013010305452811590
	.quad	-3193965096093911673
	.quad	6237759117678430531
	.quad	-1
	.quad	8534565298598421705
	.quad	3321216610294482167
	.quad	3684810824690217768
	.quad	-1345322216029872946
	.quad	-2664920944689591574
	.quad	-5673227674328399655
"wave-manager-get-lost-player-npc-group_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end13:
	.size	"wave-manager-get-lost-player-npc-group", .Lfunc_end13-"wave-manager-get-lost-player-npc-group"

	.globl	"wave-manager-stop!"
	.type	"wave-manager-stop!",@function
"wave-manager-stop!":
	Move	r50, r49
	LookupPointer	r0, 0
	Call	r0, r0, 1
	BranchIfNot	.LBB14_2, r0
	LoadStaticU64Imm	r49, 2
	LookupPointer	r0, 1
	CallFf	r0, r0, 2
	Return	r0
.LBB14_2:
	LookupPointer	r0, 5
	Move	r49, r50
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r49, 6
	LookupPointer	r1, 4
	Move	r50, r0
	CallFf	r0, r1, 2
	LookupPointer	r1, 3
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
"wave-manager-stop!_symbol_table":
	.quad	998391088342721072
	.quad	4789555171397753330
	.quad	4737644804400110462
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	-3311351115081502130
	.quad	5
"wave-manager-stop!_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
.Lfunc_end14:
	.size	"wave-manager-stop!", .Lfunc_end14-"wave-manager-stop!"

	.globl	"wave-manager-get-npc-alive-count.3"
	.type	"wave-manager-get-npc-alive-count.3",@function
"wave-manager-get-npc-alive-count.3":
	LookupPointer	r0, 0
	Call	r0, r0, 1
	BranchIf	.LBB15_2, r0
	LookupPointer	r0, 3
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r49, 4
	LookupPointer	r1, 2
	Move	r50, r0
	CallFf	r0, r1, 2
	LookupPointer	r1, 1
	Move	r49, r0
	CallFf	r0, r1, 1
.LBB15_2:
	LoadStaticU64Imm	r49, 6
	LookupPointer	r0, 5
	CallFf	r0, r0, 1
	Return	r0
"wave-manager-get-npc-alive-count.3_symbol_table":
	.quad	998391088342721072
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	-3311351115081502130
	.quad	3
	.quad	1564617029140756692
	.quad	-8578220443451849366
"wave-manager-get-npc-alive-count.3_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	0
	.byte	0
.Lfunc_end15:
	.size	"wave-manager-get-npc-alive-count.3", .Lfunc_end15-"wave-manager-get-npc-alive-count.3"

	.globl	"wave-manager-get-next-wave-faction"
	.type	"wave-manager-get-next-wave-faction",@function
"wave-manager-get-next-wave-faction":
	Move	r50, r49
	LoadStaticU64Imm	r49, 6
	LookupPointer	r0, 1
	CallFf	r0, r0, 2
	Move	r1, r0
	LoadStaticU64Imm	r0, 3
	OpBitXor	r0, r1, r0
	LoadStaticU64Imm	r2, 2
	ILessThan	r0, r0, r2
	LoadStaticU64Imm	r2, 4
	BranchIf	.LBB16_2, r0
	Move	r1, r2
.LBB16_2:
	LoadStaticU64Imm	r49, 5
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	LoadU16Imm	r3, 0
	INotEqual	r0, r0, r3
	BranchIf	.LBB16_4, r0
	Move	r1, r2
.LBB16_4:
	Move	r0, r1
	Return	r0
"wave-manager-get-next-wave-faction_symbol_table":
	.quad	54185015286451590
	.quad	-8623127435633610462
	.quad	-9223372036854775804
	.quad	-9223372036854775808
	.quad	-1
	.quad	-4142738284031958410
	.quad	3512716474478954862
"wave-manager-get-next-wave-faction_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end16:
	.size	"wave-manager-get-next-wave-faction", .Lfunc_end16-"wave-manager-get-next-wave-faction"

	.globl	"wave-manager-get-lost-player-npc-group-count"
	.type	"wave-manager-get-lost-player-npc-group-count",@function
"wave-manager-get-lost-player-npc-group-count":
	LookupPointer	r0, 1
	Call	r0, r0, 1
	LookupPointer	r1, 0
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
"wave-manager-get-lost-player-npc-group-count_symbol_table":
	.quad	4013010305452811590
	.quad	7267865752999468798
"wave-manager-get-lost-player-npc-group-count_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end17:
	.size	"wave-manager-get-lost-player-npc-group-count", .Lfunc_end17-"wave-manager-get-lost-player-npc-group-count"

	.globl	"wave-manager-spawn-next-special-wave!"
	.type	"wave-manager-spawn-next-special-wave!",@function
"wave-manager-spawn-next-special-wave!":
	Move	r50, r49
	LookupPointer	r0, 0
	Call	r0, r0, 1
	BranchIfNot	.LBB18_2, r0
	LoadStaticU64Imm	r49, 2
	LookupPointer	r0, 1
	CallFf	r0, r0, 2
	Return	r0
.LBB18_2:
	LookupPointer	r0, 5
	Move	r49, r50
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r49, 6
	LookupPointer	r1, 4
	Move	r50, r0
	CallFf	r0, r1, 2
	LookupPointer	r1, 3
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
"wave-manager-spawn-next-special-wave!_symbol_table":
	.quad	998391088342721072
	.quad	4789555171397753330
	.quad	1354182575228257266
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	-3311351115081502130
	.quad	5
"wave-manager-spawn-next-special-wave!_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
.Lfunc_end18:
	.size	"wave-manager-spawn-next-special-wave!", .Lfunc_end18-"wave-manager-spawn-next-special-wave!"

	.globl	"wave-manager-current-task-in-run?"
	.type	"wave-manager-current-task-in-run?",@function
"wave-manager-current-task-in-run?":
	LoadStaticU64Imm	r49, 2
	LoadStaticU64Imm	r50, 1
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	Return	r0
"wave-manager-current-task-in-run?_symbol_table":
	.quad	54185015286451590
	.quad	3766070110510549571
	.quad	-5700187185490357332
"wave-manager-current-task-in-run?_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end19:
	.size	"wave-manager-current-task-in-run?", .Lfunc_end19-"wave-manager-current-task-in-run?"

	.globl	"wave-manager-get-next-spawning-wave-composition.4"
	.type	"wave-manager-get-next-spawning-wave-composition.4",@function
"wave-manager-get-next-spawning-wave-composition.4":
	Move	r50, r49
	LoadStaticU64Imm	r49, 1
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	Return	r0
"wave-manager-get-next-spawning-wave-composition.4_symbol_table":
	.quad	-1136051897790519866
	.quad	-8364948414460296332
"wave-manager-get-next-spawning-wave-composition.4_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end20:
	.size	"wave-manager-get-next-spawning-wave-composition.4", .Lfunc_end20-"wave-manager-get-next-spawning-wave-composition.4"

	.globl	"wave-manager-has-requested-wave-spawned?"
	.type	"wave-manager-has-requested-wave-spawned?",@function
"wave-manager-has-requested-wave-spawned?":
	Move	r1, r49
	LookupPointer	r0, 0
	Call	r0, r0, 1
	BranchIf	.LBB21_2, r0
	LookupPointer	r0, 3
	Move	r49, r1
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r49, 4
	LookupPointer	r2, 2
	Move	r50, r0
	CallFf	r0, r2, 2
	LookupPointer	r2, 1
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB21_2:
	LoadStaticU64Imm	r49, 6
	LookupPointer	r2, 5
	Move	r50, r1
	CallFf	r0, r2, 2
	BranchIfNot	.LBB21_3, r0
	LoadStaticU64Imm	r49, 7
	Move	r50, r1
	CallFf	r0, r2, 2
	Return	r0
.LBB21_3:
	LoadU16Imm	r0, 0
	Return	r0
"wave-manager-has-requested-wave-spawned?_symbol_table":
	.quad	998391088342721072
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	-3311351115081502130
	.quad	3
	.quad	54185015286451590
	.quad	-4748032580888550375
	.quad	224422539916484139
"wave-manager-has-requested-wave-spawned?_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end21:
	.size	"wave-manager-has-requested-wave-spawned?", .Lfunc_end21-"wave-manager-has-requested-wave-spawned?"

	.globl	"wave-manager-npc-lost-player?.5"
	.type	"wave-manager-npc-lost-player?.5",@function
"wave-manager-npc-lost-player?.5":
	LoadStaticU64Imm	r50, 2
	LookupPointer	r0, 1
	CallFf	r0, r0, 2
	Move	r1, r0
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	LoadU16Imm	r2, 0
	BranchIfNot	.LBB22_3, r0
	LookupPointer	r0, 3
	Call	r0, r0, 1
	BranchIfNot	.LBB22_2, r0
.LBB22_3:
	Move	r0, r2
	Return	r0
.LBB22_2:
	LoadStaticU64Imm	r0, 4
	IAdd	r0, r1, r0
	LoadStaticU64Imm	r1, 5
	ILessThan	r2, r0, r1
	Move	r0, r2
	Return	r0
"wave-manager-npc-lost-player?.5_symbol_table":
	.quad	-4433807632111512251
	.quad	-5833053787651694927
	.quad	5008278420455340480
	.quad	-7707884455580369866
	.quad	9223372036854775804
	.quad	9223372036854775806
"wave-manager-npc-lost-player?.5_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end22:
	.size	"wave-manager-npc-lost-player?.5", .Lfunc_end22-"wave-manager-npc-lost-player?.5"

	.type	"wave-manager-get-npc-alive-count-by-faction_symbol_table_src",@object
	.section	".rodata.wave-manager-get-npc-alive-count-by-faction_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wave-manager-get-npc-alive-count-by-faction_symbol_table_src":
	.quad	6237759117678430531
	.quad	4013010305452811590
	.quad	3423406688794319677
	.quad	-2664920944689591574
	.quad	6230461421097945948
	.quad	-5673227674328399655
	.size	"wave-manager-get-npc-alive-count-by-faction_symbol_table_src", 48

	.type	"wave-manager-get-total-npc-in-wave-count_symbol_table_src",@object
	.section	".rodata.wave-manager-get-total-npc-in-wave-count_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wave-manager-get-total-npc-in-wave-count_symbol_table_src":
	.quad	998391088342721072
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	1303844840429
	.quad	-3311351115081502130
	.quad	4013010305452811590
	.quad	-7194759694070631083
	.size	"wave-manager-get-total-npc-in-wave-count_symbol_table_src", 56

	.type	"wave-manager-is-ready?_symbol_table_src",@object
	.section	".rodata.wave-manager-is-ready?_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wave-manager-is-ready?_symbol_table_src":
	.quad	-6870362608876642110
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	1303844840768
	.quad	-3311351115081502130
	.quad	54185015286451590
	.quad	-4142738284031958410
	.size	"wave-manager-is-ready?_symbol_table_src", 56

	.type	"wave-manager-get-encounter-intensity_symbol_table_src",@object
	.section	".rodata.wave-manager-get-encounter-intensity_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wave-manager-get-encounter-intensity_symbol_table_src":
	.quad	998391088342721072
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	1303844840368
	.quad	-3311351115081502130
	.quad	-8623127435633610462
	.quad	-3532724295824964118
	.size	"wave-manager-get-encounter-intensity_symbol_table_src", 56

	.type	"wave-manager-get-next-spawning-wave-npc-schemas-group_symbol_table_src",@object
	.section	".rodata.wave-manager-get-next-spawning-wave-npc-schemas-group_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wave-manager-get-next-spawning-wave-npc-schemas-group_symbol_table_src":
	.quad	-4561343946152313276
	.quad	-7966019873906053962
	.quad	-1851807429122828651
	.quad	6072285182812319085
	.quad	1303844840893
	.quad	-8623127435633610462
	.quad	-6711785430563033859
	.quad	-3193965096093911673
	.quad	8534565298598421705
	.quad	4294967295
	.quad	7920520547792940799
	.quad	-1136051897790519866
	.quad	4382432297032017585
	.quad	-160782944825580444
	.quad	-7660363871649534493
	.quad	4013010305452811590
	.quad	-1358566869505889023
	.quad	9066152901236616051
	.quad	54185015286451590
	.quad	6884240918683042611
	.quad	-2664920944689591574
	.quad	2009214271962714043
	.quad	3321216610294482167
	.quad	3684810824690217768
	.quad	1303844840822
	.quad	-3311351115081502130
	.quad	1303844840843
	.size	"wave-manager-get-next-spawning-wave-npc-schemas-group_symbol_table_src", 216

	.type	"wave-manager-get-total-wave-count_symbol_table_src",@object
	.section	".rodata.wave-manager-get-total-wave-count_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wave-manager-get-total-wave-count_symbol_table_src":
	.quad	998391088342721072
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	1303844840486
	.quad	-3311351115081502130
	.quad	-8623127435633610462
	.quad	126389912060929688
	.size	"wave-manager-get-total-wave-count_symbol_table_src", 56

	.type	"wave-manager-get-num-waves-spawned_symbol_table_src",@object
	.section	".rodata.wave-manager-get-num-waves-spawned_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wave-manager-get-num-waves-spawned_symbol_table_src":
	.quad	4013010305452811590
	.quad	-1136051897790519866
	.quad	-7660363871649534493
	.size	"wave-manager-get-num-waves-spawned_symbol_table_src", 24

	.type	"wave-manager-get-next-spawning-wave-spawn-configs_symbol_table_src",@object
	.section	".rodata.wave-manager-get-next-spawning-wave-spawn-configs_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wave-manager-get-next-spawning-wave-spawn-configs_symbol_table_src":
	.quad	-4561343946152313276
	.quad	-1136051897790519866
	.quad	-3630726564894351738
	.size	"wave-manager-get-next-spawning-wave-spawn-configs_symbol_table_src", 24

	.type	"wave-manager-get-next-spawning-wave-npc-count_symbol_table_src",@object
	.section	".rodata.cst16.wave-manager-get-next-spawning-wave-npc-count_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"wave-manager-get-next-spawning-wave-npc-count_symbol_table_src":
	.quad	1399776995102310962
	.quad	4013010305452811590
	.size	"wave-manager-get-next-spawning-wave-npc-count_symbol_table_src", 16

	.type	"wave-manager-spawn-next-wave!_symbol_table_src",@object
	.section	".rodata.wave-manager-spawn-next-wave!_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wave-manager-spawn-next-wave!_symbol_table_src":
	.quad	998391088342721072
	.quad	4789555171397753330
	.quad	5189346610966935392
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	1303844840671
	.quad	-3311351115081502130
	.size	"wave-manager-spawn-next-wave!_symbol_table_src", 56

	.type	"wave-manager-get-npc-alive-group_symbol_table_src",@object
	.section	".rodata.wave-manager-get-npc-alive-group_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wave-manager-get-npc-alive-group_symbol_table_src":
	.quad	-3193965096093911673
	.quad	8534565298598421705
	.quad	4294967295
	.quad	4013010305452811590
	.quad	-8578220443451849366
	.quad	3423406688794319677
	.quad	-2664920944689591574
	.quad	-4433807632111512251
	.quad	3321216610294482167
	.quad	3684810824690217768
	.quad	-5673227674328399655
	.size	"wave-manager-get-npc-alive-group_symbol_table_src", 88

	.type	"wave-manager-kill-all-npcs_symbol_table_src",@object
	.section	".rodata.wave-manager-kill-all-npcs_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wave-manager-kill-all-npcs_symbol_table_src":
	.quad	-8385702754139928849
	.quad	6237759117678430531
	.quad	4013010305452811590
	.quad	3423406688794319677
	.quad	-2664920944689591574
	.quad	-7738336759357028456
	.quad	0
	.quad	-5673227674328399655
	.quad	-3835701475675913431
	.size	"wave-manager-kill-all-npcs_symbol_table_src", 72

	.type	"wave-manager-get-lost-player-npc-group_symbol_table_src",@object
	.section	".rodata.wave-manager-get-lost-player-npc-group_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wave-manager-get-lost-player-npc-group_symbol_table_src":
	.quad	6237759117678430531
	.quad	-3193965096093911673
	.quad	8534565298598421705
	.quad	4294967295
	.quad	4013010305452811590
	.quad	3423406688794319677
	.quad	-2664920944689591574
	.quad	-1345322216029872946
	.quad	3321216610294482167
	.quad	3684810824690217768
	.quad	-5673227674328399655
	.size	"wave-manager-get-lost-player-npc-group_symbol_table_src", 88

	.type	"wave-manager-stop!_symbol_table_src",@object
	.section	".rodata.wave-manager-stop!_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wave-manager-stop!_symbol_table_src":
	.quad	998391088342721072
	.quad	4789555171397753330
	.quad	4737644804400110462
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	1303844840725
	.quad	-3311351115081502130
	.size	"wave-manager-stop!_symbol_table_src", 56

	.type	"wave-manager-get-npc-alive-count_symbol_table_src",@object
	.section	".rodata.wave-manager-get-npc-alive-count_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wave-manager-get-npc-alive-count_symbol_table_src":
	.quad	998391088342721072
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	1303844840429
	.quad	-3311351115081502130
	.quad	1564617029140756692
	.quad	-8578220443451849366
	.size	"wave-manager-get-npc-alive-count_symbol_table_src", 56

	.type	"wave-manager-get-next-wave-faction_symbol_table_src",@object
	.section	".rodata.wave-manager-get-next-wave-faction_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wave-manager-get-next-wave-faction_symbol_table_src":
	.quad	-8623127435633610462
	.quad	3512716474478954862
	.quad	54185015286451590
	.quad	-4142738284031958410
	.quad	4294967295
	.size	"wave-manager-get-next-wave-faction_symbol_table_src", 40

	.type	"wave-manager-get-lost-player-npc-group-count_symbol_table_src",@object
	.section	".rodata.cst16.wave-manager-get-lost-player-npc-group-count_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"wave-manager-get-lost-player-npc-group-count_symbol_table_src":
	.quad	4013010305452811590
	.quad	7267865752999468798
	.size	"wave-manager-get-lost-player-npc-group-count_symbol_table_src", 16

	.type	"wave-manager-spawn-next-special-wave!_symbol_table_src",@object
	.section	".rodata.wave-manager-spawn-next-special-wave!_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wave-manager-spawn-next-special-wave!_symbol_table_src":
	.quad	998391088342721072
	.quad	4789555171397753330
	.quad	1354182575228257266
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	1303844840609
	.quad	-3311351115081502130
	.size	"wave-manager-spawn-next-special-wave!_symbol_table_src", 56

	.type	"wave-manager-current-task-in-run?_symbol_table_src",@object
	.section	".rodata.wave-manager-current-task-in-run?_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wave-manager-current-task-in-run?_symbol_table_src":
	.quad	54185015286451590
	.quad	-5700187185490357332
	.quad	3766070110510549571
	.size	"wave-manager-current-task-in-run?_symbol_table_src", 24

	.type	"wave-manager-get-next-spawning-wave-composition_symbol_table_src",@object
	.section	".rodata.cst16.wave-manager-get-next-spawning-wave-composition_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"wave-manager-get-next-spawning-wave-composition_symbol_table_src":
	.quad	-1136051897790519866
	.quad	-8364948414460296332
	.size	"wave-manager-get-next-spawning-wave-composition_symbol_table_src", 16

	.type	"wave-manager-has-requested-wave-spawned?_symbol_table_src",@object
	.section	".rodata.wave-manager-has-requested-wave-spawned?_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wave-manager-has-requested-wave-spawned?_symbol_table_src":
	.quad	998391088342721072
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	1303844840544
	.quad	-3311351115081502130
	.quad	54185015286451590
	.quad	-4748032580888550375
	.quad	224422539916484139
	.size	"wave-manager-has-requested-wave-spawned?_symbol_table_src", 64

	.type	"wave-manager-npc-lost-player?_symbol_table_src",@object
	.section	".rodata.cst32.wave-manager-npc-lost-player?_symbol_table_src","aMR",@progbits,32
	.p2align	3, 0x0
"wave-manager-npc-lost-player?_symbol_table_src":
	.quad	-5833053787651694927
	.quad	5008278420455340480
	.quad	-4433807632111512251
	.quad	-7707884455580369866
	.size	"wave-manager-npc-lost-player?_symbol_table_src", 32

	.section	".note.GNU-stack","",@progbits

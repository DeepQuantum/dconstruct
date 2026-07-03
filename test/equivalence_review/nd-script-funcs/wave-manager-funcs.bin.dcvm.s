	.file	"wave-manager-funcs.bin"
	.text
	.globl	"wave-manager-get-npc-alive-count-by-faction"
	.type	"wave-manager-get-npc-alive-count-by-faction",@function
"wave-manager-get-npc-alive-count-by-faction":
	Move	r4, r50
	LookupPointer	r0, 2
	Call	r0, r0, 1
	Move	r2, r0
	LookupPointer	r3, 1
	Move	r49, r2
	CallFf	r0, r3, 1
	Move	r1, r0
	CallFf	r0, r3, 1
	Move	r3, r0
	LookupPointer	r0, 0
	CallFf	r0, r0, 0
	LoadU16Imm	r0, 1
	ILessThan	r0, r3, r0
	BranchIf	.LBB0_6, r0
	LoadU16Imm	r5, 0
	LookupPointer	r6, 3
	LookupPointer	r7, 4
	LoadStaticU64Imm	r8, 5
	LoadU16Imm	r9, 7
	LoadU16Imm	r10, 5
	LoadU16Imm	r11, 4
	LoadU16Imm	r12, 0
	Branch	.LBB0_2
.LBB0_4:
	LoadU64	r13, r0
.LBB0_5:
	Move	r49, r13
	Move	r50, r8
	Move	r51, r5
	Move	r52, r5
	CallFf	r0, r6, 4
	INotEqual	r0, r4, r0
	ISub	r1, r1, r0
	IAddImm	r12, r12, 1
	ILessThan	r0, r12, r3
	BranchIfNot	.LBB0_6, r0
.LBB0_2:
	Move	r49, r2
	Move	r50, r12
	CallFf	r0, r7, 2
	LoadU16Imm	r13, 0
	BranchIfNot	.LBB0_5, r0
	IAddImm	r13, r0, 12
	LoadU16	r13, r13
	INotEqual	r14, r13, r9
	BranchIfNot	.LBB0_4, r14
	IEqual	r14, r13, r10
	BranchIf	.LBB0_4, r14
	IEqual	r14, r13, r11
	LoadU16Imm	r13, 0
	BranchIf	.LBB0_4, r14
	Branch	.LBB0_5
.LBB0_6:
	LookupPointer	r0, 6
	CallFf	r0, r0, 0
	Move	r0, r1
	Return	r0
"wave-manager-get-npc-alive-count-by-faction_symbol_table":
	.quad	3423406688794319677
	.quad	4013010305452811590
	.quad	6237759117678430531
	.quad	-4267555934341449426
	.quad	-2664920944689591574
	.quad	-2929813648748357861
	.quad	-5673227674328399655
"wave-manager-get-npc-alive-count-by-faction_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end0:
	.size	"wave-manager-get-npc-alive-count-by-faction", .Lfunc_end0-"wave-manager-get-npc-alive-count-by-faction"

	.globl	"wave-manager-get-total-npc-in-wave-count"
	.type	"wave-manager-get-total-npc-in-wave-count",@function
"wave-manager-get-total-npc-in-wave-count":
	Move	r1, r49
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	BranchIf	.LBB1_2, r0
	LookupPointer	r0, 3
	Move	r49, r1
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r2, 4
	LookupPointer	r3, 2
	Move	r49, r2
	Move	r50, r0
	CallFf	r0, r3, 2
	LookupPointer	r2, 1
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB1_2:
	LoadStaticU64Imm	r0, 6
	LookupPointer	r2, 5
	Move	r49, r0
	Move	r50, r1
	CallFf	r0, r2, 2
	BranchIf	.LBB1_4, r0
	LookupPointer	r0, 3
	Move	r49, r1
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r1, 7
	LookupPointer	r2, 2
	Move	r49, r1
	Move	r50, r0
	CallFf	r0, r2, 2
	LookupPointer	r1, 1
	Move	r49, r0
	CallFf	r0, r1, 1
.LBB1_4:
	LoadStaticU64Imm	r0, 9
	LookupPointer	r1, 8
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
"wave-manager-get-total-npc-in-wave-count_symbol_table":
	.quad	-6870362608876642110
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	-3311351115081502130
	.quad	13632
	.quad	54185015286451590
	.quad	-4142738284031958410
	.quad	13293
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
	.byte	1
	.byte	0
	.byte	0
.Lfunc_end1:
	.size	"wave-manager-get-total-npc-in-wave-count", .Lfunc_end1-"wave-manager-get-total-npc-in-wave-count"

	.globl	"wave-manager-is-ready?"
	.type	"wave-manager-is-ready?",@function
"wave-manager-is-ready?":
	Move	r1, r49
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	BranchIf	.LBB2_2, r0
	LookupPointer	r0, 3
	Move	r49, r1
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r2, 4
	LookupPointer	r3, 2
	Move	r49, r2
	Move	r50, r0
	CallFf	r0, r3, 2
	LookupPointer	r2, 1
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB2_2:
	LoadStaticU64Imm	r0, 6
	LookupPointer	r2, 5
	Move	r49, r0
	Move	r50, r1
	CallFf	r0, r2, 2
	Return	r0
"wave-manager-is-ready?_symbol_table":
	.quad	-6870362608876642110
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	-3311351115081502130
	.quad	13632
	.quad	54185015286451590
	.quad	-4142738284031958410
"wave-manager-is-ready?_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	0
	.byte	0
.Lfunc_end2:
	.size	"wave-manager-is-ready?", .Lfunc_end2-"wave-manager-is-ready?"

	.globl	"wave-manager-get-encounter-intensity"
	.type	"wave-manager-get-encounter-intensity",@function
"wave-manager-get-encounter-intensity":
	Move	r1, r49
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	BranchIf	.LBB3_2, r0
	LookupPointer	r0, 3
	Move	r49, r1
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r2, 4
	LookupPointer	r3, 2
	Move	r49, r2
	Move	r50, r0
	CallFf	r0, r3, 2
	LookupPointer	r2, 1
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB3_2:
	LoadStaticU64Imm	r0, 6
	LookupPointer	r2, 5
	Move	r49, r0
	Move	r50, r1
	CallFf	r0, r2, 2
	BranchIf	.LBB3_4, r0
	LookupPointer	r0, 3
	Move	r49, r1
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r2, 7
	LookupPointer	r3, 2
	Move	r49, r2
	Move	r50, r0
	CallFf	r0, r3, 2
	LookupPointer	r2, 1
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB3_4:
	LoadStaticU64Imm	r0, 9
	LookupPointer	r2, 8
	Move	r49, r0
	Move	r50, r1
	CallFf	r0, r2, 2
	Return	r0
"wave-manager-get-encounter-intensity_symbol_table":
	.quad	-6870362608876642110
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	-3311351115081502130
	.quad	13632
	.quad	54185015286451590
	.quad	-4142738284031958410
	.quad	13232
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
	.byte	1
	.byte	0
	.byte	0
.Lfunc_end3:
	.size	"wave-manager-get-encounter-intensity", .Lfunc_end3-"wave-manager-get-encounter-intensity"

	.globl	"wave-manager-get-next-spawning-wave-npc-schemas-group"
	.type	"wave-manager-get-next-spawning-wave-npc-schemas-group",@function
"wave-manager-get-next-spawning-wave-npc-schemas-group":
	Move	r6, r49
	LoadStaticU64Imm	r0, 1
	LookupPointer	r7, 0
	Move	r49, r0
	Move	r50, r6
	CallFf	r0, r7, 2
	BranchIfNot	.LBB4_20, r0
	IAddImm	r0, r0, 32
	LoadU64	r3, r0
	IAddImm	r0, r3, 8
	LoadI32	r5, r0
	LoadStaticU64Imm	r0, 6
	LookupPointer	r1, 3
	Move	r49, r0
	Move	r50, r6
	CallFf	r0, r1, 2
	Move	r2, r0
	LoadStaticU64Imm	r0, 5
	LoadStaticU64Imm	r1, 4
	LoadU16Imm	r8, 0
	LookupPointer	r4, 2
	Move	r49, r0
	Move	r50, r1
	Move	r51, r8
	CallFf	r0, r4, 3
	Move	r1, r0
	ILessThan	r0, r2, r8
	BranchIf	.LBB4_21, r0
	IGreaterThanEqual	r0, r2, r5
	BranchIf	.LBB4_21, r0
	IMulImm	r0, r2, 24
	LoadU64	r2, r3
	IAdd	r0, r2, r0
	IAddImm	r0, r0, 8
	LoadU64	r5, r0
	LoadStaticU64Imm	r8, 5
	LoadStaticU64Imm	r9, 4
	LoadU16Imm	r10, 0
	Move	r49, r8
	Move	r50, r9
	Move	r51, r10
	CallFf	r0, r4, 3
	Move	r3, r0
	CallFf	r0, r4, 3
	Move	r2, r0
	CallFf	r0, r4, 3
	Move	r4, r0
	LoadStaticU64Imm	r0, 11
	Move	r49, r0
	Move	r50, r6
	CallFf	r0, r7, 2
	LookupPointer	r8, 8
	Move	r49, r3
	Move	r50, r0
	CallFf	r0, r8, 3
	LoadStaticU64Imm	r0, 10
	Move	r49, r0
	Move	r50, r6
	CallFf	r0, r7, 2
	Move	r49, r2
	Move	r50, r0
	CallFf	r0, r8, 3
	LoadStaticU64Imm	r0, 9
	Move	r49, r0
	Move	r50, r6
	CallFf	r0, r7, 2
	Move	r49, r4
	Move	r50, r0
	CallFf	r0, r8, 3
	LookupPointer	r4, 7
	Move	r49, r3
	CallFf	r0, r4, 1
	BranchIfNot	.LBB4_21, r0
	Move	r49, r2
	CallFf	r0, r4, 1
	BranchIf	.LBB4_6, r0
	LoadU16Imm	r0, 0
	Move	r49, r2
	Move	r50, r3
	Move	r51, r0
	CallFf	r0, r8, 3
.LBB4_6:
	LoadStaticU64Imm	r0, 15
	LoadU16Imm	r3, 1
	LookupPointer	r7, 13
	Move	r49, r0
	Move	r50, r3
	CallFf	r0, r7, 2
	Move	r3, r0
	Move	r49, r2
	CallFf	r0, r4, 1
	Move	r4, r0
	LoadStaticU64Imm	r0, 14
	LookupPointer	r7, 12
	Move	r49, r0
	Move	r50, r6
	CallFf	r0, r7, 2
	LoadU16Imm	r6, 0
	LoadU16Imm	r7, 0
	BranchIfNot	.LBB4_8, r0
	IMod	r7, r3, r4
.LBB4_8:
	LookupPointer	r0, 16
	Move	r49, r2
	Move	r50, r7
	CallFf	r0, r0, 2
	BranchIfNot	.LBB4_15, r0
	IAddImm	r2, r0, 12
	LoadU16	r2, r2
	LoadU16Imm	r3, 1
	INotEqual	r3, r2, r3
	BranchIf	.LBB4_22, r3
	LoadU8	r6, r0
	Branch	.LBB4_15
.LBB4_20:
	LoadStaticU64Imm	r0, 5
	LoadStaticU64Imm	r1, 4
	LoadU16Imm	r2, 0
	LookupPointer	r3, 2
	Move	r49, r0
	Move	r50, r1
	Move	r51, r2
	CallFf	r0, r3, 3
	Move	r1, r0
.LBB4_21:
	Move	r0, r1
	Return	r0
.LBB4_22:
	LoadU16Imm	r3, 2
	IEqual	r3, r2, r3
	BranchIfNot	.LBB4_23, r3
	LoadI32	r6, r0
	Branch	.LBB4_15
.LBB4_23:
	LoadU16Imm	r3, 3
	IEqual	r3, r2, r3
	BranchIfNot	.LBB4_24, r3
	LoadU32	r6, r0
	Branch	.LBB4_15
.LBB4_24:
	LoadU16Imm	r3, 4
	IEqual	r3, r2, r3
	BranchIf	.LBB4_13, r3
	LoadU16Imm	r3, 5
	IEqual	r3, r2, r3
	BranchIfNot	.LBB4_26, r3
.LBB4_13:
	LoadU64	r6, r0
.LBB4_15:
	IMulImm	r0, r6, 8
	LoadU64	r2, r5
	IAdd	r0, r2, r0
	LoadU64	r2, r0
	IAddImm	r3, r2, 8
	LoadI32	r0, r3
	LoadU16Imm	r4, 1
	ILessThan	r0, r0, r4
	BranchIf	.LBB4_21, r0
	LoadU16Imm	r4, 0
	LookupPointer	r5, 17
	LookupPointer	r6, 18
	LookupPointer	r7, 19
	LoadU16Imm	r8, 7
	LoadU16Imm	r9, 0
	Branch	.LBB4_17
.LBB4_19:
	IAddImm	r9, r9, 1
	LoadI32	r0, r3
	ILessThan	r0, r9, r0
	BranchIfNot	.LBB4_21, r0
.LBB4_17:
	IMulImm	r0, r9, 16
	LoadU64	r10, r2
	IAdd	r0, r10, r0
	LoadU64	r10, r0
	Move	r49, r0
	Call	r0, r7, 1
	BranchIfNot	.LBB4_19, r0
	Move	r49, r8
	Move	r50, r10
	CallFf	r0, r6, 2
	Move	r49, r1
	Move	r50, r0
	Move	r51, r4
	CallFf	r0, r5, 3
	Branch	.LBB4_19
.LBB4_26:
	LoadU16Imm	r3, 6
	IEqual	r2, r2, r3
	BranchIfNot	.LBB4_15, r2
	LoadFloat	r0, r0
	CastInteger	r6, r0
	Branch	.LBB4_15
"wave-manager-get-next-spawning-wave-npc-schemas-group_symbol_table":
	.quad	-1136051897790519866
	.quad	-8364948414460296332
	.quad	-3193965096093911673
	.quad	-8623127435633610462
	.quad	-1
	.quad	8534565298598421705
	.quad	-6711785430563033859
	.quad	4013010305452811590
	.quad	7920520547792940799
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
"wave-manager-get-next-spawning-wave-npc-schemas-group_pointer_map":
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
	.byte	0
.Lfunc_end4:
	.size	"wave-manager-get-next-spawning-wave-npc-schemas-group", .Lfunc_end4-"wave-manager-get-next-spawning-wave-npc-schemas-group"

	.globl	"wave-manager-get-total-wave-count"
	.type	"wave-manager-get-total-wave-count",@function
"wave-manager-get-total-wave-count":
	Move	r1, r49
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	BranchIf	.LBB5_2, r0
	LookupPointer	r0, 3
	Move	r49, r1
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r2, 4
	LookupPointer	r3, 2
	Move	r49, r2
	Move	r50, r0
	CallFf	r0, r3, 2
	LookupPointer	r2, 1
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB5_2:
	LoadStaticU64Imm	r0, 6
	LookupPointer	r2, 5
	Move	r49, r0
	Move	r50, r1
	CallFf	r0, r2, 2
	BranchIf	.LBB5_4, r0
	LookupPointer	r0, 3
	Move	r49, r1
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r2, 7
	LookupPointer	r3, 2
	Move	r49, r2
	Move	r50, r0
	CallFf	r0, r3, 2
	LookupPointer	r2, 1
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB5_4:
	LoadStaticU64Imm	r0, 9
	LookupPointer	r2, 8
	Move	r49, r0
	Move	r50, r1
	CallFf	r0, r2, 2
	Return	r0
"wave-manager-get-total-wave-count_symbol_table":
	.quad	-6870362608876642110
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	-3311351115081502130
	.quad	13632
	.quad	54185015286451590
	.quad	-4142738284031958410
	.quad	13350
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
	.byte	1
	.byte	0
	.byte	0
.Lfunc_end5:
	.size	"wave-manager-get-total-wave-count", .Lfunc_end5-"wave-manager-get-total-wave-count"

	.globl	"wave-manager-get-num-waves-spawned"
	.type	"wave-manager-get-num-waves-spawned",@function
"wave-manager-get-num-waves-spawned":
	Move	r0, r49
	LoadStaticU64Imm	r1, 2
	LookupPointer	r2, 1
	Move	r49, r1
	Move	r50, r0
	CallFf	r0, r2, 2
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
.Lfunc_end6:
	.size	"wave-manager-get-num-waves-spawned", .Lfunc_end6-"wave-manager-get-num-waves-spawned"

	.globl	"wave-manager-get-next-spawning-wave-spawn-configs"
	.type	"wave-manager-get-next-spawning-wave-spawn-configs",@function
"wave-manager-get-next-spawning-wave-spawn-configs":
	Move	r2, r49
	LoadStaticU64Imm	r0, 2
	LookupPointer	r3, 0
	Move	r49, r0
	Move	r50, r2
	CallFf	r0, r3, 2
	Move	r1, r0
	LoadStaticU64Imm	r0, 1
	Move	r49, r0
	CallFf	r0, r3, 2
	BranchIfNot	.LBB7_2, r0
	IAddImm	r0, r0, 24
	LoadU64	r0, r0
	BranchIfNot	.LBB7_2, r0
	Return	r0
.LBB7_2:
	BranchIfNot	.LBB7_3, r1
	IAddImm	r0, r1, 16
	LoadU64	r0, r0
	Return	r0
.LBB7_3:
	LoadU16Imm	r0, 0
	Return	r0
"wave-manager-get-next-spawning-wave-spawn-configs_symbol_table":
	.quad	-1136051897790519866
	.quad	-3630726564894351738
	.quad	-8364948414460296332
"wave-manager-get-next-spawning-wave-spawn-configs_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end7:
	.size	"wave-manager-get-next-spawning-wave-spawn-configs", .Lfunc_end7-"wave-manager-get-next-spawning-wave-spawn-configs"

	.globl	"wave-manager-get-next-spawning-wave-npc-count"
	.type	"wave-manager-get-next-spawning-wave-npc-count",@function
"wave-manager-get-next-spawning-wave-npc-count":
	LookupPointer	r1, 1
	Call	r0, r1, 1
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
.Lfunc_end8:
	.size	"wave-manager-get-next-spawning-wave-npc-count", .Lfunc_end8-"wave-manager-get-next-spawning-wave-npc-count"

	.globl	"wave-manager-spawn-next-wave!"
	.type	"wave-manager-spawn-next-wave!",@function
"wave-manager-spawn-next-wave!":
	Move	r1, r49
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	BranchIf	.LBB9_2, r0
	LookupPointer	r0, 3
	Move	r49, r1
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r2, 4
	LookupPointer	r3, 2
	Move	r49, r2
	Move	r50, r0
	CallFf	r0, r3, 2
	LookupPointer	r2, 1
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB9_2:
	LoadStaticU64Imm	r0, 6
	LookupPointer	r2, 5
	Move	r49, r0
	Move	r50, r1
	CallFf	r0, r2, 2
	BranchIfNot	.LBB9_4, r0
	LoadStaticU64Imm	r0, 8
	LookupPointer	r2, 7
	Move	r49, r0
	Move	r50, r1
	CallFf	r0, r2, 2
	Return	r0
.LBB9_4:
	LookupPointer	r0, 3
	Move	r49, r1
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r1, 9
	LookupPointer	r2, 2
	Move	r49, r1
	Move	r50, r0
	CallFf	r0, r2, 2
	LookupPointer	r1, 1
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
"wave-manager-spawn-next-wave!_symbol_table":
	.quad	-6870362608876642110
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	-3311351115081502130
	.quad	13632
	.quad	54185015286451590
	.quad	-4142738284031958410
	.quad	4789555171397753330
	.quad	5189346610966935392
	.quad	13535
"wave-manager-spawn-next-wave!_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
.Lfunc_end9:
	.size	"wave-manager-spawn-next-wave!", .Lfunc_end9-"wave-manager-spawn-next-wave!"

	.globl	"wave-manager-get-npc-alive-group"
	.type	"wave-manager-get-npc-alive-group",@function
"wave-manager-get-npc-alive-group":
	LoadStaticU64Imm	r0, 5
	LoadStaticU64Imm	r1, 4
	LoadU16Imm	r3, 0
	LookupPointer	r2, 2
	Move	r49, r0
	Move	r50, r1
	Move	r51, r3
	CallFf	r0, r2, 3
	Move	r1, r0
	LoadStaticU64Imm	r0, 3
	LookupPointer	r2, 1
	Move	r49, r0
	CallFf	r0, r2, 1
	Move	r2, r0
	LookupPointer	r0, 0
	CallFf	r0, r0, 0
	LoadU16Imm	r0, 1
	ILessThan	r0, r2, r0
	BranchIf	.LBB10_8, r0
	LookupPointer	r4, 6
	LookupPointer	r5, 7
	LookupPointer	r6, 8
	LookupPointer	r7, 9
	LoadStaticU64Imm	r8, 3
	LoadU16Imm	r9, 7
	LoadU16Imm	r10, 1
	LoadU16Imm	r11, 5
	LoadU16Imm	r12, 4
	Branch	.LBB10_2
.LBB10_7:
	IAddImm	r3, r3, 1
	ILessThan	r0, r3, r2
	BranchIfNot	.LBB10_8, r0
.LBB10_2:
	Move	r49, r8
	Move	r50, r3
	CallFf	r0, r7, 2
	LoadU16Imm	r13, 0
	BranchIfNot	.LBB10_5, r0
	IAddImm	r14, r0, 12
	LoadU16	r14, r14
	INotEqual	r15, r14, r9
	BranchIfNot	.LBB10_4, r15
	IEqual	r15, r14, r11
	BranchIf	.LBB10_4, r15
	IEqual	r14, r14, r12
	BranchIfNot	.LBB10_5, r14
.LBB10_4:
	LoadU64	r13, r0
.LBB10_5:
	Move	r49, r13
	CallFf	r0, r6, 1
	BranchIfNot	.LBB10_7, r0
	Move	r49, r9
	Move	r50, r13
	CallFf	r0, r5, 2
	Move	r49, r1
	Move	r50, r0
	Move	r51, r10
	CallFf	r0, r4, 3
	Branch	.LBB10_7
.LBB10_8:
	LookupPointer	r0, 10
	CallFf	r0, r0, 0
	Move	r0, r1
	Return	r0
"wave-manager-get-npc-alive-group_symbol_table":
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
"wave-manager-get-npc-alive-group_pointer_map":
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
.Lfunc_end10:
	.size	"wave-manager-get-npc-alive-group", .Lfunc_end10-"wave-manager-get-npc-alive-group"

	.globl	"wave-manager-kill-all-npcs"
	.type	"wave-manager-kill-all-npcs",@function
"wave-manager-kill-all-npcs":
	LookupPointer	r1, 0
	Call	r0, r1, 1
	BranchIfNot	.LBB11_1, r0
	LookupPointer	r3, 3
	Call	r0, r3, 1
	Move	r1, r0
	LookupPointer	r0, 2
	Move	r49, r1
	CallFf	r0, r0, 1
	Move	r2, r0
	LookupPointer	r0, 1
	CallFf	r0, r0, 0
	LoadU16Imm	r0, 1
	ILessThan	r0, r2, r0
	BranchIf	.LBB11_8, r0
	LoadU16Imm	r4, 0
	LookupPointer	r5, 4
	LookupPointer	r6, 5
	LoadU16Imm	r7, 2
	LoadU16Imm	r8, 7
	LoadU16Imm	r9, 5
	LoadU16Imm	r10, 4
	LoadU16Imm	r11, 0
	Branch	.LBB11_4
.LBB11_6:
	LoadU64	r12, r0
.LBB11_7:
	Move	r49, r12
	Move	r50, r7
	Move	r51, r4
	Move	r52, r4
	Move	r53, r4
	CallFf	r0, r5, 5
	IAddImm	r11, r11, 1
	ILessThan	r0, r11, r2
	BranchIfNot	.LBB11_8, r0
.LBB11_4:
	Move	r49, r1
	Move	r50, r11
	CallFf	r0, r6, 2
	LoadU16Imm	r12, 0
	BranchIfNot	.LBB11_7, r0
	IAddImm	r12, r0, 12
	LoadU16	r12, r12
	INotEqual	r13, r12, r8
	BranchIfNot	.LBB11_6, r13
	IEqual	r13, r12, r9
	BranchIf	.LBB11_6, r13
	IEqual	r13, r12, r10
	LoadU16Imm	r12, 0
	BranchIf	.LBB11_6, r13
	Branch	.LBB11_7
.LBB11_8:
	LookupPointer	r0, 7
	CallFf	r0, r0, 0
	Call	r0, r3, 1
	LoadU16Imm	r1, 0
	LookupPointer	r2, 6
	Move	r49, r0
	Move	r50, r1
	Call	r0, r2, 2
	Return	r0
.LBB11_1:
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
.Lfunc_end11:
	.size	"wave-manager-kill-all-npcs", .Lfunc_end11-"wave-manager-kill-all-npcs"

	.globl	"wave-manager-get-lost-player-npc-group"
	.type	"wave-manager-get-lost-player-npc-group",@function
"wave-manager-get-lost-player-npc-group":
	LookupPointer	r0, 3
	Call	r0, r0, 1
	Move	r1, r0
	LoadStaticU64Imm	r0, 5
	LoadStaticU64Imm	r2, 4
	LoadU16Imm	r5, 0
	LookupPointer	r3, 2
	Move	r49, r0
	Move	r50, r2
	Move	r51, r5
	CallFf	r0, r3, 3
	Move	r2, r0
	LookupPointer	r0, 1
	Move	r49, r1
	CallFf	r0, r0, 1
	Move	r3, r0
	LookupPointer	r0, 0
	CallFf	r0, r0, 0
	LoadU16Imm	r0, 1
	ILessThan	r0, r3, r0
	BranchIf	.LBB12_10, r0
	LookupPointer	r6, 6
	LookupPointer	r7, 7
	LookupPointer	r8, 8
	LookupPointer	r9, 9
	LookupPointer	r10, 10
	LookupPointer	r11, 11
	LoadStaticU64Imm	r12, 12
	LoadStaticU64Imm	r13, 13
	LoadU16Imm	r14, 2
	LoadU16Imm	r15, 7
	LoadU16Imm	r16, 1
	LoadU16Imm	r17, 5
	LoadU16Imm	r18, 4
	Branch	.LBB12_2
.LBB12_9:
	IAddImm	r5, r5, 1
	ILessThan	r0, r5, r3
	BranchIfNot	.LBB12_10, r0
.LBB12_2:
	Move	r49, r1
	Move	r50, r5
	CallFf	r0, r11, 2
	LoadU16Imm	r19, 0
	BranchIfNot	.LBB12_5, r0
	IAddImm	r4, r0, 12
	LoadU16	r4, r4
	INotEqual	r20, r4, r15
	BranchIfNot	.LBB12_4, r20
	IEqual	r20, r4, r17
	BranchIf	.LBB12_4, r20
	IEqual	r4, r4, r18
	BranchIfNot	.LBB12_5, r4
.LBB12_4:
	LoadU64	r19, r0
.LBB12_5:
	Move	r49, r19
	Move	r50, r12
	CallFf	r0, r10, 2
	Move	r4, r0
	CallFf	r0, r9, 1
	BranchIfNot	.LBB12_9, r0
	Move	r49, r19
	Call	r0, r8, 1
	BranchIf	.LBB12_9, r0
	OpBitAnd	r0, r4, r13
	IEqual	r0, r0, r14
	BranchIf	.LBB12_9, r0
	Move	r49, r15
	Move	r50, r19
	CallFf	r0, r7, 2
	Move	r49, r2
	Move	r50, r0
	Move	r51, r16
	CallFf	r0, r6, 3
	Branch	.LBB12_9
.LBB12_10:
	LookupPointer	r0, 14
	CallFf	r0, r0, 0
	Move	r0, r2
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
	.quad	-7707884455580369866
	.quad	-4433807632111512251
	.quad	-5833053787651694927
	.quad	-2664920944689591574
	.quad	5008278420455340480
	.quad	-2
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
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end12:
	.size	"wave-manager-get-lost-player-npc-group", .Lfunc_end12-"wave-manager-get-lost-player-npc-group"

	.globl	"wave-manager-stop!"
	.type	"wave-manager-stop!",@function
"wave-manager-stop!":
	Move	r1, r49
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	BranchIf	.LBB13_2, r0
	LookupPointer	r0, 3
	Move	r49, r1
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r2, 4
	LookupPointer	r3, 2
	Move	r49, r2
	Move	r50, r0
	CallFf	r0, r3, 2
	LookupPointer	r2, 1
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB13_2:
	LoadStaticU64Imm	r0, 6
	LookupPointer	r2, 5
	Move	r49, r0
	Move	r50, r1
	CallFf	r0, r2, 2
	BranchIfNot	.LBB13_4, r0
	LoadStaticU64Imm	r0, 8
	LookupPointer	r2, 7
	Move	r49, r0
	Move	r50, r1
	CallFf	r0, r2, 2
	Return	r0
.LBB13_4:
	LookupPointer	r0, 3
	Move	r49, r1
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r1, 9
	LookupPointer	r2, 2
	Move	r49, r1
	Move	r50, r0
	CallFf	r0, r2, 2
	LookupPointer	r1, 1
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
"wave-manager-stop!_symbol_table":
	.quad	-6870362608876642110
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	-3311351115081502130
	.quad	13632
	.quad	54185015286451590
	.quad	-4142738284031958410
	.quad	4789555171397753330
	.quad	4737644804400110462
	.quad	13589
"wave-manager-stop!_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
.Lfunc_end13:
	.size	"wave-manager-stop!", .Lfunc_end13-"wave-manager-stop!"

	.globl	"wave-manager-get-npc-alive-count"
	.type	"wave-manager-get-npc-alive-count",@function
"wave-manager-get-npc-alive-count":
	Move	r1, r49
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	BranchIf	.LBB14_2, r0
	LookupPointer	r0, 3
	Move	r49, r1
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r2, 4
	LookupPointer	r3, 2
	Move	r49, r2
	Move	r50, r0
	CallFf	r0, r3, 2
	LookupPointer	r2, 1
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB14_2:
	LoadStaticU64Imm	r0, 6
	LookupPointer	r2, 5
	Move	r49, r0
	Move	r50, r1
	CallFf	r0, r2, 2
	BranchIf	.LBB14_4, r0
	LookupPointer	r0, 3
	Move	r49, r1
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r1, 7
	LookupPointer	r2, 2
	Move	r49, r1
	Move	r50, r0
	CallFf	r0, r2, 2
	LookupPointer	r1, 1
	Move	r49, r0
	CallFf	r0, r1, 1
.LBB14_4:
	LoadStaticU64Imm	r0, 9
	LookupPointer	r1, 8
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
"wave-manager-get-npc-alive-count_symbol_table":
	.quad	-6870362608876642110
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	-3311351115081502130
	.quad	13632
	.quad	54185015286451590
	.quad	-4142738284031958410
	.quad	13293
	.quad	1564617029140756692
	.quad	-8578220443451849366
"wave-manager-get-npc-alive-count_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	0
	.byte	0
	.byte	1
	.byte	0
	.byte	0
.Lfunc_end14:
	.size	"wave-manager-get-npc-alive-count", .Lfunc_end14-"wave-manager-get-npc-alive-count"

	.globl	"wave-manager-get-next-wave-faction"
	.type	"wave-manager-get-next-wave-faction",@function
"wave-manager-get-next-wave-faction":
	Move	r2, r49
	LoadStaticU64Imm	r0, 6
	LookupPointer	r1, 1
	Move	r49, r0
	Move	r50, r2
	CallFf	r0, r1, 2
	Move	r1, r0
	LoadStaticU64Imm	r0, 3
	OpBitXor	r0, r1, r0
	LoadStaticU64Imm	r3, 2
	ILessThan	r0, r0, r3
	BranchIf	.LBB15_2, r0
	LoadStaticU64Imm	r1, 4
.LBB15_2:
	LoadStaticU64Imm	r0, 5
	LookupPointer	r3, 0
	Move	r49, r0
	Move	r50, r2
	CallFf	r0, r3, 2
	LoadU16Imm	r2, 0
	INotEqual	r0, r0, r2
	BranchIf	.LBB15_4, r0
	LoadStaticU64Imm	r1, 4
.LBB15_4:
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
.Lfunc_end15:
	.size	"wave-manager-get-next-wave-faction", .Lfunc_end15-"wave-manager-get-next-wave-faction"

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
.Lfunc_end16:
	.size	"wave-manager-get-lost-player-npc-group-count", .Lfunc_end16-"wave-manager-get-lost-player-npc-group-count"

	.globl	"wave-manager-spawn-next-special-wave!"
	.type	"wave-manager-spawn-next-special-wave!",@function
"wave-manager-spawn-next-special-wave!":
	Move	r1, r49
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	BranchIf	.LBB17_2, r0
	LookupPointer	r0, 3
	Move	r49, r1
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r2, 4
	LookupPointer	r3, 2
	Move	r49, r2
	Move	r50, r0
	CallFf	r0, r3, 2
	LookupPointer	r2, 1
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB17_2:
	LoadStaticU64Imm	r0, 6
	LookupPointer	r2, 5
	Move	r49, r0
	Move	r50, r1
	CallFf	r0, r2, 2
	BranchIfNot	.LBB17_4, r0
	LoadStaticU64Imm	r0, 8
	LookupPointer	r2, 7
	Move	r49, r0
	Move	r50, r1
	CallFf	r0, r2, 2
	Return	r0
.LBB17_4:
	LookupPointer	r0, 3
	Move	r49, r1
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r1, 9
	LookupPointer	r2, 2
	Move	r49, r1
	Move	r50, r0
	CallFf	r0, r2, 2
	LookupPointer	r1, 1
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
"wave-manager-spawn-next-special-wave!_symbol_table":
	.quad	-6870362608876642110
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	-3311351115081502130
	.quad	13632
	.quad	54185015286451590
	.quad	-4142738284031958410
	.quad	4789555171397753330
	.quad	1354182575228257266
	.quad	13473
"wave-manager-spawn-next-special-wave!_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
.Lfunc_end17:
	.size	"wave-manager-spawn-next-special-wave!", .Lfunc_end17-"wave-manager-spawn-next-special-wave!"

	.globl	"wave-manager-current-task-in-run?"
	.type	"wave-manager-current-task-in-run?",@function
"wave-manager-current-task-in-run?":
	LoadStaticU64Imm	r0, 2
	LoadStaticU64Imm	r1, 1
	LookupPointer	r2, 0
	Move	r49, r0
	Move	r50, r1
	CallFf	r0, r2, 2
	Return	r0
"wave-manager-current-task-in-run?_symbol_table":
	.quad	54185015286451590
	.quad	3766070110510549571
	.quad	-5700187185490357332
"wave-manager-current-task-in-run?_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end18:
	.size	"wave-manager-current-task-in-run?", .Lfunc_end18-"wave-manager-current-task-in-run?"

	.globl	"wave-manager-get-next-spawning-wave-composition"
	.type	"wave-manager-get-next-spawning-wave-composition",@function
"wave-manager-get-next-spawning-wave-composition":
	Move	r0, r49
	LoadStaticU64Imm	r1, 1
	LookupPointer	r2, 0
	Move	r49, r1
	Move	r50, r0
	CallFf	r0, r2, 2
	Return	r0
"wave-manager-get-next-spawning-wave-composition_symbol_table":
	.quad	-1136051897790519866
	.quad	-8364948414460296332
"wave-manager-get-next-spawning-wave-composition_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end19:
	.size	"wave-manager-get-next-spawning-wave-composition", .Lfunc_end19-"wave-manager-get-next-spawning-wave-composition"

	.globl	"wave-manager-has-requested-wave-spawned?"
	.type	"wave-manager-has-requested-wave-spawned?",@function
"wave-manager-has-requested-wave-spawned?":
	Move	r1, r49
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	BranchIf	.LBB20_2, r0
	LookupPointer	r0, 3
	Move	r49, r1
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r2, 4
	LookupPointer	r3, 2
	Move	r49, r2
	Move	r50, r0
	CallFf	r0, r3, 2
	LookupPointer	r2, 1
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB20_2:
	LoadStaticU64Imm	r0, 6
	LookupPointer	r2, 5
	Move	r49, r0
	Move	r50, r1
	CallFf	r0, r2, 2
	BranchIf	.LBB20_4, r0
	LookupPointer	r0, 3
	Move	r49, r1
	CallFf	r0, r0, 1
	LoadStaticPointerImm	r3, 7
	LookupPointer	r4, 2
	Move	r49, r3
	Move	r50, r0
	CallFf	r0, r4, 2
	LookupPointer	r3, 1
	Move	r49, r0
	CallFf	r0, r3, 1
.LBB20_4:
	LoadStaticU64Imm	r0, 8
	Move	r49, r0
	Move	r50, r1
	CallFf	r0, r2, 2
	BranchIfNot	.LBB20_5, r0
	LoadStaticU64Imm	r0, 9
	Move	r49, r0
	Move	r50, r1
	CallFf	r0, r2, 2
	Return	r0
.LBB20_5:
	LoadU16Imm	r0, 0
	Return	r0
"wave-manager-has-requested-wave-spawned?_symbol_table":
	.quad	-6870362608876642110
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	-3311351115081502130
	.quad	13632
	.quad	54185015286451590
	.quad	-4142738284031958410
	.quad	13408
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
	.byte	1
	.byte	0
	.byte	0
.Lfunc_end20:
	.size	"wave-manager-has-requested-wave-spawned?", .Lfunc_end20-"wave-manager-has-requested-wave-spawned?"

	.globl	"wave-manager-npc-lost-player?"
	.type	"wave-manager-npc-lost-player?",@function
"wave-manager-npc-lost-player?":
	Move	r3, r49
	LoadStaticU64Imm	r0, 2
	LookupPointer	r1, 1
	Move	r50, r0
	CallFf	r0, r1, 2
	Move	r2, r0
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	LoadU16Imm	r1, 0
	BranchIfNot	.LBB21_3, r0
	LookupPointer	r0, 3
	Move	r49, r3
	Call	r0, r0, 1
	BranchIfNot	.LBB21_2, r0
.LBB21_3:
	Move	r0, r1
	Return	r0
.LBB21_2:
	LoadStaticU64Imm	r0, 4
	IAdd	r0, r2, r0
	LoadStaticU64Imm	r1, 5
	ILessThan	r1, r0, r1
	Move	r0, r1
	Return	r0
"wave-manager-npc-lost-player?_symbol_table":
	.quad	-4433807632111512251
	.quad	-5833053787651694927
	.quad	5008278420455340480
	.quad	-7707884455580369866
	.quad	9223372036854775804
	.quad	9223372036854775806
"wave-manager-npc-lost-player?_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end21:
	.size	"wave-manager-npc-lost-player?", .Lfunc_end21-"wave-manager-npc-lost-player?"

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
	.quad	2138302561965
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
	.quad	2138302562304
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
	.quad	2138302561904
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
	.quad	2138302562429
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
	.quad	2138302562358
	.quad	-3311351115081502130
	.quad	2138302562379
	.size	"wave-manager-get-next-spawning-wave-npc-schemas-group_symbol_table_src", 216

	.type	"wave-manager-get-total-wave-count_symbol_table_src",@object
	.section	".rodata.wave-manager-get-total-wave-count_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wave-manager-get-total-wave-count_symbol_table_src":
	.quad	998391088342721072
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	2138302562022
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
	.quad	2138302562207
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
	.quad	2138302562261
	.quad	-3311351115081502130
	.size	"wave-manager-stop!_symbol_table_src", 56

	.type	"wave-manager-get-npc-alive-count_symbol_table_src",@object
	.section	".rodata.wave-manager-get-npc-alive-count_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wave-manager-get-npc-alive-count_symbol_table_src":
	.quad	998391088342721072
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	2138302561965
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
	.quad	2138302562145
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
	.quad	2138302562080
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

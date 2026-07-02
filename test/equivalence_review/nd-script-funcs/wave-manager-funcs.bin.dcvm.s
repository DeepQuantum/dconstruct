	.file	"wave-manager-funcs.bin"
	.text
	.globl	"wave-manager-get-npc-alive-count-by-faction"
	.type	"wave-manager-get-npc-alive-count-by-faction",@function
"wave-manager-get-npc-alive-count-by-faction":
	Move	r2, r50
	LookupPointer	r0, 2
	Call	r0, r0, 1
	Move	r3, r0
	LookupPointer	r4, 1
	Move	r49, r3
	CallFf	r0, r4, 1
	Move	r1, r0
	CallFf	r0, r4, 1
	Move	r4, r0
	LookupPointer	r0, 0
	CallFf	r0, r0, 0
	LoadU16Imm	r0, 1
	ILessThan	r0, r4, r0
	BranchIf	.LBB0_6, r0
	LoadU16Imm	r5, 0
	LookupPointer	r6, 3
	LookupPointer	r7, 4
	LoadU16Imm	r8, 7
	LoadU16Imm	r9, 5
	LoadU16Imm	r10, 4
	LoadU16Imm	r51, 0
	LoadU16Imm	r52, 0
	Branch	.LBB0_2
.LBB0_4:
	LoadU64	r49, r0
.LBB0_5:
	LoadStaticU64Imm	r50, 5
	CallFf	r0, r6, 4
	INotEqual	r0, r2, r0
	ISub	r1, r1, r0
	IAddImm	r5, r5, 1
	ILessThan	r0, r5, r4
	BranchIfNot	.LBB0_6, r0
.LBB0_2:
	Move	r49, r3
	Move	r50, r5
	CallFf	r0, r7, 2
	LoadU16Imm	r49, 0
	BranchIfNot	.LBB0_5, r0
	IAddImm	r11, r0, 12
	LoadU16	r11, r11
	INotEqual	r12, r11, r8
	BranchIfNot	.LBB0_4, r12
	IEqual	r12, r11, r9
	BranchIf	.LBB0_4, r12
	IEqual	r11, r11, r10
	BranchIf	.LBB0_4, r11
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
	LookupPointer	r2, 1
	LookupPointer	r3, 2
	LookupPointer	r4, 3
	BranchIf	.LBB1_2, r0
	Move	r49, r1
	CallFf	r0, r4, 1
	LoadStaticPointerImm	r49, 4
	Move	r50, r0
	CallFf	r0, r3, 2
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB1_2:
	LookupPointer	r0, 5
	LoadStaticU64Imm	r49, 6
	Move	r50, r1
	CallFf	r0, r0, 2
	BranchIf	.LBB1_4, r0
	Move	r49, r1
	CallFf	r0, r4, 1
	LoadStaticPointerImm	r49, 7
	Move	r50, r0
	CallFf	r0, r3, 2
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB1_4:
	LookupPointer	r0, 8
	LoadStaticU64Imm	r49, 9
	CallFf	r0, r0, 1
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
	LookupPointer	r2, 2
	LoadStaticPointerImm	r49, 4
	Move	r50, r0
	CallFf	r0, r2, 2
	LookupPointer	r2, 1
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB2_2:
	LookupPointer	r0, 5
	LoadStaticU64Imm	r49, 6
	Move	r50, r1
	CallFf	r0, r0, 2
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
	LookupPointer	r2, 1
	LookupPointer	r3, 2
	LookupPointer	r4, 3
	BranchIf	.LBB3_2, r0
	Move	r49, r1
	CallFf	r0, r4, 1
	LoadStaticPointerImm	r49, 4
	Move	r50, r0
	CallFf	r0, r3, 2
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB3_2:
	LookupPointer	r0, 5
	LoadStaticU64Imm	r49, 6
	Move	r50, r1
	CallFf	r0, r0, 2
	BranchIf	.LBB3_4, r0
	Move	r49, r1
	CallFf	r0, r4, 1
	LoadStaticPointerImm	r49, 7
	Move	r50, r0
	CallFf	r0, r3, 2
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB3_4:
	LookupPointer	r0, 8
	LoadStaticU64Imm	r49, 9
	Move	r50, r1
	CallFf	r0, r0, 2
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
	Move	r2, r49
	LookupPointer	r7, 0
	LoadStaticU64Imm	r49, 2
	Move	r50, r2
	CallFf	r0, r7, 2
	LookupPointer	r5, 1
	BranchIfNot	.LBB4_20, r0
	IAddImm	r0, r0, 32
	LoadU64	r4, r0
	IAddImm	r0, r4, 8
	LoadI32	r6, r0
	LookupPointer	r0, 3
	LoadStaticU64Imm	r49, 6
	Move	r50, r2
	CallFf	r0, r0, 2
	Move	r3, r0
	LoadU16Imm	r8, 0
	LoadStaticU64Imm	r49, 5
	LoadStaticU64Imm	r50, 4
	LoadU16Imm	r51, 0
	CallFf	r0, r5, 3
	Move	r1, r0
	ILessThan	r0, r3, r8
	BranchIf	.LBB4_21, r0
	IGreaterThanEqual	r0, r3, r6
	BranchIf	.LBB4_21, r0
	IMulImm	r0, r3, 24
	LoadU64	r3, r4
	IAdd	r0, r3, r0
	IAddImm	r0, r0, 8
	LoadU64	r6, r0
	CallFf	r0, r5, 3
	Move	r4, r0
	CallFf	r0, r5, 3
	Move	r3, r0
	CallFf	r0, r5, 3
	Move	r5, r0
	LoadStaticU64Imm	r49, 11
	Move	r50, r2
	CallFf	r0, r7, 2
	LookupPointer	r8, 8
	Move	r49, r4
	Move	r50, r0
	CallFf	r0, r8, 3
	LoadStaticU64Imm	r49, 10
	Move	r50, r2
	CallFf	r0, r7, 2
	Move	r49, r3
	Move	r50, r0
	CallFf	r0, r8, 3
	LoadStaticU64Imm	r49, 9
	Move	r50, r2
	CallFf	r0, r7, 2
	Move	r49, r5
	Move	r50, r0
	CallFf	r0, r8, 3
	LookupPointer	r5, 7
	Move	r49, r4
	CallFf	r0, r5, 1
	BranchIfNot	.LBB4_21, r0
	Move	r49, r3
	CallFf	r0, r5, 1
	BranchIf	.LBB4_6, r0
	Move	r49, r3
	Move	r50, r4
	CallFf	r0, r8, 3
.LBB4_6:
	LookupPointer	r0, 13
	LoadStaticU64Imm	r49, 15
	LoadU16Imm	r50, 1
	CallFf	r0, r0, 2
	Move	r4, r0
	Move	r49, r3
	CallFf	r0, r5, 1
	Move	r5, r0
	LookupPointer	r0, 12
	LoadStaticU64Imm	r49, 14
	Move	r50, r2
	CallFf	r0, r0, 2
	LoadU16Imm	r2, 0
	LoadU16Imm	r50, 0
	BranchIfNot	.LBB4_8, r0
	IMod	r50, r4, r5
.LBB4_8:
	LookupPointer	r0, 16
	Move	r49, r3
	CallFf	r0, r0, 2
	BranchIfNot	.LBB4_15, r0
	IAddImm	r3, r0, 12
	LoadU16	r3, r3
	LoadU16Imm	r4, 1
	INotEqual	r4, r3, r4
	BranchIf	.LBB4_22, r4
	LoadU8	r2, r0
	Branch	.LBB4_15
.LBB4_20:
	LoadStaticU64Imm	r49, 5
	LoadStaticU64Imm	r50, 4
	LoadU16Imm	r51, 0
	CallFf	r0, r5, 3
	Move	r1, r0
.LBB4_21:
	Move	r0, r1
	Return	r0
.LBB4_22:
	LoadU16Imm	r4, 2
	IEqual	r4, r3, r4
	BranchIfNot	.LBB4_23, r4
	LoadI32	r2, r0
	Branch	.LBB4_15
.LBB4_23:
	LoadU16Imm	r4, 3
	IEqual	r4, r3, r4
	BranchIfNot	.LBB4_24, r4
	LoadU32	r2, r0
	Branch	.LBB4_15
.LBB4_24:
	LoadU16Imm	r4, 4
	IEqual	r4, r3, r4
	BranchIf	.LBB4_13, r4
	LoadU16Imm	r4, 5
	IEqual	r4, r3, r4
	BranchIfNot	.LBB4_26, r4
.LBB4_13:
	LoadU64	r2, r0
.LBB4_15:
	IMulImm	r0, r2, 8
	LoadU64	r2, r6
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
	Branch	.LBB4_17
.LBB4_19:
	IAddImm	r4, r4, 1
	LoadI32	r0, r3
	ILessThan	r0, r4, r0
	BranchIfNot	.LBB4_21, r0
.LBB4_17:
	IMulImm	r0, r4, 16
	LoadU64	r8, r2
	IAdd	r49, r8, r0
	LoadU64	r50, r49
	Call	r0, r7, 1
	BranchIfNot	.LBB4_19, r0
	LoadU16Imm	r49, 7
	CallFf	r0, r6, 2
	Move	r49, r1
	Move	r50, r0
	CallFf	r0, r5, 3
	Branch	.LBB4_19
.LBB4_26:
	LoadU16Imm	r4, 6
	IEqual	r3, r3, r4
	BranchIfNot	.LBB4_15, r3
	LoadFloat	r0, r0
	CastInteger	r2, r0
	Branch	.LBB4_15
"wave-manager-get-next-spawning-wave-npc-schemas-group_symbol_table":
	.quad	-1136051897790519866
	.quad	-3193965096093911673
	.quad	-8364948414460296332
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
	LookupPointer	r2, 1
	LookupPointer	r3, 2
	LookupPointer	r4, 3
	BranchIf	.LBB5_2, r0
	Move	r49, r1
	CallFf	r0, r4, 1
	LoadStaticPointerImm	r49, 4
	Move	r50, r0
	CallFf	r0, r3, 2
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB5_2:
	LookupPointer	r0, 5
	LoadStaticU64Imm	r49, 6
	Move	r50, r1
	CallFf	r0, r0, 2
	BranchIf	.LBB5_4, r0
	Move	r49, r1
	CallFf	r0, r4, 1
	LoadStaticPointerImm	r49, 7
	Move	r50, r0
	CallFf	r0, r3, 2
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB5_4:
	LookupPointer	r0, 8
	LoadStaticU64Imm	r49, 9
	Move	r50, r1
	CallFf	r0, r0, 2
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
	Move	r50, r49
	LookupPointer	r0, 1
	LoadStaticU64Imm	r49, 2
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
.Lfunc_end6:
	.size	"wave-manager-get-num-waves-spawned", .Lfunc_end6-"wave-manager-get-num-waves-spawned"

	.globl	"wave-manager-get-next-spawning-wave-spawn-configs"
	.type	"wave-manager-get-next-spawning-wave-spawn-configs",@function
"wave-manager-get-next-spawning-wave-spawn-configs":
	Move	r50, r49
	LookupPointer	r2, 0
	LoadStaticU64Imm	r49, 2
	CallFf	r0, r2, 2
	Move	r1, r0
	LoadStaticU64Imm	r49, 1
	CallFf	r0, r2, 2
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
.Lfunc_end8:
	.size	"wave-manager-get-next-spawning-wave-npc-count", .Lfunc_end8-"wave-manager-get-next-spawning-wave-npc-count"

	.globl	"wave-manager-spawn-next-wave!"
	.type	"wave-manager-spawn-next-wave!",@function
"wave-manager-spawn-next-wave!":
	Move	r1, r49
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	LookupPointer	r2, 1
	LookupPointer	r3, 2
	LookupPointer	r4, 3
	BranchIf	.LBB9_2, r0
	Move	r49, r1
	CallFf	r0, r4, 1
	LoadStaticPointerImm	r49, 4
	Move	r50, r0
	CallFf	r0, r3, 2
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB9_2:
	LookupPointer	r0, 5
	LoadStaticU64Imm	r49, 6
	Move	r50, r1
	CallFf	r0, r0, 2
	BranchIfNot	.LBB9_4, r0
	LookupPointer	r0, 7
	LoadStaticU64Imm	r49, 8
	Move	r50, r1
	CallFf	r0, r0, 2
	Return	r0
.LBB9_4:
	Move	r49, r1
	CallFf	r0, r4, 1
	LoadStaticPointerImm	r49, 9
	Move	r50, r0
	CallFf	r0, r3, 2
	Move	r49, r0
	CallFf	r0, r2, 1
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
	LoadU16Imm	r1, 0
	LookupPointer	r0, 2
	LoadStaticU64Imm	r49, 5
	LoadStaticU64Imm	r50, 4
	LoadU16Imm	r51, 0
	CallFf	r0, r0, 3
	Move	r2, r0
	LookupPointer	r0, 1
	LoadStaticU64Imm	r49, 3
	CallFf	r0, r0, 1
	Move	r3, r0
	LookupPointer	r0, 0
	CallFf	r0, r0, 0
	LoadU16Imm	r0, 1
	ILessThan	r0, r3, r0
	BranchIf	.LBB10_8, r0
	LookupPointer	r4, 6
	LookupPointer	r5, 7
	LookupPointer	r6, 8
	LookupPointer	r7, 9
	LoadU16Imm	r8, 7
	LoadU16Imm	r9, 5
	LoadU16Imm	r10, 4
	LoadU16Imm	r51, 1
	Branch	.LBB10_2
.LBB10_7:
	IAddImm	r1, r1, 1
	ILessThan	r0, r1, r3
	BranchIfNot	.LBB10_8, r0
.LBB10_2:
	LoadStaticU64Imm	r49, 3
	Move	r50, r1
	CallFf	r0, r7, 2
	LoadU16Imm	r50, 0
	BranchIfNot	.LBB10_5, r0
	IAddImm	r11, r0, 12
	LoadU16	r11, r11
	INotEqual	r12, r11, r8
	BranchIfNot	.LBB10_4, r12
	IEqual	r12, r11, r9
	BranchIf	.LBB10_4, r12
	IEqual	r11, r11, r10
	BranchIfNot	.LBB10_5, r11
.LBB10_4:
	LoadU64	r50, r0
.LBB10_5:
	Move	r49, r50
	CallFf	r0, r6, 1
	BranchIfNot	.LBB10_7, r0
	LoadU16Imm	r49, 7
	CallFf	r0, r5, 2
	Move	r49, r2
	Move	r50, r0
	CallFf	r0, r4, 3
	Branch	.LBB10_7
.LBB10_8:
	LookupPointer	r0, 10
	CallFf	r0, r0, 0
	Move	r0, r2
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
	LookupPointer	r0, 0
	Call	r0, r0, 1
	BranchIfNot	.LBB11_1, r0
	LookupPointer	r4, 3
	Call	r0, r4, 1
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
	LoadU16Imm	r3, 0
	LookupPointer	r5, 4
	LookupPointer	r6, 5
	LoadU16Imm	r7, 7
	LoadU16Imm	r8, 5
	LoadU16Imm	r9, 4
	LoadU16Imm	r51, 0
	LoadU16Imm	r52, 0
	LoadU16Imm	r53, 0
	Branch	.LBB11_4
.LBB11_6:
	LoadU64	r49, r0
.LBB11_7:
	LoadU16Imm	r50, 2
	CallFf	r0, r5, 5
	IAddImm	r3, r3, 1
	ILessThan	r0, r3, r2
	BranchIfNot	.LBB11_8, r0
.LBB11_4:
	Move	r49, r1
	Move	r50, r3
	CallFf	r0, r6, 2
	LoadU16Imm	r49, 0
	BranchIfNot	.LBB11_7, r0
	IAddImm	r10, r0, 12
	LoadU16	r10, r10
	INotEqual	r11, r10, r7
	BranchIfNot	.LBB11_6, r11
	IEqual	r11, r10, r8
	BranchIf	.LBB11_6, r11
	IEqual	r10, r10, r9
	BranchIf	.LBB11_6, r10
	Branch	.LBB11_7
.LBB11_8:
	LookupPointer	r0, 7
	CallFf	r0, r0, 0
	Call	r0, r4, 1
	LookupPointer	r1, 6
	Move	r49, r0
	LoadU16Imm	r50, 0
	Call	r0, r1, 2
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
	LoadU16Imm	r2, 0
	LookupPointer	r0, 2
	LoadStaticU64Imm	r49, 5
	LoadStaticU64Imm	r50, 4
	LoadU16Imm	r51, 0
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
	BranchIf	.LBB12_10, r0
	LookupPointer	r7, 6
	LookupPointer	r8, 7
	LookupPointer	r9, 8
	LookupPointer	r10, 9
	LookupPointer	r11, 10
	LookupPointer	r12, 11
	LoadStaticU64Imm	r13, 13
	LoadU16Imm	r14, 2
	LoadU16Imm	r15, 7
	LoadU16Imm	r16, 5
	LoadU16Imm	r17, 4
	LoadU16Imm	r51, 1
	Branch	.LBB12_2
.LBB12_9:
	IAddImm	r2, r2, 1
	ILessThan	r0, r2, r4
	BranchIfNot	.LBB12_10, r0
.LBB12_2:
	Move	r49, r1
	Move	r50, r2
	CallFf	r0, r12, 2
	LoadU16Imm	r5, 0
	BranchIfNot	.LBB12_5, r0
	IAddImm	r6, r0, 12
	LoadU16	r6, r6
	INotEqual	r18, r6, r15
	BranchIfNot	.LBB12_4, r18
	IEqual	r18, r6, r16
	BranchIf	.LBB12_4, r18
	IEqual	r6, r6, r17
	BranchIfNot	.LBB12_5, r6
.LBB12_4:
	LoadU64	r5, r0
.LBB12_5:
	Move	r49, r5
	LoadStaticU64Imm	r50, 12
	CallFf	r0, r11, 2
	Move	r6, r0
	CallFf	r0, r10, 1
	BranchIfNot	.LBB12_9, r0
	Move	r49, r5
	Call	r0, r9, 1
	BranchIf	.LBB12_9, r0
	OpBitAnd	r0, r6, r13
	IEqual	r0, r0, r14
	BranchIf	.LBB12_9, r0
	LoadU16Imm	r49, 7
	Move	r50, r5
	CallFf	r0, r8, 2
	Move	r49, r3
	Move	r50, r0
	CallFf	r0, r7, 3
	Branch	.LBB12_9
.LBB12_10:
	LookupPointer	r0, 14
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
	LookupPointer	r2, 1
	LookupPointer	r3, 2
	LookupPointer	r4, 3
	BranchIf	.LBB13_2, r0
	Move	r49, r1
	CallFf	r0, r4, 1
	LoadStaticPointerImm	r49, 4
	Move	r50, r0
	CallFf	r0, r3, 2
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB13_2:
	LookupPointer	r0, 5
	LoadStaticU64Imm	r49, 6
	Move	r50, r1
	CallFf	r0, r0, 2
	BranchIfNot	.LBB13_4, r0
	LookupPointer	r0, 7
	LoadStaticU64Imm	r49, 8
	Move	r50, r1
	CallFf	r0, r0, 2
	Return	r0
.LBB13_4:
	Move	r49, r1
	CallFf	r0, r4, 1
	LoadStaticPointerImm	r49, 9
	Move	r50, r0
	CallFf	r0, r3, 2
	Move	r49, r0
	CallFf	r0, r2, 1
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
	LookupPointer	r2, 1
	LookupPointer	r3, 2
	LookupPointer	r4, 3
	BranchIf	.LBB14_2, r0
	Move	r49, r1
	CallFf	r0, r4, 1
	LoadStaticPointerImm	r49, 4
	Move	r50, r0
	CallFf	r0, r3, 2
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB14_2:
	LookupPointer	r0, 5
	LoadStaticU64Imm	r49, 6
	Move	r50, r1
	CallFf	r0, r0, 2
	BranchIf	.LBB14_4, r0
	Move	r49, r1
	CallFf	r0, r4, 1
	LoadStaticPointerImm	r49, 7
	Move	r50, r0
	CallFf	r0, r3, 2
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB14_4:
	LookupPointer	r0, 8
	LoadStaticU64Imm	r49, 9
	CallFf	r0, r0, 1
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
	Move	r50, r49
	LookupPointer	r0, 1
	LoadStaticU64Imm	r49, 6
	CallFf	r0, r0, 2
	Move	r1, r0
	LoadStaticU64Imm	r0, 3
	OpBitXor	r0, r1, r0
	LoadStaticU64Imm	r2, 2
	ILessThan	r0, r0, r2
	BranchIf	.LBB15_2, r0
	LoadStaticU64Imm	r1, 4
.LBB15_2:
	LookupPointer	r0, 0
	LoadStaticU64Imm	r49, 5
	CallFf	r0, r0, 2
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
	LookupPointer	r2, 1
	LookupPointer	r3, 2
	LookupPointer	r4, 3
	BranchIf	.LBB17_2, r0
	Move	r49, r1
	CallFf	r0, r4, 1
	LoadStaticPointerImm	r49, 4
	Move	r50, r0
	CallFf	r0, r3, 2
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB17_2:
	LookupPointer	r0, 5
	LoadStaticU64Imm	r49, 6
	Move	r50, r1
	CallFf	r0, r0, 2
	BranchIfNot	.LBB17_4, r0
	LookupPointer	r0, 7
	LoadStaticU64Imm	r49, 8
	Move	r50, r1
	CallFf	r0, r0, 2
	Return	r0
.LBB17_4:
	Move	r49, r1
	CallFf	r0, r4, 1
	LoadStaticPointerImm	r49, 9
	Move	r50, r0
	CallFf	r0, r3, 2
	Move	r49, r0
	CallFf	r0, r2, 1
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
	LookupPointer	r0, 0
	LoadStaticU64Imm	r49, 2
	LoadStaticU64Imm	r50, 1
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
.Lfunc_end18:
	.size	"wave-manager-current-task-in-run?", .Lfunc_end18-"wave-manager-current-task-in-run?"

	.globl	"wave-manager-get-next-spawning-wave-composition"
	.type	"wave-manager-get-next-spawning-wave-composition",@function
"wave-manager-get-next-spawning-wave-composition":
	Move	r50, r49
	LookupPointer	r0, 0
	LoadStaticU64Imm	r49, 1
	CallFf	r0, r0, 2
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
	LookupPointer	r2, 1
	LookupPointer	r3, 2
	LookupPointer	r5, 3
	BranchIf	.LBB20_2, r0
	Move	r49, r1
	CallFf	r0, r5, 1
	LoadStaticPointerImm	r49, 4
	Move	r50, r0
	CallFf	r0, r3, 2
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB20_2:
	LookupPointer	r4, 5
	LoadStaticU64Imm	r49, 6
	Move	r50, r1
	CallFf	r0, r4, 2
	BranchIf	.LBB20_4, r0
	Move	r49, r1
	CallFf	r0, r5, 1
	LoadStaticPointerImm	r49, 7
	Move	r50, r0
	CallFf	r0, r3, 2
	Move	r49, r0
	CallFf	r0, r2, 1
.LBB20_4:
	LoadStaticU64Imm	r49, 8
	Move	r50, r1
	CallFf	r0, r4, 2
	BranchIfNot	.LBB20_5, r0
	LoadStaticU64Imm	r49, 9
	Move	r50, r1
	CallFf	r0, r4, 2
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
	LookupPointer	r0, 1
	LoadStaticU64Imm	r50, 2
	CallFf	r0, r0, 2
	Move	r2, r0
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	LoadU16Imm	r1, 0
	BranchIfNot	.LBB21_3, r0
	LookupPointer	r0, 3
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
	.quad	1538777247405
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
	.quad	1538777247744
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
	.quad	1538777247344
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
	.quad	1538777247869
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
	.quad	1538777247798
	.quad	-3311351115081502130
	.quad	1538777247819
	.size	"wave-manager-get-next-spawning-wave-npc-schemas-group_symbol_table_src", 216

	.type	"wave-manager-get-total-wave-count_symbol_table_src",@object
	.section	".rodata.wave-manager-get-total-wave-count_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wave-manager-get-total-wave-count_symbol_table_src":
	.quad	998391088342721072
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	1538777247462
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
	.quad	1538777247647
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
	.quad	1538777247701
	.quad	-3311351115081502130
	.size	"wave-manager-stop!_symbol_table_src", 56

	.type	"wave-manager-get-npc-alive-count_symbol_table_src",@object
	.section	".rodata.wave-manager-get-npc-alive-count_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wave-manager-get-npc-alive-count_symbol_table_src":
	.quad	998391088342721072
	.quad	2997365522926602638
	.quad	6072285182812319085
	.quad	1538777247405
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
	.quad	1538777247585
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
	.quad	1538777247520
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

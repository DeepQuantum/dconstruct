	.file	"anim-gas-mask-impl.bin"
	.text
	.globl	"wait-equip-gas-mask"
	.type	"wait-equip-gas-mask",@function
"wait-equip-gas-mask":
	Move	r3, r51
	Move	r2, r50
	Move	r1, r49
	LoadU16Imm	r49, 1
	LookupPointer	r4, 2
	Move	r50, r49
	CallFf	r0, r4, 2
	LoadStaticU64Imm	r50, 4
	LookupPointer	r5, 1
	Move	r49, r1
	Move	r51, r0
	CallFf	r0, r5, 3
	LoadStaticU64Imm	r49, 3
	LookupPointer	r5, 0
	Call	r0, r5, 1
	BranchIf	.LBB0_3, r0
	LookupPointer	r6, 5
.LBB0_2:
	CallFf	r0, r6, 0
	Call	r0, r5, 1
	BranchIfNot	.LBB0_2, r0
.LBB0_3:
	LoadU16Imm	r49, 7
	Move	r50, r1
	CallFf	r0, r4, 2
	LoadStaticU64Imm	r49, 9
	LoadStaticU64Imm	r4, 3
	LookupPointer	r6, 8
	Move	r50, r4
	Move	r51, r0
	CallFf	r0, r6, 3
	OpLogNot	r0, r3
	BranchIf	.LBB0_4, r0
	LookupPointer	r0, 7
	Branch	.LBB0_6
.LBB0_4:
	LookupPointer	r0, 6
.LBB0_6:
	Move	r49, r1
	Move	r50, r2
	Call	r0, r0, 2
	Move	r49, r4
	Call	r0, r5, 1
	BranchIf	.LBB0_9, r0
	LookupPointer	r2, 5
	LoadStaticU64Imm	r49, 3
.LBB0_8:
	CallFf	r0, r2, 0
	Call	r0, r5, 1
	BranchIfNot	.LBB0_8, r0
.LBB0_9:
	LookupPointer	r0, 10
	Move	r49, r1
	CallFf	r0, r0, 1
	Return	r0
"wait-equip-gas-mask_symbol_table":
	.quad	6481843155139167450
	.quad	-2210386714833194546
	.quad	3684810824690217768
	.quad	6150028017864311825
	.quad	7671589888336862994
	.quad	-3047912596559836939
	.quad	5608585023750540989
	.quad	6078802017353408438
	.quad	4789555171397753330
	.quad	-3266162430027026367
	.quad	-5076200561558624898
.Lfunc_end0:
	.size	"wait-equip-gas-mask", .Lfunc_end0-"wait-equip-gas-mask"

	.globl	"internal-put-on-gas-mask.1"
	.type	"internal-put-on-gas-mask.1",@function
"internal-put-on-gas-mask.1":
	Move	r2, r50
	Move	r1, r49
	LookupPointer	r6, 3
	CallFf	r0, r6, 0
	LookupPointer	r0, 2
	CallFf	r0, r0, 1
	LoadStaticU64Imm	r50, 4
	LookupPointer	r3, 1
	Move	r49, r0
	CallFf	r0, r3, 2
	Move	r3, r0
	IAddImm	r0, r3, 56
	LoadU8	r50, r0
	LookupPointer	r0, 0
	Move	r49, r1
	Call	r0, r0, 2
	BranchIf	.LBB1_2, r2
	IAddImm	r0, r3, 40
	LoadU64	r49, r0
	LoadStaticU64Imm	r50, 6
	LoadU16Imm	r53, 1
	LoadU16Imm	r51, 0
	LookupPointer	r0, 5
	Move	r52, r51
	Move	r54, r51
	Move	r55, r51
	Move	r56, r51
	Move	r57, r51
	CallFf	r0, r0, 9
	Move	r2, r0
.LBB1_2:
	LoadU16Imm	r50, 6
	LookupPointer	r0, 8
	Move	r49, r2
	CallFf	r0, r0, 2
	LookupPointer	r7, 7
	Move	r49, r1
	CallFf	r0, r7, 1
	BranchIfNot	.LBB1_4, r0
	LoadU16Imm	r50, 1
	LookupPointer	r0, 10
	Move	r49, r2
	CallFf	r0, r0, 2
	LookupPointer	r0, 9
	Branch	.LBB1_5
.LBB1_4:
	LookupPointer	r0, 11
.LBB1_5:
	LoadU16Imm	r4, 1
	LoadU16Imm	r5, 0
	Move	r49, r2
	Move	r50, r5
	Call	r0, r0, 2
	LookupPointer	r0, 13
	Move	r49, r1
	Move	r50, r2
	CallFf	r0, r0, 2
	LookupPointer	r0, 12
	Move	r49, r2
	Move	r50, r5
	CallFf	r0, r0, 2
	Move	r49, r1
	CallFf	r0, r7, 1
	BranchIf	.LBB1_7, r0
	CallFf	r0, r6, 0
.LBB1_7:
	LoadStaticU64Imm	r51, 24
	LoadU16Imm	r52, 0
	LookupPointer	r0, 20
	Move	r49, r2
	Move	r50, r1
	CallFf	r0, r0, 4
	LoadU16Imm	r49, 6
	LoadStaticFloatImm	r50, 23
	LookupPointer	r0, 19
	CallFf	r0, r0, 2
	LoadStaticU64Imm	r49, 22
	LookupPointer	r5, 18
	Move	r50, r1
	Move	r51, r0
	CallFf	r0, r5, 3
	IAddImm	r0, r3, 16
	LoadU64	r5, r0
	LoadStaticU64Imm	r49, 21
	LoadU16Imm	r50, 16
	LoadU16Imm	r51, 0
	LookupPointer	r7, 17
	Move	r52, r4
	CallFf	r0, r7, 4
	LookupPointer	r8, 16
	Move	r49, r2
	Move	r50, r5
	Move	r51, r0
	CallFf	r0, r8, 3
	LookupPointer	r5, 14
	LookupPointer	r9, 15
	LoadU16Imm	r10, 19
.LBB1_8:
	LoadU64	r50, r3
	CallFf	r0, r9, 0
	IAddImm	r11, r0, 36
	StoreU32	r10, [r11], r10
	IAddImm	r11, r0, 33
	StoreU8	r4, [r11], r4
	IAddImm	r11, r0, 34
	StoreU8	r4, [r11], r4
	IAddImm	r11, r0, 32
	StoreU8	r4, [r11], r4
	Move	r49, r1
	Move	r51, r0
	CallFf	r0, r5, 3
	BranchIfNot	.LBB1_10, r0
	CallFf	r0, r6, 0
	Branch	.LBB1_8
.LBB1_10:
	IAddImm	r0, r3, 24
	LoadU64	r1, r0
	LoadStaticU64Imm	r49, 21
	LoadU16Imm	r50, 16
	LoadU16Imm	r51, 0
	LoadU16Imm	r52, 1
	CallFf	r0, r7, 4
	Move	r49, r2
	Move	r50, r1
	Move	r51, r0
	CallFf	r0, r8, 3
	Return	r0
"internal-put-on-gas-mask.1_symbol_table":
	.quad	8203816822361639083
	.quad	-7658033447216601854
	.quad	-6573555005595409285
	.quad	-3047912596559836939
	.quad	3455792245619945009
	.quad	7097052834867614476
	.quad	267125575217391726
	.quad	6532480022340268902
	.quad	4766017279285775524
	.quad	-7252477440030364545
	.quad	-429226074377614887
	.quad	-3854484060414110792
	.quad	-7746349783173232884
	.quad	-5971891483814290574
	.quad	6500315221283771953
	.quad	-2962780980939953026
	.quad	-9179819928081802467
	.quad	-7745656483000545638
	.quad	4789555171397753330
	.quad	3684810824690217768
	.quad	-5676660224586178492
	.quad	8534565298598421705
	.quad	6817768622307059015
	.quad	1050253722
	.quad	8667864169316382944
.Lfunc_end1:
	.size	"internal-put-on-gas-mask.1", .Lfunc_end1-"internal-put-on-gas-mask.1"

	.globl	"internal-put-on-gas-mask-immediately.2"
	.type	"internal-put-on-gas-mask-immediately.2",@function
"internal-put-on-gas-mask-immediately.2":
	Move	r2, r50
	Move	r1, r49
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	LoadStaticU64Imm	r50, 2
	LookupPointer	r3, 0
	Move	r49, r0
	CallFf	r0, r3, 2
	Move	r3, r0
	BranchIf	.LBB2_2, r2
	IAddImm	r0, r3, 40
	LoadU64	r49, r0
	LoadStaticU64Imm	r50, 4
	LoadU16Imm	r53, 1
	LoadU16Imm	r51, 0
	LookupPointer	r0, 3
	Move	r52, r51
	Move	r54, r51
	Move	r55, r51
	Move	r56, r51
	Move	r57, r51
	CallFf	r0, r0, 9
	Move	r2, r0
.LBB2_2:
	LoadU16Imm	r50, 6
	LookupPointer	r0, 7
	Move	r49, r2
	CallFf	r0, r0, 2
	LookupPointer	r0, 6
	Move	r49, r1
	Move	r50, r2
	CallFf	r0, r0, 2
	LookupPointer	r5, 5
	CallFf	r0, r5, 1
	BranchIfNot	.LBB2_3, r0
	LoadU16Imm	r49, 6
	LoadU16Imm	r50, 0
	LookupPointer	r0, 10
	CallFf	r0, r0, 2
	LoadStaticU64Imm	r49, 11
	LookupPointer	r6, 9
	Move	r50, r1
	Move	r51, r0
	CallFf	r0, r6, 3
	LoadU16Imm	r50, 1
	LookupPointer	r0, 8
	Move	r49, r2
	CallFf	r0, r0, 2
	Branch	.LBB2_5
.LBB2_3:
	LookupPointer	r6, 9
.LBB2_5:
	LoadU16Imm	r4, 0
	LoadU16Imm	r51, 1
	LoadStaticU64Imm	r49, 17
	Move	r50, r1
	CallFf	r0, r6, 2
	Move	r49, r1
	CallFf	r0, r5, 1
	OpLogNot	r0, r0
	BranchIf	.LBB2_6, r0
	LookupPointer	r0, 14
	Branch	.LBB2_8
.LBB2_6:
	LookupPointer	r0, 15
.LBB2_8:
	Move	r49, r2
	Move	r50, r4
	Call	r0, r0, 2
	IAddImm	r0, r3, 24
	LoadU64	r3, r0
	LoadStaticU64Imm	r49, 16
	LoadU16Imm	r50, 16
	LookupPointer	r0, 13
	Move	r52, r51
	CallFf	r0, r0, 4
	LoadU64	r5, r0
	StoreU8	r1, [r5], r1
	LoadU64	r5, r1
	StoreU8	r1, [r5], r1
	LoadU16Imm	r5, 31
	StoreU32	r5, [r1], r5
	LoadU64	r5, r0
	StoreU8	r1, [r5], r1
	LoadU64	r5, r1
	StoreU8	r1, [r5], r1
	IAddImm	r5, r1, 8
	StoreU64	r4, [r5], r4
	LoadU64	r4, r0
	StoreU8	r1, [r4], r1
	LoadU64	r4, r1
	StoreU8	r1, [r4], r1
	StoreU8	r51, [r5], r51
	LookupPointer	r1, 12
	Move	r49, r2
	Move	r50, r3
	Move	r51, r0
	CallFf	r0, r1, 3
	Return	r0
"internal-put-on-gas-mask-immediately.2_symbol_table":
	.quad	-7658033447216601854
	.quad	-6573555005595409285
	.quad	3455792245619945009
	.quad	7097052834867614476
	.quad	267125575217391726
	.quad	6532480022340268902
	.quad	-5971891483814290574
	.quad	4766017279285775524
	.quad	-429226074377614887
	.quad	4789555171397753330
	.quad	3684810824690217768
	.quad	6817768622307059015
	.quad	-9179819928081802467
	.quad	-7745656483000545638
	.quad	-7252477440030364545
	.quad	-3854484060414110792
	.quad	8534565298598421705
	.quad	1827492438953869695
.Lfunc_end2:
	.size	"internal-put-on-gas-mask-immediately.2", .Lfunc_end2-"internal-put-on-gas-mask-immediately.2"

	.globl	"get-equipped-gas-mask"
	.type	"get-equipped-gas-mask",@function
"get-equipped-gas-mask":
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	Return	r0
"get-equipped-gas-mask_symbol_table":
	.quad	-5076200561558624898
.Lfunc_end3:
	.size	"get-equipped-gas-mask", .Lfunc_end3-"get-equipped-gas-mask"

	.globl	"wait-until-in-valid-gas-mask-state.3"
	.type	"wait-until-in-valid-gas-mask-state.3",@function
"wait-until-in-valid-gas-mask-state.3":
	Move	r1, r50
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	BranchIfNot	.LBB4_16, r0
	LookupPointer	r4, 1
	LookupPointer	r5, 2
	LoadStaticU64Imm	r2, 3
	LoadStaticU64Imm	r3, 4
	Branch	.LBB4_2
.LBB4_4:
	CallFf	r0, r4, 0
.LBB4_2:
	Move	r49, r2
	CallFf	r0, r5, 1
	BranchIf	.LBB4_4, r0
	Move	r49, r3
	CallFf	r0, r5, 1
	BranchIf	.LBB4_4, r0
	LookupPointer	r2, 5
	BranchIfNot	.LBB4_13, r1
	LookupPointer	r0, 6
	CallFf	r0, r0, 0
	CallFf	r0, r2, 0
	BranchIf	.LBB4_8, r0
.LBB4_12:
	CallFf	r0, r4, 0
	CallFf	r0, r2, 0
	BranchIfNot	.LBB4_12, r0
.LBB4_8:
	LoadStaticU64Imm	r49, 9
	CallFf	r0, r5, 1
	BranchIf	.LBB4_11, r0
.LBB4_9:
	CallFf	r0, r4, 0
	CallFf	r0, r5, 1
	BranchIfNot	.LBB4_9, r0
.LBB4_11:
	LoadU16Imm	r1, 0
	LoadStaticFloatImm	r50, 13
	LookupPointer	r0, 11
	Move	r49, r1
	CallFf	r0, r0, 2
	LoadStaticFloatImm	r49, 12
	LookupPointer	r0, 10
	Move	r50, r1
	Branch	.LBB4_21
.LBB4_16:
	LoadU16Imm	r51, 1
	LoadStaticFloatImm	r52, 16
	LoadU16Imm	r50, 0
	LookupPointer	r0, 15
	Move	r53, r50
	CallFf	r0, r0, 5
	LookupPointer	r2, 14
	CallFf	r0, r2, 1
	BranchIfNot	.LBB4_19, r0
	LookupPointer	r3, 1
.LBB4_18:
	CallFf	r0, r3, 0
	CallFf	r0, r2, 1
	BranchIf	.LBB4_18, r0
.LBB4_19:
	BranchIfNot	.LBB4_22, r1
	LoadU16Imm	r50, 3
	LookupPointer	r0, 17
.LBB4_21:
	CallFf	r0, r0, 2
	Move	r50, r0
.LBB4_22:
	Move	r0, r50
	Return	r0
.LBB4_13:
	LookupPointer	r0, 8
	CallFf	r0, r0, 0
	CallFf	r0, r2, 0
	LookupPointer	r1, 7
.LBB4_15:
	Move	r49, r0
	Call	r0, r1, 1
	BranchIf	.LBB4_8, r0
	CallFf	r0, r4, 0
	CallFf	r0, r2, 0
	Branch	.LBB4_15
"wait-until-in-valid-gas-mask-state.3_symbol_table":
	.quad	6532480022340268902
	.quad	-3047912596559836939
	.quad	-9003251364961024783
	.quad	6740565536053766520
	.quad	2237451099064635199
	.quad	1646463116072042322
	.quad	-4831287436211302109
	.quad	-5808627241898115124
	.quad	8594926419455150747
	.quad	4007104907368024212
	.quad	-626103520744544694
	.quad	-3092934816467423265
	.quad	1045220557
	.quad	1065353216
	.quad	7683024912818272899
	.quad	5808793362033684947
	.quad	3212836864
	.quad	8573502661920361253
.Lfunc_end4:
	.size	"wait-until-in-valid-gas-mask-state.3", .Lfunc_end4-"wait-until-in-valid-gas-mask-state.3"

	.globl	"npc-gas-mask-narrative-mode/f"
	.type	"npc-gas-mask-narrative-mode/f",@function
"npc-gas-mask-narrative-mode/f":
	Move	r1, r49
	LoadStaticU64Imm	r49, 3
	LookupPointer	r0, 2
	Move	r50, r1
	CallFf	r0, r0, 2
	LookupPointer	r0, 1
	Move	r49, r1
	Call	r0, r0, 1
	LoadU16Imm	r50, 0
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	Return	r0
"npc-gas-mask-narrative-mode/f_symbol_table":
	.quad	7610916627332680768
	.quad	-5356689657310757786
	.quad	4789555171397753330
	.quad	8119835013146602502
.Lfunc_end5:
	.size	"npc-gas-mask-narrative-mode/f", .Lfunc_end5-"npc-gas-mask-narrative-mode/f"

	.globl	"internal-put-off-gas-mask-immediately"
	.type	"internal-put-off-gas-mask-immediately",@function
"internal-put-off-gas-mask-immediately":
	Move	r2, r50
	Move	r50, r49
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	Move	r1, r0
	LoadStaticU64Imm	r49, 2
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	BranchIfNot	.LBB6_1, r2
	LookupPointer	r0, 3
	Move	r49, r1
	CallFf	r0, r0, 1
	Return	r0
.LBB6_1:
	LoadU16Imm	r0, 0
	Return	r0
"internal-put-off-gas-mask-immediately_symbol_table":
	.quad	4789555171397753330
	.quad	-5076200561558624898
	.quad	-3264453205741318791
	.quad	-804116741684351865
.Lfunc_end6:
	.size	"internal-put-off-gas-mask-immediately", .Lfunc_end6-"internal-put-off-gas-mask-immediately"

	.globl	"spawn-gas-mask"
	.type	"spawn-gas-mask",@function
"spawn-gas-mask":
	LookupPointer	r0, 3
	CallFf	r0, r0, 1
	LoadStaticU64Imm	r50, 5
	LookupPointer	r1, 2
	Move	r49, r0
	CallFf	r0, r1, 2
	IAddImm	r0, r0, 40
	LoadU64	r49, r0
	LoadStaticU64Imm	r50, 4
	LoadU16Imm	r53, 1
	LoadU16Imm	r51, 0
	LookupPointer	r0, 1
	Move	r52, r51
	Move	r54, r51
	Move	r55, r51
	Move	r56, r51
	Move	r57, r51
	CallFf	r0, r0, 9
	Move	r49, r0
	LoadU16Imm	r50, 6
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	Move	r0, r49
	Return	r0
"spawn-gas-mask_symbol_table":
	.quad	4766017279285775524
	.quad	7097052834867614476
	.quad	-7658033447216601854
	.quad	-6573555005595409285
	.quad	267125575217391726
	.quad	3455792245619945009
.Lfunc_end7:
	.size	"spawn-gas-mask", .Lfunc_end7-"spawn-gas-mask"

	.globl	"player-gas-mask-narrative-mode/f"
	.type	"player-gas-mask-narrative-mode/f",@function
"player-gas-mask-narrative-mode/f":
	Move	r1, r49
	LoadU16Imm	r49, 0
	LookupPointer	r0, 4
	CallFf	r0, r0, 1
	LookupPointer	r0, 3
	CallFf	r0, r0, 0
	LookupPointer	r0, 2
	Move	r49, r1
	Call	r0, r0, 3
	LoadStaticU64Imm	r49, 6
	LookupPointer	r0, 1
	Call	r0, r0, 1
	LoadStaticU64Imm	r49, 5
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	Return	r0
"player-gas-mask-narrative-mode/f_symbol_table":
	.quad	7121253686161376880
	.quad	-5356689657310757786
	.quad	-2826777519175913837
	.quad	8165897991486741957
	.quad	7238593933813171071
	.quad	-1699981380503476753
	.quad	5008278420455340480
.Lfunc_end8:
	.size	"player-gas-mask-narrative-mode/f", .Lfunc_end8-"player-gas-mask-narrative-mode/f"

	.globl	"internal-put-off-gas-mask"
	.type	"internal-put-off-gas-mask",@function
"internal-put-off-gas-mask":
	Move	r1, r50
	Move	r2, r49
	LookupPointer	r7, 8
	CallFf	r0, r7, 0
	LookupPointer	r0, 7
	CallFf	r0, r0, 1
	LoadStaticU64Imm	r50, 10
	LookupPointer	r3, 6
	Move	r49, r0
	CallFf	r0, r3, 2
	Move	r5, r0
	LookupPointer	r0, 5
	Move	r49, r2
	CallFf	r0, r0, 1
	Move	r3, r0
	IAddImm	r0, r5, 57
	LoadU8	r50, r0
	LookupPointer	r0, 4
	Call	r0, r0, 2
	IAddImm	r0, r5, 32
	LoadU64	r6, r0
	LoadStaticU64Imm	r49, 9
	LoadU16Imm	r50, 16
	LoadU16Imm	r4, 0
	LoadU16Imm	r52, 1
	LookupPointer	r0, 3
	Move	r51, r4
	CallFf	r0, r0, 4
	LookupPointer	r8, 2
	Move	r49, r3
	Move	r50, r6
	Move	r51, r0
	CallFf	r0, r8, 3
	LookupPointer	r6, 0
	LookupPointer	r8, 1
	IAddImm	r5, r5, 8
	LoadU16Imm	r9, 19
.LBB9_1:
	LoadU64	r50, r5
	CallFf	r0, r8, 0
	IAddImm	r10, r0, 36
	StoreU32	r9, [r10], r9
	IAddImm	r10, r0, 33
	StoreU8	r52, [r10], r52
	IAddImm	r10, r0, 34
	StoreU8	r52, [r10], r52
	IAddImm	r10, r0, 32
	StoreU8	r52, [r10], r52
	Move	r49, r2
	Move	r51, r0
	CallFf	r0, r6, 3
	BranchIfNot	.LBB9_3, r0
	CallFf	r0, r7, 0
	Branch	.LBB9_1
.LBB9_3:
	BranchIfNot	.LBB9_5, r1
	LookupPointer	r0, 11
	Move	r49, r3
	CallFf	r0, r0, 1
	Move	r4, r0
.LBB9_5:
	Move	r0, r4
	Return	r0
"internal-put-off-gas-mask_symbol_table":
	.quad	6500315221283771953
	.quad	-2962780980939953026
	.quad	-9179819928081802467
	.quad	-7745656483000545638
	.quad	8203816822361639083
	.quad	-5076200561558624898
	.quad	-7658033447216601854
	.quad	-6573555005595409285
	.quad	-3047912596559836939
	.quad	8534565298598421705
	.quad	3455792245619945009
	.quad	-804116741684351865
.Lfunc_end9:
	.size	"internal-put-off-gas-mask", .Lfunc_end9-"internal-put-off-gas-mask"

	.globl	"wait-remove-gas-mask"
	.type	"wait-remove-gas-mask",@function
"wait-remove-gas-mask":
	Move	r2, r51
	Move	r4, r50
	Move	r3, r49
	LoadU16Imm	r49, 1
	LoadU16Imm	r1, 0
	LookupPointer	r5, 2
	Move	r50, r1
	CallFf	r0, r5, 2
	LoadStaticU64Imm	r50, 4
	LookupPointer	r6, 1
	Move	r49, r3
	Move	r51, r0
	CallFf	r0, r6, 3
	LoadStaticU64Imm	r49, 3
	LookupPointer	r6, 0
	Call	r0, r6, 1
	BranchIf	.LBB10_3, r0
	LookupPointer	r7, 5
.LBB10_2:
	CallFf	r0, r7, 0
	Call	r0, r6, 1
	BranchIfNot	.LBB10_2, r0
.LBB10_3:
	LoadU16Imm	r49, 7
	Move	r50, r3
	CallFf	r0, r5, 2
	LoadStaticU64Imm	r49, 7
	LoadStaticU64Imm	r50, 3
	LookupPointer	r5, 6
	Move	r51, r0
	CallFf	r0, r5, 3
	BranchIfNot	.LBB10_6, r4
	LookupPointer	r0, 8
	Move	r49, r3
	CallFf	r0, r0, 1
	Move	r4, r0
	LoadStaticU64Imm	r49, 9
	Move	r50, r3
	CallFf	r0, r5, 2
	BranchIfNot	.LBB10_8, r2
	LookupPointer	r0, 10
	Move	r49, r4
	CallFf	r0, r0, 1
	Branch	.LBB10_7
.LBB10_6:
	LookupPointer	r0, 11
	Move	r49, r3
	Move	r50, r2
	Call	r0, r0, 2
.LBB10_7:
	Move	r1, r0
.LBB10_8:
	Move	r0, r1
	Return	r0
"wait-remove-gas-mask_symbol_table":
	.quad	6481843155139167450
	.quad	-2210386714833194546
	.quad	3684810824690217768
	.quad	6150028017864311825
	.quad	7671589888336862994
	.quad	-3047912596559836939
	.quad	4789555171397753330
	.quad	-3325057813225958160
	.quad	-5076200561558624898
	.quad	-3264453205741318791
	.quad	-804116741684351865
	.quad	-6610620527767074061
.Lfunc_end10:
	.size	"wait-remove-gas-mask", .Lfunc_end10-"wait-remove-gas-mask"

	.type	"wait-equip-gas-mask_symbol_table_src",@object
	.section	".rodata.wait-equip-gas-mask_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wait-equip-gas-mask_symbol_table_src":
	.quad	-2210386714833194546
	.quad	7671589888336862994
	.quad	3684810824690217768
	.quad	6481843155139167450
	.quad	6150028017864311825
	.quad	-3047912596559836939
	.quad	4789555171397753330
	.quad	-3266162430027026367
	.quad	6078802017353408438
	.quad	5608585023750540989
	.quad	-5076200561558624898
	.size	"wait-equip-gas-mask_symbol_table_src", 88

	.type	"internal-put-on-gas-mask_symbol_table_src",@object
	.section	".rodata.internal-put-on-gas-mask_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"internal-put-on-gas-mask_symbol_table_src":
	.quad	-3047912596559836939
	.quad	-7658033447216601854
	.quad	-6573555005595409285
	.quad	3455792245619945009
	.quad	8203816822361639083
	.quad	7097052834867614476
	.quad	267125575217391726
	.quad	4766017279285775524
	.quad	6532480022340268902
	.quad	-429226074377614887
	.quad	-7252477440030364545
	.quad	-3854484060414110792
	.quad	-5971891483814290574
	.quad	-7746349783173232884
	.quad	-5676660224586178492
	.quad	8667864169316382944
	.quad	0
	.quad	4789555171397753330
	.quad	6817768622307059015
	.quad	3684810824690217768
	.quad	1050253722
	.quad	-9179819928081802467
	.quad	-7745656483000545638
	.quad	8534565298598421705
	.quad	6500315221283771953
	.quad	-2962780980939953026
	.size	"internal-put-on-gas-mask_symbol_table_src", 208

	.type	"internal-put-on-gas-mask-immediately_symbol_table_src",@object
	.section	".rodata.internal-put-on-gas-mask-immediately_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"internal-put-on-gas-mask-immediately_symbol_table_src":
	.quad	-7658033447216601854
	.quad	-6573555005595409285
	.quad	3455792245619945009
	.quad	7097052834867614476
	.quad	267125575217391726
	.quad	4766017279285775524
	.quad	-5971891483814290574
	.quad	6532480022340268902
	.quad	4789555171397753330
	.quad	6817768622307059015
	.quad	3684810824690217768
	.quad	0
	.quad	-429226074377614887
	.quad	1827492438953869695
	.quad	-7252477440030364545
	.quad	-3854484060414110792
	.quad	-9179819928081802467
	.quad	-7745656483000545638
	.quad	8534565298598421705
	.size	"internal-put-on-gas-mask-immediately_symbol_table_src", 152

	.type	"get-equipped-gas-mask_symbol_table_src",@object
	.section	".rodata.cst8.get-equipped-gas-mask_symbol_table_src","aMR",@progbits,8
	.p2align	3, 0x0
"get-equipped-gas-mask_symbol_table_src":
	.quad	-5076200561558624898
	.size	"get-equipped-gas-mask_symbol_table_src", 8

	.type	"wait-until-in-valid-gas-mask-state_symbol_table_src",@object
	.section	".rodata.wait-until-in-valid-gas-mask-state_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wait-until-in-valid-gas-mask-state_symbol_table_src":
	.quad	6532480022340268902
	.quad	-9003251364961024783
	.quad	6740565536053766520
	.quad	2237451099064635199
	.quad	-3047912596559836939
	.quad	-4831287436211302109
	.quad	1646463116072042322
	.quad	8594926419455150747
	.quad	-5808627241898115124
	.quad	4007104907368024212
	.quad	-3092934816467423265
	.quad	1065353216
	.quad	-626103520744544694
	.quad	1045220557
	.quad	5808793362033684947
	.quad	3212836864
	.quad	7683024912818272899
	.quad	8573502661920361253
	.size	"wait-until-in-valid-gas-mask-state_symbol_table_src", 144

	.type	"npc-gas-mask-narrative-mode/f_symbol_table_src",@object
	.section	".rodata.npc-gas-mask-narrative-mode/f_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"npc-gas-mask-narrative-mode/f_symbol_table_src":
	.quad	4789555171397753330
	.quad	8119835013146602502
	.quad	-5356689657310757786
	.quad	7610916627332680768
	.quad	0
	.size	"npc-gas-mask-narrative-mode/f_symbol_table_src", 40

	.type	"internal-put-off-gas-mask-immediately_symbol_table_src",@object
	.section	".rodata.cst32.internal-put-off-gas-mask-immediately_symbol_table_src","aMR",@progbits,32
	.p2align	3, 0x0
"internal-put-off-gas-mask-immediately_symbol_table_src":
	.quad	-5076200561558624898
	.quad	4789555171397753330
	.quad	-3264453205741318791
	.quad	-804116741684351865
	.size	"internal-put-off-gas-mask-immediately_symbol_table_src", 32

	.type	"spawn-gas-mask_symbol_table_src",@object
	.section	".rodata.spawn-gas-mask_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"spawn-gas-mask_symbol_table_src":
	.quad	-7658033447216601854
	.quad	-6573555005595409285
	.quad	3455792245619945009
	.quad	7097052834867614476
	.quad	267125575217391726
	.quad	4766017279285775524
	.size	"spawn-gas-mask_symbol_table_src", 48

	.type	"player-gas-mask-narrative-mode/f_symbol_table_src",@object
	.section	".rodata.player-gas-mask-narrative-mode/f_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"player-gas-mask-narrative-mode/f_symbol_table_src":
	.quad	7238593933813171071
	.quad	0
	.quad	8165897991486741957
	.quad	-2826777519175913837
	.quad	-5356689657310757786
	.quad	5008278420455340480
	.quad	7121253686161376880
	.quad	-1699981380503476753
	.size	"player-gas-mask-narrative-mode/f_symbol_table_src", 64

	.type	"internal-put-off-gas-mask_symbol_table_src",@object
	.section	".rodata.internal-put-off-gas-mask_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"internal-put-off-gas-mask_symbol_table_src":
	.quad	-3047912596559836939
	.quad	-7658033447216601854
	.quad	-6573555005595409285
	.quad	3455792245619945009
	.quad	-5076200561558624898
	.quad	8203816822361639083
	.quad	-9179819928081802467
	.quad	-7745656483000545638
	.quad	8534565298598421705
	.quad	6500315221283771953
	.quad	-2962780980939953026
	.quad	-804116741684351865
	.size	"internal-put-off-gas-mask_symbol_table_src", 96

	.type	"wait-remove-gas-mask_symbol_table_src",@object
	.section	".rodata.wait-remove-gas-mask_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wait-remove-gas-mask_symbol_table_src":
	.quad	-2210386714833194546
	.quad	7671589888336862994
	.quad	3684810824690217768
	.quad	6481843155139167450
	.quad	6150028017864311825
	.quad	-3047912596559836939
	.quad	4789555171397753330
	.quad	-3325057813225958160
	.quad	-8559530813269772124
	.quad	-6610620527767074061
	.size	"wait-remove-gas-mask_symbol_table_src", 80

	.section	".note.GNU-stack","",@progbits

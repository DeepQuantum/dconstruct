; ModuleID = 'anim-gas-mask-impl.bin'
source_filename = "anim-gas-mask-impl.bin"

@wait-equip-gas-mask_symbol_table_src = private unnamed_addr constant [11 x i64] [i64 -2210386714833194546, i64 7671589888336862994, i64 3684810824690217768, i64 6481843155139167450, i64 6150028017864311825, i64 -3047912596559836939, i64 4789555171397753330, i64 -3266162430027026367, i64 6078802017353408438, i64 5608585023750540989, i64 -5076200561558624898], align 8
@internal-put-on-gas-mask_symbol_table_src = private unnamed_addr constant [26 x i64] [i64 -3047912596559836939, i64 -7658033447216601854, i64 -6573555005595409285, i64 3455792245619945009, i64 8203816822361639083, i64 7097052834867614476, i64 267125575217391726, i64 4766017279285775524, i64 6532480022340268902, i64 -429226074377614887, i64 -7252477440030364545, i64 -3854484060414110792, i64 -5971891483814290574, i64 -7746349783173232884, i64 -5676660224586178492, i64 8667864169316382944, i64 0, i64 4789555171397753330, i64 6817768622307059015, i64 3684810824690217768, i64 1050253722, i64 -9179819928081802467, i64 -7745656483000545638, i64 8534565298598421705, i64 6500315221283771953, i64 -2962780980939953026], align 8
@internal-put-on-gas-mask-immediately_symbol_table_src = private unnamed_addr constant [19 x i64] [i64 -7658033447216601854, i64 -6573555005595409285, i64 3455792245619945009, i64 7097052834867614476, i64 267125575217391726, i64 4766017279285775524, i64 -5971891483814290574, i64 6532480022340268902, i64 4789555171397753330, i64 6817768622307059015, i64 3684810824690217768, i64 0, i64 -429226074377614887, i64 1827492438953869695, i64 -7252477440030364545, i64 -3854484060414110792, i64 -9179819928081802467, i64 -7745656483000545638, i64 8534565298598421705], align 8
@get-equipped-gas-mask_symbol_table_src = private unnamed_addr constant [1 x i64] [i64 -5076200561558624898], align 8
@wait-until-in-valid-gas-mask-state_symbol_table_src = private unnamed_addr constant [18 x i64] [i64 6532480022340268902, i64 -9003251364961024783, i64 6740565536053766520, i64 2237451099064635199, i64 -3047912596559836939, i64 -4831287436211302109, i64 1646463116072042322, i64 8594926419455150747, i64 -5808627241898115124, i64 4007104907368024212, i64 -3092934816467423265, i64 1065353216, i64 -626103520744544694, i64 1045220557, i64 5808793362033684947, i64 3212836864, i64 7683024912818272899, i64 8573502661920361253], align 8
@"npc-gas-mask-narrative-mode/f_symbol_table_src" = private unnamed_addr constant [5 x i64] [i64 4789555171397753330, i64 8119835013146602502, i64 -5356689657310757786, i64 7610916627332680768, i64 0], align 8
@internal-put-off-gas-mask-immediately_symbol_table_src = private unnamed_addr constant [4 x i64] [i64 -5076200561558624898, i64 4789555171397753330, i64 -3264453205741318791, i64 -804116741684351865], align 8
@spawn-gas-mask_symbol_table_src = private unnamed_addr constant [6 x i64] [i64 -7658033447216601854, i64 -6573555005595409285, i64 3455792245619945009, i64 7097052834867614476, i64 267125575217391726, i64 4766017279285775524], align 8
@"player-gas-mask-narrative-mode/f_symbol_table_src" = private unnamed_addr constant [8 x i64] [i64 7238593933813171071, i64 0, i64 8165897991486741957, i64 -2826777519175913837, i64 -5356689657310757786, i64 5008278420455340480, i64 7121253686161376880, i64 -1699981380503476753], align 8
@internal-put-off-gas-mask_symbol_table_src = private unnamed_addr constant [12 x i64] [i64 -3047912596559836939, i64 -7658033447216601854, i64 -6573555005595409285, i64 3455792245619945009, i64 -5076200561558624898, i64 8203816822361639083, i64 -9179819928081802467, i64 -7745656483000545638, i64 8534565298598421705, i64 6500315221283771953, i64 -2962780980939953026, i64 -804116741684351865], align 8
@wait-remove-gas-mask_symbol_table_src = private unnamed_addr constant [10 x i64] [i64 -2210386714833194546, i64 7671589888336862994, i64 3684810824690217768, i64 6481843155139167450, i64 6150028017864311825, i64 -3047912596559836939, i64 4789555171397753330, i64 -3325057813225958160, i64 -8559530813269772124, i64 -6610620527767074061], align 8
@llvm.used = appending global [11 x ptr] [ptr @wait-equip-gas-mask_symbol_table_src, ptr @internal-put-on-gas-mask_symbol_table_src, ptr @internal-put-on-gas-mask-immediately_symbol_table_src, ptr @get-equipped-gas-mask_symbol_table_src, ptr @wait-until-in-valid-gas-mask-state_symbol_table_src, ptr @"npc-gas-mask-narrative-mode/f_symbol_table_src", ptr @internal-put-off-gas-mask-immediately_symbol_table_src, ptr @spawn-gas-mask_symbol_table_src, ptr @"player-gas-mask-narrative-mode/f_symbol_table_src", ptr @internal-put-off-gas-mask_symbol_table_src, ptr @wait-remove-gas-mask_symbol_table_src], section "llvm.metadata"

define i64 @wait-equip-gas-mask(i64 %arg_0, i64 %arg_1, i64 %arg_2) !dcvm.sid_distance !0 {
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
  store i64 %arg_0, ptr %r49, align 4
  store i64 %arg_1, ptr %r50, align 4
  store i64 %arg_2, ptr %r51, align 4
  %__Move_0___load_op1 = load i64, ptr %r49, align 4
  store i64 %__Move_0___load_op1, ptr %r0, align 4
  %__Move_1___load_op1 = load i64, ptr %r50, align 4
  store i64 %__Move_1___load_op1, ptr %r1, align 4
  %__Move_2___load_op1 = load i64, ptr %r51, align 4
  store i64 %__Move_2___load_op1, ptr %r2, align 4
  store i64 ptrtoint (ptr @fact-set to i64), ptr %r3, align 4
  %__Move_4___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_4___load_op1, ptr %r4, align 4
  %__LoadStaticU64Imm_5___st_load = load i64, ptr getelementptr inbounds ([11 x i64], ptr @wait-equip-gas-mask_symbol_table_src, i64 0, i64 1), align 4
  store i64 %__LoadStaticU64Imm_5___st_load, ptr %r5, align 4
  store i64 ptrtoint (ptr @new-boxed-value to i64), ptr %r6, align 4
  store i64 1, ptr %r7, align 4
  store i64 1, ptr %r8, align 4
  %__Move_9___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_9___load_op1, ptr %r49, align 4
  %__Move_10___load_op1 = load i64, ptr %r8, align 4
  store i64 %__Move_10___load_op1, ptr %r50, align 4
  %__CallFf_11___callee = load i64, ptr %r6, align 4
  %0 = inttoptr i64 %__CallFf_11___callee to ptr
  %__CallFf_11___arg0 = load i64, ptr %r49, align 4
  %__CallFf_11___arg1 = load i64, ptr %r50, align 4
  %__CallFf_11___call = call i64 %0(i64 %__CallFf_11___arg0, i64 %__CallFf_11___arg1), !dcvm.distance !1
  store i64 %__CallFf_11___call, ptr %r6, align 4
  %__Move_12___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_12___load_op1, ptr %r49, align 4
  %__Move_13___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_13___load_op1, ptr %r50, align 4
  %__Move_14___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_14___load_op1, ptr %r51, align 4
  %__CallFf_15___callee = load i64, ptr %r3, align 4
  %1 = inttoptr i64 %__CallFf_15___callee to ptr
  %__CallFf_15___arg0 = load i64, ptr %r49, align 4
  %__CallFf_15___arg1 = load i64, ptr %r50, align 4
  %__CallFf_15___arg2 = load i64, ptr %r51, align 4
  %__CallFf_15___call = call i64 %1(i64 %__CallFf_15___arg0, i64 %__CallFf_15___arg1, i64 %__CallFf_15___arg2), !dcvm.distance !1
  store i64 %__CallFf_15___call, ptr %r3, align 4
  br label %bb1

bb1:                                              ; preds = %bb2, %bb0
  store i64 ptrtoint (ptr @"is-script-running?" to i64), ptr %r3, align 4
  %__LoadStaticU64Imm_17___st_load = load i64, ptr getelementptr inbounds ([11 x i64], ptr @wait-equip-gas-mask_symbol_table_src, i64 0, i64 4), align 4
  store i64 %__LoadStaticU64Imm_17___st_load, ptr %r4, align 4
  %__Move_18___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_18___load_op1, ptr %r49, align 4
  %__Call_19___callee = load i64, ptr %r3, align 4
  %2 = inttoptr i64 %__Call_19___callee to ptr
  %__Call_19___arg0 = load i64, ptr %r49, align 4
  %__Call_19___call = call i64 %2(i64 %__Call_19___arg0)
  store i64 %__Call_19___call, ptr %r3, align 4
  %__OpLogNot_20___load_op1 = load i64, ptr %r3, align 4
  %__OpLogNot_20__op = icmp eq i64 %__OpLogNot_20___load_op1, 0
  %3 = zext i1 %__OpLogNot_20__op to i64
  store i64 %3, ptr %r3, align 4
  %__BranchIfNot_21___cond = load i64, ptr %r3, align 4
  %__BranchIfNot_21___bool = icmp ne i64 %__BranchIfNot_21___cond, 0
  br i1 %__BranchIfNot_21___bool, label %bb2, label %bb3

bb2:                                              ; preds = %bb1
  store i64 ptrtoint (ptr @wait-one-frame to i64), ptr %r3, align 4
  %__CallFf_23___callee = load i64, ptr %r3, align 4
  %4 = inttoptr i64 %__CallFf_23___callee to ptr
  %__CallFf_23___call = call i64 %4(), !dcvm.distance !1
  store i64 %__CallFf_23___call, ptr %r3, align 4
  br label %bb1

bb3:                                              ; preds = %bb1
  store i64 ptrtoint (ptr @send-event to i64), ptr %r3, align 4
  %__LoadStaticU64Imm_26___st_load = load i64, ptr getelementptr inbounds ([11 x i64], ptr @wait-equip-gas-mask_symbol_table_src, i64 0, i64 7), align 4
  store i64 %__LoadStaticU64Imm_26___st_load, ptr %r4, align 4
  %__LoadStaticU64Imm_27___st_load = load i64, ptr getelementptr inbounds ([11 x i64], ptr @wait-equip-gas-mask_symbol_table_src, i64 0, i64 4), align 4
  store i64 %__LoadStaticU64Imm_27___st_load, ptr %r5, align 4
  store i64 ptrtoint (ptr @new-boxed-value to i64), ptr %r6, align 4
  store i64 7, ptr %r7, align 4
  %__Move_30___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_30___load_op1, ptr %r8, align 4
  %__Move_31___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_31___load_op1, ptr %r49, align 4
  %__Move_32___load_op1 = load i64, ptr %r8, align 4
  store i64 %__Move_32___load_op1, ptr %r50, align 4
  %__CallFf_33___callee = load i64, ptr %r6, align 4
  %5 = inttoptr i64 %__CallFf_33___callee to ptr
  %__CallFf_33___arg0 = load i64, ptr %r49, align 4
  %__CallFf_33___arg1 = load i64, ptr %r50, align 4
  %__CallFf_33___call = call i64 %5(i64 %__CallFf_33___arg0, i64 %__CallFf_33___arg1), !dcvm.distance !1
  store i64 %__CallFf_33___call, ptr %r6, align 4
  %__Move_34___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_34___load_op1, ptr %r49, align 4
  %__Move_35___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_35___load_op1, ptr %r50, align 4
  %__Move_36___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_36___load_op1, ptr %r51, align 4
  %__CallFf_37___callee = load i64, ptr %r3, align 4
  %6 = inttoptr i64 %__CallFf_37___callee to ptr
  %__CallFf_37___arg0 = load i64, ptr %r49, align 4
  %__CallFf_37___arg1 = load i64, ptr %r50, align 4
  %__CallFf_37___arg2 = load i64, ptr %r51, align 4
  %__CallFf_37___call = call i64 %6(i64 %__CallFf_37___arg0, i64 %__CallFf_37___arg1, i64 %__CallFf_37___arg2), !dcvm.distance !1
  store i64 %__CallFf_37___call, ptr %r3, align 4
  %__Move_38___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_38___load_op1, ptr %r3, align 4
  %__BranchIfNot_39___cond = load i64, ptr %r3, align 4
  %__BranchIfNot_39___bool = icmp ne i64 %__BranchIfNot_39___cond, 0
  br i1 %__BranchIfNot_39___bool, label %bb4, label %bb5

bb4:                                              ; preds = %bb3
  store i64 ptrtoint (ptr @internal-put-on-gas-mask-immediately to i64), ptr %r3, align 4
  %__Move_41___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_41___load_op1, ptr %r4, align 4
  %__Move_42___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_42___load_op1, ptr %r5, align 4
  %__Move_43___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_43___load_op1, ptr %r49, align 4
  %__Move_44___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_44___load_op1, ptr %r50, align 4
  %__Call_45___callee = load i64, ptr %r3, align 4
  %7 = inttoptr i64 %__Call_45___callee to ptr
  %__Call_45___arg0 = load i64, ptr %r49, align 4
  %__Call_45___arg1 = load i64, ptr %r50, align 4
  %__Call_45___call = call i64 %7(i64 %__Call_45___arg0, i64 %__Call_45___arg1)
  store i64 %__Call_45___call, ptr %r3, align 4
  br label %bb6

bb5:                                              ; preds = %bb3
  store i64 ptrtoint (ptr @internal-put-on-gas-mask to i64), ptr %r3, align 4
  %__Move_48___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_48___load_op1, ptr %r4, align 4
  %__Move_49___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_49___load_op1, ptr %r5, align 4
  %__Move_50___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_50___load_op1, ptr %r49, align 4
  %__Move_51___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_51___load_op1, ptr %r50, align 4
  %__Call_52___callee = load i64, ptr %r3, align 4
  %8 = inttoptr i64 %__Call_52___callee to ptr
  %__Call_52___arg0 = load i64, ptr %r49, align 4
  %__Call_52___arg1 = load i64, ptr %r50, align 4
  %__Call_52___call = call i64 %8(i64 %__Call_52___arg0, i64 %__Call_52___arg1)
  store i64 %__Call_52___call, ptr %r3, align 4
  br label %bb6

bb6:                                              ; preds = %bb7, %bb5, %bb4
  store i64 ptrtoint (ptr @"is-script-running?" to i64), ptr %r3, align 4
  %__LoadStaticU64Imm_54___st_load = load i64, ptr getelementptr inbounds ([11 x i64], ptr @wait-equip-gas-mask_symbol_table_src, i64 0, i64 4), align 4
  store i64 %__LoadStaticU64Imm_54___st_load, ptr %r4, align 4
  %__Move_55___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_55___load_op1, ptr %r49, align 4
  %__Call_56___callee = load i64, ptr %r3, align 4
  %9 = inttoptr i64 %__Call_56___callee to ptr
  %__Call_56___arg0 = load i64, ptr %r49, align 4
  %__Call_56___call = call i64 %9(i64 %__Call_56___arg0)
  store i64 %__Call_56___call, ptr %r3, align 4
  %__OpLogNot_57___load_op1 = load i64, ptr %r3, align 4
  %__OpLogNot_57__op = icmp eq i64 %__OpLogNot_57___load_op1, 0
  %10 = zext i1 %__OpLogNot_57__op to i64
  store i64 %10, ptr %r3, align 4
  %__BranchIfNot_58___cond = load i64, ptr %r3, align 4
  %__BranchIfNot_58___bool = icmp ne i64 %__BranchIfNot_58___cond, 0
  br i1 %__BranchIfNot_58___bool, label %bb7, label %bb8

bb7:                                              ; preds = %bb6
  store i64 ptrtoint (ptr @wait-one-frame to i64), ptr %r3, align 4
  %__CallFf_60___callee = load i64, ptr %r3, align 4
  %11 = inttoptr i64 %__CallFf_60___callee to ptr
  %__CallFf_60___call = call i64 %11(), !dcvm.distance !1
  store i64 %__CallFf_60___call, ptr %r3, align 4
  br label %bb6

bb8:                                              ; preds = %bb6
  store i64 ptrtoint (ptr @get-gas-mask to i64), ptr %r3, align 4
  %__Move_63___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_63___load_op1, ptr %r4, align 4
  %__Move_64___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_64___load_op1, ptr %r49, align 4
  %__CallFf_65___callee = load i64, ptr %r3, align 4
  %12 = inttoptr i64 %__CallFf_65___callee to ptr
  %__CallFf_65___arg0 = load i64, ptr %r49, align 4
  %__CallFf_65___call = call i64 %12(i64 %__CallFf_65___arg0), !dcvm.distance !1
  store i64 %__CallFf_65___call, ptr %r3, align 4
  %__Return_66___retval = load i64, ptr %r3, align 4
  ret i64 %__Return_66___retval
}

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !2 i64 @fact-set(i64, i64, i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !3 i64 @new-boxed-value(i64, i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !4 i64 @"is-script-running?"(i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !5 i64 @wait-one-frame() #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !6 i64 @send-event(i64, i64, i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !7 i64 @internal-put-on-gas-mask-immediately(i64, i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !8 i64 @internal-put-on-gas-mask(i64, i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !9 i64 @get-gas-mask(i64) #0

define i64 @internal-put-on-gas-mask.1(i64 %arg_0, i64 %arg_1) !dcvm.sid_distance !10 {
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
  store i64 %arg_0, ptr %r49, align 4
  store i64 %arg_1, ptr %r50, align 4
  %__Move_0___load_op1 = load i64, ptr %r49, align 4
  store i64 %__Move_0___load_op1, ptr %r0, align 4
  %__Move_1___load_op1 = load i64, ptr %r50, align 4
  store i64 %__Move_1___load_op1, ptr %r1, align 4
  store i64 ptrtoint (ptr @wait-one-frame to i64), ptr %r2, align 4
  %__CallFf_3___callee = load i64, ptr %r2, align 4
  %0 = inttoptr i64 %__CallFf_3___callee to ptr
  %__CallFf_3___call = call i64 %0(), !dcvm.distance !1
  store i64 %__CallFf_3___call, ptr %r2, align 4
  store i64 ptrtoint (ptr @lookup-symbol to i64), ptr %r2, align 4
  store i64 ptrtoint (ptr @get-gas-mask-setting-id to i64), ptr %r3, align 4
  %__Move_6___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_6___load_op1, ptr %r4, align 4
  %__Move_7___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_7___load_op1, ptr %r49, align 4
  %__CallFf_8___callee = load i64, ptr %r3, align 4
  %1 = inttoptr i64 %__CallFf_8___callee to ptr
  %__CallFf_8___arg0 = load i64, ptr %r49, align 4
  %__CallFf_8___call = call i64 %1(i64 %__CallFf_8___arg0), !dcvm.distance !1
  store i64 %__CallFf_8___call, ptr %r3, align 4
  %__LoadStaticU64Imm_9___st_load = load i64, ptr getelementptr inbounds ([26 x i64], ptr @internal-put-on-gas-mask_symbol_table_src, i64 0, i64 3), align 4
  store i64 %__LoadStaticU64Imm_9___st_load, ptr %r4, align 4
  %__Move_10___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_10___load_op1, ptr %r49, align 4
  %__Move_11___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_11___load_op1, ptr %r50, align 4
  %__CallFf_12___callee = load i64, ptr %r2, align 4
  %2 = inttoptr i64 %__CallFf_12___callee to ptr
  %__CallFf_12___arg0 = load i64, ptr %r49, align 4
  %__CallFf_12___arg1 = load i64, ptr %r50, align 4
  %__CallFf_12___call = call i64 %2(i64 %__CallFf_12___arg0, i64 %__CallFf_12___arg1), !dcvm.distance !1
  store i64 %__CallFf_12___call, ptr %r2, align 4
  store i64 ptrtoint (ptr @wait-until-in-valid-gas-mask-state to i64), ptr %r3, align 4
  %__Move_14___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_14___load_op1, ptr %r4, align 4
  %__Move_15___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_15___load_op1, ptr %r5, align 4
  %__AssertPointer_16___ptr = load i64, ptr %r5, align 4
  %3 = inttoptr i64 %__AssertPointer_16___ptr to ptr
  %__AssertPointer_16__op = icmp ne ptr %3, null
  %__IAddImm_17___load_lhs = load i64, ptr %r5, align 4
  %4 = inttoptr i64 %__IAddImm_17___load_lhs to ptr
  %__IAddImm_17__op = getelementptr i8, ptr %4, i8 56
  %5 = ptrtoint ptr %__IAddImm_17__op to i64
  store i64 %5, ptr %r5, align 4
  %__LoadU8_18___addr = load i64, ptr %r5, align 4
  %6 = inttoptr i64 %__LoadU8_18___addr to ptr
  %__LoadU8_18___load = load i8, ptr %6, align 1
  %__LoadU8_18___ext = zext i8 %__LoadU8_18___load to i64
  store i64 %__LoadU8_18___ext, ptr %r5, align 4
  %__Move_19___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_19___load_op1, ptr %r49, align 4
  %__Move_20___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_20___load_op1, ptr %r50, align 4
  %__Call_21___callee = load i64, ptr %r3, align 4
  %7 = inttoptr i64 %__Call_21___callee to ptr
  %__Call_21___arg0 = load i64, ptr %r49, align 4
  %__Call_21___arg1 = load i64, ptr %r50, align 4
  %__Call_21___call = call i64 %7(i64 %__Call_21___arg0, i64 %__Call_21___arg1)
  store i64 %__Call_21___call, ptr %r3, align 4
  %__Move_22___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_22___load_op1, ptr %r3, align 4
  store i64 0, ptr %r4, align 4
  %__IEqual_24___load_lhs = load i64, ptr %r3, align 4
  %__IEqual_24___load_rhs = load i64, ptr %r4, align 4
  %__IEqual_24__op = icmp eq i64 %__IEqual_24___load_lhs, %__IEqual_24___load_rhs
  %8 = zext i1 %__IEqual_24__op to i64
  store i64 %8, ptr %r3, align 4
  %__BranchIfNot_25___cond = load i64, ptr %r3, align 4
  %__BranchIfNot_25___bool = icmp ne i64 %__BranchIfNot_25___cond, 0
  br i1 %__BranchIfNot_25___bool, label %bb1, label %bb2

bb1:                                              ; preds = %bb0
  store i64 ptrtoint (ptr @spawn-object to i64), ptr %r3, align 4
  %__Move_27___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_27___load_op1, ptr %r4, align 4
  %__AssertPointer_28___ptr = load i64, ptr %r4, align 4
  %9 = inttoptr i64 %__AssertPointer_28___ptr to ptr
  %__AssertPointer_28__op = icmp ne ptr %9, null
  %__IAddImm_29___load_lhs = load i64, ptr %r4, align 4
  %10 = inttoptr i64 %__IAddImm_29___load_lhs to ptr
  %__IAddImm_29__op = getelementptr i8, ptr %10, i8 40
  %11 = ptrtoint ptr %__IAddImm_29__op to i64
  store i64 %11, ptr %r4, align 4
  %__LoadU64_30___addr = load i64, ptr %r4, align 4
  %12 = inttoptr i64 %__LoadU64_30___addr to ptr
  %__LoadU64_30___load = load i64, ptr %12, align 4
  store i64 %__LoadU64_30___load, ptr %r4, align 4
  %__LoadStaticU64Imm_31___st_load = load i64, ptr getelementptr inbounds ([26 x i64], ptr @internal-put-on-gas-mask_symbol_table_src, i64 0, i64 6), align 4
  store i64 %__LoadStaticU64Imm_31___st_load, ptr %r5, align 4
  store i64 0, ptr %r6, align 4
  store i64 0, ptr %r7, align 4
  store i64 1, ptr %r8, align 4
  store i64 0, ptr %r9, align 4
  store i64 0, ptr %r10, align 4
  store i64 0, ptr %r11, align 4
  store i64 0, ptr %r12, align 4
  %__Move_39___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_39___load_op1, ptr %r49, align 4
  %__Move_40___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_40___load_op1, ptr %r50, align 4
  %__Move_41___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_41___load_op1, ptr %r51, align 4
  %__Move_42___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_42___load_op1, ptr %r52, align 4
  %__Move_43___load_op1 = load i64, ptr %r8, align 4
  store i64 %__Move_43___load_op1, ptr %r53, align 4
  %__Move_44___load_op1 = load i64, ptr %r9, align 4
  store i64 %__Move_44___load_op1, ptr %r54, align 4
  %__Move_45___load_op1 = load i64, ptr %r10, align 4
  store i64 %__Move_45___load_op1, ptr %r55, align 4
  %__Move_46___load_op1 = load i64, ptr %r11, align 4
  store i64 %__Move_46___load_op1, ptr %r56, align 4
  %__Move_47___load_op1 = load i64, ptr %r12, align 4
  store i64 %__Move_47___load_op1, ptr %r57, align 4
  %__CallFf_48___callee = load i64, ptr %r3, align 4
  %13 = inttoptr i64 %__CallFf_48___callee to ptr
  %__CallFf_48___arg0 = load i64, ptr %r49, align 4
  %__CallFf_48___arg1 = load i64, ptr %r50, align 4
  %__CallFf_48___arg2 = load i64, ptr %r51, align 4
  %__CallFf_48___arg3 = load i64, ptr %r52, align 4
  %__CallFf_48___arg4 = load i64, ptr %r53, align 4
  %__CallFf_48___arg5 = load i64, ptr %r54, align 4
  %__CallFf_48___arg6 = load i64, ptr %r55, align 4
  %__CallFf_48___arg7 = load i64, ptr %r56, align 4
  %__CallFf_48___arg8 = load i64, ptr %r57, align 4
  %__CallFf_48___call = call i64 %13(i64 %__CallFf_48___arg0, i64 %__CallFf_48___arg1, i64 %__CallFf_48___arg2, i64 %__CallFf_48___arg3, i64 %__CallFf_48___arg4, i64 %__CallFf_48___arg5, i64 %__CallFf_48___arg6, i64 %__CallFf_48___arg7, i64 %__CallFf_48___arg8), !dcvm.distance !1
  store i64 %__CallFf_48___call, ptr %r3, align 4
  %__Move_49___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_49___load_op1, ptr %r1, align 4
  %__Move_50___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_50___load_op1, ptr %r3, align 4
  br label %bb3

bb2:                                              ; preds = %bb0
  store i64 0, ptr %r3, align 4
  br label %bb3

bb3:                                              ; preds = %bb2, %bb1
  store i64 ptrtoint (ptr @set-high-contrast-mode-type to i64), ptr %r3, align 4
  %__Move_54___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_54___load_op1, ptr %r4, align 4
  store i64 6, ptr %r5, align 4
  %__Move_56___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_56___load_op1, ptr %r49, align 4
  %__Move_57___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_57___load_op1, ptr %r50, align 4
  %__CallFf_58___callee = load i64, ptr %r3, align 4
  %14 = inttoptr i64 %__CallFf_58___callee to ptr
  %__CallFf_58___arg0 = load i64, ptr %r49, align 4
  %__CallFf_58___arg1 = load i64, ptr %r50, align 4
  %__CallFf_58___call = call i64 %14(i64 %__CallFf_58___arg0, i64 %__CallFf_58___arg1), !dcvm.distance !1
  store i64 %__CallFf_58___call, ptr %r3, align 4
  store i64 ptrtoint (ptr @"is-player?" to i64), ptr %r3, align 4
  %__Move_60___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_60___load_op1, ptr %r4, align 4
  %__Move_61___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_61___load_op1, ptr %r49, align 4
  %__CallFf_62___callee = load i64, ptr %r3, align 4
  %15 = inttoptr i64 %__CallFf_62___callee to ptr
  %__CallFf_62___arg0 = load i64, ptr %r49, align 4
  %__CallFf_62___call = call i64 %15(i64 %__CallFf_62___arg0), !dcvm.distance !1
  store i64 %__CallFf_62___call, ptr %r3, align 4
  %__BranchIfNot_63___cond = load i64, ptr %r3, align 4
  %__BranchIfNot_63___bool = icmp ne i64 %__BranchIfNot_63___cond, 0
  br i1 %__BranchIfNot_63___bool, label %bb4, label %bb5

bb4:                                              ; preds = %bb3
  store i64 ptrtoint (ptr @set-instance-flag-hero to i64), ptr %r3, align 4
  %__Move_65___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_65___load_op1, ptr %r4, align 4
  store i64 1, ptr %r5, align 4
  %__Move_67___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_67___load_op1, ptr %r49, align 4
  %__Move_68___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_68___load_op1, ptr %r50, align 4
  %__CallFf_69___callee = load i64, ptr %r3, align 4
  %16 = inttoptr i64 %__CallFf_69___callee to ptr
  %__CallFf_69___arg0 = load i64, ptr %r49, align 4
  %__CallFf_69___arg1 = load i64, ptr %r50, align 4
  %__CallFf_69___call = call i64 %16(i64 %__CallFf_69___arg0, i64 %__CallFf_69___arg1), !dcvm.distance !1
  store i64 %__CallFf_69___call, ptr %r3, align 4
  store i64 ptrtoint (ptr @set-show-in-flashlight-flag to i64), ptr %r3, align 4
  %__Move_71___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_71___load_op1, ptr %r4, align 4
  store i64 0, ptr %r5, align 4
  %__Move_73___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_73___load_op1, ptr %r49, align 4
  %__Move_74___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_74___load_op1, ptr %r50, align 4
  %__CallFf_75___callee = load i64, ptr %r3, align 4
  %17 = inttoptr i64 %__CallFf_75___callee to ptr
  %__CallFf_75___arg0 = load i64, ptr %r49, align 4
  %__CallFf_75___arg1 = load i64, ptr %r50, align 4
  %__CallFf_75___call = call i64 %17(i64 %__CallFf_75___arg0, i64 %__CallFf_75___arg1), !dcvm.distance !1
  store i64 %__CallFf_75___call, ptr %r3, align 4
  br label %bb6

bb5:                                              ; preds = %bb3
  store i64 ptrtoint (ptr @set-shadow-casting-from-npc-flashlight to i64), ptr %r3, align 4
  %__Move_78___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_78___load_op1, ptr %r4, align 4
  store i64 0, ptr %r5, align 4
  %__Move_80___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_80___load_op1, ptr %r49, align 4
  %__Move_81___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_81___load_op1, ptr %r50, align 4
  %__CallFf_82___callee = load i64, ptr %r3, align 4
  %18 = inttoptr i64 %__CallFf_82___callee to ptr
  %__CallFf_82___arg0 = load i64, ptr %r49, align 4
  %__CallFf_82___arg1 = load i64, ptr %r50, align 4
  %__CallFf_82___call = call i64 %18(i64 %__CallFf_82___arg0, i64 %__CallFf_82___arg1), !dcvm.distance !1
  store i64 %__CallFf_82___call, ptr %r3, align 4
  br label %bb6

bb6:                                              ; preds = %bb5, %bb4
  store i64 ptrtoint (ptr @set-gas-mask to i64), ptr %r3, align 4
  %__Move_84___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_84___load_op1, ptr %r4, align 4
  %__Move_85___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_85___load_op1, ptr %r5, align 4
  %__Move_86___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_86___load_op1, ptr %r49, align 4
  %__Move_87___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_87___load_op1, ptr %r50, align 4
  %__CallFf_88___callee = load i64, ptr %r3, align 4
  %19 = inttoptr i64 %__CallFf_88___callee to ptr
  %__CallFf_88___arg0 = load i64, ptr %r49, align 4
  %__CallFf_88___arg1 = load i64, ptr %r50, align 4
  %__CallFf_88___call = call i64 %19(i64 %__CallFf_88___arg0, i64 %__CallFf_88___arg1), !dcvm.distance !1
  store i64 %__CallFf_88___call, ptr %r3, align 4
  store i64 ptrtoint (ptr @set-visible to i64), ptr %r3, align 4
  %__Move_90___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_90___load_op1, ptr %r4, align 4
  store i64 0, ptr %r5, align 4
  %__Move_92___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_92___load_op1, ptr %r49, align 4
  %__Move_93___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_93___load_op1, ptr %r50, align 4
  %__CallFf_94___callee = load i64, ptr %r3, align 4
  %20 = inttoptr i64 %__CallFf_94___callee to ptr
  %__CallFf_94___arg0 = load i64, ptr %r49, align 4
  %__CallFf_94___arg1 = load i64, ptr %r50, align 4
  %__CallFf_94___call = call i64 %20(i64 %__CallFf_94___arg0, i64 %__CallFf_94___arg1), !dcvm.distance !1
  store i64 %__CallFf_94___call, ptr %r3, align 4
  store i64 ptrtoint (ptr @"is-player?" to i64), ptr %r3, align 4
  %__Move_96___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_96___load_op1, ptr %r4, align 4
  %__Move_97___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_97___load_op1, ptr %r49, align 4
  %__CallFf_98___callee = load i64, ptr %r3, align 4
  %21 = inttoptr i64 %__CallFf_98___callee to ptr
  %__CallFf_98___arg0 = load i64, ptr %r49, align 4
  %__CallFf_98___call = call i64 %21(i64 %__CallFf_98___arg0), !dcvm.distance !1
  store i64 %__CallFf_98___call, ptr %r3, align 4
  %__OpLogNot_99___load_op1 = load i64, ptr %r3, align 4
  %__OpLogNot_99__op = icmp eq i64 %__OpLogNot_99___load_op1, 0
  %22 = zext i1 %__OpLogNot_99__op to i64
  store i64 %22, ptr %r3, align 4
  %__BranchIfNot_100___cond = load i64, ptr %r3, align 4
  %__BranchIfNot_100___bool = icmp ne i64 %__BranchIfNot_100___cond, 0
  br i1 %__BranchIfNot_100___bool, label %bb7, label %bb8

bb7:                                              ; preds = %bb6
  store i64 ptrtoint (ptr @wait-one-frame to i64), ptr %r3, align 4
  %__CallFf_102___callee = load i64, ptr %r3, align 4
  %23 = inttoptr i64 %__CallFf_102___callee to ptr
  %__CallFf_102___call = call i64 %23(), !dcvm.distance !1
  store i64 %__CallFf_102___call, ptr %r3, align 4
  br label %bb9

bb8:                                              ; preds = %bb6
  store i64 0, ptr %r3, align 4
  br label %bb9

bb9:                                              ; preds = %bb8, %bb7
  store i64 ptrtoint (ptr @attach to i64), ptr %r3, align 4
  %__Move_106___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_106___load_op1, ptr %r4, align 4
  %__Move_107___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_107___load_op1, ptr %r5, align 4
  %__LoadStaticU64Imm_108___st_load = load i64, ptr getelementptr inbounds ([26 x i64], ptr @internal-put-on-gas-mask_symbol_table_src, i64 0, i64 15), align 4
  store i64 %__LoadStaticU64Imm_108___st_load, ptr %r6, align 4
  %__LoadStaticFloatImm_109___st_load = load float, ptr getelementptr inbounds ([26 x i64], ptr @internal-put-on-gas-mask_symbol_table_src, i64 0, i64 16), align 4
  %24 = bitcast float %__LoadStaticFloatImm_109___st_load to i32
  %25 = zext i32 %24 to i64
  store i64 %25, ptr %r7, align 4
  %__Move_110___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_110___load_op1, ptr %r49, align 4
  %__Move_111___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_111___load_op1, ptr %r50, align 4
  %__Move_112___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_112___load_op1, ptr %r51, align 4
  %__Move_113___load_op1 = load i64, ptr %r7, align 4
  %26 = trunc i64 %__Move_113___load_op1 to i32
  %27 = bitcast i32 %26 to float
  %28 = bitcast float %27 to i32
  %29 = zext i32 %28 to i64
  store i64 %29, ptr %r52, align 4
  %__CallFf_114___callee = load i64, ptr %r3, align 4
  %30 = inttoptr i64 %__CallFf_114___callee to ptr
  %__CallFf_114___arg0 = load i64, ptr %r49, align 4
  %__CallFf_114___arg1 = load i64, ptr %r50, align 4
  %__CallFf_114___arg2 = load i64, ptr %r51, align 4
  %__CallFf_114___arg3 = load i64, ptr %r52, align 4
  %31 = trunc i64 %__CallFf_114___arg3 to i32
  %32 = bitcast i32 %31 to float
  %__CallFf_114___call = call i64 %30(i64 %__CallFf_114___arg0, i64 %__CallFf_114___arg1, i64 %__CallFf_114___arg2, float %32), !dcvm.distance !1
  store i64 %__CallFf_114___call, ptr %r3, align 4
  store i64 ptrtoint (ptr @send-event to i64), ptr %r3, align 4
  %__LoadStaticU64Imm_116___st_load = load i64, ptr getelementptr inbounds ([26 x i64], ptr @internal-put-on-gas-mask_symbol_table_src, i64 0, i64 18), align 4
  store i64 %__LoadStaticU64Imm_116___st_load, ptr %r4, align 4
  %__Move_117___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_117___load_op1, ptr %r5, align 4
  store i64 ptrtoint (ptr @new-boxed-value to i64), ptr %r6, align 4
  store i64 6, ptr %r7, align 4
  %__LoadStaticFloatImm_120___st_load = load float, ptr getelementptr inbounds ([26 x i64], ptr @internal-put-on-gas-mask_symbol_table_src, i64 0, i64 20), align 4
  %33 = bitcast float %__LoadStaticFloatImm_120___st_load to i32
  %34 = zext i32 %33 to i64
  store i64 %34, ptr %r8, align 4
  %__Move_121___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_121___load_op1, ptr %r49, align 4
  %__Move_122___load_op1 = load i64, ptr %r8, align 4
  %35 = trunc i64 %__Move_122___load_op1 to i32
  %36 = bitcast i32 %35 to float
  %37 = bitcast float %36 to i32
  %38 = zext i32 %37 to i64
  store i64 %38, ptr %r50, align 4
  %__CallFf_123___callee = load i64, ptr %r6, align 4
  %39 = inttoptr i64 %__CallFf_123___callee to ptr
  %__CallFf_123___arg0 = load i64, ptr %r49, align 4
  %__CallFf_123___arg1 = load i64, ptr %r50, align 4
  %40 = trunc i64 %__CallFf_123___arg1 to i32
  %41 = bitcast i32 %40 to float
  %__CallFf_123___call = call i64 %39(i64 %__CallFf_123___arg0, float %41), !dcvm.distance !1
  store i64 %__CallFf_123___call, ptr %r6, align 4
  %__Move_124___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_124___load_op1, ptr %r49, align 4
  %__Move_125___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_125___load_op1, ptr %r50, align 4
  %__Move_126___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_126___load_op1, ptr %r51, align 4
  %__CallFf_127___callee = load i64, ptr %r3, align 4
  %42 = inttoptr i64 %__CallFf_127___callee to ptr
  %__CallFf_127___arg0 = load i64, ptr %r49, align 4
  %__CallFf_127___arg1 = load i64, ptr %r50, align 4
  %__CallFf_127___arg2 = load i64, ptr %r51, align 4
  %__CallFf_127___call = call i64 %42(i64 %__CallFf_127___arg0, i64 %__CallFf_127___arg1, i64 %__CallFf_127___arg2), !dcvm.distance !1
  store i64 %__CallFf_127___call, ptr %r3, align 4
  store i64 ptrtoint (ptr @animate_ to i64), ptr %r3, align 4
  %__Move_129___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_129___load_op1, ptr %r4, align 4
  %__Move_130___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_130___load_op1, ptr %r5, align 4
  %__AssertPointer_131___ptr = load i64, ptr %r5, align 4
  %43 = inttoptr i64 %__AssertPointer_131___ptr to ptr
  %__AssertPointer_131__op = icmp ne ptr %43, null
  %__IAddImm_132___load_lhs = load i64, ptr %r5, align 4
  %44 = inttoptr i64 %__IAddImm_132___load_lhs to ptr
  %__IAddImm_132__op = getelementptr i8, ptr %44, i8 16
  %45 = ptrtoint ptr %__IAddImm_132__op to i64
  store i64 %45, ptr %r5, align 4
  %__LoadU64_133___addr = load i64, ptr %r5, align 4
  %46 = inttoptr i64 %__LoadU64_133___addr to ptr
  %__LoadU64_133___load = load i64, ptr %46, align 4
  store i64 %__LoadU64_133___load, ptr %r5, align 4
  store i64 ptrtoint (ptr @"#%alloc-array" to i64), ptr %r6, align 4
  %__LoadStaticU64Imm_135___st_load = load i64, ptr getelementptr inbounds ([26 x i64], ptr @internal-put-on-gas-mask_symbol_table_src, i64 0, i64 23), align 4
  store i64 %__LoadStaticU64Imm_135___st_load, ptr %r7, align 4
  store i64 16, ptr %r8, align 4
  store i64 0, ptr %r9, align 4
  store i64 1, ptr %r10, align 4
  %__Move_139___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_139___load_op1, ptr %r49, align 4
  %__Move_140___load_op1 = load i64, ptr %r8, align 4
  store i64 %__Move_140___load_op1, ptr %r50, align 4
  %__Move_141___load_op1 = load i64, ptr %r9, align 4
  store i64 %__Move_141___load_op1, ptr %r51, align 4
  %__Move_142___load_op1 = load i64, ptr %r10, align 4
  store i64 %__Move_142___load_op1, ptr %r52, align 4
  %__CallFf_143___callee = load i64, ptr %r6, align 4
  %47 = inttoptr i64 %__CallFf_143___callee to ptr
  %__CallFf_143___arg0 = load i64, ptr %r49, align 4
  %__CallFf_143___arg1 = load i64, ptr %r50, align 4
  %__CallFf_143___arg2 = load i64, ptr %r51, align 4
  %__CallFf_143___arg3 = load i64, ptr %r52, align 4
  %__CallFf_143___call = call i64 %47(i64 %__CallFf_143___arg0, i64 %__CallFf_143___arg1, i64 %__CallFf_143___arg2, i64 %__CallFf_143___arg3), !dcvm.distance !1
  store i64 %__CallFf_143___call, ptr %r6, align 4
  %__Move_144___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_144___load_op1, ptr %r7, align 4
  %__Move_145___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_145___load_op1, ptr %r6, align 4
  %__Move_146___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_146___load_op1, ptr %r49, align 4
  %__Move_147___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_147___load_op1, ptr %r50, align 4
  %__Move_148___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_148___load_op1, ptr %r51, align 4
  %__CallFf_149___callee = load i64, ptr %r3, align 4
  %48 = inttoptr i64 %__CallFf_149___callee to ptr
  %__CallFf_149___arg0 = load i64, ptr %r49, align 4
  %__CallFf_149___arg1 = load i64, ptr %r50, align 4
  %__CallFf_149___arg2 = load i64, ptr %r51, align 4
  %__CallFf_149___call = call i64 %48(i64 %__CallFf_149___arg0, i64 %__CallFf_149___arg1, i64 %__CallFf_149___arg2), !dcvm.distance !1
  store i64 %__CallFf_149___call, ptr %r3, align 4
  br label %bb10

bb10:                                             ; preds = %bb11, %bb9
  store i64 ptrtoint (ptr @gesture_ to i64), ptr %r3, align 4
  %__Move_151___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_151___load_op1, ptr %r4, align 4
  %__Move_152___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_152___load_op1, ptr %r5, align 4
  %__AssertPointer_153___ptr = load i64, ptr %r5, align 4
  %49 = inttoptr i64 %__AssertPointer_153___ptr to ptr
  %__AssertPointer_153__op = icmp ne ptr %49, null
  %__LoadU64_154___addr = load i64, ptr %r5, align 4
  %50 = inttoptr i64 %__LoadU64_154___addr to ptr
  %__LoadU64_154___load = load i64, ptr %50, align 4
  store i64 %__LoadU64_154___load, ptr %r5, align 4
  store i64 ptrtoint (ptr @alloc-gesture-play-params to i64), ptr %r6, align 4
  %__CallFf_156___callee = load i64, ptr %r6, align 4
  %51 = inttoptr i64 %__CallFf_156___callee to ptr
  %__CallFf_156___call = call i64 %51(), !dcvm.distance !1
  store i64 %__CallFf_156___call, ptr %r6, align 4
  %__Move_157___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_157___load_op1, ptr %r7, align 4
  %__IAddImm_158___load_lhs = load i64, ptr %r7, align 4
  %__IAddImm_158__op = add i64 %__IAddImm_158___load_lhs, 33
  store i64 %__IAddImm_158__op, ptr %r7, align 4
  store i64 1, ptr %r8, align 4
  %__StoreU8_160___value = load i64, ptr %r8, align 4
  %__StoreU8_160___addr = load i64, ptr %r7, align 4
  %52 = inttoptr i64 %__StoreU8_160___addr to ptr
  %__StoreU8_160___trunc = trunc i64 %__StoreU8_160___value to i8
  store i8 %__StoreU8_160___trunc, ptr %52, align 1
  store i64 %__StoreU8_160___value, ptr %r7, align 4
  %__Move_161___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_161___load_op1, ptr %r7, align 4
  %__IAddImm_162___load_lhs = load i64, ptr %r7, align 4
  %__IAddImm_162__op = add i64 %__IAddImm_162___load_lhs, 36
  store i64 %__IAddImm_162__op, ptr %r7, align 4
  store i64 19, ptr %r8, align 4
  %__StoreI32_164___value = load i64, ptr %r8, align 4
  %__StoreI32_164___addr = load i64, ptr %r7, align 4
  %53 = inttoptr i64 %__StoreI32_164___addr to ptr
  %__StoreI32_164___trunc = trunc i64 %__StoreI32_164___value to i32
  store i32 %__StoreI32_164___trunc, ptr %53, align 4
  store i64 %__StoreI32_164___value, ptr %r7, align 4
  %__Move_165___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_165___load_op1, ptr %r7, align 4
  %__IAddImm_166___load_lhs = load i64, ptr %r7, align 4
  %__IAddImm_166__op = add i64 %__IAddImm_166___load_lhs, 32
  store i64 %__IAddImm_166__op, ptr %r7, align 4
  store i64 1, ptr %r8, align 4
  %__StoreU8_168___value = load i64, ptr %r8, align 4
  %__StoreU8_168___addr = load i64, ptr %r7, align 4
  %54 = inttoptr i64 %__StoreU8_168___addr to ptr
  %__StoreU8_168___trunc = trunc i64 %__StoreU8_168___value to i8
  store i8 %__StoreU8_168___trunc, ptr %54, align 1
  store i64 %__StoreU8_168___value, ptr %r7, align 4
  %__Move_169___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_169___load_op1, ptr %r7, align 4
  %__IAddImm_170___load_lhs = load i64, ptr %r7, align 4
  %__IAddImm_170__op = add i64 %__IAddImm_170___load_lhs, 34
  store i64 %__IAddImm_170__op, ptr %r7, align 4
  store i64 1, ptr %r8, align 4
  %__StoreU8_172___value = load i64, ptr %r8, align 4
  %__StoreU8_172___addr = load i64, ptr %r7, align 4
  %55 = inttoptr i64 %__StoreU8_172___addr to ptr
  %__StoreU8_172___trunc = trunc i64 %__StoreU8_172___value to i8
  store i8 %__StoreU8_172___trunc, ptr %55, align 1
  store i64 %__StoreU8_172___value, ptr %r7, align 4
  %__Move_173___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_173___load_op1, ptr %r7, align 4
  %__Move_174___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_174___load_op1, ptr %r6, align 4
  %__Move_175___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_175___load_op1, ptr %r49, align 4
  %__Move_176___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_176___load_op1, ptr %r50, align 4
  %__Move_177___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_177___load_op1, ptr %r51, align 4
  %__CallFf_178___callee = load i64, ptr %r3, align 4
  %56 = inttoptr i64 %__CallFf_178___callee to ptr
  %__CallFf_178___arg0 = load i64, ptr %r49, align 4
  %__CallFf_178___arg1 = load i64, ptr %r50, align 4
  %__CallFf_178___arg2 = load i64, ptr %r51, align 4
  %__CallFf_178___call = call i64 %56(i64 %__CallFf_178___arg0, i64 %__CallFf_178___arg1, i64 %__CallFf_178___arg2), !dcvm.distance !1
  store i64 %__CallFf_178___call, ptr %r3, align 4
  store i64 0, ptr %r4, align 4
  %__IEqual_180___load_lhs = load i64, ptr %r3, align 4
  %__IEqual_180___load_rhs = load i64, ptr %r4, align 4
  %__IEqual_180__op = icmp eq i64 %__IEqual_180___load_lhs, %__IEqual_180___load_rhs
  %57 = zext i1 %__IEqual_180__op to i64
  store i64 %57, ptr %r3, align 4
  %__OpLogNot_181___load_op1 = load i64, ptr %r3, align 4
  %58 = trunc i64 %__OpLogNot_181___load_op1 to i1
  %__OpLogNot_181__op = icmp eq i1 %58, false
  %59 = zext i1 %__OpLogNot_181__op to i64
  store i64 %59, ptr %r3, align 4
  %__BranchIfNot_182___cond = load i64, ptr %r3, align 4
  %__BranchIfNot_182___bool = icmp ne i64 %__BranchIfNot_182___cond, 0
  br i1 %__BranchIfNot_182___bool, label %bb11, label %bb12

bb11:                                             ; preds = %bb10
  store i64 ptrtoint (ptr @wait-one-frame to i64), ptr %r3, align 4
  %__CallFf_184___callee = load i64, ptr %r3, align 4
  %60 = inttoptr i64 %__CallFf_184___callee to ptr
  %__CallFf_184___call = call i64 %60(), !dcvm.distance !1
  store i64 %__CallFf_184___call, ptr %r3, align 4
  br label %bb10

bb12:                                             ; preds = %bb10
  store i64 ptrtoint (ptr @animate_ to i64), ptr %r3, align 4
  %__Move_187___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_187___load_op1, ptr %r4, align 4
  %__Move_188___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_188___load_op1, ptr %r5, align 4
  %__AssertPointer_189___ptr = load i64, ptr %r5, align 4
  %61 = inttoptr i64 %__AssertPointer_189___ptr to ptr
  %__AssertPointer_189__op = icmp ne ptr %61, null
  %__IAddImm_190___load_lhs = load i64, ptr %r5, align 4
  %62 = inttoptr i64 %__IAddImm_190___load_lhs to ptr
  %__IAddImm_190__op = getelementptr i8, ptr %62, i8 24
  %63 = ptrtoint ptr %__IAddImm_190__op to i64
  store i64 %63, ptr %r5, align 4
  %__LoadU64_191___addr = load i64, ptr %r5, align 4
  %64 = inttoptr i64 %__LoadU64_191___addr to ptr
  %__LoadU64_191___load = load i64, ptr %64, align 4
  store i64 %__LoadU64_191___load, ptr %r5, align 4
  store i64 ptrtoint (ptr @"#%alloc-array" to i64), ptr %r6, align 4
  %__LoadStaticU64Imm_193___st_load = load i64, ptr getelementptr inbounds ([26 x i64], ptr @internal-put-on-gas-mask_symbol_table_src, i64 0, i64 23), align 4
  store i64 %__LoadStaticU64Imm_193___st_load, ptr %r7, align 4
  store i64 16, ptr %r8, align 4
  store i64 0, ptr %r9, align 4
  store i64 1, ptr %r10, align 4
  %__Move_197___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_197___load_op1, ptr %r49, align 4
  %__Move_198___load_op1 = load i64, ptr %r8, align 4
  store i64 %__Move_198___load_op1, ptr %r50, align 4
  %__Move_199___load_op1 = load i64, ptr %r9, align 4
  store i64 %__Move_199___load_op1, ptr %r51, align 4
  %__Move_200___load_op1 = load i64, ptr %r10, align 4
  store i64 %__Move_200___load_op1, ptr %r52, align 4
  %__CallFf_201___callee = load i64, ptr %r6, align 4
  %65 = inttoptr i64 %__CallFf_201___callee to ptr
  %__CallFf_201___arg0 = load i64, ptr %r49, align 4
  %__CallFf_201___arg1 = load i64, ptr %r50, align 4
  %__CallFf_201___arg2 = load i64, ptr %r51, align 4
  %__CallFf_201___arg3 = load i64, ptr %r52, align 4
  %__CallFf_201___call = call i64 %65(i64 %__CallFf_201___arg0, i64 %__CallFf_201___arg1, i64 %__CallFf_201___arg2, i64 %__CallFf_201___arg3), !dcvm.distance !1
  store i64 %__CallFf_201___call, ptr %r6, align 4
  %__Move_202___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_202___load_op1, ptr %r7, align 4
  %__Move_203___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_203___load_op1, ptr %r6, align 4
  %__Move_204___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_204___load_op1, ptr %r49, align 4
  %__Move_205___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_205___load_op1, ptr %r50, align 4
  %__Move_206___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_206___load_op1, ptr %r51, align 4
  %__CallFf_207___callee = load i64, ptr %r3, align 4
  %66 = inttoptr i64 %__CallFf_207___callee to ptr
  %__CallFf_207___arg0 = load i64, ptr %r49, align 4
  %__CallFf_207___arg1 = load i64, ptr %r50, align 4
  %__CallFf_207___arg2 = load i64, ptr %r51, align 4
  %__CallFf_207___call = call i64 %66(i64 %__CallFf_207___arg0, i64 %__CallFf_207___arg1, i64 %__CallFf_207___arg2), !dcvm.distance !1
  store i64 %__CallFf_207___call, ptr %r3, align 4
  %__Move_208___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_208___load_op1, ptr %r2, align 4
  %__Return_209___retval = load i64, ptr %r2, align 4
  ret i64 %__Return_209___retval
}

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !11 i64 @lookup-symbol(i64, i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !12 i64 @get-gas-mask-setting-id(i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !13 i64 @wait-until-in-valid-gas-mask-state(i64, i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !14 i64 @spawn-object(i64, i64, i64, i64, i64, i64, i64, i64, i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !15 i64 @set-high-contrast-mode-type(i64, i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !16 i64 @"is-player?"(i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !17 i64 @set-instance-flag-hero(i64, i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !18 i64 @set-show-in-flashlight-flag(i64, i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !19 i64 @set-shadow-casting-from-npc-flashlight(i64, i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !20 i64 @set-gas-mask(i64, i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !21 i64 @set-visible(i64, i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !22 i64 @attach(i64, i64, i64, float) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !23 i64 @animate_(i64, i64, i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !24 i64 @"#%alloc-array"(i64, i64, i64, i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !25 i64 @gesture_(i64, i64, i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !26 i64 @alloc-gesture-play-params() #0

define i64 @internal-put-on-gas-mask-immediately.2(i64 %arg_0, i64 %arg_1) !dcvm.sid_distance !27 {
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
  store i64 %arg_0, ptr %r49, align 4
  store i64 %arg_1, ptr %r50, align 4
  %__Move_0___load_op1 = load i64, ptr %r49, align 4
  store i64 %__Move_0___load_op1, ptr %r0, align 4
  %__Move_1___load_op1 = load i64, ptr %r50, align 4
  store i64 %__Move_1___load_op1, ptr %r1, align 4
  store i64 ptrtoint (ptr @lookup-symbol to i64), ptr %r2, align 4
  store i64 ptrtoint (ptr @get-gas-mask-setting-id to i64), ptr %r3, align 4
  %__Move_4___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_4___load_op1, ptr %r4, align 4
  %__Move_5___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_5___load_op1, ptr %r49, align 4
  %__CallFf_6___callee = load i64, ptr %r3, align 4
  %0 = inttoptr i64 %__CallFf_6___callee to ptr
  %__CallFf_6___arg0 = load i64, ptr %r49, align 4
  %__CallFf_6___call = call i64 %0(i64 %__CallFf_6___arg0), !dcvm.distance !1
  store i64 %__CallFf_6___call, ptr %r3, align 4
  %__LoadStaticU64Imm_7___st_load = load i64, ptr getelementptr inbounds ([19 x i64], ptr @internal-put-on-gas-mask-immediately_symbol_table_src, i64 0, i64 2), align 4
  store i64 %__LoadStaticU64Imm_7___st_load, ptr %r4, align 4
  %__Move_8___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_8___load_op1, ptr %r49, align 4
  %__Move_9___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_9___load_op1, ptr %r50, align 4
  %__CallFf_10___callee = load i64, ptr %r2, align 4
  %1 = inttoptr i64 %__CallFf_10___callee to ptr
  %__CallFf_10___arg0 = load i64, ptr %r49, align 4
  %__CallFf_10___arg1 = load i64, ptr %r50, align 4
  %__CallFf_10___call = call i64 %1(i64 %__CallFf_10___arg0, i64 %__CallFf_10___arg1), !dcvm.distance !1
  store i64 %__CallFf_10___call, ptr %r2, align 4
  %__Move_11___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_11___load_op1, ptr %r3, align 4
  store i64 0, ptr %r4, align 4
  %__IEqual_13___load_lhs = load i64, ptr %r3, align 4
  %__IEqual_13___load_rhs = load i64, ptr %r4, align 4
  %__IEqual_13__op = icmp eq i64 %__IEqual_13___load_lhs, %__IEqual_13___load_rhs
  %2 = zext i1 %__IEqual_13__op to i64
  store i64 %2, ptr %r3, align 4
  %__BranchIfNot_14___cond = load i64, ptr %r3, align 4
  %__BranchIfNot_14___bool = icmp ne i64 %__BranchIfNot_14___cond, 0
  br i1 %__BranchIfNot_14___bool, label %bb1, label %bb2

bb1:                                              ; preds = %bb0
  store i64 ptrtoint (ptr @spawn-object to i64), ptr %r3, align 4
  %__Move_16___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_16___load_op1, ptr %r4, align 4
  %__AssertPointer_17___ptr = load i64, ptr %r4, align 4
  %3 = inttoptr i64 %__AssertPointer_17___ptr to ptr
  %__AssertPointer_17__op = icmp ne ptr %3, null
  %__IAddImm_18___load_lhs = load i64, ptr %r4, align 4
  %4 = inttoptr i64 %__IAddImm_18___load_lhs to ptr
  %__IAddImm_18__op = getelementptr i8, ptr %4, i8 40
  %5 = ptrtoint ptr %__IAddImm_18__op to i64
  store i64 %5, ptr %r4, align 4
  %__LoadU64_19___addr = load i64, ptr %r4, align 4
  %6 = inttoptr i64 %__LoadU64_19___addr to ptr
  %__LoadU64_19___load = load i64, ptr %6, align 4
  store i64 %__LoadU64_19___load, ptr %r4, align 4
  %__LoadStaticU64Imm_20___st_load = load i64, ptr getelementptr inbounds ([19 x i64], ptr @internal-put-on-gas-mask-immediately_symbol_table_src, i64 0, i64 4), align 4
  store i64 %__LoadStaticU64Imm_20___st_load, ptr %r5, align 4
  store i64 0, ptr %r6, align 4
  store i64 0, ptr %r7, align 4
  store i64 1, ptr %r8, align 4
  store i64 0, ptr %r9, align 4
  store i64 0, ptr %r10, align 4
  store i64 0, ptr %r11, align 4
  store i64 0, ptr %r12, align 4
  %__Move_28___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_28___load_op1, ptr %r49, align 4
  %__Move_29___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_29___load_op1, ptr %r50, align 4
  %__Move_30___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_30___load_op1, ptr %r51, align 4
  %__Move_31___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_31___load_op1, ptr %r52, align 4
  %__Move_32___load_op1 = load i64, ptr %r8, align 4
  store i64 %__Move_32___load_op1, ptr %r53, align 4
  %__Move_33___load_op1 = load i64, ptr %r9, align 4
  store i64 %__Move_33___load_op1, ptr %r54, align 4
  %__Move_34___load_op1 = load i64, ptr %r10, align 4
  store i64 %__Move_34___load_op1, ptr %r55, align 4
  %__Move_35___load_op1 = load i64, ptr %r11, align 4
  store i64 %__Move_35___load_op1, ptr %r56, align 4
  %__Move_36___load_op1 = load i64, ptr %r12, align 4
  store i64 %__Move_36___load_op1, ptr %r57, align 4
  %__CallFf_37___callee = load i64, ptr %r3, align 4
  %7 = inttoptr i64 %__CallFf_37___callee to ptr
  %__CallFf_37___arg0 = load i64, ptr %r49, align 4
  %__CallFf_37___arg1 = load i64, ptr %r50, align 4
  %__CallFf_37___arg2 = load i64, ptr %r51, align 4
  %__CallFf_37___arg3 = load i64, ptr %r52, align 4
  %__CallFf_37___arg4 = load i64, ptr %r53, align 4
  %__CallFf_37___arg5 = load i64, ptr %r54, align 4
  %__CallFf_37___arg6 = load i64, ptr %r55, align 4
  %__CallFf_37___arg7 = load i64, ptr %r56, align 4
  %__CallFf_37___arg8 = load i64, ptr %r57, align 4
  %__CallFf_37___call = call i64 %7(i64 %__CallFf_37___arg0, i64 %__CallFf_37___arg1, i64 %__CallFf_37___arg2, i64 %__CallFf_37___arg3, i64 %__CallFf_37___arg4, i64 %__CallFf_37___arg5, i64 %__CallFf_37___arg6, i64 %__CallFf_37___arg7, i64 %__CallFf_37___arg8), !dcvm.distance !1
  store i64 %__CallFf_37___call, ptr %r3, align 4
  %__Move_38___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_38___load_op1, ptr %r1, align 4
  %__Move_39___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_39___load_op1, ptr %r3, align 4
  br label %bb3

bb2:                                              ; preds = %bb0
  store i64 0, ptr %r3, align 4
  br label %bb3

bb3:                                              ; preds = %bb2, %bb1
  store i64 ptrtoint (ptr @set-high-contrast-mode-type to i64), ptr %r3, align 4
  %__Move_43___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_43___load_op1, ptr %r4, align 4
  store i64 6, ptr %r5, align 4
  %__Move_45___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_45___load_op1, ptr %r49, align 4
  %__Move_46___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_46___load_op1, ptr %r50, align 4
  %__CallFf_47___callee = load i64, ptr %r3, align 4
  %8 = inttoptr i64 %__CallFf_47___callee to ptr
  %__CallFf_47___arg0 = load i64, ptr %r49, align 4
  %__CallFf_47___arg1 = load i64, ptr %r50, align 4
  %__CallFf_47___call = call i64 %8(i64 %__CallFf_47___arg0, i64 %__CallFf_47___arg1), !dcvm.distance !1
  store i64 %__CallFf_47___call, ptr %r3, align 4
  store i64 ptrtoint (ptr @set-gas-mask to i64), ptr %r3, align 4
  %__Move_49___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_49___load_op1, ptr %r4, align 4
  %__Move_50___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_50___load_op1, ptr %r5, align 4
  %__Move_51___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_51___load_op1, ptr %r49, align 4
  %__Move_52___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_52___load_op1, ptr %r50, align 4
  %__CallFf_53___callee = load i64, ptr %r3, align 4
  %9 = inttoptr i64 %__CallFf_53___callee to ptr
  %__CallFf_53___arg0 = load i64, ptr %r49, align 4
  %__CallFf_53___arg1 = load i64, ptr %r50, align 4
  %__CallFf_53___call = call i64 %9(i64 %__CallFf_53___arg0, i64 %__CallFf_53___arg1), !dcvm.distance !1
  store i64 %__CallFf_53___call, ptr %r3, align 4
  store i64 ptrtoint (ptr @"is-player?" to i64), ptr %r3, align 4
  %__Move_55___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_55___load_op1, ptr %r4, align 4
  %__Move_56___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_56___load_op1, ptr %r49, align 4
  %__CallFf_57___callee = load i64, ptr %r3, align 4
  %10 = inttoptr i64 %__CallFf_57___callee to ptr
  %__CallFf_57___arg0 = load i64, ptr %r49, align 4
  %__CallFf_57___call = call i64 %10(i64 %__CallFf_57___arg0), !dcvm.distance !1
  store i64 %__CallFf_57___call, ptr %r3, align 4
  %__BranchIfNot_58___cond = load i64, ptr %r3, align 4
  %__BranchIfNot_58___bool = icmp ne i64 %__BranchIfNot_58___cond, 0
  br i1 %__BranchIfNot_58___bool, label %bb4, label %bb5

bb4:                                              ; preds = %bb3
  store i64 ptrtoint (ptr @send-event to i64), ptr %r3, align 4
  %__LoadStaticU64Imm_60___st_load = load i64, ptr getelementptr inbounds ([19 x i64], ptr @internal-put-on-gas-mask-immediately_symbol_table_src, i64 0, i64 9), align 4
  store i64 %__LoadStaticU64Imm_60___st_load, ptr %r4, align 4
  %__Move_61___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_61___load_op1, ptr %r5, align 4
  store i64 ptrtoint (ptr @new-boxed-value to i64), ptr %r6, align 4
  store i64 6, ptr %r7, align 4
  %__LoadStaticFloatImm_64___st_load = load float, ptr getelementptr inbounds ([19 x i64], ptr @internal-put-on-gas-mask-immediately_symbol_table_src, i64 0, i64 11), align 4
  %11 = bitcast float %__LoadStaticFloatImm_64___st_load to i32
  %12 = zext i32 %11 to i64
  store i64 %12, ptr %r8, align 4
  %__Move_65___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_65___load_op1, ptr %r49, align 4
  %__Move_66___load_op1 = load i64, ptr %r8, align 4
  %13 = trunc i64 %__Move_66___load_op1 to i32
  %14 = bitcast i32 %13 to float
  %15 = bitcast float %14 to i32
  %16 = zext i32 %15 to i64
  store i64 %16, ptr %r50, align 4
  %__CallFf_67___callee = load i64, ptr %r6, align 4
  %17 = inttoptr i64 %__CallFf_67___callee to ptr
  %__CallFf_67___arg0 = load i64, ptr %r49, align 4
  %__CallFf_67___arg1 = load i64, ptr %r50, align 4
  %18 = trunc i64 %__CallFf_67___arg1 to i32
  %19 = bitcast i32 %18 to float
  %__CallFf_67___call = call i64 %17(i64 %__CallFf_67___arg0, float %19), !dcvm.distance !1
  store i64 %__CallFf_67___call, ptr %r6, align 4
  %__Move_68___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_68___load_op1, ptr %r49, align 4
  %__Move_69___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_69___load_op1, ptr %r50, align 4
  %__Move_70___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_70___load_op1, ptr %r51, align 4
  %__CallFf_71___callee = load i64, ptr %r3, align 4
  %20 = inttoptr i64 %__CallFf_71___callee to ptr
  %__CallFf_71___arg0 = load i64, ptr %r49, align 4
  %__CallFf_71___arg1 = load i64, ptr %r50, align 4
  %__CallFf_71___arg2 = load i64, ptr %r51, align 4
  %__CallFf_71___call = call i64 (i64, i64, ...) %20(i64 %__CallFf_71___arg0, i64 %__CallFf_71___arg1, i64 %__CallFf_71___arg2), !dcvm.distance !1
  store i64 %__CallFf_71___call, ptr %r3, align 4
  store i64 ptrtoint (ptr @set-instance-flag-hero to i64), ptr %r3, align 4
  %__Move_73___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_73___load_op1, ptr %r4, align 4
  store i64 1, ptr %r5, align 4
  %__Move_75___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_75___load_op1, ptr %r49, align 4
  %__Move_76___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_76___load_op1, ptr %r50, align 4
  %__CallFf_77___callee = load i64, ptr %r3, align 4
  %21 = inttoptr i64 %__CallFf_77___callee to ptr
  %__CallFf_77___arg0 = load i64, ptr %r49, align 4
  %__CallFf_77___arg1 = load i64, ptr %r50, align 4
  %__CallFf_77___call = call i64 %21(i64 %__CallFf_77___arg0, i64 %__CallFf_77___arg1), !dcvm.distance !1
  store i64 %__CallFf_77___call, ptr %r3, align 4
  br label %bb6

bb5:                                              ; preds = %bb3
  store i64 0, ptr %r3, align 4
  br label %bb6

bb6:                                              ; preds = %bb5, %bb4
  store i64 ptrtoint (ptr @send-event to i64), ptr %r3, align 4
  %__LoadStaticU64Imm_81___st_load = load i64, ptr getelementptr inbounds ([19 x i64], ptr @internal-put-on-gas-mask-immediately_symbol_table_src, i64 0, i64 13), align 4
  store i64 %__LoadStaticU64Imm_81___st_load, ptr %r4, align 4
  %__Move_82___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_82___load_op1, ptr %r5, align 4
  %__Move_83___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_83___load_op1, ptr %r49, align 4
  %__Move_84___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_84___load_op1, ptr %r50, align 4
  %__CallFf_85___callee = load i64, ptr %r3, align 4
  %22 = inttoptr i64 %__CallFf_85___callee to ptr
  %__CallFf_85___arg0 = load i64, ptr %r49, align 4
  %__CallFf_85___arg1 = load i64, ptr %r50, align 4
  %__CallFf_85___call = call i64 (i64, i64, ...) %22(i64 %__CallFf_85___arg0, i64 %__CallFf_85___arg1), !dcvm.distance !1
  store i64 %__CallFf_85___call, ptr %r3, align 4
  store i64 ptrtoint (ptr @"is-player?" to i64), ptr %r3, align 4
  %__Move_87___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_87___load_op1, ptr %r4, align 4
  %__Move_88___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_88___load_op1, ptr %r49, align 4
  %__CallFf_89___callee = load i64, ptr %r3, align 4
  %23 = inttoptr i64 %__CallFf_89___callee to ptr
  %__CallFf_89___arg0 = load i64, ptr %r49, align 4
  %__CallFf_89___call = call i64 %23(i64 %__CallFf_89___arg0), !dcvm.distance !1
  store i64 %__CallFf_89___call, ptr %r3, align 4
  %__BranchIfNot_90___cond = load i64, ptr %r3, align 4
  %__BranchIfNot_90___bool = icmp ne i64 %__BranchIfNot_90___cond, 0
  br i1 %__BranchIfNot_90___bool, label %bb7, label %bb8

bb7:                                              ; preds = %bb6
  store i64 ptrtoint (ptr @set-show-in-flashlight-flag to i64), ptr %r3, align 4
  %__Move_92___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_92___load_op1, ptr %r4, align 4
  store i64 0, ptr %r5, align 4
  %__Move_94___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_94___load_op1, ptr %r49, align 4
  %__Move_95___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_95___load_op1, ptr %r50, align 4
  %__CallFf_96___callee = load i64, ptr %r3, align 4
  %24 = inttoptr i64 %__CallFf_96___callee to ptr
  %__CallFf_96___arg0 = load i64, ptr %r49, align 4
  %__CallFf_96___arg1 = load i64, ptr %r50, align 4
  %__CallFf_96___call = call i64 %24(i64 %__CallFf_96___arg0, i64 %__CallFf_96___arg1), !dcvm.distance !1
  store i64 %__CallFf_96___call, ptr %r3, align 4
  br label %bb9

bb8:                                              ; preds = %bb6
  store i64 ptrtoint (ptr @set-shadow-casting-from-npc-flashlight to i64), ptr %r3, align 4
  %__Move_99___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_99___load_op1, ptr %r4, align 4
  store i64 0, ptr %r5, align 4
  %__Move_101___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_101___load_op1, ptr %r49, align 4
  %__Move_102___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_102___load_op1, ptr %r50, align 4
  %__CallFf_103___callee = load i64, ptr %r3, align 4
  %25 = inttoptr i64 %__CallFf_103___callee to ptr
  %__CallFf_103___arg0 = load i64, ptr %r49, align 4
  %__CallFf_103___arg1 = load i64, ptr %r50, align 4
  %__CallFf_103___call = call i64 %25(i64 %__CallFf_103___arg0, i64 %__CallFf_103___arg1), !dcvm.distance !1
  store i64 %__CallFf_103___call, ptr %r3, align 4
  br label %bb9

bb9:                                              ; preds = %bb8, %bb7
  store i64 ptrtoint (ptr @animate_ to i64), ptr %r3, align 4
  %__Move_105___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_105___load_op1, ptr %r4, align 4
  %__Move_106___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_106___load_op1, ptr %r5, align 4
  %__AssertPointer_107___ptr = load i64, ptr %r5, align 4
  %26 = inttoptr i64 %__AssertPointer_107___ptr to ptr
  %__AssertPointer_107__op = icmp ne ptr %26, null
  %__IAddImm_108___load_lhs = load i64, ptr %r5, align 4
  %27 = inttoptr i64 %__IAddImm_108___load_lhs to ptr
  %__IAddImm_108__op = getelementptr i8, ptr %27, i8 24
  %28 = ptrtoint ptr %__IAddImm_108__op to i64
  store i64 %28, ptr %r5, align 4
  %__LoadU64_109___addr = load i64, ptr %r5, align 4
  %29 = inttoptr i64 %__LoadU64_109___addr to ptr
  %__LoadU64_109___load = load i64, ptr %29, align 4
  store i64 %__LoadU64_109___load, ptr %r5, align 4
  store i64 ptrtoint (ptr @"#%alloc-array" to i64), ptr %r6, align 4
  %__LoadStaticU64Imm_111___st_load = load i64, ptr getelementptr inbounds ([19 x i64], ptr @internal-put-on-gas-mask-immediately_symbol_table_src, i64 0, i64 18), align 4
  store i64 %__LoadStaticU64Imm_111___st_load, ptr %r7, align 4
  store i64 16, ptr %r8, align 4
  store i64 1, ptr %r9, align 4
  store i64 1, ptr %r10, align 4
  %__Move_115___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_115___load_op1, ptr %r49, align 4
  %__Move_116___load_op1 = load i64, ptr %r8, align 4
  store i64 %__Move_116___load_op1, ptr %r50, align 4
  %__Move_117___load_op1 = load i64, ptr %r9, align 4
  store i64 %__Move_117___load_op1, ptr %r51, align 4
  %__Move_118___load_op1 = load i64, ptr %r10, align 4
  store i64 %__Move_118___load_op1, ptr %r52, align 4
  %__CallFf_119___callee = load i64, ptr %r6, align 4
  %30 = inttoptr i64 %__CallFf_119___callee to ptr
  %__CallFf_119___arg0 = load i64, ptr %r49, align 4
  %__CallFf_119___arg1 = load i64, ptr %r50, align 4
  %__CallFf_119___arg2 = load i64, ptr %r51, align 4
  %__CallFf_119___arg3 = load i64, ptr %r52, align 4
  %__CallFf_119___call = call i64 %30(i64 %__CallFf_119___arg0, i64 %__CallFf_119___arg1, i64 %__CallFf_119___arg2, i64 %__CallFf_119___arg3), !dcvm.distance !1
  store i64 %__CallFf_119___call, ptr %r6, align 4
  %__Move_120___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_120___load_op1, ptr %r7, align 4
  %__LoadPointer_121___addr = load i64, ptr %r7, align 4
  %31 = inttoptr i64 %__LoadPointer_121___addr to ptr
  %__LoadPointer_121___load = load ptr, ptr %31, align 8
  %32 = ptrtoint ptr %__LoadPointer_121___load to i64
  store i64 %32, ptr %r7, align 4
  %__LoadPointer_121___value = load i64, ptr %r0, align 4
  %__LoadPointer_121___addr1 = load i64, ptr %r7, align 4
  %33 = inttoptr i64 %__LoadPointer_121___addr1 to ptr
  %__LoadPointer_121___trunc = trunc i64 %__LoadPointer_121___value to i8
  store i8 %__LoadPointer_121___trunc, ptr %33, align 1
  store i64 %__LoadPointer_121___value, ptr %r7, align 4
  store i64 0, ptr %r8, align 4
  %__IMulImm_123___load_lhs = load i64, ptr %r8, align 4
  %__IMulImm_123__op = mul i64 %__IMulImm_123___load_lhs, 8
  store i64 %__IMulImm_123__op, ptr %r8, align 4
  %__IAdd_124___load_lhs = load i64, ptr %r7, align 4
  %__IAdd_124___load_rhs = load i64, ptr %r8, align 4
  %__IAdd_124__op = add i64 %__IAdd_124___load_lhs, %__IAdd_124___load_rhs
  store i64 %__IAdd_124__op, ptr %r7, align 4
  %__LoadPointer_125___addr = load i64, ptr %r7, align 4
  %34 = inttoptr i64 %__LoadPointer_125___addr to ptr
  %__LoadPointer_125___load = load ptr, ptr %34, align 8
  %35 = ptrtoint ptr %__LoadPointer_125___load to i64
  store i64 %35, ptr %r7, align 4
  %__LoadPointer_125___value = load i64, ptr %r0, align 4
  %__LoadPointer_125___addr2 = load i64, ptr %r7, align 4
  %36 = inttoptr i64 %__LoadPointer_125___addr2 to ptr
  %__LoadPointer_125___trunc = trunc i64 %__LoadPointer_125___value to i8
  store i8 %__LoadPointer_125___trunc, ptr %36, align 1
  store i64 %__LoadPointer_125___value, ptr %r7, align 4
  store i64 31, ptr %r8, align 4
  %__StoreI32_127___value = load i64, ptr %r8, align 4
  %__StoreI32_127___addr = load i64, ptr %r7, align 4
  %37 = inttoptr i64 %__StoreI32_127___addr to ptr
  %__StoreI32_127___trunc = trunc i64 %__StoreI32_127___value to i32
  store i32 %__StoreI32_127___trunc, ptr %37, align 4
  store i64 %__StoreI32_127___value, ptr %r7, align 4
  %__Move_128___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_128___load_op1, ptr %r7, align 4
  %__LoadPointer_129___addr = load i64, ptr %r7, align 4
  %38 = inttoptr i64 %__LoadPointer_129___addr to ptr
  %__LoadPointer_129___load = load ptr, ptr %38, align 8
  %39 = ptrtoint ptr %__LoadPointer_129___load to i64
  store i64 %39, ptr %r7, align 4
  %__LoadPointer_129___value = load i64, ptr %r0, align 4
  %__LoadPointer_129___addr3 = load i64, ptr %r7, align 4
  %40 = inttoptr i64 %__LoadPointer_129___addr3 to ptr
  %__LoadPointer_129___trunc = trunc i64 %__LoadPointer_129___value to i8
  store i8 %__LoadPointer_129___trunc, ptr %40, align 1
  store i64 %__LoadPointer_129___value, ptr %r7, align 4
  store i64 0, ptr %r8, align 4
  %__IMulImm_131___load_lhs = load i64, ptr %r8, align 4
  %__IMulImm_131__op = mul i64 %__IMulImm_131___load_lhs, 8
  store i64 %__IMulImm_131__op, ptr %r8, align 4
  %__IAdd_132___load_lhs = load i64, ptr %r7, align 4
  %__IAdd_132___load_rhs = load i64, ptr %r8, align 4
  %__IAdd_132__op = add i64 %__IAdd_132___load_lhs, %__IAdd_132___load_rhs
  store i64 %__IAdd_132__op, ptr %r7, align 4
  %__LoadPointer_133___addr = load i64, ptr %r7, align 4
  %41 = inttoptr i64 %__LoadPointer_133___addr to ptr
  %__LoadPointer_133___load = load ptr, ptr %41, align 8
  %42 = ptrtoint ptr %__LoadPointer_133___load to i64
  store i64 %42, ptr %r7, align 4
  %__LoadPointer_133___value = load i64, ptr %r0, align 4
  %__LoadPointer_133___addr4 = load i64, ptr %r7, align 4
  %43 = inttoptr i64 %__LoadPointer_133___addr4 to ptr
  %__LoadPointer_133___trunc = trunc i64 %__LoadPointer_133___value to i8
  store i8 %__LoadPointer_133___trunc, ptr %43, align 1
  store i64 %__LoadPointer_133___value, ptr %r7, align 4
  %__IAddImm_134___load_lhs = load i64, ptr %r7, align 4
  %__IAddImm_134__op = add i64 %__IAddImm_134___load_lhs, 8
  store i64 %__IAddImm_134__op, ptr %r7, align 4
  store i64 0, ptr %r8, align 4
  %__StorePointer_136___value = load i64, ptr %r8, align 4
  %__StorePointer_136___addr = load i64, ptr %r7, align 4
  %44 = inttoptr i64 %__StorePointer_136___addr to ptr
  store i64 %__StorePointer_136___value, ptr %44, align 4
  store i64 %__StorePointer_136___value, ptr %r7, align 4
  %__StorePointer_136___st_load = load ptr, ptr getelementptr inbounds ([19 x i64], ptr @internal-put-on-gas-mask-immediately_symbol_table_src, i64 0, i64 7), align 8
  %45 = ptrtoint ptr %__StorePointer_136___st_load to i64
  store i64 %45, ptr %r7, align 4
  %__Move_137___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_137___load_op1, ptr %r7, align 4
  %__LoadPointer_138___addr = load i64, ptr %r7, align 4
  %46 = inttoptr i64 %__LoadPointer_138___addr to ptr
  %__LoadPointer_138___load = load ptr, ptr %46, align 8
  %47 = ptrtoint ptr %__LoadPointer_138___load to i64
  store i64 %47, ptr %r7, align 4
  %__LoadPointer_138___value = load i64, ptr %r0, align 4
  %__LoadPointer_138___addr5 = load i64, ptr %r7, align 4
  %48 = inttoptr i64 %__LoadPointer_138___addr5 to ptr
  %__LoadPointer_138___trunc = trunc i64 %__LoadPointer_138___value to i8
  store i8 %__LoadPointer_138___trunc, ptr %48, align 1
  store i64 %__LoadPointer_138___value, ptr %r7, align 4
  store i64 0, ptr %r8, align 4
  %__IMulImm_140___load_lhs = load i64, ptr %r8, align 4
  %__IMulImm_140__op = mul i64 %__IMulImm_140___load_lhs, 8
  store i64 %__IMulImm_140__op, ptr %r8, align 4
  %__IAdd_141___load_lhs = load i64, ptr %r7, align 4
  %__IAdd_141___load_rhs = load i64, ptr %r8, align 4
  %__IAdd_141__op = add i64 %__IAdd_141___load_lhs, %__IAdd_141___load_rhs
  store i64 %__IAdd_141__op, ptr %r7, align 4
  %__LoadPointer_142___addr = load i64, ptr %r7, align 4
  %49 = inttoptr i64 %__LoadPointer_142___addr to ptr
  %__LoadPointer_142___load = load ptr, ptr %49, align 8
  %50 = ptrtoint ptr %__LoadPointer_142___load to i64
  store i64 %50, ptr %r7, align 4
  %__LoadPointer_142___value = load i64, ptr %r0, align 4
  %__LoadPointer_142___addr6 = load i64, ptr %r7, align 4
  %51 = inttoptr i64 %__LoadPointer_142___addr6 to ptr
  %__LoadPointer_142___trunc = trunc i64 %__LoadPointer_142___value to i8
  store i8 %__LoadPointer_142___trunc, ptr %51, align 1
  store i64 %__LoadPointer_142___value, ptr %r7, align 4
  %__IAddImm_143___load_lhs = load i64, ptr %r7, align 4
  %__IAddImm_143__op = add i64 %__IAddImm_143___load_lhs, 8
  store i64 %__IAddImm_143__op, ptr %r7, align 4
  store i64 1, ptr %r8, align 4
  %__StoreU8_145___value = load i64, ptr %r8, align 4
  %__StoreU8_145___addr = load i64, ptr %r7, align 4
  %52 = inttoptr i64 %__StoreU8_145___addr to ptr
  %__StoreU8_145___trunc = trunc i64 %__StoreU8_145___value to i8
  store i8 %__StoreU8_145___trunc, ptr %52, align 1
  store i64 %__StoreU8_145___value, ptr %r7, align 4
  %__Move_146___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_146___load_op1, ptr %r7, align 4
  %__Move_147___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_147___load_op1, ptr %r6, align 4
  %__Move_148___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_148___load_op1, ptr %r49, align 4
  %__Move_149___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_149___load_op1, ptr %r50, align 4
  %__Move_150___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_150___load_op1, ptr %r51, align 4
  %__CallFf_151___callee = load i64, ptr %r3, align 4
  %53 = inttoptr i64 %__CallFf_151___callee to ptr
  %__CallFf_151___arg0 = load i64, ptr %r49, align 4
  %__CallFf_151___arg1 = load i64, ptr %r50, align 4
  %__CallFf_151___arg2 = load i64, ptr %r51, align 4
  %__CallFf_151___call = call i64 %53(i64 %__CallFf_151___arg0, i64 %__CallFf_151___arg1, i64 %__CallFf_151___arg2), !dcvm.distance !1
  store i64 %__CallFf_151___call, ptr %r3, align 4
  %__Move_152___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_152___load_op1, ptr %r2, align 4
  %__Return_153___retval = load i64, ptr %r2, align 4
  ret i64 %__Return_153___retval
}

define i64 @get-equipped-gas-mask(i64 %arg_0) !dcvm.sid_distance !28 {
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
  store i64 %arg_0, ptr %r49, align 4
  %__Move_0___load_op1 = load i64, ptr %r49, align 4
  store i64 %__Move_0___load_op1, ptr %r0, align 4
  store i64 ptrtoint (ptr @get-gas-mask to i64), ptr %r1, align 4
  %__Move_2___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_2___load_op1, ptr %r2, align 4
  %__Move_3___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_3___load_op1, ptr %r49, align 4
  %__CallFf_4___callee = load i64, ptr %r1, align 4
  %0 = inttoptr i64 %__CallFf_4___callee to ptr
  %__CallFf_4___arg0 = load i64, ptr %r49, align 4
  %__CallFf_4___call = call i64 %0(i64 %__CallFf_4___arg0), !dcvm.distance !1
  store i64 %__CallFf_4___call, ptr %r1, align 4
  %__Return_5___retval = load i64, ptr %r1, align 4
  ret i64 %__Return_5___retval
}

define i64 @wait-until-in-valid-gas-mask-state.3(i64 %arg_0, i64 %arg_1) !dcvm.sid_distance !29 {
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
  store i64 %arg_0, ptr %r49, align 4
  store i64 %arg_1, ptr %r50, align 4
  %__Move_0___load_op1 = load i64, ptr %r49, align 4
  store i64 %__Move_0___load_op1, ptr %r0, align 4
  %__Move_1___load_op1 = load i64, ptr %r50, align 4
  store i64 %__Move_1___load_op1, ptr %r1, align 4
  store i64 ptrtoint (ptr @"is-player?" to i64), ptr %r2, align 4
  %__Move_3___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_3___load_op1, ptr %r3, align 4
  %__Move_4___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_4___load_op1, ptr %r49, align 4
  %__CallFf_5___callee = load i64, ptr %r2, align 4
  %0 = inttoptr i64 %__CallFf_5___callee to ptr
  %__CallFf_5___arg0 = load i64, ptr %r49, align 4
  %__CallFf_5___call = call i64 %0(i64 %__CallFf_5___arg0), !dcvm.distance !1
  store i64 %__CallFf_5___call, ptr %r2, align 4
  %__BranchIfNot_6___cond = load i64, ptr %r2, align 4
  %__BranchIfNot_6___bool = icmp ne i64 %__BranchIfNot_6___cond, 0
  br i1 %__BranchIfNot_6___bool, label %bb1, label %bb15

bb1:                                              ; preds = %bb3, %bb0
  store i64 ptrtoint (ptr @"player-in-state?" to i64), ptr %r2, align 4
  %__LoadStaticU64Imm_8___st_load = load i64, ptr getelementptr inbounds ([18 x i64], ptr @wait-until-in-valid-gas-mask-state_symbol_table_src, i64 0, i64 2), align 4
  store i64 %__LoadStaticU64Imm_8___st_load, ptr %r3, align 4
  %__Move_9___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_9___load_op1, ptr %r49, align 4
  %__CallFf_10___callee = load i64, ptr %r2, align 4
  %1 = inttoptr i64 %__CallFf_10___callee to ptr
  %__CallFf_10___arg0 = load i64, ptr %r49, align 4
  %__CallFf_10___call = call i64 %1(i64 %__CallFf_10___arg0), !dcvm.distance !1
  store i64 %__CallFf_10___call, ptr %r2, align 4
  %__OpLogNot_11___load_op1 = load i64, ptr %r2, align 4
  %__OpLogNot_11__op = icmp eq i64 %__OpLogNot_11___load_op1, 0
  %2 = zext i1 %__OpLogNot_11__op to i64
  store i64 %2, ptr %r2, align 4
  %__BranchIfNot_12___cond = load i64, ptr %r2, align 4
  %__BranchIfNot_12___bool = icmp ne i64 %__BranchIfNot_12___cond, 0
  br i1 %__BranchIfNot_12___bool, label %bb2, label %bb3

bb2:                                              ; preds = %bb1
  store i64 ptrtoint (ptr @"player-in-state?" to i64), ptr %r2, align 4
  %__LoadStaticU64Imm_14___st_load = load i64, ptr getelementptr inbounds ([18 x i64], ptr @wait-until-in-valid-gas-mask-state_symbol_table_src, i64 0, i64 3), align 4
  store i64 %__LoadStaticU64Imm_14___st_load, ptr %r3, align 4
  %__Move_15___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_15___load_op1, ptr %r49, align 4
  %__CallFf_16___callee = load i64, ptr %r2, align 4
  %3 = inttoptr i64 %__CallFf_16___callee to ptr
  %__CallFf_16___arg0 = load i64, ptr %r49, align 4
  %__CallFf_16___call = call i64 %3(i64 %__CallFf_16___arg0), !dcvm.distance !1
  store i64 %__CallFf_16___call, ptr %r2, align 4
  %__OpLogNot_17___load_op1 = load i64, ptr %r2, align 4
  %__OpLogNot_17__op = icmp eq i64 %__OpLogNot_17___load_op1, 0
  %4 = zext i1 %__OpLogNot_17__op to i64
  store i64 %4, ptr %r2, align 4
  %__OpLogNot_18___load_op1 = load i64, ptr %r2, align 4
  %5 = trunc i64 %__OpLogNot_18___load_op1 to i1
  %__OpLogNot_18__op = icmp eq i1 %5, false
  %6 = zext i1 %__OpLogNot_18__op to i64
  store i64 %6, ptr %r2, align 4
  %__BranchIfNot_19___cond = load i64, ptr %r2, align 4
  %__BranchIfNot_19___bool = icmp ne i64 %__BranchIfNot_19___cond, 0
  br i1 %__BranchIfNot_19___bool, label %bb3, label %bb4

bb3:                                              ; preds = %bb2, %bb1
  store i64 ptrtoint (ptr @wait-one-frame to i64), ptr %r2, align 4
  %__CallFf_21___callee = load i64, ptr %r2, align 4
  %7 = inttoptr i64 %__CallFf_21___callee to ptr
  %__CallFf_21___call = call i64 %7(), !dcvm.distance !1
  store i64 %__CallFf_21___call, ptr %r2, align 4
  br label %bb1

bb4:                                              ; preds = %bb2
  %__Move_23___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_23___load_op1, ptr %r2, align 4
  %__BranchIfNot_24___cond = load i64, ptr %r2, align 4
  %__BranchIfNot_24___bool = icmp ne i64 %__BranchIfNot_24___cond, 0
  br i1 %__BranchIfNot_24___bool, label %bb5, label %bb9

bb5:                                              ; preds = %bb4
  store i64 ptrtoint (ptr @player-force-crouch to i64), ptr %r2, align 4
  %__CallFf_26___callee = load i64, ptr %r2, align 4
  %8 = inttoptr i64 %__CallFf_26___callee to ptr
  %__CallFf_26___call = call i64 %8(), !dcvm.distance !1
  store i64 %__CallFf_26___call, ptr %r2, align 4
  br label %bb6

bb6:                                              ; preds = %bb7, %bb5
  store i64 ptrtoint (ptr @"player-is-crouched?" to i64), ptr %r2, align 4
  %__CallFf_28___callee = load i64, ptr %r2, align 4
  %9 = inttoptr i64 %__CallFf_28___callee to ptr
  %__CallFf_28___call = call i64 %9(), !dcvm.distance !1
  store i64 %__CallFf_28___call, ptr %r2, align 4
  %__OpLogNot_29___load_op1 = load i64, ptr %r2, align 4
  %__OpLogNot_29__op = icmp eq i64 %__OpLogNot_29___load_op1, 0
  %10 = zext i1 %__OpLogNot_29__op to i64
  store i64 %10, ptr %r2, align 4
  %__BranchIfNot_30___cond = load i64, ptr %r2, align 4
  %__BranchIfNot_30___bool = icmp ne i64 %__BranchIfNot_30___cond, 0
  br i1 %__BranchIfNot_30___bool, label %bb7, label %bb8

bb7:                                              ; preds = %bb6
  store i64 ptrtoint (ptr @wait-one-frame to i64), ptr %r2, align 4
  %__CallFf_32___callee = load i64, ptr %r2, align 4
  %11 = inttoptr i64 %__CallFf_32___callee to ptr
  %__CallFf_32___call = call i64 %11(), !dcvm.distance !1
  store i64 %__CallFf_32___call, ptr %r2, align 4
  br label %bb6

bb8:                                              ; preds = %bb6
  br label %bb12

bb9:                                              ; preds = %bb4
  store i64 ptrtoint (ptr @player-force-stand to i64), ptr %r2, align 4
  %__CallFf_36___callee = load i64, ptr %r2, align 4
  %12 = inttoptr i64 %__CallFf_36___callee to ptr
  %__CallFf_36___call = call i64 %12(), !dcvm.distance !1
  store i64 %__CallFf_36___call, ptr %r2, align 4
  br label %bb10

bb10:                                             ; preds = %bb11, %bb9
  store i64 ptrtoint (ptr @"!" to i64), ptr %r2, align 4
  store i64 ptrtoint (ptr @"player-is-crouched?" to i64), ptr %r3, align 4
  %__CallFf_39___callee = load i64, ptr %r3, align 4
  %13 = inttoptr i64 %__CallFf_39___callee to ptr
  %__CallFf_39___call = call i64 %13(), !dcvm.distance !1
  store i64 %__CallFf_39___call, ptr %r3, align 4
  %__Move_40___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_40___load_op1, ptr %r49, align 4
  %__Call_41___callee = load i64, ptr %r2, align 4
  %14 = inttoptr i64 %__Call_41___callee to ptr
  %__Call_41___arg0 = load i64, ptr %r49, align 4
  %__Call_41___call = call i64 %14(i64 %__Call_41___arg0)
  store i64 %__Call_41___call, ptr %r2, align 4
  %__OpLogNot_42___load_op1 = load i64, ptr %r2, align 4
  %__OpLogNot_42__op = icmp eq i64 %__OpLogNot_42___load_op1, 0
  %15 = zext i1 %__OpLogNot_42__op to i64
  store i64 %15, ptr %r2, align 4
  %__BranchIfNot_43___cond = load i64, ptr %r2, align 4
  %__BranchIfNot_43___bool = icmp ne i64 %__BranchIfNot_43___cond, 0
  br i1 %__BranchIfNot_43___bool, label %bb11, label %bb12

bb11:                                             ; preds = %bb10
  store i64 ptrtoint (ptr @wait-one-frame to i64), ptr %r2, align 4
  %__CallFf_45___callee = load i64, ptr %r2, align 4
  %16 = inttoptr i64 %__CallFf_45___callee to ptr
  %__CallFf_45___call = call i64 %16(), !dcvm.distance !1
  store i64 %__CallFf_45___call, ptr %r2, align 4
  br label %bb10

bb12:                                             ; preds = %bb13, %bb10, %bb8
  store i64 ptrtoint (ptr @"player-in-state?" to i64), ptr %r2, align 4
  %__LoadStaticU64Imm_48___st_load = load i64, ptr getelementptr inbounds ([18 x i64], ptr @wait-until-in-valid-gas-mask-state_symbol_table_src, i64 0, i64 9), align 4
  store i64 %__LoadStaticU64Imm_48___st_load, ptr %r3, align 4
  %__Move_49___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_49___load_op1, ptr %r49, align 4
  %__CallFf_50___callee = load i64, ptr %r2, align 4
  %17 = inttoptr i64 %__CallFf_50___callee to ptr
  %__CallFf_50___arg0 = load i64, ptr %r49, align 4
  %__CallFf_50___call = call i64 %17(i64 %__CallFf_50___arg0), !dcvm.distance !1
  store i64 %__CallFf_50___call, ptr %r2, align 4
  %__OpLogNot_51___load_op1 = load i64, ptr %r2, align 4
  %__OpLogNot_51__op = icmp eq i64 %__OpLogNot_51___load_op1, 0
  %18 = zext i1 %__OpLogNot_51__op to i64
  store i64 %18, ptr %r2, align 4
  %__BranchIfNot_52___cond = load i64, ptr %r2, align 4
  %__BranchIfNot_52___bool = icmp ne i64 %__BranchIfNot_52___cond, 0
  br i1 %__BranchIfNot_52___bool, label %bb13, label %bb14

bb13:                                             ; preds = %bb12
  store i64 ptrtoint (ptr @wait-one-frame to i64), ptr %r2, align 4
  %__CallFf_54___callee = load i64, ptr %r2, align 4
  %19 = inttoptr i64 %__CallFf_54___callee to ptr
  %__CallFf_54___call = call i64 %19(), !dcvm.distance !1
  store i64 %__CallFf_54___call, ptr %r2, align 4
  br label %bb12

bb14:                                             ; preds = %bb12
  store i64 ptrtoint (ptr @wait-holster-player-weapon to i64), ptr %r2, align 4
  store i64 0, ptr %r3, align 4
  %__LoadStaticFloatImm_58___st_load = load float, ptr getelementptr inbounds ([18 x i64], ptr @wait-until-in-valid-gas-mask-state_symbol_table_src, i64 0, i64 11), align 4
  %20 = bitcast float %__LoadStaticFloatImm_58___st_load to i32
  %21 = zext i32 %20 to i64
  store i64 %21, ptr %r4, align 4
  %__Move_59___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_59___load_op1, ptr %r49, align 4
  %__Move_60___load_op1 = load i64, ptr %r4, align 4
  %22 = trunc i64 %__Move_60___load_op1 to i32
  %23 = bitcast i32 %22 to float
  %24 = bitcast float %23 to i32
  %25 = zext i32 %24 to i64
  store i64 %25, ptr %r50, align 4
  %__CallFf_61___callee = load i64, ptr %r2, align 4
  %26 = inttoptr i64 %__CallFf_61___callee to ptr
  %__CallFf_61___arg0 = load i64, ptr %r49, align 4
  %__CallFf_61___arg1 = load i64, ptr %r50, align 4
  %27 = trunc i64 %__CallFf_61___arg1 to i32
  %28 = bitcast i32 %27 to float
  %__CallFf_61___call = call i64 %26(i64 %__CallFf_61___arg0, float %28), !dcvm.distance !1
  store i64 %__CallFf_61___call, ptr %r2, align 4
  store i64 ptrtoint (ptr @holster-player-weapon-instantly to i64), ptr %r2, align 4
  %__LoadStaticFloatImm_63___st_load = load float, ptr getelementptr inbounds ([18 x i64], ptr @wait-until-in-valid-gas-mask-state_symbol_table_src, i64 0, i64 13), align 4
  %29 = bitcast float %__LoadStaticFloatImm_63___st_load to i32
  %30 = zext i32 %29 to i64
  store i64 %30, ptr %r3, align 4
  store i64 0, ptr %r4, align 4
  %__Move_65___load_op1 = load i64, ptr %r3, align 4
  %31 = trunc i64 %__Move_65___load_op1 to i32
  %32 = bitcast i32 %31 to float
  %33 = bitcast float %32 to i32
  %34 = zext i32 %33 to i64
  store i64 %34, ptr %r49, align 4
  %__Move_66___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_66___load_op1, ptr %r50, align 4
  %__CallFf_67___callee = load i64, ptr %r2, align 4
  %35 = inttoptr i64 %__CallFf_67___callee to ptr
  %__CallFf_67___arg0 = load i64, ptr %r49, align 4
  %36 = trunc i64 %__CallFf_67___arg0 to i32
  %37 = bitcast i32 %36 to float
  %__CallFf_67___arg1 = load i64, ptr %r50, align 4
  %__CallFf_67___call = call i64 %35(float %37, i64 %__CallFf_67___arg1), !dcvm.distance !1
  store i64 %__CallFf_67___call, ptr %r2, align 4
  br label %bb21

bb15:                                             ; preds = %bb0
  store i64 ptrtoint (ptr @wait-npc-holster-weapon to i64), ptr %r2, align 4
  %__Move_70___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_70___load_op1, ptr %r3, align 4
  store i64 0, ptr %r4, align 4
  store i64 1, ptr %r5, align 4
  %__LoadStaticFloatImm_73___st_load = load float, ptr getelementptr inbounds ([18 x i64], ptr @wait-until-in-valid-gas-mask-state_symbol_table_src, i64 0, i64 15), align 4
  %38 = bitcast float %__LoadStaticFloatImm_73___st_load to i32
  %39 = zext i32 %38 to i64
  store i64 %39, ptr %r6, align 4
  store i64 0, ptr %r7, align 4
  %__Move_75___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_75___load_op1, ptr %r49, align 4
  %__Move_76___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_76___load_op1, ptr %r50, align 4
  %__Move_77___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_77___load_op1, ptr %r51, align 4
  %__Move_78___load_op1 = load i64, ptr %r6, align 4
  %40 = trunc i64 %__Move_78___load_op1 to i32
  %41 = bitcast i32 %40 to float
  %42 = bitcast float %41 to i32
  %43 = zext i32 %42 to i64
  store i64 %43, ptr %r52, align 4
  %__Move_79___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_79___load_op1, ptr %r53, align 4
  %__CallFf_80___callee = load i64, ptr %r2, align 4
  %44 = inttoptr i64 %__CallFf_80___callee to ptr
  %__CallFf_80___arg0 = load i64, ptr %r49, align 4
  %__CallFf_80___arg1 = load i64, ptr %r50, align 4
  %__CallFf_80___arg2 = load i64, ptr %r51, align 4
  %__CallFf_80___arg3 = load i64, ptr %r52, align 4
  %45 = trunc i64 %__CallFf_80___arg3 to i32
  %46 = bitcast i32 %45 to float
  %__CallFf_80___arg4 = load i64, ptr %r53, align 4
  %__CallFf_80___call = call i64 %44(i64 %__CallFf_80___arg0, i64 %__CallFf_80___arg1, i64 %__CallFf_80___arg2, float %46, i64 %__CallFf_80___arg4), !dcvm.distance !1
  store i64 %__CallFf_80___call, ptr %r2, align 4
  br label %bb16

bb16:                                             ; preds = %bb17, %bb15
  store i64 ptrtoint (ptr @"npc-has-weapon-in-hand?" to i64), ptr %r2, align 4
  %__Move_82___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_82___load_op1, ptr %r3, align 4
  %__Move_83___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_83___load_op1, ptr %r49, align 4
  %__CallFf_84___callee = load i64, ptr %r2, align 4
  %47 = inttoptr i64 %__CallFf_84___callee to ptr
  %__CallFf_84___arg0 = load i64, ptr %r49, align 4
  %__CallFf_84___call = call i64 %47(i64 %__CallFf_84___arg0), !dcvm.distance !1
  store i64 %__CallFf_84___call, ptr %r2, align 4
  %__BranchIfNot_85___cond = load i64, ptr %r2, align 4
  %__BranchIfNot_85___bool = icmp ne i64 %__BranchIfNot_85___cond, 0
  br i1 %__BranchIfNot_85___bool, label %bb17, label %bb18

bb17:                                             ; preds = %bb16
  store i64 ptrtoint (ptr @wait-one-frame to i64), ptr %r2, align 4
  %__CallFf_87___callee = load i64, ptr %r2, align 4
  %48 = inttoptr i64 %__CallFf_87___callee to ptr
  %__CallFf_87___call = call i64 %48(), !dcvm.distance !1
  store i64 %__CallFf_87___call, ptr %r2, align 4
  br label %bb16

bb18:                                             ; preds = %bb16
  %__Move_89___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_89___load_op1, ptr %r2, align 4
  %__BranchIfNot_90___cond = load i64, ptr %r2, align 4
  %__BranchIfNot_90___bool = icmp ne i64 %__BranchIfNot_90___cond, 0
  br i1 %__BranchIfNot_90___bool, label %bb19, label %bb20

bb19:                                             ; preds = %bb18
  store i64 ptrtoint (ptr @wait-npc-set-demeanor to i64), ptr %r2, align 4
  %__Move_92___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_92___load_op1, ptr %r3, align 4
  store i64 3, ptr %r4, align 4
  %__Move_94___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_94___load_op1, ptr %r49, align 4
  %__Move_95___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_95___load_op1, ptr %r50, align 4
  %__CallFf_96___callee = load i64, ptr %r2, align 4
  %49 = inttoptr i64 %__CallFf_96___callee to ptr
  %__CallFf_96___arg0 = load i64, ptr %r49, align 4
  %__CallFf_96___arg1 = load i64, ptr %r50, align 4
  %__CallFf_96___call = call i64 %49(i64 %__CallFf_96___arg0, i64 %__CallFf_96___arg1), !dcvm.distance !1
  store i64 %__CallFf_96___call, ptr %r2, align 4
  br label %bb21

bb20:                                             ; preds = %bb18
  store i64 0, ptr %r2, align 4
  br label %bb21

bb21:                                             ; preds = %bb20, %bb19, %bb14
  %__Return_99___retval = load i64, ptr %r2, align 4
  ret i64 %__Return_99___retval
}

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !30 i64 @"player-in-state?"(i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !31 i64 @player-force-crouch() #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !32 i64 @"player-is-crouched?"() #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !33 i64 @player-force-stand() #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !34 i64 @"!"(i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !35 i64 @wait-holster-player-weapon(i64, float) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !36 i64 @holster-player-weapon-instantly(float, i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !37 i64 @wait-npc-holster-weapon(i64, i64, i64, float, i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !38 i64 @"npc-has-weapon-in-hand?"(i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !39 i64 @wait-npc-set-demeanor(i64, i64) #0

define i64 @"npc-gas-mask-narrative-mode/f"(i64 %arg_0) !dcvm.sid_distance !40 {
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
  store i64 %arg_0, ptr %r49, align 4
  %__Move_0___load_op1 = load i64, ptr %r49, align 4
  store i64 %__Move_0___load_op1, ptr %r0, align 4
  store i64 ptrtoint (ptr @send-event to i64), ptr %r1, align 4
  %__LoadStaticU64Imm_2___st_load = load i64, ptr getelementptr inbounds ([5 x i64], ptr @"npc-gas-mask-narrative-mode/f_symbol_table_src", i64 0, i64 1), align 4
  store i64 %__LoadStaticU64Imm_2___st_load, ptr %r2, align 4
  %__Move_3___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_3___load_op1, ptr %r3, align 4
  %__Move_4___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_4___load_op1, ptr %r49, align 4
  %__Move_5___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_5___load_op1, ptr %r50, align 4
  %__CallFf_6___callee = load i64, ptr %r1, align 4
  %0 = inttoptr i64 %__CallFf_6___callee to ptr
  %__CallFf_6___arg0 = load i64, ptr %r49, align 4
  %__CallFf_6___arg1 = load i64, ptr %r50, align 4
  %__CallFf_6___call = call i64 %0(i64 %__CallFf_6___arg0, i64 %__CallFf_6___arg1), !dcvm.distance !1
  store i64 %__CallFf_6___call, ptr %r1, align 4
  store i64 ptrtoint (ptr @"suspend-idle-gestures/f" to i64), ptr %r1, align 4
  %__Move_8___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_8___load_op1, ptr %r2, align 4
  %__Move_9___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_9___load_op1, ptr %r49, align 4
  %__Call_10___callee = load i64, ptr %r1, align 4
  %1 = inttoptr i64 %__Call_10___callee to ptr
  %__Call_10___arg0 = load i64, ptr %r49, align 4
  %__Call_10___call = call i64 %1(i64 %__Call_10___arg0)
  store i64 %__Call_10___call, ptr %r1, align 4
  store i64 ptrtoint (ptr @npc-disable-dialog-look to i64), ptr %r1, align 4
  %__Move_12___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_12___load_op1, ptr %r2, align 4
  %__LoadStaticFloatImm_13___st_load = load float, ptr getelementptr inbounds ([5 x i64], ptr @"npc-gas-mask-narrative-mode/f_symbol_table_src", i64 0, i64 4), align 4
  %2 = bitcast float %__LoadStaticFloatImm_13___st_load to i32
  %3 = zext i32 %2 to i64
  store i64 %3, ptr %r3, align 4
  %__Move_14___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_14___load_op1, ptr %r49, align 4
  %__Move_15___load_op1 = load i64, ptr %r3, align 4
  %4 = trunc i64 %__Move_15___load_op1 to i32
  %5 = bitcast i32 %4 to float
  %6 = bitcast float %5 to i32
  %7 = zext i32 %6 to i64
  store i64 %7, ptr %r50, align 4
  %__CallFf_16___callee = load i64, ptr %r1, align 4
  %8 = inttoptr i64 %__CallFf_16___callee to ptr
  %__CallFf_16___arg0 = load i64, ptr %r49, align 4
  %__CallFf_16___arg1 = load i64, ptr %r50, align 4
  %9 = trunc i64 %__CallFf_16___arg1 to i32
  %10 = bitcast i32 %9 to float
  %__CallFf_16___call = call i64 %8(i64 %__CallFf_16___arg0, float %10), !dcvm.distance !1
  store i64 %__CallFf_16___call, ptr %r1, align 4
  %__Return_17___retval = load i64, ptr %r1, align 4
  ret i64 %__Return_17___retval
}

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !41 i64 @"suspend-idle-gestures/f"(i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !42 i64 @npc-disable-dialog-look(i64, float) #0

; Function Attrs: nosync nounwind willreturn
define i64 @internal-put-off-gas-mask-immediately(i64 %arg_0, i64 %arg_1) #0 !dcvm.sid_distance !43 {
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
  store i64 %arg_0, ptr %r49, align 4
  store i64 %arg_1, ptr %r50, align 4
  %__Move_0___load_op1 = load i64, ptr %r49, align 4
  store i64 %__Move_0___load_op1, ptr %r0, align 4
  %__Move_1___load_op1 = load i64, ptr %r50, align 4
  store i64 %__Move_1___load_op1, ptr %r1, align 4
  store i64 ptrtoint (ptr @get-gas-mask to i64), ptr %r2, align 4
  %__Move_3___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_3___load_op1, ptr %r3, align 4
  %__Move_4___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_4___load_op1, ptr %r49, align 4
  %__CallFf_5___callee = load i64, ptr %r2, align 4
  %0 = inttoptr i64 %__CallFf_5___callee to ptr
  %__CallFf_5___arg0 = load i64, ptr %r49, align 4
  %__CallFf_5___call = call i64 %0(i64 %__CallFf_5___arg0), !dcvm.distance !1
  store i64 %__CallFf_5___call, ptr %r2, align 4
  store i64 ptrtoint (ptr @send-event to i64), ptr %r3, align 4
  %__LoadStaticU64Imm_7___st_load = load i64, ptr getelementptr inbounds ([4 x i64], ptr @internal-put-off-gas-mask-immediately_symbol_table_src, i64 0, i64 2), align 4
  store i64 %__LoadStaticU64Imm_7___st_load, ptr %r4, align 4
  %__Move_8___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_8___load_op1, ptr %r5, align 4
  %__Move_9___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_9___load_op1, ptr %r49, align 4
  %__Move_10___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_10___load_op1, ptr %r50, align 4
  %__CallFf_11___callee = load i64, ptr %r3, align 4
  %1 = inttoptr i64 %__CallFf_11___callee to ptr
  %__CallFf_11___arg0 = load i64, ptr %r49, align 4
  %__CallFf_11___arg1 = load i64, ptr %r50, align 4
  %__CallFf_11___call = call i64 %1(i64 %__CallFf_11___arg0, i64 %__CallFf_11___arg1), !dcvm.distance !1
  store i64 %__CallFf_11___call, ptr %r3, align 4
  %__Move_12___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_12___load_op1, ptr %r3, align 4
  %__BranchIfNot_13___cond = load i64, ptr %r3, align 4
  %__BranchIfNot_13___bool = icmp ne i64 %__BranchIfNot_13___cond, 0
  br i1 %__BranchIfNot_13___bool, label %bb1, label %bb2

bb1:                                              ; preds = %bb0
  store i64 ptrtoint (ptr @kill-entity to i64), ptr %r3, align 4
  %__Move_15___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_15___load_op1, ptr %r4, align 4
  %__Move_16___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_16___load_op1, ptr %r49, align 4
  %__CallFf_17___callee = load i64, ptr %r3, align 4
  %2 = inttoptr i64 %__CallFf_17___callee to ptr
  %__CallFf_17___arg0 = load i64, ptr %r49, align 4
  %__CallFf_17___call = call i64 %2(i64 %__CallFf_17___arg0), !dcvm.distance !1
  store i64 %__CallFf_17___call, ptr %r3, align 4
  br label %bb3

bb2:                                              ; preds = %bb0
  store i64 0, ptr %r3, align 4
  br label %bb3

bb3:                                              ; preds = %bb2, %bb1
  %__Move_20___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_20___load_op1, ptr %r2, align 4
  %__Return_21___retval = load i64, ptr %r2, align 4
  ret i64 %__Return_21___retval
}

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !44 i64 @kill-entity(i64) #0

define i64 @spawn-gas-mask(i64 %arg_0) !dcvm.sid_distance !45 {
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
  store i64 %arg_0, ptr %r49, align 4
  %__Move_0___load_op1 = load i64, ptr %r49, align 4
  store i64 %__Move_0___load_op1, ptr %r0, align 4
  store i64 ptrtoint (ptr @lookup-symbol to i64), ptr %r1, align 4
  store i64 ptrtoint (ptr @get-gas-mask-setting-id to i64), ptr %r2, align 4
  %__Move_3___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_3___load_op1, ptr %r3, align 4
  %__Move_4___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_4___load_op1, ptr %r49, align 4
  %__CallFf_5___callee = load i64, ptr %r2, align 4
  %0 = inttoptr i64 %__CallFf_5___callee to ptr
  %__CallFf_5___arg0 = load i64, ptr %r49, align 4
  %__CallFf_5___call = call i64 %0(i64 %__CallFf_5___arg0), !dcvm.distance !1
  store i64 %__CallFf_5___call, ptr %r2, align 4
  %__LoadStaticU64Imm_6___st_load = load i64, ptr getelementptr inbounds ([6 x i64], ptr @spawn-gas-mask_symbol_table_src, i64 0, i64 2), align 4
  store i64 %__LoadStaticU64Imm_6___st_load, ptr %r3, align 4
  %__Move_7___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_7___load_op1, ptr %r49, align 4
  %__Move_8___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_8___load_op1, ptr %r50, align 4
  %__CallFf_9___callee = load i64, ptr %r1, align 4
  %1 = inttoptr i64 %__CallFf_9___callee to ptr
  %__CallFf_9___arg0 = load i64, ptr %r49, align 4
  %__CallFf_9___arg1 = load i64, ptr %r50, align 4
  %__CallFf_9___call = call i64 %1(i64 %__CallFf_9___arg0, i64 %__CallFf_9___arg1), !dcvm.distance !1
  store i64 %__CallFf_9___call, ptr %r1, align 4
  store i64 ptrtoint (ptr @spawn-object to i64), ptr %r2, align 4
  %__Move_11___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_11___load_op1, ptr %r3, align 4
  %__AssertPointer_12___ptr = load i64, ptr %r3, align 4
  %2 = inttoptr i64 %__AssertPointer_12___ptr to ptr
  %__AssertPointer_12__op = icmp ne ptr %2, null
  %__IAddImm_13___load_lhs = load i64, ptr %r3, align 4
  %3 = inttoptr i64 %__IAddImm_13___load_lhs to ptr
  %__IAddImm_13__op = getelementptr i8, ptr %3, i8 40
  %4 = ptrtoint ptr %__IAddImm_13__op to i64
  store i64 %4, ptr %r3, align 4
  %__LoadU64_14___addr = load i64, ptr %r3, align 4
  %5 = inttoptr i64 %__LoadU64_14___addr to ptr
  %__LoadU64_14___load = load i64, ptr %5, align 4
  store i64 %__LoadU64_14___load, ptr %r3, align 4
  %__LoadStaticU64Imm_15___st_load = load i64, ptr getelementptr inbounds ([6 x i64], ptr @spawn-gas-mask_symbol_table_src, i64 0, i64 4), align 4
  store i64 %__LoadStaticU64Imm_15___st_load, ptr %r4, align 4
  store i64 0, ptr %r5, align 4
  store i64 0, ptr %r6, align 4
  store i64 1, ptr %r7, align 4
  store i64 0, ptr %r8, align 4
  store i64 0, ptr %r9, align 4
  store i64 0, ptr %r10, align 4
  store i64 0, ptr %r11, align 4
  %__Move_23___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_23___load_op1, ptr %r49, align 4
  %__Move_24___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_24___load_op1, ptr %r50, align 4
  %__Move_25___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_25___load_op1, ptr %r51, align 4
  %__Move_26___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_26___load_op1, ptr %r52, align 4
  %__Move_27___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_27___load_op1, ptr %r53, align 4
  %__Move_28___load_op1 = load i64, ptr %r8, align 4
  store i64 %__Move_28___load_op1, ptr %r54, align 4
  %__Move_29___load_op1 = load i64, ptr %r9, align 4
  store i64 %__Move_29___load_op1, ptr %r55, align 4
  %__Move_30___load_op1 = load i64, ptr %r10, align 4
  store i64 %__Move_30___load_op1, ptr %r56, align 4
  %__Move_31___load_op1 = load i64, ptr %r11, align 4
  store i64 %__Move_31___load_op1, ptr %r57, align 4
  %__CallFf_32___callee = load i64, ptr %r2, align 4
  %6 = inttoptr i64 %__CallFf_32___callee to ptr
  %__CallFf_32___arg0 = load i64, ptr %r49, align 4
  %__CallFf_32___arg1 = load i64, ptr %r50, align 4
  %__CallFf_32___arg2 = load i64, ptr %r51, align 4
  %__CallFf_32___arg3 = load i64, ptr %r52, align 4
  %__CallFf_32___arg4 = load i64, ptr %r53, align 4
  %__CallFf_32___arg5 = load i64, ptr %r54, align 4
  %__CallFf_32___arg6 = load i64, ptr %r55, align 4
  %__CallFf_32___arg7 = load i64, ptr %r56, align 4
  %__CallFf_32___arg8 = load i64, ptr %r57, align 4
  %__CallFf_32___call = call i64 %6(i64 %__CallFf_32___arg0, i64 %__CallFf_32___arg1, i64 %__CallFf_32___arg2, i64 %__CallFf_32___arg3, i64 %__CallFf_32___arg4, i64 %__CallFf_32___arg5, i64 %__CallFf_32___arg6, i64 %__CallFf_32___arg7, i64 %__CallFf_32___arg8), !dcvm.distance !1
  store i64 %__CallFf_32___call, ptr %r2, align 4
  store i64 ptrtoint (ptr @set-high-contrast-mode-type to i64), ptr %r3, align 4
  %__Move_34___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_34___load_op1, ptr %r4, align 4
  store i64 6, ptr %r5, align 4
  %__Move_36___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_36___load_op1, ptr %r49, align 4
  %__Move_37___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_37___load_op1, ptr %r50, align 4
  %__CallFf_38___callee = load i64, ptr %r3, align 4
  %7 = inttoptr i64 %__CallFf_38___callee to ptr
  %__CallFf_38___arg0 = load i64, ptr %r49, align 4
  %__CallFf_38___arg1 = load i64, ptr %r50, align 4
  %__CallFf_38___call = call i64 %7(i64 %__CallFf_38___arg0, i64 %__CallFf_38___arg1), !dcvm.distance !1
  store i64 %__CallFf_38___call, ptr %r3, align 4
  %__Move_39___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_39___load_op1, ptr %r3, align 4
  %__Move_40___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_40___load_op1, ptr %r3, align 4
  %__Move_41___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_41___load_op1, ptr %r2, align 4
  %__Move_42___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_42___load_op1, ptr %r1, align 4
  %__Return_43___retval = load i64, ptr %r1, align 4
  ret i64 %__Return_43___retval
}

define i64 @"player-gas-mask-narrative-mode/f"(i64 %arg_0, i64 %arg_1, i64 %arg_2) !dcvm.sid_distance !46 {
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
  store i64 %arg_0, ptr %r49, align 4
  store i64 %arg_1, ptr %r50, align 4
  store i64 %arg_2, ptr %r51, align 4
  %__Move_0___load_op1 = load i64, ptr %r49, align 4
  store i64 %__Move_0___load_op1, ptr %r0, align 4
  %__Move_1___load_op1 = load i64, ptr %r50, align 4
  store i64 %__Move_1___load_op1, ptr %r1, align 4
  %__Move_2___load_op1 = load i64, ptr %r51, align 4
  store i64 %__Move_2___load_op1, ptr %r2, align 4
  store i64 ptrtoint (ptr @player-disable-dialog-look-gestures to i64), ptr %r3, align 4
  %__LoadStaticFloatImm_4___st_load = load float, ptr getelementptr inbounds ([8 x i64], ptr @"player-gas-mask-narrative-mode/f_symbol_table_src", i64 0, i64 1), align 4
  %0 = bitcast float %__LoadStaticFloatImm_4___st_load to i32
  %1 = zext i32 %0 to i64
  store i64 %1, ptr %r4, align 4
  %__Move_5___load_op1 = load i64, ptr %r4, align 4
  %2 = trunc i64 %__Move_5___load_op1 to i32
  %3 = bitcast i32 %2 to float
  %4 = bitcast float %3 to i32
  %5 = zext i32 %4 to i64
  store i64 %5, ptr %r49, align 4
  %__CallFf_6___callee = load i64, ptr %r3, align 4
  %6 = inttoptr i64 %__CallFf_6___callee to ptr
  %__CallFf_6___arg0 = load i64, ptr %r49, align 4
  %7 = trunc i64 %__CallFf_6___arg0 to i32
  %8 = bitcast i32 %7 to float
  %__CallFf_6___call = call i64 %6(float %8), !dcvm.distance !1
  store i64 %__CallFf_6___call, ptr %r3, align 4
  store i64 ptrtoint (ptr @"player-disable-squeeze-through/f" to i64), ptr %r3, align 4
  %__CallFf_8___callee = load i64, ptr %r3, align 4
  %9 = inttoptr i64 %__CallFf_8___callee to ptr
  %__CallFf_8___call = call i64 %9(), !dcvm.distance !1
  store i64 %__CallFf_8___call, ptr %r3, align 4
  store i64 ptrtoint (ptr @"player-set-narrative-mode/f" to i64), ptr %r3, align 4
  %__Move_10___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_10___load_op1, ptr %r4, align 4
  %__Move_11___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_11___load_op1, ptr %r5, align 4
  %__Move_12___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_12___load_op1, ptr %r6, align 4
  %__Move_13___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_13___load_op1, ptr %r49, align 4
  %__Move_14___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_14___load_op1, ptr %r50, align 4
  %__Move_15___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_15___load_op1, ptr %r51, align 4
  %__Call_16___callee = load i64, ptr %r3, align 4
  %10 = inttoptr i64 %__Call_16___callee to ptr
  %__Call_16___arg0 = load i64, ptr %r49, align 4
  %__Call_16___arg1 = load i64, ptr %r50, align 4
  %__Call_16___arg2 = load i64, ptr %r51, align 4
  %__Call_16___call = call i64 %10(i64 %__Call_16___arg0, i64 %__Call_16___arg1, i64 %__Call_16___arg2)
  store i64 %__Call_16___call, ptr %r3, align 4
  store i64 ptrtoint (ptr @"suspend-idle-gestures/f" to i64), ptr %r3, align 4
  %__LoadStaticU64Imm_18___st_load = load i64, ptr getelementptr inbounds ([8 x i64], ptr @"player-gas-mask-narrative-mode/f_symbol_table_src", i64 0, i64 5), align 4
  store i64 %__LoadStaticU64Imm_18___st_load, ptr %r4, align 4
  %__Move_19___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_19___load_op1, ptr %r49, align 4
  %__Call_20___callee = load i64, ptr %r3, align 4
  %11 = inttoptr i64 %__Call_20___callee to ptr
  %__Call_20___arg0 = load i64, ptr %r49, align 4
  %__Call_20___call = call i64 %11(i64 %__Call_20___arg0)
  store i64 %__Call_20___call, ptr %r3, align 4
  store i64 ptrtoint (ptr @joypad-disable-commands to i64), ptr %r3, align 4
  %__LoadStaticU64Imm_22___st_load = load i64, ptr getelementptr inbounds ([8 x i64], ptr @"player-gas-mask-narrative-mode/f_symbol_table_src", i64 0, i64 7), align 4
  store i64 %__LoadStaticU64Imm_22___st_load, ptr %r4, align 4
  %__Move_23___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_23___load_op1, ptr %r49, align 4
  %__CallFf_24___callee = load i64, ptr %r3, align 4
  %12 = inttoptr i64 %__CallFf_24___callee to ptr
  %__CallFf_24___arg0 = load i64, ptr %r49, align 4
  %__CallFf_24___call = call i64 %12(i64 %__CallFf_24___arg0), !dcvm.distance !1
  store i64 %__CallFf_24___call, ptr %r3, align 4
  %__Return_25___retval = load i64, ptr %r3, align 4
  ret i64 %__Return_25___retval
}

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !47 i64 @player-disable-dialog-look-gestures(float) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !48 i64 @"player-disable-squeeze-through/f"() #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !49 i64 @"player-set-narrative-mode/f"(i64, i64, i64) #0

; Function Attrs: nosync nounwind willreturn
declare !dcvm.sid_distance !50 i64 @joypad-disable-commands(i64) #0

; Function Attrs: nosync nounwind willreturn
define i64 @internal-put-off-gas-mask(i64 %arg_0, i64 %arg_1) #0 !dcvm.sid_distance !51 {
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
  store i64 %arg_0, ptr %r49, align 4
  store i64 %arg_1, ptr %r50, align 4
  %__Move_0___load_op1 = load i64, ptr %r49, align 4
  store i64 %__Move_0___load_op1, ptr %r0, align 4
  %__Move_1___load_op1 = load i64, ptr %r50, align 4
  store i64 %__Move_1___load_op1, ptr %r1, align 4
  store i64 ptrtoint (ptr @wait-one-frame to i64), ptr %r2, align 4
  %__CallFf_3___callee = load i64, ptr %r2, align 4
  %0 = inttoptr i64 %__CallFf_3___callee to ptr
  %__CallFf_3___call = call i64 %0(), !dcvm.distance !1
  store i64 %__CallFf_3___call, ptr %r2, align 4
  store i64 ptrtoint (ptr @lookup-symbol to i64), ptr %r2, align 4
  store i64 ptrtoint (ptr @get-gas-mask-setting-id to i64), ptr %r3, align 4
  %__Move_6___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_6___load_op1, ptr %r4, align 4
  %__Move_7___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_7___load_op1, ptr %r49, align 4
  %__CallFf_8___callee = load i64, ptr %r3, align 4
  %1 = inttoptr i64 %__CallFf_8___callee to ptr
  %__CallFf_8___arg0 = load i64, ptr %r49, align 4
  %__CallFf_8___call = call i64 %1(i64 %__CallFf_8___arg0), !dcvm.distance !1
  store i64 %__CallFf_8___call, ptr %r3, align 4
  %__LoadStaticU64Imm_9___st_load = load i64, ptr getelementptr inbounds ([12 x i64], ptr @internal-put-off-gas-mask_symbol_table_src, i64 0, i64 3), align 4
  store i64 %__LoadStaticU64Imm_9___st_load, ptr %r4, align 4
  %__Move_10___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_10___load_op1, ptr %r49, align 4
  %__Move_11___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_11___load_op1, ptr %r50, align 4
  %__CallFf_12___callee = load i64, ptr %r2, align 4
  %2 = inttoptr i64 %__CallFf_12___callee to ptr
  %__CallFf_12___arg0 = load i64, ptr %r49, align 4
  %__CallFf_12___arg1 = load i64, ptr %r50, align 4
  %__CallFf_12___call = call i64 %2(i64 %__CallFf_12___arg0, i64 %__CallFf_12___arg1), !dcvm.distance !1
  store i64 %__CallFf_12___call, ptr %r2, align 4
  store i64 ptrtoint (ptr @get-gas-mask to i64), ptr %r3, align 4
  %__Move_14___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_14___load_op1, ptr %r4, align 4
  %__Move_15___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_15___load_op1, ptr %r49, align 4
  %__CallFf_16___callee = load i64, ptr %r3, align 4
  %3 = inttoptr i64 %__CallFf_16___callee to ptr
  %__CallFf_16___arg0 = load i64, ptr %r49, align 4
  %__CallFf_16___call = call i64 %3(i64 %__CallFf_16___arg0), !dcvm.distance !1
  store i64 %__CallFf_16___call, ptr %r3, align 4
  store i64 ptrtoint (ptr @wait-until-in-valid-gas-mask-state to i64), ptr %r4, align 4
  %__Move_18___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_18___load_op1, ptr %r5, align 4
  %__Move_19___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_19___load_op1, ptr %r6, align 4
  %__AssertPointer_20___ptr = load i64, ptr %r6, align 4
  %4 = inttoptr i64 %__AssertPointer_20___ptr to ptr
  %__AssertPointer_20__op = icmp ne ptr %4, null
  %__IAddImm_21___load_lhs = load i64, ptr %r6, align 4
  %5 = inttoptr i64 %__IAddImm_21___load_lhs to ptr
  %__IAddImm_21__op = getelementptr i8, ptr %5, i8 57
  %6 = ptrtoint ptr %__IAddImm_21__op to i64
  store i64 %6, ptr %r6, align 4
  %__LoadU8_22___addr = load i64, ptr %r6, align 4
  %7 = inttoptr i64 %__LoadU8_22___addr to ptr
  %__LoadU8_22___load = load i8, ptr %7, align 1
  %__LoadU8_22___ext = zext i8 %__LoadU8_22___load to i64
  store i64 %__LoadU8_22___ext, ptr %r6, align 4
  %__Move_23___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_23___load_op1, ptr %r49, align 4
  %__Move_24___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_24___load_op1, ptr %r50, align 4
  %__Call_25___callee = load i64, ptr %r4, align 4
  %8 = inttoptr i64 %__Call_25___callee to ptr
  %__Call_25___arg0 = load i64, ptr %r49, align 4
  %__Call_25___arg1 = load i64, ptr %r50, align 4
  %__Call_25___call = call i64 %8(i64 %__Call_25___arg0, i64 %__Call_25___arg1)
  store i64 %__Call_25___call, ptr %r4, align 4
  store i64 ptrtoint (ptr @animate_ to i64), ptr %r4, align 4
  %__Move_27___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_27___load_op1, ptr %r5, align 4
  %__Move_28___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_28___load_op1, ptr %r6, align 4
  %__AssertPointer_29___ptr = load i64, ptr %r6, align 4
  %9 = inttoptr i64 %__AssertPointer_29___ptr to ptr
  %__AssertPointer_29__op = icmp ne ptr %9, null
  %__IAddImm_30___load_lhs = load i64, ptr %r6, align 4
  %10 = inttoptr i64 %__IAddImm_30___load_lhs to ptr
  %__IAddImm_30__op = getelementptr i8, ptr %10, i8 32
  %11 = ptrtoint ptr %__IAddImm_30__op to i64
  store i64 %11, ptr %r6, align 4
  %__LoadU64_31___addr = load i64, ptr %r6, align 4
  %12 = inttoptr i64 %__LoadU64_31___addr to ptr
  %__LoadU64_31___load = load i64, ptr %12, align 4
  store i64 %__LoadU64_31___load, ptr %r6, align 4
  store i64 ptrtoint (ptr @"#%alloc-array" to i64), ptr %r7, align 4
  %__LoadStaticU64Imm_33___st_load = load i64, ptr getelementptr inbounds ([12 x i64], ptr @internal-put-off-gas-mask_symbol_table_src, i64 0, i64 8), align 4
  store i64 %__LoadStaticU64Imm_33___st_load, ptr %r8, align 4
  store i64 16, ptr %r9, align 4
  store i64 0, ptr %r10, align 4
  store i64 1, ptr %r11, align 4
  %__Move_37___load_op1 = load i64, ptr %r8, align 4
  store i64 %__Move_37___load_op1, ptr %r49, align 4
  %__Move_38___load_op1 = load i64, ptr %r9, align 4
  store i64 %__Move_38___load_op1, ptr %r50, align 4
  %__Move_39___load_op1 = load i64, ptr %r10, align 4
  store i64 %__Move_39___load_op1, ptr %r51, align 4
  %__Move_40___load_op1 = load i64, ptr %r11, align 4
  store i64 %__Move_40___load_op1, ptr %r52, align 4
  %__CallFf_41___callee = load i64, ptr %r7, align 4
  %13 = inttoptr i64 %__CallFf_41___callee to ptr
  %__CallFf_41___arg0 = load i64, ptr %r49, align 4
  %__CallFf_41___arg1 = load i64, ptr %r50, align 4
  %__CallFf_41___arg2 = load i64, ptr %r51, align 4
  %__CallFf_41___arg3 = load i64, ptr %r52, align 4
  %__CallFf_41___call = call i64 %13(i64 %__CallFf_41___arg0, i64 %__CallFf_41___arg1, i64 %__CallFf_41___arg2, i64 %__CallFf_41___arg3), !dcvm.distance !1
  store i64 %__CallFf_41___call, ptr %r7, align 4
  %__Move_42___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_42___load_op1, ptr %r8, align 4
  %__Move_43___load_op1 = load i64, ptr %r8, align 4
  store i64 %__Move_43___load_op1, ptr %r7, align 4
  %__Move_44___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_44___load_op1, ptr %r49, align 4
  %__Move_45___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_45___load_op1, ptr %r50, align 4
  %__Move_46___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_46___load_op1, ptr %r51, align 4
  %__CallFf_47___callee = load i64, ptr %r4, align 4
  %14 = inttoptr i64 %__CallFf_47___callee to ptr
  %__CallFf_47___arg0 = load i64, ptr %r49, align 4
  %__CallFf_47___arg1 = load i64, ptr %r50, align 4
  %__CallFf_47___arg2 = load i64, ptr %r51, align 4
  %__CallFf_47___call = call i64 %14(i64 %__CallFf_47___arg0, i64 %__CallFf_47___arg1, i64 %__CallFf_47___arg2), !dcvm.distance !1
  store i64 %__CallFf_47___call, ptr %r4, align 4
  br label %bb1

bb1:                                              ; preds = %bb2, %bb0
  store i64 ptrtoint (ptr @gesture_ to i64), ptr %r4, align 4
  %__Move_49___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_49___load_op1, ptr %r5, align 4
  %__Move_50___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_50___load_op1, ptr %r6, align 4
  %__AssertPointer_51___ptr = load i64, ptr %r6, align 4
  %15 = inttoptr i64 %__AssertPointer_51___ptr to ptr
  %__AssertPointer_51__op = icmp ne ptr %15, null
  %__IAddImm_52___load_lhs = load i64, ptr %r6, align 4
  %16 = inttoptr i64 %__IAddImm_52___load_lhs to ptr
  %__IAddImm_52__op = getelementptr i8, ptr %16, i8 8
  %17 = ptrtoint ptr %__IAddImm_52__op to i64
  store i64 %17, ptr %r6, align 4
  %__LoadU64_53___addr = load i64, ptr %r6, align 4
  %18 = inttoptr i64 %__LoadU64_53___addr to ptr
  %__LoadU64_53___load = load i64, ptr %18, align 4
  store i64 %__LoadU64_53___load, ptr %r6, align 4
  store i64 ptrtoint (ptr @alloc-gesture-play-params to i64), ptr %r7, align 4
  %__CallFf_55___callee = load i64, ptr %r7, align 4
  %19 = inttoptr i64 %__CallFf_55___callee to ptr
  %__CallFf_55___call = call i64 %19(), !dcvm.distance !1
  store i64 %__CallFf_55___call, ptr %r7, align 4
  %__Move_56___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_56___load_op1, ptr %r8, align 4
  %__IAddImm_57___load_lhs = load i64, ptr %r8, align 4
  %__IAddImm_57__op = add i64 %__IAddImm_57___load_lhs, 33
  store i64 %__IAddImm_57__op, ptr %r8, align 4
  store i64 1, ptr %r9, align 4
  %__StoreU8_59___value = load i64, ptr %r9, align 4
  %__StoreU8_59___addr = load i64, ptr %r8, align 4
  %20 = inttoptr i64 %__StoreU8_59___addr to ptr
  %__StoreU8_59___trunc = trunc i64 %__StoreU8_59___value to i8
  store i8 %__StoreU8_59___trunc, ptr %20, align 1
  store i64 %__StoreU8_59___value, ptr %r8, align 4
  %__Move_60___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_60___load_op1, ptr %r8, align 4
  %__IAddImm_61___load_lhs = load i64, ptr %r8, align 4
  %__IAddImm_61__op = add i64 %__IAddImm_61___load_lhs, 36
  store i64 %__IAddImm_61__op, ptr %r8, align 4
  store i64 19, ptr %r9, align 4
  %__StoreI32_63___value = load i64, ptr %r9, align 4
  %__StoreI32_63___addr = load i64, ptr %r8, align 4
  %21 = inttoptr i64 %__StoreI32_63___addr to ptr
  %__StoreI32_63___trunc = trunc i64 %__StoreI32_63___value to i32
  store i32 %__StoreI32_63___trunc, ptr %21, align 4
  store i64 %__StoreI32_63___value, ptr %r8, align 4
  %__Move_64___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_64___load_op1, ptr %r8, align 4
  %__IAddImm_65___load_lhs = load i64, ptr %r8, align 4
  %__IAddImm_65__op = add i64 %__IAddImm_65___load_lhs, 32
  store i64 %__IAddImm_65__op, ptr %r8, align 4
  store i64 1, ptr %r9, align 4
  %__StoreU8_67___value = load i64, ptr %r9, align 4
  %__StoreU8_67___addr = load i64, ptr %r8, align 4
  %22 = inttoptr i64 %__StoreU8_67___addr to ptr
  %__StoreU8_67___trunc = trunc i64 %__StoreU8_67___value to i8
  store i8 %__StoreU8_67___trunc, ptr %22, align 1
  store i64 %__StoreU8_67___value, ptr %r8, align 4
  %__Move_68___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_68___load_op1, ptr %r8, align 4
  %__IAddImm_69___load_lhs = load i64, ptr %r8, align 4
  %__IAddImm_69__op = add i64 %__IAddImm_69___load_lhs, 34
  store i64 %__IAddImm_69__op, ptr %r8, align 4
  store i64 1, ptr %r9, align 4
  %__StoreU8_71___value = load i64, ptr %r9, align 4
  %__StoreU8_71___addr = load i64, ptr %r8, align 4
  %23 = inttoptr i64 %__StoreU8_71___addr to ptr
  %__StoreU8_71___trunc = trunc i64 %__StoreU8_71___value to i8
  store i8 %__StoreU8_71___trunc, ptr %23, align 1
  store i64 %__StoreU8_71___value, ptr %r8, align 4
  %__Move_72___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_72___load_op1, ptr %r8, align 4
  %__Move_73___load_op1 = load i64, ptr %r8, align 4
  store i64 %__Move_73___load_op1, ptr %r7, align 4
  %__Move_74___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_74___load_op1, ptr %r49, align 4
  %__Move_75___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_75___load_op1, ptr %r50, align 4
  %__Move_76___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_76___load_op1, ptr %r51, align 4
  %__CallFf_77___callee = load i64, ptr %r4, align 4
  %24 = inttoptr i64 %__CallFf_77___callee to ptr
  %__CallFf_77___arg0 = load i64, ptr %r49, align 4
  %__CallFf_77___arg1 = load i64, ptr %r50, align 4
  %__CallFf_77___arg2 = load i64, ptr %r51, align 4
  %__CallFf_77___call = call i64 %24(i64 %__CallFf_77___arg0, i64 %__CallFf_77___arg1, i64 %__CallFf_77___arg2), !dcvm.distance !1
  store i64 %__CallFf_77___call, ptr %r4, align 4
  store i64 0, ptr %r5, align 4
  %__IEqual_79___load_lhs = load i64, ptr %r4, align 4
  %__IEqual_79___load_rhs = load i64, ptr %r5, align 4
  %__IEqual_79__op = icmp eq i64 %__IEqual_79___load_lhs, %__IEqual_79___load_rhs
  %25 = zext i1 %__IEqual_79__op to i64
  store i64 %25, ptr %r4, align 4
  %__OpLogNot_80___load_op1 = load i64, ptr %r4, align 4
  %26 = trunc i64 %__OpLogNot_80___load_op1 to i1
  %__OpLogNot_80__op = icmp eq i1 %26, false
  %27 = zext i1 %__OpLogNot_80__op to i64
  store i64 %27, ptr %r4, align 4
  %__BranchIfNot_81___cond = load i64, ptr %r4, align 4
  %__BranchIfNot_81___bool = icmp ne i64 %__BranchIfNot_81___cond, 0
  br i1 %__BranchIfNot_81___bool, label %bb2, label %bb3

bb2:                                              ; preds = %bb1
  store i64 ptrtoint (ptr @wait-one-frame to i64), ptr %r4, align 4
  %__CallFf_83___callee = load i64, ptr %r4, align 4
  %28 = inttoptr i64 %__CallFf_83___callee to ptr
  %__CallFf_83___call = call i64 %28(), !dcvm.distance !1
  store i64 %__CallFf_83___call, ptr %r4, align 4
  br label %bb1

bb3:                                              ; preds = %bb1
  %__Move_85___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_85___load_op1, ptr %r4, align 4
  %__BranchIfNot_86___cond = load i64, ptr %r4, align 4
  %__BranchIfNot_86___bool = icmp ne i64 %__BranchIfNot_86___cond, 0
  br i1 %__BranchIfNot_86___bool, label %bb4, label %bb5

bb4:                                              ; preds = %bb3
  store i64 ptrtoint (ptr @kill-entity to i64), ptr %r4, align 4
  %__Move_88___load_op1 = load i64, ptr %r3, align 4
  store i64 %__Move_88___load_op1, ptr %r5, align 4
  %__Move_89___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_89___load_op1, ptr %r49, align 4
  %__CallFf_90___callee = load i64, ptr %r4, align 4
  %29 = inttoptr i64 %__CallFf_90___callee to ptr
  %__CallFf_90___arg0 = load i64, ptr %r49, align 4
  %__CallFf_90___call = call i64 %29(i64 %__CallFf_90___arg0), !dcvm.distance !1
  store i64 %__CallFf_90___call, ptr %r4, align 4
  br label %bb6

bb5:                                              ; preds = %bb3
  store i64 0, ptr %r4, align 4
  br label %bb6

bb6:                                              ; preds = %bb5, %bb4
  %__Move_93___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_93___load_op1, ptr %r2, align 4
  %__Return_94___retval = load i64, ptr %r2, align 4
  ret i64 %__Return_94___retval
}

define i64 @wait-remove-gas-mask(i64 %arg_0, i64 %arg_1, i64 %arg_2) !dcvm.sid_distance !52 {
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
  store i64 %arg_0, ptr %r49, align 4
  store i64 %arg_1, ptr %r50, align 4
  store i64 %arg_2, ptr %r51, align 4
  %__Move_0___load_op1 = load i64, ptr %r49, align 4
  store i64 %__Move_0___load_op1, ptr %r0, align 4
  %__Move_1___load_op1 = load i64, ptr %r50, align 4
  store i64 %__Move_1___load_op1, ptr %r1, align 4
  %__Move_2___load_op1 = load i64, ptr %r51, align 4
  store i64 %__Move_2___load_op1, ptr %r2, align 4
  store i64 ptrtoint (ptr @fact-set to i64), ptr %r3, align 4
  %__Move_4___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_4___load_op1, ptr %r4, align 4
  %__LoadStaticU64Imm_5___st_load = load i64, ptr getelementptr inbounds ([10 x i64], ptr @wait-remove-gas-mask_symbol_table_src, i64 0, i64 1), align 4
  store i64 %__LoadStaticU64Imm_5___st_load, ptr %r5, align 4
  store i64 ptrtoint (ptr @new-boxed-value to i64), ptr %r6, align 4
  store i64 1, ptr %r7, align 4
  store i64 0, ptr %r8, align 4
  %__Move_9___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_9___load_op1, ptr %r49, align 4
  %__Move_10___load_op1 = load i64, ptr %r8, align 4
  store i64 %__Move_10___load_op1, ptr %r50, align 4
  %__CallFf_11___callee = load i64, ptr %r6, align 4
  %0 = inttoptr i64 %__CallFf_11___callee to ptr
  %__CallFf_11___arg0 = load i64, ptr %r49, align 4
  %__CallFf_11___arg1 = load i64, ptr %r50, align 4
  %__CallFf_11___call = call i64 %0(i64 %__CallFf_11___arg0, i64 %__CallFf_11___arg1), !dcvm.distance !1
  store i64 %__CallFf_11___call, ptr %r6, align 4
  %__Move_12___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_12___load_op1, ptr %r49, align 4
  %__Move_13___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_13___load_op1, ptr %r50, align 4
  %__Move_14___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_14___load_op1, ptr %r51, align 4
  %__CallFf_15___callee = load i64, ptr %r3, align 4
  %1 = inttoptr i64 %__CallFf_15___callee to ptr
  %__CallFf_15___arg0 = load i64, ptr %r49, align 4
  %__CallFf_15___arg1 = load i64, ptr %r50, align 4
  %__CallFf_15___arg2 = load i64, ptr %r51, align 4
  %__CallFf_15___call = call i64 %1(i64 %__CallFf_15___arg0, i64 %__CallFf_15___arg1, i64 %__CallFf_15___arg2), !dcvm.distance !1
  store i64 %__CallFf_15___call, ptr %r3, align 4
  br label %bb1

bb1:                                              ; preds = %bb2, %bb0
  store i64 ptrtoint (ptr @"is-script-running?" to i64), ptr %r3, align 4
  %__LoadStaticU64Imm_17___st_load = load i64, ptr getelementptr inbounds ([10 x i64], ptr @wait-remove-gas-mask_symbol_table_src, i64 0, i64 4), align 4
  store i64 %__LoadStaticU64Imm_17___st_load, ptr %r4, align 4
  %__Move_18___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_18___load_op1, ptr %r49, align 4
  %__Call_19___callee = load i64, ptr %r3, align 4
  %2 = inttoptr i64 %__Call_19___callee to ptr
  %__Call_19___arg0 = load i64, ptr %r49, align 4
  %__Call_19___call = call i64 %2(i64 %__Call_19___arg0)
  store i64 %__Call_19___call, ptr %r3, align 4
  %__OpLogNot_20___load_op1 = load i64, ptr %r3, align 4
  %__OpLogNot_20__op = icmp eq i64 %__OpLogNot_20___load_op1, 0
  %3 = zext i1 %__OpLogNot_20__op to i64
  store i64 %3, ptr %r3, align 4
  %__BranchIfNot_21___cond = load i64, ptr %r3, align 4
  %__BranchIfNot_21___bool = icmp ne i64 %__BranchIfNot_21___cond, 0
  br i1 %__BranchIfNot_21___bool, label %bb2, label %bb3

bb2:                                              ; preds = %bb1
  store i64 ptrtoint (ptr @wait-one-frame to i64), ptr %r3, align 4
  %__CallFf_23___callee = load i64, ptr %r3, align 4
  %4 = inttoptr i64 %__CallFf_23___callee to ptr
  %__CallFf_23___call = call i64 %4(), !dcvm.distance !1
  store i64 %__CallFf_23___call, ptr %r3, align 4
  br label %bb1

bb3:                                              ; preds = %bb1
  store i64 ptrtoint (ptr @send-event to i64), ptr %r3, align 4
  %__LoadStaticU64Imm_26___st_load = load i64, ptr getelementptr inbounds ([10 x i64], ptr @wait-remove-gas-mask_symbol_table_src, i64 0, i64 7), align 4
  store i64 %__LoadStaticU64Imm_26___st_load, ptr %r4, align 4
  %__LoadStaticU64Imm_27___st_load = load i64, ptr getelementptr inbounds ([10 x i64], ptr @wait-remove-gas-mask_symbol_table_src, i64 0, i64 4), align 4
  store i64 %__LoadStaticU64Imm_27___st_load, ptr %r5, align 4
  store i64 ptrtoint (ptr @new-boxed-value to i64), ptr %r6, align 4
  store i64 7, ptr %r7, align 4
  %__Move_30___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_30___load_op1, ptr %r8, align 4
  %__Move_31___load_op1 = load i64, ptr %r7, align 4
  store i64 %__Move_31___load_op1, ptr %r49, align 4
  %__Move_32___load_op1 = load i64, ptr %r8, align 4
  store i64 %__Move_32___load_op1, ptr %r50, align 4
  %__CallFf_33___callee = load i64, ptr %r6, align 4
  %5 = inttoptr i64 %__CallFf_33___callee to ptr
  %__CallFf_33___arg0 = load i64, ptr %r49, align 4
  %__CallFf_33___arg1 = load i64, ptr %r50, align 4
  %__CallFf_33___call = call i64 %5(i64 %__CallFf_33___arg0, i64 %__CallFf_33___arg1), !dcvm.distance !1
  store i64 %__CallFf_33___call, ptr %r6, align 4
  %__Move_34___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_34___load_op1, ptr %r49, align 4
  %__Move_35___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_35___load_op1, ptr %r50, align 4
  %__Move_36___load_op1 = load i64, ptr %r6, align 4
  store i64 %__Move_36___load_op1, ptr %r51, align 4
  %__CallFf_37___callee = load i64, ptr %r3, align 4
  %6 = inttoptr i64 %__CallFf_37___callee to ptr
  %__CallFf_37___arg0 = load i64, ptr %r49, align 4
  %__CallFf_37___arg1 = load i64, ptr %r50, align 4
  %__CallFf_37___arg2 = load i64, ptr %r51, align 4
  %__CallFf_37___call = call i64 %6(i64 %__CallFf_37___arg0, i64 %__CallFf_37___arg1, i64 %__CallFf_37___arg2), !dcvm.distance !1
  store i64 %__CallFf_37___call, ptr %r3, align 4
  %__Move_38___load_op1 = load i64, ptr %r1, align 4
  store i64 %__Move_38___load_op1, ptr %r3, align 4
  %__BranchIfNot_39___cond = load i64, ptr %r3, align 4
  %__BranchIfNot_39___bool = icmp ne i64 %__BranchIfNot_39___cond, 0
  br i1 %__BranchIfNot_39___bool, label %bb4, label %bb5

bb4:                                              ; preds = %bb3
  store i64 ptrtoint (ptr @internal-put-off-gas-mask-immediately to i64), ptr %r3, align 4
  %__Move_41___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_41___load_op1, ptr %r4, align 4
  %__Move_42___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_42___load_op1, ptr %r5, align 4
  %__Move_43___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_43___load_op1, ptr %r49, align 4
  %__Move_44___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_44___load_op1, ptr %r50, align 4
  %__Call_45___callee = load i64, ptr %r3, align 4
  %7 = inttoptr i64 %__Call_45___callee to ptr
  %__Call_45___arg0 = load i64, ptr %r49, align 4
  %__Call_45___arg1 = load i64, ptr %r50, align 4
  %__Call_45___call = call i64 %7(i64 %__Call_45___arg0, i64 %__Call_45___arg1)
  store i64 %__Call_45___call, ptr %r3, align 4
  br label %bb6

bb5:                                              ; preds = %bb3
  store i64 ptrtoint (ptr @internal-put-off-gas-mask to i64), ptr %r3, align 4
  %__Move_48___load_op1 = load i64, ptr %r0, align 4
  store i64 %__Move_48___load_op1, ptr %r4, align 4
  %__Move_49___load_op1 = load i64, ptr %r2, align 4
  store i64 %__Move_49___load_op1, ptr %r5, align 4
  %__Move_50___load_op1 = load i64, ptr %r4, align 4
  store i64 %__Move_50___load_op1, ptr %r49, align 4
  %__Move_51___load_op1 = load i64, ptr %r5, align 4
  store i64 %__Move_51___load_op1, ptr %r50, align 4
  %__Call_52___callee = load i64, ptr %r3, align 4
  %8 = inttoptr i64 %__Call_52___callee to ptr
  %__Call_52___arg0 = load i64, ptr %r49, align 4
  %__Call_52___arg1 = load i64, ptr %r50, align 4
  %__Call_52___call = call i64 %8(i64 %__Call_52___arg0, i64 %__Call_52___arg1)
  store i64 %__Call_52___call, ptr %r3, align 4
  br label %bb6

bb6:                                              ; preds = %bb5, %bb4
  %__Return_53___retval = load i64, ptr %r3, align 4
  ret i64 %__Return_53___retval
}

attributes #0 = { nosync nounwind willreturn }

!0 = !{!"sid", i64 4147362863116273348}
!1 = !{!"far"}
!2 = !{!"sid", i64 -2210386714833194546, !"distance", !"far"}
!3 = !{!"sid", i64 3684810824690217768, !"distance", !"far"}
!4 = !{!"sid", i64 6481843155139167450, !"distance", !"near"}
!5 = !{!"sid", i64 -3047912596559836939, !"distance", !"far"}
!6 = !{!"sid", i64 4789555171397753330, !"distance", !"far"}
!7 = !{!"sid", i64 6078802017353408438, !"distance", !"near"}
!8 = !{!"sid", i64 5608585023750540989, !"distance", !"near"}
!9 = !{!"sid", i64 -5076200561558624898, !"distance", !"far"}
!10 = !{!"sid", i64 5608585023750540989}
!11 = !{!"sid", i64 -7658033447216601854, !"distance", !"far"}
!12 = !{!"sid", i64 -6573555005595409285, !"distance", !"far"}
!13 = !{!"sid", i64 8203816822361639083, !"distance", !"near"}
!14 = !{!"sid", i64 7097052834867614476, !"distance", !"far"}
!15 = !{!"sid", i64 4766017279285775524, !"distance", !"far"}
!16 = !{!"sid", i64 6532480022340268902, !"distance", !"far"}
!17 = !{!"sid", i64 -429226074377614887, !"distance", !"far"}
!18 = !{!"sid", i64 -7252477440030364545, !"distance", !"far"}
!19 = !{!"sid", i64 -3854484060414110792, !"distance", !"far"}
!20 = !{!"sid", i64 -5971891483814290574, !"distance", !"far"}
!21 = !{!"sid", i64 -7746349783173232884, !"distance", !"far"}
!22 = !{!"sid", i64 -5676660224586178492, !"distance", !"far"}
!23 = !{!"sid", i64 -9179819928081802467, !"distance", !"far"}
!24 = !{!"sid", i64 -7745656483000545638, !"distance", !"far"}
!25 = !{!"sid", i64 6500315221283771953, !"distance", !"far"}
!26 = !{!"sid", i64 -2962780980939953026, !"distance", !"far"}
!27 = !{!"sid", i64 6078802017353408438}
!28 = !{!"sid", i64 6464408158396193538}
!29 = !{!"sid", i64 8203816822361639083}
!30 = !{!"sid", i64 -9003251364961024783, !"distance", !"far"}
!31 = !{!"sid", i64 -4831287436211302109, !"distance", !"far"}
!32 = !{!"sid", i64 1646463116072042322, !"distance", !"far"}
!33 = !{!"sid", i64 8594926419455150747, !"distance", !"far"}
!34 = !{!"sid", i64 -5808627241898115124, !"distance", !"near"}
!35 = !{!"sid", i64 -3092934816467423265, !"distance", !"far"}
!36 = !{!"sid", i64 -626103520744544694, !"distance", !"far"}
!37 = !{!"sid", i64 5808793362033684947, !"distance", !"far"}
!38 = !{!"sid", i64 7683024912818272899, !"distance", !"far"}
!39 = !{!"sid", i64 8573502661920361253, !"distance", !"far"}
!40 = !{!"sid", i64 9135517996633164385}
!41 = !{!"sid", i64 -5356689657310757786, !"distance", !"near"}
!42 = !{!"sid", i64 7610916627332680768, !"distance", !"far"}
!43 = !{!"sid", i64 -8559530813269772124, !"distance", !"near"}
!44 = !{!"sid", i64 -804116741684351865, !"distance", !"far"}
!45 = !{!"sid", i64 -8188029138696281045}
!46 = !{!"sid", i64 -7406256944386539599}
!47 = !{!"sid", i64 7238593933813171071, !"distance", !"far"}
!48 = !{!"sid", i64 8165897991486741957, !"distance", !"far"}
!49 = !{!"sid", i64 -2826777519175913837, !"distance", !"near"}
!50 = !{!"sid", i64 7121253686161376880, !"distance", !"far"}
!51 = !{!"sid", i64 -6610620527767074061, !"distance", !"near"}
!52 = !{!"sid", i64 -1351166648529978758}

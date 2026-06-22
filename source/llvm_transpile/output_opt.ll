; ModuleID = 'anim-gas-mask-impl.bin'
source_filename = "anim-gas-mask-impl.bin"
target triple = "x86_64-pc-windows-msvc"

@symbol_table = private unnamed_addr constant [11 x i64] [i64 -2210386714833194546, i64 7671589888336862994, i64 3684810824690217768, i64 6481843155139167450, i64 6150028017864311825, i64 -3047912596559836939, i64 4789555171397753330, i64 -3266162430027026367, i64 6078802017353408438, i64 5608585023750540989, i64 -5076200561558624898], align 8
@symbol_table.2 = private unnamed_addr constant [26 x i64] [i64 -3047912596559836939, i64 -7658033447216601854, i64 -6573555005595409285, i64 3455792245619945009, i64 8203816822361639083, i64 7097052834867614476, i64 267125575217391726, i64 4766017279285775524, i64 6532480022340268902, i64 -429226074377614887, i64 -7252477440030364545, i64 -3854484060414110792, i64 -5971891483814290574, i64 -7746349783173232884, i64 -5676660224586178492, i64 8667864169316382944, i64 0, i64 4789555171397753330, i64 6817768622307059015, i64 3684810824690217768, i64 1050253722, i64 -9179819928081802467, i64 -7745656483000545638, i64 8534565298598421705, i64 6500315221283771953, i64 -2962780980939953026], align 8
@symbol_table.4 = private unnamed_addr constant [19 x i64] [i64 -7658033447216601854, i64 -6573555005595409285, i64 3455792245619945009, i64 7097052834867614476, i64 267125575217391726, i64 4766017279285775524, i64 -5971891483814290574, i64 6532480022340268902, i64 4789555171397753330, i64 6817768622307059015, i64 3684810824690217768, i64 0, i64 -429226074377614887, i64 1827492438953869695, i64 -7252477440030364545, i64 -3854484060414110792, i64 -9179819928081802467, i64 -7745656483000545638, i64 8534565298598421705], align 8
@symbol_table.5 = private unnamed_addr constant [1 x i64] [i64 -5076200561558624898], align 8
@symbol_table.7 = private unnamed_addr constant [18 x i64] [i64 6532480022340268902, i64 -9003251364961024783, i64 6740565536053766520, i64 2237451099064635199, i64 -3047912596559836939, i64 -4831287436211302109, i64 1646463116072042322, i64 8594926419455150747, i64 -5808627241898115124, i64 4007104907368024212, i64 -3092934816467423265, i64 1065353216, i64 -626103520744544694, i64 1045220557, i64 5808793362033684947, i64 3212836864, i64 7683024912818272899, i64 8573502661920361253], align 8
@symbol_table.8 = private unnamed_addr constant [5 x i64] [i64 4789555171397753330, i64 8119835013146602502, i64 -5356689657310757786, i64 7610916627332680768, i64 0], align 8
@symbol_table.9 = private unnamed_addr constant [4 x i64] [i64 -5076200561558624898, i64 4789555171397753330, i64 -3264453205741318791, i64 -804116741684351865], align 8
@symbol_table.10 = private unnamed_addr constant [6 x i64] [i64 -7658033447216601854, i64 -6573555005595409285, i64 3455792245619945009, i64 7097052834867614476, i64 267125575217391726, i64 4766017279285775524], align 8
@symbol_table.11 = private unnamed_addr constant [8 x i64] [i64 7238593933813171071, i64 0, i64 8165897991486741957, i64 -2826777519175913837, i64 -5356689657310757786, i64 5008278420455340480, i64 7121253686161376880, i64 -1699981380503476753], align 8
@symbol_table.12 = private unnamed_addr constant [12 x i64] [i64 -3047912596559836939, i64 -7658033447216601854, i64 -6573555005595409285, i64 3455792245619945009, i64 -5076200561558624898, i64 8203816822361639083, i64 -9179819928081802467, i64 -7745656483000545638, i64 8534565298598421705, i64 6500315221283771953, i64 -2962780980939953026, i64 -804116741684351865], align 8
@symbol_table.13 = private unnamed_addr constant [10 x i64] [i64 -2210386714833194546, i64 7671589888336862994, i64 3684810824690217768, i64 6481843155139167450, i64 6150028017864311825, i64 -3047912596559836939, i64 4789555171397753330, i64 -3325057813225958160, i64 -8559530813269772124, i64 -6610620527767074061], align 8
@llvm.used = appending global [11 x ptr] [ptr @symbol_table, ptr @symbol_table.10, ptr @symbol_table.11, ptr @symbol_table.12, ptr @symbol_table.13, ptr @symbol_table.2, ptr @symbol_table.4, ptr @symbol_table.5, ptr @symbol_table.7, ptr @symbol_table.8, ptr @symbol_table.9], section "llvm.metadata"

define i64 @wait-equip-gas-mask(i64 %arg_0, i64 %arg_1, i64 %arg_2) local_unnamed_addr {
bb0:
  %__CallFf_11___call = tail call i64 @new-boxed-value(i64 1, i64 1)
  %__CallFf_15___call = tail call i64 @fact-set(i64 %arg_0, i64 7671589888336862994, i64 %__CallFf_11___call)
  %__Call_19___call71 = tail call i64 @"is-script-running?"(i64 6150028017864311825)
  %0 = trunc i64 %__Call_19___call71 to i1
  br i1 %0, label %bb3, label %bb2

bb2:                                              ; preds = %bb0, %bb2
  %__CallFf_23___call = tail call i64 @wait-one-frame()
  %__Call_19___call = tail call i64 @"is-script-running?"(i64 6150028017864311825)
  %1 = trunc i64 %__Call_19___call to i1
  br i1 %1, label %bb3, label %bb2

bb3:                                              ; preds = %bb2, %bb0
  %__CallFf_33___call = tail call i64 @new-boxed-value(i64 7, i64 %arg_0)
  %__CallFf_37___call = tail call i64 @send-event(i64 -3266162430027026367, i64 6150028017864311825, i64 %__CallFf_33___call)
  %2 = trunc i64 %arg_2 to i1
  br i1 %2, label %bb4, label %bb5

bb4:                                              ; preds = %bb3
  %__Call_45___call = tail call i64 @internal-put-on-gas-mask-immediately(i64 %arg_0, i64 %arg_1)
  br label %bb6.preheader

bb5:                                              ; preds = %bb3
  %__Call_52___call = tail call i64 @internal-put-on-gas-mask(i64 %arg_0, i64 %arg_1)
  br label %bb6.preheader

bb6.preheader:                                    ; preds = %bb4, %bb5
  %__Call_56___call74 = tail call i64 @"is-script-running?"(i64 6150028017864311825)
  %3 = trunc i64 %__Call_56___call74 to i1
  br i1 %3, label %bb8, label %bb7

bb7:                                              ; preds = %bb6.preheader, %bb7
  %__CallFf_60___call = tail call i64 @wait-one-frame()
  %__Call_56___call = tail call i64 @"is-script-running?"(i64 6150028017864311825)
  %4 = trunc i64 %__Call_56___call to i1
  br i1 %4, label %bb8, label %bb7

bb8:                                              ; preds = %bb7, %bb6.preheader
  %__CallFf_65___call = tail call i64 @get-gas-mask(i64 %arg_0)
  ret i64 %__CallFf_65___call
}

declare i64 @fact-set(i64, i64, i64) local_unnamed_addr

declare i64 @new-boxed-value(i64, i64) local_unnamed_addr

declare i64 @"is-script-running?"(i64) local_unnamed_addr

declare i64 @wait-one-frame() local_unnamed_addr

declare i64 @send-event(i64, i64, i64) local_unnamed_addr

declare i64 @internal-put-on-gas-mask-immediately(i64, i64) local_unnamed_addr

declare i64 @internal-put-on-gas-mask(i64, i64) local_unnamed_addr

declare i64 @get-gas-mask(i64) local_unnamed_addr

define i64 @internal-put-on-gas-mask.1(i64 %arg_0, i64 %arg_1) local_unnamed_addr {
bb0:
  %__CallFf_3___call = tail call i64 @wait-one-frame()
  %__CallFf_8___call = tail call i64 @get-gas-mask-setting-id(i64 %arg_0)
  %__CallFf_12___call = tail call i64 @lookup-symbol(i64 %__CallFf_8___call, i64 3455792245619945009)
  %0 = inttoptr i64 %__CallFf_12___call to ptr
  %__IAddImm_17__op = getelementptr i8, ptr %0, i64 56
  %__LoadU8_18___load = load i8, ptr %__IAddImm_17__op, align 1
  %r50.sroa.0.0.insert.ext94 = zext i8 %__LoadU8_18___load to i64
  %__Call_21___call = tail call i64 @wait-until-in-valid-gas-mask-state(i64 %arg_0, i64 %r50.sroa.0.0.insert.ext94)
  %__IEqual_24__op = icmp eq i64 %arg_1, 0
  %extract.t503 = trunc i64 %arg_1 to i32
  %extract505 = lshr i64 %arg_1, 32
  %extract.t506 = trunc nuw i64 %extract505 to i32
  br i1 %__IEqual_24__op, label %bb1, label %bb3

bb1:                                              ; preds = %bb0
  %__IAddImm_29__op = getelementptr i8, ptr %0, i64 40
  %__LoadU64_30___load = load i64, ptr %__IAddImm_29__op, align 4
  %__CallFf_48___call = tail call i64 @spawn-object(i64 %__LoadU64_30___load, i64 267125575217391726, i64 0, i64 0, i64 1, i64 0, i64 0, i64 0, i64 0)
  %extract.t = trunc i64 %__CallFf_48___call to i32
  %extract = lshr i64 %__CallFf_48___call, 32
  %extract.t504 = trunc nuw i64 %extract to i32
  br label %bb3

bb3:                                              ; preds = %bb0, %bb1
  %arg_1.sink502.off0 = phi i32 [ %extract.t, %bb1 ], [ %extract.t503, %bb0 ]
  %arg_1.sink502.off32 = phi i32 [ %extract.t504, %bb1 ], [ %extract.t506, %bb0 ]
  %r1.sroa.13.0.insert.ext374 = zext i32 %arg_1.sink502.off32 to i64
  %r1.sroa.13.0.insert.shift375 = shl nuw i64 %r1.sroa.13.0.insert.ext374, 32
  %r1.sroa.0.0.insert.ext343 = zext i32 %arg_1.sink502.off0 to i64
  %r1.sroa.0.0.insert.insert345 = or disjoint i64 %r1.sroa.13.0.insert.shift375, %r1.sroa.0.0.insert.ext343
  %__CallFf_58___call = tail call i64 @set-high-contrast-mode-type(i64 %r1.sroa.0.0.insert.insert345, i64 6)
  %__CallFf_62___call = tail call i64 @"is-player?"(i64 %arg_0)
  %1 = trunc i64 %__CallFf_62___call to i1
  br i1 %1, label %bb4, label %bb5

bb4:                                              ; preds = %bb3
  %__CallFf_69___call = tail call i64 @set-instance-flag-hero(i64 %r1.sroa.0.0.insert.insert345, i64 1)
  %__CallFf_75___call = tail call i64 @set-show-in-flashlight-flag(i64 %r1.sroa.0.0.insert.insert345, i64 0)
  br label %bb6

bb5:                                              ; preds = %bb3
  %__CallFf_82___call = tail call i64 @set-shadow-casting-from-npc-flashlight(i64 %r1.sroa.0.0.insert.insert345, i64 0)
  br label %bb6

bb6:                                              ; preds = %bb5, %bb4
  %__CallFf_88___call = tail call i64 @set-gas-mask(i64 %arg_0, i64 %r1.sroa.0.0.insert.insert345)
  %__CallFf_94___call = tail call i64 @set-visible(i64 %r1.sroa.0.0.insert.insert345, i64 0)
  %__CallFf_98___call = tail call i64 @"is-player?"(i64 %arg_0)
  %2 = trunc i64 %__CallFf_98___call to i1
  br i1 %2, label %bb9, label %bb7

bb7:                                              ; preds = %bb6
  %__CallFf_102___call = tail call i64 @wait-one-frame()
  br label %bb9

bb9:                                              ; preds = %bb6, %bb7
  %__CallFf_114___call = tail call i64 @attach(i64 %r1.sroa.0.0.insert.insert345, i64 %arg_0, i64 8667864169316382944, float 0.000000e+00)
  %__CallFf_123___call = tail call i64 @new-boxed-value(i64 6, float 0x3FD3333340000000)
  %__CallFf_127___call = tail call i64 @send-event(i64 6817768622307059015, i64 %arg_0, i64 %__CallFf_123___call)
  %r3.sroa.30.0.extract.shift472 = and i64 ptrtoint (ptr @animate_ to i64), -256
  %__IAddImm_132__op = getelementptr i8, ptr %0, i64 16
  %__LoadU64_133___load = load i64, ptr %__IAddImm_132__op, align 4
  %__CallFf_143___call = tail call i64 @"#%alloc-array"(i64 8534565298598421705, i64 16, i64 0, i64 1)
  %r3.sroa.0.0.insert.ext = zext i8 ptrtoint (ptr @animate_ to i8) to i64
  %r3.sroa.0.0.insert.insert = or disjoint i64 %r3.sroa.30.0.extract.shift472, %r3.sroa.0.0.insert.ext
  %3 = inttoptr i64 %r3.sroa.0.0.insert.insert to ptr
  %__CallFf_149___call = tail call i64 %3(i64 %r1.sroa.0.0.insert.insert345, i64 %__LoadU64_133___load, i64 %__CallFf_143___call)
  %r3.sroa.30.0.extract.shift474 = and i64 ptrtoint (ptr @gesture_ to i64), -256
  %r3.sroa.0.0.insert.ext435 = zext i8 ptrtoint (ptr @gesture_ to i8) to i64
  %r3.sroa.0.0.insert.insert437 = or disjoint i64 %r3.sroa.30.0.extract.shift474, %r3.sroa.0.0.insert.ext435
  %4 = inttoptr i64 %r3.sroa.0.0.insert.insert437 to ptr
  br label %bb10

bb10:                                             ; preds = %bb11, %bb9
  %__LoadU64_154___load = load i64, ptr %0, align 4
  %__CallFf_156___call = tail call i64 @alloc-gesture-play-params()
  %__IAddImm_158__op = add i64 %__CallFf_156___call, 33
  %5 = inttoptr i64 %__IAddImm_158__op to ptr
  store i8 1, ptr %5, align 1
  %__IAddImm_162__op = add i64 %__CallFf_156___call, 36
  %6 = inttoptr i64 %__IAddImm_162__op to ptr
  store i32 19, ptr %6, align 4
  %__IAddImm_166__op = add i64 %__CallFf_156___call, 32
  %7 = inttoptr i64 %__IAddImm_166__op to ptr
  store i8 1, ptr %7, align 1
  %__IAddImm_170__op = add i64 %__CallFf_156___call, 34
  %8 = inttoptr i64 %__IAddImm_170__op to ptr
  store i8 1, ptr %8, align 1
  %__CallFf_178___call = tail call i64 %4(i64 %arg_0, i64 %__LoadU64_154___load, i64 %__CallFf_156___call)
  %__IEqual_180__op.not = icmp eq i64 %__CallFf_178___call, 0
  br i1 %__IEqual_180__op.not, label %bb12, label %bb11

bb11:                                             ; preds = %bb10
  %__CallFf_184___call = tail call i64 @wait-one-frame()
  br label %bb10

bb12:                                             ; preds = %bb10
  %__IAddImm_190__op = getelementptr i8, ptr %0, i64 24
  %__LoadU64_191___load = load i64, ptr %__IAddImm_190__op, align 4
  %__CallFf_201___call = tail call i64 @"#%alloc-array"(i64 8534565298598421705, i64 16, i64 0, i64 1)
  %__CallFf_207___call = tail call i64 %3(i64 %r1.sroa.0.0.insert.insert345, i64 %__LoadU64_191___load, i64 %__CallFf_201___call)
  ret i64 %__CallFf_207___call
}

declare i64 @lookup-symbol(i64, i64) local_unnamed_addr

declare i64 @get-gas-mask-setting-id(i64) local_unnamed_addr

declare i64 @wait-until-in-valid-gas-mask-state(i64, i64) local_unnamed_addr

declare i64 @spawn-object(i64, i64, i64, i64, i64, i64, i64, i64, i64) local_unnamed_addr

declare i64 @set-high-contrast-mode-type(i64, i64) local_unnamed_addr

declare i64 @"is-player?"(i64) local_unnamed_addr

declare i64 @set-instance-flag-hero(i64, i64) local_unnamed_addr

declare i64 @set-show-in-flashlight-flag(i64, i64) local_unnamed_addr

declare i64 @set-shadow-casting-from-npc-flashlight(i64, i64) local_unnamed_addr

declare i64 @set-gas-mask(i64, i64) local_unnamed_addr

declare i64 @set-visible(i64, i64) local_unnamed_addr

declare i64 @attach(i64, i64, i64, float) local_unnamed_addr

declare i64 @animate_(i64, i64, i64)

declare i64 @"#%alloc-array"(i64, i64, i64, i64) local_unnamed_addr

declare i64 @gesture_(i64, i64, i64)

declare i64 @alloc-gesture-play-params() local_unnamed_addr

define i64 @internal-put-on-gas-mask-immediately.3(i64 %arg_0, i64 %arg_1) local_unnamed_addr {
bb0:
  %__CallFf_6___call = tail call i64 @get-gas-mask-setting-id(i64 %arg_0)
  %__CallFf_10___call = tail call i64 @lookup-symbol(i64 %__CallFf_6___call, i64 3455792245619945009)
  %__IEqual_13__op = icmp eq i64 %arg_1, 0
  %extract.t353 = trunc i64 %arg_1 to i32
  %extract355 = lshr i64 %arg_1, 32
  %extract.t356 = trunc nuw i64 %extract355 to i32
  br i1 %__IEqual_13__op, label %bb1, label %bb3

bb1:                                              ; preds = %bb0
  %0 = inttoptr i64 %__CallFf_10___call to ptr
  %__IAddImm_18__op = getelementptr i8, ptr %0, i64 40
  %__LoadU64_19___load = load i64, ptr %__IAddImm_18__op, align 4
  %__CallFf_37___call = tail call i64 @spawn-object(i64 %__LoadU64_19___load, i64 267125575217391726, i64 0, i64 0, i64 1, i64 0, i64 0, i64 0, i64 0)
  %extract.t = trunc i64 %__CallFf_37___call to i32
  %extract = lshr i64 %__CallFf_37___call, 32
  %extract.t354 = trunc nuw i64 %extract to i32
  br label %bb3

bb3:                                              ; preds = %bb0, %bb1
  %arg_1.sink352.off0 = phi i32 [ %extract.t, %bb1 ], [ %extract.t353, %bb0 ]
  %arg_1.sink352.off32 = phi i32 [ %extract.t354, %bb1 ], [ %extract.t356, %bb0 ]
  %r1.sroa.10.0.insert.ext287 = zext i32 %arg_1.sink352.off32 to i64
  %r1.sroa.10.0.insert.shift288 = shl nuw i64 %r1.sroa.10.0.insert.ext287, 32
  %r1.sroa.0.0.insert.ext265 = zext i32 %arg_1.sink352.off0 to i64
  %r1.sroa.0.0.insert.insert267 = or disjoint i64 %r1.sroa.10.0.insert.shift288, %r1.sroa.0.0.insert.ext265
  %__CallFf_47___call = tail call i64 @set-high-contrast-mode-type(i64 %r1.sroa.0.0.insert.insert267, i64 6)
  %__CallFf_53___call = tail call i64 @set-gas-mask(i64 %arg_0, i64 %r1.sroa.0.0.insert.insert267)
  %__CallFf_57___call = tail call i64 @"is-player?"(i64 %arg_0)
  %1 = trunc i64 %__CallFf_57___call to i1
  br i1 %1, label %bb4, label %bb6

bb4:                                              ; preds = %bb3
  %__CallFf_67___call = tail call i64 @new-boxed-value(i64 6, float 0.000000e+00)
  %__CallFf_71___call = tail call i64 (i64, i64, ...) @send-event(i64 6817768622307059015, i64 %arg_0, i64 %__CallFf_67___call)
  %__CallFf_77___call = tail call i64 @set-instance-flag-hero(i64 %r1.sroa.0.0.insert.insert267, i64 1)
  br label %bb6

bb6:                                              ; preds = %bb3, %bb4
  %__CallFf_85___call = tail call i64 (i64, i64, ...) @send-event(i64 1827492438953869695, i64 %arg_0)
  %__CallFf_89___call = tail call i64 @"is-player?"(i64 %arg_0)
  %2 = trunc i64 %__CallFf_89___call to i1
  br i1 %2, label %bb7, label %bb8

bb7:                                              ; preds = %bb6
  %__CallFf_96___call = tail call i64 @set-show-in-flashlight-flag(i64 %r1.sroa.0.0.insert.insert267, i64 0)
  br label %bb9

bb8:                                              ; preds = %bb6
  %__CallFf_103___call = tail call i64 @set-shadow-casting-from-npc-flashlight(i64 %r1.sroa.0.0.insert.insert267, i64 0)
  br label %bb9

bb9:                                              ; preds = %bb8, %bb7
  %r3.sroa.19.0.extract.shift340 = and i64 ptrtoint (ptr @animate_ to i64), -256
  %3 = inttoptr i64 %__CallFf_10___call to ptr
  %__IAddImm_108__op = getelementptr i8, ptr %3, i64 24
  %__LoadU64_109___load = load i64, ptr %__IAddImm_108__op, align 4
  %__CallFf_119___call = tail call i64 @"#%alloc-array"(i64 8534565298598421705, i64 16, i64 1, i64 1)
  %4 = inttoptr i64 %__CallFf_119___call to ptr
  store i32 31, ptr %4, align 4
  %__IAddImm_143__op = add i64 %__CallFf_119___call, 8
  %5 = inttoptr i64 %__IAddImm_143__op to ptr
  store i8 1, ptr %5, align 1
  %r3.sroa.0.0.insert.ext = zext i8 ptrtoint (ptr @animate_ to i8) to i64
  %r3.sroa.0.0.insert.insert = or disjoint i64 %r3.sroa.19.0.extract.shift340, %r3.sroa.0.0.insert.ext
  %6 = inttoptr i64 %r3.sroa.0.0.insert.insert to ptr
  %__CallFf_151___call = tail call i64 %6(i64 %r1.sroa.0.0.insert.insert267, i64 %__LoadU64_109___load, i64 %__CallFf_119___call)
  ret i64 %__CallFf_151___call
}

define i64 @get-equipped-gas-mask(i64 %arg_0) local_unnamed_addr {
bb0:
  %__CallFf_4___call = tail call i64 @get-gas-mask(i64 %arg_0)
  ret i64 %__CallFf_4___call
}

define i64 @wait-until-in-valid-gas-mask-state.6(i64 %arg_0, i64 %arg_1) local_unnamed_addr {
bb0:
  %__CallFf_5___call = tail call i64 @"is-player?"(i64 %arg_0)
  %0 = trunc i64 %__CallFf_5___call to i1
  br i1 %0, label %bb1, label %bb15

bb1:                                              ; preds = %bb0, %bb3
  %__CallFf_10___call = tail call i64 @"player-in-state?"(i64 6740565536053766520)
  %1 = trunc i64 %__CallFf_10___call to i1
  br i1 %1, label %bb3, label %bb2

bb2:                                              ; preds = %bb1
  %__CallFf_16___call = tail call i64 @"player-in-state?"(i64 2237451099064635199)
  %2 = trunc i64 %__CallFf_16___call to i1
  br i1 %2, label %bb3, label %bb4

bb3:                                              ; preds = %bb2, %bb1
  %__CallFf_21___call = tail call i64 @wait-one-frame()
  br label %bb1

bb4:                                              ; preds = %bb2
  %3 = trunc i64 %arg_1 to i1
  br i1 %3, label %bb5, label %bb9

bb5:                                              ; preds = %bb4
  %__CallFf_26___call = tail call i64 @player-force-crouch()
  %__CallFf_28___call250 = tail call i64 @"player-is-crouched?"()
  %4 = trunc i64 %__CallFf_28___call250 to i1
  br i1 %4, label %bb12.preheader, label %bb7

bb12.preheader:                                   ; preds = %bb11, %bb7, %bb9, %bb5
  %__CallFf_50___call253 = tail call i64 @"player-in-state?"(i64 4007104907368024212)
  %5 = trunc i64 %__CallFf_50___call253 to i1
  br i1 %5, label %bb14, label %bb13

bb7:                                              ; preds = %bb5, %bb7
  %__CallFf_32___call = tail call i64 @wait-one-frame()
  %__CallFf_28___call = tail call i64 @"player-is-crouched?"()
  %6 = trunc i64 %__CallFf_28___call to i1
  br i1 %6, label %bb12.preheader, label %bb7

bb9:                                              ; preds = %bb4
  %__CallFf_36___call = tail call i64 @player-force-stand()
  %__CallFf_39___call246 = tail call i64 @"player-is-crouched?"()
  %__Call_41___call247 = tail call i64 @"!"(i64 %__CallFf_39___call246)
  %7 = trunc i64 %__Call_41___call247 to i1
  br i1 %7, label %bb12.preheader, label %bb11

bb11:                                             ; preds = %bb9, %bb11
  %__CallFf_45___call = tail call i64 @wait-one-frame()
  %__CallFf_39___call = tail call i64 @"player-is-crouched?"()
  %__Call_41___call = tail call i64 @"!"(i64 %__CallFf_39___call)
  %8 = trunc i64 %__Call_41___call to i1
  br i1 %8, label %bb12.preheader, label %bb11

bb13:                                             ; preds = %bb12.preheader, %bb13
  %__CallFf_54___call = tail call i64 @wait-one-frame()
  %__CallFf_50___call = tail call i64 @"player-in-state?"(i64 4007104907368024212)
  %9 = trunc i64 %__CallFf_50___call to i1
  br i1 %9, label %bb14, label %bb13

bb14:                                             ; preds = %bb13, %bb12.preheader
  %__CallFf_61___call = tail call i64 @wait-holster-player-weapon(i64 0, float 1.000000e+00)
  %__CallFf_67___call = tail call i64 @holster-player-weapon-instantly(float 0x3FC99999A0000000, i64 0)
  %r2.sroa.0.0.extract.trunc179 = trunc i64 %__CallFf_67___call to i8
  %r2.sroa.39.0.extract.shift235 = lshr i64 %__CallFf_67___call, 8
  %r2.sroa.39.0.extract.trunc236 = trunc nuw i64 %r2.sroa.39.0.extract.shift235 to i56
  br label %bb21

bb15:                                             ; preds = %bb0
  %__CallFf_80___call = tail call i64 @wait-npc-holster-weapon(i64 %arg_0, i64 0, i64 1, float -1.000000e+00, i64 0)
  %__CallFf_84___call244 = tail call i64 @"npc-has-weapon-in-hand?"(i64 %arg_0)
  %10 = trunc i64 %__CallFf_84___call244 to i1
  br i1 %10, label %bb17, label %bb18

bb17:                                             ; preds = %bb15, %bb17
  %__CallFf_87___call = tail call i64 @wait-one-frame()
  %__CallFf_84___call = tail call i64 @"npc-has-weapon-in-hand?"(i64 %arg_0)
  %11 = trunc i64 %__CallFf_84___call to i1
  br i1 %11, label %bb17, label %bb18

bb18:                                             ; preds = %bb17, %bb15
  %12 = trunc i64 %arg_1 to i1
  br i1 %12, label %bb19, label %bb21

bb19:                                             ; preds = %bb18
  %__CallFf_96___call = tail call i64 @wait-npc-set-demeanor(i64 %arg_0, i64 3)
  %r2.sroa.0.0.extract.trunc182 = trunc i64 %__CallFf_96___call to i8
  %r2.sroa.39.0.extract.shift241 = lshr i64 %__CallFf_96___call, 8
  %r2.sroa.39.0.extract.trunc242 = trunc nuw i64 %r2.sroa.39.0.extract.shift241 to i56
  br label %bb21

bb21:                                             ; preds = %bb18, %bb19, %bb14
  %r2.sroa.0.sroa.0.0.r2.sroa.0.sroa.0.0.r2.sroa.0.sroa.0.0.r2.sroa.0.0.r2.0.__Return_99___retval = phi i8 [ %r2.sroa.0.0.extract.trunc179, %bb14 ], [ %r2.sroa.0.0.extract.trunc182, %bb19 ], [ 0, %bb18 ]
  %r2.sroa.39.sroa.0.0 = phi i56 [ %r2.sroa.39.0.extract.trunc236, %bb14 ], [ %r2.sroa.39.0.extract.trunc242, %bb19 ], [ 0, %bb18 ]
  %r2.sroa.39.0.insert.ext = zext i56 %r2.sroa.39.sroa.0.0 to i64
  %r2.sroa.39.0.insert.shift = shl nuw i64 %r2.sroa.39.0.insert.ext, 8
  %r2.sroa.0.0.insert.ext = zext i8 %r2.sroa.0.sroa.0.0.r2.sroa.0.sroa.0.0.r2.sroa.0.sroa.0.0.r2.sroa.0.0.r2.0.__Return_99___retval to i64
  %r2.sroa.0.0.insert.insert = or disjoint i64 %r2.sroa.39.0.insert.shift, %r2.sroa.0.0.insert.ext
  ret i64 %r2.sroa.0.0.insert.insert
}

declare i64 @"player-in-state?"(i64) local_unnamed_addr

declare i64 @player-force-crouch() local_unnamed_addr

declare i64 @"player-is-crouched?"() local_unnamed_addr

declare i64 @player-force-stand() local_unnamed_addr

declare i64 @"!"(i64) local_unnamed_addr

declare i64 @wait-holster-player-weapon(i64, float) local_unnamed_addr

declare i64 @holster-player-weapon-instantly(float, i64) local_unnamed_addr

declare i64 @wait-npc-holster-weapon(i64, i64, i64, float, i64) local_unnamed_addr

declare i64 @"npc-has-weapon-in-hand?"(i64) local_unnamed_addr

declare i64 @wait-npc-set-demeanor(i64, i64) local_unnamed_addr

define i64 @"npc-gas-mask-narrative-mode/f"(i64 %arg_0) local_unnamed_addr {
bb0:
  %__CallFf_6___call = tail call i64 @send-event(i64 8119835013146602502, i64 %arg_0)
  %__Call_10___call = tail call i64 @"suspend-idle-gestures/f"(i64 %arg_0)
  %__CallFf_16___call = tail call i64 @npc-disable-dialog-look(i64 %arg_0, float 0.000000e+00)
  ret i64 %__CallFf_16___call
}

declare i64 @"suspend-idle-gestures/f"(i64) local_unnamed_addr

declare i64 @npc-disable-dialog-look(i64, float) local_unnamed_addr

define i64 @internal-put-off-gas-mask-immediately(i64 %arg_0, i64 %arg_1) local_unnamed_addr {
bb0:
  %__CallFf_5___call = tail call i64 @get-gas-mask(i64 %arg_0)
  %__CallFf_11___call = tail call i64 @send-event(i64 -3264453205741318791, i64 %arg_0)
  %0 = trunc i64 %arg_1 to i1
  br i1 %0, label %bb1, label %bb3

bb1:                                              ; preds = %bb0
  %__CallFf_17___call = tail call i64 @kill-entity(i64 %__CallFf_5___call)
  br label %bb3

bb3:                                              ; preds = %bb0, %bb1
  %r3.0 = phi i64 [ %__CallFf_17___call, %bb1 ], [ 0, %bb0 ]
  ret i64 %r3.0
}

declare i64 @kill-entity(i64) local_unnamed_addr

define i64 @spawn-gas-mask(i64 %arg_0) local_unnamed_addr {
bb0:
  %__CallFf_5___call = tail call i64 @get-gas-mask-setting-id(i64 %arg_0)
  %__CallFf_9___call = tail call i64 @lookup-symbol(i64 %__CallFf_5___call, i64 3455792245619945009)
  %0 = inttoptr i64 %__CallFf_9___call to ptr
  %__IAddImm_13__op = getelementptr i8, ptr %0, i64 40
  %__LoadU64_14___load = load i64, ptr %__IAddImm_13__op, align 4
  %__CallFf_32___call = tail call i64 @spawn-object(i64 %__LoadU64_14___load, i64 267125575217391726, i64 0, i64 0, i64 1, i64 0, i64 0, i64 0, i64 0)
  %__CallFf_38___call = tail call i64 @set-high-contrast-mode-type(i64 %__CallFf_32___call, i64 6)
  ret i64 %__CallFf_32___call
}

define i64 @"player-gas-mask-narrative-mode/f"(i64 %arg_0, i64 %arg_1, i64 %arg_2) local_unnamed_addr {
bb0:
  %__CallFf_6___call = tail call i64 @player-disable-dialog-look-gestures(float 0.000000e+00)
  %__CallFf_8___call = tail call i64 @"player-disable-squeeze-through/f"()
  %__Call_16___call = tail call i64 @"player-set-narrative-mode/f"(i64 %arg_0, i64 %arg_1, i64 %arg_2)
  %__Call_20___call = tail call i64 @"suspend-idle-gestures/f"(i64 5008278420455340480)
  %__CallFf_24___call = tail call i64 @joypad-disable-commands(i64 -1699981380503476753)
  ret i64 %__CallFf_24___call
}

declare i64 @player-disable-dialog-look-gestures(float) local_unnamed_addr

declare i64 @"player-disable-squeeze-through/f"() local_unnamed_addr

declare i64 @"player-set-narrative-mode/f"(i64, i64, i64) local_unnamed_addr

declare i64 @joypad-disable-commands(i64) local_unnamed_addr

define i64 @internal-put-off-gas-mask(i64 %arg_0, i64 %arg_1) local_unnamed_addr {
bb0:
  %__CallFf_3___call = tail call i64 @wait-one-frame()
  %__CallFf_8___call = tail call i64 @get-gas-mask-setting-id(i64 %arg_0)
  %__CallFf_12___call = tail call i64 @lookup-symbol(i64 %__CallFf_8___call, i64 3455792245619945009)
  %__CallFf_16___call = tail call i64 @get-gas-mask(i64 %arg_0)
  %0 = inttoptr i64 %__CallFf_12___call to ptr
  %__IAddImm_21__op = getelementptr i8, ptr %0, i64 57
  %__LoadU8_22___load = load i8, ptr %__IAddImm_21__op, align 1
  %__LoadU8_22___ext = zext i8 %__LoadU8_22___load to i64
  %__Call_25___call = tail call i64 @wait-until-in-valid-gas-mask-state(i64 %arg_0, i64 %__LoadU8_22___ext)
  %__IAddImm_30__op = getelementptr i8, ptr %0, i64 32
  %__LoadU64_31___load = load i64, ptr %__IAddImm_30__op, align 4
  %__CallFf_41___call = tail call i64 @"#%alloc-array"(i64 8534565298598421705, i64 16, i64 0, i64 1)
  %__CallFf_47___call = tail call i64 @animate_(i64 %__CallFf_16___call, i64 %__LoadU64_31___load, i64 %__CallFf_41___call)
  %r4.sroa.13.0.extract.shift53 = and i64 ptrtoint (ptr @gesture_ to i64), -256
  %__IAddImm_52__op = getelementptr i8, ptr %0, i64 8
  %r4.sroa.0.0.insert.ext = zext i8 ptrtoint (ptr @gesture_ to i8) to i64
  %r4.sroa.0.0.insert.insert = or disjoint i64 %r4.sroa.13.0.extract.shift53, %r4.sroa.0.0.insert.ext
  %1 = inttoptr i64 %r4.sroa.0.0.insert.insert to ptr
  br label %bb1

bb1:                                              ; preds = %bb2, %bb0
  %__LoadU64_53___load = load i64, ptr %__IAddImm_52__op, align 4
  %__CallFf_55___call = tail call i64 @alloc-gesture-play-params()
  %__IAddImm_57__op = add i64 %__CallFf_55___call, 33
  %2 = inttoptr i64 %__IAddImm_57__op to ptr
  store i8 1, ptr %2, align 1
  %__IAddImm_61__op = add i64 %__CallFf_55___call, 36
  %3 = inttoptr i64 %__IAddImm_61__op to ptr
  store i32 19, ptr %3, align 4
  %__IAddImm_65__op = add i64 %__CallFf_55___call, 32
  %4 = inttoptr i64 %__IAddImm_65__op to ptr
  store i8 1, ptr %4, align 1
  %__IAddImm_69__op = add i64 %__CallFf_55___call, 34
  %5 = inttoptr i64 %__IAddImm_69__op to ptr
  store i8 1, ptr %5, align 1
  %__CallFf_77___call = tail call i64 %1(i64 %arg_0, i64 %__LoadU64_53___load, i64 %__CallFf_55___call)
  %__IEqual_79__op.not = icmp eq i64 %__CallFf_77___call, 0
  br i1 %__IEqual_79__op.not, label %bb3, label %bb2

bb2:                                              ; preds = %bb1
  %__CallFf_83___call = tail call i64 @wait-one-frame()
  br label %bb1

bb3:                                              ; preds = %bb1
  %6 = trunc i64 %arg_1 to i1
  br i1 %6, label %bb4, label %bb6

bb4:                                              ; preds = %bb3
  %__CallFf_90___call = tail call i64 @kill-entity(i64 %__CallFf_16___call)
  br label %bb6

bb6:                                              ; preds = %bb3, %bb4
  %storemerge = phi i64 [ %__CallFf_90___call, %bb4 ], [ 0, %bb3 ]
  ret i64 %storemerge
}

define i64 @wait-remove-gas-mask(i64 %arg_0, i64 %arg_1, i64 %arg_2) local_unnamed_addr {
bb0:
  %__CallFf_11___call = tail call i64 @new-boxed-value(i64 1, i64 0)
  %__CallFf_15___call = tail call i64 @fact-set(i64 %arg_0, i64 7671589888336862994, i64 %__CallFf_11___call)
  %__Call_19___call48 = tail call i64 @"is-script-running?"(i64 6150028017864311825)
  %0 = trunc i64 %__Call_19___call48 to i1
  br i1 %0, label %bb3, label %bb2

bb2:                                              ; preds = %bb0, %bb2
  %__CallFf_23___call = tail call i64 @wait-one-frame()
  %__Call_19___call = tail call i64 @"is-script-running?"(i64 6150028017864311825)
  %1 = trunc i64 %__Call_19___call to i1
  br i1 %1, label %bb3, label %bb2

bb3:                                              ; preds = %bb2, %bb0
  %__CallFf_33___call = tail call i64 @new-boxed-value(i64 7, i64 %arg_0)
  %__CallFf_37___call = tail call i64 @send-event(i64 -3325057813225958160, i64 6150028017864311825, i64 %__CallFf_33___call)
  %2 = trunc i64 %arg_1 to i1
  br i1 %2, label %bb4, label %bb5

bb4:                                              ; preds = %bb3
  %__CallFf_5___call.i = tail call i64 @get-gas-mask(i64 %arg_0)
  %__CallFf_11___call.i = tail call i64 @send-event(i64 -3264453205741318791, i64 %arg_0)
  %3 = trunc i64 %arg_2 to i1
  br i1 %3, label %bb1.i, label %bb6

bb1.i:                                            ; preds = %bb4
  %__CallFf_17___call.i = tail call i64 @kill-entity(i64 %__CallFf_5___call.i)
  br label %bb6

bb5:                                              ; preds = %bb3
  %__Call_52___call = tail call i64 @internal-put-off-gas-mask(i64 %arg_0, i64 %arg_2)
  br label %bb6

bb6:                                              ; preds = %bb1.i, %bb4, %bb5
  %storemerge = phi i64 [ %__Call_52___call, %bb5 ], [ %__CallFf_17___call.i, %bb1.i ], [ 0, %bb4 ]
  ret i64 %storemerge
}

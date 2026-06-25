; ModuleID = 'anim-gas-mask-impl.bin'
source_filename = "anim-gas-mask-impl.bin"
target triple = "x86_64-pc-windows-msvc"

@wait-equip-gas-mask_symbol_table = private unnamed_addr constant [11 x i64] [i64 -2210386714833194546, i64 7671589888336862994, i64 3684810824690217768, i64 6481843155139167450, i64 6150028017864311825, i64 -3047912596559836939, i64 4789555171397753330, i64 -3266162430027026367, i64 6078802017353408438, i64 5608585023750540989, i64 -5076200561558624898], align 8
@internal-put-on-gas-mask_symbol_table = private unnamed_addr constant [26 x i64] [i64 -3047912596559836939, i64 -7658033447216601854, i64 -6573555005595409285, i64 3455792245619945009, i64 8203816822361639083, i64 7097052834867614476, i64 267125575217391726, i64 4766017279285775524, i64 6532480022340268902, i64 -429226074377614887, i64 -7252477440030364545, i64 -3854484060414110792, i64 -5971891483814290574, i64 -7746349783173232884, i64 -5676660224586178492, i64 8667864169316382944, i64 0, i64 4789555171397753330, i64 6817768622307059015, i64 3684810824690217768, i64 1050253722, i64 -9179819928081802467, i64 -7745656483000545638, i64 8534565298598421705, i64 6500315221283771953, i64 -2962780980939953026], align 8
@internal-put-on-gas-mask-immediately_symbol_table = private unnamed_addr constant [19 x i64] [i64 -7658033447216601854, i64 -6573555005595409285, i64 3455792245619945009, i64 7097052834867614476, i64 267125575217391726, i64 4766017279285775524, i64 -5971891483814290574, i64 6532480022340268902, i64 4789555171397753330, i64 6817768622307059015, i64 3684810824690217768, i64 0, i64 -429226074377614887, i64 1827492438953869695, i64 -7252477440030364545, i64 -3854484060414110792, i64 -9179819928081802467, i64 -7745656483000545638, i64 8534565298598421705], align 8
@get-equipped-gas-mask_symbol_table = private unnamed_addr constant [1 x i64] [i64 -5076200561558624898], align 8
@wait-until-in-valid-gas-mask-state_symbol_table = private unnamed_addr constant [18 x i64] [i64 6532480022340268902, i64 -9003251364961024783, i64 6740565536053766520, i64 2237451099064635199, i64 -3047912596559836939, i64 -4831287436211302109, i64 1646463116072042322, i64 8594926419455150747, i64 -5808627241898115124, i64 4007104907368024212, i64 -3092934816467423265, i64 1065353216, i64 -626103520744544694, i64 1045220557, i64 5808793362033684947, i64 3212836864, i64 7683024912818272899, i64 8573502661920361253], align 8
@"npc-gas-mask-narrative-mode/f_symbol_table" = private unnamed_addr constant [5 x i64] [i64 4789555171397753330, i64 8119835013146602502, i64 -5356689657310757786, i64 7610916627332680768, i64 0], align 8
@internal-put-off-gas-mask-immediately_symbol_table = private unnamed_addr constant [4 x i64] [i64 -5076200561558624898, i64 4789555171397753330, i64 -3264453205741318791, i64 -804116741684351865], align 8
@spawn-gas-mask_symbol_table = private unnamed_addr constant [6 x i64] [i64 -7658033447216601854, i64 -6573555005595409285, i64 3455792245619945009, i64 7097052834867614476, i64 267125575217391726, i64 4766017279285775524], align 8
@"player-gas-mask-narrative-mode/f_symbol_table" = private unnamed_addr constant [8 x i64] [i64 7238593933813171071, i64 0, i64 8165897991486741957, i64 -2826777519175913837, i64 -5356689657310757786, i64 5008278420455340480, i64 7121253686161376880, i64 -1699981380503476753], align 8
@internal-put-off-gas-mask_symbol_table = private unnamed_addr constant [12 x i64] [i64 -3047912596559836939, i64 -7658033447216601854, i64 -6573555005595409285, i64 3455792245619945009, i64 -5076200561558624898, i64 8203816822361639083, i64 -9179819928081802467, i64 -7745656483000545638, i64 8534565298598421705, i64 6500315221283771953, i64 -2962780980939953026, i64 -804116741684351865], align 8
@wait-remove-gas-mask_symbol_table = private unnamed_addr constant [10 x i64] [i64 -2210386714833194546, i64 7671589888336862994, i64 3684810824690217768, i64 6481843155139167450, i64 6150028017864311825, i64 -3047912596559836939, i64 4789555171397753330, i64 -3325057813225958160, i64 -8559530813269772124, i64 -6610620527767074061], align 8
@llvm.used = appending global [11 x ptr] [ptr @get-equipped-gas-mask_symbol_table, ptr @internal-put-off-gas-mask-immediately_symbol_table, ptr @internal-put-off-gas-mask_symbol_table, ptr @internal-put-on-gas-mask-immediately_symbol_table, ptr @internal-put-on-gas-mask_symbol_table, ptr @"npc-gas-mask-narrative-mode/f_symbol_table", ptr @"player-gas-mask-narrative-mode/f_symbol_table", ptr @spawn-gas-mask_symbol_table, ptr @wait-equip-gas-mask_symbol_table, ptr @wait-remove-gas-mask_symbol_table, ptr @wait-until-in-valid-gas-mask-state_symbol_table], section "llvm.metadata"

; Function Attrs: nosync nounwind
define i64 @wait-equip-gas-mask(i64 %arg_0, i64 %arg_1, i64 %arg_2) local_unnamed_addr #0 {
bb0:
  %__LookupPointer_new-boxed-value_fptr = call ptr @llvm.dcvm.lookup(i64 3684810824690217768)
  %__CallFf_11___call = tail call i64 %__LookupPointer_new-boxed-value_fptr(i64 1, i64 1), !callees !0
  %__LookupPointer_fact-set_fptr = call ptr @llvm.dcvm.lookup(i64 -2210386714833194546)
  %__CallFf_15___call = tail call i64 %__LookupPointer_fact-set_fptr(i64 %arg_0, i64 7671589888336862994, i64 %__CallFf_11___call), !callees !1
  %"__LookupPointer_is-script-running?_fptr" = call ptr @llvm.dcvm.lookup(i64 6481843155139167450)
  %__Call_19___call71 = tail call i64 %"__LookupPointer_is-script-running?_fptr"(i64 6150028017864311825), !callees !2
  %0 = trunc i64 %__Call_19___call71 to i1
  br i1 %0, label %bb3, label %bb2

bb2:                                              ; preds = %bb0, %bb2
  %__LookupPointer_wait-one-frame_fptr = call ptr @llvm.dcvm.lookup(i64 -3047912596559836939)
  %__CallFf_23___call = tail call i64 %__LookupPointer_wait-one-frame_fptr(), !callees !3
  %"__LookupPointer_is-script-running?_fptr77" = call ptr @llvm.dcvm.lookup(i64 6481843155139167450)
  %__Call_19___call = tail call i64 %"__LookupPointer_is-script-running?_fptr77"(i64 6150028017864311825), !callees !2
  %1 = trunc i64 %__Call_19___call to i1
  br i1 %1, label %bb3, label %bb2

bb3:                                              ; preds = %bb2, %bb0
  %__LookupPointer_new-boxed-value_fptr78 = call ptr @llvm.dcvm.lookup(i64 3684810824690217768)
  %__CallFf_33___call = tail call i64 %__LookupPointer_new-boxed-value_fptr78(i64 7, i64 %arg_0), !callees !0
  %__LookupPointer_send-event_fptr = call ptr @llvm.dcvm.lookup(i64 4789555171397753330)
  %__CallFf_37___call = tail call i64 %__LookupPointer_send-event_fptr(i64 -3266162430027026367, i64 6150028017864311825, i64 %__CallFf_33___call), !callees !4
  %__BranchIfNot_39___bool.not = icmp eq i64 %arg_2, 0
  br i1 %__BranchIfNot_39___bool.not, label %bb5, label %bb4

bb4:                                              ; preds = %bb3
  %__LookupPointer_internal-put-on-gas-mask-immediately_fptr = call ptr @llvm.dcvm.lookup(i64 6078802017353408438)
  %__Call_45___call = tail call i64 %__LookupPointer_internal-put-on-gas-mask-immediately_fptr(i64 %arg_0, i64 %arg_1), !callees !5
  br label %bb6.preheader

bb5:                                              ; preds = %bb3
  %__LookupPointer_internal-put-on-gas-mask_fptr = call ptr @llvm.dcvm.lookup(i64 5608585023750540989)
  %__Call_52___call = tail call i64 %__LookupPointer_internal-put-on-gas-mask_fptr(i64 %arg_0, i64 %arg_1), !callees !6
  br label %bb6.preheader

bb6.preheader:                                    ; preds = %bb4, %bb5
  %"__LookupPointer_is-script-running?_fptr79" = call ptr @llvm.dcvm.lookup(i64 6481843155139167450)
  %__Call_56___call74 = tail call i64 %"__LookupPointer_is-script-running?_fptr79"(i64 6150028017864311825), !callees !2
  %2 = trunc i64 %__Call_56___call74 to i1
  br i1 %2, label %bb8, label %bb7

bb7:                                              ; preds = %bb6.preheader, %bb7
  %__LookupPointer_wait-one-frame_fptr80 = call ptr @llvm.dcvm.lookup(i64 -3047912596559836939)
  %__CallFf_60___call = tail call i64 %__LookupPointer_wait-one-frame_fptr80(), !callees !3
  %"__LookupPointer_is-script-running?_fptr81" = call ptr @llvm.dcvm.lookup(i64 6481843155139167450)
  %__Call_56___call = tail call i64 %"__LookupPointer_is-script-running?_fptr81"(i64 6150028017864311825), !callees !2
  %3 = trunc i64 %__Call_56___call to i1
  br i1 %3, label %bb8, label %bb7

bb8:                                              ; preds = %bb7, %bb6.preheader
  %__LookupPointer_get-gas-mask_fptr = call ptr @llvm.dcvm.lookup(i64 -5076200561558624898)
  %__CallFf_65___call = tail call i64 %__LookupPointer_get-gas-mask_fptr(i64 %arg_0), !callees !7
  ret i64 %__CallFf_65___call
}

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !8 i64 @fact-set(i64, i64, i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !9 i64 @new-boxed-value(i64, i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !10 i64 @"is-script-running?"(i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !11 i64 @wait-one-frame() local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !12 i64 @send-event(i64, i64, i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !13 i64 @internal-put-on-gas-mask-immediately(i64, i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !14 i64 @internal-put-on-gas-mask(i64, i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !15 i64 @get-gas-mask(i64) local_unnamed_addr #1

define i64 @internal-put-on-gas-mask.1(i64 %arg_0, i64 %arg_1) local_unnamed_addr {
bb0:
  %__LookupPointer_wait-one-frame_fptr = call ptr @llvm.dcvm.lookup(i64 -3047912596559836939)
  %__CallFf_3___call = tail call i64 %__LookupPointer_wait-one-frame_fptr(), !callees !3
  %__LookupPointer_get-gas-mask-setting-id_fptr = call ptr @llvm.dcvm.lookup(i64 -6573555005595409285)
  %__CallFf_8___call = tail call i64 %__LookupPointer_get-gas-mask-setting-id_fptr(i64 %arg_0), !callees !16
  %__LookupPointer_lookup-symbol_fptr = call ptr @llvm.dcvm.lookup(i64 -7658033447216601854)
  %__CallFf_12___call = tail call i64 %__LookupPointer_lookup-symbol_fptr(i64 %__CallFf_8___call, i64 3455792245619945009), !callees !17
  %0 = inttoptr i64 %__CallFf_12___call to ptr
  %__IAddImm_17__op = getelementptr i8, ptr %0, i64 56
  %__LoadU8_18___load = load i8, ptr %__IAddImm_17__op, align 1
  %r50.sroa.0.0.insert.ext94 = zext i8 %__LoadU8_18___load to i64
  %__LookupPointer_wait-until-in-valid-gas-mask-state_fptr = call ptr @llvm.dcvm.lookup(i64 8203816822361639083)
  %__Call_21___call = tail call i64 %__LookupPointer_wait-until-in-valid-gas-mask-state_fptr(i64 %arg_0, i64 %r50.sroa.0.0.insert.ext94), !callees !18
  %__IEqual_24__op = icmp eq i64 %arg_1, 0
  %extract.t503 = trunc i64 %arg_1 to i32
  %extract505 = lshr i64 %arg_1, 32
  %extract.t506 = trunc nuw i64 %extract505 to i32
  br i1 %__IEqual_24__op, label %bb1, label %bb3

bb1:                                              ; preds = %bb0
  %__IAddImm_29__op = getelementptr i8, ptr %0, i64 40
  %__LoadU64_30___load = load i64, ptr %__IAddImm_29__op, align 4
  %__LookupPointer_spawn-object_fptr = call ptr @llvm.dcvm.lookup(i64 7097052834867614476)
  %__CallFf_48___call = tail call i64 %__LookupPointer_spawn-object_fptr(i64 %__LoadU64_30___load, i64 267125575217391726, i64 0, i64 0, i64 1, i64 0, i64 0, i64 0, i64 0), !callees !19
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
  %__LookupPointer_set-high-contrast-mode-type_fptr = call ptr @llvm.dcvm.lookup(i64 4766017279285775524)
  %__CallFf_58___call = tail call i64 %__LookupPointer_set-high-contrast-mode-type_fptr(i64 %r1.sroa.0.0.insert.insert345, i64 6), !callees !20
  %"__LookupPointer_is-player?_fptr" = call ptr @llvm.dcvm.lookup(i64 6532480022340268902)
  %__CallFf_62___call = tail call i64 %"__LookupPointer_is-player?_fptr"(i64 %arg_0), !callees !21
  %__BranchIfNot_63___bool.not = icmp eq i64 %__CallFf_62___call, 0
  br i1 %__BranchIfNot_63___bool.not, label %bb5, label %bb4

bb4:                                              ; preds = %bb3
  %__LookupPointer_set-instance-flag-hero_fptr = call ptr @llvm.dcvm.lookup(i64 -429226074377614887)
  %__CallFf_69___call = tail call i64 %__LookupPointer_set-instance-flag-hero_fptr(i64 %r1.sroa.0.0.insert.insert345, i64 1), !callees !22
  %__LookupPointer_set-show-in-flashlight-flag_fptr = call ptr @llvm.dcvm.lookup(i64 -7252477440030364545)
  %__CallFf_75___call = tail call i64 %__LookupPointer_set-show-in-flashlight-flag_fptr(i64 %r1.sroa.0.0.insert.insert345, i64 0), !callees !23
  br label %bb6

bb5:                                              ; preds = %bb3
  %__LookupPointer_set-shadow-casting-from-npc-flashlight_fptr = call ptr @llvm.dcvm.lookup(i64 -3854484060414110792)
  %__CallFf_82___call = tail call i64 %__LookupPointer_set-shadow-casting-from-npc-flashlight_fptr(i64 %r1.sroa.0.0.insert.insert345, i64 0), !callees !24
  br label %bb6

bb6:                                              ; preds = %bb5, %bb4
  %__LookupPointer_set-gas-mask_fptr = call ptr @llvm.dcvm.lookup(i64 -5971891483814290574)
  %__CallFf_88___call = tail call i64 %__LookupPointer_set-gas-mask_fptr(i64 %arg_0, i64 %r1.sroa.0.0.insert.insert345), !callees !25
  %__LookupPointer_set-visible_fptr = call ptr @llvm.dcvm.lookup(i64 -7746349783173232884)
  %__CallFf_94___call = tail call i64 %__LookupPointer_set-visible_fptr(i64 %r1.sroa.0.0.insert.insert345, i64 0), !callees !26
  %"__LookupPointer_is-player?_fptr507" = call ptr @llvm.dcvm.lookup(i64 6532480022340268902)
  %__CallFf_98___call = tail call i64 %"__LookupPointer_is-player?_fptr507"(i64 %arg_0), !callees !21
  %1 = trunc i64 %__CallFf_98___call to i1
  br i1 %1, label %bb9, label %bb7

bb7:                                              ; preds = %bb6
  %__LookupPointer_wait-one-frame_fptr508 = call ptr @llvm.dcvm.lookup(i64 -3047912596559836939)
  %__CallFf_102___call = tail call i64 %__LookupPointer_wait-one-frame_fptr508(), !callees !3
  br label %bb9

bb9:                                              ; preds = %bb6, %bb7
  %__LookupPointer_attach_fptr = call ptr @llvm.dcvm.lookup(i64 -5676660224586178492)
  %__CallFf_114___call = tail call i64 %__LookupPointer_attach_fptr(i64 %r1.sroa.0.0.insert.insert345, i64 %arg_0, i64 8667864169316382944, float 0.000000e+00), !callees !27
  %__CallFf_123___call = tail call i64 @new-boxed-value(i64 6, float 0x3FD3333340000000)
  %__LookupPointer_send-event_fptr = call ptr @llvm.dcvm.lookup(i64 4789555171397753330)
  %__CallFf_127___call = tail call i64 %__LookupPointer_send-event_fptr(i64 6817768622307059015, i64 %arg_0, i64 %__CallFf_123___call), !callees !4
  %r3.sroa.30.0.extract.shift472 = and i64 ptrtoint (ptr @animate_ to i64), -256
  %__IAddImm_132__op = getelementptr i8, ptr %0, i64 16
  %__LoadU64_133___load = load i64, ptr %__IAddImm_132__op, align 4
  %"__LookupPointer_#%alloc-array_fptr" = call ptr @llvm.dcvm.lookup(i64 -7745656483000545638)
  %__CallFf_143___call = tail call i64 %"__LookupPointer_#%alloc-array_fptr"(i64 8534565298598421705, i64 16, i64 0, i64 1), !callees !28
  %r3.sroa.0.0.insert.ext = zext i8 ptrtoint (ptr @animate_ to i8) to i64
  %r3.sroa.0.0.insert.insert = or disjoint i64 %r3.sroa.30.0.extract.shift472, %r3.sroa.0.0.insert.ext
  %2 = inttoptr i64 %r3.sroa.0.0.insert.insert to ptr
  %__CallFf_149___call = tail call i64 %2(i64 %r1.sroa.0.0.insert.insert345, i64 %__LoadU64_133___load, i64 %__CallFf_143___call)
  %r3.sroa.30.0.extract.shift474 = and i64 ptrtoint (ptr @gesture_ to i64), -256
  %r3.sroa.0.0.insert.ext435 = zext i8 ptrtoint (ptr @gesture_ to i8) to i64
  %r3.sroa.0.0.insert.insert437 = or disjoint i64 %r3.sroa.30.0.extract.shift474, %r3.sroa.0.0.insert.ext435
  %3 = inttoptr i64 %r3.sroa.0.0.insert.insert437 to ptr
  br label %bb10

bb10:                                             ; preds = %bb11, %bb9
  %__LoadU64_154___load = load i64, ptr %0, align 4
  %__LookupPointer_alloc-gesture-play-params_fptr = call ptr @llvm.dcvm.lookup(i64 -2962780980939953026)
  %__CallFf_156___call = tail call i64 %__LookupPointer_alloc-gesture-play-params_fptr(), !callees !29
  %__IAddImm_158__op = add i64 %__CallFf_156___call, 33
  %4 = inttoptr i64 %__IAddImm_158__op to ptr
  store i8 1, ptr %4, align 1
  %__IAddImm_162__op = add i64 %__CallFf_156___call, 36
  %5 = inttoptr i64 %__IAddImm_162__op to ptr
  store i32 19, ptr %5, align 4
  %__IAddImm_166__op = add i64 %__CallFf_156___call, 32
  %6 = inttoptr i64 %__IAddImm_166__op to ptr
  store i8 1, ptr %6, align 1
  %__IAddImm_170__op = add i64 %__CallFf_156___call, 34
  %7 = inttoptr i64 %__IAddImm_170__op to ptr
  store i8 1, ptr %7, align 1
  %__CallFf_178___call = tail call i64 %3(i64 %arg_0, i64 %__LoadU64_154___load, i64 %__CallFf_156___call)
  %__IEqual_180__op.not = icmp eq i64 %__CallFf_178___call, 0
  br i1 %__IEqual_180__op.not, label %bb12, label %bb11

bb11:                                             ; preds = %bb10
  %__LookupPointer_wait-one-frame_fptr509 = call ptr @llvm.dcvm.lookup(i64 -3047912596559836939)
  %__CallFf_184___call = tail call i64 %__LookupPointer_wait-one-frame_fptr509(), !callees !3
  br label %bb10

bb12:                                             ; preds = %bb10
  %__IAddImm_190__op = getelementptr i8, ptr %0, i64 24
  %__LoadU64_191___load = load i64, ptr %__IAddImm_190__op, align 4
  %"__LookupPointer_#%alloc-array_fptr510" = call ptr @llvm.dcvm.lookup(i64 -7745656483000545638)
  %__CallFf_201___call = tail call i64 %"__LookupPointer_#%alloc-array_fptr510"(i64 8534565298598421705, i64 16, i64 0, i64 1), !callees !28
  %__CallFf_207___call = tail call i64 %2(i64 %r1.sroa.0.0.insert.insert345, i64 %__LoadU64_191___load, i64 %__CallFf_201___call)
  ret i64 %__CallFf_207___call
}

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !30 i64 @lookup-symbol(i64, i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !31 i64 @get-gas-mask-setting-id(i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !32 i64 @wait-until-in-valid-gas-mask-state(i64, i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !33 i64 @spawn-object(i64, i64, i64, i64, i64, i64, i64, i64, i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !34 i64 @set-high-contrast-mode-type(i64, i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !35 i64 @"is-player?"(i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !36 i64 @set-instance-flag-hero(i64, i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !37 i64 @set-show-in-flashlight-flag(i64, i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !38 i64 @set-shadow-casting-from-npc-flashlight(i64, i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !39 i64 @set-gas-mask(i64, i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !40 i64 @set-visible(i64, i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !41 i64 @attach(i64, i64, i64, float) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !42 i64 @animate_(i64, i64, i64) #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !43 i64 @"#%alloc-array"(i64, i64, i64, i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !44 i64 @gesture_(i64, i64, i64) #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !45 i64 @alloc-gesture-play-params() local_unnamed_addr #1

define i64 @internal-put-on-gas-mask-immediately.2(i64 %arg_0, i64 %arg_1) local_unnamed_addr {
bb0:
  %__LookupPointer_get-gas-mask-setting-id_fptr = call ptr @llvm.dcvm.lookup(i64 -6573555005595409285)
  %__CallFf_6___call = tail call i64 %__LookupPointer_get-gas-mask-setting-id_fptr(i64 %arg_0), !callees !16
  %__LookupPointer_lookup-symbol_fptr = call ptr @llvm.dcvm.lookup(i64 -7658033447216601854)
  %__CallFf_10___call = tail call i64 %__LookupPointer_lookup-symbol_fptr(i64 %__CallFf_6___call, i64 3455792245619945009), !callees !17
  %__IEqual_13__op = icmp eq i64 %arg_1, 0
  %extract.t368 = trunc i64 %arg_1 to i32
  %extract370 = lshr i64 %arg_1, 32
  %extract.t371 = trunc nuw i64 %extract370 to i32
  br i1 %__IEqual_13__op, label %bb1, label %bb3

bb1:                                              ; preds = %bb0
  %0 = inttoptr i64 %__CallFf_10___call to ptr
  %__IAddImm_18__op = getelementptr i8, ptr %0, i64 40
  %__LoadU64_19___load = load i64, ptr %__IAddImm_18__op, align 4
  %__LookupPointer_spawn-object_fptr = call ptr @llvm.dcvm.lookup(i64 7097052834867614476)
  %__CallFf_37___call = tail call i64 %__LookupPointer_spawn-object_fptr(i64 %__LoadU64_19___load, i64 267125575217391726, i64 0, i64 0, i64 1, i64 0, i64 0, i64 0, i64 0), !callees !19
  %extract.t = trunc i64 %__CallFf_37___call to i32
  %extract = lshr i64 %__CallFf_37___call, 32
  %extract.t369 = trunc nuw i64 %extract to i32
  br label %bb3

bb3:                                              ; preds = %bb0, %bb1
  %arg_1.sink367.off0 = phi i32 [ %extract.t, %bb1 ], [ %extract.t368, %bb0 ]
  %arg_1.sink367.off32 = phi i32 [ %extract.t369, %bb1 ], [ %extract.t371, %bb0 ]
  %r1.sroa.10.0.insert.ext296 = zext i32 %arg_1.sink367.off32 to i64
  %r1.sroa.10.0.insert.shift297 = shl nuw i64 %r1.sroa.10.0.insert.ext296, 32
  %r1.sroa.0.0.insert.ext274 = zext i32 %arg_1.sink367.off0 to i64
  %r1.sroa.0.0.insert.insert276 = or disjoint i64 %r1.sroa.10.0.insert.shift297, %r1.sroa.0.0.insert.ext274
  %__LookupPointer_set-high-contrast-mode-type_fptr = call ptr @llvm.dcvm.lookup(i64 4766017279285775524)
  %__CallFf_47___call = tail call i64 %__LookupPointer_set-high-contrast-mode-type_fptr(i64 %r1.sroa.0.0.insert.insert276, i64 6), !callees !20
  %__LookupPointer_set-gas-mask_fptr = call ptr @llvm.dcvm.lookup(i64 -5971891483814290574)
  %__CallFf_53___call = tail call i64 %__LookupPointer_set-gas-mask_fptr(i64 %arg_0, i64 %r1.sroa.0.0.insert.insert276), !callees !25
  %"__LookupPointer_is-player?_fptr" = call ptr @llvm.dcvm.lookup(i64 6532480022340268902)
  %__CallFf_57___call = tail call i64 %"__LookupPointer_is-player?_fptr"(i64 %arg_0), !callees !21
  %__BranchIfNot_58___bool.not = icmp eq i64 %__CallFf_57___call, 0
  br i1 %__BranchIfNot_58___bool.not, label %bb6, label %bb4

bb4:                                              ; preds = %bb3
  %__CallFf_67___call = tail call i64 @new-boxed-value(i64 6, float 0.000000e+00)
  %__CallFf_71___call = tail call i64 (i64, i64, ...) @send-event(i64 6817768622307059015, i64 %arg_0, i64 %__CallFf_67___call)
  %__LookupPointer_set-instance-flag-hero_fptr = call ptr @llvm.dcvm.lookup(i64 -429226074377614887)
  %__CallFf_77___call = tail call i64 %__LookupPointer_set-instance-flag-hero_fptr(i64 %r1.sroa.0.0.insert.insert276, i64 1), !callees !22
  br label %bb6

bb6:                                              ; preds = %bb3, %bb4
  %__CallFf_85___call = tail call i64 (i64, i64, ...) @send-event(i64 1827492438953869695, i64 %arg_0)
  %"__LookupPointer_is-player?_fptr372" = call ptr @llvm.dcvm.lookup(i64 6532480022340268902)
  %__CallFf_89___call = tail call i64 %"__LookupPointer_is-player?_fptr372"(i64 %arg_0), !callees !21
  %__BranchIfNot_90___bool.not = icmp eq i64 %__CallFf_89___call, 0
  br i1 %__BranchIfNot_90___bool.not, label %bb8, label %bb7

bb7:                                              ; preds = %bb6
  %__LookupPointer_set-show-in-flashlight-flag_fptr = call ptr @llvm.dcvm.lookup(i64 -7252477440030364545)
  %__CallFf_96___call = tail call i64 %__LookupPointer_set-show-in-flashlight-flag_fptr(i64 %r1.sroa.0.0.insert.insert276, i64 0), !callees !23
  br label %bb9

bb8:                                              ; preds = %bb6
  %__LookupPointer_set-shadow-casting-from-npc-flashlight_fptr = call ptr @llvm.dcvm.lookup(i64 -3854484060414110792)
  %__CallFf_103___call = tail call i64 %__LookupPointer_set-shadow-casting-from-npc-flashlight_fptr(i64 %r1.sroa.0.0.insert.insert276, i64 0), !callees !24
  br label %bb9

bb9:                                              ; preds = %bb8, %bb7
  %r3.sroa.19.0.extract.shift355 = and i64 ptrtoint (ptr @animate_ to i64), -256
  %1 = inttoptr i64 %__CallFf_10___call to ptr
  %__IAddImm_108__op = getelementptr i8, ptr %1, i64 24
  %__LoadU64_109___load = load i64, ptr %__IAddImm_108__op, align 4
  %"__LookupPointer_#%alloc-array_fptr" = call ptr @llvm.dcvm.lookup(i64 -7745656483000545638)
  %__CallFf_119___call = tail call i64 %"__LookupPointer_#%alloc-array_fptr"(i64 8534565298598421705, i64 16, i64 1, i64 1), !callees !28
  %2 = inttoptr i64 %__CallFf_119___call to ptr
  %__LoadPointer_121___load = load ptr, ptr %2, align 8
  %__LoadPointer_121___trunc = trunc i64 %arg_0 to i8
  store i8 %__LoadPointer_121___trunc, ptr %__LoadPointer_121___load, align 1
  %3 = inttoptr i64 %arg_0 to ptr
  %__LoadPointer_125___load = load ptr, ptr %3, align 8
  store i8 %__LoadPointer_121___trunc, ptr %__LoadPointer_125___load, align 1
  store i32 31, ptr %3, align 8
  %__LoadPointer_129___load = load ptr, ptr %2, align 8
  store i8 %__LoadPointer_121___trunc, ptr %__LoadPointer_129___load, align 1
  %__LoadPointer_133___load = load ptr, ptr %3, align 8
  store i8 %__LoadPointer_121___trunc, ptr %__LoadPointer_133___load, align 1
  %__IAddImm_134__op = add i64 %arg_0, 8
  %4 = inttoptr i64 %__IAddImm_134__op to ptr
  store i32 0, ptr %4, align 4
  %__StorePointer_136___addr.sroa_idx = getelementptr inbounds nuw i8, ptr %4, i64 4
  store i32 0, ptr %__StorePointer_136___addr.sroa_idx, align 4
  %__LoadPointer_138___load = load ptr, ptr %2, align 8
  store i8 %__LoadPointer_121___trunc, ptr %__LoadPointer_138___load, align 1
  %__LoadPointer_142___load = load ptr, ptr %3, align 8
  store i8 %__LoadPointer_121___trunc, ptr %__LoadPointer_142___load, align 1
  store i8 1, ptr %4, align 4
  %r3.sroa.0.0.insert.ext = zext i8 ptrtoint (ptr @animate_ to i8) to i64
  %r3.sroa.0.0.insert.insert = or disjoint i64 %r3.sroa.19.0.extract.shift355, %r3.sroa.0.0.insert.ext
  %5 = inttoptr i64 %r3.sroa.0.0.insert.insert to ptr
  %__CallFf_151___call = tail call i64 %5(i64 %r1.sroa.0.0.insert.insert276, i64 %__LoadU64_109___load, i64 %__CallFf_119___call)
  ret i64 %__CallFf_151___call
}

; Function Attrs: mustprogress nosync nounwind willreturn
define i64 @get-equipped-gas-mask(i64 %arg_0) local_unnamed_addr #1 {
bb0:
  %__LookupPointer_get-gas-mask_fptr = call ptr @llvm.dcvm.lookup(i64 -5076200561558624898)
  %__CallFf_4___call = tail call i64 %__LookupPointer_get-gas-mask_fptr(i64 %arg_0), !callees !7
  ret i64 %__CallFf_4___call
}

; Function Attrs: nosync nounwind
define i64 @wait-until-in-valid-gas-mask-state.3(i64 %arg_0, i64 %arg_1) local_unnamed_addr #0 {
bb0:
  %"__LookupPointer_is-player?_fptr" = call ptr @llvm.dcvm.lookup(i64 6532480022340268902)
  %__CallFf_5___call = tail call i64 %"__LookupPointer_is-player?_fptr"(i64 %arg_0), !callees !21
  %__BranchIfNot_6___bool.not = icmp eq i64 %__CallFf_5___call, 0
  br i1 %__BranchIfNot_6___bool.not, label %bb15, label %bb1

bb1:                                              ; preds = %bb0, %bb3
  %"__LookupPointer_player-in-state?_fptr" = call ptr @llvm.dcvm.lookup(i64 -9003251364961024783)
  %__CallFf_10___call = tail call i64 %"__LookupPointer_player-in-state?_fptr"(i64 6740565536053766520), !callees !46
  %0 = trunc i64 %__CallFf_10___call to i1
  br i1 %0, label %bb3, label %bb2

bb2:                                              ; preds = %bb1
  %"__LookupPointer_player-in-state?_fptr262" = call ptr @llvm.dcvm.lookup(i64 -9003251364961024783)
  %__CallFf_16___call = tail call i64 %"__LookupPointer_player-in-state?_fptr262"(i64 2237451099064635199), !callees !46
  %1 = trunc i64 %__CallFf_16___call to i1
  br i1 %1, label %bb3, label %bb4

bb3:                                              ; preds = %bb2, %bb1
  %__LookupPointer_wait-one-frame_fptr = call ptr @llvm.dcvm.lookup(i64 -3047912596559836939)
  %__CallFf_21___call = tail call i64 %__LookupPointer_wait-one-frame_fptr(), !callees !3
  br label %bb1

bb4:                                              ; preds = %bb2
  %__BranchIfNot_24___bool.not = icmp eq i64 %arg_1, 0
  br i1 %__BranchIfNot_24___bool.not, label %bb9, label %bb5

bb5:                                              ; preds = %bb4
  %__LookupPointer_player-force-crouch_fptr = call ptr @llvm.dcvm.lookup(i64 -4831287436211302109)
  %__CallFf_26___call = tail call i64 %__LookupPointer_player-force-crouch_fptr(), !callees !47
  %"__LookupPointer_player-is-crouched?_fptr" = call ptr @llvm.dcvm.lookup(i64 1646463116072042322)
  %__CallFf_28___call244 = tail call i64 %"__LookupPointer_player-is-crouched?_fptr"(), !callees !48
  %2 = trunc i64 %__CallFf_28___call244 to i1
  br i1 %2, label %bb12.preheader, label %bb7

bb12.preheader:                                   ; preds = %bb7, %bb11, %bb5, %bb9
  %"__LookupPointer_player-in-state?_fptr263" = call ptr @llvm.dcvm.lookup(i64 -9003251364961024783)
  %__CallFf_50___call251 = tail call i64 %"__LookupPointer_player-in-state?_fptr263"(i64 4007104907368024212), !callees !46
  %3 = trunc i64 %__CallFf_50___call251 to i1
  br i1 %3, label %bb14, label %bb13

bb7:                                              ; preds = %bb5, %bb7
  %__LookupPointer_wait-one-frame_fptr264 = call ptr @llvm.dcvm.lookup(i64 -3047912596559836939)
  %__CallFf_32___call = tail call i64 %__LookupPointer_wait-one-frame_fptr264(), !callees !3
  %"__LookupPointer_player-is-crouched?_fptr265" = call ptr @llvm.dcvm.lookup(i64 1646463116072042322)
  %__CallFf_28___call = tail call i64 %"__LookupPointer_player-is-crouched?_fptr265"(), !callees !48
  %4 = trunc i64 %__CallFf_28___call to i1
  br i1 %4, label %bb12.preheader, label %bb7

bb9:                                              ; preds = %bb4
  %__LookupPointer_player-force-stand_fptr = call ptr @llvm.dcvm.lookup(i64 8594926419455150747)
  %__CallFf_36___call = tail call i64 %__LookupPointer_player-force-stand_fptr(), !callees !49
  %"__LookupPointer_player-is-crouched?_fptr266" = call ptr @llvm.dcvm.lookup(i64 1646463116072042322)
  %__CallFf_39___call247 = tail call i64 %"__LookupPointer_player-is-crouched?_fptr266"(), !callees !48
  %"__LookupPointer_!_fptr" = call ptr @llvm.dcvm.lookup(i64 -5808627241898115124)
  %__Call_41___call248 = tail call i64 %"__LookupPointer_!_fptr"(i64 %__CallFf_39___call247), !callees !50
  %5 = trunc i64 %__Call_41___call248 to i1
  br i1 %5, label %bb12.preheader, label %bb11

bb11:                                             ; preds = %bb9, %bb11
  %__LookupPointer_wait-one-frame_fptr267 = call ptr @llvm.dcvm.lookup(i64 -3047912596559836939)
  %__CallFf_45___call = tail call i64 %__LookupPointer_wait-one-frame_fptr267(), !callees !3
  %"__LookupPointer_player-is-crouched?_fptr268" = call ptr @llvm.dcvm.lookup(i64 1646463116072042322)
  %__CallFf_39___call = tail call i64 %"__LookupPointer_player-is-crouched?_fptr268"(), !callees !48
  %"__LookupPointer_!_fptr269" = call ptr @llvm.dcvm.lookup(i64 -5808627241898115124)
  %__Call_41___call = tail call i64 %"__LookupPointer_!_fptr269"(i64 %__CallFf_39___call), !callees !50
  %6 = trunc i64 %__Call_41___call to i1
  br i1 %6, label %bb12.preheader, label %bb11

bb13:                                             ; preds = %bb12.preheader, %bb13
  %__LookupPointer_wait-one-frame_fptr270 = call ptr @llvm.dcvm.lookup(i64 -3047912596559836939)
  %__CallFf_54___call = tail call i64 %__LookupPointer_wait-one-frame_fptr270(), !callees !3
  %"__LookupPointer_player-in-state?_fptr271" = call ptr @llvm.dcvm.lookup(i64 -9003251364961024783)
  %__CallFf_50___call = tail call i64 %"__LookupPointer_player-in-state?_fptr271"(i64 4007104907368024212), !callees !46
  %7 = trunc i64 %__CallFf_50___call to i1
  br i1 %7, label %bb14, label %bb13

bb14:                                             ; preds = %bb13, %bb12.preheader
  %__LookupPointer_wait-holster-player-weapon_fptr = call ptr @llvm.dcvm.lookup(i64 -3092934816467423265)
  %__CallFf_61___call = tail call i64 %__LookupPointer_wait-holster-player-weapon_fptr(i64 0, float 1.000000e+00), !callees !51
  %__LookupPointer_holster-player-weapon-instantly_fptr = call ptr @llvm.dcvm.lookup(i64 -626103520744544694)
  %__CallFf_67___call = tail call i64 %__LookupPointer_holster-player-weapon-instantly_fptr(float 0x3FC99999A0000000, i64 0), !callees !52
  %r2.sroa.0.0.extract.trunc179 = trunc i64 %__CallFf_67___call to i8
  %r2.sroa.39.0.extract.shift235 = lshr i64 %__CallFf_67___call, 8
  %r2.sroa.39.0.extract.trunc236 = trunc nuw i64 %r2.sroa.39.0.extract.shift235 to i56
  br label %bb21

bb15:                                             ; preds = %bb0
  %__LookupPointer_wait-npc-holster-weapon_fptr = call ptr @llvm.dcvm.lookup(i64 5808793362033684947)
  %__CallFf_80___call = tail call i64 %__LookupPointer_wait-npc-holster-weapon_fptr(i64 %arg_0, i64 0, i64 1, float -1.000000e+00, i64 0), !callees !53
  %"__LookupPointer_npc-has-weapon-in-hand?_fptr" = call ptr @llvm.dcvm.lookup(i64 7683024912818272899)
  %__CallFf_84___call254 = tail call i64 %"__LookupPointer_npc-has-weapon-in-hand?_fptr"(i64 %arg_0), !callees !54
  %__BranchIfNot_85___bool.not256 = icmp eq i64 %__CallFf_84___call254, 0
  br i1 %__BranchIfNot_85___bool.not256, label %bb18, label %bb17

bb17:                                             ; preds = %bb15, %bb17
  %__LookupPointer_wait-one-frame_fptr272 = call ptr @llvm.dcvm.lookup(i64 -3047912596559836939)
  %__CallFf_87___call = tail call i64 %__LookupPointer_wait-one-frame_fptr272(), !callees !3
  %"__LookupPointer_npc-has-weapon-in-hand?_fptr273" = call ptr @llvm.dcvm.lookup(i64 7683024912818272899)
  %__CallFf_84___call = tail call i64 %"__LookupPointer_npc-has-weapon-in-hand?_fptr273"(i64 %arg_0), !callees !54
  %__BranchIfNot_85___bool.not = icmp eq i64 %__CallFf_84___call, 0
  br i1 %__BranchIfNot_85___bool.not, label %bb18, label %bb17

bb18:                                             ; preds = %bb17, %bb15
  %__BranchIfNot_90___bool.not = icmp eq i64 %arg_1, 0
  br i1 %__BranchIfNot_90___bool.not, label %bb21, label %bb19

bb19:                                             ; preds = %bb18
  %__LookupPointer_wait-npc-set-demeanor_fptr = call ptr @llvm.dcvm.lookup(i64 8573502661920361253)
  %__CallFf_96___call = tail call i64 %__LookupPointer_wait-npc-set-demeanor_fptr(i64 %arg_0, i64 3), !callees !55
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

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !56 i64 @"player-in-state?"(i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !57 i64 @player-force-crouch() local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !58 i64 @"player-is-crouched?"() local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !59 i64 @player-force-stand() local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !60 i64 @"!"(i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !61 i64 @wait-holster-player-weapon(i64, float) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !62 i64 @holster-player-weapon-instantly(float, i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !63 i64 @wait-npc-holster-weapon(i64, i64, i64, float, i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !64 i64 @"npc-has-weapon-in-hand?"(i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !65 i64 @wait-npc-set-demeanor(i64, i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
define i64 @"npc-gas-mask-narrative-mode/f"(i64 %arg_0) local_unnamed_addr #1 {
bb0:
  %__CallFf_6___call = tail call i64 @send-event(i64 8119835013146602502, i64 %arg_0)
  %"__LookupPointer_suspend-idle-gestures/f_fptr" = call ptr @llvm.dcvm.lookup(i64 -5356689657310757786)
  %__Call_10___call = tail call i64 %"__LookupPointer_suspend-idle-gestures/f_fptr"(i64 %arg_0), !callees !66
  %__LookupPointer_npc-disable-dialog-look_fptr = call ptr @llvm.dcvm.lookup(i64 7610916627332680768)
  %__CallFf_16___call = tail call i64 %__LookupPointer_npc-disable-dialog-look_fptr(i64 %arg_0, float 0.000000e+00), !callees !67
  ret i64 %__CallFf_16___call
}

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !68 i64 @"suspend-idle-gestures/f"(i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !69 i64 @npc-disable-dialog-look(i64, float) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
define i64 @internal-put-off-gas-mask-immediately(i64 %arg_0, i64 %arg_1) local_unnamed_addr #1 !dcvm.sid_distance !70 {
bb0:
  %__LookupPointer_get-gas-mask_fptr = call ptr @llvm.dcvm.lookup(i64 -5076200561558624898)
  %__CallFf_5___call = tail call i64 %__LookupPointer_get-gas-mask_fptr(i64 %arg_0), !callees !7
  %__CallFf_11___call = tail call i64 @send-event(i64 -3264453205741318791, i64 %arg_0)
  %__BranchIfNot_13___bool.not = icmp eq i64 %arg_1, 0
  br i1 %__BranchIfNot_13___bool.not, label %bb3, label %bb1

bb1:                                              ; preds = %bb0
  %__LookupPointer_kill-entity_fptr = call ptr @llvm.dcvm.lookup(i64 -804116741684351865)
  %__CallFf_17___call = tail call i64 %__LookupPointer_kill-entity_fptr(i64 %__CallFf_5___call), !callees !71
  br label %bb3

bb3:                                              ; preds = %bb0, %bb1
  %r3.0 = phi i64 [ %__CallFf_17___call, %bb1 ], [ 0, %bb0 ]
  ret i64 %r3.0
}

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !72 i64 @kill-entity(i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
define i64 @spawn-gas-mask(i64 %arg_0) local_unnamed_addr #1 {
bb0:
  %__LookupPointer_get-gas-mask-setting-id_fptr = call ptr @llvm.dcvm.lookup(i64 -6573555005595409285)
  %__CallFf_5___call = tail call i64 %__LookupPointer_get-gas-mask-setting-id_fptr(i64 %arg_0), !callees !16
  %__LookupPointer_lookup-symbol_fptr = call ptr @llvm.dcvm.lookup(i64 -7658033447216601854)
  %__CallFf_9___call = tail call i64 %__LookupPointer_lookup-symbol_fptr(i64 %__CallFf_5___call, i64 3455792245619945009), !callees !17
  %0 = inttoptr i64 %__CallFf_9___call to ptr
  %__IAddImm_13__op = getelementptr i8, ptr %0, i64 40
  %__LoadU64_14___load = load i64, ptr %__IAddImm_13__op, align 4
  %__LookupPointer_spawn-object_fptr = call ptr @llvm.dcvm.lookup(i64 7097052834867614476)
  %__CallFf_32___call = tail call i64 %__LookupPointer_spawn-object_fptr(i64 %__LoadU64_14___load, i64 267125575217391726, i64 0, i64 0, i64 1, i64 0, i64 0, i64 0, i64 0), !callees !19
  %__LookupPointer_set-high-contrast-mode-type_fptr = call ptr @llvm.dcvm.lookup(i64 4766017279285775524)
  %__CallFf_38___call = tail call i64 %__LookupPointer_set-high-contrast-mode-type_fptr(i64 %__CallFf_32___call, i64 6), !callees !20
  ret i64 %__CallFf_32___call
}

; Function Attrs: mustprogress nosync nounwind willreturn
define i64 @"player-gas-mask-narrative-mode/f"(i64 %arg_0, i64 %arg_1, i64 %arg_2) local_unnamed_addr #1 {
bb0:
  %__LookupPointer_player-disable-dialog-look-gestures_fptr = call ptr @llvm.dcvm.lookup(i64 7238593933813171071)
  %__CallFf_6___call = tail call i64 %__LookupPointer_player-disable-dialog-look-gestures_fptr(float 0.000000e+00), !callees !73
  %"__LookupPointer_player-disable-squeeze-through/f_fptr" = call ptr @llvm.dcvm.lookup(i64 8165897991486741957)
  %__CallFf_8___call = tail call i64 %"__LookupPointer_player-disable-squeeze-through/f_fptr"(), !callees !74
  %"__LookupPointer_player-set-narrative-mode/f_fptr" = call ptr @llvm.dcvm.lookup(i64 -2826777519175913837)
  %__Call_16___call = tail call i64 %"__LookupPointer_player-set-narrative-mode/f_fptr"(i64 %arg_0, i64 %arg_1, i64 %arg_2), !callees !75
  %"__LookupPointer_suspend-idle-gestures/f_fptr" = call ptr @llvm.dcvm.lookup(i64 -5356689657310757786)
  %__Call_20___call = tail call i64 %"__LookupPointer_suspend-idle-gestures/f_fptr"(i64 5008278420455340480), !callees !66
  %__LookupPointer_joypad-disable-commands_fptr = call ptr @llvm.dcvm.lookup(i64 7121253686161376880)
  %__CallFf_24___call = tail call i64 %__LookupPointer_joypad-disable-commands_fptr(i64 -1699981380503476753), !callees !76
  ret i64 %__CallFf_24___call
}

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !77 i64 @player-disable-dialog-look-gestures(float) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !78 i64 @"player-disable-squeeze-through/f"() local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !79 i64 @"player-set-narrative-mode/f"(i64, i64, i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
declare !dcvm.sid_distance !80 i64 @joypad-disable-commands(i64) local_unnamed_addr #1

; Function Attrs: mustprogress nosync nounwind willreturn
define i64 @internal-put-off-gas-mask(i64 %arg_0, i64 %arg_1) local_unnamed_addr #1 !dcvm.sid_distance !81 {
bb0:
  %__LookupPointer_wait-one-frame_fptr = call ptr @llvm.dcvm.lookup(i64 -3047912596559836939)
  %__CallFf_3___call = tail call i64 %__LookupPointer_wait-one-frame_fptr(), !callees !3
  %__LookupPointer_get-gas-mask-setting-id_fptr = call ptr @llvm.dcvm.lookup(i64 -6573555005595409285)
  %__CallFf_8___call = tail call i64 %__LookupPointer_get-gas-mask-setting-id_fptr(i64 %arg_0), !callees !16
  %__LookupPointer_lookup-symbol_fptr = call ptr @llvm.dcvm.lookup(i64 -7658033447216601854)
  %__CallFf_12___call = tail call i64 %__LookupPointer_lookup-symbol_fptr(i64 %__CallFf_8___call, i64 3455792245619945009), !callees !17
  %__LookupPointer_get-gas-mask_fptr = call ptr @llvm.dcvm.lookup(i64 -5076200561558624898)
  %__CallFf_16___call = tail call i64 %__LookupPointer_get-gas-mask_fptr(i64 %arg_0), !callees !7
  %0 = inttoptr i64 %__CallFf_12___call to ptr
  %__IAddImm_21__op = getelementptr i8, ptr %0, i64 57
  %__LoadU8_22___load = load i8, ptr %__IAddImm_21__op, align 1
  %__LoadU8_22___ext = zext i8 %__LoadU8_22___load to i64
  %__LookupPointer_wait-until-in-valid-gas-mask-state_fptr = call ptr @llvm.dcvm.lookup(i64 8203816822361639083)
  %__Call_25___call = tail call i64 %__LookupPointer_wait-until-in-valid-gas-mask-state_fptr(i64 %arg_0, i64 %__LoadU8_22___ext), !callees !18
  %__IAddImm_30__op = getelementptr i8, ptr %0, i64 32
  %__LoadU64_31___load = load i64, ptr %__IAddImm_30__op, align 4
  %"__LookupPointer_#%alloc-array_fptr" = call ptr @llvm.dcvm.lookup(i64 -7745656483000545638)
  %__CallFf_41___call = tail call i64 %"__LookupPointer_#%alloc-array_fptr"(i64 8534565298598421705, i64 16, i64 0, i64 1), !callees !28
  %__LookupPointer_animate__fptr = call ptr @llvm.dcvm.lookup(i64 -9179819928081802467)
  %__CallFf_47___call = tail call i64 %__LookupPointer_animate__fptr(i64 %__CallFf_16___call, i64 %__LoadU64_31___load, i64 %__CallFf_41___call), !callees !82
  %r4.sroa.13.0.extract.shift53 = and i64 ptrtoint (ptr @gesture_ to i64), -256
  %__IAddImm_52__op = getelementptr i8, ptr %0, i64 8
  %r4.sroa.0.0.insert.ext = zext i8 ptrtoint (ptr @gesture_ to i8) to i64
  %r4.sroa.0.0.insert.insert = or disjoint i64 %r4.sroa.13.0.extract.shift53, %r4.sroa.0.0.insert.ext
  %1 = inttoptr i64 %r4.sroa.0.0.insert.insert to ptr
  br label %bb1

bb1:                                              ; preds = %bb2, %bb0
  %__LoadU64_53___load = load i64, ptr %__IAddImm_52__op, align 4
  %__LookupPointer_alloc-gesture-play-params_fptr = call ptr @llvm.dcvm.lookup(i64 -2962780980939953026)
  %__CallFf_55___call = tail call i64 %__LookupPointer_alloc-gesture-play-params_fptr(), !callees !29
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
  %__CallFf_77___call = tail call i64 %1(i64 %arg_0, i64 %__LoadU64_53___load, i64 %__CallFf_55___call) #3
  %__IEqual_79__op.not = icmp eq i64 %__CallFf_77___call, 0
  br i1 %__IEqual_79__op.not, label %bb3, label %bb2

bb2:                                              ; preds = %bb1
  %__LookupPointer_wait-one-frame_fptr69 = call ptr @llvm.dcvm.lookup(i64 -3047912596559836939)
  %__CallFf_83___call = tail call i64 %__LookupPointer_wait-one-frame_fptr69(), !callees !3
  br label %bb1

bb3:                                              ; preds = %bb1
  %__BranchIfNot_86___bool.not = icmp eq i64 %arg_1, 0
  br i1 %__BranchIfNot_86___bool.not, label %bb6, label %bb4

bb4:                                              ; preds = %bb3
  %__LookupPointer_kill-entity_fptr = call ptr @llvm.dcvm.lookup(i64 -804116741684351865)
  %__CallFf_90___call = tail call i64 %__LookupPointer_kill-entity_fptr(i64 %__CallFf_16___call), !callees !71
  br label %bb6

bb6:                                              ; preds = %bb3, %bb4
  %storemerge = phi i64 [ %__CallFf_90___call, %bb4 ], [ 0, %bb3 ]
  ret i64 %storemerge
}

; Function Attrs: nosync nounwind
define i64 @wait-remove-gas-mask(i64 %arg_0, i64 %arg_1, i64 %arg_2) local_unnamed_addr #0 {
bb0:
  %__LookupPointer_new-boxed-value_fptr = call ptr @llvm.dcvm.lookup(i64 3684810824690217768)
  %__CallFf_11___call = tail call i64 %__LookupPointer_new-boxed-value_fptr(i64 1, i64 0), !callees !0
  %__LookupPointer_fact-set_fptr = call ptr @llvm.dcvm.lookup(i64 -2210386714833194546)
  %__CallFf_15___call = tail call i64 %__LookupPointer_fact-set_fptr(i64 %arg_0, i64 7671589888336862994, i64 %__CallFf_11___call), !callees !1
  %"__LookupPointer_is-script-running?_fptr" = call ptr @llvm.dcvm.lookup(i64 6481843155139167450)
  %__Call_19___call48 = tail call i64 %"__LookupPointer_is-script-running?_fptr"(i64 6150028017864311825), !callees !2
  %0 = trunc i64 %__Call_19___call48 to i1
  br i1 %0, label %bb3, label %bb2

bb2:                                              ; preds = %bb0, %bb2
  %__LookupPointer_wait-one-frame_fptr = call ptr @llvm.dcvm.lookup(i64 -3047912596559836939)
  %__CallFf_23___call = tail call i64 %__LookupPointer_wait-one-frame_fptr(), !callees !3
  %"__LookupPointer_is-script-running?_fptr51" = call ptr @llvm.dcvm.lookup(i64 6481843155139167450)
  %__Call_19___call = tail call i64 %"__LookupPointer_is-script-running?_fptr51"(i64 6150028017864311825), !callees !2
  %1 = trunc i64 %__Call_19___call to i1
  br i1 %1, label %bb3, label %bb2

bb3:                                              ; preds = %bb2, %bb0
  %__LookupPointer_new-boxed-value_fptr52 = call ptr @llvm.dcvm.lookup(i64 3684810824690217768)
  %__CallFf_33___call = tail call i64 %__LookupPointer_new-boxed-value_fptr52(i64 7, i64 %arg_0), !callees !0
  %__LookupPointer_send-event_fptr = call ptr @llvm.dcvm.lookup(i64 4789555171397753330)
  %__CallFf_37___call = tail call i64 %__LookupPointer_send-event_fptr(i64 -3325057813225958160, i64 6150028017864311825, i64 %__CallFf_33___call), !callees !4
  %__BranchIfNot_39___bool.not = icmp eq i64 %arg_1, 0
  br i1 %__BranchIfNot_39___bool.not, label %bb5, label %bb4

bb4:                                              ; preds = %bb3
  %__LookupPointer_get-gas-mask_fptr = call ptr @llvm.dcvm.lookup(i64 -5076200561558624898)
  %__CallFf_5___call.i = tail call i64 %__LookupPointer_get-gas-mask_fptr(i64 %arg_0), !callees !7
  %__CallFf_11___call.i = tail call i64 @send-event(i64 -3264453205741318791, i64 %arg_0)
  %__BranchIfNot_13___bool.not.i = icmp eq i64 %arg_2, 0
  br i1 %__BranchIfNot_13___bool.not.i, label %bb6, label %bb1.i

bb1.i:                                            ; preds = %bb4
  %__LookupPointer_kill-entity_fptr = call ptr @llvm.dcvm.lookup(i64 -804116741684351865)
  %__CallFf_17___call.i = tail call i64 %__LookupPointer_kill-entity_fptr(i64 %__CallFf_5___call.i), !callees !71
  br label %bb6

bb5:                                              ; preds = %bb3
  %__LookupPointer_internal-put-off-gas-mask_fptr = call ptr @llvm.dcvm.lookup(i64 -6610620527767074061)
  %__Call_52___call = tail call i64 %__LookupPointer_internal-put-off-gas-mask_fptr(i64 %arg_0, i64 %arg_2), !callees !83
  br label %bb6

bb6:                                              ; preds = %bb1.i, %bb4, %bb5
  %storemerge = phi i64 [ %__Call_52___call, %bb5 ], [ %__CallFf_17___call.i, %bb1.i ], [ 0, %bb4 ]
  ret i64 %storemerge
}

; Function Attrs: nounwind speculatable willreturn memory(none)
declare ptr @llvm.dcvm.lookup(i64) #2

attributes #0 = { nosync nounwind }
attributes #1 = { mustprogress nosync nounwind willreturn }
attributes #2 = { nounwind speculatable willreturn memory(none) }
attributes #3 = { nounwind }

!0 = !{ptr @new-boxed-value}
!1 = !{ptr @fact-set}
!2 = !{ptr @"is-script-running?"}
!3 = !{ptr @wait-one-frame}
!4 = !{ptr @send-event}
!5 = !{ptr @internal-put-on-gas-mask-immediately}
!6 = !{ptr @internal-put-on-gas-mask}
!7 = !{ptr @get-gas-mask}
!8 = !{!"sid", i64 -2210386714833194546, !"distance", !"far"}
!9 = !{!"sid", i64 3684810824690217768, !"distance", !"far"}
!10 = !{!"sid", i64 6481843155139167450, !"distance", !"near"}
!11 = !{!"sid", i64 -3047912596559836939, !"distance", !"far"}
!12 = !{!"sid", i64 4789555171397753330, !"distance", !"far"}
!13 = !{!"sid", i64 6078802017353408438, !"distance", !"near"}
!14 = !{!"sid", i64 5608585023750540989, !"distance", !"near"}
!15 = !{!"sid", i64 -5076200561558624898, !"distance", !"far"}
!16 = !{ptr @get-gas-mask-setting-id}
!17 = !{ptr @lookup-symbol}
!18 = !{ptr @wait-until-in-valid-gas-mask-state}
!19 = !{ptr @spawn-object}
!20 = !{ptr @set-high-contrast-mode-type}
!21 = !{ptr @"is-player?"}
!22 = !{ptr @set-instance-flag-hero}
!23 = !{ptr @set-show-in-flashlight-flag}
!24 = !{ptr @set-shadow-casting-from-npc-flashlight}
!25 = !{ptr @set-gas-mask}
!26 = !{ptr @set-visible}
!27 = !{ptr @attach}
!28 = !{ptr @"#%alloc-array"}
!29 = !{ptr @alloc-gesture-play-params}
!30 = !{!"sid", i64 -7658033447216601854, !"distance", !"far"}
!31 = !{!"sid", i64 -6573555005595409285, !"distance", !"far"}
!32 = !{!"sid", i64 8203816822361639083, !"distance", !"near"}
!33 = !{!"sid", i64 7097052834867614476, !"distance", !"far"}
!34 = !{!"sid", i64 4766017279285775524, !"distance", !"far"}
!35 = !{!"sid", i64 6532480022340268902, !"distance", !"far"}
!36 = !{!"sid", i64 -429226074377614887, !"distance", !"far"}
!37 = !{!"sid", i64 -7252477440030364545, !"distance", !"far"}
!38 = !{!"sid", i64 -3854484060414110792, !"distance", !"far"}
!39 = !{!"sid", i64 -5971891483814290574, !"distance", !"far"}
!40 = !{!"sid", i64 -7746349783173232884, !"distance", !"far"}
!41 = !{!"sid", i64 -5676660224586178492, !"distance", !"far"}
!42 = !{!"sid", i64 -9179819928081802467, !"distance", !"far"}
!43 = !{!"sid", i64 -7745656483000545638, !"distance", !"far"}
!44 = !{!"sid", i64 6500315221283771953, !"distance", !"far"}
!45 = !{!"sid", i64 -2962780980939953026, !"distance", !"far"}
!46 = !{ptr @"player-in-state?"}
!47 = !{ptr @player-force-crouch}
!48 = !{ptr @"player-is-crouched?"}
!49 = !{ptr @player-force-stand}
!50 = !{ptr @"!"}
!51 = !{ptr @wait-holster-player-weapon}
!52 = !{ptr @holster-player-weapon-instantly}
!53 = !{ptr @wait-npc-holster-weapon}
!54 = !{ptr @"npc-has-weapon-in-hand?"}
!55 = !{ptr @wait-npc-set-demeanor}
!56 = !{!"sid", i64 -9003251364961024783, !"distance", !"far"}
!57 = !{!"sid", i64 -4831287436211302109, !"distance", !"far"}
!58 = !{!"sid", i64 1646463116072042322, !"distance", !"far"}
!59 = !{!"sid", i64 8594926419455150747, !"distance", !"far"}
!60 = !{!"sid", i64 -5808627241898115124, !"distance", !"near"}
!61 = !{!"sid", i64 -3092934816467423265, !"distance", !"far"}
!62 = !{!"sid", i64 -626103520744544694, !"distance", !"far"}
!63 = !{!"sid", i64 5808793362033684947, !"distance", !"far"}
!64 = !{!"sid", i64 7683024912818272899, !"distance", !"far"}
!65 = !{!"sid", i64 8573502661920361253, !"distance", !"far"}
!66 = !{ptr @"suspend-idle-gestures/f"}
!67 = !{ptr @npc-disable-dialog-look}
!68 = !{!"sid", i64 -5356689657310757786, !"distance", !"near"}
!69 = !{!"sid", i64 7610916627332680768, !"distance", !"far"}
!70 = !{!"sid", i64 -8559530813269772124, !"distance", !"near"}
!71 = !{ptr @kill-entity}
!72 = !{!"sid", i64 -804116741684351865, !"distance", !"far"}
!73 = !{ptr @player-disable-dialog-look-gestures}
!74 = !{ptr @"player-disable-squeeze-through/f"}
!75 = !{ptr @"player-set-narrative-mode/f"}
!76 = !{ptr @joypad-disable-commands}
!77 = !{!"sid", i64 7238593933813171071, !"distance", !"far"}
!78 = !{!"sid", i64 8165897991486741957, !"distance", !"far"}
!79 = !{!"sid", i64 -2826777519175913837, !"distance", !"near"}
!80 = !{!"sid", i64 7121253686161376880, !"distance", !"far"}
!81 = !{!"sid", i64 -6610620527767074061, !"distance", !"near"}
!82 = !{ptr @animate_}
!83 = !{ptr @internal-put-off-gas-mask}

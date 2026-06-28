; ModuleID = 'anim-gas-mask-impl.bin'
source_filename = "anim-gas-mask-impl.bin"
target triple = "x86_64-pc-windows-msvc"

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
@llvm.used = appending global [11 x ptr] [ptr @get-equipped-gas-mask_symbol_table_src, ptr @internal-put-off-gas-mask-immediately_symbol_table_src, ptr @internal-put-off-gas-mask_symbol_table_src, ptr @internal-put-on-gas-mask-immediately_symbol_table_src, ptr @internal-put-on-gas-mask_symbol_table_src, ptr @"npc-gas-mask-narrative-mode/f_symbol_table_src", ptr @"player-gas-mask-narrative-mode/f_symbol_table_src", ptr @spawn-gas-mask_symbol_table_src, ptr @wait-equip-gas-mask_symbol_table_src, ptr @wait-remove-gas-mask_symbol_table_src, ptr @wait-until-in-valid-gas-mask-state_symbol_table_src], section "llvm.metadata"

; Function Attrs: nosync nounwind
define i64 @wait-equip-gas-mask(i64 %arg_0, i64 %arg_1, i64 %arg_2) local_unnamed_addr #0 !dcvm.sid_distance !0 {
bb0:
  %__LookupPointer_new-boxed-value_fptr = tail call ptr @llvm.dcvm.lookup(i64 3684810824690217768)
  %__CallFf_11___call = tail call i64 %__LookupPointer_new-boxed-value_fptr(i64 1, i64 1) #3, !callees !1, !dcvm.distance !2
  %__LookupPointer_fact-set_fptr = tail call ptr @llvm.dcvm.lookup(i64 -2210386714833194546)
  %__CallFf_15___call = tail call i64 %__LookupPointer_fact-set_fptr(i64 %arg_0, i64 7671589888336862994, i64 %__CallFf_11___call) #3, !callees !3, !dcvm.distance !2
  %"__LookupPointer_is-script-running?_fptr" = tail call ptr @llvm.dcvm.lookup(i64 6481843155139167450)
  %__Call_19___call51 = tail call i64 %"__LookupPointer_is-script-running?_fptr"(i64 6150028017864311825) #3, !callees !4
  %__OpLogNot_20__op52 = icmp eq i64 %__Call_19___call51, 0
  br i1 %__OpLogNot_20__op52, label %bb2.preheader, label %bb3

bb2.preheader:                                    ; preds = %bb0
  %__LookupPointer_wait-one-frame_fptr = tail call ptr @llvm.dcvm.lookup(i64 -3047912596559836939)
  br label %bb2

bb2:                                              ; preds = %bb2.preheader, %bb2
  %__CallFf_23___call = tail call i64 %__LookupPointer_wait-one-frame_fptr() #3, !callees !5, !dcvm.distance !2
  %__Call_19___call = tail call i64 %"__LookupPointer_is-script-running?_fptr"(i64 6150028017864311825) #3, !callees !4
  %__OpLogNot_20__op = icmp eq i64 %__Call_19___call, 0
  br i1 %__OpLogNot_20__op, label %bb2, label %bb3

bb3:                                              ; preds = %bb2, %bb0
  %__CallFf_33___call = tail call i64 %__LookupPointer_new-boxed-value_fptr(i64 7, i64 %arg_0) #3, !callees !1, !dcvm.distance !2
  %__LookupPointer_send-event_fptr = tail call ptr @llvm.dcvm.lookup(i64 4789555171397753330)
  %__CallFf_37___call = tail call i64 %__LookupPointer_send-event_fptr(i64 -3266162430027026367, i64 6150028017864311825, i64 %__CallFf_33___call) #3, !callees !6, !dcvm.distance !2
  %__BranchIfNot_39___bool.not = icmp eq i64 %arg_2, 0
  %__LookupPointer_internal-put-on-gas-mask-immediately_fptr = tail call ptr @llvm.dcvm.lookup(i64 6078802017353408438)
  %__LookupPointer_internal-put-on-gas-mask_fptr = tail call ptr @llvm.dcvm.lookup(i64 5608585023750540989)
  %__LookupPointer_internal-put-on-gas-mask-immediately_fptr.sink = select i1 %__BranchIfNot_39___bool.not, ptr %__LookupPointer_internal-put-on-gas-mask_fptr, ptr %__LookupPointer_internal-put-on-gas-mask-immediately_fptr
  %__Call_45___call = tail call i64 %__LookupPointer_internal-put-on-gas-mask-immediately_fptr.sink(i64 %arg_0, i64 %arg_1) #3
  %__Call_56___call53 = tail call i64 %"__LookupPointer_is-script-running?_fptr"(i64 6150028017864311825) #3, !callees !4
  %__OpLogNot_57__op54 = icmp eq i64 %__Call_56___call53, 0
  br i1 %__OpLogNot_57__op54, label %bb7.preheader, label %bb8

bb7.preheader:                                    ; preds = %bb3
  %__LookupPointer_wait-one-frame_fptr58 = tail call ptr @llvm.dcvm.lookup(i64 -3047912596559836939)
  br label %bb7

bb7:                                              ; preds = %bb7.preheader, %bb7
  %__CallFf_60___call = tail call i64 %__LookupPointer_wait-one-frame_fptr58() #3, !callees !5, !dcvm.distance !2
  %__Call_56___call = tail call i64 %"__LookupPointer_is-script-running?_fptr"(i64 6150028017864311825) #3, !callees !4
  %__OpLogNot_57__op = icmp eq i64 %__Call_56___call, 0
  br i1 %__OpLogNot_57__op, label %bb7, label %bb8

bb8:                                              ; preds = %bb7, %bb3
  %__LookupPointer_get-gas-mask_fptr = tail call ptr @llvm.dcvm.lookup(i64 -5076200561558624898)
  %__CallFf_65___call = tail call i64 %__LookupPointer_get-gas-mask_fptr(i64 %arg_0) #3, !callees !7, !dcvm.distance !2
  ret i64 %__CallFf_65___call
}

; Function Attrs: mustprogress nosync nounwind willreturn
define i64 @internal-put-on-gas-mask(i64 %arg_0, i64 %arg_1) local_unnamed_addr #1 !dcvm.sid_distance !8 {
bb0:
  %__LookupPointer_wait-one-frame_fptr = tail call ptr @llvm.dcvm.lookup(i64 -3047912596559836939)
  %__CallFf_3___call = tail call i64 %__LookupPointer_wait-one-frame_fptr() #3, !callees !5, !dcvm.distance !2
  %__LookupPointer_get-gas-mask-setting-id_fptr = tail call ptr @llvm.dcvm.lookup(i64 -6573555005595409285)
  %__CallFf_8___call = tail call i64 %__LookupPointer_get-gas-mask-setting-id_fptr(i64 %arg_0) #3, !callees !9, !dcvm.distance !2
  %__LookupPointer_lookup-symbol_fptr = tail call ptr @llvm.dcvm.lookup(i64 -7658033447216601854)
  %__CallFf_12___call = tail call i64 %__LookupPointer_lookup-symbol_fptr(i64 %__CallFf_8___call, i64 3455792245619945009) #3, !callees !10, !dcvm.distance !2
  %0 = inttoptr i64 %__CallFf_12___call to ptr
  %__IAddImm_17__op = getelementptr i8, ptr %0, i64 56
  %__LoadU8_18___load = load i8, ptr %__IAddImm_17__op, align 1
  %__LoadU8_18___ext = zext i8 %__LoadU8_18___load to i64
  %__LookupPointer_wait-until-in-valid-gas-mask-state_fptr = tail call ptr @llvm.dcvm.lookup(i64 8203816822361639083)
  %__Call_21___call = tail call i64 %__LookupPointer_wait-until-in-valid-gas-mask-state_fptr(i64 %arg_0, i64 %__LoadU8_18___ext) #3, !callees !11
  %__IEqual_24__op = icmp eq i64 %arg_1, 0
  br i1 %__IEqual_24__op, label %bb1, label %bb3

bb1:                                              ; preds = %bb0
  %__IAddImm_29__op = getelementptr i8, ptr %0, i64 40
  %__LoadU64_30___load = load i64, ptr %__IAddImm_29__op, align 4
  %__LookupPointer_spawn-object_fptr = tail call ptr @llvm.dcvm.lookup(i64 7097052834867614476)
  %__CallFf_48___call = tail call i64 %__LookupPointer_spawn-object_fptr(i64 %__LoadU64_30___load, i64 267125575217391726, i64 0, i64 0, i64 1, i64 0, i64 0, i64 0, i64 0) #3, !callees !12, !dcvm.distance !2
  br label %bb3

bb3:                                              ; preds = %bb0, %bb1
  %r1.0 = phi i64 [ %__CallFf_48___call, %bb1 ], [ %arg_1, %bb0 ]
  %__LookupPointer_set-high-contrast-mode-type_fptr = tail call ptr @llvm.dcvm.lookup(i64 4766017279285775524)
  %__CallFf_58___call = tail call i64 %__LookupPointer_set-high-contrast-mode-type_fptr(i64 %r1.0, i64 6) #3, !callees !13, !dcvm.distance !2
  %"__LookupPointer_is-player?_fptr" = tail call ptr @llvm.dcvm.lookup(i64 6532480022340268902)
  %__CallFf_62___call = tail call i64 %"__LookupPointer_is-player?_fptr"(i64 %arg_0) #3, !callees !14, !dcvm.distance !2
  %__BranchIfNot_63___bool.not = icmp eq i64 %__CallFf_62___call, 0
  br i1 %__BranchIfNot_63___bool.not, label %bb5, label %bb4

bb4:                                              ; preds = %bb3
  %__LookupPointer_set-instance-flag-hero_fptr = tail call ptr @llvm.dcvm.lookup(i64 -429226074377614887)
  %__CallFf_69___call = tail call i64 %__LookupPointer_set-instance-flag-hero_fptr(i64 %r1.0, i64 1) #3, !callees !15, !dcvm.distance !2
  %__LookupPointer_set-show-in-flashlight-flag_fptr = tail call ptr @llvm.dcvm.lookup(i64 -7252477440030364545)
  br label %bb6

bb5:                                              ; preds = %bb3
  %__LookupPointer_set-shadow-casting-from-npc-flashlight_fptr = tail call ptr @llvm.dcvm.lookup(i64 -3854484060414110792)
  br label %bb6

bb6:                                              ; preds = %bb5, %bb4
  %__LookupPointer_set-shadow-casting-from-npc-flashlight_fptr.sink = phi ptr [ %__LookupPointer_set-shadow-casting-from-npc-flashlight_fptr, %bb5 ], [ %__LookupPointer_set-show-in-flashlight-flag_fptr, %bb4 ]
  %__CallFf_82___call = tail call i64 %__LookupPointer_set-shadow-casting-from-npc-flashlight_fptr.sink(i64 %r1.0, i64 0) #3
  %__LookupPointer_set-gas-mask_fptr = tail call ptr @llvm.dcvm.lookup(i64 -5971891483814290574)
  %__CallFf_88___call = tail call i64 %__LookupPointer_set-gas-mask_fptr(i64 %arg_0, i64 %r1.0) #3, !callees !16, !dcvm.distance !2
  %__LookupPointer_set-visible_fptr = tail call ptr @llvm.dcvm.lookup(i64 -7746349783173232884)
  %__CallFf_94___call = tail call i64 %__LookupPointer_set-visible_fptr(i64 %r1.0, i64 0) #3, !callees !17, !dcvm.distance !2
  %__CallFf_98___call = tail call i64 %"__LookupPointer_is-player?_fptr"(i64 %arg_0) #3, !callees !14, !dcvm.distance !2
  %__OpLogNot_99__op = icmp eq i64 %__CallFf_98___call, 0
  br i1 %__OpLogNot_99__op, label %bb7, label %bb9

bb7:                                              ; preds = %bb6
  %__CallFf_102___call = tail call i64 %__LookupPointer_wait-one-frame_fptr() #3, !callees !5, !dcvm.distance !2
  br label %bb9

bb9:                                              ; preds = %bb6, %bb7
  %__LookupPointer_attach_fptr = tail call ptr @llvm.dcvm.lookup(i64 -5676660224586178492)
  %__CallFf_114___call = tail call i64 %__LookupPointer_attach_fptr(i64 %r1.0, i64 %arg_0, i64 8667864169316382944, float 0.000000e+00) #3, !callees !18, !dcvm.distance !2
  %__LookupPointer_new-boxed-value_fptr = tail call ptr @llvm.dcvm.lookup(i64 3684810824690217768)
  %__CallFf_123___call = tail call i64 %__LookupPointer_new-boxed-value_fptr(i64 6, float 0x3FD3333340000000) #3, !callees !1, !dcvm.distance !2
  %__LookupPointer_send-event_fptr = tail call ptr @llvm.dcvm.lookup(i64 4789555171397753330)
  %__CallFf_127___call = tail call i64 %__LookupPointer_send-event_fptr(i64 6817768622307059015, i64 %arg_0, i64 %__CallFf_123___call) #3, !callees !6, !dcvm.distance !2
  %__IAddImm_132__op = getelementptr i8, ptr %0, i64 16
  %__LoadU64_133___load = load i64, ptr %__IAddImm_132__op, align 4
  %"__LookupPointer_#%alloc-array_fptr" = tail call ptr @llvm.dcvm.lookup(i64 -7745656483000545638)
  %__CallFf_143___call = tail call i64 %"__LookupPointer_#%alloc-array_fptr"(i64 8534565298598421705, i64 16, i64 0, i64 1) #3, !callees !19, !dcvm.distance !2
  %__LookupPointer_animate__fptr = tail call ptr @llvm.dcvm.lookup(i64 -9179819928081802467)
  %__CallFf_149___call = tail call i64 %__LookupPointer_animate__fptr(i64 %r1.0, i64 %__LoadU64_133___load, i64 %__CallFf_143___call) #3, !callees !20, !dcvm.distance !2
  %__LookupPointer_alloc-gesture-play-params_fptr = tail call ptr @llvm.dcvm.lookup(i64 -2962780980939953026)
  %__LookupPointer_gesture__fptr = tail call ptr @llvm.dcvm.lookup(i64 6500315221283771953)
  br label %bb10

bb10:                                             ; preds = %bb11, %bb9
  %__LoadU64_154___load = load i64, ptr %0, align 4
  %__CallFf_156___call = tail call i64 %__LookupPointer_alloc-gesture-play-params_fptr() #3, !callees !21, !dcvm.distance !2
  %__IAddImm_158__op = add i64 %__CallFf_156___call, 33
  %1 = inttoptr i64 %__IAddImm_158__op to ptr
  store i8 1, ptr %1, align 1
  %__IAddImm_162__op = add i64 %__CallFf_156___call, 36
  %2 = inttoptr i64 %__IAddImm_162__op to ptr
  store i32 19, ptr %2, align 4
  %__IAddImm_166__op = add i64 %__CallFf_156___call, 32
  %3 = inttoptr i64 %__IAddImm_166__op to ptr
  store i8 1, ptr %3, align 1
  %__IAddImm_170__op = add i64 %__CallFf_156___call, 34
  %4 = inttoptr i64 %__IAddImm_170__op to ptr
  store i8 1, ptr %4, align 1
  %__CallFf_178___call = tail call i64 %__LookupPointer_gesture__fptr(i64 %arg_0, i64 %__LoadU64_154___load, i64 %__CallFf_156___call) #3, !callees !22, !dcvm.distance !2
  %__IEqual_180__op.not = icmp eq i64 %__CallFf_178___call, 0
  br i1 %__IEqual_180__op.not, label %bb12, label %bb11

bb11:                                             ; preds = %bb10
  %__CallFf_184___call = tail call i64 %__LookupPointer_wait-one-frame_fptr() #3, !callees !5, !dcvm.distance !2
  br label %bb10

bb12:                                             ; preds = %bb10
  %__IAddImm_190__op = getelementptr i8, ptr %0, i64 24
  %__LoadU64_191___load = load i64, ptr %__IAddImm_190__op, align 4
  %__CallFf_201___call = tail call i64 %"__LookupPointer_#%alloc-array_fptr"(i64 8534565298598421705, i64 16, i64 0, i64 1) #3, !callees !19, !dcvm.distance !2
  %__CallFf_207___call = tail call i64 %__LookupPointer_animate__fptr(i64 %r1.0, i64 %__LoadU64_191___load, i64 %__CallFf_201___call) #3, !callees !20, !dcvm.distance !2
  ret i64 %__CallFf_207___call
}

; Function Attrs: mustprogress nosync nounwind willreturn
define i64 @internal-put-on-gas-mask-immediately(i64 %arg_0, i64 %arg_1) local_unnamed_addr #1 !dcvm.sid_distance !23 {
bb0:
  %__LookupPointer_get-gas-mask-setting-id_fptr = tail call ptr @llvm.dcvm.lookup(i64 -6573555005595409285)
  %__CallFf_6___call = tail call i64 %__LookupPointer_get-gas-mask-setting-id_fptr(i64 %arg_0) #3, !callees !9, !dcvm.distance !2
  %__LookupPointer_lookup-symbol_fptr = tail call ptr @llvm.dcvm.lookup(i64 -7658033447216601854)
  %__CallFf_10___call = tail call i64 %__LookupPointer_lookup-symbol_fptr(i64 %__CallFf_6___call, i64 3455792245619945009) #3, !callees !10, !dcvm.distance !2
  %__IEqual_13__op = icmp eq i64 %arg_1, 0
  br i1 %__IEqual_13__op, label %bb1, label %bb3

bb1:                                              ; preds = %bb0
  %0 = inttoptr i64 %__CallFf_10___call to ptr
  %__IAddImm_18__op = getelementptr i8, ptr %0, i64 40
  %__LoadU64_19___load = load i64, ptr %__IAddImm_18__op, align 4
  %__LookupPointer_spawn-object_fptr = tail call ptr @llvm.dcvm.lookup(i64 7097052834867614476)
  %__CallFf_37___call = tail call i64 %__LookupPointer_spawn-object_fptr(i64 %__LoadU64_19___load, i64 267125575217391726, i64 0, i64 0, i64 1, i64 0, i64 0, i64 0, i64 0) #3, !callees !12, !dcvm.distance !2
  br label %bb3

bb3:                                              ; preds = %bb0, %bb1
  %r1.0 = phi i64 [ %__CallFf_37___call, %bb1 ], [ %arg_1, %bb0 ]
  %__LookupPointer_set-high-contrast-mode-type_fptr = tail call ptr @llvm.dcvm.lookup(i64 4766017279285775524)
  %__CallFf_47___call = tail call i64 %__LookupPointer_set-high-contrast-mode-type_fptr(i64 %r1.0, i64 6) #3, !callees !13, !dcvm.distance !2
  %__LookupPointer_set-gas-mask_fptr = tail call ptr @llvm.dcvm.lookup(i64 -5971891483814290574)
  %__CallFf_53___call = tail call i64 %__LookupPointer_set-gas-mask_fptr(i64 %arg_0, i64 %r1.0) #3, !callees !16, !dcvm.distance !2
  %"__LookupPointer_is-player?_fptr" = tail call ptr @llvm.dcvm.lookup(i64 6532480022340268902)
  %__CallFf_57___call = tail call i64 %"__LookupPointer_is-player?_fptr"(i64 %arg_0) #3, !callees !14, !dcvm.distance !2
  %__BranchIfNot_58___bool.not = icmp eq i64 %__CallFf_57___call, 0
  br i1 %__BranchIfNot_58___bool.not, label %bb3.bb6_crit_edge, label %bb4

bb3.bb6_crit_edge:                                ; preds = %bb3
  %.pre = tail call ptr @llvm.dcvm.lookup(i64 4789555171397753330)
  br label %bb6

bb4:                                              ; preds = %bb3
  %__LookupPointer_new-boxed-value_fptr = tail call ptr @llvm.dcvm.lookup(i64 3684810824690217768)
  %__CallFf_67___call = tail call i64 %__LookupPointer_new-boxed-value_fptr(i64 6, float 0.000000e+00) #3, !callees !1, !dcvm.distance !2
  %__LookupPointer_send-event_fptr = tail call ptr @llvm.dcvm.lookup(i64 4789555171397753330)
  %__CallFf_71___call = tail call i64 (i64, i64, ...) %__LookupPointer_send-event_fptr(i64 6817768622307059015, i64 %arg_0, i64 %__CallFf_67___call) #3, !callees !6, !dcvm.distance !2
  %__LookupPointer_set-instance-flag-hero_fptr = tail call ptr @llvm.dcvm.lookup(i64 -429226074377614887)
  %__CallFf_77___call = tail call i64 %__LookupPointer_set-instance-flag-hero_fptr(i64 %r1.0, i64 1) #3, !callees !15, !dcvm.distance !2
  br label %bb6

bb6:                                              ; preds = %bb3.bb6_crit_edge, %bb4
  %__LookupPointer_send-event_fptr130.pre-phi = phi ptr [ %.pre, %bb3.bb6_crit_edge ], [ %__LookupPointer_send-event_fptr, %bb4 ]
  %__CallFf_85___call = tail call i64 (i64, i64, ...) %__LookupPointer_send-event_fptr130.pre-phi(i64 1827492438953869695, i64 %arg_0) #3, !callees !6, !dcvm.distance !2
  %__CallFf_89___call = tail call i64 %"__LookupPointer_is-player?_fptr"(i64 %arg_0) #3, !callees !14, !dcvm.distance !2
  %__BranchIfNot_90___bool.not = icmp eq i64 %__CallFf_89___call, 0
  %__LookupPointer_set-shadow-casting-from-npc-flashlight_fptr = tail call ptr @llvm.dcvm.lookup(i64 -3854484060414110792)
  %__LookupPointer_set-show-in-flashlight-flag_fptr = tail call ptr @llvm.dcvm.lookup(i64 -7252477440030364545)
  %__LookupPointer_set-shadow-casting-from-npc-flashlight_fptr.sink = select i1 %__BranchIfNot_90___bool.not, ptr %__LookupPointer_set-shadow-casting-from-npc-flashlight_fptr, ptr %__LookupPointer_set-show-in-flashlight-flag_fptr
  %__CallFf_103___call = tail call i64 %__LookupPointer_set-shadow-casting-from-npc-flashlight_fptr.sink(i64 %r1.0, i64 0) #3
  %1 = inttoptr i64 %__CallFf_10___call to ptr
  %__IAddImm_108__op = getelementptr i8, ptr %1, i64 24
  %__LoadU64_109___load = load i64, ptr %__IAddImm_108__op, align 4
  %"__LookupPointer_#%alloc-array_fptr" = tail call ptr @llvm.dcvm.lookup(i64 -7745656483000545638)
  %__CallFf_119___call = tail call i64 %"__LookupPointer_#%alloc-array_fptr"(i64 8534565298598421705, i64 16, i64 1, i64 1) #3, !callees !19, !dcvm.distance !2
  %2 = inttoptr i64 %__CallFf_119___call to ptr
  %__LoadPointer_121___load = load ptr, ptr %2, align 8
  %__LoadPointer_125___load = load ptr, ptr %__LoadPointer_121___load, align 8
  store i32 31, ptr %__LoadPointer_125___load, align 4
  %__LoadPointer_129___load = load ptr, ptr %2, align 8
  %__LoadPointer_133___load = load ptr, ptr %__LoadPointer_129___load, align 8
  %__IAddImm_134__op = getelementptr i8, ptr %__LoadPointer_133___load, i64 8
  store i64 0, ptr %__IAddImm_134__op, align 4
  %__LoadPointer_138___load = load ptr, ptr %2, align 8
  %__LoadPointer_142___load = load ptr, ptr %__LoadPointer_138___load, align 8
  %__IAddImm_143__op = getelementptr i8, ptr %__LoadPointer_142___load, i64 8
  store i8 1, ptr %__IAddImm_143__op, align 1
  %__LookupPointer_animate__fptr = tail call ptr @llvm.dcvm.lookup(i64 -9179819928081802467)
  %__CallFf_151___call = tail call i64 %__LookupPointer_animate__fptr(i64 %r1.0, i64 %__LoadU64_109___load, i64 %__CallFf_119___call) #3, !callees !20, !dcvm.distance !2
  ret i64 %__CallFf_151___call
}

; Function Attrs: mustprogress nosync nounwind willreturn
define i64 @get-equipped-gas-mask(i64 %arg_0) local_unnamed_addr #1 !dcvm.sid_distance !24 {
bb0:
  %__LookupPointer_get-gas-mask_fptr = tail call ptr @llvm.dcvm.lookup(i64 -5076200561558624898)
  %__CallFf_4___call = tail call i64 %__LookupPointer_get-gas-mask_fptr(i64 %arg_0) #3, !callees !7, !dcvm.distance !2
  ret i64 %__CallFf_4___call
}

; Function Attrs: mustprogress nosync nounwind willreturn
define i64 @wait-until-in-valid-gas-mask-state(i64 %arg_0, i64 %arg_1) local_unnamed_addr #1 !dcvm.sid_distance !25 {
bb0:
  %"__LookupPointer_is-player?_fptr" = tail call ptr @llvm.dcvm.lookup(i64 6532480022340268902)
  %__CallFf_5___call = tail call i64 %"__LookupPointer_is-player?_fptr"(i64 %arg_0) #3, !callees !14, !dcvm.distance !2
  %__BranchIfNot_6___bool.not = icmp eq i64 %__CallFf_5___call, 0
  br i1 %__BranchIfNot_6___bool.not, label %bb15, label %bb1.preheader

bb1.preheader:                                    ; preds = %bb0
  %"__LookupPointer_player-in-state?_fptr" = tail call ptr @llvm.dcvm.lookup(i64 -9003251364961024783)
  %__LookupPointer_wait-one-frame_fptr = tail call ptr @llvm.dcvm.lookup(i64 -3047912596559836939)
  br label %bb1

bb1:                                              ; preds = %bb1.preheader, %bb3
  %__CallFf_10___call = tail call i64 %"__LookupPointer_player-in-state?_fptr"(i64 6740565536053766520) #3, !callees !26, !dcvm.distance !2
  %__OpLogNot_11__op = icmp eq i64 %__CallFf_10___call, 0
  br i1 %__OpLogNot_11__op, label %bb2, label %bb3

bb2:                                              ; preds = %bb1
  %__CallFf_16___call = tail call i64 %"__LookupPointer_player-in-state?_fptr"(i64 2237451099064635199) #3, !callees !26, !dcvm.distance !2
  %__OpLogNot_17__op.not = icmp eq i64 %__CallFf_16___call, 0
  br i1 %__OpLogNot_17__op.not, label %bb4, label %bb3

bb3:                                              ; preds = %bb2, %bb1
  %__CallFf_21___call = tail call i64 %__LookupPointer_wait-one-frame_fptr() #3, !callees !5, !dcvm.distance !2
  br label %bb1

bb4:                                              ; preds = %bb2
  %__BranchIfNot_24___bool.not = icmp eq i64 %arg_1, 0
  %"__LookupPointer_player-is-crouched?_fptr83" = tail call ptr @llvm.dcvm.lookup(i64 1646463116072042322)
  br i1 %__BranchIfNot_24___bool.not, label %bb9, label %bb5

bb5:                                              ; preds = %bb4
  %__LookupPointer_player-force-crouch_fptr = tail call ptr @llvm.dcvm.lookup(i64 -4831287436211302109)
  %__CallFf_26___call = tail call i64 %__LookupPointer_player-force-crouch_fptr() #3, !callees !27, !dcvm.distance !2
  %__CallFf_28___call66 = tail call i64 %"__LookupPointer_player-is-crouched?_fptr83"() #3, !callees !28, !dcvm.distance !2
  %__OpLogNot_29__op67 = icmp eq i64 %__CallFf_28___call66, 0
  br i1 %__OpLogNot_29__op67, label %bb7, label %bb12.preheader

bb12.preheader:                                   ; preds = %bb7, %bb11, %bb5, %bb9
  %__CallFf_50___call71 = tail call i64 %"__LookupPointer_player-in-state?_fptr"(i64 4007104907368024212) #3, !callees !26, !dcvm.distance !2
  %__OpLogNot_51__op72 = icmp eq i64 %__CallFf_50___call71, 0
  br i1 %__OpLogNot_51__op72, label %bb13, label %bb14

bb7:                                              ; preds = %bb5, %bb7
  %__CallFf_32___call = tail call i64 %__LookupPointer_wait-one-frame_fptr() #3, !callees !5, !dcvm.distance !2
  %__CallFf_28___call = tail call i64 %"__LookupPointer_player-is-crouched?_fptr83"() #3, !callees !28, !dcvm.distance !2
  %__OpLogNot_29__op = icmp eq i64 %__CallFf_28___call, 0
  br i1 %__OpLogNot_29__op, label %bb7, label %bb12.preheader

bb9:                                              ; preds = %bb4
  %__LookupPointer_player-force-stand_fptr = tail call ptr @llvm.dcvm.lookup(i64 8594926419455150747)
  %__CallFf_36___call = tail call i64 %__LookupPointer_player-force-stand_fptr() #3, !callees !29, !dcvm.distance !2
  %__CallFf_39___call68 = tail call i64 %"__LookupPointer_player-is-crouched?_fptr83"() #3, !callees !28, !dcvm.distance !2
  %"__LookupPointer_!_fptr" = tail call ptr @llvm.dcvm.lookup(i64 -5808627241898115124)
  %__Call_41___call69 = tail call i64 %"__LookupPointer_!_fptr"(i64 %__CallFf_39___call68) #3, !callees !30
  %__OpLogNot_42__op70 = icmp eq i64 %__Call_41___call69, 0
  br i1 %__OpLogNot_42__op70, label %bb11, label %bb12.preheader

bb11:                                             ; preds = %bb9, %bb11
  %__CallFf_45___call = tail call i64 %__LookupPointer_wait-one-frame_fptr() #3, !callees !5, !dcvm.distance !2
  %__CallFf_39___call = tail call i64 %"__LookupPointer_player-is-crouched?_fptr83"() #3, !callees !28, !dcvm.distance !2
  %__Call_41___call = tail call i64 %"__LookupPointer_!_fptr"(i64 %__CallFf_39___call) #3, !callees !30
  %__OpLogNot_42__op = icmp eq i64 %__Call_41___call, 0
  br i1 %__OpLogNot_42__op, label %bb11, label %bb12.preheader

bb13:                                             ; preds = %bb12.preheader, %bb13
  %__CallFf_54___call = tail call i64 %__LookupPointer_wait-one-frame_fptr() #3, !callees !5, !dcvm.distance !2
  %__CallFf_50___call = tail call i64 %"__LookupPointer_player-in-state?_fptr"(i64 4007104907368024212) #3, !callees !26, !dcvm.distance !2
  %__OpLogNot_51__op = icmp eq i64 %__CallFf_50___call, 0
  br i1 %__OpLogNot_51__op, label %bb13, label %bb14

bb14:                                             ; preds = %bb13, %bb12.preheader
  %__LookupPointer_wait-holster-player-weapon_fptr = tail call ptr @llvm.dcvm.lookup(i64 -3092934816467423265)
  %__CallFf_61___call = tail call i64 %__LookupPointer_wait-holster-player-weapon_fptr(i64 0, float 1.000000e+00) #3, !callees !31, !dcvm.distance !2
  %__LookupPointer_holster-player-weapon-instantly_fptr = tail call ptr @llvm.dcvm.lookup(i64 -626103520744544694)
  %__CallFf_67___call = tail call i64 %__LookupPointer_holster-player-weapon-instantly_fptr(float 0x3FC99999A0000000, i64 0) #3, !callees !32, !dcvm.distance !2
  br label %bb21

bb15:                                             ; preds = %bb0
  %__LookupPointer_wait-npc-holster-weapon_fptr = tail call ptr @llvm.dcvm.lookup(i64 5808793362033684947)
  %__CallFf_80___call = tail call i64 %__LookupPointer_wait-npc-holster-weapon_fptr(i64 %arg_0, i64 0, i64 1, float -1.000000e+00, i64 0) #3, !callees !33, !dcvm.distance !2
  %"__LookupPointer_npc-has-weapon-in-hand?_fptr" = tail call ptr @llvm.dcvm.lookup(i64 7683024912818272899)
  %__CallFf_84___call73 = tail call i64 %"__LookupPointer_npc-has-weapon-in-hand?_fptr"(i64 %arg_0) #3, !callees !34, !dcvm.distance !2
  %__BranchIfNot_85___bool.not74 = icmp eq i64 %__CallFf_84___call73, 0
  br i1 %__BranchIfNot_85___bool.not74, label %bb18, label %bb17.preheader

bb17.preheader:                                   ; preds = %bb15
  %__LookupPointer_wait-one-frame_fptr89 = tail call ptr @llvm.dcvm.lookup(i64 -3047912596559836939)
  br label %bb17

bb17:                                             ; preds = %bb17.preheader, %bb17
  %__CallFf_87___call = tail call i64 %__LookupPointer_wait-one-frame_fptr89() #3, !callees !5, !dcvm.distance !2
  %__CallFf_84___call = tail call i64 %"__LookupPointer_npc-has-weapon-in-hand?_fptr"(i64 %arg_0) #3, !callees !34, !dcvm.distance !2
  %__BranchIfNot_85___bool.not = icmp eq i64 %__CallFf_84___call, 0
  br i1 %__BranchIfNot_85___bool.not, label %bb18, label %bb17

bb18:                                             ; preds = %bb17, %bb15
  %__BranchIfNot_90___bool.not = icmp eq i64 %arg_1, 0
  br i1 %__BranchIfNot_90___bool.not, label %bb21, label %bb19

bb19:                                             ; preds = %bb18
  %__LookupPointer_wait-npc-set-demeanor_fptr = tail call ptr @llvm.dcvm.lookup(i64 8573502661920361253)
  %__CallFf_96___call = tail call i64 %__LookupPointer_wait-npc-set-demeanor_fptr(i64 %arg_0, i64 3) #3, !callees !35, !dcvm.distance !2
  br label %bb21

bb21:                                             ; preds = %bb18, %bb19, %bb14
  %r2.0 = phi i64 [ %__CallFf_67___call, %bb14 ], [ %__CallFf_96___call, %bb19 ], [ 0, %bb18 ]
  ret i64 %r2.0
}

; Function Attrs: mustprogress nosync nounwind willreturn
define i64 @"npc-gas-mask-narrative-mode/f"(i64 %arg_0) local_unnamed_addr #1 !dcvm.sid_distance !36 {
bb0:
  %__LookupPointer_send-event_fptr = tail call ptr @llvm.dcvm.lookup(i64 4789555171397753330)
  %__CallFf_6___call = tail call i64 %__LookupPointer_send-event_fptr(i64 8119835013146602502, i64 %arg_0) #3, !callees !6, !dcvm.distance !2
  %"__LookupPointer_suspend-idle-gestures/f_fptr" = tail call ptr @llvm.dcvm.lookup(i64 -5356689657310757786)
  %__Call_10___call = tail call i64 %"__LookupPointer_suspend-idle-gestures/f_fptr"(i64 %arg_0) #3, !callees !37
  %__LookupPointer_npc-disable-dialog-look_fptr = tail call ptr @llvm.dcvm.lookup(i64 7610916627332680768)
  %__CallFf_16___call = tail call i64 %__LookupPointer_npc-disable-dialog-look_fptr(i64 %arg_0, float 0.000000e+00) #3, !callees !38, !dcvm.distance !2
  ret i64 %__CallFf_16___call
}

; Function Attrs: mustprogress nosync nounwind willreturn
define i64 @internal-put-off-gas-mask-immediately(i64 %arg_0, i64 %arg_1) local_unnamed_addr #1 !dcvm.sid_distance !39 {
bb0:
  %__LookupPointer_get-gas-mask_fptr = tail call ptr @llvm.dcvm.lookup(i64 -5076200561558624898)
  %__CallFf_5___call = tail call i64 %__LookupPointer_get-gas-mask_fptr(i64 %arg_0) #3, !callees !7, !dcvm.distance !2
  %__LookupPointer_send-event_fptr = tail call ptr @llvm.dcvm.lookup(i64 4789555171397753330)
  %__CallFf_11___call = tail call i64 %__LookupPointer_send-event_fptr(i64 -3264453205741318791, i64 %arg_0) #3, !callees !6, !dcvm.distance !2
  %__BranchIfNot_13___bool.not = icmp eq i64 %arg_1, 0
  br i1 %__BranchIfNot_13___bool.not, label %bb3, label %bb1

bb1:                                              ; preds = %bb0
  %__LookupPointer_kill-entity_fptr = tail call ptr @llvm.dcvm.lookup(i64 -804116741684351865)
  %__CallFf_17___call = tail call i64 %__LookupPointer_kill-entity_fptr(i64 %__CallFf_5___call) #3, !callees !40, !dcvm.distance !2
  br label %bb3

bb3:                                              ; preds = %bb0, %bb1
  %r3.0 = phi i64 [ %__CallFf_17___call, %bb1 ], [ 0, %bb0 ]
  ret i64 %r3.0
}

; Function Attrs: mustprogress nosync nounwind willreturn
define i64 @spawn-gas-mask(i64 %arg_0) local_unnamed_addr #1 !dcvm.sid_distance !41 {
bb0:
  %__LookupPointer_get-gas-mask-setting-id_fptr = tail call ptr @llvm.dcvm.lookup(i64 -6573555005595409285)
  %__CallFf_5___call = tail call i64 %__LookupPointer_get-gas-mask-setting-id_fptr(i64 %arg_0) #3, !callees !9, !dcvm.distance !2
  %__LookupPointer_lookup-symbol_fptr = tail call ptr @llvm.dcvm.lookup(i64 -7658033447216601854)
  %__CallFf_9___call = tail call i64 %__LookupPointer_lookup-symbol_fptr(i64 %__CallFf_5___call, i64 3455792245619945009) #3, !callees !10, !dcvm.distance !2
  %0 = inttoptr i64 %__CallFf_9___call to ptr
  %__IAddImm_13__op = getelementptr i8, ptr %0, i64 40
  %__LoadU64_14___load = load i64, ptr %__IAddImm_13__op, align 4
  %__LookupPointer_spawn-object_fptr = tail call ptr @llvm.dcvm.lookup(i64 7097052834867614476)
  %__CallFf_32___call = tail call i64 %__LookupPointer_spawn-object_fptr(i64 %__LoadU64_14___load, i64 267125575217391726, i64 0, i64 0, i64 1, i64 0, i64 0, i64 0, i64 0) #3, !callees !12, !dcvm.distance !2
  %__LookupPointer_set-high-contrast-mode-type_fptr = tail call ptr @llvm.dcvm.lookup(i64 4766017279285775524)
  %__CallFf_38___call = tail call i64 %__LookupPointer_set-high-contrast-mode-type_fptr(i64 %__CallFf_32___call, i64 6) #3, !callees !13, !dcvm.distance !2
  ret i64 %__CallFf_32___call
}

; Function Attrs: mustprogress nosync nounwind willreturn
define i64 @"player-gas-mask-narrative-mode/f"(i64 %arg_0, i64 %arg_1, i64 %arg_2) local_unnamed_addr #1 !dcvm.sid_distance !42 {
bb0:
  %__LookupPointer_player-disable-dialog-look-gestures_fptr = tail call ptr @llvm.dcvm.lookup(i64 7238593933813171071)
  %__CallFf_6___call = tail call i64 %__LookupPointer_player-disable-dialog-look-gestures_fptr(float 0.000000e+00) #3, !callees !43, !dcvm.distance !2
  %"__LookupPointer_player-disable-squeeze-through/f_fptr" = tail call ptr @llvm.dcvm.lookup(i64 8165897991486741957)
  %__CallFf_8___call = tail call i64 %"__LookupPointer_player-disable-squeeze-through/f_fptr"() #3, !callees !44, !dcvm.distance !2
  %"__LookupPointer_player-set-narrative-mode/f_fptr" = tail call ptr @llvm.dcvm.lookup(i64 -2826777519175913837)
  %__Call_16___call = tail call i64 %"__LookupPointer_player-set-narrative-mode/f_fptr"(i64 %arg_0, i64 %arg_1, i64 %arg_2) #3, !callees !45
  %"__LookupPointer_suspend-idle-gestures/f_fptr" = tail call ptr @llvm.dcvm.lookup(i64 -5356689657310757786)
  %__Call_20___call = tail call i64 %"__LookupPointer_suspend-idle-gestures/f_fptr"(i64 5008278420455340480) #3, !callees !37
  %__LookupPointer_joypad-disable-commands_fptr = tail call ptr @llvm.dcvm.lookup(i64 7121253686161376880)
  %__CallFf_24___call = tail call i64 %__LookupPointer_joypad-disable-commands_fptr(i64 -1699981380503476753) #3, !callees !46, !dcvm.distance !2
  ret i64 %__CallFf_24___call
}

; Function Attrs: mustprogress nosync nounwind willreturn
define i64 @internal-put-off-gas-mask(i64 %arg_0, i64 %arg_1) local_unnamed_addr #1 !dcvm.sid_distance !47 {
bb0:
  %__LookupPointer_wait-one-frame_fptr = tail call ptr @llvm.dcvm.lookup(i64 -3047912596559836939)
  %__CallFf_3___call = tail call i64 %__LookupPointer_wait-one-frame_fptr() #3, !callees !5, !dcvm.distance !2
  %__LookupPointer_get-gas-mask-setting-id_fptr = tail call ptr @llvm.dcvm.lookup(i64 -6573555005595409285)
  %__CallFf_8___call = tail call i64 %__LookupPointer_get-gas-mask-setting-id_fptr(i64 %arg_0) #3, !callees !9, !dcvm.distance !2
  %__LookupPointer_lookup-symbol_fptr = tail call ptr @llvm.dcvm.lookup(i64 -7658033447216601854)
  %__CallFf_12___call = tail call i64 %__LookupPointer_lookup-symbol_fptr(i64 %__CallFf_8___call, i64 3455792245619945009) #3, !callees !10, !dcvm.distance !2
  %__LookupPointer_get-gas-mask_fptr = tail call ptr @llvm.dcvm.lookup(i64 -5076200561558624898)
  %__CallFf_16___call = tail call i64 %__LookupPointer_get-gas-mask_fptr(i64 %arg_0) #3, !callees !7, !dcvm.distance !2
  %0 = inttoptr i64 %__CallFf_12___call to ptr
  %__IAddImm_21__op = getelementptr i8, ptr %0, i64 57
  %__LoadU8_22___load = load i8, ptr %__IAddImm_21__op, align 1
  %__LoadU8_22___ext = zext i8 %__LoadU8_22___load to i64
  %__LookupPointer_wait-until-in-valid-gas-mask-state_fptr = tail call ptr @llvm.dcvm.lookup(i64 8203816822361639083)
  %__Call_25___call = tail call i64 %__LookupPointer_wait-until-in-valid-gas-mask-state_fptr(i64 %arg_0, i64 %__LoadU8_22___ext) #3, !callees !11
  %__IAddImm_30__op = getelementptr i8, ptr %0, i64 32
  %__LoadU64_31___load = load i64, ptr %__IAddImm_30__op, align 4
  %"__LookupPointer_#%alloc-array_fptr" = tail call ptr @llvm.dcvm.lookup(i64 -7745656483000545638)
  %__CallFf_41___call = tail call i64 %"__LookupPointer_#%alloc-array_fptr"(i64 8534565298598421705, i64 16, i64 0, i64 1) #3, !callees !19, !dcvm.distance !2
  %__LookupPointer_animate__fptr = tail call ptr @llvm.dcvm.lookup(i64 -9179819928081802467)
  %__CallFf_47___call = tail call i64 %__LookupPointer_animate__fptr(i64 %__CallFf_16___call, i64 %__LoadU64_31___load, i64 %__CallFf_41___call) #3, !callees !20, !dcvm.distance !2
  %__IAddImm_52__op = getelementptr i8, ptr %0, i64 8
  %__LookupPointer_alloc-gesture-play-params_fptr = tail call ptr @llvm.dcvm.lookup(i64 -2962780980939953026)
  %__LookupPointer_gesture__fptr = tail call ptr @llvm.dcvm.lookup(i64 6500315221283771953)
  br label %bb1

bb1:                                              ; preds = %bb2, %bb0
  %__LoadU64_53___load = load i64, ptr %__IAddImm_52__op, align 4
  %__CallFf_55___call = tail call i64 %__LookupPointer_alloc-gesture-play-params_fptr() #3, !callees !21, !dcvm.distance !2
  %__IAddImm_57__op = add i64 %__CallFf_55___call, 33
  %1 = inttoptr i64 %__IAddImm_57__op to ptr
  store i8 1, ptr %1, align 1
  %__IAddImm_61__op = add i64 %__CallFf_55___call, 36
  %2 = inttoptr i64 %__IAddImm_61__op to ptr
  store i32 19, ptr %2, align 4
  %__IAddImm_65__op = add i64 %__CallFf_55___call, 32
  %3 = inttoptr i64 %__IAddImm_65__op to ptr
  store i8 1, ptr %3, align 1
  %__IAddImm_69__op = add i64 %__CallFf_55___call, 34
  %4 = inttoptr i64 %__IAddImm_69__op to ptr
  store i8 1, ptr %4, align 1
  %__CallFf_77___call = tail call i64 %__LookupPointer_gesture__fptr(i64 %arg_0, i64 %__LoadU64_53___load, i64 %__CallFf_55___call) #3, !callees !22, !dcvm.distance !2
  %__IEqual_79__op.not = icmp eq i64 %__CallFf_77___call, 0
  br i1 %__IEqual_79__op.not, label %bb3, label %bb2

bb2:                                              ; preds = %bb1
  %__CallFf_83___call = tail call i64 %__LookupPointer_wait-one-frame_fptr() #3, !callees !5, !dcvm.distance !2
  br label %bb1

bb3:                                              ; preds = %bb1
  %__BranchIfNot_86___bool.not = icmp eq i64 %arg_1, 0
  br i1 %__BranchIfNot_86___bool.not, label %bb6, label %bb4

bb4:                                              ; preds = %bb3
  %__LookupPointer_kill-entity_fptr = tail call ptr @llvm.dcvm.lookup(i64 -804116741684351865)
  %__CallFf_90___call = tail call i64 %__LookupPointer_kill-entity_fptr(i64 %__CallFf_16___call) #3, !callees !40, !dcvm.distance !2
  br label %bb6

bb6:                                              ; preds = %bb3, %bb4
  %r4.0 = phi i64 [ %__CallFf_90___call, %bb4 ], [ 0, %bb3 ]
  ret i64 %r4.0
}

; Function Attrs: nosync nounwind
define i64 @wait-remove-gas-mask(i64 %arg_0, i64 %arg_1, i64 %arg_2) local_unnamed_addr #0 !dcvm.sid_distance !48 {
bb0:
  %__LookupPointer_new-boxed-value_fptr = tail call ptr @llvm.dcvm.lookup(i64 3684810824690217768)
  %__CallFf_11___call = tail call i64 %__LookupPointer_new-boxed-value_fptr(i64 1, i64 0) #3, !callees !1, !dcvm.distance !2
  %__LookupPointer_fact-set_fptr = tail call ptr @llvm.dcvm.lookup(i64 -2210386714833194546)
  %__CallFf_15___call = tail call i64 %__LookupPointer_fact-set_fptr(i64 %arg_0, i64 7671589888336862994, i64 %__CallFf_11___call) #3, !callees !3, !dcvm.distance !2
  %"__LookupPointer_is-script-running?_fptr" = tail call ptr @llvm.dcvm.lookup(i64 6481843155139167450)
  %__Call_19___call41 = tail call i64 %"__LookupPointer_is-script-running?_fptr"(i64 6150028017864311825) #3, !callees !4
  %__OpLogNot_20__op42 = icmp eq i64 %__Call_19___call41, 0
  br i1 %__OpLogNot_20__op42, label %bb2.preheader, label %bb3

bb2.preheader:                                    ; preds = %bb0
  %__LookupPointer_wait-one-frame_fptr = tail call ptr @llvm.dcvm.lookup(i64 -3047912596559836939)
  br label %bb2

bb2:                                              ; preds = %bb2.preheader, %bb2
  %__CallFf_23___call = tail call i64 %__LookupPointer_wait-one-frame_fptr() #3, !callees !5, !dcvm.distance !2
  %__Call_19___call = tail call i64 %"__LookupPointer_is-script-running?_fptr"(i64 6150028017864311825) #3, !callees !4
  %__OpLogNot_20__op = icmp eq i64 %__Call_19___call, 0
  br i1 %__OpLogNot_20__op, label %bb2, label %bb3

bb3:                                              ; preds = %bb2, %bb0
  %__CallFf_33___call = tail call i64 %__LookupPointer_new-boxed-value_fptr(i64 7, i64 %arg_0) #3, !callees !1, !dcvm.distance !2
  %__LookupPointer_send-event_fptr = tail call ptr @llvm.dcvm.lookup(i64 4789555171397753330)
  %__CallFf_37___call = tail call i64 %__LookupPointer_send-event_fptr(i64 -3325057813225958160, i64 6150028017864311825, i64 %__CallFf_33___call) #3, !callees !6, !dcvm.distance !2
  %__BranchIfNot_39___bool.not = icmp eq i64 %arg_1, 0
  br i1 %__BranchIfNot_39___bool.not, label %bb5, label %bb4

bb4:                                              ; preds = %bb3
  %__LookupPointer_get-gas-mask_fptr = tail call ptr @llvm.dcvm.lookup(i64 -5076200561558624898)
  %__CallFf_5___call.i = tail call i64 %__LookupPointer_get-gas-mask_fptr(i64 %arg_0) #3, !callees !7, !dcvm.distance !2
  %__CallFf_11___call.i = tail call i64 %__LookupPointer_send-event_fptr(i64 -3264453205741318791, i64 %arg_0) #3, !callees !6, !dcvm.distance !2
  %__BranchIfNot_13___bool.not.i = icmp eq i64 %arg_2, 0
  br i1 %__BranchIfNot_13___bool.not.i, label %bb6, label %bb1.i

bb1.i:                                            ; preds = %bb4
  %__LookupPointer_kill-entity_fptr = tail call ptr @llvm.dcvm.lookup(i64 -804116741684351865)
  %__CallFf_17___call.i = tail call i64 %__LookupPointer_kill-entity_fptr(i64 %__CallFf_5___call.i) #3, !callees !40, !dcvm.distance !2
  br label %bb6

bb5:                                              ; preds = %bb3
  %__LookupPointer_internal-put-off-gas-mask_fptr = tail call ptr @llvm.dcvm.lookup(i64 -6610620527767074061)
  %__Call_52___call = tail call i64 %__LookupPointer_internal-put-off-gas-mask_fptr(i64 %arg_0, i64 %arg_2) #3, !callees !49
  br label %bb6

bb6:                                              ; preds = %bb1.i, %bb4, %bb5
  %r3.0 = phi i64 [ %__Call_52___call, %bb5 ], [ %__CallFf_17___call.i, %bb1.i ], [ 0, %bb4 ]
  ret i64 %r3.0
}

; Function Attrs: mustprogress nofree nosync nounwind speculatable willreturn memory(none)
declare ptr @llvm.dcvm.lookup(i64) #2

attributes #0 = { nosync nounwind }
attributes #1 = { mustprogress nosync nounwind willreturn }
attributes #2 = { mustprogress nofree nosync nounwind speculatable willreturn memory(none) }
attributes #3 = { nounwind }

!0 = !{!"sid", i64 4147362863116273348}
!1 = distinct !{null}
!2 = !{!"far"}
!3 = distinct !{null}
!4 = distinct !{null}
!5 = distinct !{null}
!6 = distinct !{null}
!7 = distinct !{null}
!8 = !{!"sid", i64 5608585023750540989, !"distance", !"near"}
!9 = distinct !{null}
!10 = distinct !{null}
!11 = !{ptr @wait-until-in-valid-gas-mask-state}
!12 = distinct !{null}
!13 = distinct !{null}
!14 = distinct !{null}
!15 = distinct !{null}
!16 = distinct !{null}
!17 = distinct !{null}
!18 = distinct !{null}
!19 = distinct !{null}
!20 = distinct !{null}
!21 = distinct !{null}
!22 = distinct !{null}
!23 = !{!"sid", i64 6078802017353408438, !"distance", !"near"}
!24 = !{!"sid", i64 6464408158396193538}
!25 = !{!"sid", i64 8203816822361639083, !"distance", !"near"}
!26 = distinct !{null}
!27 = distinct !{null}
!28 = distinct !{null}
!29 = distinct !{null}
!30 = distinct !{null}
!31 = distinct !{null}
!32 = distinct !{null}
!33 = distinct !{null}
!34 = distinct !{null}
!35 = distinct !{null}
!36 = !{!"sid", i64 9135517996633164385}
!37 = distinct !{null}
!38 = distinct !{null}
!39 = !{!"sid", i64 -8559530813269772124, !"distance", !"near"}
!40 = distinct !{null}
!41 = !{!"sid", i64 -8188029138696281045}
!42 = !{!"sid", i64 -7406256944386539599}
!43 = distinct !{null}
!44 = distinct !{null}
!45 = distinct !{null}
!46 = distinct !{null}
!47 = !{!"sid", i64 -6610620527767074061, !"distance", !"near"}
!48 = !{!"sid", i64 -1351166648529978758}
!49 = !{ptr @internal-put-off-gas-mask}

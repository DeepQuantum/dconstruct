	.file	"nd-script-funcs.bin"
	.text
	.globl	"print-point"
	.type	"print-point",@function
"print-point":
	LoadU16Imm	r0, 0
	Return	r0
.Lfunc_end0:
	.size	"print-point", .Lfunc_end0-"print-point"

	.globl	"set-bit"
	.type	"set-bit",@function
"set-bit":
	LoadU16Imm	r0, 1
	IntAsh	r0, r0, r50
	OpLogNot	r1, r51
	BranchIf	.LBB1_1, r1
	OpBitOr	r0, r0, r49
	Return	r0
.LBB1_1:
	OpBitNot	r0, r0
	OpBitAnd	r0, r49, r0
	Return	r0
.Lfunc_end1:
	.size	"set-bit", .Lfunc_end1-"set-bit"

	.globl	"wait-task-complete"
	.type	"wait-task-complete",@function
"wait-task-complete":
	LookupPointer	r2, 0
	LookupPointer	r3, 1
	LookupPointer	r4, 2
	LookupPointer	r5, 3
	LoadU16Imm	r6, 4
	Branch	.LBB2_1
.LBB2_5:
	IAddImm	r0, r1, 96
	LoadU32	r0, r0
	IEqual	r0, r0, r6
	BranchIf	.LBB2_4, r0
.LBB2_6:
	CallFf	r0, r2, 0
.LBB2_1:
	CallFf	r0, r4, 2
	Move	r1, r0
	BranchIfNot	.LBB2_6, r1
	CallFf	r0, r3, 0
	BranchIfNot	.LBB2_5, r0
	CallFf	r0, r5, 2
	BranchIfNot	.LBB2_6, r0
.LBB2_4:
	LoadU16Imm	r0, 0
	Return	r0
"wait-task-complete_symbol_table":
	.quad	-3047912596559836939
	.quad	-3098704140420144833
	.quad	4627505823601477936
	.quad	-9020003840458003388
"wait-task-complete_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end2:
	.size	"wait-task-complete", .Lfunc_end2-"wait-task-complete"

	.globl	"task-get-timeline"
	.type	"task-get-timeline",@function
"task-get-timeline":
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	BranchIfNot	.LBB3_1, r0
	LookupPointer	r1, 2
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
.LBB3_1:
	LoadStaticU64Imm	r0, 1
	Return	r0
"task-get-timeline_symbol_table":
	.quad	4627505823601477936
	.quad	-1
	.quad	-1194883309477416555
"task-get-timeline_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end3:
	.size	"task-get-timeline", .Lfunc_end3-"task-get-timeline"

	.globl	"print-string"
	.type	"print-string",@function
"print-string":
	LoadU16Imm	r0, 0
	Return	r0
.Lfunc_end4:
	.size	"print-string", .Lfunc_end4-"print-string"

	.globl	"set-shader-instance-shimmer-intensity"
	.type	"set-shader-instance-shimmer-intensity",@function
"set-shader-instance-shimmer-intensity":
	Move	r51, r50
	LoadStaticU64Imm	r50, 1
	LookupPointer	r0, 0
	CallFf	r0, r0, 3
	Return	r0
"set-shader-instance-shimmer-intensity_symbol_table":
	.quad	-2085764591387011864
	.quad	4338449670443486021
"set-shader-instance-shimmer-intensity_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end5:
	.size	"set-shader-instance-shimmer-intensity", .Lfunc_end5-"set-shader-instance-shimmer-intensity"

	.globl	"stop-timer"
	.type	"stop-timer",@function
"stop-timer":
	Move	r51, r50
	LoadU16Imm	r50, 0
	LookupPointer	r0, 0
	CallFf	r0, r0, 3
	Return	r0
"stop-timer_symbol_table":
	.quad	2780616685946547256
"stop-timer_pointer_map":
	.byte	0
.Lfunc_end6:
	.size	"stop-timer", .Lfunc_end6-"stop-timer"

	.globl	"camera-enable-animated-no-fail-over"
	.type	"camera-enable-animated-no-fail-over",@function
"camera-enable-animated-no-fail-over":
	LoadU16Imm	r55, 1
	LookupPointer	r0, 0
	CallFf	r0, r0, 7
	Return	r0
"camera-enable-animated-no-fail-over_symbol_table":
	.quad	3112973522196772772
"camera-enable-animated-no-fail-over_pointer_map":
	.byte	0
.Lfunc_end7:
	.size	"camera-enable-animated-no-fail-over", .Lfunc_end7-"camera-enable-animated-no-fail-over"

	.globl	"wait-simple-npc-move-to-goal"
	.type	"wait-simple-npc-move-to-goal",@function
"wait-simple-npc-move-to-goal":
	Move	r55, r54
	LoadU16Imm	r54, 1
	LookupPointer	r0, 0
	CallFf	r0, r0, 7
	Return	r0
"wait-simple-npc-move-to-goal_symbol_table":
	.quad	-6869446841486065293
"wait-simple-npc-move-to-goal_pointer_map":
	.byte	0
.Lfunc_end8:
	.size	"wait-simple-npc-move-to-goal", .Lfunc_end8-"wait-simple-npc-move-to-goal"

	.globl	"cin-lerp-maya"
	.type	"cin-lerp-maya",@function
"cin-lerp-maya":
	Move	r1, r49
	LoadU16Imm	r49, 0
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	LoadU16Imm	r51, 0
	LoadStaticFloatImm	r52, 2
	LookupPointer	r2, 0
	Move	r49, r1
	Move	r53, r0
	CallFf	r0, r2, 5
	Return	r0
"cin-lerp-maya_symbol_table":
	.quad	6895426483596981631
	.quad	8088041438043820006
	.quad	1065353216
"cin-lerp-maya_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end9:
	.size	"cin-lerp-maya", .Lfunc_end9-"cin-lerp-maya"

	.globl	"event-message"
	.type	"event-message",@function
"event-message":
	LookupPointer	r0, 1
	CallFf	r0, r0, 0
	LookupPointer	r1, 0
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
"event-message_symbol_table":
	.quad	2968890880210854491
	.quad	-734627043301783238
"event-message_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end10:
	.size	"event-message", .Lfunc_end10-"event-message"

	.globl	"camera-enable-very-tight-squeeze-through/f"
	.type	"camera-enable-very-tight-squeeze-through/f",@function
"camera-enable-very-tight-squeeze-through/f":
	LoadStaticFloatImm	r49, 4
	LookupPointer	r0, 2
	CallFf	r0, r0, 1
	LookupPointer	r0, 1
	CallFf	r0, r0, 0
	LoadStaticFloatImm	r49, 3
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	Return	r0
"camera-enable-very-tight-squeeze-through/f_symbol_table":
	.quad	-6307733480328395657
	.quad	4822232382305105757
	.quad	6196078168405531082
	.quad	1056964608
	.quad	1028443341
"camera-enable-very-tight-squeeze-through/f_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end11:
	.size	"camera-enable-very-tight-squeeze-through/f", .Lfunc_end11-"camera-enable-very-tight-squeeze-through/f"

	.globl	"pause-hint-timer"
	.type	"pause-hint-timer",@function
"pause-hint-timer":
	Move	r50, r49
	LoadStaticU64Imm	r49, 1
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	Return	r0
"pause-hint-timer_symbol_table":
	.quad	4789555171397753330
	.quad	-6479984080625612975
"pause-hint-timer_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end12:
	.size	"pause-hint-timer", .Lfunc_end12-"pause-hint-timer"

	.globl	"from-upf-no-dt"
	.type	"from-upf-no-dt",@function
"from-upf-no-dt":
	LookupPointer	r0, 0
	CallFf	r0, r0, 0
	FDiv	r0, r49, r0
	Return	r0
"from-upf-no-dt_symbol_table":
	.quad	4835029146322721710
"from-upf-no-dt_pointer_map":
	.byte	0
.Lfunc_end13:
	.size	"from-upf-no-dt", .Lfunc_end13-"from-upf-no-dt"

	.globl	"wait-for-object-spawned-with-look"
	.type	"wait-for-object-spawned-with-look",@function
"wait-for-object-spawned-with-look":
	Move	r1, r50
	Move	r3, r49
	LookupPointer	r5, 0
	OpLogNot	r2, r51
	BranchIfNot	.LBB14_5, r51
	LookupPointer	r6, 1
	LookupPointer	r7, 2
	LookupPointer	r8, 3
	LoadU16Imm	r50, 1
	Branch	.LBB14_2
.LBB14_4:
	CallFf	r0, r6, 0
.LBB14_2:
	Move	r49, r3
	CallFf	r0, r7, 2
	Move	r4, r0
	Move	r49, r1
	CallFf	r0, r5, 1
	INotEqual	r0, r4, r0
	BranchIf	.LBB14_4, r0
	Move	r49, r3
	CallFf	r0, r8, 1
	IEqual	r0, r0, r51
	BranchIfNot	.LBB14_4, r0
	Branch	.LBB14_8
.LBB14_5:
	LoadU16Imm	r50, 1
	LookupPointer	r6, 2
	Move	r49, r3
	CallFf	r0, r6, 2
	Move	r4, r0
	Move	r49, r1
	CallFf	r0, r5, 1
	IEqual	r0, r4, r0
	BranchIf	.LBB14_8, r0
	LookupPointer	r7, 1
.LBB14_7:
	CallFf	r0, r7, 0
	Move	r49, r3
	CallFf	r0, r6, 2
	Move	r4, r0
	Move	r49, r1
	CallFf	r0, r5, 1
	IEqual	r0, r4, r0
	BranchIfNot	.LBB14_7, r0
.LBB14_8:
	Move	r0, r2
	Return	r0
"wait-for-object-spawned-with-look_symbol_table":
	.quad	-7861648282172184102
	.quad	-3047912596559836939
	.quad	-23842759317690077
	.quad	8517895422230748631
"wait-for-object-spawned-with-look_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end14:
	.size	"wait-for-object-spawned-with-look", .Lfunc_end14-"wait-for-object-spawned-with-look"

	.globl	"current-task-total-elapsed-time"
	.type	"current-task-total-elapsed-time",@function
"current-task-total-elapsed-time":
	LookupPointer	r0, 1
	CallFf	r0, r0, 0
	Move	r49, r0
	LookupPointer	r0, 0
	CallFf	r0, r0, 0
	Move	r50, r0
	LoadU16Imm	r0, 0
	BranchIfNot	.LBB15_3, r49
	BranchIfNot	.LBB15_3, r50
	LookupPointer	r0, 2
	CallFf	r0, r0, 2
.LBB15_3:
	Return	r0
"current-task-total-elapsed-time_symbol_table":
	.quad	7172296021436955284
	.quad	-2485240315990590634
	.quad	654070727927837254
"current-task-total-elapsed-time_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end15:
	.size	"current-task-total-elapsed-time", .Lfunc_end15-"current-task-total-elapsed-time"

	.globl	"sprint-bits"
	.type	"sprint-bits",@function
"sprint-bits":
	Move	r1, r49
	LookupPointer	r0, 0
	Move	r49, r50
	CallFf	r0, r0, 1
	BranchIfNot	.LBB16_3, r0
	LookupPointer	r2, 1
	Move	r49, r50
	CallFf	r0, r2, 1
	BranchIfNot	.LBB16_3, r0
	Move	r49, r50
	CallFf	r0, r2, 1
	LookupPointer	r2, 2
	Move	r49, r0
	CallFf	r0, r2, 1
	Branch	.LBB16_7
.LBB16_3:
	LoadStaticU64Imm	r0, 3
	INotEqual	r0, r50, r0
	BranchIfNot	.LBB16_4, r0
	BranchIfNot	.LBB16_4, r50
	LookupPointer	r0, 2
	Move	r49, r50
	CallFf	r0, r0, 1
.LBB16_7:
	Move	r2, r0
	Branch	.LBB16_8
.LBB16_4:
	LoadStaticPointerImm	r2, 4
.LBB16_8:
	LoadU16Imm	r8, 0
	LookupPointer	r7, 5
	LookupPointer	r9, 6
	LoadStaticPointerImm	r3, 7
	LoadStaticPointerImm	r6, 8
	LoadU16Imm	r4, 2
	LoadStaticU64Imm	r10, 10
	LoadStaticU64Imm	r11, 9
	Move	r5, r8
.LBB16_9:
	Move	r49, r4
	Move	r50, r5
	CallFf	r0, r9, 2
	OpBitAnd	r0, r0, r1
	INotEqual	r51, r0, r8
	Move	r49, r3
	Move	r50, r6
	CallFf	r0, r7, 3
	Move	r6, r0
	OpBitXor	r0, r5, r10
	ILessThan	r0, r0, r11
	IAddImm	r5, r5, 1
	BranchIf	.LBB16_9, r0
	LoadStaticPointerImm	r49, 11
	Move	r50, r2
	Move	r51, r6
	CallFf	r0, r7, 3
	Return	r0
"sprint-bits_symbol_table":
	.quad	1313740198704887414
	.quad	3741915750828142429
	.quad	-3311351115081502130
	.quad	3249880500923765431
	.quad	56852
	.quad	6072285182812319085
	.quad	3204416655431713234
	.quad	56565
	.quad	56512
	.quad	-9223372036854775778
	.quad	-9223372036854775808
	.quad	56586
"sprint-bits_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	0
	.byte	0
	.byte	1
	.byte	1
	.byte	0
	.byte	0
	.byte	1
.Lfunc_end16:
	.size	"sprint-bits", .Lfunc_end16-"sprint-bits"

	.globl	"flashback-task?"
	.type	"flashback-task?",@function
"flashback-task?":
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	BranchIfNot	.LBB17_1, r0
	LoadU16Imm	r1, 480
	IAdd	r0, r0, r1
	LoadU32	r0, r0
	LoadU16Imm	r1, 1
	IEqual	r0, r0, r1
	Return	r0
.LBB17_1:
	LoadU16Imm	r0, 0
	Return	r0
"flashback-task?_symbol_table":
	.quad	4627505823601477936
"flashback-task?_pointer_map":
	.byte	0
.Lfunc_end17:
	.size	"flashback-task?", .Lfunc_end17-"flashback-task?"

	.globl	"wait-simple-npc-move-to-spawner"
	.type	"wait-simple-npc-move-to-spawner",@function
"wait-simple-npc-move-to-spawner":
	Move	r55, r54
	LoadU16Imm	r54, 1
	LookupPointer	r0, 0
	CallFf	r0, r0, 7
	Return	r0
"wait-simple-npc-move-to-spawner_symbol_table":
	.quad	-8553643611204354492
"wait-simple-npc-move-to-spawner_pointer_map":
	.byte	0
.Lfunc_end18:
	.size	"wait-simple-npc-move-to-spawner", .Lfunc_end18-"wait-simple-npc-move-to-spawner"

	.globl	"get-bit"
	.type	"get-bit",@function
"get-bit":
	LoadU16Imm	r0, 1
	IntAsh	r0, r0, r50
	OpBitAnd	r0, r0, r49
	LoadU16Imm	r1, 0
	INotEqual	r0, r0, r1
	Return	r0
.Lfunc_end19:
	.size	"get-bit", .Lfunc_end19-"get-bit"

	.globl	"box-event-arg-string"
	.type	"box-event-arg-string",@function
"box-event-arg-string":
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	LoadU16Imm	r49, 19
	LookupPointer	r1, 0
	Move	r50, r0
	CallFf	r0, r1, 2
	Return	r0
"box-event-arg-string_symbol_table":
	.quad	3684810824690217768
	.quad	-7040857727560215700
"box-event-arg-string_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end20:
	.size	"box-event-arg-string", .Lfunc_end20-"box-event-arg-string"

	.globl	"print-vector2"
	.type	"print-vector2",@function
"print-vector2":
	LoadU16Imm	r0, 0
	Return	r0
.Lfunc_end21:
	.size	"print-vector2", .Lfunc_end21-"print-vector2"

	.globl	"set-shader-instance-incandescence-strength"
	.type	"set-shader-instance-incandescence-strength",@function
"set-shader-instance-incandescence-strength":
	Move	r51, r50
	LoadStaticU64Imm	r50, 1
	LookupPointer	r0, 0
	CallFf	r0, r0, 3
	Return	r0
"set-shader-instance-incandescence-strength_symbol_table":
	.quad	-2085764591387011864
	.quad	-3186304195179347301
"set-shader-instance-incandescence-strength_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end22:
	.size	"set-shader-instance-incandescence-strength", .Lfunc_end22-"set-shader-instance-incandescence-strength"

	.globl	"get-bounding-sphere-center"
	.type	"get-bounding-sphere-center",@function
"get-bounding-sphere-center":
	Move	r0, r49
	LoadFloat	r49, r0
	BranchIfNot	.LBB23_2, r50
	LoadU16Imm	r50, 0
	Branch	.LBB23_3
.LBB23_2:
	IAddImm	r1, r0, 4
	LoadFloat	r50, r1
.LBB23_3:
	IAddImm	r0, r0, 8
	LoadFloat	r51, r0
	LoadU16Imm	r52, 0
	LookupPointer	r0, 0
	CallFf	r0, r0, 4
	Return	r0
"get-bounding-sphere-center_symbol_table":
	.quad	-8586803022565429251
"get-bounding-sphere-center_pointer_map":
	.byte	0
.Lfunc_end23:
	.size	"get-bounding-sphere-center", .Lfunc_end23-"get-bounding-sphere-center"

	.globl	"darray-extend"
	.type	"darray-extend",@function
"darray-extend":
	Move	r1, r50
	Move	r2, r49
	LookupPointer	r0, 1
	Move	r49, r1
	CallFf	r0, r0, 1
	Move	r3, r0
	LookupPointer	r0, 0
	CallFf	r0, r0, 0
	LoadU16Imm	r0, 1
	ILessThan	r0, r3, r0
	BranchIf	.LBB24_3, r0
	LoadU16Imm	r51, 0
	LookupPointer	r5, 2
	LookupPointer	r6, 3
	Move	r4, r51
.LBB24_2:
	Move	r49, r1
	Move	r50, r4
	CallFf	r0, r6, 2
	Move	r49, r2
	Move	r50, r0
	CallFf	r0, r5, 3
	IAddImm	r4, r4, 1
	ILessThan	r0, r4, r3
	BranchIf	.LBB24_2, r0
.LBB24_3:
	LookupPointer	r0, 4
	CallFf	r0, r0, 0
	Return	r0
"darray-extend_symbol_table":
	.quad	3423406688794319677
	.quad	4013010305452811590
	.quad	3321216610294482167
	.quad	-2664920944689591574
	.quad	-5673227674328399655
"darray-extend_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end24:
	.size	"darray-extend", .Lfunc_end24-"darray-extend"

	.globl	"subtitle-force-show"
	.type	"subtitle-force-show",@function
"subtitle-force-show":
	LoadU16Imm	r52, 1
	LookupPointer	r0, 0
	CallFf	r0, r0, 4
	Return	r0
"subtitle-force-show_symbol_table":
	.quad	-7089072539499493873
"subtitle-force-show_pointer_map":
	.byte	0
.Lfunc_end25:
	.size	"subtitle-force-show", .Lfunc_end25-"subtitle-force-show"

	.globl	"wait-simple-npc-move-to-entry-frame"
	.type	"wait-simple-npc-move-to-entry-frame",@function
"wait-simple-npc-move-to-entry-frame":
	Move	r1, r54
	Move	r2, r53
	Move	r3, r52
	Move	r4, r49
	LoadU16Imm	r52, 0
	LoadU16Imm	r53, 0
	LoadStaticU64Imm	r54, 3
	LookupPointer	r0, 2
	CallFf	r0, r0, 6
	LookupPointer	r5, 1
	Move	r49, r0
	CallFf	r0, r5, 1
	LoadU16Imm	r54, 1
	LookupPointer	r5, 0
	Move	r49, r4
	Move	r50, r0
	Move	r51, r3
	Move	r52, r2
	Move	r53, r1
	CallFf	r0, r5, 7
	Return	r0
"wait-simple-npc-move-to-entry-frame_symbol_table":
	.quad	-6869446841486065293
	.quad	-3881025521939511168
	.quad	1932324078104646333
	.quad	432021034919055241
"wait-simple-npc-move-to-entry-frame_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end26:
	.size	"wait-simple-npc-move-to-entry-frame", .Lfunc_end26-"wait-simple-npc-move-to-entry-frame"

	.globl	"set-shader-instance-uv-offset"
	.type	"set-shader-instance-uv-offset",@function
"set-shader-instance-uv-offset":
	Move	r1, r51
	Move	r51, r50
	LoadStaticU64Imm	r50, 2
	LookupPointer	r2, 0
	CallFf	r0, r2, 3
	LoadStaticU64Imm	r50, 1
	Move	r51, r1
	CallFf	r0, r2, 3
	Return	r0
"set-shader-instance-uv-offset_symbol_table":
	.quad	-2085764591387011864
	.quad	3597406555356376597
	.quad	3597403256821491964
"set-shader-instance-uv-offset_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end27:
	.size	"set-shader-instance-uv-offset", .Lfunc_end27-"set-shader-instance-uv-offset"

	.globl	"task-subnode-complete?"
	.type	"task-subnode-complete?",@function
"task-subnode-complete?":
	IAddImm	r0, r49, 96
	LoadU32	r0, r0
	LoadU16Imm	r1, 4
	IEqual	r0, r0, r1
	Return	r0
.Lfunc_end28:
	.size	"task-subnode-complete?", .Lfunc_end28-"task-subnode-complete?"

	.globl	"get-object-speed"
	.type	"get-object-speed",@function
"get-object-speed":
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	LookupPointer	r1, 0
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
"get-object-speed_symbol_table":
	.quad	-4487271785256571635
	.quad	-1905058307015679353
"get-object-speed_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end29:
	.size	"get-object-speed", .Lfunc_end29-"get-object-speed"

	.globl	"current-task-is-test?"
	.type	"current-task-is-test?",@function
"current-task-is-test?":
	LookupPointer	r0, 2
	CallFf	r0, r0, 0
	Move	r49, r0
	LookupPointer	r0, 1
	CallFf	r0, r0, 0
	LookupPointer	r1, 0
	Move	r50, r0
	CallFf	r0, r1, 2
	BranchIfNot	.LBB30_1, r0
	LookupPointer	r1, 3
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
.LBB30_1:
	LoadU16Imm	r0, 0
	Return	r0
"current-task-is-test?_symbol_table":
	.quad	4627505823601477936
	.quad	7172296021436955284
	.quad	-2485240315990590634
	.quad	-8607372249571315066
"current-task-is-test?_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end30:
	.size	"current-task-is-test?", .Lfunc_end30-"current-task-is-test?"

	.globl	"wait-simple-npc-move-to-action-pack"
	.type	"wait-simple-npc-move-to-action-pack",@function
"wait-simple-npc-move-to-action-pack":
	Move	r53, r52
	LoadU16Imm	r52, 1
	LookupPointer	r0, 0
	CallFf	r0, r0, 5
	Return	r0
"wait-simple-npc-move-to-action-pack_symbol_table":
	.quad	1238128838624674224
"wait-simple-npc-move-to-action-pack_pointer_map":
	.byte	0
.Lfunc_end31:
	.size	"wait-simple-npc-move-to-action-pack", .Lfunc_end31-"wait-simple-npc-move-to-action-pack"

	.globl	"simple-npc-transfer-to-spline"
	.type	"simple-npc-transfer-to-spline",@function
"simple-npc-transfer-to-spline":
	LookupPointer	r0, 0
	CallFf	r0, r0, 3
	Return	r0
"simple-npc-transfer-to-spline_symbol_table":
	.quad	-2272449730465418826
"simple-npc-transfer-to-spline_pointer_map":
	.byte	0
.Lfunc_end32:
	.size	"simple-npc-transfer-to-spline", .Lfunc_end32-"simple-npc-transfer-to-spline"

	.globl	"get-bounding-sphere-radius"
	.type	"get-bounding-sphere-radius",@function
"get-bounding-sphere-radius":
	IAddImm	r0, r49, 12
	LoadU32	r0, r0
	Return	r0
.Lfunc_end33:
	.size	"get-bounding-sphere-radius", .Lfunc_end33-"get-bounding-sphere-radius"

	.globl	"flashback-task-node?"
	.type	"flashback-task-node?",@function
"flashback-task-node?":
	BranchIfNot	.LBB34_1, r49
	LoadU16Imm	r0, 480
	IAdd	r0, r49, r0
	LoadU32	r0, r0
	LoadU16Imm	r1, 1
	IEqual	r0, r0, r1
	Return	r0
.LBB34_1:
	LoadU16Imm	r0, 0
	Return	r0
.Lfunc_end34:
	.size	"flashback-task-node?", .Lfunc_end34-"flashback-task-node?"

	.globl	"set-bit-uint64"
	.type	"set-bit-uint64",@function
"set-bit-uint64":
	LoadU16Imm	r0, 1
	IntAsh	r0, r0, r50
	OpLogNot	r1, r51
	BranchIf	.LBB35_1, r1
	OpBitOr	r0, r0, r49
	Return	r0
.LBB35_1:
	OpBitNot	r0, r0
	OpBitAnd	r0, r49, r0
	Return	r0
.Lfunc_end35:
	.size	"set-bit-uint64", .Lfunc_end35-"set-bit-uint64"

	.globl	"find-index-in-symbol-array-broken"
	.type	"find-index-in-symbol-array-broken",@function
"find-index-in-symbol-array-broken":
	LoadI32	r1, r50
	LoadU16Imm	r0, 1
	ILessThan	r0, r1, r0
	BranchIf	.LBB36_1, r0
	LoadU16Imm	r2, 0
	LoadStaticU64Imm	r4, 0
	IAddImm	r0, r50, 8
	LoadU64	r3, r0
	Branch	.LBB36_3
.LBB36_5:
	IAddImm	r2, r2, 1
	ILessThan	r5, r2, r1
	Move	r4, r0
	BranchIfNot	.LBB36_6, r5
.LBB36_3:
	IMulImm	r0, r2, 8
	IAdd	r0, r3, r0
	LoadU64	r0, r0
	IEqual	r5, r0, r49
	Move	r0, r2
	BranchIf	.LBB36_5, r5
	Move	r0, r4
	Branch	.LBB36_5
.LBB36_6:
	Return	r0
.LBB36_1:
	LoadStaticU64Imm	r0, 0
	Return	r0
"find-index-in-symbol-array-broken_symbol_table":
	.quad	-1
"find-index-in-symbol-array-broken_pointer_map":
	.byte	0
.Lfunc_end36:
	.size	"find-index-in-symbol-array-broken", .Lfunc_end36-"find-index-in-symbol-array-broken"

	.globl	"wait-for-want-load-levels-fully-spawned"
	.type	"wait-for-want-load-levels-fully-spawned",@function
"wait-for-want-load-levels-fully-spawned":
	Return	r0
.Lfunc_end37:
	.size	"wait-for-want-load-levels-fully-spawned", .Lfunc_end37-"wait-for-want-load-levels-fully-spawned"

	.globl	"regular-timeline-task?"
	.type	"regular-timeline-task?",@function
"regular-timeline-task?":
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	BranchIfNot	.LBB38_1, r0
	LoadU16Imm	r1, 480
	IAdd	r0, r0, r1
	LoadU32	r0, r0
	OpLogNot	r0, r0
	Return	r0
.LBB38_1:
	LoadU16Imm	r0, 0
	Return	r0
"regular-timeline-task?_symbol_table":
	.quad	4627505823601477936
"regular-timeline-task?_pointer_map":
	.byte	0
.Lfunc_end38:
	.size	"regular-timeline-task?", .Lfunc_end38-"regular-timeline-task?"

	.globl	"ddict-print"
	.type	"ddict-print",@function
"ddict-print":
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	Move	r1, r0
	LookupPointer	r0, 0
	CallFf	r0, r0, 0
	LoadU16Imm	r0, 1
	ILessThan	r0, r1, r0
	BranchIf	.LBB39_3, r0
	LoadU16Imm	r50, 0
	LookupPointer	r2, 2
.LBB39_2:
	CallFf	r0, r2, 2
	IAddImm	r50, r50, 1
	ILessThan	r0, r50, r1
	BranchIf	.LBB39_2, r0
.LBB39_3:
	LookupPointer	r0, 3
	CallFf	r0, r0, 0
	Return	r0
"ddict-print_symbol_table":
	.quad	3423406688794319677
	.quad	2736978041656912235
	.quad	-2290456712816956831
	.quad	-5673227674328399655
"ddict-print_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end39:
	.size	"ddict-print", .Lfunc_end39-"ddict-print"

	.globl	"empty-string?"
	.type	"empty-string?",@function
"empty-string?":
	LoadStaticPointerImm	r50, 1
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	Return	r0
"empty-string?_symbol_table":
	.quad	-3711322450294902906
	.quad	56512
"empty-string?_pointer_map":
	.byte	0
	.byte	1
.Lfunc_end40:
	.size	"empty-string?", .Lfunc_end40-"empty-string?"

	.globl	"print-bits"
	.type	"print-bits",@function
"print-bits":
	LoadU16Imm	r0, 0
	Return	r0
.Lfunc_end41:
	.size	"print-bits", .Lfunc_end41-"print-bits"

	.globl	"task-play-fadeout!"
	.type	"task-play-fadeout!",@function
"task-play-fadeout!":
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	BranchIfNot	.LBB42_1, r0
	LookupPointer	r1, 1
	Move	r49, r0
	Move	r50, r51
	Move	r51, r52
	Move	r52, r54
	CallFf	r0, r1, 5
	Return	r0
.LBB42_1:
	LoadU16Imm	r0, 0
	Return	r0
"task-play-fadeout!_symbol_table":
	.quad	4627505823601477936
	.quad	4701419364567053568
"task-play-fadeout!_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end42:
	.size	"task-play-fadeout!", .Lfunc_end42-"task-play-fadeout!"

	.globl	"box-event-arg-int32"
	.type	"box-event-arg-int32",@function
"box-event-arg-int32":
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	LoadU16Imm	r49, 2
	LookupPointer	r1, 0
	Move	r50, r0
	CallFf	r0, r1, 2
	Return	r0
"box-event-arg-int32_symbol_table":
	.quad	3684810824690217768
	.quad	-4700555454704927757
"box-event-arg-int32_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end43:
	.size	"box-event-arg-int32", .Lfunc_end43-"box-event-arg-int32"

	.globl	"task-or-continue-active?"
	.type	"task-or-continue-active?",@function
"task-or-continue-active?":
	Move	r1, r51
	Move	r2, r50
	Move	r3, r49
	LoadStaticU64Imm	r4, 2
	LoadU16Imm	r52, 1
	LookupPointer	r0, 1
	Move	r50, r4
	Move	r51, r52
	CallFf	r0, r0, 4
	Move	r5, r0
	LookupPointer	r0, 0
	Move	r49, r2
	Move	r50, r1
	Move	r51, r4
	Move	r53, r52
	CallFf	r0, r0, 5
	Move	r4, r0
	BranchIfNot	.LBB44_5, r5
	LookupPointer	r0, 3
	Move	r49, r5
	CallFf	r0, r0, 1
	Move	r3, r0
	BranchIfNot	.LBB44_6, r4
.LBB44_2:
	LookupPointer	r0, 6
	Move	r49, r4
	CallFf	r0, r0, 1
	Move	r50, r0
	LookupPointer	r0, 5
	CallFf	r0, r0, 1
	Move	r1, r0
	BranchIfNot	.LBB44_7, r1
	LookupPointer	r0, 7
	Move	r49, r3
	CallFf	r0, r0, 2
	BranchIfNot	.LBB44_10, r0
	LookupPointer	r2, 8
	Move	r49, r0
	CallFf	r0, r2, 1
	IEqual	r0, r0, r1
	Return	r0
.LBB44_5:
	LoadStaticU64Imm	r50, 2
	LookupPointer	r0, 4
	Move	r49, r3
	CallFf	r0, r0, 2
	Move	r3, r0
	BranchIf	.LBB44_2, r4
.LBB44_6:
	LoadStaticU64Imm	r51, 2
	LookupPointer	r0, 9
	Move	r49, r2
	Move	r50, r1
	CallFf	r0, r0, 3
	Move	r50, r0
.LBB44_7:
	LookupPointer	r0, 7
	Move	r49, r3
	CallFf	r0, r0, 2
	BranchIfNot	.LBB44_10, r0
	LookupPointer	r1, 10
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
.LBB44_10:
	LoadU16Imm	r0, 0
	Return	r0
"task-or-continue-active?_symbol_table":
	.quad	-3513786953354759377
	.quad	8164313781786671539
	.quad	3249880500923765431
	.quad	475792274027315025
	.quad	-2218206844643806166
	.quad	-7080778341598050516
	.quad	-3530598740559159857
	.quad	4627505823601477936
	.quad	8528085549934399215
	.quad	-8479599307812291032
	.quad	1014617123432502549
"task-or-continue-active?_pointer_map":
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
.Lfunc_end44:
	.size	"task-or-continue-active?", .Lfunc_end44-"task-or-continue-active?"

	.globl	"random-float-range"
	.type	"random-float-range",@function
"random-float-range":
	Move	r52, r50
	Move	r1, r49
	LookupPointer	r0, 1
	Move	r49, r51
	CallFf	r0, r0, 1
	LoadU16Imm	r49, 0
	LoadStaticFloatImm	r50, 2
	LookupPointer	r2, 0
	Move	r51, r1
	Move	r53, r0
	CallFf	r0, r2, 5
	Return	r0
"random-float-range_symbol_table":
	.quad	6895426483596981631
	.quad	-3265080734848834707
	.quad	1065353216
"random-float-range_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end45:
	.size	"random-float-range", .Lfunc_end45-"random-float-range"

	.globl	"print-symbol"
	.type	"print-symbol",@function
"print-symbol":
	LoadU16Imm	r0, 0
	Return	r0
.Lfunc_end46:
	.size	"print-symbol", .Lfunc_end46-"print-symbol"

	.globl	"wait-stop-animating-by-layer-name"
	.type	"wait-stop-animating-by-layer-name",@function
"wait-stop-animating-by-layer-name":
	LookupPointer	r0, 1
	CallFf	r0, r0, 3
	LookupPointer	r1, 0
	CallFf	r0, r1, 0
	CallFf	r0, r1, 0
	Return	r0
"wait-stop-animating-by-layer-name_symbol_table":
	.quad	-3047912596559836939
	.quad	6918975437027605210
"wait-stop-animating-by-layer-name_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end47:
	.size	"wait-stop-animating-by-layer-name", .Lfunc_end47-"wait-stop-animating-by-layer-name"

	.globl	"get-object-relative-speed"
	.type	"get-object-relative-speed",@function
"get-object-relative-speed":
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	LookupPointer	r1, 0
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
"get-object-relative-speed_symbol_table":
	.quad	-4487271785256571635
	.quad	-653718488554981094
"get-object-relative-speed_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end48:
	.size	"get-object-relative-speed", .Lfunc_end48-"get-object-relative-speed"

	.globl	pow
	.type	pow,@function
pow:
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	Return	r0
pow_symbol_table:
	.quad	3204416655431713234
pow_pointer_map:
	.byte	0
.Lfunc_end49:
	.size	pow, .Lfunc_end49-pow

	.globl	"rand-range"
	.type	"rand-range",@function
"rand-range":
	Move	r1, r49
	LoadU16Imm	r49, 0
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	FSub	r2, r50, r1
	FMul	r0, r2, r0
	FAdd	r0, r0, r1
	Return	r0
"rand-range_symbol_table":
	.quad	-3265080734848834707
"rand-range_pointer_map":
	.byte	0
.Lfunc_end50:
	.size	"rand-range", .Lfunc_end50-"rand-range"

	.globl	"cin-lerp-frames"
	.type	"cin-lerp-frames",@function
"cin-lerp-frames":
	Move	r1, r49
	LoadU16Imm	r49, 0
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	LoadU16Imm	r51, 0
	LoadStaticFloatImm	r52, 2
	LookupPointer	r2, 0
	Move	r49, r1
	Move	r53, r0
	CallFf	r0, r2, 5
	Return	r0
"cin-lerp-frames_symbol_table":
	.quad	6895426483596981631
	.quad	-6493350602403076389
	.quad	1065353216
"cin-lerp-frames_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end51:
	.size	"cin-lerp-frames", .Lfunc_end51-"cin-lerp-frames"

	.globl	"task-complete?"
	.type	"task-complete?",@function
"task-complete?":
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	Move	r1, r0
	BranchIfNot	.LBB52_1, r1
	LookupPointer	r0, 1
	CallFf	r0, r0, 0
	BranchIfNot	.LBB52_4, r0
	LookupPointer	r0, 2
	CallFf	r0, r0, 2
	Return	r0
.LBB52_1:
	LoadU16Imm	r0, 0
	Return	r0
.LBB52_4:
	IAddImm	r0, r1, 96
	LoadU32	r0, r0
	LoadU16Imm	r1, 4
	IEqual	r0, r0, r1
	Return	r0
"task-complete?_symbol_table":
	.quad	4627505823601477936
	.quad	-3098704140420144833
	.quad	-9020003840458003388
"task-complete?_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end52:
	.size	"task-complete?", .Lfunc_end52-"task-complete?"

	.globl	"task-complete!"
	.type	"task-complete!",@function
"task-complete!":
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	BranchIfNot	.LBB53_1, r0
	LookupPointer	r1, 1
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
.LBB53_1:
	LoadU16Imm	r0, 0
	Return	r0
"task-complete!_symbol_table":
	.quad	4627505823601477936
	.quad	-4779101365983111435
"task-complete!_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end53:
	.size	"task-complete!", .Lfunc_end53-"task-complete!"

	.globl	"box-event-arg-boolean"
	.type	"box-event-arg-boolean",@function
"box-event-arg-boolean":
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	LoadU16Imm	r49, 1
	LookupPointer	r1, 0
	Move	r50, r0
	CallFf	r0, r1, 2
	Return	r0
"box-event-arg-boolean_symbol_table":
	.quad	3684810824690217768
	.quad	-696762128607962741
"box-event-arg-boolean_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end54:
	.size	"box-event-arg-boolean", .Lfunc_end54-"box-event-arg-boolean"

	.globl	"simple-npc-move-to-entry"
	.type	"simple-npc-move-to-entry",@function
"simple-npc-move-to-entry":
	Move	r1, r54
	Move	r2, r53
	Move	r3, r52
	Move	r4, r49
	LookupPointer	r0, 0
	Move	r49, r51
	CallFf	r0, r0, 1
	BranchIfNot	.LBB55_2, r0
	LookupPointer	r0, 1
	Move	r49, r51
	CallFf	r0, r0, 1
	Branch	.LBB55_3
.LBB55_2:
	LoadU16Imm	r0, 1
	LookupPointer	r6, 2
	Move	r49, r51
	Move	r5, r50
	Move	r50, r0
	CallFf	r0, r6, 2
	Move	r50, r5
.LBB55_3:
	LoadU16Imm	r5, 0
	LoadU16Imm	r53, 0
	LoadStaticU64Imm	r54, 6
	LookupPointer	r6, 5
	Move	r49, r4
	Move	r51, r0
	Move	r52, r5
	CallFf	r0, r6, 6
	LookupPointer	r6, 4
	Move	r49, r0
	CallFf	r0, r6, 1
	LookupPointer	r6, 3
	Move	r49, r4
	Move	r50, r0
	Move	r51, r3
	Move	r52, r2
	Move	r53, r1
	Move	r54, r5
	CallFf	r0, r6, 7
	Return	r0
"simple-npc-move-to-entry_symbol_table":
	.quad	-6870362608876642110
	.quad	61417594283941650
	.quad	-1563483893137887143
	.quad	-6869446841486065293
	.quad	-3881025521939511168
	.quad	1932324078104646333
	.quad	432021034919055241
"simple-npc-move-to-entry_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end55:
	.size	"simple-npc-move-to-entry", .Lfunc_end55-"simple-npc-move-to-entry"

	.globl	"box-event-arg-uint64"
	.type	"box-event-arg-uint64",@function
"box-event-arg-uint64":
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	LoadU16Imm	r49, 5
	LookupPointer	r1, 0
	Move	r50, r0
	CallFf	r0, r1, 2
	Return	r0
"box-event-arg-uint64_symbol_table":
	.quad	3684810824690217768
	.quad	-3476174700326899881
"box-event-arg-uint64_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end56:
	.size	"box-event-arg-uint64", .Lfunc_end56-"box-event-arg-uint64"

	.globl	"joypad-stick-pulled-away?"
	.type	"joypad-stick-pulled-away?",@function
"joypad-stick-pulled-away?":
	LoadStaticFloatImm	r53, 1
	LookupPointer	r0, 0
	CallFf	r0, r0, 5
	Return	r0
"joypad-stick-pulled-away?_symbol_table":
	.quad	7657644897092222194
	.quad	1127481344
"joypad-stick-pulled-away?_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end57:
	.size	"joypad-stick-pulled-away?", .Lfunc_end57-"joypad-stick-pulled-away?"

	.globl	"cloth-disable-collider"
	.type	"cloth-disable-collider",@function
"cloth-disable-collider":
	Move	r1, r49
	LoadU16Imm	r2, 7
	LookupPointer	r4, 1
	Move	r49, r2
	CallFf	r0, r4, 2
	Move	r3, r0
	LoadU16Imm	r49, 1
	LoadU16Imm	r50, 0
	CallFf	r0, r4, 2
	Move	r52, r0
	Move	r49, r2
	Move	r50, r51
	CallFf	r0, r4, 2
	LoadStaticU64Imm	r49, 2
	LookupPointer	r2, 0
	Move	r50, r1
	Move	r51, r3
	Move	r53, r0
	CallFf	r0, r2, 5
	Return	r0
"cloth-disable-collider_symbol_table":
	.quad	4789555171397753330
	.quad	3684810824690217768
	.quad	-5115115189087745418
"cloth-disable-collider_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end58:
	.size	"cloth-disable-collider", .Lfunc_end58-"cloth-disable-collider"

	.globl	"camera-disable-animated"
	.type	"camera-disable-animated",@function
"camera-disable-animated":
	LoadStaticU64Imm	r49, 1
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	Return	r0
"camera-disable-animated_symbol_table":
	.quad	2188720273919511335
	.quad	3249880500923765431
"camera-disable-animated_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end59:
	.size	"camera-disable-animated", .Lfunc_end59-"camera-disable-animated"

	.globl	"valid-symbol-var?"
	.type	"valid-symbol-var?",@function
"valid-symbol-var?":
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	LoadU16Imm	r1, 0
	INotEqual	r0, r0, r1
	Return	r0
"valid-symbol-var?_symbol_table":
	.quad	-1136051897790519866
"valid-symbol-var?_pointer_map":
	.byte	0
.Lfunc_end60:
	.size	"valid-symbol-var?", .Lfunc_end60-"valid-symbol-var?"

	.globl	"task-get-active-continue"
	.type	"task-get-active-continue",@function
"task-get-active-continue":
	LookupPointer	r0, 0
	CallFf	r0, r0, 0
	BranchIfNot	.LBB61_1, r0
	LookupPointer	r1, 1
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
.LBB61_1:
	LoadU16Imm	r0, 0
	Return	r0
"task-get-active-continue_symbol_table":
	.quad	8046519909153555413
	.quad	8528085549934399215
"task-get-active-continue_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end61:
	.size	"task-get-active-continue", .Lfunc_end61-"task-get-active-continue"

	.globl	"print-bits-uint64"
	.type	"print-bits-uint64",@function
"print-bits-uint64":
	LoadU16Imm	r0, 0
	Return	r0
.Lfunc_end62:
	.size	"print-bits-uint64", .Lfunc_end62-"print-bits-uint64"

	.globl	"box-event-arg-float"
	.type	"box-event-arg-float",@function
"box-event-arg-float":
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	LoadU16Imm	r49, 6
	LookupPointer	r1, 0
	Move	r50, r0
	CallFf	r0, r1, 2
	Return	r0
"box-event-arg-float_symbol_table":
	.quad	3684810824690217768
	.quad	-7227679359147913623
"box-event-arg-float_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end63:
	.size	"box-event-arg-float", .Lfunc_end63-"box-event-arg-float"

	.globl	"camera-abandon-animated"
	.type	"camera-abandon-animated",@function
"camera-abandon-animated":
	LoadStaticU64Imm	r49, 1
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	Return	r0
"camera-abandon-animated_symbol_table":
	.quad	261166031521791970
	.quad	3249880500923765431
"camera-abandon-animated_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end64:
	.size	"camera-abandon-animated", .Lfunc_end64-"camera-abandon-animated"

	.globl	"get-int64"
	.type	"get-int64",@function
"get-int64":
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	Return	r0
"get-int64_symbol_table":
	.quad	-571212388772813384
"get-int64_pointer_map":
	.byte	0
.Lfunc_end65:
	.size	"get-int64", .Lfunc_end65-"get-int64"

	.globl	"conversation-interactable-enable"
	.type	"conversation-interactable-enable",@function
"conversation-interactable-enable":
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	BranchIfNot	.LBB66_1, r50
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	Return	r0
.LBB66_1:
	LoadU16Imm	r0, 0
	Return	r0
"conversation-interactable-enable_symbol_table":
	.quad	4010924026564459985
	.quad	-6803722351039902803
"conversation-interactable-enable_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end66:
	.size	"conversation-interactable-enable", .Lfunc_end66-"conversation-interactable-enable"

	.globl	"vector-set-magnitude"
	.type	"vector-set-magnitude",@function
"vector-set-magnitude":
	Move	r1, r49
	LoadU16Imm	r53, 0
	LookupPointer	r0, 1
	Move	r49, r50
	Move	r50, r53
	CallFf	r0, r0, 2
	IAddImm	r2, r0, 4
	LoadFloat	r2, r2
	LoadFloat	r3, r0
	FMul	r49, r3, r1
	FMul	r50, r2, r1
	IAddImm	r0, r0, 8
	LoadFloat	r0, r0
	FMul	r51, r0, r1
	LoadStaticFloatImm	r52, 2
	LookupPointer	r0, 0
	CallFf	r0, r0, 5
	Return	r0
"vector-set-magnitude_symbol_table":
	.quad	-1744966098031674466
	.quad	3026091680885120756
	.quad	1065353216
"vector-set-magnitude_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end67:
	.size	"vector-set-magnitude", .Lfunc_end67-"vector-set-magnitude"

	.globl	"dot-product"
	.type	"dot-product",@function
"dot-product":
	LoadFloat	r0, r50
	LoadFloat	r1, r49
	FMul	r0, r1, r0
	IAddImm	r1, r50, 4
	LoadFloat	r1, r1
	IAddImm	r2, r49, 4
	LoadFloat	r2, r2
	FMul	r1, r2, r1
	FAdd	r0, r0, r1
	IAddImm	r1, r50, 8
	LoadFloat	r1, r1
	IAddImm	r2, r49, 8
	LoadFloat	r2, r2
	FMul	r1, r2, r1
	FAdd	r0, r0, r1
	Return	r0
.Lfunc_end68:
	.size	"dot-product", .Lfunc_end68-"dot-product"

	.globl	"change-render-setting-value-vector"
	.type	"change-render-setting-value-vector",@function
"change-render-setting-value-vector":
	Move	r2, r51
	Move	r3, r50
	Move	r1, r49
	LoadStaticPointerImm	r50, 3
	LookupPointer	r5, 2
	Move	r49, r3
	CallFf	r0, r5, 2
	LoadFloat	r51, r2
	LookupPointer	r4, 1
	Move	r49, r1
	Move	r50, r0
	CallFf	r0, r4, 3
	LoadStaticPointerImm	r50, 0
	Move	r49, r3
	CallFf	r0, r5, 2
	IAddImm	r6, r2, 8
	LoadFloat	r51, r6
	Move	r49, r1
	Move	r50, r0
	CallFf	r0, r4, 3
	BranchIfNot	.LBB69_2, r52
	LoadU16Imm	r0, 0
	Return	r0
.LBB69_2:
	LoadStaticPointerImm	r50, 4
	Move	r49, r3
	CallFf	r0, r5, 2
	IAddImm	r2, r2, 4
	LoadFloat	r51, r2
	Move	r49, r1
	Move	r50, r0
	CallFf	r0, r4, 3
	Return	r0
"change-render-setting-value-vector_symbol_table":
	.quad	56644
	.quad	8318787222681770230
	.quad	-6109586778408437506
	.quad	56638
	.quad	56641
"change-render-setting-value-vector_pointer_map":
	.byte	1
	.byte	0
	.byte	0
	.byte	1
	.byte	1
.Lfunc_end69:
	.size	"change-render-setting-value-vector", .Lfunc_end69-"change-render-setting-value-vector"

	.globl	"task-not-complete?"
	.type	"task-not-complete?",@function
"task-not-complete?":
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	Move	r1, r0
	BranchIfNot	.LBB70_1, r1
	LookupPointer	r0, 1
	CallFf	r0, r0, 0
	BranchIfNot	.LBB70_4, r0
	LookupPointer	r0, 2
	CallFf	r0, r0, 2
	OpLogNot	r0, r0
	Return	r0
.LBB70_1:
	LoadU16Imm	r0, 1
	Return	r0
.LBB70_4:
	IAddImm	r0, r1, 96
	LoadU32	r0, r0
	LoadU16Imm	r1, 4
	INotEqual	r0, r0, r1
	Return	r0
"task-not-complete?_symbol_table":
	.quad	4627505823601477936
	.quad	-3098704140420144833
	.quad	-9020003840458003388
"task-not-complete?_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end70:
	.size	"task-not-complete?", .Lfunc_end70-"task-not-complete?"

	.globl	"transform-world-to-localspace"
	.type	"transform-world-to-localspace",@function
"transform-world-to-localspace":
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	Return	r0
"transform-world-to-localspace_symbol_table":
	.quad	3523432566325309160
"transform-world-to-localspace_pointer_map":
	.byte	0
.Lfunc_end71:
	.size	"transform-world-to-localspace", .Lfunc_end71-"transform-world-to-localspace"

	.globl	"timer-to-blend-value"
	.type	"timer-to-blend-value",@function
"timer-to-blend-value":
	Move	r1, r50
	LoadStaticU64Imm	r50, 2
	LookupPointer	r0, 1
	CallFf	r0, r0, 2
	LoadU16Imm	r50, 0
	LookupPointer	r2, 0
	Move	r49, r1
	Move	r53, r0
	CallFf	r0, r2, 5
	Return	r0
"timer-to-blend-value_symbol_table":
	.quad	6895426483596981631
	.quad	-7712151158057785724
	.quad	3249880500923765431
"timer-to-blend-value_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end72:
	.size	"timer-to-blend-value", .Lfunc_end72-"timer-to-blend-value"

	.globl	"cinematic-get-current-time"
	.type	"cinematic-get-current-time",@function
"cinematic-get-current-time":
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	LoadStaticFloatImm	r1, 1
	FDiv	r0, r0, r1
	Return	r0
"cinematic-get-current-time_symbol_table":
	.quad	-6493350602403076389
	.quad	1106247680
"cinematic-get-current-time_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end73:
	.size	"cinematic-get-current-time", .Lfunc_end73-"cinematic-get-current-time"

	.globl	"darray-find-remove"
	.type	"darray-find-remove",@function
"darray-find-remove":
	LookupPointer	r0, 0
	CallFf	r0, r0, 3
	LoadU16Imm	r1, 0
	ILessThan	r2, r0, r1
	BranchIf	.LBB74_2, r2
	LookupPointer	r1, 1
	Move	r50, r0
	CallFf	r0, r1, 2
	Move	r1, r0
.LBB74_2:
	Move	r0, r1
	Return	r0
"darray-find-remove_symbol_table":
	.quad	-4632259724434563068
	.quad	8925139681302529347
"darray-find-remove_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end74:
	.size	"darray-find-remove", .Lfunc_end74-"darray-find-remove"

	.globl	"vector-scale"
	.type	"vector-scale",@function
"vector-scale":
	IAddImm	r0, r50, 4
	LoadFloat	r1, r0
	LoadFloat	r0, r50
	FMul	r0, r0, r49
	FMul	r1, r1, r49
	IAddImm	r2, r50, 8
	LoadFloat	r2, r2
	FMul	r51, r2, r49
	LoadStaticFloatImm	r52, 1
	LoadU16Imm	r53, 0
	LookupPointer	r2, 0
	Move	r49, r0
	Move	r50, r1
	CallFf	r0, r2, 5
	Return	r0
"vector-scale_symbol_table":
	.quad	-1744966098031674466
	.quad	1065353216
"vector-scale_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end75:
	.size	"vector-scale", .Lfunc_end75-"vector-scale"

	.globl	"task-is-test?"
	.type	"task-is-test?",@function
"task-is-test?":
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	BranchIfNot	.LBB76_1, r0
	LookupPointer	r1, 1
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
.LBB76_1:
	LoadU16Imm	r0, 0
	Return	r0
"task-is-test?_symbol_table":
	.quad	4627505823601477936
	.quad	-8607372249571315066
"task-is-test?_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end76:
	.size	"task-is-test?", .Lfunc_end76-"task-is-test?"

	.globl	"regular-timeline-task-node?"
	.type	"regular-timeline-task-node?",@function
"regular-timeline-task-node?":
	BranchIfNot	.LBB77_1, r49
	LoadU16Imm	r0, 480
	IAdd	r0, r49, r0
	LoadU32	r0, r0
	OpLogNot	r0, r0
	Return	r0
.LBB77_1:
	LoadU16Imm	r0, 0
	Return	r0
.Lfunc_end77:
	.size	"regular-timeline-task-node?", .Lfunc_end77-"regular-timeline-task-node?"

	.globl	"set-object-teleport"
	.type	"set-object-teleport",@function
"set-object-teleport":
	Move	r50, r49
	LoadStaticU64Imm	r49, 1
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	Return	r0
"set-object-teleport_symbol_table":
	.quad	4789555171397753330
	.quad	-2616194530340952722
"set-object-teleport_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end78:
	.size	"set-object-teleport", .Lfunc_end78-"set-object-teleport"

	.globl	"set-shader-instance-wind-intensity"
	.type	"set-shader-instance-wind-intensity",@function
"set-shader-instance-wind-intensity":
	Move	r51, r50
	LoadStaticU64Imm	r50, 1
	LookupPointer	r0, 0
	CallFf	r0, r0, 3
	Return	r0
"set-shader-instance-wind-intensity_symbol_table":
	.quad	-2085764591387011864
	.quad	-2823856363275367900
"set-shader-instance-wind-intensity_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end79:
	.size	"set-shader-instance-wind-intensity", .Lfunc_end79-"set-shader-instance-wind-intensity"

	.globl	"invalid-symbol-var?"
	.type	"invalid-symbol-var?",@function
"invalid-symbol-var?":
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	OpLogNot	r0, r0
	Return	r0
"invalid-symbol-var?_symbol_table":
	.quad	-1136051897790519866
"invalid-symbol-var?_pointer_map":
	.byte	0
.Lfunc_end80:
	.size	"invalid-symbol-var?", .Lfunc_end80-"invalid-symbol-var?"

	.globl	"box-event-arg-point"
	.type	"box-event-arg-point",@function
"box-event-arg-point":
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	LoadU16Imm	r49, 14
	LookupPointer	r1, 0
	Move	r50, r0
	CallFf	r0, r1, 2
	Return	r0
"box-event-arg-point_symbol_table":
	.quad	3684810824690217768
	.quad	-2672655112469118099
"box-event-arg-point_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end81:
	.size	"box-event-arg-point", .Lfunc_end81-"box-event-arg-point"

	.globl	"task-get-full-name"
	.type	"task-get-full-name",@function
"task-get-full-name":
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	BranchIfNot	.LBB82_2, r0
	IAddImm	r0, r0, 40
	LoadU64	r0, r0
	Return	r0
.LBB82_2:
	LookupPointer	r0, 1
	Return	r0
"task-get-full-name_symbol_table":
	.quad	4627505823601477936
	.quad	-517923468232710089
"task-get-full-name_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end82:
	.size	"task-get-full-name", .Lfunc_end82-"task-get-full-name"

	.globl	"wait-simple-npc-move-to-entry"
	.type	"wait-simple-npc-move-to-entry",@function
"wait-simple-npc-move-to-entry":
	Move	r1, r54
	Move	r2, r53
	Move	r3, r52
	Move	r4, r49
	LookupPointer	r0, 0
	Move	r49, r51
	CallFf	r0, r0, 1
	BranchIfNot	.LBB83_2, r0
	LookupPointer	r0, 1
	Move	r49, r51
	CallFf	r0, r0, 1
	Branch	.LBB83_3
.LBB83_2:
	LoadU16Imm	r0, 1
	LookupPointer	r6, 2
	Move	r49, r51
	Move	r5, r50
	Move	r50, r0
	CallFf	r0, r6, 2
	Move	r50, r5
.LBB83_3:
	LoadU16Imm	r52, 0
	LoadStaticU64Imm	r54, 6
	LookupPointer	r5, 5
	Move	r49, r4
	Move	r51, r0
	Move	r53, r56
	CallFf	r0, r5, 6
	LookupPointer	r5, 4
	Move	r49, r0
	CallFf	r0, r5, 1
	LoadU16Imm	r54, 1
	LookupPointer	r5, 3
	Move	r49, r4
	Move	r50, r0
	Move	r51, r3
	Move	r52, r2
	Move	r53, r1
	CallFf	r0, r5, 7
	Return	r0
"wait-simple-npc-move-to-entry_symbol_table":
	.quad	-6870362608876642110
	.quad	61417594283941650
	.quad	-1563483893137887143
	.quad	-6869446841486065293
	.quad	-3881025521939511168
	.quad	1932324078104646333
	.quad	432021034919055241
"wait-simple-npc-move-to-entry_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end83:
	.size	"wait-simple-npc-move-to-entry", .Lfunc_end83-"wait-simple-npc-move-to-entry"

	.globl	"task-skip-to!"
	.type	"task-skip-to!",@function
"task-skip-to!":
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	BranchIfNot	.LBB84_1, r0
	OpLogNot	r51, r51
	LoadU16Imm	r50, 0
	LookupPointer	r1, 1
	Move	r49, r0
	CallFf	r0, r1, 5
	Return	r0
.LBB84_1:
	LoadU16Imm	r0, 0
	Return	r0
"task-skip-to!_symbol_table":
	.quad	4627505823601477936
	.quad	4701419364567053568
"task-skip-to!_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end84:
	.size	"task-skip-to!", .Lfunc_end84-"task-skip-to!"

	.globl	"camera-fade-out-animated"
	.type	"camera-fade-out-animated",@function
"camera-fade-out-animated":
	Move	r51, r50
	Move	r50, r49
	LoadStaticU64Imm	r49, 1
	LookupPointer	r0, 0
	CallFf	r0, r0, 3
	Return	r0
"camera-fade-out-animated_symbol_table":
	.quad	-8117085121181574950
	.quad	3249880500923765431
"camera-fade-out-animated_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end85:
	.size	"camera-fade-out-animated", .Lfunc_end85-"camera-fade-out-animated"

	.globl	"sprint-bits-uint64"
	.type	"sprint-bits-uint64",@function
"sprint-bits-uint64":
	Move	r1, r49
	LookupPointer	r0, 0
	Move	r49, r50
	CallFf	r0, r0, 1
	BranchIfNot	.LBB86_3, r0
	LookupPointer	r2, 1
	Move	r49, r50
	CallFf	r0, r2, 1
	BranchIfNot	.LBB86_3, r0
	Move	r49, r50
	CallFf	r0, r2, 1
	LookupPointer	r2, 2
	Move	r49, r0
	CallFf	r0, r2, 1
	Branch	.LBB86_7
.LBB86_3:
	LoadStaticU64Imm	r0, 3
	INotEqual	r0, r50, r0
	BranchIfNot	.LBB86_4, r0
	BranchIfNot	.LBB86_4, r50
	LookupPointer	r0, 2
	Move	r49, r50
	CallFf	r0, r0, 1
.LBB86_7:
	Move	r2, r0
	Branch	.LBB86_8
.LBB86_4:
	LoadStaticPointerImm	r2, 4
.LBB86_8:
	LoadU16Imm	r8, 0
	LookupPointer	r7, 5
	LookupPointer	r9, 6
	LoadStaticPointerImm	r3, 7
	LoadStaticPointerImm	r6, 8
	LoadU16Imm	r4, 2
	LoadStaticU64Imm	r10, 10
	LoadStaticU64Imm	r11, 9
	Move	r5, r8
.LBB86_9:
	Move	r49, r4
	Move	r50, r5
	CallFf	r0, r9, 2
	OpBitAnd	r0, r0, r1
	INotEqual	r51, r0, r8
	Move	r49, r3
	Move	r50, r6
	CallFf	r0, r7, 3
	Move	r6, r0
	OpBitXor	r0, r5, r10
	ILessThan	r0, r0, r11
	IAddImm	r5, r5, 1
	BranchIf	.LBB86_9, r0
	LoadStaticPointerImm	r49, 11
	Move	r50, r2
	Move	r51, r6
	CallFf	r0, r7, 3
	Return	r0
"sprint-bits-uint64_symbol_table":
	.quad	1313740198704887414
	.quad	3741915750828142429
	.quad	-3311351115081502130
	.quad	3249880500923765431
	.quad	56852
	.quad	6072285182812319085
	.quad	3204416655431713234
	.quad	56565
	.quad	56512
	.quad	-9223372036854775746
	.quad	-9223372036854775808
	.quad	56586
"sprint-bits-uint64_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	0
	.byte	0
	.byte	1
	.byte	1
	.byte	0
	.byte	0
	.byte	1
.Lfunc_end86:
	.size	"sprint-bits-uint64", .Lfunc_end86-"sprint-bits-uint64"

	.globl	"darray-print"
	.type	"darray-print",@function
"darray-print":
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	Move	r1, r0
	LookupPointer	r0, 0
	CallFf	r0, r0, 0
	LoadU16Imm	r0, 1
	ILessThan	r0, r1, r0
	BranchIf	.LBB87_3, r0
	LoadU16Imm	r50, 0
	LookupPointer	r2, 2
.LBB87_2:
	CallFf	r0, r2, 2
	IAddImm	r50, r50, 1
	ILessThan	r0, r50, r1
	BranchIf	.LBB87_2, r0
.LBB87_3:
	LookupPointer	r0, 3
	CallFf	r0, r0, 0
	Return	r0
"darray-print_symbol_table":
	.quad	3423406688794319677
	.quad	4013010305452811590
	.quad	-2664920944689591574
	.quad	-5673227674328399655
"darray-print_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end87:
	.size	"darray-print", .Lfunc_end87-"darray-print"

	.globl	"flashback-task-active?"
	.type	"flashback-task-active?",@function
"flashback-task-active?":
	LookupPointer	r0, 0
	CallFf	r0, r0, 0
	BranchIfNot	.LBB88_1, r0
	LoadU16Imm	r1, 480
	IAdd	r0, r0, r1
	LoadU32	r0, r0
	LoadU16Imm	r1, 1
	IEqual	r0, r0, r1
	Return	r0
.LBB88_1:
	LoadU16Imm	r0, 0
	Return	r0
"flashback-task-active?_symbol_table":
	.quad	8046519909153555413
"flashback-task-active?_pointer_map":
	.byte	0
.Lfunc_end88:
	.size	"flashback-task-active?", .Lfunc_end88-"flashback-task-active?"

	.globl	"wait-stop-animating"
	.type	"wait-stop-animating",@function
"wait-stop-animating":
	LoadU16Imm	r53, 0
	LookupPointer	r0, 1
	CallFf	r0, r0, 5
	LookupPointer	r1, 0
	CallFf	r0, r1, 0
	CallFf	r0, r1, 0
	Return	r0
"wait-stop-animating_symbol_table":
	.quad	-3047912596559836939
	.quad	-7738336759357028456
"wait-stop-animating_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end89:
	.size	"wait-stop-animating", .Lfunc_end89-"wait-stop-animating"

	.globl	"!"
	.type	"!",@function
"!":
	OpLogNot	r0, r49
	Return	r0
.Lfunc_end90:
	.size	"!", .Lfunc_end90-"!"

	.globl	"seek-float"
	.type	"seek-float",@function
"seek-float":
	LoadU16Imm	r0, 0
	FGreaterThanEqual	r1, r51, r0
	BranchIf	.LBB91_2, r1
	Move	r51, r0
.LBB91_2:
	FSub	r2, r50, r49
	FGreaterThanEqual	r3, r2, r51
	Move	r1, r51
	BranchIf	.LBB91_4, r3
	Move	r1, r2
.LBB91_4:
	FNeg	r3, r51
	FLessThanEqual	r4, r2, r3
	BranchIfNot	.LBB91_5, r4
	FGreaterThanEqual	r0, r2, r0
	BranchIfNot	.LBB91_7, r0
.LBB91_8:
	FAdd	r0, r1, r49
	Return	r0
.LBB91_5:
	Move	r3, r2
	FGreaterThanEqual	r0, r2, r0
	BranchIf	.LBB91_8, r0
.LBB91_7:
	Move	r1, r3
	FAdd	r0, r1, r49
	Return	r0
.Lfunc_end91:
	.size	"seek-float", .Lfunc_end91-"seek-float"

	.globl	"get-object-or-spawner-locator"
	.type	"get-object-or-spawner-locator",@function
"get-object-or-spawner-locator":
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	BranchIfNot	.LBB92_2, r0
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	Return	r0
.LBB92_2:
	LoadU16Imm	r50, 1
	LookupPointer	r0, 2
	CallFf	r0, r0, 2
	Return	r0
"get-object-or-spawner-locator_symbol_table":
	.quad	-6870362608876642110
	.quad	61417594283941650
	.quad	-1563483893137887143
"get-object-or-spawner-locator_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end92:
	.size	"get-object-or-spawner-locator", .Lfunc_end92-"get-object-or-spawner-locator"

	.globl	"regular-timeline-task-active?"
	.type	"regular-timeline-task-active?",@function
"regular-timeline-task-active?":
	LookupPointer	r0, 0
	CallFf	r0, r0, 0
	BranchIfNot	.LBB93_1, r0
	LoadU16Imm	r1, 480
	IAdd	r0, r0, r1
	LoadU32	r0, r0
	OpLogNot	r0, r0
	Return	r0
.LBB93_1:
	LoadU16Imm	r0, 0
	Return	r0
"regular-timeline-task-active?_symbol_table":
	.quad	8046519909153555413
"regular-timeline-task-active?_pointer_map":
	.byte	0
.Lfunc_end93:
	.size	"regular-timeline-task-active?", .Lfunc_end93-"regular-timeline-task-active?"

	.globl	"transform-local-to-worldspace"
	.type	"transform-local-to-worldspace",@function
"transform-local-to-worldspace":
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	Return	r0
"transform-local-to-worldspace_symbol_table":
	.quad	-5758420096241265658
"transform-local-to-worldspace_pointer_map":
	.byte	0
.Lfunc_end94:
	.size	"transform-local-to-worldspace", .Lfunc_end94-"transform-local-to-worldspace"

	.globl	"cloth-remove-external-collider"
	.type	"cloth-remove-external-collider",@function
"cloth-remove-external-collider":
	Move	r50, r49
	LoadStaticU64Imm	r49, 1
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	Return	r0
"cloth-remove-external-collider_symbol_table":
	.quad	4789555171397753330
	.quad	-5535122524023873783
"cloth-remove-external-collider_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end95:
	.size	"cloth-remove-external-collider", .Lfunc_end95-"cloth-remove-external-collider"

	.globl	"object-in-namespace?"
	.type	"object-in-namespace?",@function
"object-in-namespace?":
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	IEqual	r0, r0, r50
	Return	r0
"object-in-namespace?_symbol_table":
	.quad	4971238280069550491
"object-in-namespace?_pointer_map":
	.byte	0
.Lfunc_end96:
	.size	"object-in-namespace?", .Lfunc_end96-"object-in-namespace?"

	.globl	"cloth-connect-external-collider"
	.type	"cloth-connect-external-collider",@function
"cloth-connect-external-collider":
	Move	r1, r49
	LoadU16Imm	r49, 7
	LookupPointer	r3, 1
	CallFf	r0, r3, 2
	Move	r2, r0
	Move	r50, r51
	CallFf	r0, r3, 2
	LoadStaticU64Imm	r49, 2
	LookupPointer	r3, 0
	Move	r50, r1
	Move	r51, r2
	Move	r52, r0
	CallFf	r0, r3, 4
	Return	r0
"cloth-connect-external-collider_symbol_table":
	.quad	4789555171397753330
	.quad	3684810824690217768
	.quad	-4899465879314900289
"cloth-connect-external-collider_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end97:
	.size	"cloth-connect-external-collider", .Lfunc_end97-"cloth-connect-external-collider"

	.globl	"print-int32"
	.type	"print-int32",@function
"print-int32":
	LoadU16Imm	r0, 0
	Return	r0
.Lfunc_end98:
	.size	"print-int32", .Lfunc_end98-"print-int32"

	.globl	"print-float"
	.type	"print-float",@function
"print-float":
	LoadU16Imm	r0, 0
	Return	r0
.Lfunc_end99:
	.size	"print-float", .Lfunc_end99-"print-float"

	.globl	"get-object-or-spawner-rotation"
	.type	"get-object-or-spawner-rotation",@function
"get-object-or-spawner-rotation":
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	BranchIfNot	.LBB100_2, r0
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	Branch	.LBB100_3
.LBB100_2:
	LoadU16Imm	r50, 1
	LookupPointer	r0, 2
	CallFf	r0, r0, 2
.LBB100_3:
	LookupPointer	r1, 3
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
"get-object-or-spawner-rotation_symbol_table":
	.quad	-6870362608876642110
	.quad	61417594283941650
	.quad	-1563483893137887143
	.quad	-8899413508202686035
"get-object-or-spawner-rotation_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end100:
	.size	"get-object-or-spawner-rotation", .Lfunc_end100-"get-object-or-spawner-rotation"

	.globl	"task-complete-if-active!"
	.type	"task-complete-if-active!",@function
"task-complete-if-active!":
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	Move	r49, r0
	LoadU16Imm	r1, 0
	BranchIfNot	.LBB101_3, r49
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	BranchIfNot	.LBB101_3, r0
	LookupPointer	r0, 2
	CallFf	r0, r0, 1
	Move	r1, r0
.LBB101_3:
	Move	r0, r1
	Return	r0
"task-complete-if-active!_symbol_table":
	.quad	4627505823601477936
	.quad	1014617123432502549
	.quad	-4779101365983111435
"task-complete-if-active!_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end101:
	.size	"task-complete-if-active!", .Lfunc_end101-"task-complete-if-active!"

	.globl	"box-event-arg-symbol"
	.type	"box-event-arg-symbol",@function
"box-event-arg-symbol":
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	LoadU16Imm	r49, 7
	LookupPointer	r1, 0
	Move	r50, r0
	CallFf	r0, r1, 2
	Return	r0
"box-event-arg-symbol_symbol_table":
	.quad	3684810824690217768
	.quad	3525069928581975493
"box-event-arg-symbol_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end102:
	.size	"box-event-arg-symbol", .Lfunc_end102-"box-event-arg-symbol"

	.globl	"simple-npc-set-config"
	.type	"simple-npc-set-config",@function
"simple-npc-set-config":
	Move	r1, r49
	LoadU16Imm	r49, 7
	LookupPointer	r0, 1
	CallFf	r0, r0, 2
	LoadStaticU64Imm	r49, 2
	LookupPointer	r2, 0
	Move	r50, r1
	Move	r51, r0
	CallFf	r0, r2, 3
	Return	r0
"simple-npc-set-config_symbol_table":
	.quad	4789555171397753330
	.quad	3684810824690217768
	.quad	3110517948991553412
"simple-npc-set-config_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end103:
	.size	"simple-npc-set-config", .Lfunc_end103-"simple-npc-set-config"

	.globl	"set-int64"
	.type	"set-int64",@function
"set-int64":
	LookupPointer	r0, 0
	CallFf	r0, r0, 3
	Return	r0
"set-int64_symbol_table":
	.quad	5968959451188317244
"set-int64_pointer_map":
	.byte	0
.Lfunc_end104:
	.size	"set-int64", .Lfunc_end104-"set-int64"

	.globl	"box-event-arg-bound-frame"
	.type	"box-event-arg-bound-frame",@function
"box-event-arg-bound-frame":
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	LoadU16Imm	r49, 18
	LookupPointer	r1, 0
	Move	r50, r0
	CallFf	r0, r1, 2
	Return	r0
"box-event-arg-bound-frame_symbol_table":
	.quad	3684810824690217768
	.quad	-5617307358675664297
"box-event-arg-bound-frame_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end105:
	.size	"box-event-arg-bound-frame", .Lfunc_end105-"box-event-arg-bound-frame"

	.globl	"vector-random"
	.type	"vector-random",@function
"vector-random":
	Move	r52, r50
	Move	r51, r49
	LoadU16Imm	r1, 0
	LookupPointer	r6, 2
	Move	r49, r1
	CallFf	r0, r6, 1
	LoadU16Imm	r2, 0
	LoadStaticFloatImm	r3, 3
	LookupPointer	r7, 1
	Move	r49, r2
	Move	r50, r3
	Move	r53, r0
	CallFf	r0, r7, 5
	Move	r4, r0
	Move	r49, r1
	CallFf	r0, r6, 1
	Move	r49, r2
	Move	r53, r0
	CallFf	r0, r7, 5
	Move	r5, r0
	Move	r49, r1
	CallFf	r0, r6, 1
	Move	r49, r2
	Move	r53, r0
	CallFf	r0, r7, 5
	LookupPointer	r2, 0
	Move	r49, r4
	Move	r50, r5
	Move	r51, r0
	Move	r52, r3
	Move	r53, r1
	CallFf	r0, r2, 5
	Return	r0
"vector-random_symbol_table":
	.quad	-1744966098031674466
	.quad	6895426483596981631
	.quad	-3265080734848834707
	.quad	1065353216
"vector-random_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end106:
	.size	"vector-random", .Lfunc_end106-"vector-random"

	.globl	"current-task-elapsed-time"
	.type	"current-task-elapsed-time",@function
"current-task-elapsed-time":
	LookupPointer	r0, 1
	CallFf	r0, r0, 0
	Move	r49, r0
	LookupPointer	r0, 0
	CallFf	r0, r0, 0
	Move	r50, r0
	LoadU16Imm	r0, 0
	BranchIfNot	.LBB107_3, r49
	BranchIfNot	.LBB107_3, r50
	LookupPointer	r0, 2
	CallFf	r0, r0, 2
.LBB107_3:
	Return	r0
"current-task-elapsed-time_symbol_table":
	.quad	7172296021436955284
	.quad	-2485240315990590634
	.quad	-8782568257055231931
"current-task-elapsed-time_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end107:
	.size	"current-task-elapsed-time", .Lfunc_end107-"current-task-elapsed-time"

	.globl	"unparent-bound-frame"
	.type	"unparent-bound-frame",@function
"unparent-bound-frame":
	LookupPointer	r0, 2
	CallFf	r0, r0, 1
	Move	r1, r0
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	LoadU16Imm	r51, 0
	LookupPointer	r2, 0
	Move	r49, r1
	Move	r50, r0
	Move	r52, r51
	Move	r53, r51
	CallFf	r0, r2, 5
	Return	r0
"unparent-bound-frame_symbol_table":
	.quad	-8710307833156242393
	.quad	-8899413508202686035
	.quad	-3881025521939511168
"unparent-bound-frame_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end108:
	.size	"unparent-bound-frame", .Lfunc_end108-"unparent-bound-frame"

	.globl	"get-shader-instance-blend-offset"
	.type	"get-shader-instance-blend-offset",@function
"get-shader-instance-blend-offset":
	BranchIfNot	.LBB109_1, r50
	LoadU16Imm	r0, 1
	IEqual	r0, r50, r0
	BranchIfNot	.LBB109_6, r0
	LoadStaticU64Imm	r50, 1
	Branch	.LBB109_3
.LBB109_1:
	LoadStaticU64Imm	r50, 0
	Branch	.LBB109_3
.LBB109_6:
	LoadU16Imm	r0, 2
	IEqual	r0, r50, r0
	BranchIfNot	.LBB109_2, r0
	LoadStaticU64Imm	r50, 2
	Branch	.LBB109_3
.LBB109_2:
	LoadStaticU64Imm	r50, 3
.LBB109_3:
	LookupPointer	r0, 4
	Call	r0, r0, 2
	Return	r0
"get-shader-instance-blend-offset_symbol_table":
	.quad	4469282092045561281
	.quad	4469280992533933070
	.quad	4469279893022304859
	.quad	4469278793510676648
	.quad	-4412383628246461220
"get-shader-instance-blend-offset_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end109:
	.size	"get-shader-instance-blend-offset", .Lfunc_end109-"get-shader-instance-blend-offset"

	.globl	"box-event-arg-vector"
	.type	"box-event-arg-vector",@function
"box-event-arg-vector":
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	LoadU16Imm	r49, 15
	LookupPointer	r1, 0
	Move	r50, r0
	CallFf	r0, r1, 2
	Return	r0
"box-event-arg-vector_symbol_table":
	.quad	3684810824690217768
	.quad	-5564767933503521010
"box-event-arg-vector_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end110:
	.size	"box-event-arg-vector", .Lfunc_end110-"box-event-arg-vector"

	.globl	"task-complete-current!"
	.type	"task-complete-current!",@function
"task-complete-current!":
	LookupPointer	r0, 0
	CallFf	r0, r0, 0
	BranchIfNot	.LBB111_1, r0
	LookupPointer	r1, 1
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
.LBB111_1:
	LoadU16Imm	r0, 0
	Return	r0
"task-complete-current!_symbol_table":
	.quad	8046519909153555413
	.quad	-4779101365983111435
"task-complete-current!_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end111:
	.size	"task-complete-current!", .Lfunc_end111-"task-complete-current!"

	.globl	"get-object-or-spawner-position"
	.type	"get-object-or-spawner-position",@function
"get-object-or-spawner-position":
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	BranchIfNot	.LBB112_2, r0
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	Branch	.LBB112_3
.LBB112_2:
	LoadU16Imm	r50, 1
	LookupPointer	r0, 2
	CallFf	r0, r0, 2
.LBB112_3:
	LookupPointer	r1, 3
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
"get-object-or-spawner-position_symbol_table":
	.quad	-6870362608876642110
	.quad	61417594283941650
	.quad	-1563483893137887143
	.quad	-3881025521939511168
"get-object-or-spawner-position_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end112:
	.size	"get-object-or-spawner-position", .Lfunc_end112-"get-object-or-spawner-position"

	.globl	"camera-fade-out-animated-and-set-next-camera-dir"
	.type	"camera-fade-out-animated-and-set-next-camera-dir",@function
"camera-fade-out-animated-and-set-next-camera-dir":
	Move	r51, r50
	Move	r50, r49
	LoadStaticU64Imm	r49, 2
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	LookupPointer	r1, 0
	Move	r49, r0
	CallFf	r0, r1, 3
	Return	r0
"camera-fade-out-animated-and-set-next-camera-dir_symbol_table":
	.quad	-9136043082405116733
	.quad	3741915750828142429
	.quad	3249880500923765431
"camera-fade-out-animated-and-set-next-camera-dir_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end113:
	.size	"camera-fade-out-animated-and-set-next-camera-dir", .Lfunc_end113-"camera-fade-out-animated-and-set-next-camera-dir"

	.globl	"log-test?"
	.type	"log-test?",@function
"log-test?":
	OpBitAnd	r0, r50, r49
	LoadU16Imm	r1, 0
	INotEqual	r0, r0, r1
	Return	r0
.Lfunc_end114:
	.size	"log-test?", .Lfunc_end114-"log-test?"

	.globl	"spring-reset"
	.type	"spring-reset",@function
"spring-reset":
	LoadU16Imm	r50, 0
	LoadStaticU64Imm	r51, 1
	LookupPointer	r0, 0
	CallFf	r0, r0, 3
	Return	r0
"spring-reset_symbol_table":
	.quad	2780616685946547256
	.quad	3249880500923765431
"spring-reset_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end115:
	.size	"spring-reset", .Lfunc_end115-"spring-reset"

	.globl	"start-timer"
	.type	"start-timer",@function
"start-timer":
	LookupPointer	r0, 0
	CallFf	r0, r0, 3
	Return	r0
"start-timer_symbol_table":
	.quad	2780616685946547256
"start-timer_pointer_map":
	.byte	0
.Lfunc_end116:
	.size	"start-timer", .Lfunc_end116-"start-timer"

	.globl	"print-vector"
	.type	"print-vector",@function
"print-vector":
	LoadU16Imm	r0, 0
	Return	r0
.Lfunc_end117:
	.size	"print-vector", .Lfunc_end117-"print-vector"

	.globl	"task-set-active-continue!"
	.type	"task-set-active-continue!",@function
"task-set-active-continue!":
	Move	r56, r55
	Move	r55, r54
	Move	r54, r53
	Move	r53, r52
	Move	r52, r51
	Move	r51, r50
	Move	r50, r49
	LookupPointer	r0, 0
	CallFf	r0, r0, 0
	BranchIfNot	.LBB118_1, r0
	LookupPointer	r1, 1
	Move	r49, r0
	CallFf	r0, r1, 8
	Return	r0
.LBB118_1:
	LoadU16Imm	r0, 0
	Return	r0
"task-set-active-continue!_symbol_table":
	.quad	8046519909153555413
	.quad	3155490365585411203
"task-set-active-continue!_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end118:
	.size	"task-set-active-continue!", .Lfunc_end118-"task-set-active-continue!"

	.globl	"task-play-rogue"
	.type	"task-play-rogue",@function
"task-play-rogue":
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	BranchIfNot	.LBB119_1, r0
	LoadU16Imm	r1, 1
	LookupPointer	r2, 1
	Move	r49, r0
	Move	r50, r53
	Move	r53, r1
	CallFf	r0, r2, 5
	Return	r0
.LBB119_1:
	LoadU16Imm	r0, 0
	Return	r0
"task-play-rogue_symbol_table":
	.quad	4627505823601477936
	.quad	4701419364567053568
"task-play-rogue_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end119:
	.size	"task-play-rogue", .Lfunc_end119-"task-play-rogue"

	.globl	"print-float-melee"
	.type	"print-float-melee",@function
"print-float-melee":
	LoadU16Imm	r0, 1
	Return	r0
.Lfunc_end120:
	.size	"print-float-melee", .Lfunc_end120-"print-float-melee"

	.globl	"set-shader-instance-tint"
	.type	"set-shader-instance-tint",@function
"set-shader-instance-tint":
	Move	r1, r51
	Move	r51, r50
	BranchIfNot	.LBB121_1, r53
	LoadU16Imm	r0, 1
	IEqual	r0, r53, r0
	BranchIfNot	.LBB121_6, r0
	LoadStaticU64Imm	r2, 3
	LoadStaticU64Imm	r3, 4
	LoadStaticU64Imm	r50, 5
	Branch	.LBB121_3
.LBB121_1:
	LoadStaticU64Imm	r2, 0
	LoadStaticU64Imm	r3, 1
	LoadStaticU64Imm	r50, 2
	Branch	.LBB121_3
.LBB121_6:
	LoadU16Imm	r0, 2
	IEqual	r0, r53, r0
	BranchIfNot	.LBB121_2, r0
	LoadStaticU64Imm	r2, 6
	LoadStaticU64Imm	r3, 7
	LoadStaticU64Imm	r50, 8
	Branch	.LBB121_3
.LBB121_2:
	LoadStaticU64Imm	r2, 9
	LoadStaticU64Imm	r3, 10
	LoadStaticU64Imm	r50, 11
.LBB121_3:
	LookupPointer	r4, 12
	Call	r0, r4, 3
	Move	r50, r3
	Move	r51, r1
	Call	r0, r4, 3
	Move	r50, r2
	Move	r51, r52
	Call	r0, r4, 3
	Return	r0
"set-shader-instance-tint_symbol_table":
	.quad	-3597801397546541892
	.quad	-3597804696081426525
	.quad	-3597818989732593268
	.quad	-2977392065887700093
	.quad	-2977388767352815460
	.quad	-2977409658073751469
	.quad	-2330264601668629294
	.quad	-2330267900203513927
	.quad	-2330247009482577918
	.quad	-1771216814944876583
	.quad	-1771213516409991950
	.quad	-1771199222758825207
	.quad	-2085764591387011864
"set-shader-instance-tint_pointer_map":
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
.Lfunc_end121:
	.size	"set-shader-instance-tint", .Lfunc_end121-"set-shader-instance-tint"

	.globl	"camera-fade-out-auto-gen-animated"
	.type	"camera-fade-out-auto-gen-animated",@function
"camera-fade-out-auto-gen-animated":
	Move	r52, r51
	Move	r51, r50
	Move	r50, r49
	LoadStaticU64Imm	r49, 2
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	LookupPointer	r1, 0
	Move	r49, r0
	CallFf	r0, r1, 4
	Return	r0
"camera-fade-out-auto-gen-animated_symbol_table":
	.quad	9146608591743818257
	.quad	3741915750828142429
	.quad	3249880500923765431
"camera-fade-out-auto-gen-animated_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end122:
	.size	"camera-fade-out-auto-gen-animated", .Lfunc_end122-"camera-fade-out-auto-gen-animated"

	.globl	"task-continue-active?"
	.type	"task-continue-active?",@function
"task-continue-active?":
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	BranchIfNot	.LBB123_1, r0
	LookupPointer	r1, 1
	Move	r49, r0
	CallFf	r0, r1, 1
	IEqual	r0, r0, r51
	Return	r0
.LBB123_1:
	LoadU16Imm	r0, 0
	Return	r0
"task-continue-active?_symbol_table":
	.quad	4627505823601477936
	.quad	8528085549934399215
"task-continue-active?_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end123:
	.size	"task-continue-active?", .Lfunc_end123-"task-continue-active?"

	.globl	"wait-random"
	.type	"wait-random",@function
"wait-random":
	Move	r1, r49
	LoadU16Imm	r49, 0
	LookupPointer	r0, 1
	CallFf	r0, r0, 1
	FSub	r2, r50, r1
	FMul	r0, r2, r0
	FAdd	r49, r0, r1
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	Return	r0
"wait-random_symbol_table":
	.quad	6809905488770624160
	.quad	-3265080734848834707
"wait-random_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end124:
	.size	"wait-random", .Lfunc_end124-"wait-random"

	.globl	"task-disable!"
	.type	"task-disable!",@function
"task-disable!":
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	BranchIfNot	.LBB125_1, r0
	LookupPointer	r1, 1
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
.LBB125_1:
	LoadU16Imm	r0, 0
	Return	r0
"task-disable!_symbol_table":
	.quad	4627505823601477936
	.quad	-5062543229031975002
"task-disable!_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end125:
	.size	"task-disable!", .Lfunc_end125-"task-disable!"

	.globl	"cloth-enable-collider"
	.type	"cloth-enable-collider",@function
"cloth-enable-collider":
	Move	r1, r49
	LoadU16Imm	r2, 7
	LookupPointer	r4, 1
	Move	r49, r2
	CallFf	r0, r4, 2
	Move	r3, r0
	LoadU16Imm	r49, 1
	Move	r50, r49
	CallFf	r0, r4, 2
	Move	r52, r0
	Move	r49, r2
	Move	r50, r51
	CallFf	r0, r4, 2
	LoadStaticU64Imm	r49, 2
	LookupPointer	r2, 0
	Move	r50, r1
	Move	r51, r3
	Move	r53, r0
	CallFf	r0, r2, 5
	Return	r0
"cloth-enable-collider_symbol_table":
	.quad	4789555171397753330
	.quad	3684810824690217768
	.quad	-5115115189087745418
"cloth-enable-collider_pointer_map":
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end126:
	.size	"cloth-enable-collider", .Lfunc_end126-"cloth-enable-collider"

	.globl	"task-active?"
	.type	"task-active?",@function
"task-active?":
	LookupPointer	r0, 0
	CallFf	r0, r0, 2
	BranchIfNot	.LBB127_1, r0
	LookupPointer	r1, 1
	Move	r49, r0
	CallFf	r0, r1, 1
	Return	r0
.LBB127_1:
	LoadU16Imm	r0, 0
	Return	r0
"task-active?_symbol_table":
	.quad	4627505823601477936
	.quad	1014617123432502549
"task-active?_pointer_map":
	.byte	0
	.byte	0
.Lfunc_end127:
	.size	"task-active?", .Lfunc_end127-"task-active?"

	.globl	"get-bit-uint64"
	.type	"get-bit-uint64",@function
"get-bit-uint64":
	LoadU16Imm	r0, 1
	IntAsh	r0, r0, r50
	OpBitAnd	r0, r0, r49
	LoadU16Imm	r1, 0
	INotEqual	r0, r0, r1
	Return	r0
.Lfunc_end128:
	.size	"get-bit-uint64", .Lfunc_end128-"get-bit-uint64"

	.globl	"conversation-interactable-disable"
	.type	"conversation-interactable-disable",@function
"conversation-interactable-disable":
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	Return	r0
"conversation-interactable-disable_symbol_table":
	.quad	-1642320195793687078
"conversation-interactable-disable_pointer_map":
	.byte	0
.Lfunc_end129:
	.size	"conversation-interactable-disable", .Lfunc_end129-"conversation-interactable-disable"

	.globl	"simple-npc-is-female?"
	.type	"simple-npc-is-female?",@function
"simple-npc-is-female?":
	LookupPointer	r0, 0
	CallFf	r0, r0, 1
	Return	r0
"simple-npc-is-female?_symbol_table":
	.quad	-2523998684583063419
"simple-npc-is-female?_pointer_map":
	.byte	0
.Lfunc_end130:
	.size	"simple-npc-is-female?", .Lfunc_end130-"simple-npc-is-female?"

	.globl	"spawn-camera"
	.type	"spawn-camera",@function
"spawn-camera":
	Move	r53, r51
	Move	r52, r50
	Move	r51, r49
	LoadStaticU64Imm	r49, 5
	LoadStaticU64Imm	r50, 4
	LoadU16Imm	r54, 0
	LookupPointer	r0, 2
	Move	r55, r54
	Move	r56, r54
	Move	r57, r54
	CallFf	r0, r0, 9
	Move	r1, r0
	LoadU16Imm	r49, 1
	LookupPointer	r0, 1
	Move	r50, r54
	CallFf	r0, r0, 2
	LoadStaticU64Imm	r49, 3
	LookupPointer	r2, 0
	Move	r50, r1
	Move	r51, r0
	CallFf	r0, r2, 3
	Move	r0, r1
	Return	r0
"spawn-camera_symbol_table":
	.quad	4789555171397753330
	.quad	3684810824690217768
	.quad	7097052834867614476
	.quad	-5058770882771356923
	.quad	2283337050572107970
	.quad	-6022348500830226157
"spawn-camera_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end131:
	.size	"spawn-camera", .Lfunc_end131-"spawn-camera"

	.globl	"set-shader-instance-blend-offset"
	.type	"set-shader-instance-blend-offset",@function
"set-shader-instance-blend-offset":
	Move	r0, r50
	BranchIfNot	.LBB132_1, r51
	LoadU16Imm	r1, 1
	IEqual	r1, r51, r1
	BranchIfNot	.LBB132_6, r1
	LoadStaticU64Imm	r50, 1
	Branch	.LBB132_3
.LBB132_1:
	LoadStaticU64Imm	r50, 0
	Branch	.LBB132_3
.LBB132_6:
	LoadU16Imm	r1, 2
	IEqual	r1, r51, r1
	BranchIfNot	.LBB132_2, r1
	LoadStaticU64Imm	r50, 2
	Branch	.LBB132_3
.LBB132_2:
	LoadStaticU64Imm	r50, 3
.LBB132_3:
	LookupPointer	r1, 4
	Move	r51, r0
	Call	r0, r1, 3
	Return	r0
"set-shader-instance-blend-offset_symbol_table":
	.quad	4469282092045561281
	.quad	4469280992533933070
	.quad	4469279893022304859
	.quad	4469278793510676648
	.quad	-2085764591387011864
"set-shader-instance-blend-offset_pointer_map":
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
.Lfunc_end132:
	.size	"set-shader-instance-blend-offset", .Lfunc_end132-"set-shader-instance-blend-offset"

	.type	"print-point_symbol_table_src",@object
	.section	".rodata.print-point_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"print-point_symbol_table_src":
	.quad	-7966019873906053962
	.quad	-1851807429122828651
	.quad	6072285182812319085
	.quad	2492347785568
	.quad	1313740198704887414
	.quad	3741915750828142429
	.quad	-3311351115081502130
	.quad	3249880500923765431
	.quad	2492347785847
	.size	"print-point_symbol_table_src", 72

	.type	"set-bit_symbol_table_src",@object
	.section	".rodata.set-bit_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"set-bit_symbol_table_src":
	.size	"set-bit_symbol_table_src", 0

	.type	"wait-task-complete_symbol_table_src",@object
	.section	".rodata.cst16.wait-task-complete_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"wait-task-complete_symbol_table_src":
	.quad	8743832621443356413
	.quad	-3047912596559836939
	.size	"wait-task-complete_symbol_table_src", 16

	.type	"task-get-timeline_symbol_table_src",@object
	.section	".rodata.task-get-timeline_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"task-get-timeline_symbol_table_src":
	.quad	4627505823601477936
	.quad	-1194883309477416555
	.quad	4294967295
	.size	"task-get-timeline_symbol_table_src", 24

	.type	"print-string_symbol_table_src",@object
	.section	".rodata.print-string_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"print-string_symbol_table_src":
	.quad	-7966019873906053962
	.quad	-1851807429122828651
	.quad	6072285182812319085
	.quad	2492347785546
	.quad	1313740198704887414
	.quad	3741915750828142429
	.quad	-3311351115081502130
	.quad	3249880500923765431
	.quad	2492347785859
	.size	"print-string_symbol_table_src", 72

	.type	"set-shader-instance-shimmer-intensity_symbol_table_src",@object
	.section	".rodata.cst16.set-shader-instance-shimmer-intensity_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"set-shader-instance-shimmer-intensity_symbol_table_src":
	.quad	-2085764591387011864
	.quad	4338449670443486021
	.size	"set-shader-instance-shimmer-intensity_symbol_table_src", 16

	.type	"stop-timer_symbol_table_src",@object
	.section	".rodata.cst16.stop-timer_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"stop-timer_symbol_table_src":
	.quad	2780616685946547256
	.quad	0
	.size	"stop-timer_symbol_table_src", 16

	.type	"camera-enable-animated-no-fail-over_symbol_table_src",@object
	.section	".rodata.cst8.camera-enable-animated-no-fail-over_symbol_table_src","aMR",@progbits,8
	.p2align	3, 0x0
"camera-enable-animated-no-fail-over_symbol_table_src":
	.quad	3112973522196772772
	.size	"camera-enable-animated-no-fail-over_symbol_table_src", 8

	.type	"wait-simple-npc-move-to-goal_symbol_table_src",@object
	.section	".rodata.cst8.wait-simple-npc-move-to-goal_symbol_table_src","aMR",@progbits,8
	.p2align	3, 0x0
"wait-simple-npc-move-to-goal_symbol_table_src":
	.quad	-6869446841486065293
	.size	"wait-simple-npc-move-to-goal_symbol_table_src", 8

	.type	"cin-lerp-maya_symbol_table_src",@object
	.section	".rodata.cst32.cin-lerp-maya_symbol_table_src","aMR",@progbits,32
	.p2align	3, 0x0
"cin-lerp-maya_symbol_table_src":
	.quad	6895426483596981631
	.quad	0
	.quad	1065353216
	.quad	8088041438043820006
	.size	"cin-lerp-maya_symbol_table_src", 32

	.type	"event-message_symbol_table_src",@object
	.section	".rodata.cst16.event-message_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"event-message_symbol_table_src":
	.quad	2968890880210854491
	.quad	-734627043301783238
	.size	"event-message_symbol_table_src", 16

	.type	"camera-enable-very-tight-squeeze-through/f_symbol_table_src",@object
	.section	".rodata.camera-enable-very-tight-squeeze-through/f_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"camera-enable-very-tight-squeeze-through/f_symbol_table_src":
	.quad	6196078168405531082
	.quad	1028443341
	.quad	4822232382305105757
	.quad	-6307733480328395657
	.quad	1056964608
	.size	"camera-enable-very-tight-squeeze-through/f_symbol_table_src", 40

	.type	"pause-hint-timer_symbol_table_src",@object
	.section	".rodata.cst16.pause-hint-timer_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"pause-hint-timer_symbol_table_src":
	.quad	4789555171397753330
	.quad	-6479984080625612975
	.size	"pause-hint-timer_symbol_table_src", 16

	.type	"from-upf-no-dt_symbol_table_src",@object
	.section	".rodata.cst8.from-upf-no-dt_symbol_table_src","aMR",@progbits,8
	.p2align	3, 0x0
"from-upf-no-dt_symbol_table_src":
	.quad	4835029146322721710
	.size	"from-upf-no-dt_symbol_table_src", 8

	.type	"wait-for-object-spawned-with-look_symbol_table_src",@object
	.section	".rodata.cst32.wait-for-object-spawned-with-look_symbol_table_src","aMR",@progbits,32
	.p2align	3, 0x0
"wait-for-object-spawned-with-look_symbol_table_src":
	.quad	-23842759317690077
	.quad	-7861648282172184102
	.quad	8517895422230748631
	.quad	-3047912596559836939
	.size	"wait-for-object-spawned-with-look_symbol_table_src", 32

	.type	"current-task-total-elapsed-time_symbol_table_src",@object
	.section	".rodata.cst32.current-task-total-elapsed-time_symbol_table_src","aMR",@progbits,32
	.p2align	3, 0x0
"current-task-total-elapsed-time_symbol_table_src":
	.quad	-2485240315990590634
	.quad	7172296021436955284
	.quad	654070727927837254
	.quad	0
	.size	"current-task-total-elapsed-time_symbol_table_src", 32

	.type	"sprint-bits_symbol_table_src",@object
	.section	".rodata.sprint-bits_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"sprint-bits_symbol_table_src":
	.quad	6072285182812319085
	.quad	2492347785546
	.quad	1313740198704887414
	.quad	3741915750828142429
	.quad	-3311351115081502130
	.quad	3249880500923765431
	.quad	2492347785812
	.quad	2492347785472
	.quad	2492347785525
	.quad	8618586874330430261
	.size	"sprint-bits_symbol_table_src", 80

	.type	"flashback-task?_symbol_table_src",@object
	.section	".rodata.cst16.flashback-task?_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"flashback-task?_symbol_table_src":
	.quad	4627505823601477936
	.quad	6284621658636359800
	.size	"flashback-task?_symbol_table_src", 16

	.type	"wait-simple-npc-move-to-spawner_symbol_table_src",@object
	.section	".rodata.cst8.wait-simple-npc-move-to-spawner_symbol_table_src","aMR",@progbits,8
	.p2align	3, 0x0
"wait-simple-npc-move-to-spawner_symbol_table_src":
	.quad	-8553643611204354492
	.size	"wait-simple-npc-move-to-spawner_symbol_table_src", 8

	.type	"get-bit_symbol_table_src",@object
	.section	".rodata.get-bit_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"get-bit_symbol_table_src":
	.size	"get-bit_symbol_table_src", 0

	.type	"box-event-arg-string_symbol_table_src",@object
	.section	".rodata.cst16.box-event-arg-string_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"box-event-arg-string_symbol_table_src":
	.quad	3684810824690217768
	.quad	-7040857727560215700
	.size	"box-event-arg-string_symbol_table_src", 16

	.type	"print-vector2_symbol_table_src",@object
	.section	".rodata.print-vector2_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"print-vector2_symbol_table_src":
	.quad	-7966019873906053962
	.quad	-1851807429122828651
	.quad	6072285182812319085
	.quad	2492347785554
	.quad	1313740198704887414
	.quad	3741915750828142429
	.quad	-3311351115081502130
	.quad	3249880500923765431
	.quad	2492347785898
	.size	"print-vector2_symbol_table_src", 72

	.type	"set-shader-instance-incandescence-strength_symbol_table_src",@object
	.section	".rodata.cst16.set-shader-instance-incandescence-strength_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"set-shader-instance-incandescence-strength_symbol_table_src":
	.quad	-2085764591387011864
	.quad	-3186304195179347301
	.size	"set-shader-instance-incandescence-strength_symbol_table_src", 16

	.type	"get-bounding-sphere-center_symbol_table_src",@object
	.section	".rodata.cst16.get-bounding-sphere-center_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"get-bounding-sphere-center_symbol_table_src":
	.quad	-8586803022565429251
	.quad	0
	.size	"get-bounding-sphere-center_symbol_table_src", 16

	.type	"darray-extend_symbol_table_src",@object
	.section	".rodata.darray-extend_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"darray-extend_symbol_table_src":
	.quad	4013010305452811590
	.quad	3423406688794319677
	.quad	-2664920944689591574
	.quad	3321216610294482167
	.quad	-5673227674328399655
	.size	"darray-extend_symbol_table_src", 40

	.type	"subtitle-force-show_symbol_table_src",@object
	.section	".rodata.cst8.subtitle-force-show_symbol_table_src","aMR",@progbits,8
	.p2align	3, 0x0
"subtitle-force-show_symbol_table_src":
	.quad	-7089072539499493873
	.size	"subtitle-force-show_symbol_table_src", 8

	.type	"wait-simple-npc-move-to-entry-frame_symbol_table_src",@object
	.section	".rodata.wait-simple-npc-move-to-entry-frame_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wait-simple-npc-move-to-entry-frame_symbol_table_src":
	.quad	-3881025521939511168
	.quad	1932324078104646333
	.quad	0
	.quad	432021034919055241
	.quad	1584371146664193409
	.size	"wait-simple-npc-move-to-entry-frame_symbol_table_src", 40

	.type	"set-shader-instance-uv-offset_symbol_table_src",@object
	.section	".rodata.set-shader-instance-uv-offset_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"set-shader-instance-uv-offset_symbol_table_src":
	.quad	-2085764591387011864
	.quad	3597403256821491964
	.quad	3597406555356376597
	.size	"set-shader-instance-uv-offset_symbol_table_src", 24

	.type	"task-subnode-complete?_symbol_table_src",@object
	.section	".rodata.task-subnode-complete?_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"task-subnode-complete?_symbol_table_src":
	.size	"task-subnode-complete?_symbol_table_src", 0

	.type	"get-object-speed_symbol_table_src",@object
	.section	".rodata.cst16.get-object-speed_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"get-object-speed_symbol_table_src":
	.quad	-4487271785256571635
	.quad	-1905058307015679353
	.size	"get-object-speed_symbol_table_src", 16

	.type	"current-task-is-test?_symbol_table_src",@object
	.section	".rodata.current-task-is-test?_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"current-task-is-test?_symbol_table_src":
	.quad	-2485240315990590634
	.quad	7172296021436955284
	.quad	-7741487554082613093
	.size	"current-task-is-test?_symbol_table_src", 24

	.type	"wait-simple-npc-move-to-action-pack_symbol_table_src",@object
	.section	".rodata.cst8.wait-simple-npc-move-to-action-pack_symbol_table_src","aMR",@progbits,8
	.p2align	3, 0x0
"wait-simple-npc-move-to-action-pack_symbol_table_src":
	.quad	1238128838624674224
	.size	"wait-simple-npc-move-to-action-pack_symbol_table_src", 8

	.type	"simple-npc-transfer-to-spline_symbol_table_src",@object
	.section	".rodata.cst8.simple-npc-transfer-to-spline_symbol_table_src","aMR",@progbits,8
	.p2align	3, 0x0
"simple-npc-transfer-to-spline_symbol_table_src":
	.quad	-2272449730465418826
	.size	"simple-npc-transfer-to-spline_symbol_table_src", 8

	.type	"get-bounding-sphere-radius_symbol_table_src",@object
	.section	".rodata.get-bounding-sphere-radius_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"get-bounding-sphere-radius_symbol_table_src":
	.size	"get-bounding-sphere-radius_symbol_table_src", 0

	.type	"flashback-task-node?_symbol_table_src",@object
	.section	".rodata.flashback-task-node?_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"flashback-task-node?_symbol_table_src":
	.size	"flashback-task-node?_symbol_table_src", 0

	.type	"set-bit-uint64_symbol_table_src",@object
	.section	".rodata.set-bit-uint64_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"set-bit-uint64_symbol_table_src":
	.size	"set-bit-uint64_symbol_table_src", 0

	.type	"find-index-in-symbol-array-broken_symbol_table_src",@object
	.section	".rodata.cst8.find-index-in-symbol-array-broken_symbol_table_src","aMR",@progbits,8
	.p2align	3, 0x0
"find-index-in-symbol-array-broken_symbol_table_src":
	.quad	4294967295
	.size	"find-index-in-symbol-array-broken_symbol_table_src", 8

	.type	"wait-for-want-load-levels-fully-spawned_symbol_table_src",@object
	.section	".rodata.cst32.wait-for-want-load-levels-fully-spawned_symbol_table_src","aMR",@progbits,32
	.p2align	3, 0x0
"wait-for-want-load-levels-fully-spawned_symbol_table_src":
	.quad	7377869489011700959
	.quad	-1851807429122828651
	.quad	2492347785912
	.quad	-3047912596559836939
	.size	"wait-for-want-load-levels-fully-spawned_symbol_table_src", 32

	.type	"regular-timeline-task?_symbol_table_src",@object
	.section	".rodata.cst16.regular-timeline-task?_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"regular-timeline-task?_symbol_table_src":
	.quad	4627505823601477936
	.quad	-7721148249613330231
	.size	"regular-timeline-task?_symbol_table_src", 16

	.type	"ddict-print_symbol_table_src",@object
	.section	".rodata.ddict-print_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"ddict-print_symbol_table_src":
	.quad	-7966019873906053962
	.quad	-1851807429122828651
	.quad	6072285182812319085
	.quad	2492347785791
	.quad	-3311351115081502130
	.quad	2736978041656912235
	.quad	3423406688794319677
	.quad	-2290456712816956831
	.quad	2492347785478
	.quad	-4538651403539196202
	.quad	2492347785522
	.quad	2492347785498
	.quad	2492347785495
	.quad	2492347785501
	.quad	2492347785590
	.quad	2492347785507
	.quad	2492347785514
	.quad	2492347785504
	.quad	0
	.quad	2492347785594
	.quad	2492347785664
	.quad	2492347785689
	.quad	2492347785649
	.quad	2492347750768
	.quad	2492347785742
	.quad	2492347750784
	.quad	2492347785754
	.quad	2492347785706
	.quad	2492347785639
	.quad	2492347785615
	.quad	2492347785490
	.quad	2492347785472
	.quad	2492347785629
	.quad	2492347785607
	.quad	2492347785719
	.quad	2492347785732
	.quad	-5673227674328399655
	.size	"ddict-print_symbol_table_src", 296

	.type	"empty-string?_symbol_table_src",@object
	.section	".rodata.cst16.empty-string?_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"empty-string?_symbol_table_src":
	.quad	-3711322450294902906
	.quad	2492347785472
	.size	"empty-string?_symbol_table_src", 16

	.type	"print-bits_symbol_table_src",@object
	.section	".rodata.cst32.print-bits_symbol_table_src","aMR",@progbits,32
	.p2align	3, 0x0
"print-bits_symbol_table_src":
	.quad	-7966019873906053962
	.quad	-1851807429122828651
	.quad	6072285182812319085
	.quad	2862327205561979872
	.size	"print-bits_symbol_table_src", 32

	.type	"task-play-fadeout!_symbol_table_src",@object
	.section	".rodata.cst16.task-play-fadeout!_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"task-play-fadeout!_symbol_table_src":
	.quad	4627505823601477936
	.quad	4701419364567053568
	.size	"task-play-fadeout!_symbol_table_src", 16

	.type	"box-event-arg-int32_symbol_table_src",@object
	.section	".rodata.cst16.box-event-arg-int32_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"box-event-arg-int32_symbol_table_src":
	.quad	3684810824690217768
	.quad	-4700555454704927757
	.size	"box-event-arg-int32_symbol_table_src", 16

	.type	"task-or-continue-active?_symbol_table_src",@object
	.section	".rodata.task-or-continue-active?_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"task-or-continue-active?_symbol_table_src":
	.quad	8164313781786671539
	.quad	3249880500923765431
	.quad	-3513786953354759377
	.quad	475792274027315025
	.quad	-2218206844643806166
	.quad	-3530598740559159857
	.quad	-8479599307812291032
	.quad	-7080778341598050516
	.quad	-1282962725907059120
	.quad	-921104678677575066
	.size	"task-or-continue-active?_symbol_table_src", 80

	.type	"random-float-range_symbol_table_src",@object
	.section	".rodata.cst32.random-float-range_symbol_table_src","aMR",@progbits,32
	.p2align	3, 0x0
"random-float-range_symbol_table_src":
	.quad	6895426483596981631
	.quad	0
	.quad	1065353216
	.quad	-3265080734848834707
	.size	"random-float-range_symbol_table_src", 32

	.type	"print-symbol_symbol_table_src",@object
	.section	".rodata.print-symbol_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"print-symbol_symbol_table_src":
	.quad	-7966019873906053962
	.quad	-1851807429122828651
	.quad	6072285182812319085
	.quad	2492347785546
	.quad	1313740198704887414
	.quad	3741915750828142429
	.quad	-3311351115081502130
	.quad	3249880500923765431
	.quad	2492347785872
	.size	"print-symbol_symbol_table_src", 72

	.type	"wait-stop-animating-by-layer-name_symbol_table_src",@object
	.section	".rodata.cst16.wait-stop-animating-by-layer-name_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"wait-stop-animating-by-layer-name_symbol_table_src":
	.quad	6918975437027605210
	.quad	-3047912596559836939
	.size	"wait-stop-animating-by-layer-name_symbol_table_src", 16

	.type	"get-object-relative-speed_symbol_table_src",@object
	.section	".rodata.cst16.get-object-relative-speed_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"get-object-relative-speed_symbol_table_src":
	.quad	-4487271785256571635
	.quad	-653718488554981094
	.size	"get-object-relative-speed_symbol_table_src", 16

	.type	pow_symbol_table_src,@object
	.section	.rodata.cst8.pow_symbol_table_src,"aMR",@progbits,8
	.p2align	3, 0x0
pow_symbol_table_src:
	.quad	3204416655431713234
	.size	pow_symbol_table_src, 8

	.type	"rand-range_symbol_table_src",@object
	.section	".rodata.cst8.rand-range_symbol_table_src","aMR",@progbits,8
	.p2align	3, 0x0
"rand-range_symbol_table_src":
	.quad	-3265080734848834707
	.size	"rand-range_symbol_table_src", 8

	.type	"cin-lerp-frames_symbol_table_src",@object
	.section	".rodata.cst32.cin-lerp-frames_symbol_table_src","aMR",@progbits,32
	.p2align	3, 0x0
"cin-lerp-frames_symbol_table_src":
	.quad	6895426483596981631
	.quad	0
	.quad	1065353216
	.quad	-6493350602403076389
	.size	"cin-lerp-frames_symbol_table_src", 32

	.type	"task-complete?_symbol_table_src",@object
	.section	".rodata.cst32.task-complete?_symbol_table_src","aMR",@progbits,32
	.p2align	3, 0x0
"task-complete?_symbol_table_src":
	.quad	4627505823601477936
	.quad	-3098704140420144833
	.quad	-9020003840458003388
	.quad	5435285489286428990
	.size	"task-complete?_symbol_table_src", 32

	.type	"task-complete!_symbol_table_src",@object
	.section	".rodata.cst16.task-complete!_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"task-complete!_symbol_table_src":
	.quad	4627505823601477936
	.quad	-4779101365983111435
	.size	"task-complete!_symbol_table_src", 16

	.type	"box-event-arg-boolean_symbol_table_src",@object
	.section	".rodata.cst16.box-event-arg-boolean_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"box-event-arg-boolean_symbol_table_src":
	.quad	3684810824690217768
	.quad	-696762128607962741
	.size	"box-event-arg-boolean_symbol_table_src", 16

	.type	"simple-npc-move-to-entry_symbol_table_src",@object
	.section	".rodata.simple-npc-move-to-entry_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"simple-npc-move-to-entry_symbol_table_src":
	.quad	-5739656845705241185
	.quad	-3881025521939511168
	.quad	1932324078104646333
	.quad	0
	.quad	432021034919055241
	.quad	-6869446841486065293
	.size	"simple-npc-move-to-entry_symbol_table_src", 48

	.type	"box-event-arg-uint64_symbol_table_src",@object
	.section	".rodata.cst16.box-event-arg-uint64_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"box-event-arg-uint64_symbol_table_src":
	.quad	3684810824690217768
	.quad	-3476174700326899881
	.size	"box-event-arg-uint64_symbol_table_src", 16

	.type	"joypad-stick-pulled-away?_symbol_table_src",@object
	.section	".rodata.cst16.joypad-stick-pulled-away?_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"joypad-stick-pulled-away?_symbol_table_src":
	.quad	7657644897092222194
	.quad	1127481344
	.size	"joypad-stick-pulled-away?_symbol_table_src", 16

	.type	"cloth-disable-collider_symbol_table_src",@object
	.section	".rodata.cloth-disable-collider_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"cloth-disable-collider_symbol_table_src":
	.quad	4789555171397753330
	.quad	-5115115189087745418
	.quad	3684810824690217768
	.size	"cloth-disable-collider_symbol_table_src", 24

	.type	"camera-disable-animated_symbol_table_src",@object
	.section	".rodata.cst16.camera-disable-animated_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"camera-disable-animated_symbol_table_src":
	.quad	2188720273919511335
	.quad	3249880500923765431
	.size	"camera-disable-animated_symbol_table_src", 16

	.type	"valid-symbol-var?_symbol_table_src",@object
	.section	".rodata.cst8.valid-symbol-var?_symbol_table_src","aMR",@progbits,8
	.p2align	3, 0x0
"valid-symbol-var?_symbol_table_src":
	.quad	-1136051897790519866
	.size	"valid-symbol-var?_symbol_table_src", 8

	.type	"task-get-active-continue_symbol_table_src",@object
	.section	".rodata.cst16.task-get-active-continue_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"task-get-active-continue_symbol_table_src":
	.quad	8046519909153555413
	.quad	8528085549934399215
	.size	"task-get-active-continue_symbol_table_src", 16

	.type	"print-bits-uint64_symbol_table_src",@object
	.section	".rodata.cst32.print-bits-uint64_symbol_table_src","aMR",@progbits,32
	.p2align	3, 0x0
"print-bits-uint64_symbol_table_src":
	.quad	-7966019873906053962
	.quad	-1851807429122828651
	.quad	6072285182812319085
	.quad	-6193232601609950795
	.size	"print-bits-uint64_symbol_table_src", 32

	.type	"box-event-arg-float_symbol_table_src",@object
	.section	".rodata.cst16.box-event-arg-float_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"box-event-arg-float_symbol_table_src":
	.quad	3684810824690217768
	.quad	-7227679359147913623
	.size	"box-event-arg-float_symbol_table_src", 16

	.type	"camera-abandon-animated_symbol_table_src",@object
	.section	".rodata.cst16.camera-abandon-animated_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"camera-abandon-animated_symbol_table_src":
	.quad	261166031521791970
	.quad	3249880500923765431
	.size	"camera-abandon-animated_symbol_table_src", 16

	.type	"get-int64_symbol_table_src",@object
	.section	".rodata.cst8.get-int64_symbol_table_src","aMR",@progbits,8
	.p2align	3, 0x0
"get-int64_symbol_table_src":
	.quad	-571212388772813384
	.size	"get-int64_symbol_table_src", 8

	.type	"conversation-interactable-enable_symbol_table_src",@object
	.section	".rodata.cst16.conversation-interactable-enable_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"conversation-interactable-enable_symbol_table_src":
	.quad	4010924026564459985
	.quad	-6803722351039902803
	.size	"conversation-interactable-enable_symbol_table_src", 16

	.type	"vector-set-magnitude_symbol_table_src",@object
	.section	".rodata.cst16.vector-set-magnitude_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"vector-set-magnitude_symbol_table_src":
	.quad	-7766000234705723511
	.quad	3026091680885120756
	.size	"vector-set-magnitude_symbol_table_src", 16

	.type	"dot-product_symbol_table_src",@object
	.section	".rodata.dot-product_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"dot-product_symbol_table_src":
	.size	"dot-product_symbol_table_src", 0

	.type	"change-render-setting-value-vector_symbol_table_src",@object
	.section	".rodata.change-render-setting-value-vector_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"change-render-setting-value-vector_symbol_table_src":
	.quad	8318787222681770230
	.quad	-6109586778408437506
	.quad	2492347785598
	.quad	2492347785604
	.quad	2492347785601
	.size	"change-render-setting-value-vector_symbol_table_src", 40

	.type	"task-not-complete?_symbol_table_src",@object
	.section	".rodata.cst8.task-not-complete?_symbol_table_src","aMR",@progbits,8
	.p2align	3, 0x0
"task-not-complete?_symbol_table_src":
	.quad	8743832621443356413
	.size	"task-not-complete?_symbol_table_src", 8

	.type	"transform-world-to-localspace_symbol_table_src",@object
	.section	".rodata.cst8.transform-world-to-localspace_symbol_table_src","aMR",@progbits,8
	.p2align	3, 0x0
"transform-world-to-localspace_symbol_table_src":
	.quad	3523432566325309160
	.size	"transform-world-to-localspace_symbol_table_src", 8

	.type	"timer-to-blend-value_symbol_table_src",@object
	.section	".rodata.cst32.timer-to-blend-value_symbol_table_src","aMR",@progbits,32
	.p2align	3, 0x0
"timer-to-blend-value_symbol_table_src":
	.quad	6895426483596981631
	.quad	0
	.quad	-7712151158057785724
	.quad	3249880500923765431
	.size	"timer-to-blend-value_symbol_table_src", 32

	.type	"cinematic-get-current-time_symbol_table_src",@object
	.section	".rodata.cst16.cinematic-get-current-time_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"cinematic-get-current-time_symbol_table_src":
	.quad	-6493350602403076389
	.quad	1106247680
	.size	"cinematic-get-current-time_symbol_table_src", 16

	.type	"darray-find-remove_symbol_table_src",@object
	.section	".rodata.cst16.darray-find-remove_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"darray-find-remove_symbol_table_src":
	.quad	-4632259724434563068
	.quad	8925139681302529347
	.size	"darray-find-remove_symbol_table_src", 16

	.type	"vector-scale_symbol_table_src",@object
	.section	".rodata.cst16.vector-scale_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"vector-scale_symbol_table_src":
	.quad	-1744966098031674466
	.quad	1065353216
	.size	"vector-scale_symbol_table_src", 16

	.type	"task-is-test?_symbol_table_src",@object
	.section	".rodata.cst16.task-is-test?_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"task-is-test?_symbol_table_src":
	.quad	4627505823601477936
	.quad	-8607372249571315066
	.size	"task-is-test?_symbol_table_src", 16

	.type	"regular-timeline-task-node?_symbol_table_src",@object
	.section	".rodata.regular-timeline-task-node?_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"regular-timeline-task-node?_symbol_table_src":
	.size	"regular-timeline-task-node?_symbol_table_src", 0

	.type	"set-object-teleport_symbol_table_src",@object
	.section	".rodata.cst16.set-object-teleport_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"set-object-teleport_symbol_table_src":
	.quad	4789555171397753330
	.quad	-2616194530340952722
	.size	"set-object-teleport_symbol_table_src", 16

	.type	"set-shader-instance-wind-intensity_symbol_table_src",@object
	.section	".rodata.cst16.set-shader-instance-wind-intensity_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"set-shader-instance-wind-intensity_symbol_table_src":
	.quad	-2085764591387011864
	.quad	-2823856363275367900
	.size	"set-shader-instance-wind-intensity_symbol_table_src", 16

	.type	"invalid-symbol-var?_symbol_table_src",@object
	.section	".rodata.cst8.invalid-symbol-var?_symbol_table_src","aMR",@progbits,8
	.p2align	3, 0x0
"invalid-symbol-var?_symbol_table_src":
	.quad	-1136051897790519866
	.size	"invalid-symbol-var?_symbol_table_src", 8

	.type	"box-event-arg-point_symbol_table_src",@object
	.section	".rodata.cst16.box-event-arg-point_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"box-event-arg-point_symbol_table_src":
	.quad	3684810824690217768
	.quad	-2672655112469118099
	.size	"box-event-arg-point_symbol_table_src", 16

	.type	"task-get-full-name_symbol_table_src",@object
	.section	".rodata.cst16.task-get-full-name_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"task-get-full-name_symbol_table_src":
	.quad	4627505823601477936
	.quad	-517923468232710089
	.size	"task-get-full-name_symbol_table_src", 16

	.type	"wait-simple-npc-move-to-entry_symbol_table_src",@object
	.section	".rodata.wait-simple-npc-move-to-entry_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"wait-simple-npc-move-to-entry_symbol_table_src":
	.quad	-5739656845705241185
	.quad	-3881025521939511168
	.quad	1932324078104646333
	.quad	432021034919055241
	.quad	1584371146664193409
	.size	"wait-simple-npc-move-to-entry_symbol_table_src", 40

	.type	"task-skip-to!_symbol_table_src",@object
	.section	".rodata.task-skip-to!_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"task-skip-to!_symbol_table_src":
	.quad	4627505823601477936
	.quad	4701419364567053568
	.quad	0
	.size	"task-skip-to!_symbol_table_src", 24

	.type	"camera-fade-out-animated_symbol_table_src",@object
	.section	".rodata.cst16.camera-fade-out-animated_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"camera-fade-out-animated_symbol_table_src":
	.quad	-8117085121181574950
	.quad	3249880500923765431
	.size	"camera-fade-out-animated_symbol_table_src", 16

	.type	"sprint-bits-uint64_symbol_table_src",@object
	.section	".rodata.sprint-bits-uint64_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"sprint-bits-uint64_symbol_table_src":
	.quad	6072285182812319085
	.quad	2492347785546
	.quad	1313740198704887414
	.quad	3741915750828142429
	.quad	-3311351115081502130
	.quad	3249880500923765431
	.quad	2492347785812
	.quad	2492347785472
	.quad	2492347785525
	.quad	8618586874330430261
	.size	"sprint-bits-uint64_symbol_table_src", 80

	.type	"darray-print_symbol_table_src",@object
	.section	".rodata.darray-print_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"darray-print_symbol_table_src":
	.quad	-7966019873906053962
	.quad	-1851807429122828651
	.quad	6072285182812319085
	.quad	2492347785766
	.quad	-3311351115081502130
	.quad	4013010305452811590
	.quad	3423406688794319677
	.quad	-2664920944689591574
	.quad	2492347785473
	.quad	2492347785522
	.quad	2492347785498
	.quad	2492347785495
	.quad	2492347785501
	.quad	2492347785590
	.quad	2492347785507
	.quad	2492347785514
	.quad	2492347785504
	.quad	0
	.quad	2492347785594
	.quad	2492347785664
	.quad	2492347785689
	.quad	2492347785649
	.quad	2492347771680
	.quad	2492347785742
	.quad	2492347771696
	.quad	2492347785754
	.quad	2492347785706
	.quad	2492347785639
	.quad	2492347785615
	.quad	2492347785490
	.quad	2492347785472
	.quad	2492347785629
	.quad	2492347785607
	.quad	2492347785719
	.quad	2492347785732
	.quad	-5673227674328399655
	.size	"darray-print_symbol_table_src", 288

	.type	"flashback-task-active?_symbol_table_src",@object
	.section	".rodata.cst16.flashback-task-active?_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"flashback-task-active?_symbol_table_src":
	.quad	6284621658636359800
	.quad	8046519909153555413
	.size	"flashback-task-active?_symbol_table_src", 16

	.type	"wait-stop-animating_symbol_table_src",@object
	.section	".rodata.cst16.wait-stop-animating_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"wait-stop-animating_symbol_table_src":
	.quad	-7738336759357028456
	.quad	-3047912596559836939
	.size	"wait-stop-animating_symbol_table_src", 16

	.type	"!_symbol_table_src",@object
	.section	".rodata.!_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"!_symbol_table_src":
	.size	"!_symbol_table_src", 0

	.type	"seek-float_symbol_table_src",@object
	.section	".rodata.cst16.seek-float_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"seek-float_symbol_table_src":
	.quad	0
	.quad	3212836864
	.size	"seek-float_symbol_table_src", 16

	.type	"get-object-or-spawner-locator_symbol_table_src",@object
	.section	".rodata.get-object-or-spawner-locator_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"get-object-or-spawner-locator_symbol_table_src":
	.quad	-6870362608876642110
	.quad	61417594283941650
	.quad	-1563483893137887143
	.size	"get-object-or-spawner-locator_symbol_table_src", 24

	.type	"regular-timeline-task-active?_symbol_table_src",@object
	.section	".rodata.cst16.regular-timeline-task-active?_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"regular-timeline-task-active?_symbol_table_src":
	.quad	-7721148249613330231
	.quad	8046519909153555413
	.size	"regular-timeline-task-active?_symbol_table_src", 16

	.type	"transform-local-to-worldspace_symbol_table_src",@object
	.section	".rodata.cst8.transform-local-to-worldspace_symbol_table_src","aMR",@progbits,8
	.p2align	3, 0x0
"transform-local-to-worldspace_symbol_table_src":
	.quad	-5758420096241265658
	.size	"transform-local-to-worldspace_symbol_table_src", 8

	.type	"cloth-remove-external-collider_symbol_table_src",@object
	.section	".rodata.cst16.cloth-remove-external-collider_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"cloth-remove-external-collider_symbol_table_src":
	.quad	4789555171397753330
	.quad	-5535122524023873783
	.size	"cloth-remove-external-collider_symbol_table_src", 16

	.type	"object-in-namespace?_symbol_table_src",@object
	.section	".rodata.cst8.object-in-namespace?_symbol_table_src","aMR",@progbits,8
	.p2align	3, 0x0
"object-in-namespace?_symbol_table_src":
	.quad	4971238280069550491
	.size	"object-in-namespace?_symbol_table_src", 8

	.type	"cloth-connect-external-collider_symbol_table_src",@object
	.section	".rodata.cloth-connect-external-collider_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"cloth-connect-external-collider_symbol_table_src":
	.quad	4789555171397753330
	.quad	-4899465879314900289
	.quad	3684810824690217768
	.size	"cloth-connect-external-collider_symbol_table_src", 24

	.type	"print-int32_symbol_table_src",@object
	.section	".rodata.print-int32_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"print-int32_symbol_table_src":
	.quad	-7966019873906053962
	.quad	-1851807429122828651
	.quad	6072285182812319085
	.quad	2492347785530
	.quad	1313740198704887414
	.quad	3741915750828142429
	.quad	-3311351115081502130
	.quad	3249880500923765431
	.quad	2492347785835
	.size	"print-int32_symbol_table_src", 72

	.type	"print-float_symbol_table_src",@object
	.section	".rodata.print-float_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"print-float_symbol_table_src":
	.quad	-7966019873906053962
	.quad	-1851807429122828651
	.quad	6072285182812319085
	.quad	2492347785538
	.quad	1313740198704887414
	.quad	3741915750828142429
	.quad	-3311351115081502130
	.quad	3249880500923765431
	.quad	2492347785823
	.size	"print-float_symbol_table_src", 72

	.type	"get-object-or-spawner-rotation_symbol_table_src",@object
	.section	".rodata.cst16.get-object-or-spawner-rotation_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"get-object-or-spawner-rotation_symbol_table_src":
	.quad	-8899413508202686035
	.quad	-5739656845705241185
	.size	"get-object-or-spawner-rotation_symbol_table_src", 16

	.type	"task-complete-if-active!_symbol_table_src",@object
	.section	".rodata.task-complete-if-active!_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"task-complete-if-active!_symbol_table_src":
	.quad	4627505823601477936
	.quad	1014617123432502549
	.quad	-4779101365983111435
	.size	"task-complete-if-active!_symbol_table_src", 24

	.type	"box-event-arg-symbol_symbol_table_src",@object
	.section	".rodata.cst16.box-event-arg-symbol_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"box-event-arg-symbol_symbol_table_src":
	.quad	3684810824690217768
	.quad	3525069928581975493
	.size	"box-event-arg-symbol_symbol_table_src", 16

	.type	"simple-npc-set-config_symbol_table_src",@object
	.section	".rodata.simple-npc-set-config_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"simple-npc-set-config_symbol_table_src":
	.quad	4789555171397753330
	.quad	3110517948991553412
	.quad	3684810824690217768
	.size	"simple-npc-set-config_symbol_table_src", 24

	.type	"set-int64_symbol_table_src",@object
	.section	".rodata.cst8.set-int64_symbol_table_src","aMR",@progbits,8
	.p2align	3, 0x0
"set-int64_symbol_table_src":
	.quad	5968959451188317244
	.size	"set-int64_symbol_table_src", 8

	.type	"box-event-arg-bound-frame_symbol_table_src",@object
	.section	".rodata.cst16.box-event-arg-bound-frame_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"box-event-arg-bound-frame_symbol_table_src":
	.quad	3684810824690217768
	.quad	-5617307358675664297
	.size	"box-event-arg-bound-frame_symbol_table_src", 16

	.type	"vector-random_symbol_table_src",@object
	.section	".rodata.vector-random_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"vector-random_symbol_table_src":
	.quad	7676327678198839157
	.quad	-1744966098031674466
	.quad	1065353216
	.size	"vector-random_symbol_table_src", 24

	.type	"current-task-elapsed-time_symbol_table_src",@object
	.section	".rodata.cst32.current-task-elapsed-time_symbol_table_src","aMR",@progbits,32
	.p2align	3, 0x0
"current-task-elapsed-time_symbol_table_src":
	.quad	-2485240315990590634
	.quad	7172296021436955284
	.quad	-8782568257055231931
	.quad	0
	.size	"current-task-elapsed-time_symbol_table_src", 32

	.type	"unparent-bound-frame_symbol_table_src",@object
	.section	".rodata.unparent-bound-frame_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"unparent-bound-frame_symbol_table_src":
	.quad	-8710307833156242393
	.quad	-3881025521939511168
	.quad	-8899413508202686035
	.size	"unparent-bound-frame_symbol_table_src", 24

	.type	"get-shader-instance-blend-offset_symbol_table_src",@object
	.section	".rodata.get-shader-instance-blend-offset_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"get-shader-instance-blend-offset_symbol_table_src":
	.quad	-4412383628246461220
	.quad	4469282092045561281
	.quad	4469280992533933070
	.quad	4469279893022304859
	.quad	4469278793510676648
	.size	"get-shader-instance-blend-offset_symbol_table_src", 40

	.type	"box-event-arg-vector_symbol_table_src",@object
	.section	".rodata.cst16.box-event-arg-vector_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"box-event-arg-vector_symbol_table_src":
	.quad	3684810824690217768
	.quad	-5564767933503521010
	.size	"box-event-arg-vector_symbol_table_src", 16

	.type	"task-complete-current!_symbol_table_src",@object
	.section	".rodata.cst16.task-complete-current!_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"task-complete-current!_symbol_table_src":
	.quad	8046519909153555413
	.quad	-4779101365983111435
	.size	"task-complete-current!_symbol_table_src", 16

	.type	"get-object-or-spawner-position_symbol_table_src",@object
	.section	".rodata.cst16.get-object-or-spawner-position_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"get-object-or-spawner-position_symbol_table_src":
	.quad	-3881025521939511168
	.quad	-5739656845705241185
	.size	"get-object-or-spawner-position_symbol_table_src", 16

	.type	"camera-fade-out-animated-and-set-next-camera-dir_symbol_table_src",@object
	.section	".rodata.camera-fade-out-animated-and-set-next-camera-dir_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"camera-fade-out-animated-and-set-next-camera-dir_symbol_table_src":
	.quad	-9136043082405116733
	.quad	3741915750828142429
	.quad	3249880500923765431
	.size	"camera-fade-out-animated-and-set-next-camera-dir_symbol_table_src", 24

	.type	"log-test?_symbol_table_src",@object
	.section	".rodata.log-test?_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"log-test?_symbol_table_src":
	.size	"log-test?_symbol_table_src", 0

	.type	"spring-reset_symbol_table_src",@object
	.section	".rodata.spring-reset_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"spring-reset_symbol_table_src":
	.quad	2780616685946547256
	.quad	0
	.quad	3249880500923765431
	.size	"spring-reset_symbol_table_src", 24

	.type	"start-timer_symbol_table_src",@object
	.section	".rodata.cst8.start-timer_symbol_table_src","aMR",@progbits,8
	.p2align	3, 0x0
"start-timer_symbol_table_src":
	.quad	2780616685946547256
	.size	"start-timer_symbol_table_src", 8

	.type	"print-vector_symbol_table_src",@object
	.section	".rodata.print-vector_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"print-vector_symbol_table_src":
	.quad	-7966019873906053962
	.quad	-1851807429122828651
	.quad	6072285182812319085
	.quad	2492347785568
	.quad	1313740198704887414
	.quad	3741915750828142429
	.quad	-3311351115081502130
	.quad	3249880500923765431
	.quad	2492347785885
	.size	"print-vector_symbol_table_src", 72

	.type	"task-set-active-continue!_symbol_table_src",@object
	.section	".rodata.cst16.task-set-active-continue!_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"task-set-active-continue!_symbol_table_src":
	.quad	8046519909153555413
	.quad	3155490365585411203
	.size	"task-set-active-continue!_symbol_table_src", 16

	.type	"task-play-rogue_symbol_table_src",@object
	.section	".rodata.cst16.task-play-rogue_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"task-play-rogue_symbol_table_src":
	.quad	4627505823601477936
	.quad	4701419364567053568
	.size	"task-play-rogue_symbol_table_src", 16

	.type	"print-float-melee_symbol_table_src",@object
	.section	".rodata.cst8.print-float-melee_symbol_table_src","aMR",@progbits,8
	.p2align	3, 0x0
"print-float-melee_symbol_table_src":
	.quad	-4805973402115968277
	.size	"print-float-melee_symbol_table_src", 8

	.type	"set-shader-instance-tint_symbol_table_src",@object
	.section	".rodata.set-shader-instance-tint_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"set-shader-instance-tint_symbol_table_src":
	.quad	-2085764591387011864
	.quad	-3597818989732593268
	.quad	-3597804696081426525
	.quad	-3597801397546541892
	.quad	-2977409658073751469
	.quad	-2977388767352815460
	.quad	-2977392065887700093
	.quad	-2330247009482577918
	.quad	-2330267900203513927
	.quad	-2330264601668629294
	.quad	-1771199222758825207
	.quad	-1771213516409991950
	.quad	-1771216814944876583
	.size	"set-shader-instance-tint_symbol_table_src", 104

	.type	"camera-fade-out-auto-gen-animated_symbol_table_src",@object
	.section	".rodata.camera-fade-out-auto-gen-animated_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"camera-fade-out-auto-gen-animated_symbol_table_src":
	.quad	9146608591743818257
	.quad	3741915750828142429
	.quad	3249880500923765431
	.size	"camera-fade-out-auto-gen-animated_symbol_table_src", 24

	.type	"task-continue-active?_symbol_table_src",@object
	.section	".rodata.cst16.task-continue-active?_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"task-continue-active?_symbol_table_src":
	.quad	4627505823601477936
	.quad	8528085549934399215
	.size	"task-continue-active?_symbol_table_src", 16

	.type	"wait-random_symbol_table_src",@object
	.section	".rodata.cst16.wait-random_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"wait-random_symbol_table_src":
	.quad	6809905488770624160
	.quad	-3265080734848834707
	.size	"wait-random_symbol_table_src", 16

	.type	"task-disable!_symbol_table_src",@object
	.section	".rodata.cst16.task-disable!_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"task-disable!_symbol_table_src":
	.quad	4627505823601477936
	.quad	-5062543229031975002
	.size	"task-disable!_symbol_table_src", 16

	.type	"cloth-enable-collider_symbol_table_src",@object
	.section	".rodata.cloth-enable-collider_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"cloth-enable-collider_symbol_table_src":
	.quad	4789555171397753330
	.quad	-5115115189087745418
	.quad	3684810824690217768
	.size	"cloth-enable-collider_symbol_table_src", 24

	.type	"task-active?_symbol_table_src",@object
	.section	".rodata.cst16.task-active?_symbol_table_src","aMR",@progbits,16
	.p2align	3, 0x0
"task-active?_symbol_table_src":
	.quad	4627505823601477936
	.quad	1014617123432502549
	.size	"task-active?_symbol_table_src", 16

	.type	"get-bit-uint64_symbol_table_src",@object
	.section	".rodata.get-bit-uint64_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"get-bit-uint64_symbol_table_src":
	.size	"get-bit-uint64_symbol_table_src", 0

	.type	"conversation-interactable-disable_symbol_table_src",@object
	.section	".rodata.cst8.conversation-interactable-disable_symbol_table_src","aMR",@progbits,8
	.p2align	3, 0x0
"conversation-interactable-disable_symbol_table_src":
	.quad	-1642320195793687078
	.size	"conversation-interactable-disable_symbol_table_src", 8

	.type	"simple-npc-is-female?_symbol_table_src",@object
	.section	".rodata.cst8.simple-npc-is-female?_symbol_table_src","aMR",@progbits,8
	.p2align	3, 0x0
"simple-npc-is-female?_symbol_table_src":
	.quad	-2523998684583063419
	.size	"simple-npc-is-female?_symbol_table_src", 8

	.type	"spawn-camera_symbol_table_src",@object
	.section	".rodata.spawn-camera_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"spawn-camera_symbol_table_src":
	.quad	7097052834867614476
	.quad	-6022348500830226157
	.quad	2283337050572107970
	.quad	4789555171397753330
	.quad	-5058770882771356923
	.quad	3684810824690217768
	.size	"spawn-camera_symbol_table_src", 48

	.type	"set-shader-instance-blend-offset_symbol_table_src",@object
	.section	".rodata.set-shader-instance-blend-offset_symbol_table_src","aR",@progbits
	.p2align	3, 0x0
"set-shader-instance-blend-offset_symbol_table_src":
	.quad	-2085764591387011864
	.quad	4469282092045561281
	.quad	4469280992533933070
	.quad	4469279893022304859
	.quad	4469278793510676648
	.size	"set-shader-instance-blend-offset_symbol_table_src", 40

	.section	".note.GNU-stack","",@progbits

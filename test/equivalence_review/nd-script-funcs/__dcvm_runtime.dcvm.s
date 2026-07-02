	.file	"__dcvm_runtime"
	.text
	.globl	"is-final-build?"
	.type	"is-final-build?",@function
"is-final-build?":
	LoadU16Imm	r0, 1
	Return	r0
.Lfunc_end0:
	.size	"is-final-build?", .Lfunc_end0-"is-final-build?"

	.globl	"dc-assert-enabled?"
	.type	"dc-assert-enabled?",@function
"dc-assert-enabled?":
	LoadU16Imm	r0, 0
	Return	r0
.Lfunc_end1:
	.size	"dc-assert-enabled?", .Lfunc_end1-"dc-assert-enabled?"

	.globl	"debug-draw-line"
	.type	"debug-draw-line",@function
"debug-draw-line":
	LoadU16Imm	r0, 0
	Return	r0
.Lfunc_end2:
	.size	"debug-draw-line", .Lfunc_end2-"debug-draw-line"

	.globl	"debug-draw-locator"
	.type	"debug-draw-locator",@function
"debug-draw-locator":
	LoadU16Imm	r0, 0
	Return	r0
.Lfunc_end3:
	.size	"debug-draw-locator", .Lfunc_end3-"debug-draw-locator"

	.globl	"debug-draw-line-2d"
	.type	"debug-draw-line-2d",@function
"debug-draw-line-2d":
	LoadU16Imm	r0, 0
	Return	r0
.Lfunc_end4:
	.size	"debug-draw-line-2d", .Lfunc_end4-"debug-draw-line-2d"

	.globl	"debug-draw-string"
	.type	"debug-draw-string",@function
"debug-draw-string":
	LoadU16Imm	r0, 0
	Return	r0
.Lfunc_end5:
	.size	"debug-draw-string", .Lfunc_end5-"debug-draw-string"

	.globl	"debug-draw-string-2d"
	.type	"debug-draw-string-2d",@function
"debug-draw-string-2d":
	LoadU16Imm	r0, 0
	Return	r0
.Lfunc_end6:
	.size	"debug-draw-string-2d", .Lfunc_end6-"debug-draw-string-2d"

	.globl	"debug-draw-sphere"
	.type	"debug-draw-sphere",@function
"debug-draw-sphere":
	LoadU16Imm	r0, 0
	Return	r0
.Lfunc_end7:
	.size	"debug-draw-sphere", .Lfunc_end7-"debug-draw-sphere"

	.globl	"debug-draw-cross"
	.type	"debug-draw-cross",@function
"debug-draw-cross":
	LoadU16Imm	r0, 0
	Return	r0
.Lfunc_end8:
	.size	"debug-draw-cross", .Lfunc_end8-"debug-draw-cross"

	.globl	"debug-draw-box-2d"
	.type	"debug-draw-box-2d",@function
"debug-draw-box-2d":
	LoadU16Imm	r0, 0
	Return	r0
.Lfunc_end9:
	.size	"debug-draw-box-2d", .Lfunc_end9-"debug-draw-box-2d"

	.section	".note.GNU-stack","",@progbits

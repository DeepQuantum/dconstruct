	.file	"__dcvm_runtime"
	.text
	.globl	"is-final-build?"
	.type	"is-final-build?",@function
"is-final-build?":
	LoadU16Imm	r0, 1
	Return	r0
.Lfunc_end0:
	.size	"is-final-build?", .Lfunc_end0-"is-final-build?"

	.section	".note.GNU-stack","",@progbits

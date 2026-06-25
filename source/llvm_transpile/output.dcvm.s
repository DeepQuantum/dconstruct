	.file	"dcvm_smoke"
	.text
	.globl	add
	.type	add,@function
add:
	iadd	r0, r49, r50
	ret r0
.Lfunc_end0:
	.size	add, .Lfunc_end0-add

	.globl	sub
	.type	sub,@function
sub:
	isub	r0, r49, r50
	ret r0
.Lfunc_end1:
	.size	sub, .Lfunc_end1-sub

	.globl	mul
	.type	mul,@function
mul:
	imul	r0, r49, r50
	ret r0
.Lfunc_end2:
	.size	mul, .Lfunc_end2-mul

	.globl	sdiv
	.type	sdiv,@function
sdiv:
	idiv	r0, r49, r50
	ret r0
.Lfunc_end3:
	.size	sdiv, .Lfunc_end3-sdiv

	.globl	band
	.type	band,@function
band:
	opbitand	r0, r49, r50
	ret r0
.Lfunc_end4:
	.size	band, .Lfunc_end4-band

	.globl	bor
	.type	bor,@function
bor:
	opbitor	r0, r49, r50
	ret r0
.Lfunc_end5:
	.size	bor, .Lfunc_end5-bor

	.globl	bxor
	.type	bxor,@function
bxor:
	opbitxor	r0, r49, r50
	ret r0
.Lfunc_end6:
	.size	bxor, .Lfunc_end6-bxor

	.globl	bnor
	.type	bnor,@function
bnor:
	opbitnor	r0, r49, r50
	ret r0
.Lfunc_end7:
	.size	bnor, .Lfunc_end7-bnor

	.globl	bnot
	.type	bnot,@function
bnot:
	opbitnot	r0, r49
	ret r0
.Lfunc_end8:
	.size	bnot, .Lfunc_end8-bnot

	.globl	ineg
	.type	ineg,@function
ineg:
	ineg	r0, r49
	ret r0
.Lfunc_end9:
	.size	ineg, .Lfunc_end9-ineg

	.globl	ieq
	.type	ieq,@function
ieq:
	ieq	r0, r49, r50
	ret r0
.Lfunc_end10:
	.size	ieq, .Lfunc_end10-ieq

	.globl	ine
	.type	ine,@function
ine:
	ine	r0, r49, r50
	ret r0
.Lfunc_end11:
	.size	ine, .Lfunc_end11-ine

	.globl	igt
	.type	igt,@function
igt:
	igt	r0, r49, r50
	ret r0
.Lfunc_end12:
	.size	igt, .Lfunc_end12-igt

	.globl	ige
	.type	ige,@function
ige:
	ige	r0, r49, r50
	ret r0
.Lfunc_end13:
	.size	ige, .Lfunc_end13-ige

	.globl	ilt
	.type	ilt,@function
ilt:
	ilt	r0, r49, r50
	ret r0
.Lfunc_end14:
	.size	ilt, .Lfunc_end14-ilt

	.globl	ile
	.type	ile,@function
ile:
	ile	r0, r49, r50
	ret r0
.Lfunc_end15:
	.size	ile, .Lfunc_end15-ile

	.globl	feq
	.type	feq,@function
feq:
	feq	r0, r49, r50
	ret r0
.Lfunc_end16:
	.size	feq, .Lfunc_end16-feq

	.globl	fne
	.type	fne,@function
fne:
	fne	r0, r49, r50
	ret r0
.Lfunc_end17:
	.size	fne, .Lfunc_end17-fne

	.globl	fgt
	.type	fgt,@function
fgt:
	fgt	r0, r49, r50
	ret r0
.Lfunc_end18:
	.size	fgt, .Lfunc_end18-fgt

	.globl	fge
	.type	fge,@function
fge:
	fge	r0, r49, r50
	ret r0
.Lfunc_end19:
	.size	fge, .Lfunc_end19-fge

	.globl	flt
	.type	flt,@function
flt:
	flt	r0, r49, r50
	ret r0
.Lfunc_end20:
	.size	flt, .Lfunc_end20-flt

	.globl	fle
	.type	fle,@function
fle:
	fle	r0, r49, r50
	ret r0
.Lfunc_end21:
	.size	fle, .Lfunc_end21-fle

	.globl	small_const
	.type	small_const,@function
small_const:
	loadu16imm r0, 42
	ret r0
.Lfunc_end22:
	.size	small_const, .Lfunc_end22-small_const

	.globl	big_const
	.type	big_const,@function
big_const:
	loadstaticu64imm r0, 0
	ret r0
big_const_symbol_table:
	.quad	1311768467463790320
.Lfunc_end23:
	.size	big_const, .Lfunc_end23-big_const

	.globl	dup_const
	.type	dup_const,@function
dup_const:
	loadstaticu64imm r0, 0
	ret r0
dup_const_symbol_table:
	.quad	1311768467463790320
.Lfunc_end24:
	.size	dup_const, .Lfunc_end24-dup_const

	.globl	float_const
	.type	float_const,@function
float_const:
	loadstaticfloatimm r0, 0
	ret r0
float_const_symbol_table:
	.quad	1078530000
.Lfunc_end25:
	.size	float_const, .Lfunc_end25-float_const

	.globl	imod
	.type	imod,@function
imod:
	imod	r0, r49, r50
	ret r0
.Lfunc_end26:
	.size	imod, .Lfunc_end26-imod

	.globl	fmod
	.type	fmod,@function
fmod:
	fmod	r0, r49, r50
	ret r0
.Lfunc_end27:
	.size	fmod, .Lfunc_end27-fmod

	.globl	iabs
	.type	iabs,@function
iabs:
	iabs	r0, r49
	ret r0
.Lfunc_end28:
	.size	iabs, .Lfunc_end28-iabs

	.globl	fabs
	.type	fabs,@function
fabs:
	fabs	r0, r49
	ret r0
.Lfunc_end29:
	.size	fabs, .Lfunc_end29-fabs

	.globl	lookup_demo
	.type	lookup_demo,@function
lookup_demo:
	lookuppointer r0, 0
	ret r0
lookup_demo_symbol_table:
	.quad	-6144092013047381999
.Lfunc_end30:
	.size	lookup_demo, .Lfunc_end30-lookup_demo

	.globl	call_near
	.type	call_near,@function
call_near:
	lookuppointer r0, 0
	call r0, r0, 1
	ret r0
call_near_symbol_table:
	.quad	1311693406324658740
.Lfunc_end31:
	.size	call_near, .Lfunc_end31-call_near

	.globl	call_far
	.type	call_far,@function
call_far:
	lookuppointer r0, 0
	callff r0, r0, 1
	ret r0
call_far_symbol_table:
	.quad	6230825159313872504
.Lfunc_end32:
	.size	call_far, .Lfunc_end32-call_far

	.section	".note.GNU-stack","",@progbits

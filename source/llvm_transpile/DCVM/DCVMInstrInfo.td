class DCVMInst<dag outs, dag ins, string asmstr, list<dag> pattern = []> : Instruction {
    let Namespace = "DCVM";
    dag OutOperandList = outs;
    dag InOperandList = ins;
    string AsmString = asmstr;
    list<dag> Pattern = pattern;

    field bits<64> Inst;
    bits<8> opcode;
    let Inst{7-0} = opcode;
    let Size = 8;
}

class BasicInst<dag outs, dag ins, string asmstr, list<dag> pattern = []> : DCVMInst<outs, ins, asmstr, pattern> {
    let isReturn = 0;
    let isTerminator = 0;
    let isBarrier = 0;
    let hasSideEffects = 0;
    let mayLoad = 0;
    let mayStore = 0;
}

class RegInst<bits<8> op, int nops, list<dag> pattern = []>
    : BasicInst<(outs GPR:$dst),
                !cond(!eq(nops, 1): (ins),
                      !eq(nops, 2): (ins GPR:$src0),
                      !eq(nops, 3): (ins GPR:$src0, GPR:$src1)),
                "", pattern> {
  bits<8> dst;
  bits<8> src0;
  bits<8> src1;
  let opcode = op;
  let AsmString = !substr(!tolower(NAME), 0, !sub(!size(NAME), !sub(nops, 1))) # "\t$dst"
                  # !if(!ge(nops, 2), ", $src0", "")
                  # !if(!ge(nops, 3), ", $src1", "");
  let Inst{15-8}  = dst;
  let Inst{23-16} = !if(!ge(nops, 2), src0, 0);
  let Inst{31-24} = !if(!ge(nops, 3), src1, 0);
}


def u16imm : ImmLeaf<i64, [{
    return isUInt<16>(Imm);
}]>;

def u8imm : ImmLeaf<i64, [{
    return isUInt<8>(Imm);
}]>;

def brtarget : Operand<OtherVT>;

def LOADU16IMMri : BasicInst<
    (outs GPR:$dst),
    (ins i64imm:$imm),
    "loadu16imm $dst, $imm",
    [(set GPR:$dst, (i64 u16imm:$imm))]
> {
    bits<8> dst;
    bits<16> imm;
    let opcode = 12;
    let Inst{15-8}  = dst;
    let Inst{23-16} = imm{7-0};
    let Inst{31-24} = imm{15-8};
}

def LOADSTATICU64IMMri : BasicInst<
    (outs GPR:$dst),
    (ins i64imm:$idx),
    "loadstaticu64imm $dst, $idx"
> {
    bits<8> dst;
    bits<8> idx;
    let opcode = 74;
    let Inst{15-8}  = dst;
    let Inst{23-16} = idx;
}

def LOADSTATICFLOATIMMri : BasicInst<
    (outs GPR:$dst),
    (ins i64imm:$idx),
    "loadstaticfloatimm $dst, $idx"
> {
    bits<8> dst;
    bits<8> idx;
    let opcode = 64;
    let Inst{15-8}  = dst;
    let Inst{23-16} = idx;
}

def LOOKUPPOINTERri : BasicInst<
    (outs GPR:$dst),
    (ins i64imm:$idx),
    "lookuppointer $dst, $idx"
> {
    bits<8> dst;
    bits<8> idx;
    let opcode = 21;
    let Inst{15-8}  = dst;
    let Inst{23-16} = idx;
}
def IADDrr : RegInst<1, 3, [(set GPR:$dst, ( add GPR:$src0, GPR:$src1))]>;
def ISUBrr : RegInst<2, 3, [(set GPR:$dst, ( sub GPR:$src0, GPR:$src1))]>;
def IMULrr : RegInst<3, 3, [(set GPR:$dst, ( mul GPR:$src0, GPR:$src1))]>;
def IDIVrr : RegInst<4, 3, [(set GPR:$dst, (sdiv GPR:$src0, GPR:$src1))]>;
def FADDrr : RegInst<5, 3, [(set GPR:$dst, (fadd GPR:$src0, GPR:$src1))]>;
def FSUBrr : RegInst<6, 3, [(set GPR:$dst, (fsub GPR:$src0, GPR:$src1))]>;
def FMULrr : RegInst<7, 3, [(set GPR:$dst, (fmul GPR:$src0, GPR:$src1))]>;
def FDIVrr : RegInst<8, 3, [(set GPR:$dst, (fdiv GPR:$src0, GPR:$src1))]>;

def IEQrr : RegInst<29, 3, [(set GPR:$dst, (seteq (i64 GPR:$src0), (i64 GPR:$src1)))]>;
def IGTrr : RegInst<30, 3, [(set GPR:$dst, (setgt (i64 GPR:$src0), (i64 GPR:$src1)))]>;
def IGErr : RegInst<31, 3, [(set GPR:$dst, (setge (i64 GPR:$src0), (i64 GPR:$src1)))]>;
def ILTrr : RegInst<32, 3, [(set GPR:$dst, (setlt (i64 GPR:$src0), (i64 GPR:$src1)))]>;
def ILErr : RegInst<33, 3, [(set GPR:$dst, (setle (i64 GPR:$src0), (i64 GPR:$src1)))]>;
def INErr : RegInst<97, 3, [(set GPR:$dst, (setne (i64 GPR:$src0), (i64 GPR:$src1)))]>;

def FEQrr : RegInst<34, 3, [(set GPR:$dst, (setueq (f32 GPR:$src0), (f32 GPR:$src1)))]>;
def FGTrr : RegInst<35, 3, [(set GPR:$dst, (setugt (f32 GPR:$src0), (f32 GPR:$src1)))]>;
def FGErr : RegInst<36, 3, [(set GPR:$dst, (setuge (f32 GPR:$src0), (f32 GPR:$src1)))]>;
def FLTrr : RegInst<37, 3, [(set GPR:$dst, (setult (f32 GPR:$src0), (f32 GPR:$src1)))]>;
def FLErr : RegInst<38, 3, [(set GPR:$dst, (setule (f32 GPR:$src0), (f32 GPR:$src1)))]>;
def FNErr : RegInst<98, 3, [(set GPR:$dst, (setune (f32 GPR:$src0), (f32 GPR:$src1)))]>;

def IMODrr : RegInst<39, 3, [(set GPR:$dst, (srem (i64 GPR:$src0), (i64 GPR:$src1)))]>;
def FMODrr : RegInst<40, 3, [(set GPR:$dst, (frem (f32 GPR:$src0), (f32 GPR:$src1)))]>;

def IABSr : RegInst<41, 2, [(set GPR:$dst, (abs (i64 GPR:$src0)))]>;
def FABSr : RegInst<42, 2, [(set GPR:$dst, (fabs (f32 GPR:$src0)))]>;

def CASTINTEGERr : RegInst<25, 2, [(set GPR:$dst, (i64 (fp_to_sint (f32 GPR:$src0))))]>;
def CASTFLOATr   : RegInst<26, 2, [(set GPR:$dst, (f32 (sint_to_fp (i64 GPR:$src0))))]>;

class ArithmeticImmInst<bits<8> op, string mnemonic, SDPatternOperator node> : BasicInst<
    (outs GPR:$dst),
    (ins GPR:$src0, i64imm:$imm),
    mnemonic # " $dst, $src0, $imm",
    [(set GPR:$dst, (node (i64 GPR:$src0), u8imm:$imm))]
> {
    bits<8> dst;
    bits<8> src0;
    bits<8> imm;
    let opcode = op;
    let Inst{15-8}  = dst;
    let Inst{23-16} = src0;
    let Inst{31-24} = imm;
}

def IADDIMMri : ArithmeticImmInst<59, "iaddimm", add>;
def ISUBIMMri : ArithmeticImmInst<60, "isubimm", sub>;
def IMULIMMri : ArithmeticImmInst<61, "imulimm", mul>;
def IDIVIMMri : ArithmeticImmInst<62, "idivimm", sdiv>;

class LoadInst<bits<8> op, string mnemonic, PatFrag loadop> : BasicInst<
    (outs GPR:$dst),
    (ins GPR:$addr),
    mnemonic # " $dst, $addr",
    [(set GPR:$dst, (i64 (loadop (i64 GPR:$addr))))]
> {
    bits<8> dst;
    bits<8> addr;
    let opcode = op;
    let mayLoad = 1;
    let Inst{15-8}  = dst;
    let Inst{23-16} = addr;
}

class StoreInst<bits<8> op, string mnemonic, PatFrag storeop> : BasicInst<
    (outs),
    (ins GPR:$addr, GPR:$value),
    mnemonic # " $value, $addr",
    [(storeop (i64 GPR:$value), (i64 GPR:$addr))]
> {
    bits<8> addr;
    bits<8> value;
    let opcode = op;
    let mayStore = 1;
    let Inst{15-8}  = value;
    let Inst{23-16} = addr;
    let Inst{31-24} = value;
}

def LOADU32  : LoadInst<13, "loadu32",  zextloadi32>;
def LOADI8   : LoadInst<82, "loadi8",   sextloadi8>;
def LOADU8   : LoadInst<83, "loadu8",   zextloadi8>;
def LOADI16  : LoadInst<84, "loadi16",  sextloadi16>;
def LOADU16  : LoadInst<85, "loadu16",  zextloadi16>;
def LOADI32  : LoadInst<86, "loadi32",  sextloadi32>;
def LOADU64  : LoadInst<88, "loadu64",  load>;

def STOREU8  : StoreInst<90, "storeu8",  truncstorei8>;
def STOREU16 : StoreInst<92, "storeu16", truncstorei16>;
def STOREU32 : StoreInst<94, "storeu32", truncstorei32>;
def STOREU64 : StoreInst<96, "storeu64", store>;

def : Pat<(i64 (extloadi8  (i64 GPR:$addr))), (LOADU8  GPR:$addr)>;
def : Pat<(i64 (extloadi16 (i64 GPR:$addr))), (LOADU16 GPR:$addr)>;
def : Pat<(i64 (extloadi32 (i64 GPR:$addr))), (LOADU32 GPR:$addr)>;

def OPLOGNOTr  : RegInst<48, 2>;
def OPBITANDrr : RegInst<49, 3, [(set GPR:$dst, (and GPR:$src0, GPR:$src1))]>;
def OPBITNOTr  : RegInst<50, 2, [(set GPR:$dst, (not GPR:$src0))]>;

def OPBITORrr  : RegInst<51, 3, [(set GPR:$dst, (or GPR:$src0, GPR:$src1))]>;
def OPBITXORrr : RegInst<52, 3, [(set GPR:$dst, (xor GPR:$src0, GPR:$src1))]>;
def OPBITNORrr : RegInst<53, 3, [(set GPR:$dst, (not (or GPR:$src0, GPR:$src1)))]>;


def INEGr : RegInst<56, 2, [(set GPR:$dst, (ineg GPR:$src0))]>;
def MOVEr : RegInst<67, 2>;

def INTASHrr : RegInst<66, 3>;


def dcvm_ret : SDNode<"DCVMISD::RET", SDTypeProfile<0, 1, [SDTCisInt<0>]>, [SDNPHasChain, SDNPOptInGlue]>;

def SDT_DCVMSelect : SDTypeProfile<1, 3, [SDTCisSameAs<0, 2>, SDTCisSameAs<0, 3>, SDTCisInt<1>]>;
def dcvm_select : SDNode<"DCVMISD::SELECT", SDT_DCVMSelect>;

class SelectInst<ValueType vt> : DCVMInst<
    (outs GPR:$dst),
    (ins GPR:$cond, GPR:$t, GPR:$f),
    "# select $dst, $cond, $t, $f",
    [(set GPR:$dst, (vt (dcvm_select (i64 GPR:$cond), (vt GPR:$t), (vt GPR:$f))))]
> {
    let usesCustomInserter = 1;
    let isPseudo = 1;
    let hasSideEffects = 0;
    let opcode = 255;
}

def SELECT_I64 : SelectInst<i64>;
def SELECT_F32 : SelectInst<f32>;

def SDT_DCVMCall : SDTypeProfile<0, 2, [SDTCisVT<0, i64>, SDTCisVT<1, i64>]>;
def dcvm_call   : SDNode<"DCVMISD::CALL",   SDT_DCVMCall, [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>;
def dcvm_callff : SDNode<"DCVMISD::CALLFF", SDT_DCVMCall, [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>;

let isCall = 1, Defs = [R0] in {
    def CALL : BasicInst<
        (outs),
        (ins GPR:$func, i64imm:$nargs),
        "call r0, $func, $nargs",
        [(dcvm_call (i64 GPR:$func), (i64 timm:$nargs))]
    > {
        bits<8> func;
        bits<8> nargs;
        let opcode = 27;
        let Inst{15-8}  = 0;
        let Inst{23-16} = func;
        let Inst{31-24} = nargs;
    }

    def CALLFF : BasicInst<
        (outs),
        (ins GPR:$func, i64imm:$nargs),
        "callff r0, $func, $nargs",
        [(dcvm_callff (i64 GPR:$func), (i64 timm:$nargs))]
    > {
        bits<8> func;
        bits<8> nargs;
        let opcode = 28;
        let Inst{15-8}  = 0;
        let Inst{23-16} = func;
        let Inst{31-24} = nargs;
    }
}

let isBranch = 1, isTerminator = 1, isBarrier = 1, hasSideEffects = 0, mayLoad = 0, mayStore = 0 in
def BR : DCVMInst<
    (outs),
    (ins brtarget:$target),
    "branch $target",
    [(br bb:$target)]
> {
    bits<16> target;
    let opcode = 45;
    let Inst{15-8}  = target{7-0};
    let Inst{31-24} = target{15-8};
}

let isBranch = 1, isTerminator = 1, hasSideEffects = 0, mayLoad = 0, mayStore = 0 in {
    def BRANCHIF : DCVMInst<
        (outs),
        (ins brtarget:$target, GPR:$cond),
        "branchif $target, $cond",
        [(brcond (i64 GPR:$cond), bb:$target)]
    > {
        bits<16> target;
        bits<8> cond;
        let opcode = 46;
        let Inst{15-8}  = target{7-0};
        let Inst{23-16} = cond;
        let Inst{31-24} = target{15-8};
    }

    def BRANCHIFNOT : DCVMInst<
        (outs),
        (ins brtarget:$target, GPR:$cond),
        "branchifnot $target, $cond"
    > {
        bits<16> target;
        bits<8> cond;
        let opcode = 47;
        let Inst{15-8}  = target{7-0};
        let Inst{23-16} = cond;
        let Inst{31-24} = target{15-8};
    }
}

let isReturn = 1, isTerminator = 1, isBarrier = 1, hasSideEffects = 0, mayLoad = 0, mayStore = 0 in
def RET : DCVMInst<
    (outs),
    (ins GPR:$value),
    "ret $value",
    [(dcvm_ret GPR:$value)]
> {
    bits<8> value;
    let opcode = 0;
    let Inst{15-8} = value;
    let Inst{23-16} = value;
}

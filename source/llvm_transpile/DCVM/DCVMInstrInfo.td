class DCVMInst<dag outs, dag ins, string asmstr, list<dag> pattern = []> : Instruction {
    let Namespace = "DCVM";
    dag OutOperandList = outs;
    dag InOperandList = ins;
    string AsmString = asmstr;
    list<dag> Pattern = pattern;
}

class BasicInst<dag outs, dag ins, string asmstr, list<dag> pattern = []> : DCVMInst<outs, ins, asmstr, pattern> {
    let isReturn = 0;
    let isTerminator = 0;
    let isBarrier = 0;
    let hasSideEffects = 0;
    let mayLoad = 0;
    let mayStore = 0;
}

def MOVErr : BasicInst<
    (outs GPR:$dst),
    (ins GPR:$src),
    "move $dst, $src"
>;

def u16imm : ImmLeaf<i64, [{
    return Imm.isUInt<16>();
}]>;

def LOADU16IMMri : BasicInst<
    (outs GPR:$dst),
    (ins i64imm:$imm),
    "loadu16imm $dst, $imm",
    [(set GPR:$dst, (i64 u16imm:$imm))]
>;

def IADDrr : BasicInst<
    (outs GPR:$dst),
    (ins GPR:$lhs, GPR:$rhs),
    "iadd $dst, $lhs, $rhs",
    [(set GPR:$dst, (add GPR:$lhs, GPR:$rhs))]
>;

let isReturn = 1, isTerminator = 1, isBarrier = 1, hasSideEffects = 0, mayLoad = 0, mayStore = 0 in
def RET : DCVMInst<
    (outs),
    (ins GPR:$value),
    "ret $value"
>;

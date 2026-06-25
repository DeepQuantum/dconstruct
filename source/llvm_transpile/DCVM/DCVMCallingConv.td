defvar CC_DCVM_GPRegs = !foreach(i, !range(0, ARG_REGISTER_START), !cast<Register>("R"#i));
defvar CC_DCVM_ArgRegs = !foreach(i, !range(ARG_REGISTER_START, MAX_REGISTER), !cast<Register>("R"#i));

def CC_DCVM : CallingConv<[
    CCIfType<[i1, i8, i16, i32], CCPromoteToType<i64>>,
    CCIfType<[iPTR], CCBitConvertToType<i64>>,
    CCIfType<[i64, f32], CCAssignToReg<CC_DCVM_ArgRegs>>
]>;

def RetCC_DCVM : CallingConv<[
    CCIfType<[i1, i8, i16, i32], CCPromoteToType<i64>>,
    CCIfType<[iPTR], CCBitConvertToType<i64>>,
    CCIfType<[i64, f32], CCAssignToReg<[R0]>>
]>;

def CSR_DCVM : CalleeSavedRegs<(add)>;
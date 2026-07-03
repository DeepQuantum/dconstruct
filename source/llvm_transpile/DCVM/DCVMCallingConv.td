defvar CC_DCVM_GPRegs = !foreach(i, !range(0, ARG_REGISTER_START), !cast<Register>("R"#i));
defvar CC_DCVM_ArgRegs = !foreach(i, !range(ARG_REGISTER_START, !add(ARG_REGISTER_END, 1)), !cast<Register>("R"#i));

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

// SaveList intentionally stays empty: the VM gives every invocation its own
// register frame, so a callee never has to save anything (and there is no
// stack to save to). Calls clobber only r0 (the return value, an explicit
// def on the call instructions); everything else, including the argument
// window r49-r65, survives a call, so a call site may reuse an argument
// value that is already in place from an earlier call.
def CSR_DCVM : CalleeSavedRegs<(add)>;

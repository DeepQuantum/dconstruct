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
// stack to save to). The preserved mask below is caller-side knowledge only:
// r1-r48 survive a call, r0 receives the return value, and the argument
// window r49-r65 must be treated as clobbered so no value is ever kept there
// across a call and every call site rewrites its full argument list.
def CSR_DCVM : CalleeSavedRegs<(add)>;
def CSR_DCVM_Preserved : CalleeSavedRegs<(add (sequence "R%u", 1, 48))>;

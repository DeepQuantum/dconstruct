class DCVMReg<bits<16> enc, string n> : Register<n> {
    let HWEncoding = enc;
    let Namespace = "DCVM";
}

foreach i = 0...MAX_REGISTER in
    def R#i : DCVMReg<i, "r"#i>;

// r0-r48 is the only space for general evaluation, exactly like the original
// compiler's output. r49-r65 is the argument window: it is written right
// before a call and is otherwise dead, so it must never hold a general value.
// No instruction operand and no virtual register ever uses the ARG class, so
// the allocator cannot place a general value there; the class still has to be
// allocatable, because an unallocatable def-free physreg counts as a constant
// (MachineRegisterInfo::isConstantPhysReg) and MachineSink would then sink
// live-in argument copies out of the entry block, even across calls.
def GPR : RegisterClass<"DCVM", [i64, i32, f32], 64, (sequence "R%u", 0, GP_REGISTER_END)>;

def ARG : RegisterClass<"DCVM", [i64, i32, f32], 64, (sequence "R%u", ARG_REGISTER_START, ARG_REGISTER_END)>;

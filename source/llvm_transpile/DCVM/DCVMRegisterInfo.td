class DCVMReg<bits<16> enc, string n> : Register<n> {
    let HWEncoding = enc;
    let Namespace = "DCVM";
}

foreach i = 0...ARG_REGISTER_END in
    def R#i : DCVMReg<i, "r"#i>;

def GPR : RegisterClass<"DCVM", [i64, i32, f32], 64, (sequence "R%u", 0, GP_REGISTER_END)>;

def ARG : RegisterClass<"DCVM", [i64, i32, f32], 64, (sequence "R%u", ARG_REGISTER_START, ARG_REGISTER_END)>;

class DCVMReg<bits<16> enc, string n> : Register<n> {
  let HWEncoding = enc;
  let Namespace = "DCVM";
}

foreach i = 0...MAX_REGISTER in
  def R#i : DCVMReg<i, "r"#i>;

def GPR : RegisterClass<"DCVM", [i64], 64, (sequence "R%u", 0, MAX_REGISTER)>;

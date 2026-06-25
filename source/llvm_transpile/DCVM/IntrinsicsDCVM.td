let TargetPrefix = "dcvm" in {
  def int_dcvm_lookup : Intrinsic<[llvm_ptr_ty], [llvm_i64_ty], [IntrNoMem, IntrSpeculatable, IntrWillReturn]>;
}

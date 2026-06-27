let TargetPrefix = "dcvm" in {
  def int_dcvm_lookup : Intrinsic<[llvm_ptr_ty], [llvm_i64_ty], [IntrNoMem, IntrSpeculatable, IntrWillReturn]>;
  def int_dcvm_intash : Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem, IntrSpeculatable, IntrWillReturn]>;
  def int_dcvm_static_pointer : Intrinsic<[llvm_ptr_ty], [llvm_i64_ty], [IntrNoMem, IntrSpeculatable, IntrWillReturn]>;
  def int_dcvm_load_static_int : Intrinsic<[llvm_i64_ty], [llvm_i64_ty], [IntrNoMem, IntrSpeculatable, IntrWillReturn]>;
  def int_dcvm_load_static_float : Intrinsic<[llvm_float_ty], [llvm_i64_ty], [IntrNoMem, IntrSpeculatable, IntrWillReturn]>;
  def int_dcvm_load_static_pointer : Intrinsic<[llvm_ptr_ty], [llvm_i64_ty], [IntrNoMem, IntrSpeculatable, IntrWillReturn]>;
}

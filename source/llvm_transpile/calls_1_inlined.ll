; ModuleID = 'call_smoke'
source_filename = "call_smoke"
target triple = "x86_64-pc-windows-msvc"

; Function Attrs: mustprogress nofree noinline norecurse nosync nounwind willreturn memory(none)
define i64 @helper_kept(i64 %0) local_unnamed_addr #0 !dcvm.sid_distance !0 {
entry:
  %1 = mul i64 %0, 3
  ret i64 %1
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define i64 @caller(i64 %0) local_unnamed_addr #1 {
entry:
  %1 = add i64 %0, 1
  %b = tail call i64 @helper_kept(i64 %1)
  %c = tail call i64 @helper_kept(i64 %b)
  %2 = add i64 %c, %b
  ret i64 %2
}

attributes #0 = { mustprogress nofree noinline norecurse nosync nounwind willreturn memory(none) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }

!0 = !{!"sid", i64 2459565876494606882, !"distance", !"far"}

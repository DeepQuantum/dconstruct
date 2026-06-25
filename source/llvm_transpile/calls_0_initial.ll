; ModuleID = 'call_smoke'
source_filename = "call_smoke"

; Function Attrs: alwaysinline
define internal i64 @helper_inlined(i64 %0) #0 !dcvm.sid_distance !0 {
entry:
  %1 = add i64 %0, 1
  ret i64 %1
}

; Function Attrs: noinline
define i64 @helper_kept(i64 %0) #1 !dcvm.sid_distance !1 {
entry:
  %1 = mul i64 %0, 3
  ret i64 %1
}

define i64 @caller(i64 %0) {
entry:
  %a = call i64 @helper_inlined(i64 %0)
  %b = call i64 @helper_kept(i64 %a)
  %c = call i64 @helper_kept(i64 %b)
  %1 = add i64 %b, %c
  ret i64 %1
}

attributes #0 = { alwaysinline }
attributes #1 = { noinline }

!0 = !{!"sid", i64 1229782938247303441, !"distance", !"far"}
!1 = !{!"sid", i64 2459565876494606882, !"distance", !"far"}

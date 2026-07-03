import re
import sys
from collections import Counter, defaultdict

COST = {}
def setc(names, c):
    for n in names.split():
        COST[n] = c

setc("IAdd ISub IMul IDiv IMod INeg IAbs IntAsh IAddImm ISubImm IMulImm IDivImm IMulImmri", 1.0)
setc("FAdd FSub FMul FDiv FMod FNeg FAbs", 1.0)
setc("IEqual INotEqual IGreaterThan IGreaterThanEqual ILessThan ILessThanEqual", 1.0)
setc("FEqual FNotEqual FGreaterThan FGreaterThanEqual FLessThan FLessThanEqual", 1.0)
setc("OpLogNot OpLogAnd OpLogOr OpBitAnd OpBitOr OpBitXor OpBitNot OpBitNor", 1.0)
setc("Branch BranchIf BranchIfNot LoadU16Imm LoadParamCnt CastInteger CastFloat AssertPointer", 1.0)
setc("Move MoveInt MovePointer MoveFloat", 1.3)
setc("LoadI8 LoadU8 LoadI16 LoadU16 LoadI32 LoadU32 LoadI64 LoadU64 LoadFloat LoadPointer", 1.3)
setc("StoreI8 StoreU8 StoreI16 StoreU16 StoreI32 StoreU32 StoreInt StoreI64 StoreU64 StoreFloat StorePointer StoreArray", 1.5)
setc("LoadStaticInt LoadStaticFloat LoadStaticPointer LoadStaticI8Imm LoadStaticU8Imm "
     "LoadStaticI16Imm LoadStaticU16Imm LoadStaticI32Imm LoadStaticU32Imm "
     "LoadStaticI64Imm LoadStaticU64Imm LoadStaticFloatImm LoadStaticPointerImm", 1.3)
setc("LookupInt LookupFloat LookupPointer", 60.0)
setc("Call", 45.0)
setc("CallFf", 15.0)
setc("Return", 3.0)

CAT = {}
for op in COST:
    if op.startswith("Lookup"):
        CAT[op] = "lookup"
    elif op == "Call":
        CAT[op] = "call"
    elif op == "CallFf":
        CAT[op] = "callff"
    elif op.startswith("Move"):
        CAT[op] = "move"
    elif op.startswith("LoadStatic"):
        CAT[op] = "static"
    elif op.startswith(("Load", "Store")) and not op.endswith("Imm") or op == "StoreArray":
        CAT[op] = "mem"
    else:
        CAT[op] = "other"
CAT["LoadU16Imm"] = "other"


def parse_asm(path):
    funcs = {}
    cur = None
    mode = None
    hdr_named = re.compile(r"^([^\s=]+) = script-lambda \[(0x[0-9A-F]+)\] \{")
    hdr_lambda = re.compile(r"^\s+lambda (\S+) \{")
    instr_re = re.compile(r"^\s+([0-9A-F]{4})\s+0x[0-9A-F]+\s+(?:[0-9A-F]{2} ){4}\s+(\S+)\s*(.*)$")
    label_re = re.compile(r"^\s+L_(\d+):")
    for line in open(path, encoding="utf-8", errors="replace"):
        line = line.rstrip("\n")
        m = hdr_named.match(line) or hdr_lambda.match(line)
        if m:
            cur = {"instrs": [], "labels": {}}
            funcs[m.group(1)] = cur
            mode = "code"
            continue
        if cur is None:
            continue
        if "SYMBOL TABLE" in line:
            mode = None
            continue
        if mode == "code":
            lm = label_re.match(line)
            if lm:
                cur["labels"][int(lm.group(1))] = len(cur["instrs"])
                continue
            m = instr_re.match(line)
            if m:
                cur["instrs"].append((m.group(2), m.group(3)))
    return funcs


def parse_s(path):
    funcs = {}
    cur = None
    for line in open(path, encoding="utf-8", errors="replace"):
        line = line.rstrip("\n")
        m = re.match(r'^"(.+)":\s*$', line)
        if m:
            name = m.group(1)
            if name.endswith(("_symbol_table", "_pointer_map", "_symbol_table_src")):
                cur = None
                continue
            cur = {"instrs": [], "labels": {}}
            funcs[name] = cur
            continue
        if cur is None:
            continue
        m = re.match(r"^(\.LBB\S+):", line)
        if m:
            cur["labels"][m.group(1)] = len(cur["instrs"])
            continue
        m = re.match(r"^\t(\S+)\t?\s*(.*)$", line)
        if m and m.group(1) in COST:
            cur["instrs"].append((m.group(1), m.group(2)))
        elif m and m.group(1) not in (".globl", ".type", ".text", ".file", ".size",
                                      ".section", ".quad", ".byte", ".p2align", ".data"):
            if not m.group(1).startswith("."):
                print(f"WARN unknown op {m.group(1)!r} in {path}", file=sys.stderr)
    return funcs


def loop_weights_asm(fn):
    n = len(fn["instrs"])
    w = [1.0] * n
    for i, (op, operands) in enumerate(fn["instrs"]):
        if op in ("Branch", "BranchIf", "BranchIfNot"):
            m = re.search(r"0x([0-9A-Fa-f]+)", operands)
            if m:
                tgt = int(m.group(1), 16)
                if tgt <= i:
                    for j in range(tgt, i + 1):
                        w[j] = 10.0
    return w


def loop_weights_s(fn):
    n = len(fn["instrs"])
    w = [1.0] * n
    for i, (op, operands) in enumerate(fn["instrs"]):
        if op in ("Branch", "BranchIf", "BranchIfNot"):
            m = re.match(r"(\.LBB\S+)", operands)
            if m:
                tgt = fn["labels"].get(m.group(1))
                if tgt is not None and tgt <= i:
                    for j in range(tgt, i + 1):
                        w[j] = 10.0
    return w


def score(fn, weights):
    static_cost = 0.0
    dyn_cost = 0.0
    cats = Counter()
    cat_cost = Counter()
    for (op, _), w in zip(fn["instrs"], weights):
        c = COST.get(op, 1.0)
        static_cost += c
        dyn_cost += c * w
        cats[CAT.get(op, "other")] += 1
        cat_cost[CAT.get(op, "other")] += c * w
    return static_cost, dyn_cost, cats, cat_cost


def arg_window_moves(fn):
    n = 0
    for op, operands in fn["instrs"]:
        if op.startswith("Move"):
            m = re.match(r"r(\d+)\s*,", operands)
            if m and int(m.group(1)) >= 49:
                n += 1
    return n


def entry_arg_copies(fn):
    n = 0
    for op, operands in fn["instrs"]:
        if op.startswith("Move"):
            m = re.match(r"r(\d+)\s*,\s*r(\d+)", operands)
            if m and int(m.group(2)) >= 49 and int(m.group(1)) < 49:
                n += 1
                continue
        break
    return n


def audit(asm_path, s_path, sample=None):
    orig = parse_asm(asm_path)
    gen = parse_s(s_path)
    rows = []
    for name in gen:
        if name not in orig:
            continue
        o, g = orig[name], gen[name]
        ow, gw = loop_weights_asm(o), loop_weights_s(g)
        os_, od, oc, occ = score(o, ow)
        gs, gd, gc, gcc = score(g, gw)
        rows.append({
            "name": name,
            "o_n": len(o["instrs"]), "g_n": len(g["instrs"]),
            "o_static": os_, "g_static": gs,
            "o_dyn": od, "g_dyn": gd,
            "o_cats": oc, "g_cats": gc,
            "o_argmoves": arg_window_moves(o), "g_argmoves": arg_window_moves(g),
            "o_entry": entry_arg_copies(o),
            "o_calls": oc["call"] + oc["callff"], "g_calls": gc["call"] + gc["callff"],
        })
    return orig, gen, rows


def fmt_row(r):
    ratio_s = r["g_static"] / r["o_static"] if r["o_static"] else 0
    ratio_d = r["g_dyn"] / r["o_dyn"] if r["o_dyn"] else 0
    return (f"{r['name'][:52]:<52} {r['o_n']:>5} {r['g_n']:>5} "
            f"{ratio_s:>6.2f} {ratio_d:>6.2f}  "
            f"lk {r['o_cats']['lookup']:>3}->{r['g_cats']['lookup']:<3} "
            f"call {r['o_cats']['call']:>3}->{r['g_cats']['call']:<3} "
            f"ff {r['o_cats']['callff']:>3}->{r['g_cats']['callff']:<3} "
            f"mv {r['o_cats']['move']:>4}->{r['g_cats']['move']:<4} "
            f"argmv {r['o_argmoves']:>4}->{r['g_argmoves']:<4}")


if __name__ == "__main__":
    asm_path, s_path = sys.argv[1], sys.argv[2]
    orig, gen, rows = audit(asm_path, s_path)
    rows.sort(key=lambda r: -r["o_n"])
    print(f"module: {asm_path}")
    print(f"orig funcs: {len(orig)}  gen funcs: {len(gen)}  matched: {len(rows)}")
    tot_os = sum(r["o_static"] for r in rows)
    tot_gs = sum(r["g_static"] for r in rows)
    tot_od = sum(r["o_dyn"] for r in rows)
    tot_gd = sum(r["g_dyn"] for r in rows)
    tot_on = sum(r["o_n"] for r in rows)
    tot_gn = sum(r["g_n"] for r in rows)
    print(f"matched totals: instrs {tot_on}->{tot_gn} ({tot_gn/tot_on:.2f}x)  "
          f"static cost {tot_os:.0f}->{tot_gs:.0f} ({tot_gs/tot_os:.2f}x)  "
          f"loop-weighted cost {tot_od:.0f}->{tot_gd:.0f} ({tot_gd/tot_od:.2f}x)")
    oc = Counter()
    gc = Counter()
    oam = sum(r["o_argmoves"] for r in rows)
    gam = sum(r["g_argmoves"] for r in rows)
    oec = sum(r["o_entry"] for r in rows)
    for r in rows:
        oc.update(r["o_cats"])
        gc.update(r["g_cats"])
    print(f"category counts (matched funcs): " +
          "  ".join(f"{k} {oc[k]}->{gc[k]}" for k in ("lookup", "call", "callff", "move", "mem", "static", "other")))
    print(f"arg-window moves {oam}->{gam}  orig entry arg copies: {oec}")
    print()
    hdr = (f"{'function':<52} {'oIns':>5} {'gIns':>5} {'stRat':>6} {'dynRat':>6}")
    print(hdr)
    for r in rows[:25]:
        print(fmt_row(r))
    print("...")
    small = [r for r in rows if 5 <= r["o_n"] <= 40]
    for r in small[-15:]:
        print(fmt_row(r))

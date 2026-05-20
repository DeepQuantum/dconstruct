import struct
import pathlib
import idaapi  # type: ignore
import idautils  # type: ignore
import idc  # type: ignore
import ida_bytes  # type: ignore
import ida_auto  # type: ignore

SIDBASE_PATH = pathlib.Path(r"C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\fixtures\dc\sidbase_new_fixed.bin")


def load_sidbase(path):
    with open(path, "rb") as f:
        data = f.read()
    n = struct.unpack_from("<Q", data, 0)[0]
    table_off = 8
    out = {}
    for i in range(n - 1):
        h, s_off = struct.unpack_from("<QQ", data, table_off + i * 16)
        end = data.index(b"\x00", s_off)
        out[h] = data[s_off:end].decode("utf-8", errors="replace")
    return out


def apply_to_code(hashes):
    candidates = []
    get_operand_type = idc.get_operand_type
    get_operand_value = idc.get_operand_value
    print_insn_mnem = idc.print_insn_mnem
    o_imm = idc.o_imm
    for seg_ea in idautils.Segments():
        seg = idaapi.getseg(seg_ea)
        if seg.perm & idaapi.SEGPERM_EXEC == 0:
            continue
        for head in idautils.Heads(seg.start_ea, seg.end_ea):
            if print_insn_mnem(head) != "mov":
                continue
            for op_idx in range(2):
                if get_operand_type(head, op_idx) != o_imm:
                    continue
                val = get_operand_value(head, op_idx) & 0xFFFFFFFFFFFFFFFF
                if val < 0x100000000:
                    continue
                name = hashes.get(val)
                if name is not None:
                    candidates.append((head, op_idx, name))
    print(f"[sidbase] {len(candidates)} code candidates to tag")
    count = 0
    set_forced_operand = ida_bytes.set_forced_operand
    ida_auto.enable_auto(False)
    try:
        for ea, op_idx, name in candidates:
            if set_forced_operand(ea, op_idx, name):
                count += 1
    finally:
        ida_auto.enable_auto(True)
    return count


def apply_to_data(hashes):
    candidates = []
    for seg_ea in idautils.Segments():
        seg = idaapi.getseg(seg_ea)
        if seg.perm & idaapi.SEGPERM_EXEC:
            continue
        start = (seg.start_ea + 7) & ~7
        end = seg.end_ea & ~7
        if end <= start:
            continue
        buf = ida_bytes.get_bytes(start, end - start)
        if buf is None:
            continue
        for offset, (val,) in zip(range(0, len(buf), 8), struct.iter_unpack("<Q", buf)):
            name = hashes.get(val)
            if name is not None:
                candidates.append((start + offset, name))
    print(f"[sidbase] {len(candidates)} data candidates to tag")
    count = 0
    set_forced_operand = ida_bytes.set_forced_operand
    ida_auto.enable_auto(False)
    try:
        for ea, name in candidates:
            if set_forced_operand(ea, 0, name):
                count += 1
    finally:
        ida_auto.enable_auto(True)
    return count


def main():
    if not SIDBASE_PATH.exists():
        print(f"[sidbase] not found: {SIDBASE_PATH}")
        return

    print(f"[sidbase] loading {SIDBASE_PATH}")
    hashes = load_sidbase(SIDBASE_PATH)
    print(f"[sidbase] {len(hashes)} entries")

    idaapi.show_wait_box("Applying sidbase...")
    try:
        code_hits = apply_to_code(hashes)
        print(f"[sidbase] tagged {code_hits} immediate operands")
        data_hits = apply_to_data(hashes)
        print(f"[sidbase] tagged {data_hits} data qwords")
    finally:
        idaapi.hide_wait_box()


if __name__ == "__main__":
    main()

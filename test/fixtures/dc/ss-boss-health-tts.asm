####################################################################################################
#                                                                                                  #
#   DeepQuantum's DC Disassembler ver. beta_3                                                      #
#   Listing for file: ss-boss-health-tts.bin                                                       #
#   Filesize: 14804 bytes                                                                          #
#                                                                                                  #
####################################################################################################



##############################  ENTRY 0  ##############################

#142211B6C3E9D2D7 = script-lambda [0x00078] {
    [1 args]
    0000   0x0000C8   43 00 31 00   Move                 r0, r49       r0 = arg_0
    0001   0x0000D0   15 01 00 00   LookupPointer        r1, 0         r1 = ST[0] -> <darray-count>
    0002   0x0000D8   15 02 01 00   LookupPointer        r2, 1         r2 = ST[1] -> <get-symbol>
    0003   0x0000E0   4A 03 02 00   LoadStaticU64Imm     r3, 2         r3 = ST[2] -> <#D2F26D6B19D49A54>
    0004   0x0000E8   4A 04 03 00   LoadStaticU64Imm     r4, 3         r4 = ST[3] -> <self>
    0005   0x0000F0   43 31 03 00   Move                 r49, r3       r49 = #D2F26D6B19D49A54
    0006   0x0000F8   43 32 04 00   Move                 r50, r4       r50 = self
    0007   0x000100   1C 02 02 02   CallFf               r2, r2, 2     r2 = get-symbol(#D2F26D6B19D49A54, self)
    0008   0x000108   43 31 02 00   Move                 r49, r2       r49 = RET_get-symbol
    0009   0x000110   1C 01 01 01   CallFf               r1, r1, 1     r1 = darray-count(RET_get-symbol)
    000A   0x000118   0C 02 00 00   LoadU16Imm           r2, 0         r2 = 0
    000B   0x000120   1D 01 01 02   IEqual               r1, r1, r2    r1 = r1 == r2
    000C   0x000128   30 01 01 00   OpLogNot             r1, r1        r1 = !RET_darray-count
    000D   0x000130   2F 92 01 00   BranchIfNot          r1, 0x92      IF NOT r1 => L_0
    000E   0x000138   0C 01 00 00   LoadU16Imm           r1, 0         r1 = 0
    000F   0x000140   15 02 00 00   LookupPointer        r2, 0         r2 = ST[0] -> <darray-count>
    0010   0x000148   15 03 01 00   LookupPointer        r3, 1         r3 = ST[1] -> <get-symbol>
    0011   0x000150   4A 04 02 00   LoadStaticU64Imm     r4, 2         r4 = ST[2] -> <#D2F26D6B19D49A54>
    0012   0x000158   4A 05 03 00   LoadStaticU64Imm     r5, 3         r5 = ST[3] -> <self>
    0013   0x000160   43 31 04 00   Move                 r49, r4       r49 = #D2F26D6B19D49A54
    0014   0x000168   43 32 05 00   Move                 r50, r5       r50 = self
    0015   0x000170   1C 03 03 02   CallFf               r3, r3, 2     r3 = get-symbol(#D2F26D6B19D49A54, self)
    0016   0x000178   43 31 03 00   Move                 r49, r3       r49 = RET_get-symbol
    0017   0x000180   1C 02 02 01   CallFf               r2, r2, 1     r2 = darray-count(RET_get-symbol)
    0018   0x000188   15 03 04 00   LookupPointer        r3, 4         r3 = ST[4] -> <begin-foreach>
    0019   0x000190   1C 03 03 00   CallFf               r3, r3, 0     r3 = begin-foreach()
  L_9:
    001A   0x000198   43 03 01 00   Move                 r3, r1        r3 = 0
    001B   0x0001A0   43 04 02 00   Move                 r4, r2        r4 = RET_darray-count
    001C   0x0001A8   20 03 03 04   ILessThan            r3, r3, r4    r3 = r3 < r4
    001D   0x0001B0   2F 85 03 00   BranchIfNot          r3, 0x85      IF NOT r3 => L_1
    001E   0x0001B8   15 03 05 00   LookupPointer        r3, 5         r3 = ST[5] -> <darray-at>
    001F   0x0001C0   15 04 01 00   LookupPointer        r4, 1         r4 = ST[1] -> <get-symbol>
    0020   0x0001C8   4A 05 02 00   LoadStaticU64Imm     r5, 2         r5 = ST[2] -> <#D2F26D6B19D49A54>
    0021   0x0001D0   4A 06 03 00   LoadStaticU64Imm     r6, 3         r6 = ST[3] -> <self>
    0022   0x0001D8   43 31 05 00   Move                 r49, r5       r49 = #D2F26D6B19D49A54
    0023   0x0001E0   43 32 06 00   Move                 r50, r6       r50 = self
    0024   0x0001E8   1C 04 04 02   CallFf               r4, r4, 2     r4 = get-symbol(#D2F26D6B19D49A54, self)
    0025   0x0001F0   43 05 01 00   Move                 r5, r1        r5 = 0
    0026   0x0001F8   43 31 04 00   Move                 r49, r4       r49 = RET_get-symbol
    0027   0x000200   43 32 05 00   Move                 r50, r5       r50 = 0
    0028   0x000208   1C 03 03 02   CallFf               r3, r3, 2     r3 = darray-at(RET_get-symbol, 0)
    0029   0x000210   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    002A   0x000218   2F 31 04 00   BranchIfNot          r4, 0x36      IF NOT r4 => L_2
    002B   0x000220   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    002C   0x000228   5D 04 00 00   AssertPointer        r4            r4 != nullptr
    002D   0x000230   3B 04 04 0C   IAddImm              r4, r4, 12    r4 = RET_darray-at + 12 -> <RET_(null)>
    002E   0x000238   4E 04 04 00   LoadU16              r4, [r4]      r4 = *(u16*)RET_darray-at
    002F   0x000240   0C 05 01 00   LoadU16Imm           r5, 1         r5 = 1
    0030   0x000248   1D 04 04 05   IEqual               r4, r4, r5    r4 = r4 == r5
    0031   0x000250   2F 36 04 00   BranchIfNot          r4, 0x36      IF NOT r4 => L_2
    0032   0x000258   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    0033   0x000260   5D 04 00 00   AssertPointer        r4            r4 != nullptr
    0034   0x000268   4C 04 04 00   LoadU8               r4, [r4]      r4 = *(u8*)RET_darray-at
    0035   0x000270   2D 79 00 00   Branch               0x79          GOTO => L_3
  L_2:
    0036   0x000278   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    0037   0x000280   2F 3E 04 00   BranchIfNot          r4, 0x43      IF NOT r4 => L_4
    0038   0x000288   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    0039   0x000290   5D 04 00 00   AssertPointer        r4            r4 != nullptr
    003A   0x000298   3B 04 04 0C   IAddImm              r4, r4, 12    r4 = RET_darray-at + 12 -> <RET_(null)>
    003B   0x0002A0   4E 04 04 00   LoadU16              r4, [r4]      r4 = *(u16*)RET_darray-at
    003C   0x0002A8   0C 05 02 00   LoadU16Imm           r5, 2         r5 = 2
    003D   0x0002B0   1D 04 04 05   IEqual               r4, r4, r5    r4 = r4 == r5
    003E   0x0002B8   2F 43 04 00   BranchIfNot          r4, 0x43      IF NOT r4 => L_4
    003F   0x0002C0   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    0040   0x0002C8   5D 04 00 00   AssertPointer        r4            r4 != nullptr
    0041   0x0002D0   4F 04 04 00   LoadI32              r4, [r4]      r4 = *(i32*)RET_darray-at
    0042   0x0002D8   2D 79 00 00   Branch               0x79          GOTO => L_3
  L_4:
    0043   0x0002E0   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    0044   0x0002E8   2F 4B 04 00   BranchIfNot          r4, 0x50      IF NOT r4 => L_5
    0045   0x0002F0   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    0046   0x0002F8   5D 04 00 00   AssertPointer        r4            r4 != nullptr
    0047   0x000300   3B 04 04 0C   IAddImm              r4, r4, 12    r4 = RET_darray-at + 12 -> <RET_(null)>
    0048   0x000308   4E 04 04 00   LoadU16              r4, [r4]      r4 = *(u16*)RET_darray-at
    0049   0x000310   0C 05 03 00   LoadU16Imm           r5, 3         r5 = 3
    004A   0x000318   1D 04 04 05   IEqual               r4, r4, r5    r4 = r4 == r5
    004B   0x000320   2F 50 04 00   BranchIfNot          r4, 0x50      IF NOT r4 => L_5
    004C   0x000328   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    004D   0x000330   5D 04 00 00   AssertPointer        r4            r4 != nullptr
    004E   0x000338   0D 04 04 00   LoadU32              r4, [r4]      r4 = *(u32*)RET_darray-at
    004F   0x000340   2D 79 00 00   Branch               0x79          GOTO => L_3
  L_5:
    0050   0x000348   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    0051   0x000350   2F 58 04 00   BranchIfNot          r4, 0x5D      IF NOT r4 => L_6
    0052   0x000358   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    0053   0x000360   5D 04 00 00   AssertPointer        r4            r4 != nullptr
    0054   0x000368   3B 04 04 0C   IAddImm              r4, r4, 12    r4 = RET_darray-at + 12 -> <RET_(null)>
    0055   0x000370   4E 04 04 00   LoadU16              r4, [r4]      r4 = *(u16*)RET_darray-at
    0056   0x000378   0C 05 04 00   LoadU16Imm           r5, 4         r5 = 4
    0057   0x000380   1D 04 04 05   IEqual               r4, r4, r5    r4 = r4 == r5
    0058   0x000388   2F 5D 04 00   BranchIfNot          r4, 0x5D      IF NOT r4 => L_6
    0059   0x000390   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    005A   0x000398   5D 04 00 00   AssertPointer        r4            r4 != nullptr
    005B   0x0003A0   50 04 04 00   LoadI64              r4, [r4]      r4 = *(i64*)RET_darray-at
    005C   0x0003A8   2D 79 00 00   Branch               0x79          GOTO => L_3
  L_6:
    005D   0x0003B0   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    005E   0x0003B8   2F 65 04 00   BranchIfNot          r4, 0x6A      IF NOT r4 => L_7
    005F   0x0003C0   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    0060   0x0003C8   5D 04 00 00   AssertPointer        r4            r4 != nullptr
    0061   0x0003D0   3B 04 04 0C   IAddImm              r4, r4, 12    r4 = RET_darray-at + 12 -> <RET_(null)>
    0062   0x0003D8   4E 04 04 00   LoadU16              r4, [r4]      r4 = *(u16*)RET_darray-at
    0063   0x0003E0   0C 05 05 00   LoadU16Imm           r5, 5         r5 = 5
    0064   0x0003E8   1D 04 04 05   IEqual               r4, r4, r5    r4 = r4 == r5
    0065   0x0003F0   2F 6A 04 00   BranchIfNot          r4, 0x6A      IF NOT r4 => L_7
    0066   0x0003F8   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    0067   0x000400   5D 04 00 00   AssertPointer        r4            r4 != nullptr
    0068   0x000408   51 04 04 00   LoadU64              r4, [r4]      r4 = *(u64*)RET_darray-at
    0069   0x000410   2D 79 00 00   Branch               0x79          GOTO => L_3
  L_7:
    006A   0x000418   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    006B   0x000420   2F 72 04 00   BranchIfNot          r4, 0x78      IF NOT r4 => L_8
    006C   0x000428   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    006D   0x000430   5D 04 00 00   AssertPointer        r4            r4 != nullptr
    006E   0x000438   3B 04 04 0C   IAddImm              r4, r4, 12    r4 = RET_darray-at + 12 -> <RET_(null)>
    006F   0x000440   4E 04 04 00   LoadU16              r4, [r4]      r4 = *(u16*)RET_darray-at
    0070   0x000448   0C 05 06 00   LoadU16Imm           r5, 6         r5 = 6
    0071   0x000450   1D 04 04 05   IEqual               r4, r4, r5    r4 = r4 == r5
    0072   0x000458   2F 78 04 00   BranchIfNot          r4, 0x78      IF NOT r4 => L_8
    0073   0x000460   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    0074   0x000468   5D 04 00 00   AssertPointer        r4            r4 != nullptr
    0075   0x000470   0E 04 04 00   LoadFloat            r4, [r4]      r4 = *(f32*)RET_darray-at
    0076   0x000478   19 04 04 00   CastInteger          r4, r4        r4 = int(r4) -> <?> => <?>
    0077   0x000480   2D 79 00 00   Branch               0x79          GOTO => L_3
  L_8:
    0078   0x000488   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
  L_3:
    0079   0x000490   43 03 04 00   Move                 r3, r4        r3 = 0
    007A   0x000498   15 04 06 00   LookupPointer        r4, 6         r4 = ST[6] -> <kill-text-to-speech-frame>
    007B   0x0004A0   43 05 03 00   Move                 r5, r3        r5 = 0
    007C   0x0004A8   43 31 05 00   Move                 r49, r5       r49 = 0
    007D   0x0004B0   1C 04 04 01   CallFf               r4, r4, 1     r4 = kill-text-to-speech-frame(0)
    007E   0x0004B8   43 04 01 00   Move                 r4, r1        r4 = 0
    007F   0x0004C0   0C 05 01 00   LoadU16Imm           r5, 1         r5 = 1
    0080   0x0004C8   01 04 04 05   IAdd                 r4, r4, r5    r4 = 0 + 1
    0081   0x0004D0   43 01 04 00   Move                 r1, r4        r1 = 0
    0082   0x0004D8   43 04 01 00   Move                 r4, r1        r4 = 0
    0083   0x0004E0   43 03 04 00   Move                 r3, r4        r3 = 0
    0084   0x0004E8   2D 1A 00 00   Branch               0x1A          GOTO => L_9
  L_1:
    0085   0x0004F0   15 03 07 00   LookupPointer        r3, 7         r3 = ST[7] -> <end-foreach>
    0086   0x0004F8   1C 03 03 00   CallFf               r3, r3, 0     r3 = end-foreach()
    0087   0x000500   43 01 03 00   Move                 r1, r3        r1 = RET_end-foreach
    0088   0x000508   15 01 08 00   LookupPointer        r1, 8         r1 = ST[8] -> <darray-clear>
    0089   0x000510   15 02 01 00   LookupPointer        r2, 1         r2 = ST[1] -> <get-symbol>
    008A   0x000518   4A 03 02 00   LoadStaticU64Imm     r3, 2         r3 = ST[2] -> <#D2F26D6B19D49A54>
    008B   0x000520   4A 04 03 00   LoadStaticU64Imm     r4, 3         r4 = ST[3] -> <self>
    008C   0x000528   43 31 03 00   Move                 r49, r3       r49 = #D2F26D6B19D49A54
    008D   0x000530   43 32 04 00   Move                 r50, r4       r50 = self
    008E   0x000538   1C 02 02 02   CallFf               r2, r2, 2     r2 = get-symbol(#D2F26D6B19D49A54, self)
    008F   0x000540   43 31 02 00   Move                 r49, r2       r49 = RET_get-symbol
    0090   0x000548   1C 01 01 01   CallFf               r1, r1, 1     r1 = darray-clear(RET_get-symbol)
    0091   0x000550   2D 93 00 00   Branch               0x93          GOTO => L_10
  L_0:
    0092   0x000558   0C 01 00 00   LoadU16Imm           r1, 0         r1 = 0
  L_10:
    0093   0x000560   15 01 09 00   LookupPointer        r1, 9         r1 = ST[9] -> <speak-text-to-speech-raw>
    0094   0x000568   15 02 0A 00   LookupPointer        r2, 10        r2 = ST[10] -> <concat-boxed-arrays>
    0095   0x000570   15 03 0B 00   LookupPointer        r3, 11        r3 = ST[11] -> <generate-text-to-speech-formatted>
    0096   0x000578   15 04 0C 00   LookupPointer        r4, 12        r4 = ST[12] -> <new-boxed-value>
    0097   0x000580   0C 05 07 00   LoadU16Imm           r5, 7         r5 = 7
    0098   0x000588   15 06 01 00   LookupPointer        r6, 1         r6 = ST[1] -> <get-symbol>
    0099   0x000590   4A 07 0D 00   LoadStaticU64Imm     r7, 13        r7 = ST[13] -> <name>
    009A   0x000598   4A 08 03 00   LoadStaticU64Imm     r8, 3         r8 = ST[3] -> <self>
    009B   0x0005A0   43 31 07 00   Move                 r49, r7       r49 = name
    009C   0x0005A8   43 32 08 00   Move                 r50, r8       r50 = self
    009D   0x0005B0   1C 06 06 02   CallFf               r6, r6, 2     r6 = get-symbol(name, self)
    009E   0x0005B8   43 31 05 00   Move                 r49, r5       r49 = 7
    009F   0x0005C0   43 32 06 00   Move                 r50, r6       r50 = RET_get-symbol
    00A0   0x0005C8   1C 04 04 02   CallFf               r4, r4, 2     r4 = new-boxed-value(7, RET_get-symbol)
    00A1   0x0005D0   0C 05 00 00   LoadU16Imm           r5, 0         r5 = 0
    00A2   0x0005D8   43 31 04 00   Move                 r49, r4       r49 = RET_new-boxed-value
    00A3   0x0005E0   43 32 05 00   Move                 r50, r5       r50 = 0
    00A4   0x0005E8   1C 03 03 02   CallFf               r3, r3, 2     r3 = generate-text-to-speech-formatted(RET_new-boxed-value, 0)
    00A5   0x0005F0   15 04 0B 00   LookupPointer        r4, 11        r4 = ST[11] -> <generate-text-to-speech-formatted>
    00A6   0x0005F8   15 05 0E 00   LookupPointer        r5, 14        r5 = ST[14] -> <cipher-box-formatted-number-string>
    00A7   0x000600   4A 06 0F 00   LoadStaticU64Imm     r6, 15        r6 = ST[15] -> <BOSS_REMAINING_HEALTH_PERCENT>
    00A8   0x000608   43 07 00 00   Move                 r7, r0        r7 = arg_0
    00A9   0x000610   40 08 10 00   LoadStaticFloatImm   r8, 16        r8 = ST[16] -> <100.000000>
    00AA   0x000618   07 07 07 08   FMul                 r7, r7, r8    arg_0 = arg_0 * 100.000000
    00AB   0x000620   19 07 07 00   CastInteger          r7, r7        r7 = int(r7) -> <arg_0> => <arg_0>
    00AC   0x000628   0C 08 00 00   LoadU16Imm           r8, 0         r8 = 0
    00AD   0x000630   0C 09 00 00   LoadU16Imm           r9, 0         r9 = 0
    00AE   0x000638   0C 0A 00 00   LoadU16Imm           r10, 0        r10 = 0
    00AF   0x000640   43 31 06 00   Move                 r49, r6       r49 = BOSS_REMAINING_HEALTH_PERCENT
    00B0   0x000648   43 32 07 00   Move                 r50, r7       r50 = arg_0
    00B1   0x000650   43 33 08 00   Move                 r51, r8       r51 = 0
    00B2   0x000658   43 34 09 00   Move                 r52, r9       r52 = 0
    00B3   0x000660   43 35 0A 00   Move                 r53, r10      r53 = 0
    00B4   0x000668   1C 05 05 05   CallFf               r5, r5, 5     r5 = cipher-box-formatted-number-string(BOSS_REMAINING_HEALTH_PERCENT, arg_0, 0, 0, 0)
    00B5   0x000670   0C 06 00 00   LoadU16Imm           r6, 0         r6 = 0
    00B6   0x000678   43 31 05 00   Move                 r49, r5       r49 = RET_cipher-box-formatted-number-string
    00B7   0x000680   43 32 06 00   Move                 r50, r6       r50 = 0
    00B8   0x000688   1C 04 04 02   CallFf               r4, r4, 2     r4 = generate-text-to-speech-formatted(RET_cipher-box-formatted-number-string, 0)
    00B9   0x000690   0C 05 00 00   LoadU16Imm           r5, 0         r5 = 0
    00BA   0x000698   0C 06 00 00   LoadU16Imm           r6, 0         r6 = 0
    00BB   0x0006A0   0C 07 00 00   LoadU16Imm           r7, 0         r7 = 0
    00BC   0x0006A8   0C 08 00 00   LoadU16Imm           r8, 0         r8 = 0
    00BD   0x0006B0   0C 09 00 00   LoadU16Imm           r9, 0         r9 = 0
    00BE   0x0006B8   0C 0A 00 00   LoadU16Imm           r10, 0        r10 = 0
    00BF   0x0006C0   43 31 03 00   Move                 r49, r3       r49 = RET_generate-text-to-speech-formatted
    00C0   0x0006C8   43 32 04 00   Move                 r50, r4       r50 = RET_generate-text-to-speech-formatted
    00C1   0x0006D0   43 33 05 00   Move                 r51, r5       r51 = 0
    00C2   0x0006D8   43 34 06 00   Move                 r52, r6       r52 = 0
    00C3   0x0006E0   43 35 07 00   Move                 r53, r7       r53 = 0
    00C4   0x0006E8   43 36 08 00   Move                 r54, r8       r54 = 0
    00C5   0x0006F0   43 37 09 00   Move                 r55, r9       r55 = 0
    00C6   0x0006F8   43 38 0A 00   Move                 r56, r10      r56 = 0
    00C7   0x000700   1C 02 02 08   CallFf               r2, r2, 8     r2 = concat-boxed-arrays(RET_generate-text-to-speech-formatted, RET_generate-text-to-speech-formatted, 0, 0, 0, 0, 0, 0)
    00C8   0x000708   0C 03 00 00   LoadU16Imm           r3, 0         r3 = 0
    00C9   0x000710   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
    00CA   0x000718   0C 05 01 00   LoadU16Imm           r5, 1         r5 = 1
    00CB   0x000720   43 31 02 00   Move                 r49, r2       r49 = RET_concat-boxed-arrays
    00CC   0x000728   43 32 03 00   Move                 r50, r3       r50 = 0
    00CD   0x000730   43 33 04 00   Move                 r51, r4       r51 = 0
    00CE   0x000738   43 34 05 00   Move                 r52, r5       r52 = 1
    00CF   0x000740   1C 01 01 04   CallFf               r1, r1, 4     r1 = speak-text-to-speech-raw(RET_concat-boxed-arrays, 0, 0, 1)
    00D0   0x000748   00 01 01 00   Return               r1            Return 

    SYMBOL TABLE: 
    0000   0x000750    function: darray-count
    0001   0x000758    function: get-symbol
    0002   0x000760    sid: #D2F26D6B19D49A54
    0003   0x000768    sid: self
    0004   0x000770    function: begin-foreach
    0005   0x000778    function: darray-at
    0006   0x000780    function: kill-text-to-speech-frame
    0007   0x000788    function: end-foreach
    0008   0x000790    function: darray-clear
    0009   0x000798    function: speak-text-to-speech-raw
    000A   0x0007A0    function: concat-boxed-arrays
    000B   0x0007A8    function: generate-text-to-speech-formatted
    000C   0x0007B0    function: new-boxed-value
    000D   0x0007B8    sid: name
    000E   0x0007C0    function: cipher-box-formatted-number-string
    000F   0x0007C8    sid: BOSS_REMAINING_HEALTH_PERCENT
    0010   0x0007D0    float: 100.000000
}


##############################  ENTRY 1  ##############################

ss-accessbility-boss-health-tts = state-script [0x007E0] {
  OPTIONS:     player
  DECLARATIONS: 
    [0x000D98] symbol   npc                  = uninitialized
    [0x000DC8] symbol   name                 = uninitialized
    [0x000DF8] float    #ACCC87ED9AE6837C    = 0.00
    [0x000E28] boolean  #60C441473AC3126E    = true
    [0x000E58] boolean  #934EE4E849CBD538    = uninitialized
    [0x000E88] boolean  #29137C2A7D7263BB    = uninitialized
    [0x000EB8] boolean  #11C859308AA9F60B    = uninitialized
    [0x000EE8] boolean  #0D288B4554B68C50    = true
    [0x000F18] boolean  #3193E21B9FE4FC53    = uninitialized
    [0x000F48] boolean  #87B5D4A36BD8ECC3    = uninitialized
    [0x000F78] boolean  #DBA22D94C61A6F07    = true
    [0x000FA8] boolean  #52AB8DE16851EBC8    = uninitialized
    [0x000FD8] boolean  #98A1648B00B03E48    = uninitialized
    [0x001008] timer    #8FFD0BAEEF4F2DB1    = uninitialized
    [0x001038] float    #C9D6A8C470495D51    = 2.00
    [0x001068] symbol   #D2F26D6B19D49A54    = uninitialized
    STATE --script-- {
      ON start {
        TRACK main {
          {
            0000   0x003570   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <set-symbol>
            0001   0x003578   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <#D2F26D6B19D49A54>
            0002   0x003580   15 02 02 00   LookupPointer        r2, 2         r2 = ST[2] -> <darray-create>
            0003   0x003588   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <self>
            0004   0x003590   3F 04 04 00   LoadStaticI32Imm     r4, 4         r4 = ST[4] -> <-1>
            0005   0x003598   0C 05 00 00   LoadU16Imm           r5, 0         r5 = 0
            0006   0x0035A0   43 31 03 00   Move                 r49, r3       r49 = self
            0007   0x0035A8   43 32 04 00   Move                 r50, r4       r50 = -1
            0008   0x0035B0   43 33 05 00   Move                 r51, r5       r51 = 0
            0009   0x0035B8   1C 02 02 03   CallFf               r2, r2, 3     r2 = darray-create(self, -1, 0)
            000A   0x0035C0   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <self>
            000B   0x0035C8   43 31 01 00   Move                 r49, r1       r49 = #D2F26D6B19D49A54
            000C   0x0035D0   43 32 02 00   Move                 r50, r2       r50 = RET_darray-create
            000D   0x0035D8   43 33 03 00   Move                 r51, r3       r51 = self
            000E   0x0035E0   1C 00 00 03   CallFf               r0, r0, 3     r0 = set-symbol(#D2F26D6B19D49A54, RET_darray-create, self)
            000F   0x0035E8   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x0035F0    function: set-symbol
            0001   0x0035F8    sid: #D2F26D6B19D49A54
            0002   0x003600    function: darray-create
            0003   0x003608    sid: self
            0004   0x003610    int: -1
          }
        }

      }
      ON event_#9CACFB930AE2D219 {
        TRACK main {
          {
            0000   0x003470   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <#9B6270342402C4F8>
            0001   0x003478   1B 00 00 00   Call                 r0, r0, 0     r0 = #9B6270342402C4F8()
            0002   0x003480   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003488    function: #9B6270342402C4F8
          }
        }

      }
    } END STATE --script--

    STATE tts-off {
      ON update {
        TRACK main {
          {
            0000   0x003618   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <is-text-to-speech-enabled?>
            0001   0x003620   1C 00 00 00   CallFf               r0, r0, 0     r0 = is-text-to-speech-enabled?()
            0002   0x003628   2F 0A 00 00   BranchIfNot          r0, 0xA       IF NOT r0 => L_RETURN_0
            0003   0x003630   15 00 01 00   LookupPointer        r0, 1         r0 = ST[1] -> <go>
            0004   0x003638   4A 01 02 00   LoadStaticU64Imm     r1, 2         r1 = ST[2] -> <tracking>
            0005   0x003640   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0006   0x003648   43 31 01 00   Move                 r49, r1       r49 = tracking
            0007   0x003650   43 32 02 00   Move                 r50, r2       r50 = 1
            0008   0x003658   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(tracking, 1)
            0009   0x003660   2D 0B 00 00   Branch               0xB           GOTO => L_RETURN
          L_RETURN_0:
            000A   0x003668   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
          L_RETURN:
            000B   0x003670   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003678    function: is-text-to-speech-enabled?
            0001   0x003680    function: go
            0002   0x003688    sid: tracking
          }
        }

      }
    } END STATE tts-off

    STATE tracking {
      ON start {
        TRACK main {
          {
            0000   0x003490   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <set-symbol>
            0001   0x003498   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <npc>
            0002   0x0034A0   15 02 02 00   LookupPointer        r2, 2         r2 = ST[2] -> <get-self-id>
            0003   0x0034A8   1C 02 02 00   CallFf               r2, r2, 0     r2 = get-self-id()
            0004   0x0034B0   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <self>
            0005   0x0034B8   43 31 01 00   Move                 r49, r1       r49 = npc
            0006   0x0034C0   43 32 02 00   Move                 r50, r2       r50 = RET_get-self-id
            0007   0x0034C8   43 33 03 00   Move                 r51, r3       r51 = self
            0008   0x0034D0   1C 00 00 03   CallFf               r0, r0, 3     r0 = set-symbol(npc, RET_get-self-id, self)
            0009   0x0034D8   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x0034E0    function: set-symbol
            0001   0x0034E8    sid: npc
            0002   0x0034F0    function: get-self-id
            0003   0x0034F8    sid: self
          }
        }

      }
      ON event_npc-died {
        TRACK main {
          {
            0000   0x002F90   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <darray-count>
            0001   0x002F98   15 01 01 00   LookupPointer        r1, 1         r1 = ST[1] -> <get-symbol>
            0002   0x002FA0   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <#D2F26D6B19D49A54>
            0003   0x002FA8   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <self>
            0004   0x002FB0   43 31 02 00   Move                 r49, r2       r49 = #D2F26D6B19D49A54
            0005   0x002FB8   43 32 03 00   Move                 r50, r3       r50 = self
            0006   0x002FC0   1C 01 01 02   CallFf               r1, r1, 2     r1 = get-symbol(#D2F26D6B19D49A54, self)
            0007   0x002FC8   43 31 01 00   Move                 r49, r1       r49 = RET_get-symbol
            0008   0x002FD0   1C 00 00 01   CallFf               r0, r0, 1     r0 = darray-count(RET_get-symbol)
            0009   0x002FD8   0C 01 00 00   LoadU16Imm           r1, 0         r1 = 0
            000A   0x002FE0   1D 00 00 01   IEqual               r0, r0, r1    r0 = r0 == r1
            000B   0x002FE8   30 00 00 00   OpLogNot             r0, r0        r0 = !RET_darray-count
            000C   0x002FF0   2F 91 00 00   BranchIfNot          r0, 0x91      IF NOT r0 => L_RETURN_0
            000D   0x002FF8   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
            000E   0x003000   15 01 00 00   LookupPointer        r1, 0         r1 = ST[0] -> <darray-count>
            000F   0x003008   15 02 01 00   LookupPointer        r2, 1         r2 = ST[1] -> <get-symbol>
            0010   0x003010   4A 03 02 00   LoadStaticU64Imm     r3, 2         r3 = ST[2] -> <#D2F26D6B19D49A54>
            0011   0x003018   4A 04 03 00   LoadStaticU64Imm     r4, 3         r4 = ST[3] -> <self>
            0012   0x003020   43 31 03 00   Move                 r49, r3       r49 = #D2F26D6B19D49A54
            0013   0x003028   43 32 04 00   Move                 r50, r4       r50 = self
            0014   0x003030   1C 02 02 02   CallFf               r2, r2, 2     r2 = get-symbol(#D2F26D6B19D49A54, self)
            0015   0x003038   43 31 02 00   Move                 r49, r2       r49 = RET_get-symbol
            0016   0x003040   1C 01 01 01   CallFf               r1, r1, 1     r1 = darray-count(RET_get-symbol)
            0017   0x003048   15 02 04 00   LookupPointer        r2, 4         r2 = ST[4] -> <begin-foreach>
            0018   0x003050   1C 02 02 00   CallFf               r2, r2, 0     r2 = begin-foreach()
          L_9:
            0019   0x003058   43 02 00 00   Move                 r2, r0        r2 = 0
            001A   0x003060   43 03 01 00   Move                 r3, r1        r3 = RET_darray-count
            001B   0x003068   20 02 02 03   ILessThan            r2, r2, r3    r2 = r2 < r3
            001C   0x003070   2F 84 02 00   BranchIfNot          r2, 0x84      IF NOT r2 => L_1
            001D   0x003078   15 02 05 00   LookupPointer        r2, 5         r2 = ST[5] -> <darray-at>
            001E   0x003080   15 03 01 00   LookupPointer        r3, 1         r3 = ST[1] -> <get-symbol>
            001F   0x003088   4A 04 02 00   LoadStaticU64Imm     r4, 2         r4 = ST[2] -> <#D2F26D6B19D49A54>
            0020   0x003090   4A 05 03 00   LoadStaticU64Imm     r5, 3         r5 = ST[3] -> <self>
            0021   0x003098   43 31 04 00   Move                 r49, r4       r49 = #D2F26D6B19D49A54
            0022   0x0030A0   43 32 05 00   Move                 r50, r5       r50 = self
            0023   0x0030A8   1C 03 03 02   CallFf               r3, r3, 2     r3 = get-symbol(#D2F26D6B19D49A54, self)
            0024   0x0030B0   43 04 00 00   Move                 r4, r0        r4 = 0
            0025   0x0030B8   43 31 03 00   Move                 r49, r3       r49 = RET_get-symbol
            0026   0x0030C0   43 32 04 00   Move                 r50, r4       r50 = 0
            0027   0x0030C8   1C 02 02 02   CallFf               r2, r2, 2     r2 = darray-at(RET_get-symbol, 0)
            0028   0x0030D0   43 03 02 00   Move                 r3, r2        r3 = RET_darray-at
            0029   0x0030D8   2F 30 03 00   BranchIfNot          r3, 0x35      IF NOT r3 => L_2
            002A   0x0030E0   43 03 02 00   Move                 r3, r2        r3 = RET_darray-at
            002B   0x0030E8   5D 03 00 00   AssertPointer        r3            r3 != nullptr
            002C   0x0030F0   3B 03 03 0C   IAddImm              r3, r3, 12    r3 = RET_darray-at + 12 -> <RET_(null)>
            002D   0x0030F8   4E 03 03 00   LoadU16              r3, [r3]      r3 = *(u16*)RET_darray-at
            002E   0x003100   0C 04 01 00   LoadU16Imm           r4, 1         r4 = 1
            002F   0x003108   1D 03 03 04   IEqual               r3, r3, r4    r3 = r3 == r4
            0030   0x003110   2F 35 03 00   BranchIfNot          r3, 0x35      IF NOT r3 => L_2
            0031   0x003118   43 03 02 00   Move                 r3, r2        r3 = RET_darray-at
            0032   0x003120   5D 03 00 00   AssertPointer        r3            r3 != nullptr
            0033   0x003128   4C 03 03 00   LoadU8               r3, [r3]      r3 = *(u8*)RET_darray-at
            0034   0x003130   2D 78 00 00   Branch               0x78          GOTO => L_3
          L_2:
            0035   0x003138   43 03 02 00   Move                 r3, r2        r3 = RET_darray-at
            0036   0x003140   2F 3D 03 00   BranchIfNot          r3, 0x42      IF NOT r3 => L_4
            0037   0x003148   43 03 02 00   Move                 r3, r2        r3 = RET_darray-at
            0038   0x003150   5D 03 00 00   AssertPointer        r3            r3 != nullptr
            0039   0x003158   3B 03 03 0C   IAddImm              r3, r3, 12    r3 = RET_darray-at + 12 -> <RET_(null)>
            003A   0x003160   4E 03 03 00   LoadU16              r3, [r3]      r3 = *(u16*)RET_darray-at
            003B   0x003168   0C 04 02 00   LoadU16Imm           r4, 2         r4 = 2
            003C   0x003170   1D 03 03 04   IEqual               r3, r3, r4    r3 = r3 == r4
            003D   0x003178   2F 42 03 00   BranchIfNot          r3, 0x42      IF NOT r3 => L_4
            003E   0x003180   43 03 02 00   Move                 r3, r2        r3 = RET_darray-at
            003F   0x003188   5D 03 00 00   AssertPointer        r3            r3 != nullptr
            0040   0x003190   4F 03 03 00   LoadI32              r3, [r3]      r3 = *(i32*)RET_darray-at
            0041   0x003198   2D 78 00 00   Branch               0x78          GOTO => L_3
          L_4:
            0042   0x0031A0   43 03 02 00   Move                 r3, r2        r3 = RET_darray-at
            0043   0x0031A8   2F 4A 03 00   BranchIfNot          r3, 0x4F      IF NOT r3 => L_5
            0044   0x0031B0   43 03 02 00   Move                 r3, r2        r3 = RET_darray-at
            0045   0x0031B8   5D 03 00 00   AssertPointer        r3            r3 != nullptr
            0046   0x0031C0   3B 03 03 0C   IAddImm              r3, r3, 12    r3 = RET_darray-at + 12 -> <RET_(null)>
            0047   0x0031C8   4E 03 03 00   LoadU16              r3, [r3]      r3 = *(u16*)RET_darray-at
            0048   0x0031D0   0C 04 03 00   LoadU16Imm           r4, 3         r4 = 3
            0049   0x0031D8   1D 03 03 04   IEqual               r3, r3, r4    r3 = r3 == r4
            004A   0x0031E0   2F 4F 03 00   BranchIfNot          r3, 0x4F      IF NOT r3 => L_5
            004B   0x0031E8   43 03 02 00   Move                 r3, r2        r3 = RET_darray-at
            004C   0x0031F0   5D 03 00 00   AssertPointer        r3            r3 != nullptr
            004D   0x0031F8   0D 03 03 00   LoadU32              r3, [r3]      r3 = *(u32*)RET_darray-at
            004E   0x003200   2D 78 00 00   Branch               0x78          GOTO => L_3
          L_5:
            004F   0x003208   43 03 02 00   Move                 r3, r2        r3 = RET_darray-at
            0050   0x003210   2F 57 03 00   BranchIfNot          r3, 0x5C      IF NOT r3 => L_6
            0051   0x003218   43 03 02 00   Move                 r3, r2        r3 = RET_darray-at
            0052   0x003220   5D 03 00 00   AssertPointer        r3            r3 != nullptr
            0053   0x003228   3B 03 03 0C   IAddImm              r3, r3, 12    r3 = RET_darray-at + 12 -> <RET_(null)>
            0054   0x003230   4E 03 03 00   LoadU16              r3, [r3]      r3 = *(u16*)RET_darray-at
            0055   0x003238   0C 04 04 00   LoadU16Imm           r4, 4         r4 = 4
            0056   0x003240   1D 03 03 04   IEqual               r3, r3, r4    r3 = r3 == r4
            0057   0x003248   2F 5C 03 00   BranchIfNot          r3, 0x5C      IF NOT r3 => L_6
            0058   0x003250   43 03 02 00   Move                 r3, r2        r3 = RET_darray-at
            0059   0x003258   5D 03 00 00   AssertPointer        r3            r3 != nullptr
            005A   0x003260   50 03 03 00   LoadI64              r3, [r3]      r3 = *(i64*)RET_darray-at
            005B   0x003268   2D 78 00 00   Branch               0x78          GOTO => L_3
          L_6:
            005C   0x003270   43 03 02 00   Move                 r3, r2        r3 = RET_darray-at
            005D   0x003278   2F 64 03 00   BranchIfNot          r3, 0x69      IF NOT r3 => L_7
            005E   0x003280   43 03 02 00   Move                 r3, r2        r3 = RET_darray-at
            005F   0x003288   5D 03 00 00   AssertPointer        r3            r3 != nullptr
            0060   0x003290   3B 03 03 0C   IAddImm              r3, r3, 12    r3 = RET_darray-at + 12 -> <RET_(null)>
            0061   0x003298   4E 03 03 00   LoadU16              r3, [r3]      r3 = *(u16*)RET_darray-at
            0062   0x0032A0   0C 04 05 00   LoadU16Imm           r4, 5         r4 = 5
            0063   0x0032A8   1D 03 03 04   IEqual               r3, r3, r4    r3 = r3 == r4
            0064   0x0032B0   2F 69 03 00   BranchIfNot          r3, 0x69      IF NOT r3 => L_7
            0065   0x0032B8   43 03 02 00   Move                 r3, r2        r3 = RET_darray-at
            0066   0x0032C0   5D 03 00 00   AssertPointer        r3            r3 != nullptr
            0067   0x0032C8   51 03 03 00   LoadU64              r3, [r3]      r3 = *(u64*)RET_darray-at
            0068   0x0032D0   2D 78 00 00   Branch               0x78          GOTO => L_3
          L_7:
            0069   0x0032D8   43 03 02 00   Move                 r3, r2        r3 = RET_darray-at
            006A   0x0032E0   2F 71 03 00   BranchIfNot          r3, 0x77      IF NOT r3 => L_8
            006B   0x0032E8   43 03 02 00   Move                 r3, r2        r3 = RET_darray-at
            006C   0x0032F0   5D 03 00 00   AssertPointer        r3            r3 != nullptr
            006D   0x0032F8   3B 03 03 0C   IAddImm              r3, r3, 12    r3 = RET_darray-at + 12 -> <RET_(null)>
            006E   0x003300   4E 03 03 00   LoadU16              r3, [r3]      r3 = *(u16*)RET_darray-at
            006F   0x003308   0C 04 06 00   LoadU16Imm           r4, 6         r4 = 6
            0070   0x003310   1D 03 03 04   IEqual               r3, r3, r4    r3 = r3 == r4
            0071   0x003318   2F 77 03 00   BranchIfNot          r3, 0x77      IF NOT r3 => L_8
            0072   0x003320   43 03 02 00   Move                 r3, r2        r3 = RET_darray-at
            0073   0x003328   5D 03 00 00   AssertPointer        r3            r3 != nullptr
            0074   0x003330   0E 03 03 00   LoadFloat            r3, [r3]      r3 = *(f32*)RET_darray-at
            0075   0x003338   19 03 03 00   CastInteger          r3, r3        r3 = int(r3) -> <?> => <?>
            0076   0x003340   2D 78 00 00   Branch               0x78          GOTO => L_3
          L_8:
            0077   0x003348   0C 03 00 00   LoadU16Imm           r3, 0         r3 = 0
          L_3:
            0078   0x003350   43 02 03 00   Move                 r2, r3        r2 = 0
            0079   0x003358   15 03 06 00   LookupPointer        r3, 6         r3 = ST[6] -> <kill-text-to-speech-frame>
            007A   0x003360   43 04 02 00   Move                 r4, r2        r4 = 0
            007B   0x003368   43 31 04 00   Move                 r49, r4       r49 = 0
            007C   0x003370   1C 03 03 01   CallFf               r3, r3, 1     r3 = kill-text-to-speech-frame(0)
            007D   0x003378   43 03 00 00   Move                 r3, r0        r3 = 0
            007E   0x003380   0C 04 01 00   LoadU16Imm           r4, 1         r4 = 1
            007F   0x003388   01 03 03 04   IAdd                 r3, r3, r4    r3 = 0 + 1
            0080   0x003390   43 00 03 00   Move                 r0, r3        r0 = 0
            0081   0x003398   43 03 00 00   Move                 r3, r0        r3 = 0
            0082   0x0033A0   43 02 03 00   Move                 r2, r3        r2 = 0
            0083   0x0033A8   2D 19 00 00   Branch               0x19          GOTO => L_9
          L_1:
            0084   0x0033B0   15 02 07 00   LookupPointer        r2, 7         r2 = ST[7] -> <end-foreach>
            0085   0x0033B8   1C 02 02 00   CallFf               r2, r2, 0     r2 = end-foreach()
            0086   0x0033C0   43 00 02 00   Move                 r0, r2        r0 = RET_end-foreach
            0087   0x0033C8   15 00 08 00   LookupPointer        r0, 8         r0 = ST[8] -> <darray-clear>
            0088   0x0033D0   15 01 01 00   LookupPointer        r1, 1         r1 = ST[1] -> <get-symbol>
            0089   0x0033D8   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <#D2F26D6B19D49A54>
            008A   0x0033E0   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <self>
            008B   0x0033E8   43 31 02 00   Move                 r49, r2       r49 = #D2F26D6B19D49A54
            008C   0x0033F0   43 32 03 00   Move                 r50, r3       r50 = self
            008D   0x0033F8   1C 01 01 02   CallFf               r1, r1, 2     r1 = get-symbol(#D2F26D6B19D49A54, self)
            008E   0x003400   43 31 01 00   Move                 r49, r1       r49 = RET_get-symbol
            008F   0x003408   1C 00 00 01   CallFf               r0, r0, 1     r0 = darray-clear(RET_get-symbol)
            0090   0x003410   2D 92 00 00   Branch               0x92          GOTO => L_RETURN
          L_RETURN_0:
            0091   0x003418   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
          L_RETURN:
            0092   0x003420   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003428    function: darray-count
            0001   0x003430    function: get-symbol
            0002   0x003438    sid: #D2F26D6B19D49A54
            0003   0x003440    sid: self
            0004   0x003448    function: begin-foreach
            0005   0x003450    function: darray-at
            0006   0x003458    function: kill-text-to-speech-frame
            0007   0x003460    function: end-foreach
            0008   0x003468    function: darray-clear
          }
          {
            0000   0x0029F8   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <go>
            0001   0x002A00   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <done>
            0002   0x002A08   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0003   0x002A10   43 31 01 00   Move                 r49, r1       r49 = done
            0004   0x002A18   43 32 02 00   Move                 r50, r2       r50 = 1
            0005   0x002A20   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(done, 1)
            0006   0x002A28   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x002A30    function: go
            0001   0x002A38    sid: done
          }
        }

      }
      ON update {
        TRACK main {
          {
            0000   0x003690   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <is-text-to-speech-enabled?>
            0001   0x003698   1C 00 00 00   CallFf               r0, r0, 0     r0 = is-text-to-speech-enabled?()
            0002   0x0036A0   30 00 00 00   OpLogNot             r0, r0        r0 = !RET_is-text-to-speech-enabled?
            0003   0x0036A8   2F 0B 00 00   BranchIfNot          r0, 0xB       IF NOT r0 => L_RETURN_0
            0004   0x0036B0   15 00 01 00   LookupPointer        r0, 1         r0 = ST[1] -> <go>
            0005   0x0036B8   4A 01 02 00   LoadStaticU64Imm     r1, 2         r1 = ST[2] -> <tts-off>
            0006   0x0036C0   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0007   0x0036C8   43 31 01 00   Move                 r49, r1       r49 = tts-off
            0008   0x0036D0   43 32 02 00   Move                 r50, r2       r50 = 1
            0009   0x0036D8   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(tts-off, 1)
            000A   0x0036E0   2D 0C 00 00   Branch               0xC           GOTO => L_RETURN
          L_RETURN_0:
            000B   0x0036E8   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
          L_RETURN:
            000C   0x0036F0   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x0036F8    function: is-text-to-speech-enabled?
            0001   0x003700    function: go
            0002   0x003708    sid: tts-off
          }
          {
            0000   0x002A40   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <set-float>
            0001   0x002A48   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <#ACCC87ED9AE6837C>
            0002   0x002A50   15 02 02 00   LookupPointer        r2, 2         r2 = ST[2] -> <get-object-health-percentage>
            0003   0x002A58   15 03 03 00   LookupPointer        r3, 3         r3 = ST[3] -> <get-symbol>
            0004   0x002A60   4A 04 04 00   LoadStaticU64Imm     r4, 4         r4 = ST[4] -> <npc>
            0005   0x002A68   4A 05 05 00   LoadStaticU64Imm     r5, 5         r5 = ST[5] -> <self>
            0006   0x002A70   43 31 04 00   Move                 r49, r4       r49 = npc
            0007   0x002A78   43 32 05 00   Move                 r50, r5       r50 = self
            0008   0x002A80   1C 03 03 02   CallFf               r3, r3, 2     r3 = get-symbol(npc, self)
            0009   0x002A88   43 31 03 00   Move                 r49, r3       r49 = RET_get-symbol
            000A   0x002A90   1C 02 02 01   CallFf               r2, r2, 1     r2 = get-object-health-percentage(RET_get-symbol)
            000B   0x002A98   4A 03 05 00   LoadStaticU64Imm     r3, 5         r3 = ST[5] -> <self>
            000C   0x002AA0   43 31 01 00   Move                 r49, r1       r49 = #ACCC87ED9AE6837C
            000D   0x002AA8   43 32 02 00   Move                 r50, r2       r50 = RET_get-object-health-percentage
            000E   0x002AB0   43 33 03 00   Move                 r51, r3       r51 = self
            000F   0x002AB8   1C 00 00 03   CallFf               r0, r0, 3     r0 = set-float(#ACCC87ED9AE6837C, RET_get-object-health-percentage, self)
            0010   0x002AC0   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x002AC8    function: set-float
            0001   0x002AD0    sid: #ACCC87ED9AE6837C
            0002   0x002AD8    function: get-object-health-percentage
            0003   0x002AE0    function: get-symbol
            0004   0x002AE8    sid: npc
            0005   0x002AF0    sid: self
          }
          {
            0000   0x002060   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <get-black-fade>
            0001   0x002068   1C 00 00 00   CallFf               r0, r0, 0     r0 = get-black-fade()
            0002   0x002070   0C 01 00 00   LoadU16Imm           r1, 0         r1 = 0
            0003   0x002078   1A 01 01 00   CastFloat            r1, r1        r1 = float(r1) -> <0> => <0>
            0004   0x002080   5B 00 00 01   FNotEqual            r0, r0, r1    r0 = r0 != r1
            0005   0x002088   2E 0C 00 00   BranchIf             r0, 0x117     IF r0 => L_RETURN_0
            0006   0x002090   15 00 01 00   LookupPointer        r0, 1         r0 = ST[1] -> <timer-running?>
            0007   0x002098   4A 01 02 00   LoadStaticU64Imm     r1, 2         r1 = ST[2] -> <#8FFD0BAEEF4F2DB1>
            0008   0x0020A0   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <self>
            0009   0x0020A8   43 31 01 00   Move                 r49, r1       r49 = #8FFD0BAEEF4F2DB1
            000A   0x0020B0   43 32 02 00   Move                 r50, r2       r50 = self
            000B   0x0020B8   1B 00 00 02   Call                 r0, r0, 2     r0 = timer-running?(#8FFD0BAEEF4F2DB1, self)
            000C   0x0020C0   30 00 00 00   OpLogNot             r0, r0        r0 = !RET_timer-running?
            000D   0x0020C8   2F 17 00 01   BranchIfNot          r0, 0x117     IF NOT r0 => L_RETURN_0
            000E   0x0020D0   15 00 04 00   LookupPointer        r0, 4         r0 = ST[4] -> <get-boolean>
            000F   0x0020D8   4A 01 05 00   LoadStaticU64Imm     r1, 5         r1 = ST[5] -> <#60C441473AC3126E>
            0010   0x0020E0   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <self>
            0011   0x0020E8   43 31 01 00   Move                 r49, r1       r49 = #60C441473AC3126E
            0012   0x0020F0   43 32 02 00   Move                 r50, r2       r50 = self
            0013   0x0020F8   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-boolean(symbol: #60C441473AC3126E, location: self)
            0014   0x002100   2F 15 00 01   BranchIfNot          r0, 0x115     IF NOT r0 => L_1
            0015   0x002108   15 00 04 00   LookupPointer        r0, 4         r0 = ST[4] -> <get-boolean>
            0016   0x002110   4A 01 06 00   LoadStaticU64Imm     r1, 6         r1 = ST[6] -> <#98A1648B00B03E48>
            0017   0x002118   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <self>
            0018   0x002120   43 31 01 00   Move                 r49, r1       r49 = #98A1648B00B03E48
            0019   0x002128   43 32 02 00   Move                 r50, r2       r50 = self
            001A   0x002130   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-boolean(symbol: #98A1648B00B03E48, location: self)
            001B   0x002138   30 00 00 00   OpLogNot             r0, r0        r0 = !54185015286451590
            001C   0x002140   2F 25 00 00   BranchIfNot          r0, 0x2F      IF NOT r0 => L_2
            001D   0x002148   15 00 07 00   LookupPointer        r0, 7         r0 = ST[7] -> <get-float>
            001E   0x002150   4A 01 08 00   LoadStaticU64Imm     r1, 8         r1 = ST[8] -> <#ACCC87ED9AE6837C>
            001F   0x002158   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <self>
            0020   0x002160   43 31 01 00   Move                 r49, r1       r49 = #ACCC87ED9AE6837C
            0021   0x002168   43 32 02 00   Move                 r50, r2       r50 = self
            0022   0x002170   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-float(#ACCC87ED9AE6837C, self)
            0023   0x002178   40 01 09 00   LoadStaticFloatImm   r1, 9         r1 = ST[9] -> <0.250000>
            0024   0x002180   26 00 00 01   FLessThanEqual       r0, r0, r1    r0 = r0 <= r1
            0025   0x002188   2F 2F 00 00   BranchIfNot          r0, 0x2F      IF NOT r0 => L_2
            0026   0x002190   15 00 0A 00   LookupPointer        r0, 10        r0 = ST[10] -> <set-boolean>
            0027   0x002198   4A 01 0B 00   LoadStaticU64Imm     r1, 11        r1 = ST[11] -> <#52AB8DE16851EBC8>
            0028   0x0021A0   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0029   0x0021A8   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <self>
            002A   0x0021B0   43 31 01 00   Move                 r49, r1       r49 = #52AB8DE16851EBC8
            002B   0x0021B8   43 32 02 00   Move                 r50, r2       r50 = 1
            002C   0x0021C0   43 33 03 00   Move                 r51, r3       r51 = self
            002D   0x0021C8   1C 00 00 03   CallFf               r0, r0, 3     r0 = set-boolean(#52AB8DE16851EBC8, 1, self)
            002E   0x0021D0   2D 64 00 00   Branch               0x64          GOTO => L_3
          L_2:
            002F   0x0021D8   15 00 04 00   LookupPointer        r0, 4         r0 = ST[4] -> <get-boolean>
            0030   0x0021E0   4A 01 0C 00   LoadStaticU64Imm     r1, 12        r1 = ST[12] -> <#87B5D4A36BD8ECC3>
            0031   0x0021E8   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <self>
            0032   0x0021F0   43 31 01 00   Move                 r49, r1       r49 = #87B5D4A36BD8ECC3
            0033   0x0021F8   43 32 02 00   Move                 r50, r2       r50 = self
            0034   0x002200   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-boolean(symbol: #87B5D4A36BD8ECC3, location: self)
            0035   0x002208   30 00 00 00   OpLogNot             r0, r0        r0 = !54185015286451590
            0036   0x002210   2F 3F 00 00   BranchIfNot          r0, 0x49      IF NOT r0 => L_4
            0037   0x002218   15 00 07 00   LookupPointer        r0, 7         r0 = ST[7] -> <get-float>
            0038   0x002220   4A 01 08 00   LoadStaticU64Imm     r1, 8         r1 = ST[8] -> <#ACCC87ED9AE6837C>
            0039   0x002228   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <self>
            003A   0x002230   43 31 01 00   Move                 r49, r1       r49 = #ACCC87ED9AE6837C
            003B   0x002238   43 32 02 00   Move                 r50, r2       r50 = self
            003C   0x002240   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-float(#ACCC87ED9AE6837C, self)
            003D   0x002248   40 01 0D 00   LoadStaticFloatImm   r1, 13        r1 = ST[13] -> <0.500000>
            003E   0x002250   26 00 00 01   FLessThanEqual       r0, r0, r1    r0 = r0 <= r1
            003F   0x002258   2F 49 00 00   BranchIfNot          r0, 0x49      IF NOT r0 => L_4
            0040   0x002260   15 00 0A 00   LookupPointer        r0, 10        r0 = ST[10] -> <set-boolean>
            0041   0x002268   4A 01 0E 00   LoadStaticU64Imm     r1, 14        r1 = ST[14] -> <#3193E21B9FE4FC53>
            0042   0x002270   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0043   0x002278   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <self>
            0044   0x002280   43 31 01 00   Move                 r49, r1       r49 = #3193E21B9FE4FC53
            0045   0x002288   43 32 02 00   Move                 r50, r2       r50 = 1
            0046   0x002290   43 33 03 00   Move                 r51, r3       r51 = self
            0047   0x002298   1C 00 00 03   CallFf               r0, r0, 3     r0 = set-boolean(#3193E21B9FE4FC53, 1, self)
            0048   0x0022A0   2D 64 00 00   Branch               0x64          GOTO => L_3
          L_4:
            0049   0x0022A8   15 00 04 00   LookupPointer        r0, 4         r0 = ST[4] -> <get-boolean>
            004A   0x0022B0   4A 01 0F 00   LoadStaticU64Imm     r1, 15        r1 = ST[15] -> <#11C859308AA9F60B>
            004B   0x0022B8   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <self>
            004C   0x0022C0   43 31 01 00   Move                 r49, r1       r49 = #11C859308AA9F60B
            004D   0x0022C8   43 32 02 00   Move                 r50, r2       r50 = self
            004E   0x0022D0   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-boolean(symbol: #11C859308AA9F60B, location: self)
            004F   0x0022D8   30 00 00 00   OpLogNot             r0, r0        r0 = !54185015286451590
            0050   0x0022E0   2F 59 00 00   BranchIfNot          r0, 0x63      IF NOT r0 => L_5
            0051   0x0022E8   15 00 07 00   LookupPointer        r0, 7         r0 = ST[7] -> <get-float>
            0052   0x0022F0   4A 01 08 00   LoadStaticU64Imm     r1, 8         r1 = ST[8] -> <#ACCC87ED9AE6837C>
            0053   0x0022F8   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <self>
            0054   0x002300   43 31 01 00   Move                 r49, r1       r49 = #ACCC87ED9AE6837C
            0055   0x002308   43 32 02 00   Move                 r50, r2       r50 = self
            0056   0x002310   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-float(#ACCC87ED9AE6837C, self)
            0057   0x002318   40 01 10 00   LoadStaticFloatImm   r1, 16        r1 = ST[16] -> <1.000000>
            0058   0x002320   24 00 00 01   FGreaterThanEqual    r0, r0, r1    r0 = r0 >= r1
            0059   0x002328   2F 63 00 00   BranchIfNot          r0, 0x63      IF NOT r0 => L_5
            005A   0x002330   15 00 0A 00   LookupPointer        r0, 10        r0 = ST[10] -> <set-boolean>
            005B   0x002338   4A 01 11 00   LoadStaticU64Imm     r1, 17        r1 = ST[17] -> <#29137C2A7D7263BB>
            005C   0x002340   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            005D   0x002348   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <self>
            005E   0x002350   43 31 01 00   Move                 r49, r1       r49 = #29137C2A7D7263BB
            005F   0x002358   43 32 02 00   Move                 r50, r2       r50 = 1
            0060   0x002360   43 33 03 00   Move                 r51, r3       r51 = self
            0061   0x002368   1C 00 00 03   CallFf               r0, r0, 3     r0 = set-boolean(#29137C2A7D7263BB, 1, self)
            0062   0x002370   2D 64 00 00   Branch               0x64          GOTO => L_3
          L_5:
            0063   0x002378   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
          L_3:
            0064   0x002380   15 00 04 00   LookupPointer        r0, 4         r0 = ST[4] -> <get-boolean>
            0065   0x002388   4A 01 12 00   LoadStaticU64Imm     r1, 18        r1 = ST[18] -> <#DBA22D94C61A6F07>
            0066   0x002390   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <self>
            0067   0x002398   43 31 01 00   Move                 r49, r1       r49 = #DBA22D94C61A6F07
            0068   0x0023A0   43 32 02 00   Move                 r50, r2       r50 = self
            0069   0x0023A8   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-boolean(symbol: #DBA22D94C61A6F07, location: self)
            006A   0x0023B0   2F 71 00 00   BranchIfNot          r0, 0xAB      IF NOT r0 => L_6
            006B   0x0023B8   15 00 04 00   LookupPointer        r0, 4         r0 = ST[4] -> <get-boolean>
            006C   0x0023C0   4A 01 0B 00   LoadStaticU64Imm     r1, 11        r1 = ST[11] -> <#52AB8DE16851EBC8>
            006D   0x0023C8   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <self>
            006E   0x0023D0   43 31 01 00   Move                 r49, r1       r49 = #52AB8DE16851EBC8
            006F   0x0023D8   43 32 02 00   Move                 r50, r2       r50 = self
            0070   0x0023E0   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-boolean(symbol: #52AB8DE16851EBC8, location: self)
            0071   0x0023E8   2F 79 00 00   BranchIfNot          r0, 0xAB      IF NOT r0 => L_6
            0072   0x0023F0   15 00 04 00   LookupPointer        r0, 4         r0 = ST[4] -> <get-boolean>
            0073   0x0023F8   4A 01 06 00   LoadStaticU64Imm     r1, 6         r1 = ST[6] -> <#98A1648B00B03E48>
            0074   0x002400   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <self>
            0075   0x002408   43 31 01 00   Move                 r49, r1       r49 = #98A1648B00B03E48
            0076   0x002410   43 32 02 00   Move                 r50, r2       r50 = self
            0077   0x002418   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-boolean(symbol: #98A1648B00B03E48, location: self)
            0078   0x002420   30 00 00 00   OpLogNot             r0, r0        r0 = !54185015286451590
            0079   0x002428   2F AB 00 00   BranchIfNot          r0, 0xAB      IF NOT r0 => L_6
            007A   0x002430   15 00 13 00   LookupPointer        r0, 19        r0 = ST[19] -> <darray-append>
            007B   0x002438   15 01 14 00   LookupPointer        r1, 20        r1 = ST[20] -> <get-symbol>
            007C   0x002440   4A 02 15 00   LoadStaticU64Imm     r2, 21        r2 = ST[21] -> <#D2F26D6B19D49A54>
            007D   0x002448   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <self>
            007E   0x002450   43 31 02 00   Move                 r49, r2       r49 = #D2F26D6B19D49A54
            007F   0x002458   43 32 03 00   Move                 r50, r3       r50 = self
            0080   0x002460   1C 01 01 02   CallFf               r1, r1, 2     r1 = get-symbol(#D2F26D6B19D49A54, self)
            0081   0x002468   15 02 16 00   LookupPointer        r2, 22        r2 = ST[22] -> <new-boxed-value>
            0082   0x002470   0C 03 05 00   LoadU16Imm           r3, 5         r3 = 5
            0083   0x002478   15 04 17 00   LookupPointer        r4, 23        r4 = ST[23] -> <#142211B6C3E9D2D7>
            0084   0x002480   40 05 09 00   LoadStaticFloatImm   r5, 9         r5 = ST[9] -> <0.250000>
            0085   0x002488   43 31 05 00   Move                 r49, r5       r49 = 0.250000
            0086   0x002490   1B 04 04 01   Call                 r4, r4, 1     r4 = #142211B6C3E9D2D7(0.250000)
            0087   0x002498   43 31 03 00   Move                 r49, r3       r49 = 5
            0088   0x0024A0   43 32 04 00   Move                 r50, r4       r50 = RET_#142211B6C3E9D2D7
            0089   0x0024A8   1C 02 02 02   CallFf               r2, r2, 2     r2 = new-boxed-value(5, RET_#142211B6C3E9D2D7)
            008A   0x0024B0   0C 03 00 00   LoadU16Imm           r3, 0         r3 = 0
            008B   0x0024B8   43 31 01 00   Move                 r49, r1       r49 = RET_get-symbol
            008C   0x0024C0   43 32 02 00   Move                 r50, r2       r50 = RET_new-boxed-value
            008D   0x0024C8   43 33 03 00   Move                 r51, r3       r51 = 0
            008E   0x0024D0   1C 00 00 03   CallFf               r0, r0, 3     r0 = darray-append(RET_get-symbol, RET_new-boxed-value, 0)
            008F   0x0024D8   15 00 0A 00   LookupPointer        r0, 10        r0 = ST[10] -> <set-boolean>
            0090   0x0024E0   4A 01 06 00   LoadStaticU64Imm     r1, 6         r1 = ST[6] -> <#98A1648B00B03E48>
            0091   0x0024E8   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0092   0x0024F0   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <self>
            0093   0x0024F8   43 31 01 00   Move                 r49, r1       r49 = #98A1648B00B03E48
            0094   0x002500   43 32 02 00   Move                 r50, r2       r50 = 1
            0095   0x002508   43 33 03 00   Move                 r51, r3       r51 = self
            0096   0x002510   1C 00 00 03   CallFf               r0, r0, 3     r0 = set-boolean(#98A1648B00B03E48, 1, self)
            0097   0x002518   15 00 04 00   LookupPointer        r0, 4         r0 = ST[4] -> <get-boolean>
            0098   0x002520   4A 01 0C 00   LoadStaticU64Imm     r1, 12        r1 = ST[12] -> <#87B5D4A36BD8ECC3>
            0099   0x002528   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <self>
            009A   0x002530   43 31 01 00   Move                 r49, r1       r49 = #87B5D4A36BD8ECC3
            009B   0x002538   43 32 02 00   Move                 r50, r2       r50 = self
            009C   0x002540   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-boolean(symbol: #87B5D4A36BD8ECC3, location: self)
            009D   0x002548   30 00 00 00   OpLogNot             r0, r0        r0 = !54185015286451590
            009E   0x002550   2F A8 00 00   BranchIfNot          r0, 0xA8      IF NOT r0 => L_7
            009F   0x002558   15 00 0A 00   LookupPointer        r0, 10        r0 = ST[10] -> <set-boolean>
            00A0   0x002560   4A 01 0C 00   LoadStaticU64Imm     r1, 12        r1 = ST[12] -> <#87B5D4A36BD8ECC3>
            00A1   0x002568   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            00A2   0x002570   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <self>
            00A3   0x002578   43 31 01 00   Move                 r49, r1       r49 = #87B5D4A36BD8ECC3
            00A4   0x002580   43 32 02 00   Move                 r50, r2       r50 = 1
            00A5   0x002588   43 33 03 00   Move                 r51, r3       r51 = self
            00A6   0x002590   1C 00 00 03   CallFf               r0, r0, 3     r0 = set-boolean(#87B5D4A36BD8ECC3, 1, self)
            00A7   0x002598   2D A9 00 00   Branch               0xA9          GOTO => L_8
          L_7:
            00A8   0x0025A0   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
          L_8:
            00A9   0x0025A8   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
            00AA   0x0025B0   2D 14 00 01   Branch               0x114         GOTO => L_9
          L_6:
            00AB   0x0025B8   15 00 04 00   LookupPointer        r0, 4         r0 = ST[4] -> <get-boolean>
            00AC   0x0025C0   4A 01 18 00   LoadStaticU64Imm     r1, 24        r1 = ST[24] -> <#0D288B4554B68C50>
            00AD   0x0025C8   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <self>
            00AE   0x0025D0   43 31 01 00   Move                 r49, r1       r49 = #0D288B4554B68C50
            00AF   0x0025D8   43 32 02 00   Move                 r50, r2       r50 = self
            00B0   0x0025E0   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-boolean(symbol: #0D288B4554B68C50, location: self)
            00B1   0x0025E8   2F B8 00 00   BranchIfNot          r0, 0xDF      IF NOT r0 => L_10
            00B2   0x0025F0   15 00 04 00   LookupPointer        r0, 4         r0 = ST[4] -> <get-boolean>
            00B3   0x0025F8   4A 01 0E 00   LoadStaticU64Imm     r1, 14        r1 = ST[14] -> <#3193E21B9FE4FC53>
            00B4   0x002600   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <self>
            00B5   0x002608   43 31 01 00   Move                 r49, r1       r49 = #3193E21B9FE4FC53
            00B6   0x002610   43 32 02 00   Move                 r50, r2       r50 = self
            00B7   0x002618   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-boolean(symbol: #3193E21B9FE4FC53, location: self)
            00B8   0x002620   2F C0 00 00   BranchIfNot          r0, 0xDF      IF NOT r0 => L_10
            00B9   0x002628   15 00 04 00   LookupPointer        r0, 4         r0 = ST[4] -> <get-boolean>
            00BA   0x002630   4A 01 0C 00   LoadStaticU64Imm     r1, 12        r1 = ST[12] -> <#87B5D4A36BD8ECC3>
            00BB   0x002638   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <self>
            00BC   0x002640   43 31 01 00   Move                 r49, r1       r49 = #87B5D4A36BD8ECC3
            00BD   0x002648   43 32 02 00   Move                 r50, r2       r50 = self
            00BE   0x002650   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-boolean(symbol: #87B5D4A36BD8ECC3, location: self)
            00BF   0x002658   30 00 00 00   OpLogNot             r0, r0        r0 = !54185015286451590
            00C0   0x002660   2F DF 00 00   BranchIfNot          r0, 0xDF      IF NOT r0 => L_10
            00C1   0x002668   15 00 13 00   LookupPointer        r0, 19        r0 = ST[19] -> <darray-append>
            00C2   0x002670   15 01 14 00   LookupPointer        r1, 20        r1 = ST[20] -> <get-symbol>
            00C3   0x002678   4A 02 15 00   LoadStaticU64Imm     r2, 21        r2 = ST[21] -> <#D2F26D6B19D49A54>
            00C4   0x002680   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <self>
            00C5   0x002688   43 31 02 00   Move                 r49, r2       r49 = #D2F26D6B19D49A54
            00C6   0x002690   43 32 03 00   Move                 r50, r3       r50 = self
            00C7   0x002698   1C 01 01 02   CallFf               r1, r1, 2     r1 = get-symbol(#D2F26D6B19D49A54, self)
            00C8   0x0026A0   15 02 16 00   LookupPointer        r2, 22        r2 = ST[22] -> <new-boxed-value>
            00C9   0x0026A8   0C 03 05 00   LoadU16Imm           r3, 5         r3 = 5
            00CA   0x0026B0   15 04 17 00   LookupPointer        r4, 23        r4 = ST[23] -> <#142211B6C3E9D2D7>
            00CB   0x0026B8   40 05 0D 00   LoadStaticFloatImm   r5, 13        r5 = ST[13] -> <0.500000>
            00CC   0x0026C0   43 31 05 00   Move                 r49, r5       r49 = 0.500000
            00CD   0x0026C8   1B 04 04 01   Call                 r4, r4, 1     r4 = #142211B6C3E9D2D7(0.500000)
            00CE   0x0026D0   43 31 03 00   Move                 r49, r3       r49 = 5
            00CF   0x0026D8   43 32 04 00   Move                 r50, r4       r50 = RET_#142211B6C3E9D2D7
            00D0   0x0026E0   1C 02 02 02   CallFf               r2, r2, 2     r2 = new-boxed-value(5, RET_#142211B6C3E9D2D7)
            00D1   0x0026E8   0C 03 00 00   LoadU16Imm           r3, 0         r3 = 0
            00D2   0x0026F0   43 31 01 00   Move                 r49, r1       r49 = RET_get-symbol
            00D3   0x0026F8   43 32 02 00   Move                 r50, r2       r50 = RET_new-boxed-value
            00D4   0x002700   43 33 03 00   Move                 r51, r3       r51 = 0
            00D5   0x002708   1C 00 00 03   CallFf               r0, r0, 3     r0 = darray-append(RET_get-symbol, RET_new-boxed-value, 0)
            00D6   0x002710   15 00 0A 00   LookupPointer        r0, 10        r0 = ST[10] -> <set-boolean>
            00D7   0x002718   4A 01 0C 00   LoadStaticU64Imm     r1, 12        r1 = ST[12] -> <#87B5D4A36BD8ECC3>
            00D8   0x002720   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            00D9   0x002728   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <self>
            00DA   0x002730   43 31 01 00   Move                 r49, r1       r49 = #87B5D4A36BD8ECC3
            00DB   0x002738   43 32 02 00   Move                 r50, r2       r50 = 1
            00DC   0x002740   43 33 03 00   Move                 r51, r3       r51 = self
            00DD   0x002748   1C 00 00 03   CallFf               r0, r0, 3     r0 = set-boolean(#87B5D4A36BD8ECC3, 1, self)
            00DE   0x002750   2D 14 00 01   Branch               0x114         GOTO => L_9
          L_10:
            00DF   0x002758   15 00 04 00   LookupPointer        r0, 4         r0 = ST[4] -> <get-boolean>
            00E0   0x002760   4A 01 19 00   LoadStaticU64Imm     r1, 25        r1 = ST[25] -> <#934EE4E849CBD538>
            00E1   0x002768   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <self>
            00E2   0x002770   43 31 01 00   Move                 r49, r1       r49 = #934EE4E849CBD538
            00E3   0x002778   43 32 02 00   Move                 r50, r2       r50 = self
            00E4   0x002780   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-boolean(symbol: #934EE4E849CBD538, location: self)
            00E5   0x002788   2F EC 00 00   BranchIfNot          r0, 0x113     IF NOT r0 => L_11
            00E6   0x002790   15 00 04 00   LookupPointer        r0, 4         r0 = ST[4] -> <get-boolean>
            00E7   0x002798   4A 01 11 00   LoadStaticU64Imm     r1, 17        r1 = ST[17] -> <#29137C2A7D7263BB>
            00E8   0x0027A0   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <self>
            00E9   0x0027A8   43 31 01 00   Move                 r49, r1       r49 = #29137C2A7D7263BB
            00EA   0x0027B0   43 32 02 00   Move                 r50, r2       r50 = self
            00EB   0x0027B8   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-boolean(symbol: #29137C2A7D7263BB, location: self)
            00EC   0x0027C0   2F F4 00 00   BranchIfNot          r0, 0x113     IF NOT r0 => L_11
            00ED   0x0027C8   15 00 04 00   LookupPointer        r0, 4         r0 = ST[4] -> <get-boolean>
            00EE   0x0027D0   4A 01 0F 00   LoadStaticU64Imm     r1, 15        r1 = ST[15] -> <#11C859308AA9F60B>
            00EF   0x0027D8   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <self>
            00F0   0x0027E0   43 31 01 00   Move                 r49, r1       r49 = #11C859308AA9F60B
            00F1   0x0027E8   43 32 02 00   Move                 r50, r2       r50 = self
            00F2   0x0027F0   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-boolean(symbol: #11C859308AA9F60B, location: self)
            00F3   0x0027F8   30 00 00 00   OpLogNot             r0, r0        r0 = !54185015286451590
            00F4   0x002800   2F 13 00 01   BranchIfNot          r0, 0x113     IF NOT r0 => L_11
            00F5   0x002808   15 00 13 00   LookupPointer        r0, 19        r0 = ST[19] -> <darray-append>
            00F6   0x002810   15 01 14 00   LookupPointer        r1, 20        r1 = ST[20] -> <get-symbol>
            00F7   0x002818   4A 02 15 00   LoadStaticU64Imm     r2, 21        r2 = ST[21] -> <#D2F26D6B19D49A54>
            00F8   0x002820   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <self>
            00F9   0x002828   43 31 02 00   Move                 r49, r2       r49 = #D2F26D6B19D49A54
            00FA   0x002830   43 32 03 00   Move                 r50, r3       r50 = self
            00FB   0x002838   1C 01 01 02   CallFf               r1, r1, 2     r1 = get-symbol(#D2F26D6B19D49A54, self)
            00FC   0x002840   15 02 16 00   LookupPointer        r2, 22        r2 = ST[22] -> <new-boxed-value>
            00FD   0x002848   0C 03 05 00   LoadU16Imm           r3, 5         r3 = 5
            00FE   0x002850   15 04 17 00   LookupPointer        r4, 23        r4 = ST[23] -> <#142211B6C3E9D2D7>
            00FF   0x002858   40 05 10 00   LoadStaticFloatImm   r5, 16        r5 = ST[16] -> <1.000000>
            0100   0x002860   43 31 05 00   Move                 r49, r5       r49 = 1.000000
            0101   0x002868   1B 04 04 01   Call                 r4, r4, 1     r4 = #142211B6C3E9D2D7(1.000000)
            0102   0x002870   43 31 03 00   Move                 r49, r3       r49 = 5
            0103   0x002878   43 32 04 00   Move                 r50, r4       r50 = RET_#142211B6C3E9D2D7
            0104   0x002880   1C 02 02 02   CallFf               r2, r2, 2     r2 = new-boxed-value(5, RET_#142211B6C3E9D2D7)
            0105   0x002888   0C 03 00 00   LoadU16Imm           r3, 0         r3 = 0
            0106   0x002890   43 31 01 00   Move                 r49, r1       r49 = RET_get-symbol
            0107   0x002898   43 32 02 00   Move                 r50, r2       r50 = RET_new-boxed-value
            0108   0x0028A0   43 33 03 00   Move                 r51, r3       r51 = 0
            0109   0x0028A8   1C 00 00 03   CallFf               r0, r0, 3     r0 = darray-append(RET_get-symbol, RET_new-boxed-value, 0)
            010A   0x0028B0   15 00 0A 00   LookupPointer        r0, 10        r0 = ST[10] -> <set-boolean>
            010B   0x0028B8   4A 01 0F 00   LoadStaticU64Imm     r1, 15        r1 = ST[15] -> <#11C859308AA9F60B>
            010C   0x0028C0   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            010D   0x0028C8   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <self>
            010E   0x0028D0   43 31 01 00   Move                 r49, r1       r49 = #11C859308AA9F60B
            010F   0x0028D8   43 32 02 00   Move                 r50, r2       r50 = 1
            0110   0x0028E0   43 33 03 00   Move                 r51, r3       r51 = self
            0111   0x0028E8   1C 00 00 03   CallFf               r0, r0, 3     r0 = set-boolean(#11C859308AA9F60B, 1, self)
            0112   0x0028F0   2D 14 00 01   Branch               0x114         GOTO => L_9
          L_11:
            0113   0x0028F8   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
          L_9:
            0114   0x002900   2D 16 00 01   Branch               0x116         GOTO => L_12
          L_1:
            0115   0x002908   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
          L_12:
            0116   0x002910   2D 18 00 01   Branch               0x118         GOTO => L_RETURN
          L_RETURN_0:
            0117   0x002918   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
          L_RETURN:
            0118   0x002920   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x002928    function: get-black-fade
            0001   0x002930    function: timer-running?
            0002   0x002938    sid: #8FFD0BAEEF4F2DB1
            0003   0x002940    sid: self
            0004   0x002948    function: get-boolean(sid, sid) -> bool
            0005   0x002950    sid: #60C441473AC3126E
            0006   0x002958    sid: #98A1648B00B03E48
            0007   0x002960    function: get-float
            0008   0x002968    sid: #ACCC87ED9AE6837C
            0009   0x002970    float: 0.250000
            000A   0x002978    function: set-boolean
            000B   0x002980    sid: #52AB8DE16851EBC8
            000C   0x002988    sid: #87B5D4A36BD8ECC3
            000D   0x002990    float: 0.500000
            000E   0x002998    sid: #3193E21B9FE4FC53
            000F   0x0029A0    sid: #11C859308AA9F60B
            0010   0x0029A8    float: 1.000000
            0011   0x0029B0    sid: #29137C2A7D7263BB
            0012   0x0029B8    sid: #DBA22D94C61A6F07
            0013   0x0029C0    function: darray-append
            0014   0x0029C8    function: get-symbol
            0015   0x0029D0    sid: #D2F26D6B19D49A54
            0016   0x0029D8    function: new-boxed-value
            0017   0x0029E0    function: #142211B6C3E9D2D7
            0018   0x0029E8    sid: #0D288B4554B68C50
            0019   0x0029F0    sid: #934EE4E849CBD538
          }
          {
            0000   0x001BA8   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
            0001   0x001BB0   0C 01 00 00   LoadU16Imm           r1, 0         r1 = 0
            0002   0x001BB8   15 02 00 00   LookupPointer        r2, 0         r2 = ST[0] -> <darray-count>
            0003   0x001BC0   15 03 01 00   LookupPointer        r3, 1         r3 = ST[1] -> <get-symbol>
            0004   0x001BC8   4A 04 02 00   LoadStaticU64Imm     r4, 2         r4 = ST[2] -> <#D2F26D6B19D49A54>
            0005   0x001BD0   4A 05 03 00   LoadStaticU64Imm     r5, 3         r5 = ST[3] -> <self>
            0006   0x001BD8   43 31 04 00   Move                 r49, r4       r49 = #D2F26D6B19D49A54
            0007   0x001BE0   43 32 05 00   Move                 r50, r5       r50 = self
            0008   0x001BE8   1C 03 03 02   CallFf               r3, r3, 2     r3 = get-symbol(#D2F26D6B19D49A54, self)
            0009   0x001BF0   43 31 03 00   Move                 r49, r3       r49 = RET_get-symbol
            000A   0x001BF8   1C 02 02 01   CallFf               r2, r2, 1     r2 = darray-count(RET_get-symbol)
            000B   0x001C00   15 03 04 00   LookupPointer        r3, 4         r3 = ST[4] -> <begin-foreach>
            000C   0x001C08   1C 03 03 00   CallFf               r3, r3, 0     r3 = begin-foreach()
          L_10:
            000D   0x001C10   43 03 01 00   Move                 r3, r1        r3 = 0
            000E   0x001C18   43 04 02 00   Move                 r4, r2        r4 = RET_darray-count
            000F   0x001C20   20 03 03 04   ILessThan            r3, r3, r4    r3 = r3 < r4
            0010   0x001C28   2F 7E 03 00   BranchIfNot          r3, 0x7E      IF NOT r3 => L_0
            0011   0x001C30   15 03 05 00   LookupPointer        r3, 5         r3 = ST[5] -> <darray-at>
            0012   0x001C38   15 04 01 00   LookupPointer        r4, 1         r4 = ST[1] -> <get-symbol>
            0013   0x001C40   4A 05 02 00   LoadStaticU64Imm     r5, 2         r5 = ST[2] -> <#D2F26D6B19D49A54>
            0014   0x001C48   4A 06 03 00   LoadStaticU64Imm     r6, 3         r6 = ST[3] -> <self>
            0015   0x001C50   43 31 05 00   Move                 r49, r5       r49 = #D2F26D6B19D49A54
            0016   0x001C58   43 32 06 00   Move                 r50, r6       r50 = self
            0017   0x001C60   1C 04 04 02   CallFf               r4, r4, 2     r4 = get-symbol(#D2F26D6B19D49A54, self)
            0018   0x001C68   43 05 01 00   Move                 r5, r1        r5 = 0
            0019   0x001C70   43 31 04 00   Move                 r49, r4       r49 = RET_get-symbol
            001A   0x001C78   43 32 05 00   Move                 r50, r5       r50 = 0
            001B   0x001C80   1C 03 03 02   CallFf               r3, r3, 2     r3 = darray-at(RET_get-symbol, 0)
            001C   0x001C88   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            001D   0x001C90   2F 24 04 00   BranchIfNot          r4, 0x29      IF NOT r4 => L_1
            001E   0x001C98   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            001F   0x001CA0   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            0020   0x001CA8   3B 04 04 0C   IAddImm              r4, r4, 12    r4 = RET_darray-at + 12 -> <RET_(null)>
            0021   0x001CB0   4E 04 04 00   LoadU16              r4, [r4]      r4 = *(u16*)RET_darray-at
            0022   0x001CB8   0C 05 01 00   LoadU16Imm           r5, 1         r5 = 1
            0023   0x001CC0   1D 04 04 05   IEqual               r4, r4, r5    r4 = r4 == r5
            0024   0x001CC8   2F 29 04 00   BranchIfNot          r4, 0x29      IF NOT r4 => L_1
            0025   0x001CD0   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            0026   0x001CD8   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            0027   0x001CE0   4C 04 04 00   LoadU8               r4, [r4]      r4 = *(u8*)RET_darray-at
            0028   0x001CE8   2D 6C 00 00   Branch               0x6C          GOTO => L_2
          L_1:
            0029   0x001CF0   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            002A   0x001CF8   2F 31 04 00   BranchIfNot          r4, 0x36      IF NOT r4 => L_3
            002B   0x001D00   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            002C   0x001D08   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            002D   0x001D10   3B 04 04 0C   IAddImm              r4, r4, 12    r4 = RET_darray-at + 12 -> <RET_(null)>
            002E   0x001D18   4E 04 04 00   LoadU16              r4, [r4]      r4 = *(u16*)RET_darray-at
            002F   0x001D20   0C 05 02 00   LoadU16Imm           r5, 2         r5 = 2
            0030   0x001D28   1D 04 04 05   IEqual               r4, r4, r5    r4 = r4 == r5
            0031   0x001D30   2F 36 04 00   BranchIfNot          r4, 0x36      IF NOT r4 => L_3
            0032   0x001D38   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            0033   0x001D40   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            0034   0x001D48   4F 04 04 00   LoadI32              r4, [r4]      r4 = *(i32*)RET_darray-at
            0035   0x001D50   2D 6C 00 00   Branch               0x6C          GOTO => L_2
          L_3:
            0036   0x001D58   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            0037   0x001D60   2F 3E 04 00   BranchIfNot          r4, 0x43      IF NOT r4 => L_4
            0038   0x001D68   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            0039   0x001D70   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            003A   0x001D78   3B 04 04 0C   IAddImm              r4, r4, 12    r4 = RET_darray-at + 12 -> <RET_(null)>
            003B   0x001D80   4E 04 04 00   LoadU16              r4, [r4]      r4 = *(u16*)RET_darray-at
            003C   0x001D88   0C 05 03 00   LoadU16Imm           r5, 3         r5 = 3
            003D   0x001D90   1D 04 04 05   IEqual               r4, r4, r5    r4 = r4 == r5
            003E   0x001D98   2F 43 04 00   BranchIfNot          r4, 0x43      IF NOT r4 => L_4
            003F   0x001DA0   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            0040   0x001DA8   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            0041   0x001DB0   0D 04 04 00   LoadU32              r4, [r4]      r4 = *(u32*)RET_darray-at
            0042   0x001DB8   2D 6C 00 00   Branch               0x6C          GOTO => L_2
          L_4:
            0043   0x001DC0   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            0044   0x001DC8   2F 4B 04 00   BranchIfNot          r4, 0x50      IF NOT r4 => L_5
            0045   0x001DD0   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            0046   0x001DD8   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            0047   0x001DE0   3B 04 04 0C   IAddImm              r4, r4, 12    r4 = RET_darray-at + 12 -> <RET_(null)>
            0048   0x001DE8   4E 04 04 00   LoadU16              r4, [r4]      r4 = *(u16*)RET_darray-at
            0049   0x001DF0   0C 05 04 00   LoadU16Imm           r5, 4         r5 = 4
            004A   0x001DF8   1D 04 04 05   IEqual               r4, r4, r5    r4 = r4 == r5
            004B   0x001E00   2F 50 04 00   BranchIfNot          r4, 0x50      IF NOT r4 => L_5
            004C   0x001E08   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            004D   0x001E10   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            004E   0x001E18   50 04 04 00   LoadI64              r4, [r4]      r4 = *(i64*)RET_darray-at
            004F   0x001E20   2D 6C 00 00   Branch               0x6C          GOTO => L_2
          L_5:
            0050   0x001E28   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            0051   0x001E30   2F 58 04 00   BranchIfNot          r4, 0x5D      IF NOT r4 => L_6
            0052   0x001E38   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            0053   0x001E40   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            0054   0x001E48   3B 04 04 0C   IAddImm              r4, r4, 12    r4 = RET_darray-at + 12 -> <RET_(null)>
            0055   0x001E50   4E 04 04 00   LoadU16              r4, [r4]      r4 = *(u16*)RET_darray-at
            0056   0x001E58   0C 05 05 00   LoadU16Imm           r5, 5         r5 = 5
            0057   0x001E60   1D 04 04 05   IEqual               r4, r4, r5    r4 = r4 == r5
            0058   0x001E68   2F 5D 04 00   BranchIfNot          r4, 0x5D      IF NOT r4 => L_6
            0059   0x001E70   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            005A   0x001E78   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            005B   0x001E80   51 04 04 00   LoadU64              r4, [r4]      r4 = *(u64*)RET_darray-at
            005C   0x001E88   2D 6C 00 00   Branch               0x6C          GOTO => L_2
          L_6:
            005D   0x001E90   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            005E   0x001E98   2F 65 04 00   BranchIfNot          r4, 0x6B      IF NOT r4 => L_7
            005F   0x001EA0   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            0060   0x001EA8   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            0061   0x001EB0   3B 04 04 0C   IAddImm              r4, r4, 12    r4 = RET_darray-at + 12 -> <RET_(null)>
            0062   0x001EB8   4E 04 04 00   LoadU16              r4, [r4]      r4 = *(u16*)RET_darray-at
            0063   0x001EC0   0C 05 06 00   LoadU16Imm           r5, 6         r5 = 6
            0064   0x001EC8   1D 04 04 05   IEqual               r4, r4, r5    r4 = r4 == r5
            0065   0x001ED0   2F 6B 04 00   BranchIfNot          r4, 0x6B      IF NOT r4 => L_7
            0066   0x001ED8   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            0067   0x001EE0   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            0068   0x001EE8   0E 04 04 00   LoadFloat            r4, [r4]      r4 = *(f32*)RET_darray-at
            0069   0x001EF0   19 04 04 00   CastInteger          r4, r4        r4 = int(r4) -> <?> => <?>
            006A   0x001EF8   2D 6C 00 00   Branch               0x6C          GOTO => L_2
          L_7:
            006B   0x001F00   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
          L_2:
            006C   0x001F08   43 03 04 00   Move                 r3, r4        r3 = 0
            006D   0x001F10   15 04 06 00   LookupPointer        r4, 6         r4 = ST[6] -> <text-to-speech-frametick-active?>
            006E   0x001F18   43 05 03 00   Move                 r5, r3        r5 = 0
            006F   0x001F20   43 31 05 00   Move                 r49, r5       r49 = 0
            0070   0x001F28   1C 04 04 01   CallFf               r4, r4, 1     r4 = text-to-speech-frametick-active?(0)
            0071   0x001F30   2F 76 04 00   BranchIfNot          r4, 0x76      IF NOT r4 => L_8
            0072   0x001F38   0C 04 01 00   LoadU16Imm           r4, 1         r4 = 1
            0073   0x001F40   43 00 04 00   Move                 r0, r4        r0 = 1
            0074   0x001F48   43 04 00 00   Move                 r4, r0        r4 = 1
            0075   0x001F50   2D 77 00 00   Branch               0x77          GOTO => L_9
          L_8:
            0076   0x001F58   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
          L_9:
            0077   0x001F60   43 04 01 00   Move                 r4, r1        r4 = 0
            0078   0x001F68   0C 05 01 00   LoadU16Imm           r5, 1         r5 = 1
            0079   0x001F70   01 04 04 05   IAdd                 r4, r4, r5    r4 = 0 + 1
            007A   0x001F78   43 01 04 00   Move                 r1, r4        r1 = 0
            007B   0x001F80   43 04 01 00   Move                 r4, r1        r4 = 0
            007C   0x001F88   43 03 04 00   Move                 r3, r4        r3 = 0
            007D   0x001F90   2D 0D 00 00   Branch               0xD           GOTO => L_10
          L_0:
            007E   0x001F98   15 03 07 00   LookupPointer        r3, 7         r3 = ST[7] -> <end-foreach>
            007F   0x001FA0   1C 03 03 00   CallFf               r3, r3, 0     r3 = end-foreach()
            0080   0x001FA8   43 01 03 00   Move                 r1, r3        r1 = RET_end-foreach
            0081   0x001FB0   43 01 00 00   Move                 r1, r0        r1 = 1
            0082   0x001FB8   43 00 01 00   Move                 r0, r1        r0 = 1
            0083   0x001FC0   2F 8B 00 00   BranchIfNot          r0, 0x8B      IF NOT r0 => L_RETURN_0
            0084   0x001FC8   15 00 08 00   LookupPointer        r0, 8         r0 = ST[8] -> <go>
            0085   0x001FD0   4A 01 09 00   LoadStaticU64Imm     r1, 9         r1 = ST[9] -> <wait-for-tts>
            0086   0x001FD8   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0087   0x001FE0   43 31 01 00   Move                 r49, r1       r49 = wait-for-tts
            0088   0x001FE8   43 32 02 00   Move                 r50, r2       r50 = 1
            0089   0x001FF0   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(wait-for-tts, 1)
            008A   0x001FF8   2D 8C 00 00   Branch               0x8C          GOTO => L_RETURN
          L_RETURN_0:
            008B   0x002000   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
          L_RETURN:
            008C   0x002008   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x002010    function: darray-count
            0001   0x002018    function: get-symbol
            0002   0x002020    sid: #D2F26D6B19D49A54
            0003   0x002028    sid: self
            0004   0x002030    function: begin-foreach
            0005   0x002038    function: darray-at
            0006   0x002040    function: text-to-speech-frametick-active?
            0007   0x002048    function: end-foreach
            0008   0x002050    function: go
            0009   0x002058    sid: wait-for-tts
          }
        }

      }
    } END STATE tracking

    STATE wait-for-tts {
      ON start {
        TRACK main {
          {
          L_12:
            0000   0x002AF8   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
            0001   0x002B00   0C 01 00 00   LoadU16Imm           r1, 0         r1 = 0
            0002   0x002B08   15 02 00 00   LookupPointer        r2, 0         r2 = ST[0] -> <darray-count>
            0003   0x002B10   15 03 01 00   LookupPointer        r3, 1         r3 = ST[1] -> <get-symbol>
            0004   0x002B18   4A 04 02 00   LoadStaticU64Imm     r4, 2         r4 = ST[2] -> <#D2F26D6B19D49A54>
            0005   0x002B20   4A 05 03 00   LoadStaticU64Imm     r5, 3         r5 = ST[3] -> <self>
            0006   0x002B28   43 31 04 00   Move                 r49, r4       r49 = #D2F26D6B19D49A54
            0007   0x002B30   43 32 05 00   Move                 r50, r5       r50 = self
            0008   0x002B38   1C 03 03 02   CallFf               r3, r3, 2     r3 = get-symbol(#D2F26D6B19D49A54, self)
            0009   0x002B40   43 31 03 00   Move                 r49, r3       r49 = RET_get-symbol
            000A   0x002B48   1C 02 02 01   CallFf               r2, r2, 1     r2 = darray-count(RET_get-symbol)
            000B   0x002B50   15 03 04 00   LookupPointer        r3, 4         r3 = ST[4] -> <begin-foreach>
            000C   0x002B58   1C 03 03 00   CallFf               r3, r3, 0     r3 = begin-foreach()
          L_10:
            000D   0x002B60   43 03 01 00   Move                 r3, r1        r3 = 0
            000E   0x002B68   43 04 02 00   Move                 r4, r2        r4 = RET_darray-count
            000F   0x002B70   20 03 03 04   ILessThan            r3, r3, r4    r3 = r3 < r4
            0010   0x002B78   2F 7E 03 00   BranchIfNot          r3, 0x7E      IF NOT r3 => L_0
            0011   0x002B80   15 03 05 00   LookupPointer        r3, 5         r3 = ST[5] -> <darray-at>
            0012   0x002B88   15 04 01 00   LookupPointer        r4, 1         r4 = ST[1] -> <get-symbol>
            0013   0x002B90   4A 05 02 00   LoadStaticU64Imm     r5, 2         r5 = ST[2] -> <#D2F26D6B19D49A54>
            0014   0x002B98   4A 06 03 00   LoadStaticU64Imm     r6, 3         r6 = ST[3] -> <self>
            0015   0x002BA0   43 31 05 00   Move                 r49, r5       r49 = #D2F26D6B19D49A54
            0016   0x002BA8   43 32 06 00   Move                 r50, r6       r50 = self
            0017   0x002BB0   1C 04 04 02   CallFf               r4, r4, 2     r4 = get-symbol(#D2F26D6B19D49A54, self)
            0018   0x002BB8   43 05 01 00   Move                 r5, r1        r5 = 0
            0019   0x002BC0   43 31 04 00   Move                 r49, r4       r49 = RET_get-symbol
            001A   0x002BC8   43 32 05 00   Move                 r50, r5       r50 = 0
            001B   0x002BD0   1C 03 03 02   CallFf               r3, r3, 2     r3 = darray-at(RET_get-symbol, 0)
            001C   0x002BD8   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            001D   0x002BE0   2F 24 04 00   BranchIfNot          r4, 0x29      IF NOT r4 => L_1
            001E   0x002BE8   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            001F   0x002BF0   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            0020   0x002BF8   3B 04 04 0C   IAddImm              r4, r4, 12    r4 = RET_darray-at + 12 -> <RET_(null)>
            0021   0x002C00   4E 04 04 00   LoadU16              r4, [r4]      r4 = *(u16*)RET_darray-at
            0022   0x002C08   0C 05 01 00   LoadU16Imm           r5, 1         r5 = 1
            0023   0x002C10   1D 04 04 05   IEqual               r4, r4, r5    r4 = r4 == r5
            0024   0x002C18   2F 29 04 00   BranchIfNot          r4, 0x29      IF NOT r4 => L_1
            0025   0x002C20   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            0026   0x002C28   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            0027   0x002C30   4C 04 04 00   LoadU8               r4, [r4]      r4 = *(u8*)RET_darray-at
            0028   0x002C38   2D 6C 00 00   Branch               0x6C          GOTO => L_2
          L_1:
            0029   0x002C40   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            002A   0x002C48   2F 31 04 00   BranchIfNot          r4, 0x36      IF NOT r4 => L_3
            002B   0x002C50   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            002C   0x002C58   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            002D   0x002C60   3B 04 04 0C   IAddImm              r4, r4, 12    r4 = RET_darray-at + 12 -> <RET_(null)>
            002E   0x002C68   4E 04 04 00   LoadU16              r4, [r4]      r4 = *(u16*)RET_darray-at
            002F   0x002C70   0C 05 02 00   LoadU16Imm           r5, 2         r5 = 2
            0030   0x002C78   1D 04 04 05   IEqual               r4, r4, r5    r4 = r4 == r5
            0031   0x002C80   2F 36 04 00   BranchIfNot          r4, 0x36      IF NOT r4 => L_3
            0032   0x002C88   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            0033   0x002C90   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            0034   0x002C98   4F 04 04 00   LoadI32              r4, [r4]      r4 = *(i32*)RET_darray-at
            0035   0x002CA0   2D 6C 00 00   Branch               0x6C          GOTO => L_2
          L_3:
            0036   0x002CA8   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            0037   0x002CB0   2F 3E 04 00   BranchIfNot          r4, 0x43      IF NOT r4 => L_4
            0038   0x002CB8   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            0039   0x002CC0   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            003A   0x002CC8   3B 04 04 0C   IAddImm              r4, r4, 12    r4 = RET_darray-at + 12 -> <RET_(null)>
            003B   0x002CD0   4E 04 04 00   LoadU16              r4, [r4]      r4 = *(u16*)RET_darray-at
            003C   0x002CD8   0C 05 03 00   LoadU16Imm           r5, 3         r5 = 3
            003D   0x002CE0   1D 04 04 05   IEqual               r4, r4, r5    r4 = r4 == r5
            003E   0x002CE8   2F 43 04 00   BranchIfNot          r4, 0x43      IF NOT r4 => L_4
            003F   0x002CF0   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            0040   0x002CF8   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            0041   0x002D00   0D 04 04 00   LoadU32              r4, [r4]      r4 = *(u32*)RET_darray-at
            0042   0x002D08   2D 6C 00 00   Branch               0x6C          GOTO => L_2
          L_4:
            0043   0x002D10   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            0044   0x002D18   2F 4B 04 00   BranchIfNot          r4, 0x50      IF NOT r4 => L_5
            0045   0x002D20   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            0046   0x002D28   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            0047   0x002D30   3B 04 04 0C   IAddImm              r4, r4, 12    r4 = RET_darray-at + 12 -> <RET_(null)>
            0048   0x002D38   4E 04 04 00   LoadU16              r4, [r4]      r4 = *(u16*)RET_darray-at
            0049   0x002D40   0C 05 04 00   LoadU16Imm           r5, 4         r5 = 4
            004A   0x002D48   1D 04 04 05   IEqual               r4, r4, r5    r4 = r4 == r5
            004B   0x002D50   2F 50 04 00   BranchIfNot          r4, 0x50      IF NOT r4 => L_5
            004C   0x002D58   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            004D   0x002D60   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            004E   0x002D68   50 04 04 00   LoadI64              r4, [r4]      r4 = *(i64*)RET_darray-at
            004F   0x002D70   2D 6C 00 00   Branch               0x6C          GOTO => L_2
          L_5:
            0050   0x002D78   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            0051   0x002D80   2F 58 04 00   BranchIfNot          r4, 0x5D      IF NOT r4 => L_6
            0052   0x002D88   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            0053   0x002D90   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            0054   0x002D98   3B 04 04 0C   IAddImm              r4, r4, 12    r4 = RET_darray-at + 12 -> <RET_(null)>
            0055   0x002DA0   4E 04 04 00   LoadU16              r4, [r4]      r4 = *(u16*)RET_darray-at
            0056   0x002DA8   0C 05 05 00   LoadU16Imm           r5, 5         r5 = 5
            0057   0x002DB0   1D 04 04 05   IEqual               r4, r4, r5    r4 = r4 == r5
            0058   0x002DB8   2F 5D 04 00   BranchIfNot          r4, 0x5D      IF NOT r4 => L_6
            0059   0x002DC0   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            005A   0x002DC8   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            005B   0x002DD0   51 04 04 00   LoadU64              r4, [r4]      r4 = *(u64*)RET_darray-at
            005C   0x002DD8   2D 6C 00 00   Branch               0x6C          GOTO => L_2
          L_6:
            005D   0x002DE0   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            005E   0x002DE8   2F 65 04 00   BranchIfNot          r4, 0x6B      IF NOT r4 => L_7
            005F   0x002DF0   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            0060   0x002DF8   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            0061   0x002E00   3B 04 04 0C   IAddImm              r4, r4, 12    r4 = RET_darray-at + 12 -> <RET_(null)>
            0062   0x002E08   4E 04 04 00   LoadU16              r4, [r4]      r4 = *(u16*)RET_darray-at
            0063   0x002E10   0C 05 06 00   LoadU16Imm           r5, 6         r5 = 6
            0064   0x002E18   1D 04 04 05   IEqual               r4, r4, r5    r4 = r4 == r5
            0065   0x002E20   2F 6B 04 00   BranchIfNot          r4, 0x6B      IF NOT r4 => L_7
            0066   0x002E28   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
            0067   0x002E30   5D 04 00 00   AssertPointer        r4            r4 != nullptr
            0068   0x002E38   0E 04 04 00   LoadFloat            r4, [r4]      r4 = *(f32*)RET_darray-at
            0069   0x002E40   19 04 04 00   CastInteger          r4, r4        r4 = int(r4) -> <?> => <?>
            006A   0x002E48   2D 6C 00 00   Branch               0x6C          GOTO => L_2
          L_7:
            006B   0x002E50   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
          L_2:
            006C   0x002E58   43 03 04 00   Move                 r3, r4        r3 = 0
            006D   0x002E60   15 04 06 00   LookupPointer        r4, 6         r4 = ST[6] -> <text-to-speech-frametick-active?>
            006E   0x002E68   43 05 03 00   Move                 r5, r3        r5 = 0
            006F   0x002E70   43 31 05 00   Move                 r49, r5       r49 = 0
            0070   0x002E78   1C 04 04 01   CallFf               r4, r4, 1     r4 = text-to-speech-frametick-active?(0)
            0071   0x002E80   2F 76 04 00   BranchIfNot          r4, 0x76      IF NOT r4 => L_8
            0072   0x002E88   0C 04 01 00   LoadU16Imm           r4, 1         r4 = 1
            0073   0x002E90   43 00 04 00   Move                 r0, r4        r0 = 1
            0074   0x002E98   43 04 00 00   Move                 r4, r0        r4 = 1
            0075   0x002EA0   2D 77 00 00   Branch               0x77          GOTO => L_9
          L_8:
            0076   0x002EA8   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
          L_9:
            0077   0x002EB0   43 04 01 00   Move                 r4, r1        r4 = 0
            0078   0x002EB8   0C 05 01 00   LoadU16Imm           r5, 1         r5 = 1
            0079   0x002EC0   01 04 04 05   IAdd                 r4, r4, r5    r4 = 0 + 1
            007A   0x002EC8   43 01 04 00   Move                 r1, r4        r1 = 0
            007B   0x002ED0   43 04 01 00   Move                 r4, r1        r4 = 0
            007C   0x002ED8   43 03 04 00   Move                 r3, r4        r3 = 0
            007D   0x002EE0   2D 0D 00 00   Branch               0xD           GOTO => L_10
          L_0:
            007E   0x002EE8   15 03 07 00   LookupPointer        r3, 7         r3 = ST[7] -> <end-foreach>
            007F   0x002EF0   1C 03 03 00   CallFf               r3, r3, 0     r3 = end-foreach()
            0080   0x002EF8   43 01 03 00   Move                 r1, r3        r1 = RET_end-foreach
            0081   0x002F00   43 01 00 00   Move                 r1, r0        r1 = 1
            0082   0x002F08   43 00 01 00   Move                 r0, r1        r0 = 1
            0083   0x002F10   30 00 00 00   OpLogNot             r0, r0        r0 = !1
            0084   0x002F18   30 00 00 00   OpLogNot             r0, r0        r0 = !1
            0085   0x002F20   2F 89 00 00   BranchIfNot          r0, 0x89      IF NOT r0 => L_RETURN
            0086   0x002F28   15 00 08 00   LookupPointer        r0, 8         r0 = ST[8] -> <wait-one-frame>
            0087   0x002F30   1C 00 00 00   CallFf               r0, r0, 0     r0 = wait-one-frame()
            0088   0x002F38   2D 00 00 00   Branch               0x0           GOTO => L_12
          L_RETURN:
            0089   0x002F40   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x002F48    function: darray-count
            0001   0x002F50    function: get-symbol
            0002   0x002F58    sid: #D2F26D6B19D49A54
            0003   0x002F60    sid: self
            0004   0x002F68    function: begin-foreach
            0005   0x002F70    function: darray-at
            0006   0x002F78    function: text-to-speech-frametick-active?
            0007   0x002F80    function: end-foreach
            0008   0x002F88    function: wait-one-frame
          }
          {
            0000   0x001AB8   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <start-timer>
            0001   0x001AC0   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <#8FFD0BAEEF4F2DB1>
            0002   0x001AC8   15 02 02 00   LookupPointer        r2, 2         r2 = ST[2] -> <get-float>
            0003   0x001AD0   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <#C9D6A8C470495D51>
            0004   0x001AD8   4A 04 04 00   LoadStaticU64Imm     r4, 4         r4 = ST[4] -> <self>
            0005   0x001AE0   43 31 03 00   Move                 r49, r3       r49 = #C9D6A8C470495D51
            0006   0x001AE8   43 32 04 00   Move                 r50, r4       r50 = self
            0007   0x001AF0   1C 02 02 02   CallFf               r2, r2, 2     r2 = get-float(#C9D6A8C470495D51, self)
            0008   0x001AF8   4A 03 04 00   LoadStaticU64Imm     r3, 4         r3 = ST[4] -> <self>
            0009   0x001B00   0C 04 01 00   LoadU16Imm           r4, 1         r4 = 1
            000A   0x001B08   43 31 01 00   Move                 r49, r1       r49 = #8FFD0BAEEF4F2DB1
            000B   0x001B10   43 32 02 00   Move                 r50, r2       r50 = RET_get-float
            000C   0x001B18   43 33 03 00   Move                 r51, r3       r51 = self
            000D   0x001B20   43 34 04 00   Move                 r52, r4       r52 = 1
            000E   0x001B28   1B 00 00 04   Call                 r0, r0, 4     r0 = start-timer(#8FFD0BAEEF4F2DB1, RET_get-float, self, 1)
            000F   0x001B30   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x001B38    function: start-timer
            0001   0x001B40    sid: #8FFD0BAEEF4F2DB1
            0002   0x001B48    function: get-float
            0003   0x001B50    sid: #C9D6A8C470495D51
            0004   0x001B58    sid: self
          }
          {
            0000   0x003500   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <darray-clear>
            0001   0x003508   15 01 01 00   LookupPointer        r1, 1         r1 = ST[1] -> <get-symbol>
            0002   0x003510   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <#D2F26D6B19D49A54>
            0003   0x003518   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <self>
            0004   0x003520   43 31 02 00   Move                 r49, r2       r49 = #D2F26D6B19D49A54
            0005   0x003528   43 32 03 00   Move                 r50, r3       r50 = self
            0006   0x003530   1C 01 01 02   CallFf               r1, r1, 2     r1 = get-symbol(#D2F26D6B19D49A54, self)
            0007   0x003538   43 31 01 00   Move                 r49, r1       r49 = RET_get-symbol
            0008   0x003540   1C 00 00 01   CallFf               r0, r0, 1     r0 = darray-clear(RET_get-symbol)
            0009   0x003548   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003550    function: darray-clear
            0001   0x003558    function: get-symbol
            0002   0x003560    sid: #D2F26D6B19D49A54
            0003   0x003568    sid: self
          }
          {
            0000   0x001B60   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <go>
            0001   0x001B68   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <tracking>
            0002   0x001B70   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0003   0x001B78   43 31 01 00   Move                 r49, r1       r49 = tracking
            0004   0x001B80   43 32 02 00   Move                 r50, r2       r50 = 1
            0005   0x001B88   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(tracking, 1)
            0006   0x001B90   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x001B98    function: go
            0001   0x001BA0    sid: tracking
          }
        }

      }
    } END STATE wait-for-tts

    STATE done {
    } END STATE done

}


##############################  ENTRY 2  ##############################

#9B6270342402C4F8 = script-lambda [0x00838] {
    0000   0x000888   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <npc-alive?>
    0001   0x000890   15 01 01 00   LookupPointer        r1, 1         r1 = ST[1] -> <get-symbol>
    0002   0x000898   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <npc>
    0003   0x0008A0   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <self>
    0004   0x0008A8   43 31 02 00   Move                 r49, r2       r49 = npc
    0005   0x0008B0   43 32 03 00   Move                 r50, r3       r50 = self
    0006   0x0008B8   1C 01 01 02   CallFf               r1, r1, 2     r1 = get-symbol(npc, self)
    0007   0x0008C0   43 31 01 00   Move                 r49, r1       r49 = RET_get-symbol
    0008   0x0008C8   1C 00 00 01   CallFf               r0, r0, 1     r0 = npc-alive?(RET_get-symbol)
    0009   0x0008D0   2F 63 00 00   BranchIfNot          r0, 0x63      IF NOT r0 => L_RETURN_0
    000A   0x0008D8   15 00 04 00   LookupPointer        r0, 4         r0 = ST[4] -> <get-boolean>
    000B   0x0008E0   4A 01 05 00   LoadStaticU64Imm     r1, 5         r1 = ST[5] -> <#98A1648B00B03E48>
    000C   0x0008E8   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <self>
    000D   0x0008F0   43 31 01 00   Move                 r49, r1       r49 = #98A1648B00B03E48
    000E   0x0008F8   43 32 02 00   Move                 r50, r2       r50 = self
    000F   0x000900   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-boolean(symbol: #98A1648B00B03E48, location: self)
    0010   0x000908   2F 27 00 00   BranchIfNot          r0, 0x27      IF NOT r0 => L_1
    0011   0x000910   15 00 06 00   LookupPointer        r0, 6         r0 = ST[6] -> <darray-append>
    0012   0x000918   15 01 01 00   LookupPointer        r1, 1         r1 = ST[1] -> <get-symbol>
    0013   0x000920   4A 02 07 00   LoadStaticU64Imm     r2, 7         r2 = ST[7] -> <#D2F26D6B19D49A54>
    0014   0x000928   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <self>
    0015   0x000930   43 31 02 00   Move                 r49, r2       r49 = #D2F26D6B19D49A54
    0016   0x000938   43 32 03 00   Move                 r50, r3       r50 = self
    0017   0x000940   1C 01 01 02   CallFf               r1, r1, 2     r1 = get-symbol(#D2F26D6B19D49A54, self)
    0018   0x000948   15 02 08 00   LookupPointer        r2, 8         r2 = ST[8] -> <new-boxed-value>
    0019   0x000950   0C 03 05 00   LoadU16Imm           r3, 5         r3 = 5
    001A   0x000958   15 04 09 00   LookupPointer        r4, 9         r4 = ST[9] -> <#142211B6C3E9D2D7>
    001B   0x000960   40 05 0A 00   LoadStaticFloatImm   r5, 10        r5 = ST[10] -> <0.250000>
    001C   0x000968   43 31 05 00   Move                 r49, r5       r49 = 0.250000
    001D   0x000970   1B 04 04 01   Call                 r4, r4, 1     r4 = #142211B6C3E9D2D7(0.250000)
    001E   0x000978   43 31 03 00   Move                 r49, r3       r49 = 5
    001F   0x000980   43 32 04 00   Move                 r50, r4       r50 = RET_#142211B6C3E9D2D7
    0020   0x000988   1C 02 02 02   CallFf               r2, r2, 2     r2 = new-boxed-value(5, RET_#142211B6C3E9D2D7)
    0021   0x000990   0C 03 00 00   LoadU16Imm           r3, 0         r3 = 0
    0022   0x000998   43 31 01 00   Move                 r49, r1       r49 = RET_get-symbol
    0023   0x0009A0   43 32 02 00   Move                 r50, r2       r50 = RET_new-boxed-value
    0024   0x0009A8   43 33 03 00   Move                 r51, r3       r51 = 0
    0025   0x0009B0   1C 00 00 03   CallFf               r0, r0, 3     r0 = darray-append(RET_get-symbol, RET_new-boxed-value, 0)
    0026   0x0009B8   2D 62 00 00   Branch               0x62          GOTO => L_2
  L_1:
    0027   0x0009C0   15 00 04 00   LookupPointer        r0, 4         r0 = ST[4] -> <get-boolean>
    0028   0x0009C8   4A 01 0B 00   LoadStaticU64Imm     r1, 11        r1 = ST[11] -> <#87B5D4A36BD8ECC3>
    0029   0x0009D0   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <self>
    002A   0x0009D8   43 31 01 00   Move                 r49, r1       r49 = #87B5D4A36BD8ECC3
    002B   0x0009E0   43 32 02 00   Move                 r50, r2       r50 = self
    002C   0x0009E8   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-boolean(symbol: #87B5D4A36BD8ECC3, location: self)
    002D   0x0009F0   2F 44 00 00   BranchIfNot          r0, 0x44      IF NOT r0 => L_3
    002E   0x0009F8   15 00 06 00   LookupPointer        r0, 6         r0 = ST[6] -> <darray-append>
    002F   0x000A00   15 01 01 00   LookupPointer        r1, 1         r1 = ST[1] -> <get-symbol>
    0030   0x000A08   4A 02 07 00   LoadStaticU64Imm     r2, 7         r2 = ST[7] -> <#D2F26D6B19D49A54>
    0031   0x000A10   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <self>
    0032   0x000A18   43 31 02 00   Move                 r49, r2       r49 = #D2F26D6B19D49A54
    0033   0x000A20   43 32 03 00   Move                 r50, r3       r50 = self
    0034   0x000A28   1C 01 01 02   CallFf               r1, r1, 2     r1 = get-symbol(#D2F26D6B19D49A54, self)
    0035   0x000A30   15 02 08 00   LookupPointer        r2, 8         r2 = ST[8] -> <new-boxed-value>
    0036   0x000A38   0C 03 05 00   LoadU16Imm           r3, 5         r3 = 5
    0037   0x000A40   15 04 09 00   LookupPointer        r4, 9         r4 = ST[9] -> <#142211B6C3E9D2D7>
    0038   0x000A48   40 05 0C 00   LoadStaticFloatImm   r5, 12        r5 = ST[12] -> <0.500000>
    0039   0x000A50   43 31 05 00   Move                 r49, r5       r49 = 0.500000
    003A   0x000A58   1B 04 04 01   Call                 r4, r4, 1     r4 = #142211B6C3E9D2D7(0.500000)
    003B   0x000A60   43 31 03 00   Move                 r49, r3       r49 = 5
    003C   0x000A68   43 32 04 00   Move                 r50, r4       r50 = RET_#142211B6C3E9D2D7
    003D   0x000A70   1C 02 02 02   CallFf               r2, r2, 2     r2 = new-boxed-value(5, RET_#142211B6C3E9D2D7)
    003E   0x000A78   0C 03 00 00   LoadU16Imm           r3, 0         r3 = 0
    003F   0x000A80   43 31 01 00   Move                 r49, r1       r49 = RET_get-symbol
    0040   0x000A88   43 32 02 00   Move                 r50, r2       r50 = RET_new-boxed-value
    0041   0x000A90   43 33 03 00   Move                 r51, r3       r51 = 0
    0042   0x000A98   1C 00 00 03   CallFf               r0, r0, 3     r0 = darray-append(RET_get-symbol, RET_new-boxed-value, 0)
    0043   0x000AA0   2D 62 00 00   Branch               0x62          GOTO => L_2
  L_3:
    0044   0x000AA8   15 00 04 00   LookupPointer        r0, 4         r0 = ST[4] -> <get-boolean>
    0045   0x000AB0   4A 01 0D 00   LoadStaticU64Imm     r1, 13        r1 = ST[13] -> <#11C859308AA9F60B>
    0046   0x000AB8   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <self>
    0047   0x000AC0   43 31 01 00   Move                 r49, r1       r49 = #11C859308AA9F60B
    0048   0x000AC8   43 32 02 00   Move                 r50, r2       r50 = self
    0049   0x000AD0   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-boolean(symbol: #11C859308AA9F60B, location: self)
    004A   0x000AD8   2F 61 00 00   BranchIfNot          r0, 0x61      IF NOT r0 => L_4
    004B   0x000AE0   15 00 06 00   LookupPointer        r0, 6         r0 = ST[6] -> <darray-append>
    004C   0x000AE8   15 01 01 00   LookupPointer        r1, 1         r1 = ST[1] -> <get-symbol>
    004D   0x000AF0   4A 02 07 00   LoadStaticU64Imm     r2, 7         r2 = ST[7] -> <#D2F26D6B19D49A54>
    004E   0x000AF8   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <self>
    004F   0x000B00   43 31 02 00   Move                 r49, r2       r49 = #D2F26D6B19D49A54
    0050   0x000B08   43 32 03 00   Move                 r50, r3       r50 = self
    0051   0x000B10   1C 01 01 02   CallFf               r1, r1, 2     r1 = get-symbol(#D2F26D6B19D49A54, self)
    0052   0x000B18   15 02 08 00   LookupPointer        r2, 8         r2 = ST[8] -> <new-boxed-value>
    0053   0x000B20   0C 03 05 00   LoadU16Imm           r3, 5         r3 = 5
    0054   0x000B28   15 04 09 00   LookupPointer        r4, 9         r4 = ST[9] -> <#142211B6C3E9D2D7>
    0055   0x000B30   40 05 0E 00   LoadStaticFloatImm   r5, 14        r5 = ST[14] -> <1.000000>
    0056   0x000B38   43 31 05 00   Move                 r49, r5       r49 = 1.000000
    0057   0x000B40   1B 04 04 01   Call                 r4, r4, 1     r4 = #142211B6C3E9D2D7(1.000000)
    0058   0x000B48   43 31 03 00   Move                 r49, r3       r49 = 5
    0059   0x000B50   43 32 04 00   Move                 r50, r4       r50 = RET_#142211B6C3E9D2D7
    005A   0x000B58   1C 02 02 02   CallFf               r2, r2, 2     r2 = new-boxed-value(5, RET_#142211B6C3E9D2D7)
    005B   0x000B60   0C 03 00 00   LoadU16Imm           r3, 0         r3 = 0
    005C   0x000B68   43 31 01 00   Move                 r49, r1       r49 = RET_get-symbol
    005D   0x000B70   43 32 02 00   Move                 r50, r2       r50 = RET_new-boxed-value
    005E   0x000B78   43 33 03 00   Move                 r51, r3       r51 = 0
    005F   0x000B80   1C 00 00 03   CallFf               r0, r0, 3     r0 = darray-append(RET_get-symbol, RET_new-boxed-value, 0)
    0060   0x000B88   2D 62 00 00   Branch               0x62          GOTO => L_2
  L_4:
    0061   0x000B90   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
  L_2:
    0062   0x000B98   2D 64 00 00   Branch               0x64          GOTO => L_RETURN
  L_RETURN_0:
    0063   0x000BA0   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
  L_RETURN:
    0064   0x000BA8   00 00 00 00   Return               r0            Return 

    SYMBOL TABLE: 
    0000   0x000BB0    function: npc-alive?
    0001   0x000BB8    function: get-symbol
    0002   0x000BC0    sid: npc
    0003   0x000BC8    sid: self
    0004   0x000BD0    function: get-boolean(sid, sid) -> bool
    0005   0x000BD8    sid: #98A1648B00B03E48
    0006   0x000BE0    function: darray-append
    0007   0x000BE8    sid: #D2F26D6B19D49A54
    0008   0x000BF0    function: new-boxed-value
    0009   0x000BF8    function: #142211B6C3E9D2D7
    000A   0x000C00    float: 0.250000
    000B   0x000C08    sid: #87B5D4A36BD8ECC3
    000C   0x000C10    float: 0.500000
    000D   0x000C18    sid: #11C859308AA9F60B
    000E   0x000C20    float: 1.000000
}

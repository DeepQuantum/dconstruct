####################################################################################################
#                                                                                                  #
#   DeepQuantum's DC Disassembler ver. beta_3                                                      #
#   Listing for file: ss-vox-san-streets-resort-pool.bin                                           #
#   Filesize: 17786 bytes                                                                          #
#                                                                                                  #
####################################################################################################



##############################  ENTRY 0  ##############################

#029902C61617476E = d-array [0x000C0] {
  [0] array* [0x00570] {
    [0] anonymous array [0x570] {size: 3} {
      [0] anonymous struct [0x6d8] {
        [0] boxed-symbol [0x00BA8] {
          [0] sid: npc-chained-infected-8
          [1] int: 0
          [2] int: 7
        }
      }
      [1] anonymous struct [0x6dd] {
        [0] boxed-symbol [0x00BA8] {
          [0] sid: npc-chained-infected-8
          [1] int: 0
          [2] int: 7
        }
      }
      [2] anonymous struct [0x6e2] {
        [0] boxed-symbol [0x007E8] {
          [0] sid: npc-chained-infected-10
          [1] int: 0
          [2] int: 7
        }
      }
    }
    [1] int: 3
    [2] int: -1
  }
  [1] int: 0
  [2] int: 0
}


##############################  ENTRY 1  ##############################

ss-vox-san-streets-resort-pool = state-script [0x000D8] {
  OPTIONS:     player
  DECLARATIONS: 
    [0x000FD8] boolean  #4B8BA068B7E3B15D    = uninitialized
    [0x001008] boolean  #24DE0D413BE844A3    = uninitialized
    [0x001038] boolean  #31801F738FFE8116    = uninitialized
    [0x001068] boolean  #F0DB741BDB6AC2EE    = uninitialized
    [0x001098] boolean  #0E3C81AFAF2E1018    = uninitialized
    [0x0010C8] boolean  #F83BCD15E0984FA1    = uninitialized
    [0x0010F8] boolean  #754B6F0089A1D514    = uninitialized
    [0x001128] boolean  #1E28BB7445A5BD9E    = uninitialized
    [0x001158] boolean  #ED34F6AD0C9FB9F4    = uninitialized
    [0x001188] boolean  #99310993AD6FCB2A    = uninitialized
    [0x0011B8] boolean  #28E19830CB07A9EF    = uninitialized
    STATE task-check {
      ON start {
        TRACK main {
          {
            0000   0x003DB0   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <task-active?>
            0001   0x003DB8   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <santa-barbara>
            0002   0x003DC0   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <san-streets-resort-pool>
            0003   0x003DC8   43 31 01 00   Move                 r49, r1       r49 = santa-barbara
            0004   0x003DD0   43 32 02 00   Move                 r50, r2       r50 = san-streets-resort-pool
            0005   0x003DD8   1B 00 00 02   Call                 r0, r0, 2     r0 = task-active?(santa-barbara, san-streets-resort-pool)
            0006   0x003DE0   2F 1A 00 00   BranchIfNot          r0, 0x1A      IF NOT r0 => L_RETURN_0
            0007   0x003DE8   15 00 03 00   LookupPointer        r0, 3         r0 = ST[3] -> <interactable-disable>
            0008   0x003DF0   4A 01 04 00   LoadStaticU64Imm     r1, 4         r1 = ST[4] -> <look-reinforce-round-building-1>
            0009   0x003DF8   43 31 01 00   Move                 r49, r1       r49 = look-reinforce-round-building-1
            000A   0x003E00   1C 00 00 01   CallFf               r0, r0, 1     r0 = interactable-disable(look-reinforce-round-building-1)
            000B   0x003E08   15 00 05 00   LookupPointer        r0, 5         r0 = ST[5] -> <set-boolean>
            000C   0x003E10   4A 01 06 00   LoadStaticU64Imm     r1, 6         r1 = ST[6] -> <#754B6F0089A1D514>
            000D   0x003E18   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            000E   0x003E20   4A 03 07 00   LoadStaticU64Imm     r3, 7         r3 = ST[7] -> <self>
            000F   0x003E28   43 31 01 00   Move                 r49, r1       r49 = #754B6F0089A1D514
            0010   0x003E30   43 32 02 00   Move                 r50, r2       r50 = 1
            0011   0x003E38   43 33 03 00   Move                 r51, r3       r51 = self
            0012   0x003E40   1C 00 00 03   CallFf               r0, r0, 3     r0 = set-boolean(#754B6F0089A1D514, 1, self)
            0013   0x003E48   15 00 08 00   LookupPointer        r0, 8         r0 = ST[8] -> <go>
            0014   0x003E50   4A 01 09 00   LoadStaticU64Imm     r1, 9         r1 = ST[9] -> <wait-for-event>
            0015   0x003E58   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0016   0x003E60   43 31 01 00   Move                 r49, r1       r49 = wait-for-event
            0017   0x003E68   43 32 02 00   Move                 r50, r2       r50 = 1
            0018   0x003E70   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(wait-for-event, 1)
            0019   0x003E78   2D 1B 00 00   Branch               0x1B          GOTO => L_RETURN
          L_RETURN_0:
            001A   0x003E80   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
          L_RETURN:
            001B   0x003E88   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003E90    function: task-active?
            0001   0x003E98    sid: santa-barbara
            0002   0x003EA0    sid: san-streets-resort-pool
            0003   0x003EA8    function: interactable-disable
            0004   0x003EB0    sid: look-reinforce-round-building-1
            0005   0x003EB8    function: set-boolean
            0006   0x003EC0    sid: #754B6F0089A1D514
            0007   0x003EC8    sid: self
            0008   0x003ED0    function: go
            0009   0x003ED8    sid: wait-for-event
          }
          {
            0000   0x002BE0   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <task-complete?>
            0001   0x002BE8   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <santa-barbara>
            0002   0x002BF0   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <san-streets-resort-pool>
            0003   0x002BF8   43 31 01 00   Move                 r49, r1       r49 = santa-barbara
            0004   0x002C00   43 32 02 00   Move                 r50, r2       r50 = san-streets-resort-pool
            0005   0x002C08   1B 00 00 02   Call                 r0, r0, 2     r0 = task-complete?(santa-barbara, san-streets-resort-pool)
            0006   0x002C10   2F 0E 00 00   BranchIfNot          r0, 0xE       IF NOT r0 => L_RETURN_0
            0007   0x002C18   15 00 03 00   LookupPointer        r0, 3         r0 = ST[3] -> <go>
            0008   0x002C20   4A 01 04 00   LoadStaticU64Imm     r1, 4         r1 = ST[4] -> <done>
            0009   0x002C28   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            000A   0x002C30   43 31 01 00   Move                 r49, r1       r49 = done
            000B   0x002C38   43 32 02 00   Move                 r50, r2       r50 = 1
            000C   0x002C40   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(done, 1)
            000D   0x002C48   2D 0F 00 00   Branch               0xF           GOTO => L_RETURN
          L_RETURN_0:
            000E   0x002C50   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
          L_RETURN:
            000F   0x002C58   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x002C60    function: task-complete?
            0001   0x002C68    sid: santa-barbara
            0002   0x002C70    sid: san-streets-resort-pool
            0003   0x002C78    function: go
            0004   0x002C80    sid: done
          }
        }

      }
      ON event_task-complete {
        TRACK main {
          {
            0000   0x002B38   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <task-active?>
            0001   0x002B40   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <santa-barbara>
            0002   0x002B48   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <san-streets-resort-pool>
            0003   0x002B50   43 31 01 00   Move                 r49, r1       r49 = santa-barbara
            0004   0x002B58   43 32 02 00   Move                 r50, r2       r50 = san-streets-resort-pool
            0005   0x002B60   1B 00 00 02   Call                 r0, r0, 2     r0 = task-active?(santa-barbara, san-streets-resort-pool)
            0006   0x002B68   2F 0E 00 00   BranchIfNot          r0, 0xE       IF NOT r0 => L_RETURN_0
            0007   0x002B70   15 00 03 00   LookupPointer        r0, 3         r0 = ST[3] -> <go>
            0008   0x002B78   4A 01 04 00   LoadStaticU64Imm     r1, 4         r1 = ST[4] -> <wait-for-event>
            0009   0x002B80   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            000A   0x002B88   43 31 01 00   Move                 r49, r1       r49 = wait-for-event
            000B   0x002B90   43 32 02 00   Move                 r50, r2       r50 = 1
            000C   0x002B98   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(wait-for-event, 1)
            000D   0x002BA0   2D 0F 00 00   Branch               0xF           GOTO => L_RETURN
          L_RETURN_0:
            000E   0x002BA8   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
          L_RETURN:
            000F   0x002BB0   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x002BB8    function: task-active?
            0001   0x002BC0    sid: santa-barbara
            0002   0x002BC8    sid: san-streets-resort-pool
            0003   0x002BD0    function: go
            0004   0x002BD8    sid: wait-for-event
          }
        }

      }
    } END STATE task-check

    STATE wait-for-event {
      ON event_task-complete {
        TRACK main {
          {
            0000   0x002BE0   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <task-complete?>
            0001   0x002BE8   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <santa-barbara>
            0002   0x002BF0   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <san-streets-resort-pool>
            0003   0x002BF8   43 31 01 00   Move                 r49, r1       r49 = santa-barbara
            0004   0x002C00   43 32 02 00   Move                 r50, r2       r50 = san-streets-resort-pool
            0005   0x002C08   1B 00 00 02   Call                 r0, r0, 2     r0 = task-complete?(santa-barbara, san-streets-resort-pool)
            0006   0x002C10   2F 0E 00 00   BranchIfNot          r0, 0xE       IF NOT r0 => L_RETURN_0
            0007   0x002C18   15 00 03 00   LookupPointer        r0, 3         r0 = ST[3] -> <go>
            0008   0x002C20   4A 01 04 00   LoadStaticU64Imm     r1, 4         r1 = ST[4] -> <done>
            0009   0x002C28   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            000A   0x002C30   43 31 01 00   Move                 r49, r1       r49 = done
            000B   0x002C38   43 32 02 00   Move                 r50, r2       r50 = 1
            000C   0x002C40   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(done, 1)
            000D   0x002C48   2D 0F 00 00   Branch               0xF           GOTO => L_RETURN
          L_RETURN_0:
            000E   0x002C50   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
          L_RETURN:
            000F   0x002C58   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x002C60    function: task-complete?
            0001   0x002C68    sid: santa-barbara
            0002   0x002C70    sid: san-streets-resort-pool
            0003   0x002C78    function: go
            0004   0x002C80    sid: done
          }
        }

      }
      ON event_npc-died {
        TRACK main {
          {
            0000   0x002C88   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <get-boolean>
            0001   0x002C90   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <#F83BCD15E0984FA1>
            0002   0x002C98   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <self>
            0003   0x002CA0   43 31 01 00   Move                 r49, r1       r49 = #F83BCD15E0984FA1
            0004   0x002CA8   43 32 02 00   Move                 r50, r2       r50 = self
            0005   0x002CB0   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-boolean(symbol: #F83BCD15E0984FA1, location: self)
            0006   0x002CB8   30 00 00 00   OpLogNot             r0, r0        r0 = !54185015286451590
            0007   0x002CC0   2F 30 00 00   BranchIfNot          r0, 0x30      IF NOT r0 => L_RETURN_0
            0008   0x002CC8   15 00 03 00   LookupPointer        r0, 3         r0 = ST[3] -> <no-npcs-in-group-alive?>
            0009   0x002CD0   4A 01 04 00   LoadStaticU64Imm     r1, 4         r1 = ST[4] -> <#26AD4BAABF5B1899>
            000A   0x002CD8   43 31 01 00   Move                 r49, r1       r49 = #26AD4BAABF5B1899
            000B   0x002CE0   1B 00 00 01   Call                 r0, r0, 1     r0 = no-npcs-in-group-alive?(#26AD4BAABF5B1899)
            000C   0x002CE8   2F 0F 00 00   BranchIfNot          r0, 0x2E      IF NOT r0 => L_1
            000D   0x002CF0   15 00 05 00   LookupPointer        r0, 5         r0 = ST[5] -> <#502C6E38E46C6A50>
            000E   0x002CF8   1B 00 00 00   Call                 r0, r0, 0     r0 = #502C6E38E46C6A50()
            000F   0x002D00   2F 1E 00 00   BranchIfNot          r0, 0x2E      IF NOT r0 => L_1
            0010   0x002D08   15 00 06 00   LookupPointer        r0, 6         r0 = ST[6] -> <darray-contains?>
            0011   0x002D10   4A 01 04 00   LoadStaticU64Imm     r1, 4         r1 = ST[4] -> <#26AD4BAABF5B1899>
            0012   0x002D18   15 02 07 00   LookupPointer        r2, 7         r2 = ST[7] -> <new-boxed-value>
            0013   0x002D20   0C 03 07 00   LoadU16Imm           r3, 7         r3 = 7
            0014   0x002D28   15 04 08 00   LookupPointer        r4, 8         r4 = ST[8] -> <event-arg-symbol>
            0015   0x002D30   0C 05 00 00   LoadU16Imm           r5, 0         r5 = 0
            0016   0x002D38   43 31 05 00   Move                 r49, r5       r49 = 0
            0017   0x002D40   1C 04 04 01   CallFf               r4, r4, 1     r4 = event-arg-symbol(0)
            0018   0x002D48   43 31 03 00   Move                 r49, r3       r49 = 7
            0019   0x002D50   43 32 04 00   Move                 r50, r4       r50 = RET_event-arg-symbol
            001A   0x002D58   1C 02 02 02   CallFf               r2, r2, 2     r2 = new-boxed-value(7, RET_event-arg-symbol)
            001B   0x002D60   43 31 01 00   Move                 r49, r1       r49 = #26AD4BAABF5B1899
            001C   0x002D68   43 32 02 00   Move                 r50, r2       r50 = RET_new-boxed-value
            001D   0x002D70   1C 00 00 02   CallFf               r0, r0, 2     r0 = darray-contains?(#26AD4BAABF5B1899, RET_new-boxed-value)
            001E   0x002D78   2F 2E 00 00   BranchIfNot          r0, 0x2E      IF NOT r0 => L_1
            001F   0x002D80   15 00 09 00   LookupPointer        r0, 9         r0 = ST[9] -> <set-boolean>
            0020   0x002D88   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <#F83BCD15E0984FA1>
            0021   0x002D90   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0022   0x002D98   4A 03 02 00   LoadStaticU64Imm     r3, 2         r3 = ST[2] -> <self>
            0023   0x002DA0   43 31 01 00   Move                 r49, r1       r49 = #F83BCD15E0984FA1
            0024   0x002DA8   43 32 02 00   Move                 r50, r2       r50 = 1
            0025   0x002DB0   43 33 03 00   Move                 r51, r3       r51 = self
            0026   0x002DB8   1C 00 00 03   CallFf               r0, r0, 3     r0 = set-boolean(#F83BCD15E0984FA1, 1, self)
            0027   0x002DC0   15 00 0A 00   LookupPointer        r0, 10        r0 = ST[10] -> <go>
            0028   0x002DC8   4A 01 0B 00   LoadStaticU64Imm     r1, 11        r1 = ST[11] -> <pool-combat-done>
            0029   0x002DD0   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            002A   0x002DD8   43 31 01 00   Move                 r49, r1       r49 = pool-combat-done
            002B   0x002DE0   43 32 02 00   Move                 r50, r2       r50 = 1
            002C   0x002DE8   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(pool-combat-done, 1)
            002D   0x002DF0   2D 2F 00 00   Branch               0x2F          GOTO => L_2
          L_1:
            002E   0x002DF8   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
          L_2:
            002F   0x002E00   2D 31 00 00   Branch               0x31          GOTO => L_RETURN
          L_RETURN_0:
            0030   0x002E08   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
          L_RETURN:
            0031   0x002E10   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x002E18    function: get-boolean(sid, sid) -> bool
            0001   0x002E20    sid: #F83BCD15E0984FA1
            0002   0x002E28    sid: self
            0003   0x002E30    function: no-npcs-in-group-alive?
            0004   0x002E38    sid: #26AD4BAABF5B1899
            0005   0x002E40    function: #502C6E38E46C6A50
            0006   0x002E48    function: darray-contains?
            0007   0x002E50    function: new-boxed-value
            0008   0x002E58    function: event-arg-symbol
            0009   0x002E60    function: set-boolean
            000A   0x002E68    function: go
            000B   0x002E70    sid: pool-combat-done
          }
          {
            0000   0x002948   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <get-boolean>
            0001   0x002950   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <#1E28BB7445A5BD9E>
            0002   0x002958   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <self>
            0003   0x002960   43 31 01 00   Move                 r49, r1       r49 = #1E28BB7445A5BD9E
            0004   0x002968   43 32 02 00   Move                 r50, r2       r50 = self
            0005   0x002970   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-boolean(symbol: #1E28BB7445A5BD9E, location: self)
            0006   0x002978   30 00 00 00   OpLogNot             r0, r0        r0 = !54185015286451590
            0007   0x002980   2F 30 00 00   BranchIfNot          r0, 0x30      IF NOT r0 => L_RETURN_0
            0008   0x002988   15 00 03 00   LookupPointer        r0, 3         r0 = ST[3] -> <no-npcs-in-group-alive?>
            0009   0x002990   4A 01 04 00   LoadStaticU64Imm     r1, 4         r1 = ST[4] -> <#26AD48AABF5B1380>
            000A   0x002998   43 31 01 00   Move                 r49, r1       r49 = #26AD48AABF5B1380
            000B   0x0029A0   1B 00 00 01   Call                 r0, r0, 1     r0 = no-npcs-in-group-alive?(#26AD48AABF5B1380)
            000C   0x0029A8   2F 0F 00 00   BranchIfNot          r0, 0x2E      IF NOT r0 => L_1
            000D   0x0029B0   15 00 05 00   LookupPointer        r0, 5         r0 = ST[5] -> <#502C6E38E46C6A50>
            000E   0x0029B8   1B 00 00 00   Call                 r0, r0, 0     r0 = #502C6E38E46C6A50()
            000F   0x0029C0   2F 1E 00 00   BranchIfNot          r0, 0x2E      IF NOT r0 => L_1
            0010   0x0029C8   15 00 06 00   LookupPointer        r0, 6         r0 = ST[6] -> <darray-contains?>
            0011   0x0029D0   4A 01 04 00   LoadStaticU64Imm     r1, 4         r1 = ST[4] -> <#26AD48AABF5B1380>
            0012   0x0029D8   15 02 07 00   LookupPointer        r2, 7         r2 = ST[7] -> <new-boxed-value>
            0013   0x0029E0   0C 03 07 00   LoadU16Imm           r3, 7         r3 = 7
            0014   0x0029E8   15 04 08 00   LookupPointer        r4, 8         r4 = ST[8] -> <event-arg-symbol>
            0015   0x0029F0   0C 05 00 00   LoadU16Imm           r5, 0         r5 = 0
            0016   0x0029F8   43 31 05 00   Move                 r49, r5       r49 = 0
            0017   0x002A00   1C 04 04 01   CallFf               r4, r4, 1     r4 = event-arg-symbol(0)
            0018   0x002A08   43 31 03 00   Move                 r49, r3       r49 = 7
            0019   0x002A10   43 32 04 00   Move                 r50, r4       r50 = RET_event-arg-symbol
            001A   0x002A18   1C 02 02 02   CallFf               r2, r2, 2     r2 = new-boxed-value(7, RET_event-arg-symbol)
            001B   0x002A20   43 31 01 00   Move                 r49, r1       r49 = #26AD48AABF5B1380
            001C   0x002A28   43 32 02 00   Move                 r50, r2       r50 = RET_new-boxed-value
            001D   0x002A30   1C 00 00 02   CallFf               r0, r0, 2     r0 = darray-contains?(#26AD48AABF5B1380, RET_new-boxed-value)
            001E   0x002A38   2F 2E 00 00   BranchIfNot          r0, 0x2E      IF NOT r0 => L_1
            001F   0x002A40   15 00 09 00   LookupPointer        r0, 9         r0 = ST[9] -> <set-boolean>
            0020   0x002A48   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <#1E28BB7445A5BD9E>
            0021   0x002A50   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0022   0x002A58   4A 03 02 00   LoadStaticU64Imm     r3, 2         r3 = ST[2] -> <self>
            0023   0x002A60   43 31 01 00   Move                 r49, r1       r49 = #1E28BB7445A5BD9E
            0024   0x002A68   43 32 02 00   Move                 r50, r2       r50 = 1
            0025   0x002A70   43 33 03 00   Move                 r51, r3       r51 = self
            0026   0x002A78   1C 00 00 03   CallFf               r0, r0, 3     r0 = set-boolean(#1E28BB7445A5BD9E, 1, self)
            0027   0x002A80   15 00 0A 00   LookupPointer        r0, 10        r0 = ST[10] -> <go>
            0028   0x002A88   4A 01 0B 00   LoadStaticU64Imm     r1, 11        r1 = ST[11] -> <landing-combat-done>
            0029   0x002A90   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            002A   0x002A98   43 31 01 00   Move                 r49, r1       r49 = landing-combat-done
            002B   0x002AA0   43 32 02 00   Move                 r50, r2       r50 = 1
            002C   0x002AA8   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(landing-combat-done, 1)
            002D   0x002AB0   2D 2F 00 00   Branch               0x2F          GOTO => L_2
          L_1:
            002E   0x002AB8   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
          L_2:
            002F   0x002AC0   2D 31 00 00   Branch               0x31          GOTO => L_RETURN
          L_RETURN_0:
            0030   0x002AC8   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
          L_RETURN:
            0031   0x002AD0   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x002AD8    function: get-boolean(sid, sid) -> bool
            0001   0x002AE0    sid: #1E28BB7445A5BD9E
            0002   0x002AE8    sid: self
            0003   0x002AF0    function: no-npcs-in-group-alive?
            0004   0x002AF8    sid: #26AD48AABF5B1380
            0005   0x002B00    function: #502C6E38E46C6A50
            0006   0x002B08    function: darray-contains?
            0007   0x002B10    function: new-boxed-value
            0008   0x002B18    function: event-arg-symbol
            0009   0x002B20    function: set-boolean
            000A   0x002B28    function: go
            000B   0x002B30    sid: landing-combat-done
          }
        }

      }
      ON event_player-requested-interact {
        TRACK main {
          {
            0000   0x003C08   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <interactable-disable>
            0001   0x003C10   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <look-reinforce-round-building-1>
            0002   0x003C18   43 31 01 00   Move                 r49, r1       r49 = look-reinforce-round-building-1
            0003   0x003C20   1C 00 00 01   CallFf               r0, r0, 1     r0 = interactable-disable(look-reinforce-round-building-1)
            0004   0x003C28   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003C30    function: interactable-disable
            0001   0x003C38    sid: look-reinforce-round-building-1
          }
          {
            0000   0x0026D0   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <go>
            0001   0x0026D8   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <see-building>
            0002   0x0026E0   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0003   0x0026E8   43 31 01 00   Move                 r49, r1       r49 = see-building
            0004   0x0026F0   43 32 02 00   Move                 r50, r2       r50 = 1
            0005   0x0026F8   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(see-building, 1)
            0006   0x002700   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x002708    function: go
            0001   0x002710    sid: see-building
          }
        }

      }
      ON event_player-draw-weapon-done {
        TRACK main {
          {
            0000   0x0031C0   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <get-boolean>
            0001   0x0031C8   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <#754B6F0089A1D514>
            0002   0x0031D0   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <self>
            0003   0x0031D8   43 31 01 00   Move                 r49, r1       r49 = #754B6F0089A1D514
            0004   0x0031E0   43 32 02 00   Move                 r50, r2       r50 = self
            0005   0x0031E8   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-boolean(symbol: #754B6F0089A1D514, location: self)
            0006   0x0031F0   30 00 00 00   OpLogNot             r0, r0        r0 = !54185015286451590
            0007   0x0031F8   2F 17 00 00   BranchIfNot          r0, 0x17      IF NOT r0 => L_RETURN_0
            0008   0x003200   15 00 03 00   LookupPointer        r0, 3         r0 = ST[3] -> <set-boolean>
            0009   0x003208   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <#754B6F0089A1D514>
            000A   0x003210   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            000B   0x003218   4A 03 02 00   LoadStaticU64Imm     r3, 2         r3 = ST[2] -> <self>
            000C   0x003220   43 31 01 00   Move                 r49, r1       r49 = #754B6F0089A1D514
            000D   0x003228   43 32 02 00   Move                 r50, r2       r50 = 1
            000E   0x003230   43 33 03 00   Move                 r51, r3       r51 = self
            000F   0x003238   1C 00 00 03   CallFf               r0, r0, 3     r0 = set-boolean(#754B6F0089A1D514, 1, self)
            0010   0x003240   15 00 04 00   LookupPointer        r0, 4         r0 = ST[4] -> <go>
            0011   0x003248   4A 01 05 00   LoadStaticU64Imm     r1, 5         r1 = ST[5] -> <see-pool>
            0012   0x003250   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0013   0x003258   43 31 01 00   Move                 r49, r1       r49 = see-pool
            0014   0x003260   43 32 02 00   Move                 r50, r2       r50 = 1
            0015   0x003268   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(see-pool, 1)
            0016   0x003270   2D 18 00 00   Branch               0x18          GOTO => L_RETURN
          L_RETURN_0:
            0017   0x003278   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
          L_RETURN:
            0018   0x003280   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003288    function: get-boolean(sid, sid) -> bool
            0001   0x003290    sid: #754B6F0089A1D514
            0002   0x003298    sid: self
            0003   0x0032A0    function: set-boolean
            0004   0x0032A8    function: go
            0005   0x0032B0    sid: see-pool
          }
        }

      }
      ON event_vox-san-pool-search-start {
        TRACK main {
          {
            0000   0x003A18   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <!>
            0001   0x003A20   15 01 01 00   LookupPointer        r1, 1         r1 = ST[1] -> <get-boolean>
            0002   0x003A28   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <#28E19830CB07A9EF>
            0003   0x003A30   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <self>
            0004   0x003A38   43 31 02 00   Move                 r49, r2       r49 = #28E19830CB07A9EF
            0005   0x003A40   43 32 03 00   Move                 r50, r3       r50 = self
            0006   0x003A48   1C 01 01 02   CallFf               r1, r1, 2     r1 = get-boolean(symbol: #28E19830CB07A9EF, location: self)
            0007   0x003A50   43 31 01 00   Move                 r49, r1       r49 = 54185015286451590
            0008   0x003A58   1B 00 00 01   Call                 r0, r0, 1     r0 = !(54185015286451590)
            0009   0x003A60   2F 10 00 00   BranchIfNot          r0, 0x23      IF NOT r0 => L_RETURN_0
            000A   0x003A68   15 00 01 00   LookupPointer        r0, 1         r0 = ST[1] -> <get-boolean>
            000B   0x003A70   4A 01 04 00   LoadStaticU64Imm     r1, 4         r1 = ST[4] -> <#99310993AD6FCB2A>
            000C   0x003A78   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <self>
            000D   0x003A80   43 31 01 00   Move                 r49, r1       r49 = #99310993AD6FCB2A
            000E   0x003A88   43 32 02 00   Move                 r50, r2       r50 = self
            000F   0x003A90   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-boolean(symbol: #99310993AD6FCB2A, location: self)
            0010   0x003A98   2F 13 00 00   BranchIfNot          r0, 0x23      IF NOT r0 => L_RETURN_0
            0011   0x003AA0   15 00 05 00   LookupPointer        r0, 5         r0 = ST[5] -> <#502C6E38E46C6A50>
            0012   0x003AA8   1B 00 00 00   Call                 r0, r0, 0     r0 = #502C6E38E46C6A50()
            0013   0x003AB0   2F 23 00 00   BranchIfNot          r0, 0x23      IF NOT r0 => L_RETURN_0
            0014   0x003AB8   15 00 06 00   LookupPointer        r0, 6         r0 = ST[6] -> <set-boolean>
            0015   0x003AC0   4A 01 02 00   LoadStaticU64Imm     r1, 2         r1 = ST[2] -> <#28E19830CB07A9EF>
            0016   0x003AC8   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0017   0x003AD0   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <self>
            0018   0x003AD8   43 31 01 00   Move                 r49, r1       r49 = #28E19830CB07A9EF
            0019   0x003AE0   43 32 02 00   Move                 r50, r2       r50 = 1
            001A   0x003AE8   43 33 03 00   Move                 r51, r3       r51 = self
            001B   0x003AF0   1C 00 00 03   CallFf               r0, r0, 3     r0 = set-boolean(#28E19830CB07A9EF, 1, self)
            001C   0x003AF8   15 00 07 00   LookupPointer        r0, 7         r0 = ST[7] -> <go>
            001D   0x003B00   4A 01 08 00   LoadStaticU64Imm     r1, 8         r1 = ST[8] -> <first-search-start>
            001E   0x003B08   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            001F   0x003B10   43 31 01 00   Move                 r49, r1       r49 = first-search-start
            0020   0x003B18   43 32 02 00   Move                 r50, r2       r50 = 1
            0021   0x003B20   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(first-search-start, 1)
            0022   0x003B28   2D 24 00 00   Branch               0x24          GOTO => L_RETURN
          L_RETURN_0:
            0023   0x003B30   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
          L_RETURN:
            0024   0x003B38   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003B40    function: !
            0001   0x003B48    function: get-boolean(sid, sid) -> bool
            0002   0x003B50    sid: #28E19830CB07A9EF
            0003   0x003B58    sid: self
            0004   0x003B60    sid: #99310993AD6FCB2A
            0005   0x003B68    function: #502C6E38E46C6A50
            0006   0x003B70    function: set-boolean
            0007   0x003B78    function: go
            0008   0x003B80    sid: first-search-start
          }
        }

      }
      ON event_#A28A54B7288437D2 {
        TRACK main {
          {
            0000   0x0036B8   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <get-boolean>
            0001   0x0036C0   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <#28E19830CB07A9EF>
            0002   0x0036C8   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <self>
            0003   0x0036D0   43 31 01 00   Move                 r49, r1       r49 = #28E19830CB07A9EF
            0004   0x0036D8   43 32 02 00   Move                 r50, r2       r50 = self
            0005   0x0036E0   1C 00 00 02   CallFf               r0, r0, 2     r0 = get-boolean(symbol: #28E19830CB07A9EF, location: self)
            0006   0x0036E8   2F 18 00 00   BranchIfNot          r0, 0x18      IF NOT r0 => L_0
            0007   0x0036F0   15 00 03 00   LookupPointer        r0, 3         r0 = ST[3] -> <request-dialog>
            0008   0x0036F8   4A 01 04 00   LoadStaticU64Imm     r1, 4         r1 = ST[4] -> <cya-san-resort-pool-landing-001>
            0009   0x003700   4A 02 05 00   LoadStaticU64Imm     r2, 5         r2 = ST[5] -> <npc-san-pool-landing-2>
            000A   0x003708   4A 03 06 00   LoadStaticU64Imm     r3, 6         r3 = ST[6] -> <npc-san-pool-landing-5>
            000B   0x003710   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
            000C   0x003718   0C 05 00 00   LoadU16Imm           r5, 0         r5 = 0
            000D   0x003720   0C 06 00 00   LoadU16Imm           r6, 0         r6 = 0
            000E   0x003728   0C 07 01 00   LoadU16Imm           r7, 1         r7 = 1
            000F   0x003730   43 31 01 00   Move                 r49, r1       r49 = cya-san-resort-pool-landing-001
            0010   0x003738   43 32 02 00   Move                 r50, r2       r50 = npc-san-pool-landing-2
            0011   0x003740   43 33 03 00   Move                 r51, r3       r51 = npc-san-pool-landing-5
            0012   0x003748   43 34 04 00   Move                 r52, r4       r52 = 0
            0013   0x003750   43 35 05 00   Move                 r53, r5       r53 = 0
            0014   0x003758   43 36 06 00   Move                 r54, r6       r54 = 0
            0015   0x003760   43 37 07 00   Move                 r55, r7       r55 = 1
            0016   0x003768   1C 00 00 07   CallFf               r0, r0, 7     r0 = request-dialog(cya-san-resort-pool-landing-001, npc-san-pool-landing-2, npc-san-pool-landing-5, 0, 0, 0, 1)
            0017   0x003770   2D 28 00 00   Branch               0x28          GOTO => L_RETURN
          L_0:
            0018   0x003778   15 00 03 00   LookupPointer        r0, 3         r0 = ST[3] -> <request-dialog>
            0019   0x003780   4A 01 07 00   LoadStaticU64Imm     r1, 7         r1 = ST[7] -> <san-streets-resort-reinforcements-stealth-001>
            001A   0x003788   4A 02 08 00   LoadStaticU64Imm     r2, 8         r2 = ST[8] -> <npc-san-pool-landing-3>
            001B   0x003790   4A 03 06 00   LoadStaticU64Imm     r3, 6         r3 = ST[6] -> <npc-san-pool-landing-5>
            001C   0x003798   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
            001D   0x0037A0   0C 05 00 00   LoadU16Imm           r5, 0         r5 = 0
            001E   0x0037A8   0C 06 00 00   LoadU16Imm           r6, 0         r6 = 0
            001F   0x0037B0   0C 07 00 00   LoadU16Imm           r7, 0         r7 = 0
            0020   0x0037B8   43 31 01 00   Move                 r49, r1       r49 = san-streets-resort-reinforcements-stealth-001
            0021   0x0037C0   43 32 02 00   Move                 r50, r2       r50 = npc-san-pool-landing-3
            0022   0x0037C8   43 33 03 00   Move                 r51, r3       r51 = npc-san-pool-landing-5
            0023   0x0037D0   43 34 04 00   Move                 r52, r4       r52 = 0
            0024   0x0037D8   43 35 05 00   Move                 r53, r5       r53 = 0
            0025   0x0037E0   43 36 06 00   Move                 r54, r6       r54 = 0
            0026   0x0037E8   43 37 07 00   Move                 r55, r7       r55 = 0
            0027   0x0037F0   1C 00 00 07   CallFf               r0, r0, 7     r0 = request-dialog(san-streets-resort-reinforcements-stealth-001, npc-san-pool-landing-3, npc-san-pool-landing-5, 0, 0, 0, 0)
          L_RETURN:
            0028   0x0037F8   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003800    function: get-boolean(sid, sid) -> bool
            0001   0x003808    sid: #28E19830CB07A9EF
            0002   0x003810    sid: self
            0003   0x003818    function: request-dialog
            0004   0x003820    sid: cya-san-resort-pool-landing-001
            0005   0x003828    sid: npc-san-pool-landing-2
            0006   0x003830    sid: npc-san-pool-landing-5
            0007   0x003838    sid: san-streets-resort-reinforcements-stealth-001
            0008   0x003840    sid: npc-san-pool-landing-3
          }
        }

      }
    } END STATE wait-for-event

    STATE see-building {
      ON start {
        TRACK vox {
          {
            0000   0x003B88   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <wait-until-camera-is-looking-at>
            0001   0x003B90   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <ic-vox-san-streets-see-resort-2>
            0002   0x003B98   40 02 02 00   LoadStaticFloatImm   r2, 2         r2 = ST[2] -> <0.100000>
            0003   0x003BA0   40 03 03 00   LoadStaticFloatImm   r3, 3         r3 = ST[3] -> <400.000000>
            0004   0x003BA8   40 04 04 00   LoadStaticFloatImm   r4, 4         r4 = ST[4] -> <0.000000>
            0005   0x003BB0   43 31 01 00   Move                 r49, r1       r49 = ic-vox-san-streets-see-resort-2
            0006   0x003BB8   43 32 02 00   Move                 r50, r2       r50 = 0.100000
            0007   0x003BC0   43 33 03 00   Move                 r51, r3       r51 = 400.000000
            0008   0x003BC8   43 34 04 00   Move                 r52, r4       r52 = 0.000000
            0009   0x003BD0   1B 00 00 04   Call                 r0, r0, 4     r0 = wait-until-camera-is-looking-at(ic-vox-san-streets-see-resort-2, 0.100000, 400.000000, 0.000000)
            000A   0x003BD8   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003BE0    function: wait-until-camera-is-looking-at
            0001   0x003BE8    sid: ic-vox-san-streets-see-resort-2
            0002   0x003BF0    float: 0.100000
            0003   0x003BF8    float: 400.000000
            0004   0x003C00    float: 0.000000
          }
          {
            0000   0x002788   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <wait>
            0001   0x002790   40 01 01 00   LoadStaticFloatImm   r1, 1         r1 = ST[1] -> <0.500000>
            0002   0x002798   43 31 01 00   Move                 r49, r1       r49 = 0.500000
            0003   0x0027A0   1C 00 00 01   CallFf               r0, r0, 1     r0 = wait(0.500000)
            0004   0x0027A8   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x0027B0    function: wait
            0001   0x0027B8    float: 0.500000
          }
          {
            0000   0x003D68   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <signal>
            0001   0x003D70   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <vox>
            0002   0x003D78   0C 02 00 00   LoadU16Imm           r2, 0         r2 = 0
            0003   0x003D80   43 31 01 00   Move                 r49, r1       r49 = vox
            0004   0x003D88   43 32 02 00   Move                 r50, r2       r50 = 0
            0005   0x003D90   1C 00 00 02   CallFf               r0, r0, 2     r0 = signal(vox, 0)
            0006   0x003D98   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003DA0    function: signal
            0001   0x003DA8    sid: vox
          }
          {
            0000   0x003558   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <wait-say>
            0001   0x003560   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <player>
            0002   0x003568   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <vox-igc-ell-gam-san-streets-resort-enter-slf-009>
            0003   0x003570   0C 03 02 00   LoadU16Imm           r3, 2         r3 = 2
            0004   0x003578   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
            0005   0x003580   43 31 01 00   Move                 r49, r1       r49 = player
            0006   0x003588   43 32 02 00   Move                 r50, r2       r50 = vox-igc-ell-gam-san-streets-resort-enter-slf-009
            0007   0x003590   43 33 03 00   Move                 r51, r3       r51 = 2
            0008   0x003598   43 34 04 00   Move                 r52, r4       r52 = 0
            0009   0x0035A0   1C 00 00 04   CallFf               r0, r0, 4     r0 = wait-say(player, vox-igc-ell-gam-san-streets-resort-enter-slf-009, 2, 0)
            000A   0x0035A8   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x0035B0    function: wait-say
            0001   0x0035B8    sid: player
            0002   0x0035C0    sid: vox-igc-ell-gam-san-streets-resort-enter-slf-009
          }
          {
            0000   0x003C40   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <wait-say>
            0001   0x003C48   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <player>
            0002   0x003C50   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <vox-igc-ell-gam-san-streets-resort-enter-slf-008>
            0003   0x003C58   0C 03 02 00   LoadU16Imm           r3, 2         r3 = 2
            0004   0x003C60   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
            0005   0x003C68   43 31 01 00   Move                 r49, r1       r49 = player
            0006   0x003C70   43 32 02 00   Move                 r50, r2       r50 = vox-igc-ell-gam-san-streets-resort-enter-slf-008
            0007   0x003C78   43 33 03 00   Move                 r51, r3       r51 = 2
            0008   0x003C80   43 34 04 00   Move                 r52, r4       r52 = 0
            0009   0x003C88   1C 00 00 04   CallFf               r0, r0, 4     r0 = wait-say(player, vox-igc-ell-gam-san-streets-resort-enter-slf-008, 2, 0)
            000A   0x003C90   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003C98    function: wait-say
            0001   0x003CA0    sid: player
            0002   0x003CA8    sid: vox-igc-ell-gam-san-streets-resort-enter-slf-008
          }
          {
            0000   0x0035C8   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <go>
            0001   0x0035D0   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <wait-for-event>
            0002   0x0035D8   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0003   0x0035E0   43 31 01 00   Move                 r49, r1       r49 = wait-for-event
            0004   0x0035E8   43 32 02 00   Move                 r50, r2       r50 = 1
            0005   0x0035F0   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(wait-for-event, 1)
            0006   0x0035F8   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003600    function: go
            0001   0x003608    sid: wait-for-event
          }
        }

        TRACK timeout {
          {
            0000   0x003610   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <wait>
            0001   0x003618   40 01 01 00   LoadStaticFloatImm   r1, 1         r1 = ST[1] -> <5.000000>
            0002   0x003620   43 31 01 00   Move                 r49, r1       r49 = 5.000000
            0003   0x003628   1C 00 00 01   CallFf               r0, r0, 1     r0 = wait(5.000000)
            0004   0x003630   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003638    function: wait
            0001   0x003640    float: 5.000000
          }
          {
            0000   0x0033D0   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <signal-raised?>
            0001   0x0033D8   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <vox>
            0002   0x0033E0   0C 02 00 00   LoadU16Imm           r2, 0         r2 = 0
            0003   0x0033E8   43 31 01 00   Move                 r49, r1       r49 = vox
            0004   0x0033F0   43 32 02 00   Move                 r50, r2       r50 = 0
            0005   0x0033F8   1C 00 00 02   CallFf               r0, r0, 2     r0 = signal-raised?(vox, 0)
            0006   0x003400   30 00 00 00   OpLogNot             r0, r0        r0 = !RET_signal-raised?
            0007   0x003408   2F 0F 00 00   BranchIfNot          r0, 0xF       IF NOT r0 => L_RETURN_0
            0008   0x003410   15 00 02 00   LookupPointer        r0, 2         r0 = ST[2] -> <go>
            0009   0x003418   4A 01 03 00   LoadStaticU64Imm     r1, 3         r1 = ST[3] -> <wait-for-event>
            000A   0x003420   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            000B   0x003428   43 31 01 00   Move                 r49, r1       r49 = wait-for-event
            000C   0x003430   43 32 02 00   Move                 r50, r2       r50 = 1
            000D   0x003438   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(wait-for-event, 1)
            000E   0x003440   2D 10 00 00   Branch               0x10          GOTO => L_RETURN
          L_RETURN_0:
            000F   0x003448   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
          L_RETURN:
            0010   0x003450   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003458    function: signal-raised?
            0001   0x003460    sid: vox
            0002   0x003468    function: go
            0003   0x003470    sid: wait-for-event
          }
        }

      }
      ON event_tension-changed {
        TRACK main {
          {
            0000   0x0035C8   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <go>
            0001   0x0035D0   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <wait-for-event>
            0002   0x0035D8   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0003   0x0035E0   43 31 01 00   Move                 r49, r1       r49 = wait-for-event
            0004   0x0035E8   43 32 02 00   Move                 r50, r2       r50 = 1
            0005   0x0035F0   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(wait-for-event, 1)
            0006   0x0035F8   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003600    function: go
            0001   0x003608    sid: wait-for-event
          }
        }

      }
    } END STATE see-building

    STATE land-in-pool-area {
      ON start {
        TRACK main {
          {
          L_1:
            0000   0x003350   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <player-in-state?>
            0001   0x003358   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <Move>
            0002   0x003360   43 31 01 00   Move                 r49, r1       r49 = Move
            0003   0x003368   1C 00 00 01   CallFf               r0, r0, 1     r0 = player-in-state?(Move)
            0004   0x003370   30 00 00 00   OpLogNot             r0, r0        r0 = !RET_player-in-state?
            0005   0x003378   2F 0B 00 00   BranchIfNot          r0, 0xB       IF NOT r0 => L_RETURN
            0006   0x003380   15 00 02 00   LookupPointer        r0, 2         r0 = ST[2] -> <wait>
            0007   0x003388   40 01 03 00   LoadStaticFloatImm   r1, 3         r1 = ST[3] -> <0.100000>
            0008   0x003390   43 31 01 00   Move                 r49, r1       r49 = 0.100000
            0009   0x003398   1C 00 00 01   CallFf               r0, r0, 1     r0 = wait(0.100000)
            000A   0x0033A0   2D 00 00 00   Branch               0x0           GOTO => L_1
          L_RETURN:
            000B   0x0033A8   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x0033B0    function: player-in-state?
            0001   0x0033B8    sid: Move
            0002   0x0033C0    function: wait
            0003   0x0033C8    float: 0.100000
          }
          {
            0000   0x002788   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <wait>
            0001   0x002790   40 01 01 00   LoadStaticFloatImm   r1, 1         r1 = ST[1] -> <0.500000>
            0002   0x002798   43 31 01 00   Move                 r49, r1       r49 = 0.500000
            0003   0x0027A0   1C 00 00 01   CallFf               r0, r0, 1     r0 = wait(0.500000)
            0004   0x0027A8   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x0027B0    function: wait
            0001   0x0027B8    float: 0.500000
          }
          {
            0000   0x003150   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <wait-say>
            0001   0x003158   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <player>
            0002   0x003160   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <vox-igc-ell-gam-san-streets-resort-enter-slf-014>
            0003   0x003168   0C 03 02 00   LoadU16Imm           r3, 2         r3 = 2
            0004   0x003170   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
            0005   0x003178   43 31 01 00   Move                 r49, r1       r49 = player
            0006   0x003180   43 32 02 00   Move                 r50, r2       r50 = vox-igc-ell-gam-san-streets-resort-enter-slf-014
            0007   0x003188   43 33 03 00   Move                 r51, r3       r51 = 2
            0008   0x003190   43 34 04 00   Move                 r52, r4       r52 = 0
            0009   0x003198   1C 00 00 04   CallFf               r0, r0, 4     r0 = wait-say(player, vox-igc-ell-gam-san-streets-resort-enter-slf-014, 2, 0)
            000A   0x0031A0   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x0031A8    function: wait-say
            0001   0x0031B0    sid: player
            0002   0x0031B8    sid: vox-igc-ell-gam-san-streets-resort-enter-slf-014
          }
          {
            0000   0x004088   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <wait>
            0001   0x004090   40 01 01 00   LoadStaticFloatImm   r1, 1         r1 = ST[1] -> <0.300000>
            0002   0x004098   43 31 01 00   Move                 r49, r1       r49 = 0.300000
            0003   0x0040A0   1C 00 00 01   CallFf               r0, r0, 1     r0 = wait(0.300000)
            0004   0x0040A8   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x0040B0    function: wait
            0001   0x0040B8    float: 0.300000
          }
          {
            0000   0x003478   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <wait-say>
            0001   0x003480   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <player>
            0002   0x003488   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <vox-igc-ell-gam-san-streets-resort-enter-wh-015>
            0003   0x003490   0C 03 02 00   LoadU16Imm           r3, 2         r3 = 2
            0004   0x003498   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
            0005   0x0034A0   43 31 01 00   Move                 r49, r1       r49 = player
            0006   0x0034A8   43 32 02 00   Move                 r50, r2       r50 = vox-igc-ell-gam-san-streets-resort-enter-wh-015
            0007   0x0034B0   43 33 03 00   Move                 r51, r3       r51 = 2
            0008   0x0034B8   43 34 04 00   Move                 r52, r4       r52 = 0
            0009   0x0034C0   1C 00 00 04   CallFf               r0, r0, 4     r0 = wait-say(player, vox-igc-ell-gam-san-streets-resort-enter-wh-015, 2, 0)
            000A   0x0034C8   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x0034D0    function: wait-say
            0001   0x0034D8    sid: player
            0002   0x0034E0    sid: vox-igc-ell-gam-san-streets-resort-enter-wh-015
          }
          {
            0000   0x0035C8   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <go>
            0001   0x0035D0   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <wait-for-event>
            0002   0x0035D8   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0003   0x0035E0   43 31 01 00   Move                 r49, r1       r49 = wait-for-event
            0004   0x0035E8   43 32 02 00   Move                 r50, r2       r50 = 1
            0005   0x0035F0   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(wait-for-event, 1)
            0006   0x0035F8   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003600    function: go
            0001   0x003608    sid: wait-for-event
          }
        }

      }
    } END STATE land-in-pool-area

    STATE see-pool {
      ON start {
        TRACK main {
          {
            0000   0x002718   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <wait-say>
            0001   0x002720   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <player>
            0002   0x002728   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <vox-igc-ell-gam-san-streets-resort-enter-slf-004>
            0003   0x002730   0C 03 02 00   LoadU16Imm           r3, 2         r3 = 2
            0004   0x002738   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
            0005   0x002740   43 31 01 00   Move                 r49, r1       r49 = player
            0006   0x002748   43 32 02 00   Move                 r50, r2       r50 = vox-igc-ell-gam-san-streets-resort-enter-slf-004
            0007   0x002750   43 33 03 00   Move                 r51, r3       r51 = 2
            0008   0x002758   43 34 04 00   Move                 r52, r4       r52 = 0
            0009   0x002760   1C 00 00 04   CallFf               r0, r0, 4     r0 = wait-say(player, vox-igc-ell-gam-san-streets-resort-enter-slf-004, 2, 0)
            000A   0x002768   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x002770    function: wait-say
            0001   0x002778    sid: player
            0002   0x002780    sid: vox-igc-ell-gam-san-streets-resort-enter-slf-004
          }
          {
            0000   0x003D20   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <go>
            0001   0x003D28   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <dinner-convo>
            0002   0x003D30   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0003   0x003D38   43 31 01 00   Move                 r49, r1       r49 = dinner-convo
            0004   0x003D40   43 32 02 00   Move                 r50, r2       r50 = 1
            0005   0x003D48   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(dinner-convo, 1)
            0006   0x003D50   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003D58    function: go
            0001   0x003D60    sid: dinner-convo
          }
        }

      }
    } END STATE see-pool

    STATE dinner-convo {
      ON start {
        TRACK main {
          {
          L_2:
            0000   0x002868   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <tension-mode-is?>
            0001   0x002870   0C 01 02 00   LoadU16Imm           r1, 2         r1 = 2
            0002   0x002878   43 31 01 00   Move                 r49, r1       r49 = 2
            0003   0x002880   1B 00 00 01   Call                 r0, r0, 1     r0 = tension-mode-is?(2)
            0004   0x002888   2F 0D 00 00   BranchIfNot          r0, 0xF       IF NOT r0 => L_0
            0005   0x002890   15 00 01 00   LookupPointer        r0, 1         r0 = ST[1] -> <distance-between-objects-less-than?>
            0006   0x002898   4A 01 02 00   LoadStaticU64Imm     r1, 2         r1 = ST[2] -> <npc-san-pool-inside-1>
            0007   0x0028A0   4A 02 03 00   LoadStaticU64Imm     r2, 3         r2 = ST[3] -> <player>
            0008   0x0028A8   40 03 04 00   LoadStaticFloatImm   r3, 4         r3 = ST[4] -> <20.000000>
            0009   0x0028B0   43 31 01 00   Move                 r49, r1       r49 = npc-san-pool-inside-1
            000A   0x0028B8   43 32 02 00   Move                 r50, r2       r50 = player
            000B   0x0028C0   43 33 03 00   Move                 r51, r3       r51 = 20.000000
            000C   0x0028C8   1B 00 00 03   Call                 r0, r0, 3     r0 = distance-between-objects-less-than?(npc-san-pool-inside-1, player, 20.000000)
            000D   0x0028D0   30 00 00 00   OpLogNot             r0, r0        r0 = !RET_distance-between-objects-less-than?
            000E   0x0028D8   2F 14 00 00   BranchIfNot          r0, 0x14      IF NOT r0 => L_RETURN
          L_0:
            000F   0x0028E0   15 00 05 00   LookupPointer        r0, 5         r0 = ST[5] -> <wait>
            0010   0x0028E8   40 01 06 00   LoadStaticFloatImm   r1, 6         r1 = ST[6] -> <0.100000>
            0011   0x0028F0   43 31 01 00   Move                 r49, r1       r49 = 0.100000
            0012   0x0028F8   1C 00 00 01   CallFf               r0, r0, 1     r0 = wait(0.100000)
            0013   0x002900   2D 00 00 00   Branch               0x0           GOTO => L_2
          L_RETURN:
            0014   0x002908   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x002910    function: tension-mode-is?
            0001   0x002918    function: distance-between-objects-less-than?
            0002   0x002920    sid: npc-san-pool-inside-1
            0003   0x002928    sid: player
            0004   0x002930    float: 20.000000
            0005   0x002938    function: wait
            0006   0x002940    float: 0.100000
          }
          {
            0000   0x003F50   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <npc-using-skill?>
            0001   0x003F58   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <npc-san-pool-inside-1>
            0002   0x003F60   0C 02 14 00   LoadU16Imm           r2, 20        r2 = 20
            0003   0x003F68   43 31 01 00   Move                 r49, r1       r49 = npc-san-pool-inside-1
            0004   0x003F70   43 32 02 00   Move                 r50, r2       r50 = 20
            0005   0x003F78   1C 00 00 02   CallFf               r0, r0, 2     r0 = npc-using-skill?(npc-san-pool-inside-1, 20)
            0006   0x003F80   2E 0D 00 00   BranchIf             r0, 0x20      IF r0 => L_RETURN_0
            0007   0x003F88   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <npc-using-skill?>
            0008   0x003F90   4A 01 02 00   LoadStaticU64Imm     r1, 2         r1 = ST[2] -> <npc-san-pool-inside-2>
            0009   0x003F98   0C 02 14 00   LoadU16Imm           r2, 20        r2 = 20
            000A   0x003FA0   43 31 01 00   Move                 r49, r1       r49 = npc-san-pool-inside-2
            000B   0x003FA8   43 32 02 00   Move                 r50, r2       r50 = 20
            000C   0x003FB0   1C 00 00 02   CallFf               r0, r0, 2     r0 = npc-using-skill?(npc-san-pool-inside-2, 20)
            000D   0x003FB8   30 00 00 00   OpLogNot             r0, r0        r0 = !RET_npc-using-skill?
            000E   0x003FC0   2F 20 00 00   BranchIfNot          r0, 0x20      IF NOT r0 => L_RETURN_0
            000F   0x003FC8   15 00 03 00   LookupPointer        r0, 3         r0 = ST[3] -> <request-dialog>
            0010   0x003FD0   4A 01 04 00   LoadStaticU64Imm     r1, 4         r1 = ST[4] -> <san-streets-resort-enter-enemy-convo-2-001>
            0011   0x003FD8   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <npc-san-pool-inside-2>
            0012   0x003FE0   4A 03 01 00   LoadStaticU64Imm     r3, 1         r3 = ST[1] -> <npc-san-pool-inside-1>
            0013   0x003FE8   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
            0014   0x003FF0   0C 05 00 00   LoadU16Imm           r5, 0         r5 = 0
            0015   0x003FF8   0C 06 00 00   LoadU16Imm           r6, 0         r6 = 0
            0016   0x004000   0C 07 00 00   LoadU16Imm           r7, 0         r7 = 0
            0017   0x004008   43 31 01 00   Move                 r49, r1       r49 = san-streets-resort-enter-enemy-convo-2-001
            0018   0x004010   43 32 02 00   Move                 r50, r2       r50 = npc-san-pool-inside-2
            0019   0x004018   43 33 03 00   Move                 r51, r3       r51 = npc-san-pool-inside-1
            001A   0x004020   43 34 04 00   Move                 r52, r4       r52 = 0
            001B   0x004028   43 35 05 00   Move                 r53, r5       r53 = 0
            001C   0x004030   43 36 06 00   Move                 r54, r6       r54 = 0
            001D   0x004038   43 37 07 00   Move                 r55, r7       r55 = 0
            001E   0x004040   1C 00 00 07   CallFf               r0, r0, 7     r0 = request-dialog(san-streets-resort-enter-enemy-convo-2-001, npc-san-pool-inside-2, npc-san-pool-inside-1, 0, 0, 0, 0)
            001F   0x004048   2D 21 00 00   Branch               0x21          GOTO => L_RETURN
          L_RETURN_0:
            0020   0x004050   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
          L_RETURN:
            0021   0x004058   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x004060    function: npc-using-skill?
            0001   0x004068    sid: npc-san-pool-inside-1
            0002   0x004070    sid: npc-san-pool-inside-2
            0003   0x004078    function: request-dialog
            0004   0x004080    sid: san-streets-resort-enter-enemy-convo-2-001
          }
          {
            0000   0x0035C8   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <go>
            0001   0x0035D0   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <wait-for-event>
            0002   0x0035D8   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0003   0x0035E0   43 31 01 00   Move                 r49, r1       r49 = wait-for-event
            0004   0x0035E8   43 32 02 00   Move                 r50, r2       r50 = 1
            0005   0x0035F0   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(wait-for-event, 1)
            0006   0x0035F8   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003600    function: go
            0001   0x003608    sid: wait-for-event
          }
        }

      }
      ON event_tension-changed {
        TRACK main {
          {
            0000   0x0035C8   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <go>
            0001   0x0035D0   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <wait-for-event>
            0002   0x0035D8   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0003   0x0035E0   43 31 01 00   Move                 r49, r1       r49 = wait-for-event
            0004   0x0035E8   43 32 02 00   Move                 r50, r2       r50 = 1
            0005   0x0035F0   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(wait-for-event, 1)
            0006   0x0035F8   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003600    function: go
            0001   0x003608    sid: wait-for-event
          }
        }

      }
    } END STATE dinner-convo

    STATE pool-combat-done {
      ON start {
        TRACK main {
          {
            0000   0x002E78   0C 00 01 00   LoadU16Imm           r0, 1         r0 = 1
            0001   0x002E80   15 01 00 00   LookupPointer        r1, 0         r1 = ST[0] -> <tension-mode-is?>
            0002   0x002E88   0C 02 03 00   LoadU16Imm           r2, 3         r2 = 3
            0003   0x002E90   43 31 02 00   Move                 r49, r2       r49 = 3
            0004   0x002E98   1B 01 01 01   Call                 r1, r1, 1     r1 = tension-mode-is?(3)
            0005   0x002EA0   2F 0A 01 00   BranchIfNot          r1, 0xA       IF NOT r1 => L_0
            0006   0x002EA8   0C 01 00 00   LoadU16Imm           r1, 0         r1 = 0
            0007   0x002EB0   43 00 01 00   Move                 r0, r1        r0 = 0
            0008   0x002EB8   43 01 00 00   Move                 r1, r0        r1 = 0
            0009   0x002EC0   2D 0B 00 00   Branch               0xB           GOTO => L_1
          L_0:
            000A   0x002EC8   0C 01 00 00   LoadU16Imm           r1, 0         r1 = 0
          L_1:
            000B   0x002ED0   15 01 01 00   LookupPointer        r1, 1         r1 = ST[1] -> <wait-until-non-combat-tension>
            000C   0x002ED8   40 02 02 00   LoadStaticFloatImm   r2, 2         r2 = ST[2] -> <4.000000>
            000D   0x002EE0   40 03 02 00   LoadStaticFloatImm   r3, 2         r3 = ST[2] -> <4.000000>
            000E   0x002EE8   43 31 02 00   Move                 r49, r2       r49 = 4.000000
            000F   0x002EF0   43 32 03 00   Move                 r50, r3       r50 = 4.000000
            0010   0x002EF8   1B 01 01 02   Call                 r1, r1, 2     r1 = wait-until-non-combat-tension(4.000000, 4.000000)
            0011   0x002F00   43 01 00 00   Move                 r1, r0        r1 = 0
            0012   0x002F08   43 02 01 00   Move                 r2, r1        r2 = 0
            0013   0x002F10   0C 03 01 00   LoadU16Imm           r3, 1         r3 = 1
            0014   0x002F18   1D 02 02 03   IEqual               r2, r2, r3    r2 = r2 == r3
            0015   0x002F20   2F 2F 02 00   BranchIfNot          r2, 0x2F      IF NOT r2 => L_2
            0016   0x002F28   15 02 03 00   LookupPointer        r2, 3         r2 = ST[3] -> <wait-say>
            0017   0x002F30   4A 03 04 00   LoadStaticU64Imm     r3, 4         r3 = ST[4] -> <player>
            0018   0x002F38   4A 04 05 00   LoadStaticU64Imm     r4, 5         r4 = ST[5] -> <vox-igc-ell-gam-san-streets-resort-combat-end-wh-004>
            0019   0x002F40   0C 05 02 00   LoadU16Imm           r5, 2         r5 = 2
            001A   0x002F48   0C 06 00 00   LoadU16Imm           r6, 0         r6 = 0
            001B   0x002F50   43 31 03 00   Move                 r49, r3       r49 = player
            001C   0x002F58   43 32 04 00   Move                 r50, r4       r50 = vox-igc-ell-gam-san-streets-resort-combat-end-wh-004
            001D   0x002F60   43 33 05 00   Move                 r51, r5       r51 = 2
            001E   0x002F68   43 34 06 00   Move                 r52, r6       r52 = 0
            001F   0x002F70   1C 02 02 04   CallFf               r2, r2, 4     r2 = wait-say(player, vox-igc-ell-gam-san-streets-resort-combat-end-wh-004, 2, 0)
            0020   0x002F78   15 02 06 00   LookupPointer        r2, 6         r2 = ST[6] -> <wait>
            0021   0x002F80   40 03 07 00   LoadStaticFloatImm   r3, 7         r3 = ST[7] -> <0.500000>
            0022   0x002F88   43 31 03 00   Move                 r49, r3       r49 = 0.500000
            0023   0x002F90   1C 02 02 01   CallFf               r2, r2, 1     r2 = wait(0.500000)
            0024   0x002F98   15 02 03 00   LookupPointer        r2, 3         r2 = ST[3] -> <wait-say>
            0025   0x002FA0   4A 03 04 00   LoadStaticU64Imm     r3, 4         r3 = ST[4] -> <player>
            0026   0x002FA8   4A 04 08 00   LoadStaticU64Imm     r4, 8         r4 = ST[8] -> <vox-igc-ell-gam-san-streets-resort-combat-end-slf-003>
            0027   0x002FB0   0C 05 02 00   LoadU16Imm           r5, 2         r5 = 2
            0028   0x002FB8   0C 06 00 00   LoadU16Imm           r6, 0         r6 = 0
            0029   0x002FC0   43 31 03 00   Move                 r49, r3       r49 = player
            002A   0x002FC8   43 32 04 00   Move                 r50, r4       r50 = vox-igc-ell-gam-san-streets-resort-combat-end-slf-003
            002B   0x002FD0   43 33 05 00   Move                 r51, r5       r51 = 2
            002C   0x002FD8   43 34 06 00   Move                 r52, r6       r52 = 0
            002D   0x002FE0   1C 02 02 04   CallFf               r2, r2, 4     r2 = wait-say(player, vox-igc-ell-gam-san-streets-resort-combat-end-slf-003, 2, 0)
            002E   0x002FE8   2D 4D 00 00   Branch               0x4D          GOTO => L_3
          L_2:
            002F   0x002FF0   43 02 01 00   Move                 r2, r1        r2 = 0
            0030   0x002FF8   0C 03 00 00   LoadU16Imm           r3, 0         r3 = 0
            0031   0x003000   1D 02 02 03   IEqual               r2, r2, r3    r2 = r2 == r3
            0032   0x003008   2F 4C 02 00   BranchIfNot          r2, 0x4C      IF NOT r2 => L_4
            0033   0x003010   15 02 03 00   LookupPointer        r2, 3         r2 = ST[3] -> <wait-say>
            0034   0x003018   4A 03 04 00   LoadStaticU64Imm     r3, 4         r3 = ST[4] -> <player>
            0035   0x003020   4A 04 09 00   LoadStaticU64Imm     r4, 9         r4 = ST[9] -> <vox-igc-ell-gam-san-streets-resort-combat-end-slf-002>
            0036   0x003028   0C 05 02 00   LoadU16Imm           r5, 2         r5 = 2
            0037   0x003030   0C 06 00 00   LoadU16Imm           r6, 0         r6 = 0
            0038   0x003038   43 31 03 00   Move                 r49, r3       r49 = player
            0039   0x003040   43 32 04 00   Move                 r50, r4       r50 = vox-igc-ell-gam-san-streets-resort-combat-end-slf-002
            003A   0x003048   43 33 05 00   Move                 r51, r5       r51 = 2
            003B   0x003050   43 34 06 00   Move                 r52, r6       r52 = 0
            003C   0x003058   1C 02 02 04   CallFf               r2, r2, 4     r2 = wait-say(player, vox-igc-ell-gam-san-streets-resort-combat-end-slf-002, 2, 0)
            003D   0x003060   15 02 06 00   LookupPointer        r2, 6         r2 = ST[6] -> <wait>
            003E   0x003068   40 03 07 00   LoadStaticFloatImm   r3, 7         r3 = ST[7] -> <0.500000>
            003F   0x003070   43 31 03 00   Move                 r49, r3       r49 = 0.500000
            0040   0x003078   1C 02 02 01   CallFf               r2, r2, 1     r2 = wait(0.500000)
            0041   0x003080   15 02 03 00   LookupPointer        r2, 3         r2 = ST[3] -> <wait-say>
            0042   0x003088   4A 03 04 00   LoadStaticU64Imm     r3, 4         r3 = ST[4] -> <player>
            0043   0x003090   4A 04 0A 00   LoadStaticU64Imm     r4, 10        r4 = ST[10] -> <vox-igc-ell-gam-san-streets-resort-combat-end-slf-001>
            0044   0x003098   0C 05 02 00   LoadU16Imm           r5, 2         r5 = 2
            0045   0x0030A0   0C 06 00 00   LoadU16Imm           r6, 0         r6 = 0
            0046   0x0030A8   43 31 03 00   Move                 r49, r3       r49 = player
            0047   0x0030B0   43 32 04 00   Move                 r50, r4       r50 = vox-igc-ell-gam-san-streets-resort-combat-end-slf-001
            0048   0x0030B8   43 33 05 00   Move                 r51, r5       r51 = 2
            0049   0x0030C0   43 34 06 00   Move                 r52, r6       r52 = 0
            004A   0x0030C8   1C 02 02 04   CallFf               r2, r2, 4     r2 = wait-say(player, vox-igc-ell-gam-san-streets-resort-combat-end-slf-001, 2, 0)
            004B   0x0030D0   2D 4D 00 00   Branch               0x4D          GOTO => L_3
          L_4:
            004C   0x0030D8   0C 02 00 00   LoadU16Imm           r2, 0         r2 = 0
          L_3:
            004D   0x0030E0   43 01 02 00   Move                 r1, r2        r1 = 0
            004E   0x0030E8   43 00 01 00   Move                 r0, r1        r0 = 0
            004F   0x0030F0   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x0030F8    function: tension-mode-is?
            0001   0x003100    function: wait-until-non-combat-tension
            0002   0x003108    float: 4.000000
            0003   0x003110    function: wait-say
            0004   0x003118    sid: player
            0005   0x003120    sid: vox-igc-ell-gam-san-streets-resort-combat-end-wh-004
            0006   0x003128    function: wait
            0007   0x003130    float: 0.500000
            0008   0x003138    sid: vox-igc-ell-gam-san-streets-resort-combat-end-slf-003
            0009   0x003140    sid: vox-igc-ell-gam-san-streets-resort-combat-end-slf-002
            000A   0x003148    sid: vox-igc-ell-gam-san-streets-resort-combat-end-slf-001
          }
          {
            0000   0x0027C0   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <wait>
            0001   0x0027C8   40 01 01 00   LoadStaticFloatImm   r1, 1         r1 = ST[1] -> <2.000000>
            0002   0x0027D0   43 31 01 00   Move                 r49, r1       r49 = 2.000000
            0003   0x0027D8   1C 00 00 01   CallFf               r0, r0, 1     r0 = wait(2.000000)
            0004   0x0027E0   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x0027E8    function: wait
            0001   0x0027F0    float: 2.000000
          }
          {
            0000   0x0039A8   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <wait-say>
            0001   0x0039B0   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <player>
            0002   0x0039B8   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <vox-igc-ell-gam-san-streets-resort-enter-slf-013>
            0003   0x0039C0   0C 03 02 00   LoadU16Imm           r3, 2         r3 = 2
            0004   0x0039C8   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
            0005   0x0039D0   43 31 01 00   Move                 r49, r1       r49 = player
            0006   0x0039D8   43 32 02 00   Move                 r50, r2       r50 = vox-igc-ell-gam-san-streets-resort-enter-slf-013
            0007   0x0039E0   43 33 03 00   Move                 r51, r3       r51 = 2
            0008   0x0039E8   43 34 04 00   Move                 r52, r4       r52 = 0
            0009   0x0039F0   1C 00 00 04   CallFf               r0, r0, 4     r0 = wait-say(player, vox-igc-ell-gam-san-streets-resort-enter-slf-013, 2, 0)
            000A   0x0039F8   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003A00    function: wait-say
            0001   0x003A08    sid: player
            0002   0x003A10    sid: vox-igc-ell-gam-san-streets-resort-enter-slf-013
          }
          {
            0000   0x003CB0   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <wait-say>
            0001   0x003CB8   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <player>
            0002   0x003CC0   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <vox-igc-ell-gam-san-streets-resort-enter-wh-001>
            0003   0x003CC8   0C 03 02 00   LoadU16Imm           r3, 2         r3 = 2
            0004   0x003CD0   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
            0005   0x003CD8   43 31 01 00   Move                 r49, r1       r49 = player
            0006   0x003CE0   43 32 02 00   Move                 r50, r2       r50 = vox-igc-ell-gam-san-streets-resort-enter-wh-001
            0007   0x003CE8   43 33 03 00   Move                 r51, r3       r51 = 2
            0008   0x003CF0   43 34 04 00   Move                 r52, r4       r52 = 0
            0009   0x003CF8   1C 00 00 04   CallFf               r0, r0, 4     r0 = wait-say(player, vox-igc-ell-gam-san-streets-resort-enter-wh-001, 2, 0)
            000A   0x003D00   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003D08    function: wait-say
            0001   0x003D10    sid: player
            0002   0x003D18    sid: vox-igc-ell-gam-san-streets-resort-enter-wh-001
          }
          {
            0000   0x004088   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <wait>
            0001   0x004090   40 01 01 00   LoadStaticFloatImm   r1, 1         r1 = ST[1] -> <0.300000>
            0002   0x004098   43 31 01 00   Move                 r49, r1       r49 = 0.300000
            0003   0x0040A0   1C 00 00 01   CallFf               r0, r0, 1     r0 = wait(0.300000)
            0004   0x0040A8   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x0040B0    function: wait
            0001   0x0040B8    float: 0.300000
          }
          {
            0000   0x0034E8   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <wait-say>
            0001   0x0034F0   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <player>
            0002   0x0034F8   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <vox-igc-ell-gam-san-streets-resort-enter-slf-018>
            0003   0x003500   0C 03 02 00   LoadU16Imm           r3, 2         r3 = 2
            0004   0x003508   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
            0005   0x003510   43 31 01 00   Move                 r49, r1       r49 = player
            0006   0x003518   43 32 02 00   Move                 r50, r2       r50 = vox-igc-ell-gam-san-streets-resort-enter-slf-018
            0007   0x003520   43 33 03 00   Move                 r51, r3       r51 = 2
            0008   0x003528   43 34 04 00   Move                 r52, r4       r52 = 0
            0009   0x003530   1C 00 00 04   CallFf               r0, r0, 4     r0 = wait-say(player, vox-igc-ell-gam-san-streets-resort-enter-slf-018, 2, 0)
            000A   0x003538   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003540    function: wait-say
            0001   0x003548    sid: player
            0002   0x003550    sid: vox-igc-ell-gam-san-streets-resort-enter-slf-018
          }
          {
            0000   0x002788   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <wait>
            0001   0x002790   40 01 01 00   LoadStaticFloatImm   r1, 1         r1 = ST[1] -> <0.500000>
            0002   0x002798   43 31 01 00   Move                 r49, r1       r49 = 0.500000
            0003   0x0027A0   1C 00 00 01   CallFf               r0, r0, 1     r0 = wait(0.500000)
            0004   0x0027A8   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x0027B0    function: wait
            0001   0x0027B8    float: 0.500000
          }
          {
            0000   0x0027F8   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <wait-say>
            0001   0x002800   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <player>
            0002   0x002808   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <vox-igc-ell-gam-san-streets-resort-enter-slf-019>
            0003   0x002810   0C 03 02 00   LoadU16Imm           r3, 2         r3 = 2
            0004   0x002818   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
            0005   0x002820   43 31 01 00   Move                 r49, r1       r49 = player
            0006   0x002828   43 32 02 00   Move                 r50, r2       r50 = vox-igc-ell-gam-san-streets-resort-enter-slf-019
            0007   0x002830   43 33 03 00   Move                 r51, r3       r51 = 2
            0008   0x002838   43 34 04 00   Move                 r52, r4       r52 = 0
            0009   0x002840   1C 00 00 04   CallFf               r0, r0, 4     r0 = wait-say(player, vox-igc-ell-gam-san-streets-resort-enter-slf-019, 2, 0)
            000A   0x002848   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x002850    function: wait-say
            0001   0x002858    sid: player
            0002   0x002860    sid: vox-igc-ell-gam-san-streets-resort-enter-slf-019
          }
          {
            0000   0x0035C8   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <go>
            0001   0x0035D0   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <wait-for-event>
            0002   0x0035D8   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0003   0x0035E0   43 31 01 00   Move                 r49, r1       r49 = wait-for-event
            0004   0x0035E8   43 32 02 00   Move                 r50, r2       r50 = 1
            0005   0x0035F0   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(wait-for-event, 1)
            0006   0x0035F8   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003600    function: go
            0001   0x003608    sid: wait-for-event
          }
        }

      }
    } END STATE pool-combat-done

    STATE landing-combat-done {
      ON start {
        TRACK main {
          {
            0000   0x0040F8   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <wait-until-non-combat-tension>
            0001   0x004100   40 01 01 00   LoadStaticFloatImm   r1, 1         r1 = ST[1] -> <4.000000>
            0002   0x004108   40 02 01 00   LoadStaticFloatImm   r2, 1         r2 = ST[1] -> <4.000000>
            0003   0x004110   43 31 01 00   Move                 r49, r1       r49 = 4.000000
            0004   0x004118   43 32 02 00   Move                 r50, r2       r50 = 4.000000
            0005   0x004120   1B 00 00 02   Call                 r0, r0, 2     r0 = wait-until-non-combat-tension(4.000000, 4.000000)
            0006   0x004128   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x004130    function: wait-until-non-combat-tension
            0001   0x004138    float: 4.000000
          }
          {
            0000   0x003848   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <wait-say>
            0001   0x003850   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <player>
            0002   0x003858   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <vox-igc-ell-gam-san-streets-resort-downstairs-wh-007>
            0003   0x003860   0C 03 02 00   LoadU16Imm           r3, 2         r3 = 2
            0004   0x003868   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
            0005   0x003870   43 31 01 00   Move                 r49, r1       r49 = player
            0006   0x003878   43 32 02 00   Move                 r50, r2       r50 = vox-igc-ell-gam-san-streets-resort-downstairs-wh-007
            0007   0x003880   43 33 03 00   Move                 r51, r3       r51 = 2
            0008   0x003888   43 34 04 00   Move                 r52, r4       r52 = 0
            0009   0x003890   1C 00 00 04   CallFf               r0, r0, 4     r0 = wait-say(player, vox-igc-ell-gam-san-streets-resort-downstairs-wh-007, 2, 0)
            000A   0x003898   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x0038A0    function: wait-say
            0001   0x0038A8    sid: player
            0002   0x0038B0    sid: vox-igc-ell-gam-san-streets-resort-downstairs-wh-007
          }
          {
            0000   0x003648   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <wait-say>
            0001   0x003650   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <player>
            0002   0x003658   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <vox-igc-ell-gam-san-streets-resort-downstairs-slf-006>
            0003   0x003660   0C 03 02 00   LoadU16Imm           r3, 2         r3 = 2
            0004   0x003668   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
            0005   0x003670   43 31 01 00   Move                 r49, r1       r49 = player
            0006   0x003678   43 32 02 00   Move                 r50, r2       r50 = vox-igc-ell-gam-san-streets-resort-downstairs-slf-006
            0007   0x003680   43 33 03 00   Move                 r51, r3       r51 = 2
            0008   0x003688   43 34 04 00   Move                 r52, r4       r52 = 0
            0009   0x003690   1C 00 00 04   CallFf               r0, r0, 4     r0 = wait-say(player, vox-igc-ell-gam-san-streets-resort-downstairs-slf-006, 2, 0)
            000A   0x003698   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x0036A0    function: wait-say
            0001   0x0036A8    sid: player
            0002   0x0036B0    sid: vox-igc-ell-gam-san-streets-resort-downstairs-slf-006
          }
          {
            0000   0x0040C0   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <wait>
            0001   0x0040C8   40 01 01 00   LoadStaticFloatImm   r1, 1         r1 = ST[1] -> <1.000000>
            0002   0x0040D0   43 31 01 00   Move                 r49, r1       r49 = 1.000000
            0003   0x0040D8   1C 00 00 01   CallFf               r0, r0, 1     r0 = wait(1.000000)
            0004   0x0040E0   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x0040E8    function: wait
            0001   0x0040F0    float: 1.000000
          }
          {
            0000   0x003EE0   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <wait-say>
            0001   0x003EE8   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <player>
            0002   0x003EF0   4A 02 02 00   LoadStaticU64Imm     r2, 2         r2 = ST[2] -> <vox-igc-ell-gam-san-streets-resort-lobby-combat-end-slf-001>
            0003   0x003EF8   0C 03 02 00   LoadU16Imm           r3, 2         r3 = 2
            0004   0x003F00   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
            0005   0x003F08   43 31 01 00   Move                 r49, r1       r49 = player
            0006   0x003F10   43 32 02 00   Move                 r50, r2       r50 = vox-igc-ell-gam-san-streets-resort-lobby-combat-end-slf-001
            0007   0x003F18   43 33 03 00   Move                 r51, r3       r51 = 2
            0008   0x003F20   43 34 04 00   Move                 r52, r4       r52 = 0
            0009   0x003F28   1C 00 00 04   CallFf               r0, r0, 4     r0 = wait-say(player, vox-igc-ell-gam-san-streets-resort-lobby-combat-end-slf-001, 2, 0)
            000A   0x003F30   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003F38    function: wait-say
            0001   0x003F40    sid: player
            0002   0x003F48    sid: vox-igc-ell-gam-san-streets-resort-lobby-combat-end-slf-001
          }
          {
            0000   0x0032B8   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <is-encounter-select?>
            0001   0x0032C0   1C 00 00 00   CallFf               r0, r0, 0     r0 = is-encounter-select?()
            0002   0x0032C8   2F 0C 00 00   BranchIfNot          r0, 0xC       IF NOT r0 => L_RETURN_0
            0003   0x0032D0   15 00 01 00   LookupPointer        r0, 1         r0 = ST[1] -> <wait>
            0004   0x0032D8   40 01 02 00   LoadStaticFloatImm   r1, 2         r1 = ST[2] -> <1.000000>
            0005   0x0032E0   43 31 01 00   Move                 r49, r1       r49 = 1.000000
            0006   0x0032E8   1C 00 00 01   CallFf               r0, r0, 1     r0 = wait(1.000000)
            0007   0x0032F0   15 00 03 00   LookupPointer        r0, 3         r0 = ST[3] -> <select-mode-mark-complete>
            0008   0x0032F8   40 01 04 00   LoadStaticFloatImm   r1, 4         r1 = ST[4] -> <0.000000>
            0009   0x003300   43 31 01 00   Move                 r49, r1       r49 = 0.000000
            000A   0x003308   1C 00 00 01   CallFf               r0, r0, 1     r0 = select-mode-mark-complete(0.000000)
            000B   0x003310   2D 0D 00 00   Branch               0xD           GOTO => L_RETURN
          L_RETURN_0:
            000C   0x003318   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
          L_RETURN:
            000D   0x003320   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003328    function: is-encounter-select?
            0001   0x003330    function: wait
            0002   0x003338    float: 1.000000
            0003   0x003340    function: select-mode-mark-complete
            0004   0x003348    float: 0.000000
          }
          {
            0000   0x0035C8   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <go>
            0001   0x0035D0   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <wait-for-event>
            0002   0x0035D8   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0003   0x0035E0   43 31 01 00   Move                 r49, r1       r49 = wait-for-event
            0004   0x0035E8   43 32 02 00   Move                 r50, r2       r50 = 1
            0005   0x0035F0   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(wait-for-event, 1)
            0006   0x0035F8   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003600    function: go
            0001   0x003608    sid: wait-for-event
          }
        }

      }
    } END STATE landing-combat-done

    STATE first-search-start {
      ON start {
        TRACK main {
          {
            0000   0x0038B8   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <request-dialog>
            0001   0x0038C0   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <cya-san-resort-pool-search-start-first-001>
            0002   0x0038C8   15 02 02 00   LookupPointer        r2, 2         r2 = ST[2] -> <get-closest-living-npc-in-group>
            0003   0x0038D0   4A 03 03 00   LoadStaticU64Imm     r3, 3         r3 = ST[3] -> <all-enemy-npcs>
            0004   0x0038D8   4A 04 04 00   LoadStaticU64Imm     r4, 4         r4 = ST[4] -> <player>
            0005   0x0038E0   0C 05 00 00   LoadU16Imm           r5, 0         r5 = 0
            0006   0x0038E8   43 31 03 00   Move                 r49, r3       r49 = all-enemy-npcs
            0007   0x0038F0   43 32 04 00   Move                 r50, r4       r50 = player
            0008   0x0038F8   43 33 05 00   Move                 r51, r5       r51 = 0
            0009   0x003900   1B 02 02 03   Call                 r2, r2, 3     r2 = get-closest-living-npc-in-group(all-enemy-npcs, player, 0)
            000A   0x003908   4A 03 05 00   LoadStaticU64Imm     r3, 5         r3 = ST[5] -> <vox-san-pool-fake-slaver-male-d-1>
            000B   0x003910   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
            000C   0x003918   0C 05 00 00   LoadU16Imm           r5, 0         r5 = 0
            000D   0x003920   0C 06 00 00   LoadU16Imm           r6, 0         r6 = 0
            000E   0x003928   0C 07 00 00   LoadU16Imm           r7, 0         r7 = 0
            000F   0x003930   43 31 01 00   Move                 r49, r1       r49 = cya-san-resort-pool-search-start-first-001
            0010   0x003938   43 32 02 00   Move                 r50, r2       r50 = RET_get-closest-living-npc-in-group
            0011   0x003940   43 33 03 00   Move                 r51, r3       r51 = vox-san-pool-fake-slaver-male-d-1
            0012   0x003948   43 34 04 00   Move                 r52, r4       r52 = 0
            0013   0x003950   43 35 05 00   Move                 r53, r5       r53 = 0
            0014   0x003958   43 36 06 00   Move                 r54, r6       r54 = 0
            0015   0x003960   43 37 07 00   Move                 r55, r7       r55 = 0
            0016   0x003968   1C 00 00 07   CallFf               r0, r0, 7     r0 = request-dialog(cya-san-resort-pool-search-start-first-001, RET_get-closest-living-npc-in-group, vox-san-pool-fake-slaver-male-d-1, 0, 0, 0, 0)
            0017   0x003970   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003978    function: request-dialog
            0001   0x003980    sid: cya-san-resort-pool-search-start-first-001
            0002   0x003988    function: get-closest-living-npc-in-group
            0003   0x003990    sid: all-enemy-npcs
            0004   0x003998    sid: player
            0005   0x0039A0    sid: vox-san-pool-fake-slaver-male-d-1
          }
          {
            0000   0x0035C8   15 00 00 00   LookupPointer        r0, 0         r0 = ST[0] -> <go>
            0001   0x0035D0   4A 01 01 00   LoadStaticU64Imm     r1, 1         r1 = ST[1] -> <wait-for-event>
            0002   0x0035D8   0C 02 01 00   LoadU16Imm           r2, 1         r2 = 1
            0003   0x0035E0   43 31 01 00   Move                 r49, r1       r49 = wait-for-event
            0004   0x0035E8   43 32 02 00   Move                 r50, r2       r50 = 1
            0005   0x0035F0   1C 00 00 02   CallFf               r0, r0, 2     r0 = go(wait-for-event, 1)
            0006   0x0035F8   00 00 00 00   Return               r0            Return 

            SYMBOL TABLE: 
            0000   0x003600    function: go
            0001   0x003608    sid: wait-for-event
          }
        }

      }
    } END STATE first-search-start

    STATE done {
    } END STATE done

}


##############################  ENTRY 2  ##############################

#26AD48AABF5B1380 = d-array [0x00130] {
  [0] array* [0x006C8] {
    [0] anonymous array [0x6c8] {size: 9} {
      [0] anonymous struct [0x798] {
        [0] boxed-symbol [0x01228] {
          [0] sid: npc-san-pool-landing-2
          [1] int: 0
          [2] int: 7
        }
      }
      [1] anonymous struct [0x7a0] {
        [0] boxed-symbol [0x00C48] {
          [0] sid: npc-san-pool-landing-3
          [1] int: 0
          [2] int: 7
        }
      }
      [2] anonymous struct [0x7a8] {
        [0] boxed-symbol [0x00D20] {
          [0] sid: npc-san-pool-landing-5
          [1] int: 0
          [2] int: 7
        }
      }
      [3] anonymous struct [0x7b0] {
        [0] boxed-symbol [0x00BC0] {
          [0] sid: npc-san-pool-landing-6
          [1] int: 0
          [2] int: 7
        }
      }
      [4] anonymous struct [0x7b8] {
        [0] boxed-symbol [0x011F0] {
          [0] sid: npc-san-pool-landing-7
          [1] int: 0
          [2] int: 7
        }
      }
      [5] anonymous struct [0x7c0] {
        [0] boxed-symbol [0x00848] {
          [0] sid: npc-san-pool-landing-4
          [1] int: 0
          [2] int: 7
        }
      }
      [6] anonymous struct [0x7c8] {
        [0] boxed-symbol [0x009F0] {
          [0] sid: npc-san-pool-landing-8
          [1] int: 0
          [2] int: 7
        }
      }
      [7] anonymous struct [0x7d0] {
        [0] boxed-symbol [0x00A20] {
          [0] sid: npc-san-pool-landing-9
          [1] int: 0
          [2] int: 7
        }
      }
      [8] anonymous struct [0x7d8] {
        [0] boxed-symbol [0x00900] {
          [0] sid: npc-san-pool-landing-10
          [1] int: 0
          [2] int: 7
        }
      }
    }
  }
  [1] int: 0
  [2] int: 0
}


##############################  ENTRY 3  ##############################

#26AD4BAABF5B1899 = d-array [0x00148] {
  [0] array* [0x005B8] {
    [0] anonymous array [0x5b8] {size: 15} {
      [0] anonymous struct [0x720] {
        [0] boxed-symbol [0x00D38] {
          [0] sid: npc-san-pool-outside-1
          [1] int: 0
          [2] int: 7
        }
      }
      [1] anonymous struct [0x727] {
        [0] boxed-symbol [0x00D38] {
          [0] sid: npc-san-pool-outside-1
          [1] int: 0
          [2] int: 7
        }
      }
      [2] anonymous struct [0x72e] {
        [0] boxed-symbol [0x00BD8] {
          [0] sid: npc-san-pool-outside-2
          [1] int: 0
          [2] int: 7
        }
      }
      [3] anonymous struct [0x735] {
        [0] boxed-symbol [0x00DA8] {
          [0] sid: npc-san-pool-outside-3
          [1] int: 0
          [2] int: 7
        }
      }
      [4] anonymous struct [0x73c] {
        [0] boxed-symbol [0x008D0] {
          [0] sid: npc-san-pool-outside-4
          [1] int: 0
          [2] int: 7
        }
      }
      [5] anonymous struct [0x743] {
        [0] boxed-symbol [0x00818] {
          [0] sid: npc-san-pool-outside-5
          [1] int: 0
          [2] int: 7
        }
      }
      [6] anonymous struct [0x74a] {
        [0] boxed-symbol [0x01210] {
          [0] sid: npc-san-pool-outside-6
          [1] int: 0
          [2] int: 7
        }
      }
      [7] anonymous struct [0x751] {
        [0] boxed-symbol [0x009C0] {
          [0] sid: npc-san-pool-inside-1
          [1] int: 0
          [2] int: 7
        }
      }
      [8] anonymous struct [0x758] {
        [0] boxed-symbol [0x008E8] {
          [0] sid: npc-san-pool-inside-2
          [1] int: 0
          [2] int: 7
        }
      }
      [9] anonymous struct [0x75f] {
        [0] boxed-symbol [0x008E8] {
          [0] sid: npc-san-pool-inside-2
          [1] int: 0
          [2] int: 7
        }
      }
      [10] anonymous struct [0x766] {
        [0] boxed-symbol [0x00860] {
          [0] sid: npc-san-pool-inside-3
          [1] int: 0
          [2] int: 7
        }
      }
      [11] anonymous struct [0x76d] {
        [0] boxed-symbol [0x00FC0] {
          [0] sid: npc-san-pool-inside-4
          [1] int: 0
          [2] int: 7
        }
      }
      [12] anonymous struct [0x774] {
        [0] boxed-symbol [0x00DC0] {
          [0] sid: npc-san-pool-inside-5
          [1] int: 0
          [2] int: 7
        }
      }
      [13] anonymous struct [0x77b] {
        [0] boxed-symbol [0x00B38] {
          [0] sid: npc-san-pool-inside-6
          [1] int: 0
          [2] int: 7
        }
      }
      [14] anonymous struct [0x782] {
        [0] boxed-symbol [0x00830] {
          [0] sid: npc-san-pool-upstairs-3
          [1] int: 0
          [2] int: 7
        }
      }
    }
    [1] int: 15
    [2] int: -1
  }
  [1] int: 0
  [2] int: 0
}


##############################  ENTRY 4  ##############################

#502C6E38E46C6A50 = script-lambda [0x00160] {
    0000   0x0001B0   0C 00 00 00   LoadU16Imm           r0, 0         r0 = 0
    0001   0x0001B8   0C 01 00 00   LoadU16Imm           r1, 0         r1 = 0
    0002   0x0001C0   15 02 00 00   LookupPointer        r2, 0         r2 = ST[0] -> <darray-count>
    0003   0x0001C8   4A 03 01 00   LoadStaticU64Imm     r3, 1         r3 = ST[1] -> <#029902C61617476E>
    0004   0x0001D0   43 31 03 00   Move                 r49, r3       r49 = #029902C61617476E
    0005   0x0001D8   1C 02 02 01   CallFf               r2, r2, 1     r2 = darray-count(#029902C61617476E)
    0006   0x0001E0   15 03 02 00   LookupPointer        r3, 2         r3 = ST[2] -> <begin-foreach>
    0007   0x0001E8   1C 03 03 00   CallFf               r3, r3, 0     r3 = begin-foreach()
  L_9:
    0008   0x0001F0   43 03 01 00   Move                 r3, r1        r3 = 0
    0009   0x0001F8   43 04 02 00   Move                 r4, r2        r4 = RET_darray-count
    000A   0x000200   20 03 03 04   ILessThan            r3, r3, r4    r3 = r3 < r4
    000B   0x000208   2F 56 03 00   BranchIfNot          r3, 0x56      IF NOT r3 => L_0
    000C   0x000210   15 03 03 00   LookupPointer        r3, 3         r3 = ST[3] -> <darray-at>
    000D   0x000218   4A 04 01 00   LoadStaticU64Imm     r4, 1         r4 = ST[1] -> <#029902C61617476E>
    000E   0x000220   43 05 01 00   Move                 r5, r1        r5 = 0
    000F   0x000228   43 31 04 00   Move                 r49, r4       r49 = #029902C61617476E
    0010   0x000230   43 32 05 00   Move                 r50, r5       r50 = 0
    0011   0x000238   1C 03 03 02   CallFf               r3, r3, 2     r3 = darray-at(#029902C61617476E, 0)
    0012   0x000240   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    0013   0x000248   2F 1A 04 00   BranchIfNot          r4, 0x1F      IF NOT r4 => L_1
    0014   0x000250   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    0015   0x000258   5D 04 00 00   AssertPointer        r4            r4 != nullptr
    0016   0x000260   3B 04 04 0C   IAddImm              r4, r4, 12    r4 = RET_darray-at + 12 -> <RET_(null)>
    0017   0x000268   4E 04 04 00   LoadU16              r4, [r4]      r4 = *(u16*)RET_darray-at
    0018   0x000270   0C 05 07 00   LoadU16Imm           r5, 7         r5 = 7
    0019   0x000278   1D 04 04 05   IEqual               r4, r4, r5    r4 = r4 == r5
    001A   0x000280   2F 1F 04 00   BranchIfNot          r4, 0x1F      IF NOT r4 => L_1
    001B   0x000288   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    001C   0x000290   5D 04 00 00   AssertPointer        r4            r4 != nullptr
    001D   0x000298   51 04 04 00   LoadU64              r4, [r4]      r4 = *(u64*)RET_darray-at
    001E   0x0002A0   2D 3A 00 00   Branch               0x3A          GOTO => L_2
  L_1:
    001F   0x0002A8   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    0020   0x0002B0   2F 27 04 00   BranchIfNot          r4, 0x2C      IF NOT r4 => L_3
    0021   0x0002B8   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    0022   0x0002C0   5D 04 00 00   AssertPointer        r4            r4 != nullptr
    0023   0x0002C8   3B 04 04 0C   IAddImm              r4, r4, 12    r4 = RET_darray-at + 12 -> <RET_(null)>
    0024   0x0002D0   4E 04 04 00   LoadU16              r4, [r4]      r4 = *(u16*)RET_darray-at
    0025   0x0002D8   0C 05 05 00   LoadU16Imm           r5, 5         r5 = 5
    0026   0x0002E0   1D 04 04 05   IEqual               r4, r4, r5    r4 = r4 == r5
    0027   0x0002E8   2F 2C 04 00   BranchIfNot          r4, 0x2C      IF NOT r4 => L_3
    0028   0x0002F0   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    0029   0x0002F8   5D 04 00 00   AssertPointer        r4            r4 != nullptr
    002A   0x000300   51 04 04 00   LoadU64              r4, [r4]      r4 = *(u64*)RET_darray-at
    002B   0x000308   2D 3A 00 00   Branch               0x3A          GOTO => L_2
  L_3:
    002C   0x000310   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    002D   0x000318   2F 34 04 00   BranchIfNot          r4, 0x39      IF NOT r4 => L_4
    002E   0x000320   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    002F   0x000328   5D 04 00 00   AssertPointer        r4            r4 != nullptr
    0030   0x000330   3B 04 04 0C   IAddImm              r4, r4, 12    r4 = RET_darray-at + 12 -> <RET_(null)>
    0031   0x000338   4E 04 04 00   LoadU16              r4, [r4]      r4 = *(u16*)RET_darray-at
    0032   0x000340   0C 05 04 00   LoadU16Imm           r5, 4         r5 = 4
    0033   0x000348   1D 04 04 05   IEqual               r4, r4, r5    r4 = r4 == r5
    0034   0x000350   2F 39 04 00   BranchIfNot          r4, 0x39      IF NOT r4 => L_4
    0035   0x000358   43 04 03 00   Move                 r4, r3        r4 = RET_darray-at
    0036   0x000360   5D 04 00 00   AssertPointer        r4            r4 != nullptr
    0037   0x000368   51 04 04 00   LoadU64              r4, [r4]      r4 = *(u64*)RET_darray-at
    0038   0x000370   2D 3A 00 00   Branch               0x3A          GOTO => L_2
  L_4:
    0039   0x000378   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
  L_2:
    003A   0x000380   43 03 04 00   Move                 r3, r4        r3 = 0
    003B   0x000388   15 04 04 00   LookupPointer        r4, 4         r4 = ST[4] -> <npc-alive?>
    003C   0x000390   43 05 03 00   Move                 r5, r3        r5 = 0
    003D   0x000398   43 31 05 00   Move                 r49, r5       r49 = 0
    003E   0x0003A0   1C 04 04 01   CallFf               r4, r4, 1     r4 = npc-alive?(0)
    003F   0x0003A8   2F 4E 04 00   BranchIfNot          r4, 0x4E      IF NOT r4 => L_5
    0040   0x0003B0   15 04 05 00   LookupPointer        r4, 5         r4 = ST[5] -> <script-in-state?>
    0041   0x0003B8   43 05 03 00   Move                 r5, r3        r5 = 0
    0042   0x0003C0   4A 06 06 00   LoadStaticU64Imm     r6, 6         r6 = ST[6] -> <unchained>
    0043   0x0003C8   43 31 05 00   Move                 r49, r5       r49 = 0
    0044   0x0003D0   43 32 06 00   Move                 r50, r6       r50 = unchained
    0045   0x0003D8   1B 04 04 02   Call                 r4, r4, 2     r4 = script-in-state?(0, unchained)
    0046   0x0003E0   30 04 04 00   OpLogNot             r4, r4        r4 = !RET_script-in-state?
    0047   0x0003E8   2F 4C 04 00   BranchIfNot          r4, 0x4C      IF NOT r4 => L_6
    0048   0x0003F0   0C 04 01 00   LoadU16Imm           r4, 1         r4 = 1
    0049   0x0003F8   43 00 04 00   Move                 r0, r4        r0 = 1
    004A   0x000400   43 04 00 00   Move                 r4, r0        r4 = 1
    004B   0x000408   2D 4D 00 00   Branch               0x4D          GOTO => L_7
  L_6:
    004C   0x000410   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
  L_7:
    004D   0x000418   2D 4F 00 00   Branch               0x4F          GOTO => L_8
  L_5:
    004E   0x000420   0C 04 00 00   LoadU16Imm           r4, 0         r4 = 0
  L_8:
    004F   0x000428   43 04 01 00   Move                 r4, r1        r4 = 0
    0050   0x000430   0C 05 01 00   LoadU16Imm           r5, 1         r5 = 1
    0051   0x000438   01 04 04 05   IAdd                 r4, r4, r5    r4 = 0 + 1
    0052   0x000440   43 01 04 00   Move                 r1, r4        r1 = 0
    0053   0x000448   43 04 01 00   Move                 r4, r1        r4 = 0
    0054   0x000450   43 03 04 00   Move                 r3, r4        r3 = 0
    0055   0x000458   2D 08 00 00   Branch               0x8           GOTO => L_9
  L_0:
    0056   0x000460   15 03 07 00   LookupPointer        r3, 7         r3 = ST[7] -> <end-foreach>
    0057   0x000468   1C 03 03 00   CallFf               r3, r3, 0     r3 = end-foreach()
    0058   0x000470   43 01 03 00   Move                 r1, r3        r1 = RET_end-foreach
    0059   0x000478   43 01 00 00   Move                 r1, r0        r1 = 1
    005A   0x000480   00 01 01 00   Return               r1            Return 
    005B   0x000488   43 00 01 00   Move                 r0, r1        r0 = 1
    005C   0x000490   00 00 00 00   Return               r0            Return 

    SYMBOL TABLE: 
    0000   0x000498    function: darray-count
    0001   0x0004A0    sid: #029902C61617476E
    0002   0x0004A8    function: begin-foreach
    0003   0x0004B0    function: darray-at
    0004   0x0004B8    function: npc-alive?
    0005   0x0004C0    function: script-in-state?
    0006   0x0004C8    sid: unchained
    0007   0x0004D0    function: end-foreach
}


##############################  ENTRY 5  ##############################

vox-san-pool-outside-npcs = d-array [0x004E0] {
  [0] array* [0x00588] {
    [0] anonymous array [0x588] {size: 6} {
      [0] anonymous struct [0x6f0] {
        [0] boxed-symbol [0x00D38] {
          [0] sid: npc-san-pool-outside-1
          [1] int: 0
          [2] int: 7
        }
      }
      [1] anonymous struct [0x6f6] {
        [0] boxed-symbol [0x00D38] {
          [0] sid: npc-san-pool-outside-1
          [1] int: 0
          [2] int: 7
        }
      }
      [2] anonymous struct [0x6fc] {
        [0] boxed-symbol [0x00BD8] {
          [0] sid: npc-san-pool-outside-2
          [1] int: 0
          [2] int: 7
        }
      }
      [3] anonymous struct [0x702] {
        [0] boxed-symbol [0x00DA8] {
          [0] sid: npc-san-pool-outside-3
          [1] int: 0
          [2] int: 7
        }
      }
      [4] anonymous struct [0x708] {
        [0] boxed-symbol [0x008D0] {
          [0] sid: npc-san-pool-outside-4
          [1] int: 0
          [2] int: 7
        }
      }
      [5] anonymous struct [0x70e] {
        [0] boxed-symbol [0x008D0] {
          [0] sid: npc-san-pool-outside-4
          [1] int: 0
          [2] int: 7
        }
      }
    }
    [1] int: 6
    [2] int: -1
  }
  [1] int: 0
  [2] int: 0
}

#pragma endian little

#include "std/string"

using sid64 = u64;

const bool is_t1x = false;


fn to_sid64(ref str name) {
    u64 base = 0xCBF29CE484222325;
    for (u64 i = 0, i < std::string::length(name), i = i + 1) {
        base = 0x100000001B3 * (base ^ name[i]);
    }
    return base;
};

//out_sid = to_sid64(in_str);

enum TypeId : u64 {
    StateScript = to_sid64("state-script"),
    Float = to_sid64("float"),
    UInt64 = to_sid64("uint64"),
    Vector = to_sid64("vector"),
    Symbol = to_sid64("symbol"),
    Boolean = to_sid64("boolean"),
    ScriptLambda = to_sid64("script-lambda"),
};

enum Opcode : u8 {
    Return,
    IAdd,
    ISub,
    IMul,
    IDiv,
    FAdd,
    FSub,
    FMul,
    FDiv,
    LoadStaticInt,
    LoadStaticFloat,
    LoadStaticPointer,
    LoadU16Imm,
    LoadU32,
    LoadFloat,
    LoadPointer,
    StoreInt,
    StoreFloat,
    StorePointer,
    LookupInt,
    LookupFloat,
    LookupPointer,
    MoveInt,
    MoveFloat,
    MovePointer,
    CastInteger,
    CastFloat,
    Call,
    CallFf,
    IEqual,
    IGreaterThan,
    IGreaterThanEqual,
    ILessThan,
    ILessThanEqual,
    FEqual,
    FGreaterThan,
    FGreaterThanEqual,
    FLessThan,
    FLessThanEqual,
    IMod,
    FMod,
    IAbs,
    FAbs,
    GoTo,
    Label,
    Branch,
    BranchIf,
    BranchIfNot,
    OpLogNot,
    OpBitAnd,
    OpBitNot,
    OpBitOr,
    OpBitXor,
    OpBitNor,
    OpLogAnd,
    OpLogOr,
    INeg,
    FNeg,
    LoadParamCnt,
    IAddImm,
    ISubImm,
    IMulImm,
    IDivImm,
    LoadStaticI32Imm,
    LoadStaticFloatImm,
    LoadStaticPointerImm,
    IntAsh,
    Move,
    LoadStaticU32Imm,
    LoadStaticI8Imm,
    LoadStaticU8Imm,
    LoadStaticI16Imm,
    LoadStaticU16Imm,
    LoadStaticI64Imm,
    LoadStaticU64Imm,
    LoadI8,
    LoadU8,
    LoadI16,
    LoadU16,
    LoadI32,
    LoadI64,
    LoadU64,
    StoreI8,
    StoreU8,
    StoreI16,
    StoreU16,
    StoreI32,
    StoreU32,
    StoreI64,
    StoreU64,
    INotEqual,
    FNotEqual,
    StoreArray,
    AssertPointer,
    BreakFlag,
    Breakpoint,
};

struct Instruction {
    Opcode m_opcode;
    u8 m_arg0;
    u8 m_arg1;
    u8 m_arg2;
    padding[4];
};

struct vector {
    float x, y, z, w;
};

struct ScriptLambda {
    u64 m_pInstruction;
    u64 m_pSymbols;
    sid64 m_typeId;
    u64 m_sum;
    sid64 m_funcName;
    u64 m_instructionFlag;
    u8 _padding0[4];
    u32 m_numInstructions;
    s64 m_neg1;
    u64 m_sidGlobal;
    u8 _padding1[8];
    
    Instruction instructions[m_numInstructions] @ m_pInstruction;
    u64 symbols[(m_sum - 4 * m_numInstructions - 12) / 4] @ m_pSymbols;
};

struct SsLambda {
    u64 m_pScriptLambda;
    u64 m_someSortOfCounter;

    if (m_pScriptLambda != 0) {
        ScriptLambda scriptLambda @ m_pScriptLambda;
    }
};

struct SsTrack {
    sid64 m_trackId;
    u16 m_trackIdx;
    s16 m_totalLambdaCount;
    u8 _padding0[4];
    u64 m_pSsLambda;

    if (m_pSsLambda != 0) {
        SsLambda lambdas[m_totalLambdaCount] @ m_pSsLambda;
    }
};

struct SsTrackGroup {
    u8 _padding0[8];
    u16 m_totalLambdaCount;
    s16 m_numTracks;
    u8 _padding1[4];
    u64 m_aTracks;
    u64 m_name;
    u8 _padding2[16];
    u64 m_rareScriptLambda;

    SsTrack tracks[m_numTracks] @ m_aTracks;
    if (m_name != 0) {
        char track_name[] @ m_name;
    }
};

struct SsOnBlock {
    s32 m_blockType;
    u8 _padding0[4];
    sid64 m_blockEventId;
    u64 m_pScriptLambda;
    
    SsTrackGroup m_trackGroup;
    
    if (m_pScriptLambda != 0) {
        ScriptLambda scriptLambda @ m_pScriptLambda;
    }
};

struct SsState {
    sid64 m_stateId;
    s64 m_numSsOnBlocks;
    u64 m_pSsOnBlocks;

    SsOnBlock onBlocks[m_numSsOnBlocks] @ m_pSsOnBlocks;
};

using StateScript;

struct TypedValue {
    u64 m_typeId;
    match (m_typeId) {
        (TypeId::StateScript): StateScript script;
        (TypeId::Float): float value;
        (TypeId::UInt64): u64 value;
        (TypeId::Vector): vector value;
        (TypeId::Symbol): sid64 value;
        (TypeId::Boolean): bool value;
        (TypeId::ScriptLambda): ScriptLambda value;
    }
};

struct SsDeclaration {
    sid64 m_declId;
    u64 m_declIdString;
    sid64 m_declTypeId;
    u16 m_varSizeSum;
    u16 m_isVar;
    u8 _padding0[4];
    u64 m_pDeclValue;
    u8 _padding1[8];
    
    if (m_declIdString != 0) {
        char decl_string[] @ m_declIdString;
    }
    
    if (m_pDeclValue) {
        TypedValue value @ m_pDeclValue - 8;
    }
};

struct SsDeclarationList {
    u32 m_totalDeclarationSize;
    u32 m_numDeclarations;
    u64 m_pDeclarations;

    SsDeclaration declarations[m_numDeclarations] @ m_pDeclarations;
};

struct SymbolArray {
    s32 m_numEntries;
    u8 _padding0[4];
    u64 m_pSymbols;
    
    if (m_pSymbols) {
       sid64 symbols[m_numEntries] @ m_pSymbols;    
    }
};

struct SsOptions {
    u64 m_optionString;
    u64 m_unknownFlags;
    u8 _padding0[8];
    u64 m_pSymbolArray;
    u8 _padding1[24];
    u32 m_always5;
    u32 m_mostly0;
    u64 m_mostly0Rarely1;
    u8 _padding2[8];
    
    if (m_pSymbolArray) {
        SymbolArray symbolArray @ m_pSymbolArray;
    }
  
    if (m_optionString) {
        char options_string[] @ m_optionString;    
    }
};

struct StateScript {
    if (is_t1x) {
        u64 m_pStateScriptName;
    }
    sid64 m_stateScriptId;
    u64 m_pSsDeclList;
    sid64 m_initialStateId;
    u64 m_pSsOptions;
    u8 _padding0[8];
    u64 m_pSsStateTable;
    s16 m_stateCount;
    s16 m_line;
    u8 _padding1[4];
    u64 m_pDebugFileName;
    u64 m_pErrorName;
    u8 _padding2[8];
    
    if (is_t1x) {
        char name [] @ m_pStateScriptName;
    }
    
   if (m_pSsDeclList) {
       SsDeclarationList declList @ m_pSsDeclList;
   }
   
    SsState states[m_stateCount] @ m_pSsStateTable;
    
    if (m_pSsOptions) SsOptions options @ m_pSsOptions;
    
    if (m_pDebugFileName) {
       char debug_name[] @ m_pDebugFileName;
    }
};


struct Entry {
    sid64 m_nameID;
    sid64 m_typeId;
    u64   m_entryPtr;
    
    if (m_entryPtr) {
        TypedValue value @ m_entryPtr - 8;
    }
};

bitfield SymbolTableBits {
    chunk0 : 1;
    chunk1 : 1;
    chunk2 : 1;
    chunk3 : 1;
    chunk4 : 1;
    chunk5 : 1;
    chunk6 : 1;
    chunk7 : 1;
};

struct SymbolTableByte {
    u8 m_raw;
    SymbolTableBits m_bits @ $ - 1;
};

struct SymbolTable {
    u32 m_size;
    SymbolTableByte bytes[m_size];
};

struct SymbolLinkedChunk {
    u64 chunk [[hidden]];
    u8 symbolByte @ header.m_textSize + 4 + (($ - 8) / 64);
    if (((symbolByte >> ((($ - 8) / 8) % 8)) & 1) != 0) {
        u64 pointer_chunk @ $ - 8;
    }
};

struct DC_Header {
    padding[4];
    u32   m_versionNumber;
    u32   m_textSize;
    u32   m_stringsOffset;
    u32   field_10;
    u32   m_numEntries;
    u64   m_pStartOfData;
    
    //Entry entries[m_numEntries] @ m_pStartOfData; 
    //SymbolTable symbolTable @ m_textSize;
};

DC_Header header @ 0x0;
//SymbolLinkedChunk linkedChunks[(header.m_textSize + 7) / 8] @ 0x0;
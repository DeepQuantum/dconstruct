#pragma once
#ifndef DCSCRIPT_H
#define DCSCRIPT_H

// Thanks to icemesh

#include "base.h"

namespace dconstruct {
    struct SsDeclarationList;
    struct SsDeclaration;
    struct StateScript;
    struct SsOptions;
    struct SymbolArray;
    struct SsState;
    struct SsTrackGroup;
    struct SsOnBlock;
    struct SsTrack;
    struct SsLambda;
    struct ScriptLambda;

    struct StateScript //0x50
    {
        sid64				m_stateScriptId;		///< <c>0x00</c>: StringId64 of the script name
        SsDeclarationList* m_pSsDeclList;			///< <c>0x08</c>: ptr to the declaration list
        sid64				m_initialStateId;		///< <c>0x10</c>: StringId64 of the initial state
        SsOptions* m_pSsOptions;			///< <c>0x18</c>: ptr to the SsOptions
        uint64_t				field_20;				///< <c>0x20</c>: always 0 ?
        SsState* m_pSsStateTable;		///< <c>0x28</c>: ptr to the SsState Table
        int16_t					m_stateCount;			///< <c>0x30</c>: number of states
        //start of debug info
        int16_t					m_line;					///< <c>0x32</c>: this is a line number thats get displayed in the debug display
        uint32_t				field_34;				///< <c>0x34</c>: padding probably
        const char* m_pDebugFileName;		///< <c>0x38</c>: the debug filename eg: t2r/src/game/scriptx/ss/ss-tag-as-hero.dcx
        const char* m_pErrorName;			///< <c>0x40</c>: used to store any error if any
        uint64_t				m_padding;				///< <c>0x48</c>: padding probably
    };

    struct SsDeclarationList //0x10
    {
        int32_t					m_unkNumber;			///< <c>0x00</c>: unk number
        int32_t					m_numDeclarations;		///< <c>0x04</c>: number of declared vars in the table
        SsDeclaration* m_pDeclarations;		///< <c>0x08</c>: ptr to the list of declarations
    };

    struct SsDeclaration //0x30
    {
        sid64				m_declId;				///< <c>0x00</c>: StringId of the declaration name
        uint64_t				m_padding;				///< <c>0x08</c>: padding probably
        sid64				m_declTypeId;			///< <c>0x10</c>: StringId of the declaration type eg: boolean, int32, float etc..
        int16_t					m_unk;					///< <c>0x18</c>: unk number
        int16_t					m_isVar;				///< <c>0x1A</c>: is variable ?
        uint32_t				unk3;					///< <c>0x1C</c>: always 0 ?
        void* m_pDeclValue;			///< <c>0x20</c>: ptr to the declaration value
        uint64_t				m_unkNumber;			///< <c>0x28</c>: always 0x80 ?
    };

    struct SsOptions //0x50
    {
        uint8_t					m_imLazy[0x18];			///< <c>0x00</c>: self explanatory
        SymbolArray* m_pSymbolArray;			///< <c>0x18</c>: ptr to the symbol array
        uint8_t					m_imLazyPt2[0x18];		///< <c>0x20</c>: self explanatory always 0 ?
        int64_t					m_unkNumber;			///< <c>0x38</c>: unk number
        uint8_t					m_imLazyPt3[0x10];		///< <c>0x20</c>: self explanatory always 0 ?
    };

    struct SymbolArray //0x10
    {
        int32_t					m_numEntries;			///< <c>0x00</c>: number of entries
        uint32_t				m_unk;					///< <c>0x04</c>: always 0 ?
        sid64* m_pSymbols;				///< <c>0x08</c>: ptr to the symbols
    };

    struct SsState //0x18
    {
        sid64				m_stateId;				///< <c>0x00</c>: StringId64 of the state name
        int64_t					m_numSsOnBlocks;		///< <c>0x08</c>: numSsOnBlocks
        SsOnBlock* m_pSsOnBlocks;			///< <c>0x10</c>: ptr to the SsOnBlocks table
    };

    struct SsTrackGroup
    {
        uint64_t				field_0;                ///< <c>0x00</c>:
        uint16_t				m_totalLambdaCount;	///< <c>0x08</c>: unk number
        int16_t					m_numTracks;			///< <c>0x0A</c>: num tracks
        uint32_t				m_padding;				///< <c>0x1C</c>: padding probably
        SsTrack* m_aTracks;				///< <c>0x10</c>: ptr to an array of Tracks
        const char* m_name;					///< <c>0x18</c>: eg: ss-fp-test initial (on (start))
        uint64_t				field_20;               ///< <c>0x20</c>:
        uint64_t				field_28;               ///< <c>0x28</c>:
        uint64_t				field_30;               ///< <c>0x30</c>:
    };

    struct SsOnBlock //
    {
        int32_t					m_blockType;			///< <c>0x00</c>: //on start || on update || on event etc
        uint32_t				m_unkNumber;			///< <c>0x04</c>: unk number
        sid64				m_blockEventId;			///< <c>0x08</c>: UNSURE. Can be null. if its null there's no script lambda ptr
        void* m_pScriptLambda;		///< <c>0x10</c>: ptr to the script Lambda
        SsTrackGroup            m_trackGroup;
    };

    struct SsTrack //0x18
    {
        sid64				m_trackId;				///< <c>0x00</c>: StringId64 of the track name
        uint16_t				field_8;				///< <c>0x08</c>: always 0?
        int16_t					m_totalLambdaCount;		///< <c>0x0A</c>: unsure
        uint32_t				m_padding;				///< <c>0x0C</c>: padding probably
        SsLambda* m_pSsLambda;			///< <c>0x10</c>: ptr to the SsLambda table
    };

    struct SsLambda //0x10
    {
        ScriptLambda* m_pScriptLambda;		///< <c>0x00</c>: ptr to the script lambda of the track
        uint64_t				m_unkNumber;			///< <c>0x08</c>: UNSURE: size maybe ?
    };

    struct ScriptLambda //0x50
    {
        u64* m_pInstruction;				                ///< <c>0x00</c>: ptr to the first opcode of the script
        u64*         m_pSymbols;				            ///< <c>0x08</c>: ptr to the symbol table
        sid64	     m_typeId;				///< <c>0x10</c>: usually SID("function")
        u64			 m_unknown;			///< <c>0x18</c>: no idea what this is used for
        u64			 m_unk;					///< <c>0x20</c>: always 0?
        u64			 m_instructionFlag;		///< <c>0x28</c>: 0xDEADBEEF1337F00D
        u32			 m_always0;					///< <c>0x2C</c>: always 0 ?
        u32			 m_numInstructions;			///< <c>0x34</c>: alwasy 0x10 ?
        i64          m_neg1;
        u64		     m_sidGlobal;				///< <c>0x38</c>: Unknown StringId64
        u64          m_always0_2;
    };
    constexpr auto x = sizeof(ScriptLambda);

#endif // DCSCRIPT_H

}


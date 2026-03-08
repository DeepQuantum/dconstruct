/******************************************************************************/
/*
  Author  - icemesh
*/
/******************************************************************************/
#pragma once
#include <stdint.h>

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

typedef uint32_t StringId;

struct Entry;

struct DC_Header
{
	uint32_t				m_magic;				///< <c>0x00</c>: magic 0x44433030 -> DC00
	uint32_t				m_versionNumber;		///< <c>0x04</c>: always 0x1
	uint32_t				m_textSize;				///< <c>0x08</c>: size from 0x0C to (0xC+m_TextSize)
	uint32_t				unk;					///< <c>0x0C</c>: always 0 
	uint32_t				unk1;					///< <c>0x10</c>: always 1
	int32_t					m_numEntries;			///< <c>0x14</c>: num of entries
	Entry*					m_pStartOfData;			///< <c>0x18</c>: ptr to the start of data/state script(s) 
};

struct Entry //0x10
{
	StringId				m_scriptId;				///< <c>0x00</c>: StringId of the script name
	StringId				m_typeId;				///< <c>0x04</c>: StringId of the script type eg SID("state-script")
	void*					m_entryPtr;				///< <c>0x08</c>: ptr to the scriptType cast this to the IdGroup || StateScript etc..
};

struct StateScript //0x5C
{
	//probably the StringIds are aligned to 8 bytes
	StringId				m_stateScriptId;		///< <c>0x00</c>: StringId of the script name eg SID("ss-mad-volcano-ascent-ruins-combat")
	uint32_t				unk;					///< <c>0x04</c>: stringId  padding 
	SsDeclarationList*		m_pSsDeclList;			///< <c>0x08</c>: ptr to a StringId
	StringId				m_initialStateId;		///< <c>0x10</c>: StringId of the name of the initial state
	uint32_t				unk1;					///< <c>0x14</c>: always 0 ?
	SsOptions*				m_pSsOptions;			///< <c>0x18</c>: ptr to the SsOptions
	uint64_t				unk2;					///< <c>0x20</c>: always 0 ?
	uint64_t				m_hasError;				///< <c>0x28</c>: used to check if the script has some error
	SsState*				m_pSsStateTable;		///< <c>0x30</c>: ptr to the SsState Table
	int16_t					m_numStates;			///< <c>0x38</c>: number of states
	//start of debug info
	int16_t					m_line;					///< <c>0x3A</c>: this is a line number thats get displayed in the debug display
	uint32_t				unk4;					///< <c>0x3C</c>: padding probably
	const char*				m_pDebugFileName;		///< <c>0x40</c>: the debug filename 
	const char*				m_pErrorName;			///< <c>0x48</c>: used to store any error if any
	uint8_t					m_padding[0xC];			///< <c>0x50</c>: 
};

struct SsDeclarationList //0x10 
{
	int32_t					m_unkNum1;				///< <c>0x00</c>: unk number
	int32_t					m_numDeclarations;		///< <c>0x04</c>: number of declared vars in the table
	SsDeclaration*			m_pDeclarations;		///< <c>0x08</c>: ptr to the list of declarations
	uint32_t				unk2;					///< <c>0x10</c>: padding probably
};

struct SsDeclaration //0x18
{
	StringId				m_declId;				///< <c>0x00</c>: StringId of the declaration name
	StringId				m_declTypeId;			///< <c>0x04</c>: StringId of the declaration type eg: boolean, int32 etc..
	int16_t					m_unk;					///< <c>0x08</c>: unk number
	int16_t					m_isVar;				///< <c>0x0A</c>: is variable ?
	uint32_t				unk3;					///< <c>0x0C</c>: always 0 ?
	void*					m_pDeclValue;			///< <c>0x10</c>: ptr to the declaration value
};

struct SsOptions //0x3C UNSURE
{
	uint8_t					m_imLazy[0x20];			///< <c>0x00</c>: maybe always 0 maybe not
	SymbolArray*			m_pSymbolArray;			///< <c>0x20</c>: ptr to the symbol array
	int32_t					m_unkNum;				///< <c>0x28</c>: unk Number
	uint8_t					m_imLazyPt2[0x10];		///< <c>0x2C</c>: maybe always 0 maybe not
};

struct SymbolArray //0x14
{
	int32_t					m_numEntries;			///< <c>0x00</c>: number of entries probably
	uint32_t				unk;					///< <c>0x04</c>: always 0 ?
	StringId*				m_pSymbols;				///< <c>0x08</c>: ptr to symbol. TODO: check if they're all sids
	uint32_t				unk2;					///< <c>0x10</c>: padding probably
};

struct SsState //0x10
{
	StringId				m_nameId;				///< <c>0x00</c>:  StringId of the state name 
	int32_t					m_numSsOnBlocks;		///< <c>0x04</c>:  numTracks
	SsOnBlock*				m_pSsOnBlocks;			///< <c>0x08</c>:  ptr to the SsOnBlocks
};

struct SsOnBlock //size 0x38
{
	int32_t					m_blockType;			///< <c>0x00</c>:  //on start || on update || on event etc
	StringId				m_blockId;				///< <c>0x04</c>:  UNSURE. Can be null. if its null there's no script lambda ptr
	void*					m_pScriptLambda;		///< <c>0x08</c>:  ptr to the script Lambda
	uint16_t				m_unk;					///< <c>0x10</c>:  unk number
	int16_t					m_numTracks;			///< <c>0x12</c>:  num tracks
	uint32_t				m_unk3;					///< <c>0x14</c>:  always 0?
	SsTrack*				m_pTrack;				///< <c>0x18</c>:  ptr to a track
	const char*				m_name;					///< <c>0x20</c>:  eg: ss-asoria-player-test initial (on (start))
	uint8_t					m_imLazy[0x10];			///< <c>0x28</c>:  always 0?
};

struct SsTrack //0x10
{
	StringId				m_trackId;				///< <c>0x00</c>:  StringId of the track name
	uint16_t				m_unk;					///< <c>0x04</c>:  always 0?
	int16_t					m_totalLambdaCount;		///< <c>0x06</c>:  unsure
	SsLambda*				m_pSsLambda;			///< <c>0x08</c>:  ptr to the SsLambda table
};

struct SsLambda //0x14
{
	ScriptLambda*			m_pScriptLambda;		///< <c>0x00</c>:  ptr to the script lambda of the track
	uint64_t				m_unkNumber;			///< <c>0x08</c>:  unk number maybe size ?
};

struct ScriptLambda //0x24 || 0x20 if its an header entry
{
	uint8_t*				m_pOpcode;				///< <c>0x00</c>:  ptr to the first opcode of the script
	uint32_t*				m_pSymbols;				///< <c>0x08</c>:  ptr to the symbol table
	StringId                m_typeId;				///< <c>0x10</c>:  usually SID("function")
	uint64_t				m_unkNumber;			///< <c>0x14</c>:  no idea what this is used for
	uint64_t				m_unk;					///< <c>0x1C</c>:  always 0 ?
};
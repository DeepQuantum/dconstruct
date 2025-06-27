#pragma once
#ifndef DCHEADER_H
#define DCHEADER_H

#include "base.h"

// Thanks to icemesh!

namespace dconstruct {

    struct Entry
    {
        sid64			    m_scriptId;				///< <c>0x00</c>: StringId64 of the script name
        sid64				m_typeId;				///< <c>0x08</c>: StringId64 of the script type eg SID("state-script")
        const void* m_entryPtr;				///< <c>0x10</c>: ptr to the scriptType cast this to the IdGroup || StateScript etc..
    };

    struct DC_Header
    {
        uint32_t				m_magic;				///< <c>0x00</c>: magic 0x44433030 -> DC00
        uint32_t				m_versionNumber;		///< <c>0x04</c>: always 0x1
        uint32_t				m_textSize;				///< <c>0x08</c>: size from 0x0C to (0xC+m_TextSize)
        uint32_t				m_stringsOffset;		///< <c>0x0C</c>: strings offset ?

        uint32_t				field_10;				///< <c>0x10</c>: always 1
        int32_t					m_numEntries;			///< <c>0x14</c>: num of entries
        Entry* m_pStartOfData;			///< <c>0x18</c>: ptr to the start of data/state script(s)
    };

    
}



#endif // DCHEADER_H

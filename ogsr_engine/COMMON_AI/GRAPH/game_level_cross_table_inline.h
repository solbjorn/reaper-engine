////////////////////////////////////////////////////////////////////////////
//	Module 		: game_level_cross_table_inline.h
//	Created 	: 20.02.2003
//  Modified 	: 13.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Cross table between game and level graphs inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC CGameLevelCrossTable::CGameLevelCrossTable(LPCSTR fName)
{
    m_tpCrossTableVFS = XR_ASSERT_VAL(FS.r_open(fName) != nullptr, "can't open level cross table", fName);

    IReader* chunk = XR_ASSERT_VAL(m_tpCrossTableVFS->open_chunk(CROSS_TABLE_CHUNK_VERSION) != nullptr, "level cross table is corrupted", fName);
    chunk->r(&m_tCrossTableHeader, sizeof(m_tCrossTableHeader));
    chunk->close();

    XR_ASSERT(m_tCrossTableHeader.version() == XRAI_CURRENT_VERSION, "level cross table version mismatch", fName);

    m_chunk = XR_ASSERT_VAL(m_tpCrossTableVFS->open_chunk(CROSS_TABLE_CHUNK_DATA) != nullptr, "level cross table is corrupted", fName);
    m_tpaCrossTable = (const CCell*)m_chunk->pointer();
}

IC CGameLevelCrossTable::CGameLevelCrossTable(const void* buffer, u32)
{
    memcpy(&m_tCrossTableHeader, buffer, sizeof(m_tCrossTableHeader));
    buffer = (const u8*)buffer + sizeof(m_tCrossTableHeader);

    XR_ASSERT(m_tCrossTableHeader.version() == XRAI_CURRENT_VERSION, "level cross table version mismatch");

    m_tpaCrossTable = (const CCell*)buffer;
    m_chunk = nullptr;
    m_tpCrossTableVFS = nullptr;
}

IC CGameLevelCrossTable::~CGameLevelCrossTable()
{
    if (m_chunk)
        m_chunk->close();

    FS.r_close(m_tpCrossTableVFS);
}

IC const CGameLevelCrossTable::CCell& CGameLevelCrossTable::vertex(u32 level_vertex_id) const
{
    return m_tpaCrossTable[XR_ASSERT_VAL(level_vertex_id < header().level_vertex_count())];
}

IC u32 CGameLevelCrossTable::CHeader::version() const { return (dwVersion); }

IC u32 CGameLevelCrossTable::CHeader::level_vertex_count() const { return (dwNodeCount); }

IC u32 CGameLevelCrossTable::CHeader::game_vertex_count() const { return (dwGraphPointCount); }

IC const xrGUID& CGameLevelCrossTable::CHeader::level_guid() const { return (m_level_guid); }

IC const xrGUID& CGameLevelCrossTable::CHeader::game_guid() const { return (m_game_guid); }

IC GameGraph::_GRAPH_ID CGameLevelCrossTable::CCell::game_vertex_id() const { return (tGraphIndex); }

IC float CGameLevelCrossTable::CCell::distance() const { return (fDistance); }

IC const CGameLevelCrossTable::CHeader& CGameLevelCrossTable::header() const { return (m_tCrossTableHeader); }

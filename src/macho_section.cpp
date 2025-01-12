// macho_section.cpp
#include "macho_section.h"

MachoSection::MachoSection(const LIEF::MachO::Section& section) {
    m_name = section.name();
    m_segment_name = section.segment_name();
    m_offset = section.offset();
    m_size = section.size();
    m_virtual_address = section.virtual_address();
    m_flags = static_cast<uint32_t>(section.flags());
}

nlohmann::json MachoSection::to_json() const {
    nlohmann::json j;
    j["name"] = m_name;
    j["segment_name"] = m_segment_name;
    j["offset"] = m_offset;
    j["size"] = m_size;
    j["virtual_address"] = m_virtual_address;
    j["flags"] = m_flags;

    // Add human-readable flags
    j["flags_detail"] = nlohmann::json::object();

    // Use the proper flags from LIEF::MachO::Section::FLAGS
    using SectionFlags = LIEF::MachO::Section::FLAGS;

    if (has_flag(static_cast<uint32_t>(SectionFlags::DEBUG_INFO))) {
        j["flags_detail"]["DEBUG_INFO"] = true;
    }
    if (has_flag(static_cast<uint32_t>(SectionFlags::PURE_INSTRUCTIONS))) {
        j["flags_detail"]["PURE_INSTRUCTIONS"] = true;
    }
    if (has_flag(static_cast<uint32_t>(SectionFlags::SOME_INSTRUCTIONS))) {
        j["flags_detail"]["SOME_INSTRUCTIONS"] = true;
    }
    if (has_flag(static_cast<uint32_t>(SectionFlags::NO_DEAD_STRIP))) {
        j["flags_detail"]["NO_DEAD_STRIP"] = true;
    }
    if (has_flag(static_cast<uint32_t>(SectionFlags::LIVE_SUPPORT))) {
        j["flags_detail"]["LIVE_SUPPORT"] = true;
    }
    if (has_flag(static_cast<uint32_t>(SectionFlags::SELF_MODIFYING_CODE))) {
        j["flags_detail"]["SELF_MODIFYING_CODE"] = true;
    }
    if (has_flag(static_cast<uint32_t>(SectionFlags::EXT_RELOC))) {
        j["flags_detail"]["EXT_RELOC"] = true;
    }
    if (has_flag(static_cast<uint32_t>(SectionFlags::LOC_RELOC))) {
        j["flags_detail"]["LOC_RELOC"] = true;
    }
    if (has_flag(static_cast<uint32_t>(SectionFlags::STRIP_STATIC_SYMS))) {
        j["flags_detail"]["STRIP_STATIC_SYMS"] = true;
    }
    if (has_flag(static_cast<uint32_t>(SectionFlags::NO_TOC))) {
        j["flags_detail"]["NO_TOC"] = true;
    }

    return j;
}

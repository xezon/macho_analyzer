// macho_segment.cpp
#include "macho_segment.h"

MachoSegment::MachoSegment(const LIEF::MachO::SegmentCommand& segment) {
    m_name = segment.name();
    m_virtual_address = segment.virtual_address();
    m_virtual_size = segment.virtual_size();
    m_file_offset = segment.file_offset();
    m_file_size = segment.file_size();
    m_max_protection = segment.max_protection();
    m_init_protection = segment.init_protection();
}

nlohmann::json MachoSegment::to_json() const {
    nlohmann::json j;
    j["name"] = m_name;
    j["virtual_address"] = m_virtual_address;
    j["virtual_size"] = m_virtual_size;
    j["file_offset"] = m_file_offset;
    j["file_size"] = m_file_size;
    j["max_protection"] = m_max_protection;
    j["init_protection"] = m_init_protection;
    return j;
}

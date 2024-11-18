// macho_segment.cpp
#include "macho_segment.h"

MachoSegment::MachoSegment(const LIEF::MachO::SegmentCommand& segment) {
    name_ = segment.name();
    virtual_address_ = segment.virtual_address();
    virtual_size_ = segment.virtual_size();
    file_offset_ = segment.file_offset();
    file_size_ = segment.file_size();
    max_protection_ = segment.max_protection();
    init_protection_ = segment.init_protection();
}

nlohmann::json MachoSegment::to_json() const {
    nlohmann::json j;
    j["name"] = name_;
    j["virtual_address"] = virtual_address_;
    j["virtual_size"] = virtual_size_;
    j["file_offset"] = file_offset_;
    j["file_size"] = file_size_;
    j["max_protection"] = max_protection_;
    j["init_protection"] = init_protection_;
    return j;
}
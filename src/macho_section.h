// macho_section.h
#pragma once

#include <LIEF/LIEF.hpp>
#include <LIEF/MachO.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

class MachoSection {
public:
    explicit MachoSection(const LIEF::MachO::Section& section);
    virtual ~MachoSection() = default;

    nlohmann::json to_json() const;

    // Getters
    std::string get_name() const { return name_; }
    std::string get_segment_name() const { return segment_name_; }
    uint64_t get_offset() const { return offset_; }
    uint64_t get_size() const { return size_; }
    uint64_t get_virtual_address() const { return virtual_address_; }
    uint32_t get_flags() const { return flags_; }

    // Helper methods for flag checking
    bool has_flag(uint32_t flag) const { return (flags_ & flag) == flag; }

private:
    std::string name_;
    std::string segment_name_;
    uint64_t offset_;
    uint64_t size_;
    uint64_t virtual_address_;
    uint32_t flags_;
};
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

    std::string get_name() const { return m_name; }
    std::string get_segment_name() const { return m_segment_name; }
    uint64_t get_offset() const { return m_offset; }
    uint64_t get_size() const { return m_size; }
    uint64_t get_virtual_address() const { return m_virtual_address; }
    uint32_t get_flags() const { return m_flags; }

    // Helper methods for flag checking
    bool has_flag(uint32_t flag) const { return (m_flags & flag) == flag; }

private:
    std::string m_name;
    std::string m_segment_name;
    uint64_t m_offset;
    uint64_t m_size;
    uint64_t m_virtual_address;
    uint32_t m_flags;
};

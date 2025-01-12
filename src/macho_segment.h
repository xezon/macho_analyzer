// macho_segment.h
#pragma once

#include <LIEF/LIEF.hpp>
#include <LIEF/MachO.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

class MachoSegment {
public:
    explicit MachoSegment(const LIEF::MachO::SegmentCommand& segment);
    virtual ~MachoSegment() = default;

    nlohmann::json to_json() const;

    std::string get_name() const { return m_name; }
    uint64_t get_virtual_address() const { return m_virtual_address; }
    uint64_t get_virtual_size() const { return m_virtual_size; }
    uint64_t get_file_offset() const { return m_file_offset; }
    uint64_t get_file_size() const { return m_file_size; }
    uint32_t get_max_protection() const { return m_max_protection; }
    uint32_t get_init_protection() const { return m_init_protection; }

private:
    std::string m_name;
    uint64_t m_virtual_address;
    uint64_t m_virtual_size;
    uint64_t m_file_offset;
    uint64_t m_file_size;
    uint32_t m_max_protection;
    uint32_t m_init_protection;
};

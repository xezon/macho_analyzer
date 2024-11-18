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

    // Getters
    std::string get_name() const { return name_; }
    uint64_t get_virtual_address() const { return virtual_address_; }
    uint64_t get_virtual_size() const { return virtual_size_; }
    uint64_t get_file_offset() const { return file_offset_; }
    uint64_t get_file_size() const { return file_size_; }
    uint32_t get_max_protection() const { return max_protection_; }
    uint32_t get_init_protection() const { return init_protection_; }

private:
    std::string name_;
    uint64_t virtual_address_;
    uint64_t virtual_size_;
    uint64_t file_offset_;
    uint64_t file_size_;
    uint32_t max_protection_;
    uint32_t init_protection_;
};
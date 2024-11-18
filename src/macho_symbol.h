// macho_symbol.h
#pragma once

#include <LIEF/LIEF.hpp>
#include <LIEF/MachO.hpp>
#include <nlohmann/json.hpp>
#include <string>

class MachoSymbol {
public:
    explicit MachoSymbol(const LIEF::MachO::Symbol& sym);
    virtual ~MachoSymbol() = default;

    nlohmann::json to_json() const;

    // Getters
    std::string get_name() const { return name_; }
    std::string get_demangled_name() const { return demangled_name_; }
    uint64_t get_address() const { return address_; }
    bool is_external() const { return is_external_; }
    bool is_exported() const { return is_exported_; }
    bool is_imported() const { return is_imported_; }

private:
    std::string name_;
    std::string demangled_name_;
    uint64_t address_;
    bool is_external_;
    bool is_exported_;
    bool is_imported_;
};
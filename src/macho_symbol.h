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

    uint8_t raw_type() const { return m_symbol.raw_type(); }

    bool is_exported() const { return m_symbol.has_export_info(); }
    bool is_imported() const { return m_symbol.has_binding_info(); }


private:
    const LIEF::MachO::Symbol& m_symbol;
};

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

    std::string get_name() const { return m_name; }
    std::string get_demangled_name() const { return m_demangled_name; }
    uint64_t get_address() const { return m_address; }
    bool is_external() const { return m_is_external; }
    bool is_exported() const { return m_is_exported; }
    bool is_imported() const { return m_is_imported; }

private:
    std::string m_name;
    std::string m_demangled_name;
    uint64_t m_address;
    bool m_is_external;
    bool m_is_exported;
    bool m_is_imported;
};

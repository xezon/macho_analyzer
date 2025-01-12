// macho_symbol.cpp
#include "macho_symbol.h"

MachoSymbol::MachoSymbol(const LIEF::MachO::Symbol& sym) {
    m_name = sym.name();
    m_demangled_name = sym.demangled_name();
    m_address = sym.value();
    m_is_external = sym.is_external();
    m_is_exported = sym.has_export_info();
    m_is_imported = sym.has_binding_info();
}

nlohmann::json MachoSymbol::to_json() const {
    nlohmann::json j;
    j["name"] = m_name;
    j["demangled_name"] = m_demangled_name;
    j["address"] = m_address;
    j["is_external"] = m_is_external;
    j["is_exported"] = m_is_exported;
    j["is_imported"] = m_is_imported;
    return j;
}

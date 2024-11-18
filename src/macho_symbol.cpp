// macho_symbol.cpp
#include "macho_symbol.h"

MachoSymbol::MachoSymbol(const LIEF::MachO::Symbol& sym) {
    name_ = sym.name();
    demangled_name_ = sym.demangled_name();
    address_ = sym.value();
    is_external_ = sym.is_external();
    is_exported_ = sym.has_export_info();
    is_imported_ = sym.has_binding_info();
}

nlohmann::json MachoSymbol::to_json() const {
    nlohmann::json j;
    j["name"] = name_;
    j["demangled_name"] = demangled_name_;
    j["address"] = address_;
    j["is_external"] = is_external_;
    j["is_exported"] = is_exported_;
    j["is_imported"] = is_imported_;
    return j;
}
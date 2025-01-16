// macho_symbol.cpp
#include "macho_symbol.h"
#include "demangle.h"
#include "utility.h"

const char* to_string(LIEF::MachO::BindingInfo::TYPES type)
{
    switch (type)
    {
    case LIEF::MachO::BindingInfo::TYPES::UNKNOWN:
        return "UNKNOWN";
    case LIEF::MachO::BindingInfo::TYPES::DYLD_INFO:
        return "DYLD_INFO";
    case LIEF::MachO::BindingInfo::TYPES::CHAINED:
        return "CHAINED";
    case LIEF::MachO::BindingInfo::TYPES::CHAINED_LIST:
        return "CHAINED_LIST";
    }
    return "";
}

MachoSymbol::MachoSymbol(const LIEF::MachO::Symbol& symbol) : m_symbol(symbol) {
}

nlohmann::json MachoSymbol::to_json() const {
    nlohmann::json j;
    j["raw_type"] = m_symbol.raw_type();
    j["type"] = LIEF::MachO::to_string(m_symbol.type());
    j["origin"] = LIEF::MachO::to_string(m_symbol.origin());
    j["category"] = LIEF::MachO::to_string(m_symbol.category());
    j["name"] = m_symbol.name();

    if (m_symbol.raw_type() == 36 && m_symbol.name().empty()) {
        // m_symbol.value() contains function code size of the previous function symbol (raw_type:36)
    }

    if (m_symbol.raw_type() == 36 || m_symbol.raw_type() == 30) {
        // m_symbol.value() contains address to static initializer value.
    }

    if (m_symbol.raw_type() <= 40) {
        const std::string& symbol_name = m_symbol.name();
        std::string_view symbol_name_view;
        switch (m_symbol.raw_type()) {
        case 2:
        case 14:
            if (starts_with(symbol_name, "__GLOBAL__")) // __GLOBAL__I, __GLOBAL__D
                symbol_name_view = std::string_view{symbol_name.data() + 10+1, symbol_name.size() - 10-1};
            else
                symbol_name_view = symbol_name;
            break;
        case 32:
            if (ends_with(symbol_name, ":G"))
                symbol_name_view = std::string_view{symbol_name.data(), symbol_name.size() - 2};
            else
                symbol_name_view = symbol_name;
            break;
        case 36:
            symbol_name_view = symbol_name;
            if (starts_with(symbol_name, "_GLOBAL__")) // _GLOBAL__I, _GLOBAL__D
                symbol_name_view = std::string_view{symbol_name_view.data() + 9+1, symbol_name.size() - 9-1};
            if (ends_with(symbol_name, ":F") || ends_with(symbol_name, ":f"))
                symbol_name_view = std::string_view{symbol_name_view.data(), symbol_name_view.size() - 2};
            break;
        case 38:
        case 40:
            if (ends_with(symbol_name, ":S") || ends_with(symbol_name, ":V"))
                symbol_name_view = std::string_view{symbol_name.data(), symbol_name.size() - 2};
            else
                symbol_name_view = symbol_name;
            break;
        default:
            symbol_name_view = symbol_name;
            break;
        }
        j["demangled_name"] = itanium_demangle(symbol_name_view);
    }
    j["value"] = m_symbol.value();
    j["size"] = m_symbol.size();
    j["is_external"] = m_symbol.is_external();
    j["is_exported"] = m_symbol.has_export_info();
    j["is_imported"] = m_symbol.has_binding_info();

    if (const LIEF::MachO::ExportInfo* export_info = m_symbol.export_info()) {
        j["export_kind"] = LIEF::MachO::to_string(export_info->kind());
        if (export_info->flags() & (uint64_t)LIEF::MachO::ExportInfo::FLAGS::WEAK_DEFINITION) {
            j["WEAK_DEFINITION"] = true;
        }
        if (export_info->flags() & (uint64_t)LIEF::MachO::ExportInfo::FLAGS::REEXPORT) {
            j["REEXPORT"] = true;
        }
        if (export_info->flags() & (uint64_t)LIEF::MachO::ExportInfo::FLAGS::STUB_AND_RESOLVER) {
            j["STUB_AND_RESOLVER"] = true;
        }
        j["export_address"] = export_info->address();
        if (const LIEF::MachO::Symbol* alias = export_info->alias()) {
            j["export_alias_name"] = alias->name();
        }
    }

    if (const LIEF::MachO::BindingInfo* binding_info = m_symbol.binding_info()) {
        j["binding_type"] = to_string(binding_info->type());
        j["binding_address"] = binding_info->address();
        j["binding_addend"] = binding_info->addend();
        j["binding_has_library"] = binding_info->has_library();
        j["binding_has_segment"] = binding_info->has_segment();
        j["binding_is_weak_import"] = binding_info->is_weak_import();
    }

    return j;
}

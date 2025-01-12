// macho_parser.cpp
#include "macho_parser.h"
#include "macho_section.h"
#include "macho_segment.h"
#include "macho_symbol.h"
#include <chrono>
#include <set>
#include <stdexcept>

class MachoParserImpl : public MachoParser {
    // Empty implementation - just allows us to create MachoParser instances
    friend class MachoParser;
};

std::unique_ptr<MachoParser> MachoParser::parse(const std::string& filepath, ParseResult& result) {
    result.success = false;
    auto parser = std::unique_ptr<MachoParser>(new MachoParserImpl());

    if (parser && parser->init(filepath)) {
        result.success = true;
        return parser;
    }

    result.error_message = "Failed to initialize parser";
    return nullptr;
}

bool MachoParser::init(const std::string& filepath) {
    m_binary = LIEF::MachO::Parser::parse(filepath);
    if (!m_binary) {
        return false;
    }

    // Check for corrupted load commands
    for (const auto& macho : *m_binary) {
        if (macho.commands().size() == 0) {
            m_warnings.push_back("Load commands appear to be corrupted or empty");
        }
    }

    parse_symbols();
    parse_sections();
    parse_segments();
    parse_dyld_info();

    return true;
}

std::string MachoParser::get_architecture() const {
    for (const auto& macho : *m_binary) {
        auto cpu_type = macho.header().cpu_type();

        if (cpu_type == LIEF::MachO::Header::CPU_TYPE::X86) {
            return "x86";
        }
        if (cpu_type == LIEF::MachO::Header::CPU_TYPE::X86_64) {
            return "x86_64";
        }
        if (cpu_type == LIEF::MachO::Header::CPU_TYPE::ARM) {
            return "arm";
        }
        if (cpu_type == LIEF::MachO::Header::CPU_TYPE::ARM64) {
            return "arm64";
        }
    }
    return "unknown";
}

void MachoParser::parse_symbols() {
    for (const auto& macho : *m_binary) {
        for (const auto& sym : macho.symbols()) {
            m_symbols.push_back(std::make_unique<MachoSymbol>(sym));
        }
    }
}

void MachoParser::parse_sections() {
    for (const auto& macho : *m_binary) {
        for (const auto& sect : macho.sections()) {
            m_sections.push_back(std::make_unique<MachoSection>(sect));
        }
    }
}

void MachoParser::parse_segments() {
    for (const auto& macho : *m_binary) {
        for (const auto& seg : macho.segments()) {
            m_segments.push_back(std::make_unique<MachoSegment>(seg));
        }
    }
}

void MachoParser::parse_dyld_info() {
    // TODO: Implementation for parsing dyld info if needed
    // For now, this is a placeholder
}

MachoParser::BinaryInfo MachoParser::analyze_binary() const {
    BinaryInfo info = {};
    std::set<std::string> unique_sections;

    // Calculate sizes
    for (const auto& seg : m_segments) {
        info.total_size += seg->get_file_size();
        
        if (seg->get_name() == "__TEXT") {
            info.code_size = seg->get_file_size();
        } else if (seg->get_name() == "__DATA") {
            info.data_size = seg->get_file_size();
        } else if (seg->get_name() == "__LINKEDIT") {
            info.linkedit_size = seg->get_file_size();
        }
    }

    // Analyze sections
    for (const auto& sect : m_sections) {
        unique_sections.insert(sect->get_name());
        if (sect->get_name().find("DEBUG") != std::string::npos) {
            info.has_debug_info = true;
        }
    }
    info.section_types.assign(unique_sections.begin(), unique_sections.end());

    // Count symbols
    info.symbol_count = m_symbols.size();
    for (const auto& sym : m_symbols) {
        if (sym->is_exported()) info.export_count++;
        if (sym->is_imported()) info.import_count++;
    }

    return info;
}

nlohmann::json MachoParser::to_json() const {
    nlohmann::json j;

    j["architecture"] = get_architecture();

    // Add binary analysis
    auto analysis = analyze_binary();
    j["analysis"] = {
        {"total_size", analysis.total_size},
        {"code_size", analysis.code_size},
        {"data_size", analysis.data_size},
        {"linkedit_size", analysis.linkedit_size},
        {"has_debug_info", analysis.has_debug_info},
        {"section_types", analysis.section_types},
        {"symbol_count", analysis.symbol_count},
        {"export_count", analysis.export_count},
        {"import_count", analysis.import_count}
    };

    j["symbols"] = nlohmann::json::array();
    for (const auto& sym : m_symbols) {
        j["symbols"].push_back(sym->to_json());
    }

    j["sections"] = nlohmann::json::array();
    for (const auto& sect : m_sections) {
        j["sections"].push_back(sect->to_json());
    }

    j["segments"] = nlohmann::json::array();
    for (const auto& seg : m_segments) {
        j["segments"].push_back(seg->to_json());
    }

    return j;
}

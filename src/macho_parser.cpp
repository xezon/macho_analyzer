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

const char* MachoParser::get_architecture() const {
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
    {
        size_t reserve_size = 0;
        for (const auto& macho : *m_binary) {
            reserve_size += macho.symbols().size();
        }
        m_symbols.reserve(reserve_size);
    }
    for (const auto& macho : *m_binary) {
        for (const LIEF::MachO::Symbol& symbol : macho.symbols()) {
            m_symbols.emplace_back(symbol);
        }
    }
}

void MachoParser::parse_sections() {
    {
        size_t reserve_size = 0;
        for (const auto& macho : *m_binary) {
            reserve_size += macho.sections().size();
        }
        m_sections.reserve(reserve_size);
    }
    for (const auto& macho : *m_binary) {
        for (const LIEF::MachO::Section& section : macho.sections()) {
            m_sections.emplace_back(section);
        }
    }
}

void MachoParser::parse_segments() {
    {
        size_t reserve_size = 0;
        for (const auto& macho : *m_binary) {
            reserve_size += macho.segments().size();
        }
        m_segments.reserve(reserve_size);
    }
    for (const auto& macho : *m_binary) {
        for (const LIEF::MachO::SegmentCommand& segment : macho.segments()) {
            m_segments.emplace_back(segment);
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
    for (const MachoSegment& segment : m_segments) {
        info.total_size += segment.get_file_size();
        
        if (segment.get_name() == "__TEXT") {
            info.code_size = segment.get_file_size();
        } else if (segment.get_name() == "__DATA") {
            info.data_size = segment.get_file_size();
        } else if (segment.get_name() == "__LINKEDIT") {
            info.linkedit_size = segment.get_file_size();
        }
    }

    // Analyze sections
    for (const MachoSection& section : m_sections) {
        unique_sections.insert(section.get_name());
        if (section.get_name().find("DEBUG") != std::string::npos) {
            info.has_debug_info = true;
        }
    }
    info.section_types.assign(unique_sections.begin(), unique_sections.end());

    // Count symbols
    info.symbol_count = m_symbols.size();
    for (const MachoSymbol& symbol : m_symbols) {
        if (symbol.is_exported())
            info.export_count++;
        if (symbol.is_imported())
            info.import_count++;
        info.symbol_raw_types.insert(symbol.raw_type());
    }

    return info;
}

nlohmann::json MachoParser::to_json() const {
    nlohmann::json j;

    j["architecture"] = get_architecture();

    // Add binary analysis
    MachoParser::BinaryInfo analysis = analyze_binary();
    j["analysis"] = {
        {"total_size", analysis.total_size},
        {"code_size", analysis.code_size},
        {"data_size", analysis.data_size},
        {"linkedit_size", analysis.linkedit_size},
        {"has_debug_info", analysis.has_debug_info},
        {"section_types", analysis.section_types},
        {"symbol_count", analysis.symbol_count},
        {"export_count", analysis.export_count},
        {"import_count", analysis.import_count},
        {"symbol_raw_types", analysis.symbol_raw_types}
    };

    j["symbols"] = nlohmann::json::array();
    for (const MachoSymbol& symbol : m_symbols) {
        j["symbols"].push_back(symbol.to_json());
    }

    j["sections"] = nlohmann::json::array();
    for (const MachoSection& section : m_sections) {
        j["sections"].push_back(section.to_json());
    }

    j["segments"] = nlohmann::json::array();
    for (const MachoSegment& segment : m_segments) {
        j["segments"].push_back(segment.to_json());
    }

    return j;
}

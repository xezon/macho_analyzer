// analyzer_main.cpp
#include "macho_parser.h"
#include <cxxopts.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>

void print_binary_summary(const nlohmann::json& json) {
    const auto& analysis = json["analysis"];
    
    std::cout << "\nBinary Analysis Summary:\n";
    std::cout << "------------------------\n";
    std::cout << fmt::format("Architecture: {}\n", json["architecture"].get<std::string>());
    std::cout << fmt::format("Total Size: {:.2f} MB\n", analysis["total_size"].get<uint64_t>() / (1024.0 * 1024.0));
    std::cout << fmt::format("Code Size: {:.2f} MB\n", analysis["code_size"].get<uint64_t>() / (1024.0 * 1024.0));
    std::cout << fmt::format("Data Size: {:.2f} MB\n", analysis["data_size"].get<uint64_t>() / (1024.0 * 1024.0));
    std::cout << fmt::format("LinkEdit Size: {:.2f} MB\n", analysis["linkedit_size"].get<uint64_t>() / (1024.0 * 1024.0));
    std::cout << fmt::format("Has Debug Info: {}\n", analysis["has_debug_info"].get<bool>() ? "Yes" : "No");
    std::cout << fmt::format("Symbol Count: {}\n", analysis["symbol_count"].get<size_t>());
    std::cout << fmt::format("Export Count: {}\n", analysis["export_count"].get<size_t>());
    std::cout << fmt::format("Import Count: {}\n", analysis["import_count"].get<size_t>());
    
    std::cout << "\nSection Types:\n";
    for (const auto& section : analysis["section_types"]) {
        std::cout << fmt::format("- {}\n", section.get<std::string>());
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    cxxopts::Options options("macho_analyzer", "Mach-O file analyzer");
        
    options.add_options()
        ("i,input", "Input Mach-O file", cxxopts::value<std::string>())
        ("o,output", "Output JSON file (optional)", cxxopts::value<std::string>())
        ("h,help", "Print help")
    ;
        
    cxxopts::ParseResult result;

    try {
        result = options.parse(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
        
    if (result.count("help") || argc == 1) {
        std::cout << options.help() << std::endl;
        return 0;
    }
        
    std::string input_file;
    if (result.count("input")) {
        input_file = result["input"].as<std::string>();
    } else if (argc > 1) {
        input_file = argv[1];
    } else {
        std::cerr << "Error: Input file is required\n" << options.help() << std::endl;
        return 1;
    }

    MachoParser::ParseResult parse_result;
    auto parser = MachoParser::parse(input_file, parse_result);
        
    if (!parse_result.success || !parser) {
        std::cerr << "Failed to parse Mach-O file: " << input_file << "\n";
        if (!parse_result.error_message.empty()) {
            std::cerr << "Error: " << parse_result.error_message << "\n";
        }
        return 1;
    }
        
    // Output any warnings
    for (const auto& warning : parser->get_warnings()) {
        std::cerr << "Warning: " << warning << "\n";
    }
        
    nlohmann::json output = parser->to_json();
        
    // Add metadata to JSON
    output["metadata"] = {
        {"warnings", parser->get_warnings()},
        {"input_file", input_file},
        {"parse_time", std::chrono::system_clock::now().time_since_epoch().count()}
    };
        
    std::string output_file = result.count("output") ? 
        result["output"].as<std::string>() : 
        input_file + ".json";
            
    std::ofstream out(output_file);
    out << output.dump(2) << std::endl;
        
    // Print summary
    print_binary_summary(output);
        
    std::cout << "Analysis complete. Results written to: " << output_file << std::endl;
        
    // Return success with warning if there were any warnings
    return parser->get_warnings().empty() ? 0 : 0;
}

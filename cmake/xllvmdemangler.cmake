
set(XLLVMDEMANGLER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/XLLVMDemangler)

FetchContent_Populate(XLLVMDemangler DOWNLOAD_EXTRACT_TIMESTAMP
    GIT_REPOSITORY https://github.com/horsicq/XLLVMDemangler
    GIT_TAG        master
    SOURCE_DIR     ${XLLVMDEMANGLER_DIR})


add_library(XLLVMDemangler STATIC
    ${XLLVMDEMANGLER_DIR}/3rdparty/llvm/Demangle/_Demangle.cpp
    ${XLLVMDEMANGLER_DIR}/3rdparty/llvm/Demangle/_Demangle.h
    ${XLLVMDEMANGLER_DIR}/3rdparty/llvm/Demangle/Demangle.cpp
    ${XLLVMDEMANGLER_DIR}/3rdparty/llvm/Demangle/Demangle.h
    ${XLLVMDEMANGLER_DIR}/3rdparty/llvm/Demangle/DemangleConfig.h
    ${XLLVMDEMANGLER_DIR}/3rdparty/llvm/Demangle/ItaniumDemangle.cpp
    ${XLLVMDEMANGLER_DIR}/3rdparty/llvm/Demangle/ItaniumDemangle.h
    ${XLLVMDEMANGLER_DIR}/3rdparty/llvm/Demangle/MicrosoftDemangle.cpp
    ${XLLVMDEMANGLER_DIR}/3rdparty/llvm/Demangle/MicrosoftDemangle.h
    ${XLLVMDEMANGLER_DIR}/3rdparty/llvm/Demangle/MicrosoftDemangleNodes.cpp
    ${XLLVMDEMANGLER_DIR}/3rdparty/llvm/Demangle/MicrosoftDemangleNodes.h
    ${XLLVMDEMANGLER_DIR}/3rdparty/llvm/Demangle/StringView.h
    ${XLLVMDEMANGLER_DIR}/3rdparty/llvm/Demangle/Utility.h
)

target_include_directories(XLLVMDemangler PUBLIC ${XLLVMDEMANGLER_DIR}/3rdparty)

#include "llvm_transpile/llvm_transpile.h"
#include "llvm/IR/Function.h"
#include <memory>
#include <print>


namespace dconstruct::llvm_transpile {
    [[nodiscard]] std::unique_ptr<llvm::Function> transpile_function_to_llvm(const ControlFlowGraph& graph) {
        std::println("Hello world");
        return nullptr;
    }
}

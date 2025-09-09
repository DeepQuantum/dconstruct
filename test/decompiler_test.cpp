#include "decompilation/decompiler.h"
#include "binaryfile.h"
#include "disassembly/file_disassembler.h"
#include "ast/ast.h"
#include <array>
#include <gtest/gtest.h>
#include <filesystem>

const std::string TEST_DIR = "C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/dc_test_files/";

namespace dconstruct::testing {

    static SIDBase base{R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\test_sidbase.bin)"};

    static function_disassembly get_function_disassembly(const std::string &path, const u32 offset) {
        SIDBase base{TEST_DIR + "test_sidbase.bin"}; 
        BinaryFile file(TEST_DIR + path); 
        file.dc_setup();
        FileDisassembler disassembler(&file, &base, "", DisassemblerOptions{}); 
        const ScriptLambda *lambda_ptr = disassembler.get_value_ptr_at<ScriptLambda>(offset); 
        const function_disassembly fd = disassembler.create_function_disassembly(lambda_ptr);
        return fd;
    }
    
    /*static dcompiler::decompiled_function decompile_instructions_without_disassembly(
        const std::vector<Instruction>& istrs,
        const std::string& name = "Test",
        std::vector<SymbolTableEntry>&& symbol_table = {}
    ) {
        StackFrame sf{};
        sf.m_symbolTableEntries = std::move(symbol_table);
        std::vector<function_disassembly_line> lines;
        for (u64 i = 0; i < istrs.size(); ++i) {
            lines.push_back(function_disassembly_line(i, istrs.data()));
        }
        function_disassembly fd{lines, std::move(sf), name};
        dcompiler::Decompiler dc(&fd, base);
        std::vector<dcompiler::decompiled_function> funcs = dc.decompile();
        return std::move(funcs[0]);
    }*/

    /*static dcompiler::expression_frame make_expression_frame(
        const std::vector<Instruction>& istrs, 
        const std::string& name = "Test",
        std::vector<SymbolTableEntry>&& symbol_table = {}
    ) {
        return decompile_instructions_without_disassembly(istrs, name, std::move(symbol_table)).m_frame;
    }*/

    static dcompiler::decompiled_function decompile_instructions_with_disassembly(
        std::vector<Instruction>&& istrs, 
        const std::string& name = "Test",
        const std::vector<u64>& symbol_table = {}
    ) {
        BinaryFile file{ R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\dummy.bin)" };
        Disassembler da{ &file, &base };
        auto fd = da.create_function_disassembly(std::move(istrs), name, location(symbol_table.data()));
        dcompiler::Decompiler df{ &fd, file };
        auto funcs = df.decompile();
        return std::move(funcs.at(name));
    }

    static std::string get_decompiled_function_from_file(const std::string& path, const std::string& function_id) {
        BinaryFile file{ path };
        file.dc_setup();
        Disassembler da{ &file, &base };
        da.disassemble();
        for (const auto& func : da.get_functions()) {
            if (func.m_id == function_id) {
                dcompiler::Decompiler dc{ &func, file };
                const auto& funcs = dc.decompile();
                return funcs.at(function_id).to_string();
            }
        }
        
    }

    TEST(DECOMPILER, BasicLoadImmediate) {
        const compiler::environment env{};
        std::vector<Instruction> istrs = {
            {Opcode::LoadU16Imm, 0, 1, 0},
            {Opcode::Return, 0, 0, 0}
        };
        const auto func = decompile_instructions_with_disassembly(std::move(istrs), "BasicLoadImmediate");
        const auto& frame = func.m_frame;

        ASSERT_EQ(frame.m_baseBlock.m_statements.size(), 1);

        const auto& actual = *static_cast<const ast::return_stmt*>(frame.m_baseBlock.m_statements[0].get());
        const auto rhs = actual.m_expr->compute_type(env);

        ASSERT_TRUE(rhs.has_value());

        const auto& type = rhs.value();

        auto expected = ast::return_stmt(std::make_unique<ast::literal>(static_cast<u16>(1)));

        ASSERT_EQ(actual, expected);
    }

    TEST(DECOMPILER, BasicLoadImmediateString) {
        const compiler::environment env{};
        const auto func = decompile_instructions_with_disassembly({
            {Opcode::LoadU16Imm, 0, 1, 0},
            {Opcode::Return, 0, 0, 0}
        }, "BasicLoadImmediateString");
        const auto& frame = func.m_frame;
        ASSERT_EQ(frame.m_baseBlock.m_statements.size(), 1);

        const auto& actual = *static_cast<const ast::return_stmt*>(frame.m_baseBlock.m_statements[0].get());
        const auto rhs = actual.m_expr->compute_type(env);

        ASSERT_TRUE(rhs.has_value());

        const auto& type = rhs.value();

        std::ostringstream actual_os;
        actual.pseudo(actual_os);
        const std::string actual_str = actual_os.str();
        const std::string expected_str = "return 1;";
        ASSERT_EQ(expected_str, actual_str);
    }


    TEST(DECOMPILER, BasicLoadImmediatesString) {
        std::vector<Instruction> istrs = {
            {Opcode::LoadU16Imm, 0, 1, 0},
            {Opcode::LoadU16Imm, 2, 5, 5},
            {Opcode::Return, 0, 0, 0}
        };
        const auto func = decompile_instructions_with_disassembly(std::move(istrs), "BasicLoadImmediatesString");
        const auto& frame = func.m_frame;

        const auto& actual = frame.m_baseBlock.m_statements;
        const std::string expected = "return 1;";
        std::ostringstream os;
        for (const auto& stmt : actual) {
            stmt->pseudo(os);
        }
        EXPECT_EQ(expected, os.str());
    }


    TEST(DECOMPILER, BasicIdentifierAdd) {
        const u16 dest = 2;
        std::vector<Instruction> istrs = {
            {Opcode::LoadU16Imm, 0, 1, 0},
            {Opcode::LoadU16Imm, 1, 5, 5},
            {Opcode::IAdd, dest, 0, 1},
            {Opcode::Return, dest, 0, 0}
        };
        const auto func = decompile_instructions_with_disassembly(std::move(istrs), "BasicIdentifierAdd");
        const auto& frame = func.m_frame;

        const auto& actual = *static_cast<const ast::return_stmt*>(frame.m_baseBlock.m_statements[0].get());
        const std::string expected = "return 1 + 1285;";
        std::ostringstream os;
        os << actual;
        EXPECT_EQ(expected, os.str());
    }

    TEST(DECOMPILER, TwoAdds) {
        const u16 dest = 2;
        std::vector<Instruction> istrs = {
            {Opcode::LoadU16Imm, 0, 1, 0},
            {Opcode::LoadU16Imm, 1, 5, 5},
            {Opcode::IAdd, dest, 0, 1},
            {Opcode::IAdd, dest, 2, 2}, 
            {Opcode::Return, dest, 0, 0}
        };
        const auto func = decompile_instructions_with_disassembly(std::move(istrs), "TwoAdds");
        const auto& frame = func.m_frame;

        const auto& actual = frame.m_baseBlock.m_statements;
        const std::string expected = "return (1 + 1285) + (1 + 1285);";

        std::ostringstream os;
        for (const auto& stmt : actual) {
            stmt->pseudo(os);
        }
        EXPECT_EQ(expected, os.str());
    }


    TEST(DECOMPILER, Call1) {
        std::vector<Instruction> istrs = {
            {Opcode::LookupPointer, 0, 0, 0},
            {Opcode::LoadU16Imm, 49, 5, 0},
            {Opcode::Call, 0, 0, 1},
            {Opcode::Return}
        };
        std::vector<u64> symbol_table;
        symbol_table.push_back(SID("ddict-key-count"));
        const auto func = decompile_instructions_with_disassembly(std::move(istrs), "Call1", std::move(symbol_table));
        const auto& frame = func.m_frame;

        const auto& actual = frame.m_baseBlock.m_statements;
        const std::string expected = "return ddict-key-count(5);";

        std::ostringstream os;
        for (const auto& stmt : actual) {
            stmt->pseudo(os);
        }
        EXPECT_EQ(expected, os.str());
    }

    TEST(DECOMPILER, FileFunction1) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        const std::string expected =
            "function #7C28D25188889230() {\n"
            "    return #E16F9CC43A37FADA(arg_0) * -1;\n"
            "}";

        const std::string actual = get_decompiled_function_from_file(filepath, "#7C28D25188889230");

        ASSERT_EQ(expected, actual);
    }

    TEST(DECOMPILER, FileFunction2) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        const std::string expected = 
            "function #E16F9CC43A37FADA() {\n"
            "    sid var_0 = player;\n"
            "    return distance-between-points(get-region-centroid(arg_0, 0), get-object-position(var_0));\n"
            "}";

        const std::string actual = get_decompiled_function_from_file(filepath, "#E16F9CC43A37FADA");

        ASSERT_EQ(expected, actual);
    }

    TEST(DECOMPILER, ImmediatePostdominator1) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        BinaryFile file{ filepath };
        file.dc_setup();
        Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "#8A8D5C923D5DDB3B";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.m_id == id; });
        ASSERT_NE(func, funcs.end());
        dcompiler::Decompiler dc{ &*func, file };
        const auto& dc_funcs = dc.decompile();
        const auto& dc_func = dc_funcs.at(id);
        const auto& tree = dc_func.m_graph.get_immediate_postdominators();
        for (const auto& [k, v] : tree) {
            ASSERT_EQ(v, 0x16);
        }
    }

    TEST(DECOMPILER, ImmediatePostdominator2) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        BinaryFile file{ filepath };
        file.dc_setup();
        Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "#608356039B1FD9FD";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.m_id == id; });
        ASSERT_NE(func, funcs.end());
        dcompiler::Decompiler dc{ &*func, file };
        const auto& dc_funcs = dc.decompile();
        const auto& dc_func = dc_funcs.at(id);
        const auto& tree = dc_func.m_graph.get_immediate_postdominators();
        ASSERT_EQ(tree.at(0xE), 0x3A);
    }

    /*TEST(DECOMPILER, SimpleIf1) {
        std::vector<Instruction> istrs = {
            {Opcode::LoadU16Imm, 0, 1, 0},
            {Opcode::LoadU16Imm, 1, 2, 0},
            {Opcode::IEqual, 2, 0, 1},
            {Opcode::BranchIf, 6, 2},
            {Opcode::LoadU16Imm, 0, 5, 0},
            {Opcode::Branch, 7, 0, 0},
            {Opcode::LoadU16Imm, 0, 6, 0},
            {Opcode::Return, 0, 0, 0}
        };

        auto fd = decompile_instructions_with_disassembly(std::move(istrs), {});

        const std::string expected = "if (0 == 1) { return 5; } else { return 6; }";

        EXPECT_EQ(fd.m_frame.m_baseBlock.m_statements.size(), 3);
        EXPECT_EQ(fd.to_string(), expected);
    }*/
}

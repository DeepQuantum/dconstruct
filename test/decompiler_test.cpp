#include "binaryfile.h"
#include "decompilation/decomp_function.h"
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
        
        FileDisassembler disassembler(&file, &base, "", DisassemblerOptions{}); 
        const ScriptLambda *lambda_ptr = disassembler.get_value_ptr_at<ScriptLambda>(offset); 
        const function_disassembly fd = disassembler.create_function_disassembly(lambda_ptr, "");
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

    static dcompiler::decomp_function decompile_instructions_with_disassembly(
        std::vector<Instruction>&& istrs, 
        const std::string& name = "Test",
        const SymbolTable& table = {}
    ) {
        BinaryFile file{ R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\dummy.bin)" };
        Disassembler da{ &file, &base };
        auto fd = da.create_function_disassembly(std::move(istrs), name, table.first);
        return dcompiler::decomp_function{ &fd, file };
    }

    static std::string get_decompiled_function_from_file(const std::string& path, const std::string& function_id) {
        BinaryFile file{ path };
        Disassembler da{ &file, &base };
        da.disassemble();
        for (const auto& func : da.get_functions()) {
            if (func.m_id == function_id) {
                return dcompiler::decomp_function{&func, file}.to_string();
            }
        }
        return "";
    }

    static std::string get_decompiled_node_from_file(const std::string& path, const std::string& function_id, const node_id node) {
        BinaryFile file{ path };
        Disassembler da{ &file, &base };
        da.disassemble();
        for (const auto& func : da.get_functions()) {
            if (func.m_id == function_id) {
                return dcompiler::decomp_function{&func, file}.to_string();
            }
        }
        return "";
    }

    TEST(DECOMPILER, BasicLoadImmediate) {
        const compiler::environment env{};
        std::vector<Instruction> istrs = {
            {Opcode::LoadU16Imm, 0, 1, 0},
            {Opcode::Return, 0, 0, 0}
        };
        const auto func = decompile_instructions_with_disassembly(std::move(istrs), "BasicLoadImmediate");

        ASSERT_EQ(func.m_baseBlock.m_statements.size(), 1);

        const auto& actual = *static_cast<const ast::return_stmt*>(func.m_baseBlock.m_statements[0].get());
        const auto rhs = actual.m_expr->compute_type(env);

        ASSERT_FALSE(std::holds_alternative<std::monostate>(rhs));

        const auto& type = rhs;

        auto expected = ast::return_stmt(std::make_unique<ast::literal>(static_cast<u16>(1)));

        ASSERT_EQ(actual, expected);
    }

    TEST(DECOMPILER, BasicLoadImmediateString) {
        const compiler::environment env{};
        const auto func = decompile_instructions_with_disassembly({
            {Opcode::LoadU16Imm, 0, 1, 0},
            {Opcode::Return, 0, 0, 0}
        }, "BasicLoadImmediateString");
        ASSERT_EQ(func.m_baseBlock.m_statements.size(), 1);

        const auto& actual = *static_cast<const ast::return_stmt*>(func.m_baseBlock.m_statements[0].get());
        const auto rhs = actual.m_expr->compute_type(env);

        ASSERT_FALSE(std::holds_alternative<std::monostate>(rhs));

        const auto& type = rhs;

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

        const auto& actual = func.m_baseBlock.m_statements;
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

        const auto& actual = *static_cast<const ast::return_stmt*>(func.m_baseBlock.m_statements[0].get());
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

        const auto& actual = func.m_baseBlock.m_statements;
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
            {Opcode::Return, 0, 0, 0}
        };
        std::vector<u64> table_entries;
        table_entries.push_back(SID("ddict-key-count"));
        std::vector<ast::full_type> symbol_table_types;
        symbol_table_types.push_back(ast::make_function(ast::make_type(ast::primitive_kind::I32), { {"ddict", ast::make_type(ast::primitive_kind::I32) } }));
        SymbolTable table{ location(table_entries.data()), std::move(symbol_table_types) };
        const auto func = decompile_instructions_with_disassembly(std::move(istrs), "Call1", std::move(table));

        const auto& actual = func.m_baseBlock.m_statements;
        const std::string expected =
            "return ddict-key-count(5);";

        std::ostringstream os;
        for (const auto& stmt : actual) {
            stmt->pseudo(os);
        }
        EXPECT_EQ(expected, os.str());
    }

    TEST(DECOMPILER, FileFunction1) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        const std::string expected =
            "unknown #7C28D25188889230(unknown arg_0) {\n"
            "    return #E16F9CC43A37FADA(arg_0) * -1.00;\n"
            "}";

        const std::string actual = get_decompiled_function_from_file(filepath, "#7C28D25188889230");

        ASSERT_EQ(expected, actual);
    }

    TEST(DECOMPILER, FileFunction2) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        const std::string expected = 
            "unknown #E16F9CC43A37FADA(unknown arg_0) {\n"
            "    return distance-between-points(get-region-centroid(arg_0, 0), get-object-position(player));\n"
            "}";

        const std::string actual = get_decompiled_function_from_file(filepath, "#E16F9CC43A37FADA");

        ASSERT_EQ(expected, actual);
    }

    TEST(DECOMPILER, ImmediatePostdominator1) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        BinaryFile file{ filepath };
        Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "#8A8D5C923D5DDB3B";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.m_id == id; });
        ASSERT_NE(func, funcs.end());
        const auto dc_func = dcompiler::decomp_function{ &*func, file };
        for (const auto& node : dc_func.m_graph.m_nodes) {
            ASSERT_EQ(node.m_ipdom, 0x16);
        }
    }

    TEST(DECOMPILER, ImmediatePostdominator2) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        BinaryFile file{ filepath };
        Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "#8A8D5C923D5DDB3B";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.m_id == id; });
        ASSERT_NE(func, funcs.end());
        const auto dc_func = dcompiler::decomp_function{ &*func, file };
        for (const auto& node : dc_func.m_graph.m_nodes) {
            ASSERT_EQ(node.m_ipdom, 0x16);
        }
    }
 
    /*TEST(DECOMPILER, ImmediatePostdominator2) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        BinaryFile file{ filepath };
        
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
        ASSERT_EQ(tree.at(0x15), 0x3A);
    }

    TEST(DECOMPILER, ImmediatePostdominator3) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        BinaryFile file{ filepath };
        
        Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "#D14395D282B18D18";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.m_id == id; });
        ASSERT_NE(func, funcs.end());
        dcompiler::Decompiler dc{ &*func, file };
        const auto& dc_funcs = dc.decompile();
        const auto& dc_func = dc_funcs.at(id);
        const auto& tree = dc_func.m_graph.get_immediate_postdominators();
        ASSERT_EQ(tree.at(0x0), 0x37);
        ASSERT_EQ(tree.at(0x3), 0xE);
        ASSERT_EQ(tree.at(0xE), 0x2E);
    }
    */


    TEST(DECOMPILER, DetermineArgumentType) {
        std::vector<Instruction> istrs = {
            {Opcode::Move, 0, 49, 0},
            {Opcode::LoadU16Imm, 1, 5, 0},
            {Opcode::IEqual, 2, 0, 1},
            {Opcode::Return, 2, 0, 0}
        };
        const auto func = decompile_instructions_with_disassembly(std::move(istrs), "DetermineArgumentType");
        const auto& actual = func.to_string();

        const std::string expected =
            "bool DetermineArgumentType(u16 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";

        EXPECT_EQ(expected, actual);
    }

    TEST(DECOMPILER, RegistersToEmit1) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        BinaryFile file{ filepath };
        Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "#8A8D5C923D5DDB3B";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.m_id == id; });
        ASSERT_NE(func, funcs.end());
        const auto dc_func = dcompiler::decomp_function{ &*func, file };
        const reg_set registers_to_emit = dc_func.m_graph.get_branch_phi_registers(dc_func.m_graph[0], dc_func.m_disassembly->m_isScriptFunction);
        ASSERT_TRUE(registers_to_emit.test(0));
    }

    /*TEST(DECOMPILER, RegisterReadBeforeOverwrite1) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        BinaryFile file{ filepath };
        Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "#C3B48D02AC9ECB46";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.m_id == id; });
        ASSERT_NE(func, funcs.end());
        const auto dc_func = dcompiler::decomp_function{ &*func, file };
        b8 is_read_first = dc_func.m_graph.get(0x4, 0, 0);
        ASSERT_FALSE(is_read_first);
        is_read_first = dc_func.m_graph.register_gets_read_before_overwrite(0x4, 2, 0xB - 0x4);
        ASSERT_TRUE(is_read_first);
        is_read_first = dc_func.m_graph.register_gets_read_before_overwrite(0x4, 0, 0xE - 0x4);
        ASSERT_FALSE(is_read_first);
    }*/

    TEST(DECOMPILER, If1) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        BinaryFile file{ filepath };
        Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "#8A8D5C923D5DDB3B";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.m_id == id; });
        ASSERT_NE(func, funcs.end());
        const auto dc_func = dcompiler::decomp_function{ &*func, file };
        const std::string expected =
            "i32 #8A8D5C923D5DDB3B() {\n"
            "    i32 var_0;\n"
            "    if(get-int32(#5389CC70A44E7358, self) > 0) {\n"
            "        var_0 = get-int32(#5389CC70A44E7358, self);\n"
            "    } else {\n"
            "        var_0 = get-int32(#CEF93DF859F605EA, self);\n"
            "    }\n"
            "    return var_0;\n"
            "}";
        std::ofstream out(R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dcpl\)" + id + ".dcpl");
        out << dc_func.to_string();
        ASSERT_EQ(dc_func.to_string(), expected);
    }

    TEST(DECOMPILER, If2) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        BinaryFile file{ filepath };
        Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "#C3B48D02AC9ECB46";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.m_id == id; });
        ASSERT_NE(func, funcs.end());
        const auto dc_func = dcompiler::decomp_function{ &*func, file };
        const std::string expected =
            "function DetermineArgumentType(i64 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";
        std::ofstream out(R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dcpl\)" + id + ".dcpl");
        out << dc_func.to_string();
        ASSERT_EQ(dc_func.to_string(), expected);
    }

    TEST(DECOMPILER, If3) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        BinaryFile file{ filepath };
        Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "#BC06CBDEAE8344C7";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.m_id == id; });
        ASSERT_NE(func, funcs.end());
        const auto dc_func = dcompiler::decomp_function{ &*func, file };
        const std::string expected =
            "string #BC06CBDEAE8344C7(u16 arg_0) {\n"
            "    string var_0;\n"
            "    if(arg_0 == 0) {\n"
            "        var_0 = \"none\";\n"
            "    } else if(arg_0 == 1) {\n"
            "        var_0 = \"potential\";\n"
            "    } else if(arg_0 == 2) {\n"
            "        var_0 = \"certain\";\n"
            "    } else if(arg_0 == 3) {\n"
            "        var_0 = \"missing\";\n"
            "    } else if(arg_0 == 4) {\n"
            "        var_0 = \"lost\";\n"
            "    } else {\n"
            "        var_0 = \"invalid\";\n"
            "    }\n"
            "    return var_0;\n"
            "}";
        std::ofstream out(R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dcpl\)" + id + ".dcpl");
        out << dc_func.to_string();
        ASSERT_EQ(dc_func.to_string(), expected);
    }

    TEST(DECOMPILER, Loop1) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        BinaryFile file{ filepath };
        Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "#14C6FC79122F4A87";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.m_id == id; });
        ASSERT_NE(func, funcs.end());
        const auto dc_func = dcompiler::decomp_function{ &*func, file };
        const std::string expected =
            "function DetermineArgumentType(i64 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";
        std::ofstream out(R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dcpl\)" + id + ".dcpl");
        out << dc_func.to_string();
        ASSERT_EQ(dc_func.to_string(), expected);
    }

    TEST(DECOMPILER, ShortCircuit1) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        BinaryFile file{ filepath };
        Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "#B97D31F760DB0E8E";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.m_id == id; });
        ASSERT_NE(func, funcs.end());
        const auto dc_func = dcompiler::decomp_function{ &*func, file };
        const std::string expected =
            "function DetermineArgumentType(i64 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";
        std::ofstream out(R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dcpl\)" + id + ".dcpl");
        out << dc_func.to_string();
        ASSERT_EQ(dc_func.to_string(), expected);
    }

    TEST(DECOMPILER, ShortCircuit2) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        BinaryFile file{ filepath };
        Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "#608356039B1FD9FD";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.m_id == id; });
        ASSERT_NE(func, funcs.end());
        const auto dc_func = dcompiler::decomp_function{ &*func, file };
        const std::string expected =
            "function DetermineArgumentType(i64 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";
        std::ofstream out(R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dcpl\)" + id + ".dcpl");
        out << dc_func.to_string();
        ASSERT_EQ(dc_func.to_string(), expected);
    }

    TEST(DECOMPILER, Loop2) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        BinaryFile file{ filepath };
        Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "#E5FCFC6B95B3F669";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.m_id == id; });
        ASSERT_NE(func, funcs.end());
        const auto dc_func = dcompiler::decomp_function{ &*func, file };
        const std::string expected =
            "function DetermineArgumentType(i64 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";
        std::ofstream out(R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dcpl\)" + id + ".dcpl");
        out << dc_func.to_string();
        ASSERT_EQ(dc_func.to_string(), expected);
    }

    TEST(DECOMPILER, Loop3) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        BinaryFile file{ filepath };
        Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "#A548628CB635DC72";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.m_id == id; });
        ASSERT_NE(func, funcs.end());
        const auto dc_func = dcompiler::decomp_function{ &*func, file };
        const std::string expected =
            "function DetermineArgumentType(i64 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";
        std::ofstream out(R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dcpl\)" + id + ".dcpl");
        out << dc_func.to_string();
        ASSERT_EQ(dc_func.to_string(), expected);
    }

    /*TEST(DECOMPILER, If4) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        BinaryFile file{ filepath };
        Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "#9265F983755147F4";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.m_id == id; });
        ASSERT_NE(func, funcs.end());
        const auto dc_func = dcompiler::decomp_function{ &*func, file };
        const std::string expected =
            "function DetermineArgumentType(i64 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";
        std::ofstream out(R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dcpl\)" + id + ".dcpl");
        out << dc_func.to_string();
        ASSERT_EQ(dc_func.to_string(), expected);
    }*/

    TEST(DECOMPILER, If5) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        BinaryFile file{ filepath };
        Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "#43DF4E5E85BFD47C";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.m_id == id; });
        ASSERT_NE(func, funcs.end());
        const auto dc_func = dcompiler::decomp_function{ &*func, file };
        const std::string expected =
            "function DetermineArgumentType(i64 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";
        std::ofstream out(R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dcpl\)" + id + ".dcpl");
        out << dc_func.to_string();
        ASSERT_EQ(dc_func.to_string(), expected);
    }

    TEST(DECOMPILER, If6) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        BinaryFile file{ filepath };
        Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "#14C6FC79122F4A87";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.m_id == id; });
        ASSERT_NE(func, funcs.end());
        const auto dc_func = dcompiler::decomp_function{ &*func, file };
        const std::string expected =
            "function DetermineArgumentType(i64 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";
        std::ofstream out(R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dcpl\)" + id + ".dcpl");
        out << dc_func.to_string();
        ASSERT_EQ(dc_func.to_string(), expected);
    }

    TEST(DECOMPILER, SpecialFunc1) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        BinaryFile file{ filepath };
        Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "#E5FCFC6B95B3F669";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.m_id == id; });
        ASSERT_NE(func, funcs.end());
        const auto dc_func = dcompiler::decomp_function{ &*func, file };
        std::ofstream out(R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dcpl\)" + id + ".dcpl");
        out << dc_func.to_string();
    }

    TEST(DECOMPILER, AllFuncs) {
        const std::string filepath = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\ss-wave-manager.bin)";
        BinaryFile file{ filepath };
        FileDisassembler da{ &file, &base, R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dcpl\ss-wave-manager.asm)", {} };
        da.disassemble();
        const auto& funcs = da.get_functions();
        std::set<std::string> emitted{};
        std::ofstream out(R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dcpl\ss-wave-manager.dcpl)");
        const auto start = std::chrono::high_resolution_clock::now();
        for (const auto& func : funcs) {
            if (emitted.contains(func.m_id)) {
                continue;
            }
            emitted.insert(func.m_id);
            try {
                const auto dc_func = dcompiler::decomp_function{ &func, file };
                out << dc_func.to_string() << "\n\n";
            } catch (const std::exception& e) {
                std::cout << e.what() << '\n';
            }   
        }
        const auto stop = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        std::cout << "took " << duration << "ms\n";
    }

    TEST(DECOMPILER, FullGame) {
        const std::filesystem::path base_path = R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dcpl\)";
        for (const auto& entry : std::filesystem::recursive_directory_iterator(R"(C:\Program Files (x86)\Steam\steamapps\common\The Last of Us Part II\build\pc\main\bin_unpacked\dc1)")) {
            if (entry.path().extension() != ".bin") {
                continue;
            }
            std::filesystem::path new_path = base_path / entry.path().filename().replace_extension(".dcpl");
            std::filesystem::path disassembly_path = base_path / entry.path().filename().replace_extension(".asm");
            std::set<std::string> emitted{};

            BinaryFile file{ entry.path() };
            Disassembler da{ &file, &base };
            da.disassemble();
            const auto& funcs = da.get_functions();
            for (const auto& func : funcs) {
                if (emitted.contains(func.m_id)) {
                    continue;
                }
                emitted.insert(func.m_id);
                try {
                    const auto dc_func = dcompiler::decomp_function{ &func, file };
                    std::ofstream out(new_path, std::ios::app);
                    out << dc_func.to_string() << "\n\n";
                }
                catch (const std::exception& e) {
                    std::cout << e.what() << '\n';
                }
            }
        }
    }
}

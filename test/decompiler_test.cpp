#include "BinaryFile.h"
#include "decompilation/decomp_function.h"
#include "disassembly/file_disassembler.h"
#include "ast/ast.h"
#include <array>
#include <gtest/gtest.h>
#include <filesystem>

const std::string TEST_DIR = "C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/dc_test_files/";
const std::string DCPL_PATH = "C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/dcpl/";

namespace dconstruct::testing {

    static SIDBase base = *SIDBase::from_binary(TEST_DIR + R"(\test_sidbase.bin)");

    static function_disassembly get_function_disassembly(const std::string &path, const u32 offset) {
        SIDBase base = *SIDBase::from_binary(TEST_DIR + "test_sidbase.bin"); 
        auto file_res = BinaryFile<>::from_path(TEST_DIR + path);
        if (!file_res) {
            std::cerr << file_res.error() << "\n";
            std::terminate();
        }
        auto& file = *file_res;
        FileDisassembler disassembler(&file, &base, "", DisassemblerOptions{}); 
        const ScriptLambda *lambda_ptr = disassembler.get_value_ptr_at<ScriptLambda>(offset); 
        const function_disassembly fd = disassembler.create_function_disassembly(lambda_ptr, "");
        return fd;
    }

    static ast::function_definition decompile_instructions_with_disassembly(
        std::vector<Instruction>&& istrs, 
        const std::string& name = "Test",
        const SymbolTable& table = {}
    ) {
        BinaryFile<> file = *BinaryFile<>::from_path(TEST_DIR + R"(\dummy.bin)");
        TLOU2Disassembler da{ &file, &base };
        auto fd = da.create_function_disassembly(std::move(istrs), name, table.m_location);
        auto dc_func = dconstruct::dcompiler::TLOU2decomp_function(fd, file, ControlFlowGraph::build(fd));
        auto& test = const_cast<ast::function_definition&>(dc_func.decompile(false));
        return std::move(test);
    }

    static std::string get_decompiled_function_from_file(const std::string& path, const std::string& function_id, const bool optimize = false) {
        auto file_res = BinaryFile<>::from_path(path);
        if (!file_res) {
            std::cerr << file_res.error() << "\n";
            std::terminate();
        }
        auto& file = *file_res;
        TLOU2Disassembler da{ &file, &base };
        da.disassemble();
        for (const auto& func : da.get_functions()) {
            if (func.get_id() == function_id) {
                auto fd = dcompiler::decomp_function{func, file, ControlFlowGraph::build(func)};
                return fd.decompile(optimize).to_c_string();
            }
        }
        return "";
    }

    static std::string get_decompiled_node_from_file(const std::string& path, const std::string& function_id, const node_id node) {
        auto file_res = BinaryFile<>::from_path(path);
        if (!file_res) {
            std::cerr << file_res.error() << "\n";
            std::terminate();
        }
        auto& file = *file_res;
        TLOU2Disassembler da{ &file, &base };
        da.disassemble();
        for (const auto& func : da.get_functions()) {
            if (func.get_id() == function_id) {
                auto fd = dcompiler::decomp_function{func, file, ControlFlowGraph::build(func)};
                return fd.decompile().to_c_string();
            }
        }
        return "";
    }

    static void decomp_test(const std::string& filepath, const std::string& id, const std::string& expected, dconstruct::ast::print_fn_type stream_lang = dconstruct::ast::racket, const bool optimize = false) {
        auto file_res = BinaryFile<>::from_path(filepath);
        if (!file_res) {
            std::cerr << file_res.error() << "\n";
            std::terminate();
        }
        auto& file = *file_res;
        FileDisassembler<true> da{ &file, &base, DCPL_PATH + dconstruct::sanitize_dc_string(id) + ".asm", {} };
        da.disassemble();
        da.dump();
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.get_id() == id; });
        ASSERT_NE(func, funcs.end());
        auto dc_func = dcompiler::decomp_function{ *func, file,  ControlFlowGraph::build(*func), DCPL_PATH + dconstruct::sanitize_dc_string(id) + ".svg" };
        std::ofstream file_out(DCPL_PATH + dconstruct::sanitize_dc_string(id) + ".dcpl");
        std::ostringstream out;
        out << stream_lang << dc_func.decompile(optimize);
        file_out << stream_lang << out.str();
        ASSERT_EQ(expected, out.str());
    }

    TEST(DECOMPILER, BasicLoadImmediate) {
        compiler::environment<ast::full_type> env{};
        std::vector<Instruction> istrs = {
            {Opcode::LoadU16Imm, 0, 1, 0},
            {Opcode::Return, 0, 0, 0}
        };
        const auto& func = decompile_instructions_with_disassembly(std::move(istrs), "BasicLoadImmediate");

        ASSERT_EQ(func.m_body.m_statements.size(), 1);

        const auto& actual = *static_cast<const ast::return_stmt*>(func.m_body.m_statements[0].get());
        const auto& rhs = actual.m_expr->compute_type(env);

        ASSERT_FALSE(std::holds_alternative<std::monostate>(rhs));

        const auto& type = rhs;

        auto expected = ast::return_stmt(std::make_unique<ast::literal>(static_cast<u16>(1)));

        ASSERT_EQ(actual, expected);
    }

    TEST(DECOMPILER, BasicLoadImmediateString) {
        compiler::environment<ast::full_type> env{};
        const auto& func = decompile_instructions_with_disassembly({
            {Opcode::LoadU16Imm, 0, 1, 0},
            {Opcode::Return, 0, 0, 0}
        }, "BasicLoadImmediateString");
        ASSERT_EQ(func.m_body.m_statements.size(), 1);

        const auto& actual = *static_cast<const ast::return_stmt*>(func.m_body.m_statements[0].get());
        const auto& rhs = actual.m_expr->compute_type(env);

        ASSERT_FALSE(std::holds_alternative<std::monostate>(rhs));

        const auto& type = rhs;

        std::ostringstream actual_os;
		actual.pseudo_c(actual_os);
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
        const auto& func = decompile_instructions_with_disassembly(std::move(istrs), "BasicLoadImmediatesString");

        const auto& actual = func.m_body.m_statements;
        const std::string expected = "return 1;";
        std::ostringstream os;
        for (const auto& stmt : actual) {
            stmt->pseudo_c(os);
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
        const auto& func = decompile_instructions_with_disassembly(std::move(istrs), "BasicIdentifierAdd");

        const auto& actual = static_cast<const ast::return_stmt&>(*func.m_body.m_statements[0]);
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
        const auto& func = decompile_instructions_with_disassembly(std::move(istrs), "TwoAdds");

        const auto& actual = func.m_body.m_statements[0];
        const std::string expected = "return (1 + 1285) + (1 + 1285);";

        std::ostringstream os;
        os << *actual;
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
        const auto& func = decompile_instructions_with_disassembly(std::move(istrs), "Call1", std::move(table));

        const auto& actual = func.m_body;
        const std::string expected =
            "{\n"
            "    u64? var_0 = ddict-key-count(5);\n"
            "    return var_0;\n"
            "}";

        std::ostringstream os;
        os << actual;
        EXPECT_EQ(expected, os.str());
    }

    TEST(DECOMPILER, FullFunc1) {
        const std::string filepath = TEST_DIR + R"(\ss-wave-manager.bin)";
        const std::string expected =
            "u64? #7C28D25188889230(u64? arg_0) {\n"
            "    u64? var_0 = #E16F9CC43A37FADA(arg_0);\n"
            "    return var_0 * -1.00;\n"
            "}";

        const std::string actual = get_decompiled_function_from_file(filepath, "#7C28D25188889230");

        ASSERT_EQ(expected, actual);
    }

    TEST(DECOMPILER, FullFunc2) {
        const std::string filepath = TEST_DIR + R"(\ss-wave-manager.bin)";
        const std::string expected = 
            "u64? #E16F9CC43A37FADA(u64? arg_0) {\n"
            "    u64? var_0 = get-region-centroid(arg_0, 0);\n"
            "    u64? var_1 = get-object-position(player);\n"
            "    u64? var_2 = distance-between-points(var_0, var_1);\n"
            "    return var_2;\n"
            "}";

        const std::string actual = get_decompiled_function_from_file(filepath, "#E16F9CC43A37FADA");

        ASSERT_EQ(expected, actual);
    }

    TEST(DECOMPILER, ImmediatePostdominator1) {
        const std::string filepath = TEST_DIR + R"(\ss-wave-manager.bin)";
        auto file_res = BinaryFile<>::from_path(filepath);
        if (!file_res) {
            std::cerr << file_res.error() << "\n";
            std::terminate();
        }
        auto& file = *file_res;
        TLOU2Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "#8A8D5C923D5DDB3B";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.get_id() == id; });
        ASSERT_NE(func, funcs.end());
        const auto dc_func = dcompiler::decomp_function{ *func, file, ControlFlowGraph::build(*func)};
        for (const auto& node : dc_func.m_graph.m_nodes) {
            ASSERT_EQ(node.m_ipdom, 0x3);
        }
    }

    TEST(DECOMPILER, ArgumentType) {
        std::vector<Instruction> istrs = {
            {Opcode::Move, 0, 49, 0},
            {Opcode::LoadU16Imm, 1, 5, 0},
            {Opcode::IEqual, 2, 0, 1},
            {Opcode::Return, 2, 0, 0}
        };
        const auto& func = decompile_instructions_with_disassembly(std::move(istrs), "DetermineArgumentType");
        const auto& actual = func.to_c_string();

        const std::string expected =
            "bool DetermineArgumentType(u16 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";

        EXPECT_EQ(expected, actual);
    }

    TEST(DECOMPILER, RegistersToEmit1) {
        const std::string filepath = TEST_DIR + R"(\ss-wave-manager.bin)";
        auto file_res = BinaryFile<>::from_path(filepath);
        if (!file_res) {
            std::cerr << file_res.error() << "\n";
            std::terminate();
        }
        auto& file = *file_res;
        TLOU2Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "#8A8D5C923D5DDB3B";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.get_id() == id; });
        ASSERT_NE(func, funcs.end());
        const auto dc_func = dcompiler::decomp_function{ *func, file, ControlFlowGraph::build(*func) };
        const reg_set registers_to_emit = dc_func.m_graph.get_branch_phi_registers(dc_func.m_graph[0]);
        ASSERT_TRUE(registers_to_emit.test(0));
    }
    
    TEST(DECOMPILER, If1) {
        const std::string filepath = TEST_DIR + R"(\ss-wave-manager.bin)";
        const std::string id = "#8A8D5C923D5DDB3B";
        const std::string expected =
            "i32 #8A8D5C923D5DDB3B() {\n"
            "    i32 var_0;\n"
            "    if (get-int32(#5389CC70A44E7358, self) > 0) {\n"
            "        var_0 = get-int32(#5389CC70A44E7358, self);\n"
            "    } else {\n"
            "        var_0 = get-int32(#CEF93DF859F605EA, self);\n"
            "    }\n"
            "    return var_0;\n"
            "}";
        decomp_test(filepath, id, expected, dconstruct::ast::c, true);
    }

    TEST(DECOMPILER, If2) {
        const std::string filepath = TEST_DIR + R"(\ss-wave-manager.bin)";
        const std::string id = "#C3B48D02AC9ECB46";
        const std::string expected =
            "bool DetermineArgumentType(i64 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";
        decomp_test(filepath, id, expected, dconstruct::ast::c);
    }

    TEST(DECOMPILER, If3) {
        const std::string filepath = TEST_DIR + R"(\ss-wave-manager.bin)";
        auto file_res = BinaryFile<>::from_path(filepath);
        if (!file_res) {
            std::cerr << file_res.error() << "\n";
            std::terminate();
        }
        auto& file = *file_res;
        TLOU2Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "#BC06CBDEAE8344C7";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.get_id() == id; });
        ASSERT_NE(func, funcs.end());
        const auto dc_func = dcompiler::decomp_function{ *func, file, ControlFlowGraph::build(*func) }.decompile();
        const std::string expected =
            "string #BC06CBDEAE8344C7(u16 arg_0) {\n"
            "    string var_0;\n"
            "    if (arg_0 == 0) {\n"
            "        var_0 = \"none\";\n"
            "    } else if (arg_0 == 1) {\n"
            "        var_0 = \"potential\";\n"
            "    } else if (arg_0 == 2) {\n"
            "        var_0 = \"certain\";\n"
            "    } else if (arg_0 == 3) {\n"
            "        var_0 = \"missing\";\n"
            "    } else if (arg_0 == 4) {\n"
            "        var_0 = \"lost\";\n"
            "    } else {\n"
            "        var_0 = \"invalid\";\n"
            "    }\n"
            "    return var_0;\n"
            "}";
        std::ofstream out(DCPL_PATH + id + ".dcpl");
        out << dc_func;
        ASSERT_EQ(dc_func.to_c_string(), expected);
    }

    TEST(DECOMPILER, Loop1) {
        const std::string filepath = TEST_DIR + R"(\ss-wave-manager.bin)";
        const std::string id = "#14C6FC79122F4A87";
        const std::string expected =
            "function DetermineArgumentType(i64 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";
        decomp_test(filepath, id, expected, dconstruct::ast::c);
    }

    TEST(DECOMPILER, ShortCircuit1) {
        const std::string filepath = TEST_DIR + R"(\ss-wave-manager.bin)";
        const std::string id = "#B97D31F760DB0E8E";
        const std::string expected = 
            "function DetermineArgumentType(i64 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";
        decomp_test(filepath, id, expected, dconstruct::ast::c);
    }

    TEST(DECOMPILER, ShortCircuit2) {
        const std::string filepath = TEST_DIR + R"(\ss-wave-manager.bin)";
        auto file_res = BinaryFile<>::from_path(filepath);
        const std::string id = "#608356039B1FD9FD";
        const std::string expected = "function DetermineArgumentType(i64 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";
        decomp_test(filepath, id, expected, dconstruct::ast::c);
    }

    TEST(DECOMPILER, Loop2) {
        const std::string filepath = TEST_DIR + R"(\ss-wave-manager.bin)";
        const std::string id = "#E5FCFC6B95B3F669";
        const std::string expected =
            "function DetermineArgumentType(i64 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";
        decomp_test(filepath, id, expected, dconstruct::ast::c);
    }

    TEST(DECOMPILER, Loop3) {
        const std::string filepath = TEST_DIR + R"(\ss-wave-manager.bin)";
        const std::string id = "#A548628CB635DC72";
        const std::string expected =
            "function DetermineArgumentType(i64 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";
        decomp_test(filepath, id, expected, dconstruct::ast::c);
    }

    TEST(DECOMPILER, If5) {
        const std::string filepath = TEST_DIR + R"(\ss-wave-manager.bin)";
        const std::string id = "#43DF4E5E85BFD47C";
        const std::string expected =
            "function DetermineArgumentType(i64 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";
        decomp_test(filepath, id, expected, dconstruct::ast::c);
    }

    TEST(DECOMPILER, If6) {
        const std::string filepath = TEST_DIR + R"(\ss-wave-manager.bin)";
        const std::string id = "#14C6FC79122F4A87";
        const std::string expected = "function DetermineArgumentType(i64 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";
        decomp_test(filepath, id, expected, dconstruct::ast::c);
    }

    TEST(DECOMPILER, NodeRegisters0) {
        const std::string filepath = TEST_DIR + R"(\ss-wave-manager.bin)";
        auto file_res = BinaryFile<>::from_path(filepath);
        if (!file_res) {
            std::cerr << file_res.error() << "\n";
            std::terminate();
        }
        auto& file = *file_res;
        TLOU2Disassembler da{ &file, &base };
        da.disassemble();
        const std::string id = "select-spawn-regions@main@start@0";
        const auto& funcs = da.get_functions();
        const auto& func = std::find_if(funcs.begin(), funcs.end(), [&id](const function_disassembly& f) { return f.get_id() == id; });
        ASSERT_NE(func, funcs.end());
		const auto dc_func = dcompiler::decomp_function{ *func, file, ControlFlowGraph::build(*func) };
        const auto& node0 = dc_func.m_graph.m_nodes[0];
        ASSERT_EQ(node0.m_regs.m_readFirst, 0b0);
        ASSERT_EQ(node0.m_regs.m_written, 0b1);
		const auto& node1 = dc_func.m_graph.m_nodes[1];
        ASSERT_EQ(node1.m_regs.m_readFirst, 0b0);
		ASSERT_EQ(node1.m_regs.m_written, 0b111);
		const auto& node2 = dc_func.m_graph.m_nodes[2];
		ASSERT_EQ(node2.m_regs.m_readFirst, 0b0);
		ASSERT_EQ(node2.m_regs.m_written, 0b1);
		const auto& node3 = dc_func.m_graph.m_nodes[3];
		ASSERT_EQ(node3.m_regs.m_readFirst, 0b0);
		ASSERT_EQ(node3.m_regs.m_written, 0b0);
    }

    TEST(DECOMPILER, SpecialFunc1) {
        const std::string filepath = TEST_DIR + R"(\ss-boss-health-tts.bin)";
        const std::string id = "wait-for-tts@main@start@0";
        const std::string expected = "?";
        decomp_test(filepath, id, expected, dconstruct::ast::c);
    }

    TEST(DECOMPILER, AllFuncs) {
        const std::string filepath = R"(C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/melee-script-funcs-impl.bin)";
        auto file_res = BinaryFile<>::from_path(filepath);
        if (!file_res) {
            std::cerr << file_res.error() << "\n";
            std::terminate();
        }
        auto& file = *file_res;
        FileDisassembler da{ &file, &base, R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dcpl\\ss-faq-lightning-flash-manager.asm)", {} };
        da.disassemble();
        da.dump();
        const auto& funcs = da.get_named_functions();
        std::set<std::string> emitted{};
        std::ofstream out(R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dcpl\\ss-faq-lightning-flash-manager.dcpl)");
		out << dconstruct::ast::c;
        const auto start = std::chrono::high_resolution_clock::now();
        for (const auto* func : funcs) {
            if (emitted.contains(func->get_id())) {
                continue;
            }
            emitted.insert(func->get_id());
            try {
                std::cout << func->get_id() << "\n";
                const auto dc_func = dcompiler::decomp_function{ *func, file, ControlFlowGraph::build(*func) }.decompile();
            }
            catch (const std::exception& e) {
                std::cout << e.what();
            }
        }
        const auto stop = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        std::cout << "took " << duration << "ms\n";
    }

    TEST(DECOMPILER, FullGame) {
        const std::filesystem::path base_path = DCPL_PATH;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(R"(C:\Program Files (x86)\Steam\steamapps\common\The Last of Us Part II\build\pc\main\bin_unpacked\dc1)")) {
            if (entry.path().extension() != ".bin") {
                continue;
            }
            std::filesystem::path new_path = base_path / entry.path().filename().replace_extension(".dcpl");
            std::filesystem::path disassembly_path = base_path / entry.path().filename().replace_extension(".asm");
            std::set<std::string> emitted{};

            auto file_res = BinaryFile<>::from_path(entry.path());
            if (!file_res) {
                std::cerr << file_res.error() << "\n";
                std::terminate();
            }
            auto& file = *file_res;
            std::cout << file.m_path << "\n";
            TLOU2Disassembler da{ &file, &base };
            da.disassemble();
            const auto& funcs = da.get_named_functions();
            for (const auto* func : funcs) {
                if (emitted.contains(func->get_id())) {
                    continue;
                }
                emitted.insert(func->get_id());
                try {
                    //std::cout << func->get_id() << "\n";
                    const auto dc_func = dcompiler::decomp_function{ *func, file, ControlFlowGraph::build(*func) }.decompile(false);
                    std::ofstream out(new_path, std::ios::app);
                    out << dc_func << "\n\n";
                }
                catch (const std::exception& e) {
                    std::cout << e.what() << '\n';
                }
            }
        }
    }


	

    TEST(DECOMPILER_RACKET, Racket0) {
        const std::string filepath = TEST_DIR + R"(\behaviors.bin)";
        const std::string id = "anonymous@25880";
        const std::string expected =
            "(or\n"
            "    (melee-fact-get-bool arg_2 disable-grabs)\n"
            "    (npc-struggle-cooldown-func arg_2 arg_3 10.00 20.00 1)\n"
            "    (character-in-struggle? arg_3 16)\n"
            "    (< (melee-fact-get-time-since arg_3 last-time-in-prone-struggle) 5.00)\n"
            ")";
		decomp_test(filepath, id, expected);
    }

    TEST(DECOMPILER_RACKET, Racket1) {
        const std::string filepath = TEST_DIR + R"(\behaviors.bin)";
        const std::string id = "anonymous@25038";
        const std::string expected = 
            "(and\n"
            "    (or\n"
            "        (shambler-standing-explode-line-of-motion-check arg_2 arg_3)\n"
            "        (and\n"
            "            (is-rogue-mode?)\n"
            "            (is-player? arg_3)\n"
            "            (or\n"
            "                (player-is-prone?)\n"
            "                (player-is-supine?)\n"
            "            )\n"
            "            (player-in-prone-hiding-region?)\n"
            "        )\n"
            "    )\n"
            "    (> (melee-fact-get-time-since player shambler-explode) 5.00)\n"
            "    (> (melee-fact-get-time-since arg_2 time-since-in-finisher-fail) 2.00)\n"
            ")";
		decomp_test(filepath, id, expected);
    }

    TEST(DECOMPILER_RACKET, Racket2) {
        const std::string filepath = TEST_DIR + R"(\behaviors.bin)";
        const std::string id = "anonymous@25b88";
        const std::string expected =
            "(and\n"
            "    (or\n"
            "        (shambler-standing-explode-line-of-motion-check arg_2 arg_3)\n"
            "        (and\n"
            "            (is-rogue-mode?)\n"
            "            (is-player? arg_3)\n"
            "            (or\n"
            "                (player-is-prone?)\n"
            "                (player-is-supine?)\n"
            "            )\n"
            "            (player-in-prone-hiding-region?)\n"
            "        )\n"
            "        (and\n"
            "            (is-rogue-mode?)\n"
            "            (is-player? arg_3)\n"
            "            (not (npc-can-path-to-object? arg_2 arg_3))\n"
            "            (<= (distance-between-points (npc-get-nav-destination arg_2) (get-object-position arg_3)) 4.00)\n"
            "        )\n"
            "    )\n"
            "    (> (melee-fact-get-time-since player shambler-explode) 5.00)\n"
            "    (> (melee-fact-get-time-since arg_2 time-since-in-finisher-fail) 1.50)\n"
            ")";
        decomp_test(filepath, id, expected);
    }

    TEST(DECOMPILER_RACKET, Racket3) {
        const std::string filepath = TEST_DIR + R"(\behaviors.bin)";
        const std::string id = "anonymous@267d8";
        const std::string expected =
            "(and\n"
            "    (not (character-in-struggle? arg_3 16))\n"
            "    (> (melee-fact-get-time-since arg_3 last-time-in-prone-struggle) 5.00)\n"
            "    (not (and\n"
            "        (character-in-melee-attack-as-attacker? arg_3)\n"
            "        (character-in-melee-attack-of-other-character? arg_2 arg_3)\n"
            "        (get-current-melee-attack arg_3)\n"
            "        (== (* u64*-> (+ (get-current-melee-attack arg_3) 16)) melee-player-swing-free)\n"
            "        (not (dog-in-melee-with-character? arg_3))\n"
            "    ))\n"
            ")";
        decomp_test(filepath, id, expected);
    }

    TEST(DECOMPILER_RACKET, Racket4) {
        const std::string filepath = TEST_DIR + R"(\behaviors.bin)";
        const std::string id = "anonymous@25930";
        const std::string expected =
            "(and\n"
            "    (not (character-in-struggle? arg_3 16))\n"
            "    (> (melee-fact-get-time-since arg_3 last-time-in-prone-struggle) 5.00)\n"
            "    (not (and\n"
            "        (character-in-melee-attack-as-attacker? arg_3)\n"
            "        (character-in-melee-attack-of-other-character? arg_2 arg_3)\n"
            "        (get-current-melee-attack arg_3)\n"
            "        (== (* u64*-> (+ (get-current-melee-attack arg_3) 16)) melee-player-swing-free)\n"
            "        (not (dog-in-melee-with-character? arg_3))\n"
            "    ))\n"
            ")";
        decomp_test(filepath, id, expected);
    }

    TEST(DECOMPILER, Var1) {
        const std::string filepath = TEST_DIR + R"(\ss-wave-manager.bin)";
        const std::string id = "#8A8D5C923D5DDB3B";
        std::string expected = 
            "i32 #8A8D5C923D5DDB3B() {\n"
            "    i32 var_0 = get-int32(#5389CC70A44E7358, self);\n"
            "    i32 var_1;\n"
            "    if (var_0 > 0) {\n"
            "        i32 var_2 = get-int32(#5389CC70A44E7358, self);\n"
            "        var_1 = var_2;\n"
            "    } else {\n"
            "        i32 var_3 = get-int32(#CEF93DF859F605EA, self);\n"
            "        var_1 = var_3;\n"
            "    }\n"
            "    return var_1;\n"
            "}";
        decomp_test(filepath, id, expected, dconstruct::ast::c);
    }

    TEST(DECOMPILER, Var2) {
        const std::string filepath = TEST_DIR + R"(\ss-wave-manager.bin)";
        const std::string id = "select-wave-composition@main@start@0";
        std::string expected =
            "void select-wave-composition@main@start@0() {\n"
            "    u64? var_0 = is-final-build?();\n"
            "    if (!var_0) {\n"
            "        u64? var_1 = #5445173390656D6D(\"DEB - WM: --SELECT WAVE COMPOSITION--\")\n"
            "        display(var_1, 19);\n"
            "    }\n"
            "}";
        decomp_test(filepath, id, expected, dconstruct::ast::c);
    }

    TEST(DECOMPILER, RacketSqrt) {
        const std::string filepath = TEST_DIR + R"(\user-script-funcs-impl.bin)";
        const std::string id = "sqrt-sign";
        const std::string expected = "(sqrt-sign ?)";
        decomp_test(filepath, id, expected, ast::racket, true);
    }

    TEST(DECOMPILER, Optimization1) {
        const std::string filepath = TEST_DIR + R"(\ss-wave-manager.bin)";
        const std::string id = "#14C6FC79122F4A87";
        const std::string expected = "";
        decomp_test(filepath, id, expected, ast::c, true);
    }

    TEST(DECOMPILER, Optimization2) {
        const std::string filepath = TEST_DIR + R"(\animal-behavior.bin)";
        const std::string id = "large-bird-panic-get-fly-anim-id";
        const std::string expected = "";
        decomp_test(filepath, id, expected, ast::c, true);
    }

    TEST(DECOMPILER, Optimization3) {
        const std::string filepath = R"(C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/player-upgrade-script-funcs-impl.bin)";
        const std::string id = "can-purchase-any-upgrades-in-branch-points?";
        const std::string expected = "";
        decomp_test(filepath, id, expected, ast::c, true);
    }

    TEST(DECOMPILER, Optimization4) {
        const std::string filepath = R"(C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/melee-script-funcs-impl.bin)";
        const std::string id = "npc-ratking?";
        const std::string expected = 
            "u64? npc-ratking?(bool arg_0) {\n"
            "    return !(arg_0 == 0) && is-npc?(arg_0) && npc-get-archetype(arg_0) && *(u64*)(npc-get-archetype(arg_0) + 392) == *infected-ratking-params*;\n"
            "}";
        decomp_test(filepath, id, expected, ast::c, true);
    }

    TEST(DECOMPILER, Optimization5) {
        const std::string filepath = R"(C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/script-user-funcs-impl.bin)";
        const std::string id = "wait-until-anim-fully-faded-in";
        const std::string expected = 
            "u64? wait-until-anim-fully-faded-in(u64? arg_0, u64? arg_1) {\n"
            "    u64? var_1;\n"
            "    while (get-animation-fade(arg_0, arg_1) < 1.00) {\n"
            "        wait-one-frame();\n"
            "    }\n"
            "    return var_1;\n"
            "}";
        decomp_test(filepath, id, expected, ast::c, true);
    }
    
    TEST(DECOMPILER, Optimization6) {
        const std::string filepath = R"(C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/ss/ss-transition-line-chooser.bin)";
        const std::string id = "play-transition-from-bucket";
        const std::string expected = 
            "u64? wait-until-anim-fully-faded-in(u64? arg_0, u64? arg_1) {\n"
            "    u64? var_1;\n"
            "    while (get-animation-fade(arg_0, arg_1) < 1.00) {\n"
            "        wait-one-frame();\n"
            "    }\n"
            "    return var_1;\n"
            "}";
        decomp_test(filepath, id, expected, ast::c, true);
    }

    TEST(DECOMPILER, Optimization7) {
        const std::string filepath = R"(C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/script-user-funcs-impl.bin)";
        const std::string id = "bmm-deactivate-all";
        const std::string expected = 
            "u64? bmm-deactivate-all(u64? arg_0) {\n"
            "    foreach (u64? var_1 : arg_0) {\n"
            "        u16 var_2;\n"
            "        if (var_1 && *(u16*)(var_1 + 12) == 7) {\n"
            "            var_2 = *(u64*)var_1;\n"
            "        } else if (var_1 && *(u16*)(var_1 + 12) == 5) {\n"
            "            var_2 = *(u64*)var_1;\n"
            "        } else if (var_1 && *(u16*)(var_1 + 12) == 4) {\n"
            "            var_2 = *(u64*)var_1;\n"
            "        } else {\n"
            "            var_2 = 0;\n"
            "        }\n"
            "        net-send-event-all(deactivate, var_2);\n"
            "    }\n"
            "}";
        decomp_test(filepath, id, expected, ast::c, true);
    }

    TEST(DECOMPILER, CallArguments) {
        const std::string filepath = R"(C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/ss/ss-locator-action-pack.bin)";
        const std::string id = "move-to-entry@main@start@3";
        const std::string expected = 
            "void move-to-entry@main@start@3() {\n"
            "    npc-look-at-point(\n"
            "        get-symbol(npc, self),\n"
            "        offset-point(\n"
            "            get-spawner-position(get-self-id(), 1),\n"
            "            vector-scale(\n"
            "                get-locator-distance-y(\n"
            "                    get-spawner-locator(get-self-id(), 1),\n"
            "                    get-attach-point-locator(get-symbol(npc, self), targHead)\n"
            "                ),\n"
            "                get-world-y-vec()\n"
            "            )\n"
            "        ),\n"
            "        -1.00\n"
            "    );\n"
            "}"; 
        decomp_test(filepath, id, expected, ast::c, true);
    }

    TEST(DECOMPILER, Foreach2) {
        const std::string filepath = R"(C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/nd-script-funcs.bin)";
        const std::string id = "set-bit";
        const std::string expected = "";
        decomp_test(filepath, id, expected, ast::c, true);
    }

    TEST(DECOMPILER, Match1) {
        const std::string filepath = R"(C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/ss-rogue/rogue-misc-defines.bin)";
        const std::string id = "#C57EE0A64537AE8F";
        const std::string expected = 
            "string #C57EE0A64537AE8F(u16 arg_0) {\n"
            "   string var_0 = match (arg_0) {\n"
            "       0 -> \"Militia\"\n"
            "       1 -> \"Scars\"\n"
            "       2 -> \"Rattlers\"\n"
            "       3 -> \"Infected\"\n"
            "       4 -> \"Max Num Factions\"\n"
            "       else -> \"Invalid\"\n"
            "   }\n"
            "   return var_0;";
        decomp_test(filepath, id, expected, ast::c, true);
    }

    TEST(DECOMPILER, Match4) {
        const std::string filepath = R"(C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/ss-rogue/rogue-misc-defines.bin)";
        const std::string id = "#77C65B88D2083D05";
        const std::string expected = 
            "u64?* #77C65B88D2083D05(u16 arg_0) {\n"
            "    return match (arg_0) {\n"
            "        0 -> MENU_ROGUE_FACTION_MILITIA\n"
            "        1 -> MENU_ROGUE_FACTION_SCARS\n"
            "        2 -> MENU_ROGUE_FACTION_RATTLERS\n"
            "        3 -> MENU_ROGUE_FACTION_INFECTED\n"
            "        else -> *invalid-symbol*\n"
            "    };\n"
            "}";
        decomp_test(filepath, id, expected, ast::c, true);
    }

    TEST(DECOMPILER, Optimization8) {
        const std::string filepath = R"(C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/animal-behavior.bin)";
        const std::string id = "get-landing-anim-id";
        const std::string expected = 
            "u64?* #77C65B88D2083D05(u16 arg_0) {\n"
            "    return match (arg_0) {\n"
            "        0 -> MENU_ROGUE_FACTION_MILITIA\n"
            "        1 -> MENU_ROGUE_FACTION_SCARS\n"
            "        2 -> MENU_ROGUE_FACTION_RATTLERS\n"
            "        3 -> MENU_ROGUE_FACTION_INFECTED\n"
            "        else -> *invalid-symbol*\n"
            "    };\n"
            "}";
        decomp_test(filepath, id, expected, ast::c, false);
    }

    TEST(DECOMPILER, NewFileFormat1) {
        const std::string filepath = R"(C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/ss/ss-animal-flee-simple-loop.bin)";
        auto file_res = BinaryFile<>::from_path(filepath);
        if (!file_res) {
            std::cerr << file_res.error() << "\n";
            std::terminate();
        }
        auto& file = *file_res;
        FileDisassembler<true> da{ &file, &base, DCPL_PATH + "animal_behavior.asm", {} };
        da.disassemble();
        da.dump();
        const auto& funcs = da.get_functions();
        std::vector<ast::function_definition> decompiled_funcs;
        for (const auto& func : funcs) {
            decompiled_funcs.push_back(dcompiler::decomp_function{func, file,  ControlFlowGraph::build(func), DCPL_PATH + "animal_behavior.svg"}.decompile());
        }
        dcompiler::state_script_functions<> full_file_decomp(decompiled_funcs);

        std::ofstream file_out(DCPL_PATH + dconstruct::sanitize_dc_string("animal_behavior") + ".dcpl");
        file_out << full_file_decomp.to_string();
    }

    TEST(DECOMPILER, Optimization10) {
        const std::string filepath = R"(C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/anim-gas-mask-impl.bin)";
        const std::string id = "internal-put-on-gas-mask";
        const std::string expected = "";
        decomp_test(filepath, id, expected, ast::c, true);
    }

    TEST(DECOMPILER, Optimization11) {
        const std::string filepath = R"(C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/animal-behavior.bin)";
        const std::string id = "get-landing-anim-id";
        const std::string expected = "";
        decomp_test(filepath, id, expected, ast::c, true);
    }

    TEST(DECOMPILER, Optimization12) {
        const std::string filepath = R"(C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/ss-rogue/ss-survival-manager.bin)";
        const std::string id = "survival-setup-level@main@start@36";
        const std::string expected = "";
        decomp_test(filepath, id, expected, ast::c, true);
    }
    
    TEST(DECOMPILER, Optimization13) {
        const std::string filepath = R"(C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/ss-abby-fights-militia/ss-afm-horse-chase-combat.bin)";
        const std::string id = "start-chase@militia-1@start@6";
        const std::string expected = "";
        decomp_test(filepath, id, expected, ast::c, true);
    }

    
    TEST(DECOMPILER, Optimization14) {
        const std::string filepath = R"(C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/melee-script-funcs-impl.bin)";
        const std::string id = "npc-contextual-parry-cooldown";
        const std::string expected = "";
        decomp_test(filepath, id, expected, ast::c, false);
    }

    TEST(DECOMPILER, Optimization15) {
        const std::string filepath = R"(C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/nd-script-funcs.bin)";
        const std::string id = "darray-extend";
        const std::string expected = 
        "u64? darray-extend(u64? arg_0, u64? arg_1) {\n"
        "    foreach (u64? element : arg_1) {\n"
        "        darray-append(arg_0, element, 0);\n"
        "    }\n"
        "}";
        decomp_test(filepath, id, expected, ast::c, true);
    }
}

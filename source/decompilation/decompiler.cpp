#include "decompilation/decompiler.h"
#include "compilation/tokens.h"
#include "decompilation/expression_frame.h"
#include "ast/type.h"
#include <fstream>
#include <iostream>


namespace dconstruct::dcompiler {

std::vector<decompiled_function> Decompiler::decompile() {
    std::vector<decompiled_function> funcs;
    funcs.reserve(m_functions.size());
    for (const auto &func : m_functions) {
        ControlFlowGraph cfg{func};
        //cfg.find_loops();
        //cfg.write_image("C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/build/images/" + func->m_id + ".svg");

        expression_frame frame{func->m_stackFrame.m_symbolTableEntries};
        for (const auto& [_, node] : cfg.get_nodes()) {
            parse_basic_block(node, frame);
        }
        funcs.emplace_back(std::move(frame));
    }
    return funcs;
}

void Decompiler::parse_basic_block(const control_flow_node &node, expression_frame &expression_frame) {
    for (const auto &line : node.m_lines) {
        const Instruction &istr = line.m_instruction;
        switch(istr.opcode) {
            case Opcode::IAdd: 
            case Opcode::FAdd: expression_frame.apply_binary_op<ast::add_expr>(istr, compiler::token{compiler::token_type::PLUS, "+"}); break;
            case Opcode::ISub:
            case Opcode::FSub: expression_frame.apply_binary_op<ast::sub_expr>(istr, compiler::token{compiler::token_type::MINUS, "-"}); break;
            case Opcode::IMul:
            case Opcode::FMul: expression_frame.apply_binary_op<ast::mul_expr>(istr, compiler::token{compiler::token_type::STAR, "*"}); break;
            case Opcode::IDiv:
            case Opcode::FDiv: expression_frame.apply_binary_op<ast::div_expr>(istr, compiler::token{compiler::token_type::SLASH, "/"}); break;

            case Opcode::IEqual:
            case Opcode::FEqual: expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::EQUAL_EQUAL, "=="}); break;
            case Opcode::INotEqual:
            case Opcode::FNotEqual: expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::BANG_EQUAL, "!="}); break;
            case Opcode::ILessThan:
            case Opcode::FLessThan: expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::LESS, "<"}); break;
            case Opcode::ILessThanEqual:
            case Opcode::FLessThanEqual: expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::GREATER_EQUAL, ">="}); break;
            case Opcode::IGreaterThan:
            case Opcode::FGreaterThan: expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::GREATER, ">"}); break;
            case Opcode::IGreaterThanEqual:
            case Opcode::FGreaterThanEqual: expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::GREATER_EQUAL, ">="}); break;

            case Opcode::OpLogNot: expression_frame.apply_unary_op<ast::logical_not_expr>(istr, compiler::token{compiler::token_type::BANG, "!"}); break;
            case Opcode::OpBitNot: expression_frame.apply_unary_op<ast::bitwise_not_expr>(istr, compiler::token{compiler::token_type::TILDE, "~"}); break;
            case Opcode::INeg:
            case Opcode::FNeg: expression_frame.apply_unary_op<ast::bitwise_not_expr>(istr, compiler::token{compiler::token_type::MINUS, "-"}); break;

            case Opcode::LoadU16Imm: expression_frame.load_literal(istr.destination, static_cast<u16>(istr.operand1 | static_cast<u16>(istr.operand2) << 8)); break;
            case Opcode::LoadStaticInt: expression_frame.load_literal(istr.destination, expression_frame.m_symbolTable[istr.operand1].m_i64); break;
            case Opcode::LoadStaticFloat: expression_frame.load_literal(istr.destination, expression_frame.m_symbolTable[istr.operand1].m_f32); break;

            case Opcode::Call:
            case Opcode::CallFf: {
                expression_frame.call(istr);
                break;
            }

            case Opcode::LoadStaticPointer:
            case Opcode::LoadStaticU64Imm:
            case Opcode::LookupPointer: {
                const sid64 sid = expression_frame.m_symbolTable[istr.operand1].m_hash;
                const ast::sid_literal sid_literal = {sid, m_sidbase.search(sid)};
                expr_uptr& lit = expression_frame.load_literal(istr.destination, sid_literal);
                if (expression_frame.m_symbolTable[istr.operand1].m_type != SymbolTableEntryType::FUNCTION) {
                    expression_frame.load_expression_into_var(istr.destination, std::move(lit));
                }
                break;
            }

            case Opcode::Move: {
                expression_frame.move(istr.destination, istr.operand1);
                break;
            }

            case Opcode::Return: expression_frame._return(istr.destination); break;
            default: {
                return;
            }
        }
    }
}
}
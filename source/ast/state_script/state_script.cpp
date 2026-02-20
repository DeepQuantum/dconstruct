#include "ast/state_script/state_script.h"
#include <unordered_set>

namespace dconstruct::ast {

[[nodiscard]] bool operator==(const state_script& lhs, const state_script& rhs) noexcept {
    return lhs.equals(rhs);
}

void state_script::pseudo_c(std::ostream& os) const {
    os << "statescript {\n";
    os << indent_more;
    os << indent << "options {\n";
    os << indent_more;
    for (const auto& opt : m_options) {
        os << indent << opt.second << "\n";
    }
    os << indent_less;
    os << indent << "}\n";
    os << indent << "declarations {\n";
    os << indent_more;
    for (const auto& decl : m_declarations) {
        os << indent << decl.m_identifier << "\n";
    }
    os << indent_less;
    os << indent << "}\n";
    for (const auto& s : m_states) {
        os << indent << s << "\n";
    }
    os << indent_less;
    os << indent << "}";
}

void state_script::pseudo_py(std::ostream& os) const {
    os << "statescript:\n  options: ";
    for (const auto& opt : m_options) {
        os << opt.second << " ";
    }
    os << "\n  declarations: ";
    for (const auto& decl : m_declarations) {
        os << decl.m_identifier << " ";
    }
    os << "\n  states: ";
    for (const auto& s : m_states) {
        os << s << " ";
    }
}

void state_script::pseudo_racket(std::ostream& os) const {
    os << "(statescript (options";
    for (const auto& opt : m_options) {
        os << " " << opt.second;
    }
    os << ") (declarations";
    for (const auto& decl : m_declarations) {
        os << " " << decl.m_identifier;
    }
    os << ") (states";
    for (const auto& s : m_states) {
        os << " " << s;
    }
    os << "))";
}

[[nodiscard]] std::vector<semantic_check_error> state_script::check_semantics(compilation::scope& scope) const noexcept {
    std::vector<semantic_check_error> errors;

    compilation::scope decl_scope(&scope);
    for (const auto& decl : m_declarations) {
        std::vector<semantic_check_error> decl_errors = decl.check_semantics(decl_scope);
        errors.insert(errors.end(), decl_errors.begin(), decl_errors.end());

        if (!std::holds_alternative<primitive_type>(decl.m_type)) {
            errors.push_back(semantic_check_error{"state script declaration '" + decl.m_identifier + "' must have a primitive type"});
        }
    }

    std::unordered_set<std::string> state_names;
    for (const auto& state : m_states) {
        if (!state_names.insert(state.m_name).second) {
            errors.push_back(semantic_check_error{"duplicate state name: '" + state.m_name + "'"});
        }

        std::unordered_set<std::string> block_names;
        for (const auto& block : state.m_blocks) {
            if (!block_names.insert(block.m_name).second) {
                errors.push_back(semantic_check_error{"duplicate block name: '" + block.m_name + "' in state '" + state.m_name + "'"});
            }

            std::unordered_set<std::string> track_names;
            for (const auto& track : block.m_tracks) {
                if (!track_names.insert(track.m_name).second) {
                    errors.push_back(semantic_check_error{"duplicate track name: '" + track.m_name + "' in block '" + block.m_name + "'"});
                }
            }
        }
    }

    for (const auto& state : m_states) {
        for (const auto& block : state.m_blocks) {
            for (const auto& track : block.m_tracks) {
                for (size_t i = 0; i < track.m_lambdas.size(); ++i) {
                    const auto& lambda = track.m_lambdas[i];
                    std::vector<semantic_check_error> lambda_errors = lambda.m_body.check_semantics(decl_scope);
                    const std::string path = "in state '" + state.m_name + "' block '" + block.m_name + "' track '" + track.m_name + "' lambda " + std::to_string(i);
                    for (auto& err : lambda_errors) {
                        err.m_message = path + ": " + err.m_message;
                    }
                    errors.insert(errors.end(), lambda_errors.begin(), lambda_errors.end());
                }
            }
        }
    }

    return errors;
}

[[nodiscard]] bool state_script::equals(const state_script& rhs) const noexcept {
    if (m_options != rhs.m_options || m_declarations.size() != rhs.m_declarations.size() ||
        m_states.size() != rhs.m_states.size()) {
        return false;
    }
    for (size_t i = 0; i < m_declarations.size(); ++i) {
        if (!m_declarations[i].equals(rhs.m_declarations[i])) {
            return false;
        }
    }
    for (size_t i = 0; i < m_states.size(); ++i) {
        if (!m_states[i].equals(rhs.m_states[i])) {
            return false;
        }
    }
    return true;
}


[[nodiscard]] program_binary_result state_script::emit_dc(compilation::global_state& global) const noexcept {
    constexpr sid64 script_name = SID("ss-compiled-test");
    constexpr sid64 state_script = SID("state-script");
    constexpr sid64 ss_options = SID("ss-options");
    constexpr sid64 ss_declarations = SID("ss-declarations");


    const u64 num_states = m_states.size();


    std::vector<SsDeclaration> declarations;
    for (const auto& decl : m_declarations) {
        SsDeclaration ss_decl{};
        ss_decl.m_declId = SID(decl.m_identifier.c_str());
        const auto& prim_type = std::get<primitive_type>(decl.m_type);
        switch (prim_type.m_type) {
            case primitive_kind::I32:
                ss_decl.m_declTypeId = SID("int32");
                ss_decl.m_varSizeSum = sizeof(i32);
                break;
            case primitive_kind::U32:
                ss_decl.m_declTypeId = SID("u32");
                ss_decl.m_varSizeSum = sizeof(u32);
                break;
            case primitive_kind::F32:
                ss_decl.m_declTypeId = SID("float");
                ss_decl.m_varSizeSum = sizeof(f32);
                break;
            case primitive_kind::BOOL:
                ss_decl.m_declTypeId = SID("boolean");
                ss_decl.m_varSizeSum = sizeof(bool);
                break;
        }
        declarations.push_back(ss_decl);
    }

    return std::unexpected("state script code emission not yet implemented");

}

}

#include "ast/unary_expressions/dereference_expression.h"


namespace dconstruct::ast {

    [[nodiscard]] expr_uptr dereference_expr::simplify() const {
        return std::make_unique<ast::dereference_expr>(m_operator, m_rhs->simplify());
    }

    [[nodiscard]] full_type dereference_expr::compute_type_unchecked(const compilation::scope& env) const noexcept {
        const full_type rhs_type = m_rhs->compute_type_unchecked(env);
        if (std::holds_alternative<ast::ptr_type>(rhs_type)) {
            return *std::get<ast::ptr_type>(rhs_type).m_pointedAt;
        }
        return ast::full_type{std::monostate()};
    }

    [[nodiscard]] semantic_check_res dereference_expr::compute_type_checked(compilation::scope& env) const noexcept {
        const semantic_check_res rhs_type = m_rhs->get_type_checked(env);
        if (!rhs_type) {
            return rhs_type;
        }

        const std::optional<std::string> invalid_dereference = std::visit([](auto&& rhs_type) -> std::optional<std::string> {
            using T = std::decay_t<decltype(rhs_type)>;

            if constexpr (is_pointer<T>) {
                if (is_unknown(*rhs_type.m_pointedAt)) {
                    return "cannot dereference void pointer";
                } else{ 
                    return std::nullopt;
                }
            } else {
                return "cannot dereference non-pointer type " + type_to_declaration_string(rhs_type);
            }
        }, *rhs_type);

        if (!invalid_dereference) {
            return *std::get<ptr_type>(*rhs_type).m_pointedAt;
        }

        return std::unexpected{semantic_check_error{*invalid_dereference, this}};
    }

    [[nodiscard]] bool dereference_expr::is_l_evaluable() const noexcept {
        return m_rhs->is_l_evaluable();
    }

    [[nodiscard]] emission_res dereference_expr::emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> opt_destination) const noexcept {
        emission_res rhs = m_rhs->emit_dc(fn, global);
        if (!rhs) {
            return rhs;
        }
        
        assert(std::holds_alternative<ptr_type>(*m_type));
        const ptr_type& ptr_t = std::get<ptr_type>(*m_type);

        assert(std::holds_alternative<primitive_type>(*ptr_t.m_pointedAt));

        const primitive_kind kind = std::get<primitive_type>(*ptr_t.m_pointedAt).m_type;

        Opcode load_opcode;
        switch (kind) {
            case primitive_kind::I8:  load_opcode = Opcode::LoadI8;
            case primitive_kind::U8:  load_opcode = Opcode::LoadU8;
            case primitive_kind::I16: load_opcode = Opcode::LoadI16;
            case primitive_kind::U16: load_opcode = Opcode::LoadU16;
            case primitive_kind::I32: load_opcode = Opcode::LoadI32;
            case primitive_kind::U32: load_opcode = Opcode::LoadU32;
            case primitive_kind::I64: load_opcode = Opcode::LoadI64;
            case primitive_kind::U64: load_opcode = Opcode::LoadU64;
            default: assert(false && "need primitive");
        }

        emission_res load_destination = fn.get_destination(opt_destination);
        if (!load_destination) {
            return load_destination;
        }

        fn.emit_instruction(load_opcode, *load_destination, *rhs);
        fn.free_register(*rhs);
        
        return *load_destination;
    }

    [[nodiscard]] lvalue_emission_res dereference_expr::emit_dc_lvalue(compilation::function& fn, compilation::global_state& global) const noexcept {
        lvalue_emission_res rhs = m_rhs->emit_dc_lvalue(fn, global);
        if (!rhs) {
            return rhs;
        }

        assert(std::holds_alternative<ptr_type>(*m_type));
        const ptr_type& ptr_t = std::get<ptr_type>(*m_type);

        assert(std::holds_alternative<primitive_type>(*ptr_t.m_pointedAt));

        const primitive_kind kind = std::get<primitive_type>(*ptr_t.m_pointedAt).m_type;
        
        Opcode store_opcode;
        switch (kind) {
            case primitive_kind::I8:  store_opcode = Opcode::StoreI8;
            case primitive_kind::U8:  store_opcode = Opcode::StoreU8;
            case primitive_kind::I16: store_opcode = Opcode::StoreI16;
            case primitive_kind::U16: store_opcode = Opcode::StoreU16;
            case primitive_kind::I32: store_opcode = Opcode::StoreI32;
            case primitive_kind::U32: store_opcode = Opcode::StoreU32;
            case primitive_kind::I64: store_opcode = Opcode::StoreI64;
            case primitive_kind::U64: store_opcode = Opcode::StoreU64;
            default: assert(false && "need primitive");
        }
        
        return std::pair{rhs->first, store_opcode};
    }
}
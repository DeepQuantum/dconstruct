/* Provide Declarations */
#include <stdint.h>
#ifndef __cplusplus
typedef unsigned char bool;
#endif

#ifndef _MSC_VER
#define __forceinline __attribute__((always_inline)) inline
#endif

#ifdef _MSC_VER
#define __PREFIXALIGN__(X) __declspec(align(X))
#define __POSTFIXALIGN__(X)
#else
#define __PREFIXALIGN__(X)
#define __POSTFIXALIGN__(X) __attribute__((aligned(X)))
#endif



/* Global Declarations */

/* Types Declarations */
struct l_array_11_uint64_t;
struct l_array_26_uint64_t;
struct l_array_19_uint64_t;
struct l_array_1_uint64_t;
struct l_array_18_uint64_t;
struct l_array_5_uint64_t;
struct l_array_4_uint64_t;
struct l_array_6_uint64_t;
struct l_array_8_uint64_t;
struct l_array_12_uint64_t;
struct l_array_10_uint64_t;

/* Function definitions */
typedef uint64_t l_fptr_94(uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_5(uint64_t, uint64_t);
typedef uint64_t l_fptr_17(uint64_t, uint64_t);
typedef uint64_t l_fptr_96(uint64_t);
typedef uint64_t l_fptr_101(uint64_t, uint64_t);
typedef uint64_t l_fptr_9(uint64_t);
typedef uint64_t l_fptr_30(uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_4(void);
typedef uint64_t l_fptr_50(uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_3(uint64_t);
typedef uint64_t l_fptr_79(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_19(uint64_t, uint64_t);
typedef uint64_t l_fptr_65(uint64_t, float);
typedef uint64_t l_fptr_2(uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_45(uint64_t, uint64_t, ...);
typedef uint64_t l_fptr_34(uint64_t, uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_11(uint64_t);
typedef uint64_t l_fptr_54(uint64_t);
typedef uint64_t l_fptr_36(uint64_t);
typedef uint64_t l_fptr_40(uint64_t, uint64_t);
typedef uint64_t l_fptr_78(uint64_t, uint64_t);
typedef uint64_t l_fptr_42(uint64_t, float);
typedef uint64_t l_fptr_77(uint64_t);
typedef uint64_t l_fptr_71(uint64_t, uint64_t);
typedef uint64_t l_fptr_28(uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_25(void);
typedef uint64_t l_fptr_90(uint64_t, uint64_t);
typedef uint64_t l_fptr_64(void);
typedef uint64_t l_fptr_67(uint64_t, uint64_t, uint64_t, float, uint64_t);
typedef uint64_t l_fptr_48(uint64_t, uint64_t);
typedef uint64_t l_fptr_20(uint64_t, uint64_t);
typedef uint64_t l_fptr_23(uint64_t, uint64_t);
typedef uint64_t l_fptr_102(uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_58(void);
typedef uint64_t l_fptr_38(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_63(uint64_t);
typedef uint64_t l_fptr_68(uint64_t);
typedef uint64_t l_fptr_95(void);
typedef uint64_t l_fptr_86(void);
typedef uint64_t l_fptr_97(uint64_t, uint64_t);
typedef uint64_t l_fptr_103(uint64_t, uint64_t);
typedef uint64_t l_fptr_100(void);
typedef uint64_t l_fptr_60(void);
typedef uint64_t l_fptr_46(uint64_t);
typedef uint64_t l_fptr_14(uint64_t, uint64_t);
typedef uint64_t l_fptr_69(void);
typedef uint64_t l_fptr_56(void);
typedef uint64_t l_fptr_80(uint64_t, uint64_t);
typedef uint64_t l_fptr_44(uint64_t, uint64_t);
typedef uint64_t l_fptr_81(float);
typedef uint64_t l_fptr_98(uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_99(uint64_t);
typedef uint64_t l_fptr_55(void);
typedef uint64_t l_fptr_92(uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_104(uint64_t, uint64_t);
typedef uint64_t l_fptr_26(uint64_t, uint64_t, uint64_t, float);
typedef uint64_t l_fptr_52(uint64_t);
typedef uint64_t l_fptr_84(uint64_t);
typedef uint64_t l_fptr_29(uint64_t, uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_72(uint64_t);
typedef uint64_t l_fptr_33(void);
typedef uint64_t l_fptr_6(uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_21(uint64_t, uint64_t);
typedef uint64_t l_fptr_61(uint64_t);
typedef uint64_t l_fptr_93(void);
typedef uint64_t l_fptr_75(uint64_t, uint64_t);
typedef uint64_t l_fptr_51(uint64_t);
typedef uint64_t l_fptr_88(uint64_t, uint64_t);
typedef uint64_t l_fptr_74(uint64_t);
typedef uint64_t l_fptr_22(uint64_t, uint64_t);
typedef uint64_t l_fptr_59(void);
typedef uint64_t l_fptr_1(uint64_t, uint64_t);
typedef uint64_t l_fptr_32(uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_91(uint64_t, uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_41(uint64_t);
typedef uint64_t l_fptr_43(uint64_t, uint64_t, ...);
typedef uint64_t l_fptr_10(void);
typedef uint64_t l_fptr_57(void);
typedef uint64_t l_fptr_16(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_18(uint64_t);
typedef uint64_t l_fptr_66(float, uint64_t);
typedef uint64_t l_fptr_85(uint64_t);
typedef uint64_t l_fptr_83(uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_15(uint64_t, uint64_t);
typedef uint64_t l_fptr_70(uint64_t, uint64_t);
typedef uint64_t l_fptr_24(uint64_t);
typedef uint64_t l_fptr_8(uint64_t, uint64_t);
typedef uint64_t l_fptr_39(uint64_t, uint64_t);
typedef uint64_t l_fptr_82(void);
typedef uint64_t l_fptr_89(uint64_t);
typedef uint64_t l_fptr_53(uint64_t);
typedef uint64_t l_fptr_7(uint64_t, uint64_t);
typedef uint64_t l_fptr_49(uint64_t, uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_31(void);
typedef uint64_t l_fptr_62(void);
typedef uint64_t l_fptr_12(void);
typedef uint64_t l_fptr_27(uint64_t, float);
typedef uint64_t l_fptr_37(uint64_t, uint64_t);
typedef uint64_t l_fptr_13(uint64_t);
typedef uint64_t l_fptr_47(uint64_t, uint64_t);
typedef uint64_t l_fptr_87(uint64_t);
typedef uint64_t l_fptr_35(uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_73(uint64_t, float);
typedef uint64_t l_fptr_76(uint64_t);

/* Types Definitions */
struct l_array_11_uint64_t {
  uint64_t array[11];
};
struct l_array_26_uint64_t {
  uint64_t array[26];
};
struct l_array_19_uint64_t {
  uint64_t array[19];
};
struct l_array_1_uint64_t {
  uint64_t array[1];
};
struct l_array_18_uint64_t {
  uint64_t array[18];
};
struct l_array_5_uint64_t {
  uint64_t array[5];
};
struct l_array_4_uint64_t {
  uint64_t array[4];
};
struct l_array_6_uint64_t {
  uint64_t array[6];
};
struct l_array_8_uint64_t {
  uint64_t array[8];
};
struct l_array_12_uint64_t {
  uint64_t array[12];
};
struct l_array_10_uint64_t {
  uint64_t array[10];
};

/* Global Variable Declarations */
const static __PREFIXALIGN__(8) struct l_array_11_uint64_t wait_NC_equip_NC_gas_NC_mask_symbol_table __POSTFIXALIGN__(8);
const static __PREFIXALIGN__(8) struct l_array_26_uint64_t internal_NC_put_NC_on_NC_gas_NC_mask_symbol_table __POSTFIXALIGN__(8);
const static __PREFIXALIGN__(8) struct l_array_19_uint64_t internal_NC_put_NC_on_NC_gas_NC_mask_NC_immediately_symbol_table __POSTFIXALIGN__(8);
const static __PREFIXALIGN__(8) struct l_array_1_uint64_t get_NC_equipped_NC_gas_NC_mask_symbol_table __POSTFIXALIGN__(8);
const static __PREFIXALIGN__(8) struct l_array_18_uint64_t wait_NC_until_NC_in_NC_valid_NC_gas_NC_mask_NC_state_symbol_table __POSTFIXALIGN__(8);
const static __PREFIXALIGN__(8) struct l_array_5_uint64_t npc_NC_gas_NC_mask_NC_narrative_NC_mode_PC_f_symbol_table __POSTFIXALIGN__(8);
const static __PREFIXALIGN__(8) struct l_array_4_uint64_t internal_NC_put_NC_off_NC_gas_NC_mask_NC_immediately_symbol_table __POSTFIXALIGN__(8);
const static __PREFIXALIGN__(8) struct l_array_6_uint64_t spawn_NC_gas_NC_mask_symbol_table __POSTFIXALIGN__(8);
const static __PREFIXALIGN__(8) struct l_array_8_uint64_t player_NC_gas_NC_mask_NC_narrative_NC_mode_PC_f_symbol_table __POSTFIXALIGN__(8);
const static __PREFIXALIGN__(8) struct l_array_12_uint64_t internal_NC_put_NC_off_NC_gas_NC_mask_symbol_table __POSTFIXALIGN__(8);
const static __PREFIXALIGN__(8) struct l_array_10_uint64_t wait_NC_remove_NC_gas_NC_mask_symbol_table __POSTFIXALIGN__(8);

/* Function Declarations */
uint64_t wait_NC_equip_NC_gas_NC_mask(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1, uint64_t llvm_cbe_arg_2);
uint64_t fact_NC_set(uint64_t _1, uint64_t _2, uint64_t _3);
uint64_t new_NC_boxed_NC_value(uint64_t _4, uint64_t _5);
uint64_t is_NC_script_NC_running_PD_(uint64_t _6);
uint64_t wait_NC_one_NC_frame(void);
uint64_t send_NC_event(uint64_t _7, uint64_t _8, uint64_t _9);
uint64_t internal_NC_put_NC_on_NC_gas_NC_mask_NC_immediately(uint64_t _10, uint64_t _11);
uint64_t internal_NC_put_NC_on_NC_gas_NC_mask(uint64_t _12, uint64_t _13);
uint64_t get_NC_gas_NC_mask(uint64_t _14);
uint64_t internal_NC_put_NC_on_NC_gas_NC_mask_OC_1(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1);
uint64_t lookup_NC_symbol(uint64_t _15, uint64_t _16);
uint64_t get_NC_gas_NC_mask_NC_setting_NC_id(uint64_t _17);
uint64_t wait_NC_until_NC_in_NC_valid_NC_gas_NC_mask_NC_state(uint64_t _18, uint64_t _19);
uint64_t spawn_NC_object(uint64_t _20, uint64_t _21, uint64_t _22, uint64_t _23, uint64_t _24, uint64_t _25, uint64_t _26, uint64_t _27, uint64_t _28);
uint64_t set_NC_high_NC_contrast_NC_mode_NC_type(uint64_t _29, uint64_t _30);
uint64_t is_NC_player_PD_(uint64_t _31);
uint64_t set_NC_instance_NC_flag_NC_hero(uint64_t _32, uint64_t _33);
uint64_t set_NC_show_NC_in_NC_flashlight_NC_flag(uint64_t _34, uint64_t _35);
uint64_t set_NC_shadow_NC_casting_NC_from_NC_npc_NC_flashlight(uint64_t _36, uint64_t _37);
uint64_t set_NC_gas_NC_mask(uint64_t _38, uint64_t _39);
uint64_t set_NC_visible(uint64_t _40, uint64_t _41);
uint64_t attach(uint64_t _42, uint64_t _43, uint64_t _44, float _45);
uint64_t animate_(uint64_t _46, uint64_t _47, uint64_t _48);
uint64_t _DC__FC_alloc_NC_array(uint64_t _49, uint64_t _50, uint64_t _51, uint64_t _52);
uint64_t gesture_(uint64_t _53, uint64_t _54, uint64_t _55);
uint64_t alloc_NC_gesture_NC_play_NC_params(void);
uint64_t internal_NC_put_NC_on_NC_gas_NC_mask_NC_immediately_OC_2(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1);
uint64_t get_NC_equipped_NC_gas_NC_mask(uint64_t llvm_cbe_arg_0);
uint64_t wait_NC_until_NC_in_NC_valid_NC_gas_NC_mask_NC_state_OC_3(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1);
uint64_t player_NC_in_NC_state_PD_(uint64_t _56);
uint64_t player_NC_force_NC_crouch(void);
uint64_t player_NC_is_NC_crouched_PD_(void);
uint64_t player_NC_force_NC_stand(void);
uint64_t _BC_(uint64_t _57);
uint64_t wait_NC_holster_NC_player_NC_weapon(uint64_t _58, float _59);
uint64_t holster_NC_player_NC_weapon_NC_instantly(float _60, uint64_t _61);
uint64_t wait_NC_npc_NC_holster_NC_weapon(uint64_t _62, uint64_t _63, uint64_t _64, float _65, uint64_t _66);
uint64_t npc_NC_has_NC_weapon_NC_in_NC_hand_PD_(uint64_t _67);
uint64_t wait_NC_npc_NC_set_NC_demeanor(uint64_t _68, uint64_t _69);
uint64_t npc_NC_gas_NC_mask_NC_narrative_NC_mode_PC_f(uint64_t llvm_cbe_arg_0);
uint64_t suspend_NC_idle_NC_gestures_PC_f(uint64_t _70);
uint64_t npc_NC_disable_NC_dialog_NC_look(uint64_t _71, float _72);
uint64_t internal_NC_put_NC_off_NC_gas_NC_mask_NC_immediately(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1);
uint64_t kill_NC_entity(uint64_t _73);
uint64_t spawn_NC_gas_NC_mask(uint64_t llvm_cbe_arg_0);
uint64_t player_NC_gas_NC_mask_NC_narrative_NC_mode_PC_f(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1, uint64_t llvm_cbe_arg_2);
uint64_t player_NC_disable_NC_dialog_NC_look_NC_gestures(float _74);
uint64_t player_NC_disable_NC_squeeze_NC_through_PC_f(void);
uint64_t player_NC_set_NC_narrative_NC_mode_PC_f(uint64_t _75, uint64_t _76, uint64_t _77);
uint64_t joypad_NC_disable_NC_commands(uint64_t _78);
uint64_t internal_NC_put_NC_off_NC_gas_NC_mask(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1);
uint64_t wait_NC_remove_NC_gas_NC_mask(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1, uint64_t llvm_cbe_arg_2);


/* Global Variable Definitions and Initialization */
static const __PREFIXALIGN__(8) struct l_array_11_uint64_t wait_NC_equip_NC_gas_NC_mask_symbol_table __POSTFIXALIGN__(8) = { { INT64_C(-2210386714833194546), UINT64_C(7671589888336862994), INT64_C(3684810824690217768), UINT64_C(6481843155139167450), UINT64_C(6150028017864311825), INT64_C(-3047912596559836939), UINT64_C(4789555171397753330), INT64_C(-3266162430027026367), UINT64_C(6078802017353408438), UINT64_C(5608585023750540989), UINT64_C(13370543512150926718) } };
static const __PREFIXALIGN__(8) struct l_array_26_uint64_t internal_NC_put_NC_on_NC_gas_NC_mask_symbol_table __POSTFIXALIGN__(8) = { { INT64_C(-3047912596559836939), UINT64_C(10788710626492949762), UINT64_C(11873189068114142331), INT64_C(3455792245619945009), UINT64_C(8203816822361639083), UINT64_C(7097052834867614476), INT64_C(267125575217391726), UINT64_C(4766017279285775524), UINT64_C(6532480022340268902), INT64_C(-429226074377614887), UINT64_C(11194266633679187071), INT64_C(-3854484060414110792), UINT64_C(12474852589895261042), UINT64_C(10700394290536318732), UINT64_C(12770083849123373124), UINT64_C(8667864169316382944), 0, UINT64_C(4789555171397753330), UINT64_C(6817768622307059015), INT64_C(3684810824690217768), 1050253722, UINT64_C(9266924145627749149), UINT64_C(10701087590709005978), UINT64_C(8534565298598421705), UINT64_C(6500315221283771953), INT64_C(-2962780980939953026) } };
static const __PREFIXALIGN__(8) struct l_array_19_uint64_t internal_NC_put_NC_on_NC_gas_NC_mask_NC_immediately_symbol_table __POSTFIXALIGN__(8) = { { UINT64_C(10788710626492949762), UINT64_C(11873189068114142331), INT64_C(3455792245619945009), UINT64_C(7097052834867614476), INT64_C(267125575217391726), UINT64_C(4766017279285775524), UINT64_C(12474852589895261042), UINT64_C(6532480022340268902), UINT64_C(4789555171397753330), UINT64_C(6817768622307059015), INT64_C(3684810824690217768), 0, INT64_C(-429226074377614887), INT64_C(1827492438953869695), UINT64_C(11194266633679187071), INT64_C(-3854484060414110792), UINT64_C(9266924145627749149), UINT64_C(10701087590709005978), UINT64_C(8534565298598421705) } };
static const __PREFIXALIGN__(8) struct l_array_1_uint64_t get_NC_equipped_NC_gas_NC_mask_symbol_table __POSTFIXALIGN__(8) = { { UINT64_C(13370543512150926718) } };
static const __PREFIXALIGN__(8) struct l_array_18_uint64_t wait_NC_until_NC_in_NC_valid_NC_gas_NC_mask_NC_state_symbol_table __POSTFIXALIGN__(8) = { { UINT64_C(6532480022340268902), UINT64_C(9443492708748526833), UINT64_C(6740565536053766520), INT64_C(2237451099064635199), INT64_C(-3047912596559836939), UINT64_C(13615456637498249507), INT64_C(1646463116072042322), UINT64_C(8594926419455150747), UINT64_C(12638116831811436492), INT64_C(4007104907368024212), INT64_C(-3092934816467423265), 1065353216, INT64_C(-626103520744544694), 1045220557, UINT64_C(5808793362033684947), INT64_C(3212836864), UINT64_C(7683024912818272899), UINT64_C(8573502661920361253) } };
static const __PREFIXALIGN__(8) struct l_array_5_uint64_t npc_NC_gas_NC_mask_NC_narrative_NC_mode_PC_f_symbol_table __POSTFIXALIGN__(8) = { { UINT64_C(4789555171397753330), UINT64_C(8119835013146602502), UINT64_C(13090054416398793830), UINT64_C(7610916627332680768), 0 } };
static const __PREFIXALIGN__(8) struct l_array_4_uint64_t internal_NC_put_NC_off_NC_gas_NC_mask_NC_immediately_symbol_table __POSTFIXALIGN__(8) = { { UINT64_C(13370543512150926718), UINT64_C(4789555171397753330), INT64_C(-3264453205741318791), INT64_C(-804116741684351865) } };
static const __PREFIXALIGN__(8) struct l_array_6_uint64_t spawn_NC_gas_NC_mask_symbol_table __POSTFIXALIGN__(8) = { { UINT64_C(10788710626492949762), UINT64_C(11873189068114142331), INT64_C(3455792245619945009), UINT64_C(7097052834867614476), INT64_C(267125575217391726), UINT64_C(4766017279285775524) } };
static const __PREFIXALIGN__(8) struct l_array_8_uint64_t player_NC_gas_NC_mask_NC_narrative_NC_mode_PC_f_symbol_table __POSTFIXALIGN__(8) = { { UINT64_C(7238593933813171071), 0, UINT64_C(8165897991486741957), INT64_C(-2826777519175913837), UINT64_C(13090054416398793830), UINT64_C(5008278420455340480), UINT64_C(7121253686161376880), INT64_C(-1699981380503476753) } };
static const __PREFIXALIGN__(8) struct l_array_12_uint64_t internal_NC_put_NC_off_NC_gas_NC_mask_symbol_table __POSTFIXALIGN__(8) = { { INT64_C(-3047912596559836939), UINT64_C(10788710626492949762), UINT64_C(11873189068114142331), INT64_C(3455792245619945009), UINT64_C(13370543512150926718), UINT64_C(8203816822361639083), UINT64_C(9266924145627749149), UINT64_C(10701087590709005978), UINT64_C(8534565298598421705), UINT64_C(6500315221283771953), INT64_C(-2962780980939953026), INT64_C(-804116741684351865) } };
static const __PREFIXALIGN__(8) struct l_array_10_uint64_t wait_NC_remove_NC_gas_NC_mask_symbol_table __POSTFIXALIGN__(8) = { { INT64_C(-2210386714833194546), UINT64_C(7671589888336862994), INT64_C(3684810824690217768), UINT64_C(6481843155139167450), UINT64_C(6150028017864311825), INT64_C(-3047912596559836939), UINT64_C(4789555171397753330), INT64_C(-3325057813225958160), UINT64_C(9887213260439779492), UINT64_C(11836123545942477555) } };


/* LLVM Intrinsic Builtin Function Bodies */
static __forceinline uint64_t llvm_add_u64(uint64_t a, uint64_t b) {
  uint64_t r = a + b;
  return r;
}
static __forceinline uint64_t llvm_mul_u64(uint64_t a, uint64_t b) {
  uint64_t r = a * b;
  return r;
}


/* Function Bodies */

uint64_t wait_NC_equip_NC_gas_NC_mask(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1, uint64_t llvm_cbe_arg_2) {
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r0 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r1 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r2 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r3 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r4 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r5 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r6 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r7 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r8 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r9 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r10 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r11 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r12 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r13 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r14 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r15 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r16 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r17 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r18 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r19 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r20 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r21 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r22 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r23 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r24 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r25 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r26 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r27 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r28 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r29 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r30 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r31 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r32 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r33 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r34 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r35 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r36 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r37 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r38 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r39 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r40 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r41 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r42 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r43 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r44 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r45 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r46 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r47 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r48 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r49 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r50 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r51 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r52 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r53 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r54 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r55 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r56 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r57 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r58 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r59 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r60 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r61 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r62 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r63 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r64 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r65 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r66 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r67 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r68 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r69 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r70 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r71 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r72 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r73 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r74 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r75 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r76 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r77 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r78 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r79 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r80 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r81 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r82 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r83 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r84 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r85 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r86 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r87 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r88 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r89 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r90 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r91 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r92 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r93 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r94 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r95 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r96 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r97 __POSTFIXALIGN__(8);    /* Address-exposed local */
  uint64_t llvm_cbe___Move_0___load_op1;
  uint64_t llvm_cbe___Move_1___load_op1;
  uint64_t llvm_cbe___Move_2___load_op1;
  uint64_t llvm_cbe___Move_4___load_op1;
  uint64_t llvm_cbe___LoadStaticU64Imm_5___st_load;
  uint64_t llvm_cbe___Move_9___load_op1;
  uint64_t llvm_cbe___Move_10___load_op1;
  void* llvm_cbe___CallFf_11___callee;
  uint64_t llvm_cbe___CallFf_11___arg0;
  uint64_t llvm_cbe___CallFf_11___arg1;
  uint64_t llvm_cbe___CallFf_11___call;
  uint64_t llvm_cbe___Move_12___load_op1;
  uint64_t llvm_cbe___Move_13___load_op1;
  uint64_t llvm_cbe___Move_14___load_op1;
  void* llvm_cbe___CallFf_15___callee;
  uint64_t llvm_cbe___CallFf_15___arg0;
  uint64_t llvm_cbe___CallFf_15___arg1;
  uint64_t llvm_cbe___CallFf_15___arg2;
  uint64_t llvm_cbe___CallFf_15___call;
  uint64_t llvm_cbe___LoadStaticU64Imm_17___st_load;
  uint64_t llvm_cbe___Move_18___load_op1;
  void* llvm_cbe___Call_19___callee;
  uint64_t llvm_cbe___Call_19___arg0;
  uint64_t llvm_cbe___Call_19___call;
  uint64_t llvm_cbe___OpLogNot_20___load_op1;
  bool llvm_cbe___BranchIfNot_21___cond;
  void* llvm_cbe___CallFf_23___callee;
  uint64_t llvm_cbe___CallFf_23___call;
  uint64_t llvm_cbe___LoadStaticU64Imm_26___st_load;
  uint64_t llvm_cbe___LoadStaticU64Imm_27___st_load;
  uint64_t llvm_cbe___Move_30___load_op1;
  uint64_t llvm_cbe___Move_31___load_op1;
  uint64_t llvm_cbe___Move_32___load_op1;
  void* llvm_cbe___CallFf_33___callee;
  uint64_t llvm_cbe___CallFf_33___arg0;
  uint64_t llvm_cbe___CallFf_33___arg1;
  uint64_t llvm_cbe___CallFf_33___call;
  uint64_t llvm_cbe___Move_34___load_op1;
  uint64_t llvm_cbe___Move_35___load_op1;
  uint64_t llvm_cbe___Move_36___load_op1;
  void* llvm_cbe___CallFf_37___callee;
  uint64_t llvm_cbe___CallFf_37___arg0;
  uint64_t llvm_cbe___CallFf_37___arg1;
  uint64_t llvm_cbe___CallFf_37___arg2;
  uint64_t llvm_cbe___CallFf_37___call;
  uint64_t llvm_cbe___Move_38___load_op1;
  uint64_t llvm_cbe___BranchIfNot_39___cond;
  uint64_t llvm_cbe___Move_41___load_op1;
  uint64_t llvm_cbe___Move_42___load_op1;
  uint64_t llvm_cbe___Move_43___load_op1;
  uint64_t llvm_cbe___Move_44___load_op1;
  void* llvm_cbe___Call_45___callee;
  uint64_t llvm_cbe___Call_45___arg0;
  uint64_t llvm_cbe___Call_45___arg1;
  uint64_t llvm_cbe___Call_45___call;
  uint64_t llvm_cbe___Move_48___load_op1;
  uint64_t llvm_cbe___Move_49___load_op1;
  uint64_t llvm_cbe___Move_50___load_op1;
  uint64_t llvm_cbe___Move_51___load_op1;
  void* llvm_cbe___Call_52___callee;
  uint64_t llvm_cbe___Call_52___arg0;
  uint64_t llvm_cbe___Call_52___arg1;
  uint64_t llvm_cbe___Call_52___call;
  uint64_t llvm_cbe___LoadStaticU64Imm_54___st_load;
  uint64_t llvm_cbe___Move_55___load_op1;
  void* llvm_cbe___Call_56___callee;
  uint64_t llvm_cbe___Call_56___arg0;
  uint64_t llvm_cbe___Call_56___call;
  uint64_t llvm_cbe___OpLogNot_57___load_op1;
  bool llvm_cbe___BranchIfNot_58___cond;
  void* llvm_cbe___CallFf_60___callee;
  uint64_t llvm_cbe___CallFf_60___call;
  uint64_t llvm_cbe___Move_63___load_op1;
  uint64_t llvm_cbe___Move_64___load_op1;
  void* llvm_cbe___CallFf_65___callee;
  uint64_t llvm_cbe___CallFf_65___arg0;
  uint64_t llvm_cbe___CallFf_65___call;
  uint64_t llvm_cbe___Return_66___retval;

  llvm_cbe_r49 = llvm_cbe_arg_0;
  llvm_cbe_r50 = llvm_cbe_arg_1;
  llvm_cbe_r51 = llvm_cbe_arg_2;
  llvm_cbe___Move_0___load_op1 = llvm_cbe_r49;
  llvm_cbe_r0 = llvm_cbe___Move_0___load_op1;
  llvm_cbe___Move_1___load_op1 = llvm_cbe_r50;
  llvm_cbe_r1 = llvm_cbe___Move_1___load_op1;
  llvm_cbe___Move_2___load_op1 = llvm_cbe_r51;
  llvm_cbe_r2 = llvm_cbe___Move_2___load_op1;
  *((void**)&llvm_cbe_r3) = ((void*)&fact_NC_set);
  llvm_cbe___Move_4___load_op1 = llvm_cbe_r0;
  llvm_cbe_r4 = llvm_cbe___Move_4___load_op1;
  llvm_cbe___LoadStaticU64Imm_5___st_load = *(uint64_t*)(((&(&wait_NC_equip_NC_gas_NC_mask_symbol_table)->array[((int64_t)1)])));
  llvm_cbe_r5 = llvm_cbe___LoadStaticU64Imm_5___st_load;
  *((void**)&llvm_cbe_r6) = ((void*)&new_NC_boxed_NC_value);
  llvm_cbe_r7 = 1;
  llvm_cbe_r8 = 1;
  llvm_cbe___Move_9___load_op1 = llvm_cbe_r7;
  llvm_cbe_r49 = llvm_cbe___Move_9___load_op1;
  llvm_cbe___Move_10___load_op1 = llvm_cbe_r8;
  llvm_cbe_r50 = llvm_cbe___Move_10___load_op1;
  llvm_cbe___CallFf_11___callee = *((void**)&llvm_cbe_r6);
  llvm_cbe___CallFf_11___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_11___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_11___call = ((l_fptr_1*)(void*)llvm_cbe___CallFf_11___callee)(llvm_cbe___CallFf_11___arg0, llvm_cbe___CallFf_11___arg1);
  llvm_cbe_r6 = llvm_cbe___CallFf_11___call;
  llvm_cbe___Move_12___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_12___load_op1;
  llvm_cbe___Move_13___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_13___load_op1;
  llvm_cbe___Move_14___load_op1 = llvm_cbe_r6;
  llvm_cbe_r51 = llvm_cbe___Move_14___load_op1;
  llvm_cbe___CallFf_15___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_15___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_15___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_15___arg2 = llvm_cbe_r51;
  llvm_cbe___CallFf_15___call = ((l_fptr_2*)(void*)llvm_cbe___CallFf_15___callee)(llvm_cbe___CallFf_15___arg0, llvm_cbe___CallFf_15___arg1, llvm_cbe___CallFf_15___arg2);
  llvm_cbe_r3 = llvm_cbe___CallFf_15___call;
  goto llvm_cbe_bb1;

  do {     /* Syntactic loop 'bb1' to make GCC happy */
llvm_cbe_bb1:
  *((void**)&llvm_cbe_r3) = ((void*)&is_NC_script_NC_running_PD_);
  llvm_cbe___LoadStaticU64Imm_17___st_load = *(uint64_t*)(((&(&wait_NC_equip_NC_gas_NC_mask_symbol_table)->array[((int64_t)4)])));
  llvm_cbe_r4 = llvm_cbe___LoadStaticU64Imm_17___st_load;
  llvm_cbe___Move_18___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_18___load_op1;
  llvm_cbe___Call_19___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___Call_19___arg0 = llvm_cbe_r49;
  llvm_cbe___Call_19___call = ((l_fptr_3*)(void*)llvm_cbe___Call_19___callee)(llvm_cbe___Call_19___arg0);
  llvm_cbe_r3 = llvm_cbe___Call_19___call;
  llvm_cbe___OpLogNot_20___load_op1 = llvm_cbe_r3;
  llvm_cbe_r3 = (llvm_cbe___OpLogNot_20___load_op1 ^ -1);
  llvm_cbe___BranchIfNot_21___cond = *((bool*)&llvm_cbe_r3);
  if ((llvm_cbe___BranchIfNot_21___cond != 0)) {
    goto llvm_cbe_bb2;
  } else {
    goto llvm_cbe_bb3;
  }

llvm_cbe_bb2:
  *((void**)&llvm_cbe_r3) = ((void*)&wait_NC_one_NC_frame);
  llvm_cbe___CallFf_23___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_23___call = ((l_fptr_4*)(void*)llvm_cbe___CallFf_23___callee)();
  llvm_cbe_r3 = llvm_cbe___CallFf_23___call;
  goto llvm_cbe_bb1;

  } while (1); /* end of syntactic loop 'bb1' */
llvm_cbe_bb3:
  *((void**)&llvm_cbe_r3) = ((void*)&send_NC_event);
  llvm_cbe___LoadStaticU64Imm_26___st_load = *(uint64_t*)(((&(&wait_NC_equip_NC_gas_NC_mask_symbol_table)->array[((int64_t)7)])));
  llvm_cbe_r4 = llvm_cbe___LoadStaticU64Imm_26___st_load;
  llvm_cbe___LoadStaticU64Imm_27___st_load = *(uint64_t*)(((&(&wait_NC_equip_NC_gas_NC_mask_symbol_table)->array[((int64_t)4)])));
  llvm_cbe_r5 = llvm_cbe___LoadStaticU64Imm_27___st_load;
  *((void**)&llvm_cbe_r6) = ((void*)&new_NC_boxed_NC_value);
  llvm_cbe_r7 = 7;
  llvm_cbe___Move_30___load_op1 = llvm_cbe_r0;
  llvm_cbe_r8 = llvm_cbe___Move_30___load_op1;
  llvm_cbe___Move_31___load_op1 = llvm_cbe_r7;
  llvm_cbe_r49 = llvm_cbe___Move_31___load_op1;
  llvm_cbe___Move_32___load_op1 = llvm_cbe_r8;
  llvm_cbe_r50 = llvm_cbe___Move_32___load_op1;
  llvm_cbe___CallFf_33___callee = *((void**)&llvm_cbe_r6);
  llvm_cbe___CallFf_33___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_33___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_33___call = ((l_fptr_5*)(void*)llvm_cbe___CallFf_33___callee)(llvm_cbe___CallFf_33___arg0, llvm_cbe___CallFf_33___arg1);
  llvm_cbe_r6 = llvm_cbe___CallFf_33___call;
  llvm_cbe___Move_34___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_34___load_op1;
  llvm_cbe___Move_35___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_35___load_op1;
  llvm_cbe___Move_36___load_op1 = llvm_cbe_r6;
  llvm_cbe_r51 = llvm_cbe___Move_36___load_op1;
  llvm_cbe___CallFf_37___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_37___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_37___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_37___arg2 = llvm_cbe_r51;
  llvm_cbe___CallFf_37___call = ((l_fptr_6*)(void*)llvm_cbe___CallFf_37___callee)(llvm_cbe___CallFf_37___arg0, llvm_cbe___CallFf_37___arg1, llvm_cbe___CallFf_37___arg2);
  llvm_cbe_r3 = llvm_cbe___CallFf_37___call;
  llvm_cbe___Move_38___load_op1 = llvm_cbe_r2;
  llvm_cbe_r3 = llvm_cbe___Move_38___load_op1;
  llvm_cbe___BranchIfNot_39___cond = llvm_cbe_r3;
  if ((llvm_cbe___BranchIfNot_39___cond != UINT64_C(0))) {
    goto llvm_cbe_bb4;
  } else {
    goto llvm_cbe_bb5;
  }

llvm_cbe_bb4:
  *((void**)&llvm_cbe_r3) = ((void*)&internal_NC_put_NC_on_NC_gas_NC_mask_NC_immediately);
  llvm_cbe___Move_41___load_op1 = llvm_cbe_r0;
  llvm_cbe_r4 = llvm_cbe___Move_41___load_op1;
  llvm_cbe___Move_42___load_op1 = llvm_cbe_r1;
  llvm_cbe_r5 = llvm_cbe___Move_42___load_op1;
  llvm_cbe___Move_43___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_43___load_op1;
  llvm_cbe___Move_44___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_44___load_op1;
  llvm_cbe___Call_45___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___Call_45___arg0 = llvm_cbe_r49;
  llvm_cbe___Call_45___arg1 = llvm_cbe_r50;
  llvm_cbe___Call_45___call = ((l_fptr_7*)(void*)llvm_cbe___Call_45___callee)(llvm_cbe___Call_45___arg0, llvm_cbe___Call_45___arg1);
  llvm_cbe_r3 = llvm_cbe___Call_45___call;
  goto llvm_cbe_bb6;

llvm_cbe_bb5:
  *((void**)&llvm_cbe_r3) = ((void*)&internal_NC_put_NC_on_NC_gas_NC_mask);
  llvm_cbe___Move_48___load_op1 = llvm_cbe_r0;
  llvm_cbe_r4 = llvm_cbe___Move_48___load_op1;
  llvm_cbe___Move_49___load_op1 = llvm_cbe_r1;
  llvm_cbe_r5 = llvm_cbe___Move_49___load_op1;
  llvm_cbe___Move_50___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_50___load_op1;
  llvm_cbe___Move_51___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_51___load_op1;
  llvm_cbe___Call_52___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___Call_52___arg0 = llvm_cbe_r49;
  llvm_cbe___Call_52___arg1 = llvm_cbe_r50;
  llvm_cbe___Call_52___call = ((l_fptr_8*)(void*)llvm_cbe___Call_52___callee)(llvm_cbe___Call_52___arg0, llvm_cbe___Call_52___arg1);
  llvm_cbe_r3 = llvm_cbe___Call_52___call;
  goto llvm_cbe_bb6;

  do {     /* Syntactic loop 'bb6' to make GCC happy */
llvm_cbe_bb6:
  *((void**)&llvm_cbe_r3) = ((void*)&is_NC_script_NC_running_PD_);
  llvm_cbe___LoadStaticU64Imm_54___st_load = *(uint64_t*)(((&(&wait_NC_equip_NC_gas_NC_mask_symbol_table)->array[((int64_t)4)])));
  llvm_cbe_r4 = llvm_cbe___LoadStaticU64Imm_54___st_load;
  llvm_cbe___Move_55___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_55___load_op1;
  llvm_cbe___Call_56___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___Call_56___arg0 = llvm_cbe_r49;
  llvm_cbe___Call_56___call = ((l_fptr_9*)(void*)llvm_cbe___Call_56___callee)(llvm_cbe___Call_56___arg0);
  llvm_cbe_r3 = llvm_cbe___Call_56___call;
  llvm_cbe___OpLogNot_57___load_op1 = llvm_cbe_r3;
  llvm_cbe_r3 = (llvm_cbe___OpLogNot_57___load_op1 ^ -1);
  llvm_cbe___BranchIfNot_58___cond = *((bool*)&llvm_cbe_r3);
  if ((llvm_cbe___BranchIfNot_58___cond != 0)) {
    goto llvm_cbe_bb7;
  } else {
    goto llvm_cbe_bb8;
  }

llvm_cbe_bb7:
  *((void**)&llvm_cbe_r3) = ((void*)&wait_NC_one_NC_frame);
  llvm_cbe___CallFf_60___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_60___call = ((l_fptr_10*)(void*)llvm_cbe___CallFf_60___callee)();
  llvm_cbe_r3 = llvm_cbe___CallFf_60___call;
  goto llvm_cbe_bb6;

  } while (1); /* end of syntactic loop 'bb6' */
llvm_cbe_bb8:
  *((void**)&llvm_cbe_r3) = ((void*)&get_NC_gas_NC_mask);
  llvm_cbe___Move_63___load_op1 = llvm_cbe_r0;
  llvm_cbe_r4 = llvm_cbe___Move_63___load_op1;
  llvm_cbe___Move_64___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_64___load_op1;
  llvm_cbe___CallFf_65___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_65___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_65___call = ((l_fptr_11*)(void*)llvm_cbe___CallFf_65___callee)(llvm_cbe___CallFf_65___arg0);
  llvm_cbe_r3 = llvm_cbe___CallFf_65___call;
  llvm_cbe___Return_66___retval = llvm_cbe_r3;
  return llvm_cbe___Return_66___retval;
}


uint64_t internal_NC_put_NC_on_NC_gas_NC_mask_OC_1(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1) {
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r0 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r1 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r2 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r3 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r4 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r5 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r6 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r7 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r8 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r9 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r10 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r11 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r12 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r13 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r14 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r15 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r16 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r17 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r18 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r19 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r20 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r21 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r22 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r23 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r24 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r25 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r26 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r27 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r28 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r29 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r30 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r31 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r32 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r33 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r34 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r35 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r36 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r37 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r38 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r39 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r40 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r41 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r42 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r43 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r44 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r45 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r46 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r47 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r48 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r49 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r50 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r51 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r52 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r53 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r54 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r55 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r56 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r57 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r58 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r59 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r60 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r61 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r62 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r63 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r64 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r65 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r66 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r67 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r68 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r69 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r70 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r71 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r72 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r73 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r74 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r75 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r76 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r77 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r78 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r79 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r80 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r81 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r82 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r83 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r84 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r85 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r86 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r87 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r88 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r89 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r90 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r91 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r92 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r93 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r94 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r95 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r96 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r97 __POSTFIXALIGN__(8);    /* Address-exposed local */
  uint64_t llvm_cbe___Move_0___load_op1;
  uint64_t llvm_cbe___Move_1___load_op1;
  void* llvm_cbe___CallFf_3___callee;
  uint64_t llvm_cbe___CallFf_3___call;
  uint64_t llvm_cbe___Move_6___load_op1;
  uint64_t llvm_cbe___Move_7___load_op1;
  void* llvm_cbe___CallFf_8___callee;
  uint64_t llvm_cbe___CallFf_8___arg0;
  uint64_t llvm_cbe___CallFf_8___call;
  uint64_t llvm_cbe___LoadStaticU64Imm_9___st_load;
  uint64_t llvm_cbe___Move_10___load_op1;
  uint64_t llvm_cbe___Move_11___load_op1;
  void* llvm_cbe___CallFf_12___callee;
  uint64_t llvm_cbe___CallFf_12___arg0;
  uint64_t llvm_cbe___CallFf_12___arg1;
  uint64_t llvm_cbe___CallFf_12___call;
  uint64_t llvm_cbe___Move_14___load_op1;
  uint64_t llvm_cbe___Move_15___load_op1;
  void* llvm_cbe___AssertPointer_16___ptr;
  void* llvm_cbe___IAddImm_17___load_lhs;
  void* llvm_cbe___LoadU8_18___addr;
  uint8_t llvm_cbe___LoadU8_18___load;
  uint64_t llvm_cbe___Move_19___load_op1;
  uint64_t llvm_cbe___Move_20___load_op1;
  void* llvm_cbe___Call_21___callee;
  uint64_t llvm_cbe___Call_21___arg0;
  uint64_t llvm_cbe___Call_21___arg1;
  uint64_t llvm_cbe___Call_21___call;
  uint64_t llvm_cbe___Move_22___load_op1;
  uint64_t llvm_cbe___IEqual_24___load_lhs;
  uint64_t llvm_cbe___IEqual_24___load_rhs;
  bool llvm_cbe___BranchIfNot_25___cond;
  uint64_t llvm_cbe___Move_27___load_op1;
  void* llvm_cbe___AssertPointer_28___ptr;
  void* llvm_cbe___IAddImm_29___load_lhs;
  void* llvm_cbe___LoadU64_30___addr;
  uint64_t llvm_cbe___LoadU64_30___load;
  uint64_t llvm_cbe___LoadStaticU64Imm_31___st_load;
  uint64_t llvm_cbe___Move_39___load_op1;
  uint64_t llvm_cbe___Move_40___load_op1;
  uint64_t llvm_cbe___Move_41___load_op1;
  uint64_t llvm_cbe___Move_42___load_op1;
  uint64_t llvm_cbe___Move_43___load_op1;
  uint64_t llvm_cbe___Move_44___load_op1;
  uint64_t llvm_cbe___Move_45___load_op1;
  uint64_t llvm_cbe___Move_46___load_op1;
  uint64_t llvm_cbe___Move_47___load_op1;
  void* llvm_cbe___CallFf_48___callee;
  uint64_t llvm_cbe___CallFf_48___arg0;
  uint64_t llvm_cbe___CallFf_48___arg1;
  uint64_t llvm_cbe___CallFf_48___arg2;
  uint64_t llvm_cbe___CallFf_48___arg3;
  uint64_t llvm_cbe___CallFf_48___arg4;
  uint64_t llvm_cbe___CallFf_48___arg5;
  uint64_t llvm_cbe___CallFf_48___arg6;
  uint64_t llvm_cbe___CallFf_48___arg7;
  uint64_t llvm_cbe___CallFf_48___arg8;
  uint64_t llvm_cbe___CallFf_48___call;
  uint64_t llvm_cbe___Move_49___load_op1;
  uint64_t llvm_cbe___Move_50___load_op1;
  uint64_t llvm_cbe___Move_54___load_op1;
  uint64_t llvm_cbe___Move_56___load_op1;
  uint64_t llvm_cbe___Move_57___load_op1;
  void* llvm_cbe___CallFf_58___callee;
  uint64_t llvm_cbe___CallFf_58___arg0;
  uint64_t llvm_cbe___CallFf_58___arg1;
  uint64_t llvm_cbe___CallFf_58___call;
  uint64_t llvm_cbe___Move_60___load_op1;
  uint64_t llvm_cbe___Move_61___load_op1;
  void* llvm_cbe___CallFf_62___callee;
  uint64_t llvm_cbe___CallFf_62___arg0;
  uint64_t llvm_cbe___CallFf_62___call;
  uint64_t llvm_cbe___BranchIfNot_63___cond;
  uint64_t llvm_cbe___Move_65___load_op1;
  uint64_t llvm_cbe___Move_67___load_op1;
  uint64_t llvm_cbe___Move_68___load_op1;
  void* llvm_cbe___CallFf_69___callee;
  uint64_t llvm_cbe___CallFf_69___arg0;
  uint64_t llvm_cbe___CallFf_69___arg1;
  uint64_t llvm_cbe___CallFf_69___call;
  uint64_t llvm_cbe___Move_71___load_op1;
  uint64_t llvm_cbe___Move_73___load_op1;
  uint64_t llvm_cbe___Move_74___load_op1;
  void* llvm_cbe___CallFf_75___callee;
  uint64_t llvm_cbe___CallFf_75___arg0;
  uint64_t llvm_cbe___CallFf_75___arg1;
  uint64_t llvm_cbe___CallFf_75___call;
  uint64_t llvm_cbe___Move_78___load_op1;
  uint64_t llvm_cbe___Move_80___load_op1;
  uint64_t llvm_cbe___Move_81___load_op1;
  void* llvm_cbe___CallFf_82___callee;
  uint64_t llvm_cbe___CallFf_82___arg0;
  uint64_t llvm_cbe___CallFf_82___arg1;
  uint64_t llvm_cbe___CallFf_82___call;
  uint64_t llvm_cbe___Move_84___load_op1;
  uint64_t llvm_cbe___Move_85___load_op1;
  uint64_t llvm_cbe___Move_86___load_op1;
  uint64_t llvm_cbe___Move_87___load_op1;
  void* llvm_cbe___CallFf_88___callee;
  uint64_t llvm_cbe___CallFf_88___arg0;
  uint64_t llvm_cbe___CallFf_88___arg1;
  uint64_t llvm_cbe___CallFf_88___call;
  uint64_t llvm_cbe___Move_90___load_op1;
  uint64_t llvm_cbe___Move_92___load_op1;
  uint64_t llvm_cbe___Move_93___load_op1;
  void* llvm_cbe___CallFf_94___callee;
  uint64_t llvm_cbe___CallFf_94___arg0;
  uint64_t llvm_cbe___CallFf_94___arg1;
  uint64_t llvm_cbe___CallFf_94___call;
  uint64_t llvm_cbe___Move_96___load_op1;
  uint64_t llvm_cbe___Move_97___load_op1;
  void* llvm_cbe___CallFf_98___callee;
  uint64_t llvm_cbe___CallFf_98___arg0;
  uint64_t llvm_cbe___CallFf_98___call;
  uint64_t llvm_cbe___OpLogNot_99___load_op1;
  bool llvm_cbe___BranchIfNot_100___cond;
  void* llvm_cbe___CallFf_102___callee;
  uint64_t llvm_cbe___CallFf_102___call;
  uint64_t llvm_cbe___Move_106___load_op1;
  uint64_t llvm_cbe___Move_107___load_op1;
  uint64_t llvm_cbe___LoadStaticU64Imm_108___st_load;
  float llvm_cbe___LoadStaticFloatImm_109___st_load;
  uint64_t llvm_cbe___Move_110___load_op1;
  uint64_t llvm_cbe___Move_111___load_op1;
  uint64_t llvm_cbe___Move_112___load_op1;
  float llvm_cbe___Move_113___load_op1;
  void* llvm_cbe___CallFf_114___callee;
  uint64_t llvm_cbe___CallFf_114___arg0;
  uint64_t llvm_cbe___CallFf_114___arg1;
  uint64_t llvm_cbe___CallFf_114___arg2;
  float llvm_cbe___CallFf_114___arg3;
  uint64_t llvm_cbe___CallFf_114___call;
  uint64_t llvm_cbe___LoadStaticU64Imm_116___st_load;
  uint64_t llvm_cbe___Move_117___load_op1;
  float llvm_cbe___LoadStaticFloatImm_120___st_load;
  uint64_t llvm_cbe___Move_121___load_op1;
  float llvm_cbe___Move_122___load_op1;
  void* llvm_cbe___CallFf_123___callee;
  uint64_t llvm_cbe___CallFf_123___arg0;
  float llvm_cbe___CallFf_123___arg1;
  uint64_t llvm_cbe___CallFf_123___call;
  uint64_t llvm_cbe___Move_124___load_op1;
  uint64_t llvm_cbe___Move_125___load_op1;
  uint64_t llvm_cbe___Move_126___load_op1;
  void* llvm_cbe___CallFf_127___callee;
  uint64_t llvm_cbe___CallFf_127___arg0;
  uint64_t llvm_cbe___CallFf_127___arg1;
  uint64_t llvm_cbe___CallFf_127___arg2;
  uint64_t llvm_cbe___CallFf_127___call;
  uint64_t llvm_cbe___Move_129___load_op1;
  uint64_t llvm_cbe___Move_130___load_op1;
  void* llvm_cbe___AssertPointer_131___ptr;
  void* llvm_cbe___IAddImm_132___load_lhs;
  void* llvm_cbe___LoadU64_133___addr;
  uint64_t llvm_cbe___LoadU64_133___load;
  uint64_t llvm_cbe___LoadStaticU64Imm_135___st_load;
  uint64_t llvm_cbe___Move_139___load_op1;
  uint64_t llvm_cbe___Move_140___load_op1;
  uint64_t llvm_cbe___Move_141___load_op1;
  uint64_t llvm_cbe___Move_142___load_op1;
  void* llvm_cbe___CallFf_143___callee;
  uint64_t llvm_cbe___CallFf_143___arg0;
  uint64_t llvm_cbe___CallFf_143___arg1;
  uint64_t llvm_cbe___CallFf_143___arg2;
  uint64_t llvm_cbe___CallFf_143___arg3;
  uint64_t llvm_cbe___CallFf_143___call;
  uint64_t llvm_cbe___Move_144___load_op1;
  uint64_t llvm_cbe___Move_145___load_op1;
  uint64_t llvm_cbe___Move_146___load_op1;
  uint64_t llvm_cbe___Move_147___load_op1;
  uint64_t llvm_cbe___Move_148___load_op1;
  void* llvm_cbe___CallFf_149___callee;
  uint64_t llvm_cbe___CallFf_149___arg0;
  uint64_t llvm_cbe___CallFf_149___arg1;
  uint64_t llvm_cbe___CallFf_149___arg2;
  uint64_t llvm_cbe___CallFf_149___call;
  uint64_t llvm_cbe___Move_151___load_op1;
  uint64_t llvm_cbe___Move_152___load_op1;
  void* llvm_cbe___AssertPointer_153___ptr;
  void* llvm_cbe___LoadU64_154___addr;
  uint64_t llvm_cbe___LoadU64_154___load;
  void* llvm_cbe___CallFf_156___callee;
  uint64_t llvm_cbe___CallFf_156___call;
  uint64_t llvm_cbe___Move_157___load_op1;
  uint64_t llvm_cbe___IAddImm_158___load_lhs;
  uint64_t llvm_cbe___StoreU8_160___value;
  void* llvm_cbe___StoreU8_160___addr;
  uint64_t llvm_cbe___Move_161___load_op1;
  uint64_t llvm_cbe___IAddImm_162___load_lhs;
  uint64_t llvm_cbe___StoreI32_164___value;
  void* llvm_cbe___StoreI32_164___addr;
  uint64_t llvm_cbe___Move_165___load_op1;
  uint64_t llvm_cbe___IAddImm_166___load_lhs;
  uint64_t llvm_cbe___StoreU8_168___value;
  void* llvm_cbe___StoreU8_168___addr;
  uint64_t llvm_cbe___Move_169___load_op1;
  uint64_t llvm_cbe___IAddImm_170___load_lhs;
  uint64_t llvm_cbe___StoreU8_172___value;
  void* llvm_cbe___StoreU8_172___addr;
  uint64_t llvm_cbe___Move_173___load_op1;
  uint64_t llvm_cbe___Move_174___load_op1;
  uint64_t llvm_cbe___Move_175___load_op1;
  uint64_t llvm_cbe___Move_176___load_op1;
  uint64_t llvm_cbe___Move_177___load_op1;
  void* llvm_cbe___CallFf_178___callee;
  uint64_t llvm_cbe___CallFf_178___arg0;
  uint64_t llvm_cbe___CallFf_178___arg1;
  uint64_t llvm_cbe___CallFf_178___arg2;
  uint64_t llvm_cbe___CallFf_178___call;
  uint64_t llvm_cbe___IEqual_180___load_lhs;
  uint64_t llvm_cbe___IEqual_180___load_rhs;
  bool llvm_cbe___OpLogNot_181___load_op1;
  bool llvm_cbe___BranchIfNot_182___cond;
  void* llvm_cbe___CallFf_184___callee;
  uint64_t llvm_cbe___CallFf_184___call;
  uint64_t llvm_cbe___Move_187___load_op1;
  uint64_t llvm_cbe___Move_188___load_op1;
  void* llvm_cbe___AssertPointer_189___ptr;
  void* llvm_cbe___IAddImm_190___load_lhs;
  void* llvm_cbe___LoadU64_191___addr;
  uint64_t llvm_cbe___LoadU64_191___load;
  uint64_t llvm_cbe___LoadStaticU64Imm_193___st_load;
  uint64_t llvm_cbe___Move_197___load_op1;
  uint64_t llvm_cbe___Move_198___load_op1;
  uint64_t llvm_cbe___Move_199___load_op1;
  uint64_t llvm_cbe___Move_200___load_op1;
  void* llvm_cbe___CallFf_201___callee;
  uint64_t llvm_cbe___CallFf_201___arg0;
  uint64_t llvm_cbe___CallFf_201___arg1;
  uint64_t llvm_cbe___CallFf_201___arg2;
  uint64_t llvm_cbe___CallFf_201___arg3;
  uint64_t llvm_cbe___CallFf_201___call;
  uint64_t llvm_cbe___Move_202___load_op1;
  uint64_t llvm_cbe___Move_203___load_op1;
  uint64_t llvm_cbe___Move_204___load_op1;
  uint64_t llvm_cbe___Move_205___load_op1;
  uint64_t llvm_cbe___Move_206___load_op1;
  void* llvm_cbe___CallFf_207___callee;
  uint64_t llvm_cbe___CallFf_207___arg0;
  uint64_t llvm_cbe___CallFf_207___arg1;
  uint64_t llvm_cbe___CallFf_207___arg2;
  uint64_t llvm_cbe___CallFf_207___call;
  uint64_t llvm_cbe___Move_208___load_op1;
  uint64_t llvm_cbe___Return_209___retval;

  llvm_cbe_r49 = llvm_cbe_arg_0;
  llvm_cbe_r50 = llvm_cbe_arg_1;
  llvm_cbe___Move_0___load_op1 = llvm_cbe_r49;
  llvm_cbe_r0 = llvm_cbe___Move_0___load_op1;
  llvm_cbe___Move_1___load_op1 = llvm_cbe_r50;
  llvm_cbe_r1 = llvm_cbe___Move_1___load_op1;
  *((void**)&llvm_cbe_r2) = ((void*)&wait_NC_one_NC_frame);
  llvm_cbe___CallFf_3___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_3___call = ((l_fptr_12*)(void*)llvm_cbe___CallFf_3___callee)();
  llvm_cbe_r2 = llvm_cbe___CallFf_3___call;
  *((void**)&llvm_cbe_r2) = ((void*)&lookup_NC_symbol);
  *((void**)&llvm_cbe_r3) = ((void*)&get_NC_gas_NC_mask_NC_setting_NC_id);
  llvm_cbe___Move_6___load_op1 = llvm_cbe_r0;
  llvm_cbe_r4 = llvm_cbe___Move_6___load_op1;
  llvm_cbe___Move_7___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_7___load_op1;
  llvm_cbe___CallFf_8___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_8___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_8___call = ((l_fptr_13*)(void*)llvm_cbe___CallFf_8___callee)(llvm_cbe___CallFf_8___arg0);
  llvm_cbe_r3 = llvm_cbe___CallFf_8___call;
  llvm_cbe___LoadStaticU64Imm_9___st_load = *(uint64_t*)(((&(&internal_NC_put_NC_on_NC_gas_NC_mask_symbol_table)->array[((int64_t)3)])));
  llvm_cbe_r4 = llvm_cbe___LoadStaticU64Imm_9___st_load;
  llvm_cbe___Move_10___load_op1 = llvm_cbe_r3;
  llvm_cbe_r49 = llvm_cbe___Move_10___load_op1;
  llvm_cbe___Move_11___load_op1 = llvm_cbe_r4;
  llvm_cbe_r50 = llvm_cbe___Move_11___load_op1;
  llvm_cbe___CallFf_12___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_12___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_12___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_12___call = ((l_fptr_14*)(void*)llvm_cbe___CallFf_12___callee)(llvm_cbe___CallFf_12___arg0, llvm_cbe___CallFf_12___arg1);
  llvm_cbe_r2 = llvm_cbe___CallFf_12___call;
  *((void**)&llvm_cbe_r3) = ((void*)&wait_NC_until_NC_in_NC_valid_NC_gas_NC_mask_NC_state);
  llvm_cbe___Move_14___load_op1 = llvm_cbe_r0;
  llvm_cbe_r4 = llvm_cbe___Move_14___load_op1;
  llvm_cbe___Move_15___load_op1 = llvm_cbe_r2;
  llvm_cbe_r5 = llvm_cbe___Move_15___load_op1;
  llvm_cbe___AssertPointer_16___ptr = *((void**)&llvm_cbe_r5);
  llvm_cbe___IAddImm_17___load_lhs = *((void**)&llvm_cbe_r5);
  *((void**)&llvm_cbe_r5) = (((&((uint8_t*)llvm_cbe___IAddImm_17___load_lhs)[((int8_t)56)])));
  llvm_cbe___LoadU8_18___addr = *((void**)&llvm_cbe_r5);
  llvm_cbe___LoadU8_18___load = *(uint8_t*)llvm_cbe___LoadU8_18___addr;
  llvm_cbe_r5 = (((uint64_t)(uint8_t)llvm_cbe___LoadU8_18___load));
  llvm_cbe___Move_19___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_19___load_op1;
  llvm_cbe___Move_20___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_20___load_op1;
  llvm_cbe___Call_21___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___Call_21___arg0 = llvm_cbe_r49;
  llvm_cbe___Call_21___arg1 = llvm_cbe_r50;
  llvm_cbe___Call_21___call = ((l_fptr_15*)(void*)llvm_cbe___Call_21___callee)(llvm_cbe___Call_21___arg0, llvm_cbe___Call_21___arg1);
  llvm_cbe_r3 = llvm_cbe___Call_21___call;
  llvm_cbe___Move_22___load_op1 = llvm_cbe_r1;
  llvm_cbe_r3 = llvm_cbe___Move_22___load_op1;
  llvm_cbe_r4 = 0;
  llvm_cbe___IEqual_24___load_lhs = llvm_cbe_r3;
  llvm_cbe___IEqual_24___load_rhs = llvm_cbe_r4;
  *((bool*)&llvm_cbe_r3) = (llvm_cbe___IEqual_24___load_lhs == llvm_cbe___IEqual_24___load_rhs);
  llvm_cbe___BranchIfNot_25___cond = *((bool*)&llvm_cbe_r3);
  if ((llvm_cbe___BranchIfNot_25___cond != 0)) {
    goto llvm_cbe_bb1;
  } else {
    goto llvm_cbe_bb2;
  }

llvm_cbe_bb1:
  *((void**)&llvm_cbe_r3) = ((void*)&spawn_NC_object);
  llvm_cbe___Move_27___load_op1 = llvm_cbe_r2;
  llvm_cbe_r4 = llvm_cbe___Move_27___load_op1;
  llvm_cbe___AssertPointer_28___ptr = *((void**)&llvm_cbe_r4);
  llvm_cbe___IAddImm_29___load_lhs = *((void**)&llvm_cbe_r4);
  *((void**)&llvm_cbe_r4) = (((&((uint8_t*)llvm_cbe___IAddImm_29___load_lhs)[((int8_t)40)])));
  llvm_cbe___LoadU64_30___addr = *((void**)&llvm_cbe_r4);
  llvm_cbe___LoadU64_30___load = *(uint64_t*)llvm_cbe___LoadU64_30___addr;
  llvm_cbe_r4 = llvm_cbe___LoadU64_30___load;
  llvm_cbe___LoadStaticU64Imm_31___st_load = *(uint64_t*)(((&(&internal_NC_put_NC_on_NC_gas_NC_mask_symbol_table)->array[((int64_t)6)])));
  llvm_cbe_r5 = llvm_cbe___LoadStaticU64Imm_31___st_load;
  llvm_cbe_r6 = 0;
  llvm_cbe_r7 = 0;
  llvm_cbe_r8 = 1;
  llvm_cbe_r9 = 0;
  llvm_cbe_r10 = 0;
  llvm_cbe_r11 = 0;
  llvm_cbe_r12 = 0;
  llvm_cbe___Move_39___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_39___load_op1;
  llvm_cbe___Move_40___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_40___load_op1;
  llvm_cbe___Move_41___load_op1 = llvm_cbe_r6;
  llvm_cbe_r51 = llvm_cbe___Move_41___load_op1;
  llvm_cbe___Move_42___load_op1 = llvm_cbe_r7;
  llvm_cbe_r52 = llvm_cbe___Move_42___load_op1;
  llvm_cbe___Move_43___load_op1 = llvm_cbe_r8;
  llvm_cbe_r53 = llvm_cbe___Move_43___load_op1;
  llvm_cbe___Move_44___load_op1 = llvm_cbe_r9;
  llvm_cbe_r54 = llvm_cbe___Move_44___load_op1;
  llvm_cbe___Move_45___load_op1 = llvm_cbe_r10;
  llvm_cbe_r55 = llvm_cbe___Move_45___load_op1;
  llvm_cbe___Move_46___load_op1 = llvm_cbe_r11;
  llvm_cbe_r56 = llvm_cbe___Move_46___load_op1;
  llvm_cbe___Move_47___load_op1 = llvm_cbe_r12;
  llvm_cbe_r57 = llvm_cbe___Move_47___load_op1;
  llvm_cbe___CallFf_48___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_48___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_48___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_48___arg2 = llvm_cbe_r51;
  llvm_cbe___CallFf_48___arg3 = llvm_cbe_r52;
  llvm_cbe___CallFf_48___arg4 = llvm_cbe_r53;
  llvm_cbe___CallFf_48___arg5 = llvm_cbe_r54;
  llvm_cbe___CallFf_48___arg6 = llvm_cbe_r55;
  llvm_cbe___CallFf_48___arg7 = llvm_cbe_r56;
  llvm_cbe___CallFf_48___arg8 = llvm_cbe_r57;
  llvm_cbe___CallFf_48___call = ((l_fptr_16*)(void*)llvm_cbe___CallFf_48___callee)(llvm_cbe___CallFf_48___arg0, llvm_cbe___CallFf_48___arg1, llvm_cbe___CallFf_48___arg2, llvm_cbe___CallFf_48___arg3, llvm_cbe___CallFf_48___arg4, llvm_cbe___CallFf_48___arg5, llvm_cbe___CallFf_48___arg6, llvm_cbe___CallFf_48___arg7, llvm_cbe___CallFf_48___arg8);
  llvm_cbe_r3 = llvm_cbe___CallFf_48___call;
  llvm_cbe___Move_49___load_op1 = llvm_cbe_r3;
  llvm_cbe_r1 = llvm_cbe___Move_49___load_op1;
  llvm_cbe___Move_50___load_op1 = llvm_cbe_r1;
  llvm_cbe_r3 = llvm_cbe___Move_50___load_op1;
  goto llvm_cbe_bb3;

llvm_cbe_bb2:
  llvm_cbe_r3 = 0;
  goto llvm_cbe_bb3;

llvm_cbe_bb3:
  *((void**)&llvm_cbe_r3) = ((void*)&set_NC_high_NC_contrast_NC_mode_NC_type);
  llvm_cbe___Move_54___load_op1 = llvm_cbe_r1;
  llvm_cbe_r4 = llvm_cbe___Move_54___load_op1;
  llvm_cbe_r5 = 6;
  llvm_cbe___Move_56___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_56___load_op1;
  llvm_cbe___Move_57___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_57___load_op1;
  llvm_cbe___CallFf_58___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_58___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_58___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_58___call = ((l_fptr_17*)(void*)llvm_cbe___CallFf_58___callee)(llvm_cbe___CallFf_58___arg0, llvm_cbe___CallFf_58___arg1);
  llvm_cbe_r3 = llvm_cbe___CallFf_58___call;
  *((void**)&llvm_cbe_r3) = ((void*)&is_NC_player_PD_);
  llvm_cbe___Move_60___load_op1 = llvm_cbe_r0;
  llvm_cbe_r4 = llvm_cbe___Move_60___load_op1;
  llvm_cbe___Move_61___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_61___load_op1;
  llvm_cbe___CallFf_62___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_62___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_62___call = ((l_fptr_18*)(void*)llvm_cbe___CallFf_62___callee)(llvm_cbe___CallFf_62___arg0);
  llvm_cbe_r3 = llvm_cbe___CallFf_62___call;
  llvm_cbe___BranchIfNot_63___cond = llvm_cbe_r3;
  if ((llvm_cbe___BranchIfNot_63___cond != UINT64_C(0))) {
    goto llvm_cbe_bb4;
  } else {
    goto llvm_cbe_bb5;
  }

llvm_cbe_bb4:
  *((void**)&llvm_cbe_r3) = ((void*)&set_NC_instance_NC_flag_NC_hero);
  llvm_cbe___Move_65___load_op1 = llvm_cbe_r1;
  llvm_cbe_r4 = llvm_cbe___Move_65___load_op1;
  llvm_cbe_r5 = 1;
  llvm_cbe___Move_67___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_67___load_op1;
  llvm_cbe___Move_68___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_68___load_op1;
  llvm_cbe___CallFf_69___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_69___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_69___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_69___call = ((l_fptr_19*)(void*)llvm_cbe___CallFf_69___callee)(llvm_cbe___CallFf_69___arg0, llvm_cbe___CallFf_69___arg1);
  llvm_cbe_r3 = llvm_cbe___CallFf_69___call;
  *((void**)&llvm_cbe_r3) = ((void*)&set_NC_show_NC_in_NC_flashlight_NC_flag);
  llvm_cbe___Move_71___load_op1 = llvm_cbe_r1;
  llvm_cbe_r4 = llvm_cbe___Move_71___load_op1;
  llvm_cbe_r5 = 0;
  llvm_cbe___Move_73___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_73___load_op1;
  llvm_cbe___Move_74___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_74___load_op1;
  llvm_cbe___CallFf_75___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_75___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_75___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_75___call = ((l_fptr_20*)(void*)llvm_cbe___CallFf_75___callee)(llvm_cbe___CallFf_75___arg0, llvm_cbe___CallFf_75___arg1);
  llvm_cbe_r3 = llvm_cbe___CallFf_75___call;
  goto llvm_cbe_bb6;

llvm_cbe_bb5:
  *((void**)&llvm_cbe_r3) = ((void*)&set_NC_shadow_NC_casting_NC_from_NC_npc_NC_flashlight);
  llvm_cbe___Move_78___load_op1 = llvm_cbe_r1;
  llvm_cbe_r4 = llvm_cbe___Move_78___load_op1;
  llvm_cbe_r5 = 0;
  llvm_cbe___Move_80___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_80___load_op1;
  llvm_cbe___Move_81___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_81___load_op1;
  llvm_cbe___CallFf_82___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_82___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_82___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_82___call = ((l_fptr_21*)(void*)llvm_cbe___CallFf_82___callee)(llvm_cbe___CallFf_82___arg0, llvm_cbe___CallFf_82___arg1);
  llvm_cbe_r3 = llvm_cbe___CallFf_82___call;
  goto llvm_cbe_bb6;

llvm_cbe_bb6:
  *((void**)&llvm_cbe_r3) = ((void*)&set_NC_gas_NC_mask);
  llvm_cbe___Move_84___load_op1 = llvm_cbe_r0;
  llvm_cbe_r4 = llvm_cbe___Move_84___load_op1;
  llvm_cbe___Move_85___load_op1 = llvm_cbe_r1;
  llvm_cbe_r5 = llvm_cbe___Move_85___load_op1;
  llvm_cbe___Move_86___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_86___load_op1;
  llvm_cbe___Move_87___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_87___load_op1;
  llvm_cbe___CallFf_88___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_88___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_88___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_88___call = ((l_fptr_22*)(void*)llvm_cbe___CallFf_88___callee)(llvm_cbe___CallFf_88___arg0, llvm_cbe___CallFf_88___arg1);
  llvm_cbe_r3 = llvm_cbe___CallFf_88___call;
  *((void**)&llvm_cbe_r3) = ((void*)&set_NC_visible);
  llvm_cbe___Move_90___load_op1 = llvm_cbe_r1;
  llvm_cbe_r4 = llvm_cbe___Move_90___load_op1;
  llvm_cbe_r5 = 0;
  llvm_cbe___Move_92___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_92___load_op1;
  llvm_cbe___Move_93___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_93___load_op1;
  llvm_cbe___CallFf_94___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_94___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_94___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_94___call = ((l_fptr_23*)(void*)llvm_cbe___CallFf_94___callee)(llvm_cbe___CallFf_94___arg0, llvm_cbe___CallFf_94___arg1);
  llvm_cbe_r3 = llvm_cbe___CallFf_94___call;
  *((void**)&llvm_cbe_r3) = ((void*)&is_NC_player_PD_);
  llvm_cbe___Move_96___load_op1 = llvm_cbe_r0;
  llvm_cbe_r4 = llvm_cbe___Move_96___load_op1;
  llvm_cbe___Move_97___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_97___load_op1;
  llvm_cbe___CallFf_98___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_98___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_98___call = ((l_fptr_24*)(void*)llvm_cbe___CallFf_98___callee)(llvm_cbe___CallFf_98___arg0);
  llvm_cbe_r3 = llvm_cbe___CallFf_98___call;
  llvm_cbe___OpLogNot_99___load_op1 = llvm_cbe_r3;
  llvm_cbe_r3 = (llvm_cbe___OpLogNot_99___load_op1 ^ -1);
  llvm_cbe___BranchIfNot_100___cond = *((bool*)&llvm_cbe_r3);
  if ((llvm_cbe___BranchIfNot_100___cond != 0)) {
    goto llvm_cbe_bb7;
  } else {
    goto llvm_cbe_bb8;
  }

llvm_cbe_bb7:
  *((void**)&llvm_cbe_r3) = ((void*)&wait_NC_one_NC_frame);
  llvm_cbe___CallFf_102___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_102___call = ((l_fptr_25*)(void*)llvm_cbe___CallFf_102___callee)();
  llvm_cbe_r3 = llvm_cbe___CallFf_102___call;
  goto llvm_cbe_bb9;

llvm_cbe_bb8:
  llvm_cbe_r3 = 0;
  goto llvm_cbe_bb9;

llvm_cbe_bb9:
  *((void**)&llvm_cbe_r3) = ((void*)&attach);
  llvm_cbe___Move_106___load_op1 = llvm_cbe_r1;
  llvm_cbe_r4 = llvm_cbe___Move_106___load_op1;
  llvm_cbe___Move_107___load_op1 = llvm_cbe_r0;
  llvm_cbe_r5 = llvm_cbe___Move_107___load_op1;
  llvm_cbe___LoadStaticU64Imm_108___st_load = *(uint64_t*)(((&(&internal_NC_put_NC_on_NC_gas_NC_mask_symbol_table)->array[((int64_t)15)])));
  llvm_cbe_r6 = llvm_cbe___LoadStaticU64Imm_108___st_load;
  llvm_cbe___LoadStaticFloatImm_109___st_load = *(float*)(((&(&internal_NC_put_NC_on_NC_gas_NC_mask_symbol_table)->array[((int64_t)16)])));
  *((float*)&llvm_cbe_r7) = llvm_cbe___LoadStaticFloatImm_109___st_load;
  llvm_cbe___Move_110___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_110___load_op1;
  llvm_cbe___Move_111___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_111___load_op1;
  llvm_cbe___Move_112___load_op1 = llvm_cbe_r6;
  llvm_cbe_r51 = llvm_cbe___Move_112___load_op1;
  llvm_cbe___Move_113___load_op1 = *((float*)&llvm_cbe_r7);
  *((float*)&llvm_cbe_r52) = llvm_cbe___Move_113___load_op1;
  llvm_cbe___CallFf_114___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_114___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_114___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_114___arg2 = llvm_cbe_r51;
  llvm_cbe___CallFf_114___arg3 = *((float*)&llvm_cbe_r52);
  llvm_cbe___CallFf_114___call = ((l_fptr_26*)(void*)llvm_cbe___CallFf_114___callee)(llvm_cbe___CallFf_114___arg0, llvm_cbe___CallFf_114___arg1, llvm_cbe___CallFf_114___arg2, llvm_cbe___CallFf_114___arg3);
  llvm_cbe_r3 = llvm_cbe___CallFf_114___call;
  *((void**)&llvm_cbe_r3) = ((void*)&send_NC_event);
  llvm_cbe___LoadStaticU64Imm_116___st_load = *(uint64_t*)(((&(&internal_NC_put_NC_on_NC_gas_NC_mask_symbol_table)->array[((int64_t)18)])));
  llvm_cbe_r4 = llvm_cbe___LoadStaticU64Imm_116___st_load;
  llvm_cbe___Move_117___load_op1 = llvm_cbe_r0;
  llvm_cbe_r5 = llvm_cbe___Move_117___load_op1;
  *((void**)&llvm_cbe_r6) = ((void*)&new_NC_boxed_NC_value);
  llvm_cbe_r7 = 6;
  llvm_cbe___LoadStaticFloatImm_120___st_load = *(float*)(((&(&internal_NC_put_NC_on_NC_gas_NC_mask_symbol_table)->array[((int64_t)20)])));
  *((float*)&llvm_cbe_r8) = llvm_cbe___LoadStaticFloatImm_120___st_load;
  llvm_cbe___Move_121___load_op1 = llvm_cbe_r7;
  llvm_cbe_r49 = llvm_cbe___Move_121___load_op1;
  llvm_cbe___Move_122___load_op1 = *((float*)&llvm_cbe_r8);
  *((float*)&llvm_cbe_r50) = llvm_cbe___Move_122___load_op1;
  llvm_cbe___CallFf_123___callee = *((void**)&llvm_cbe_r6);
  llvm_cbe___CallFf_123___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_123___arg1 = *((float*)&llvm_cbe_r50);
  llvm_cbe___CallFf_123___call = ((l_fptr_27*)(void*)llvm_cbe___CallFf_123___callee)(llvm_cbe___CallFf_123___arg0, llvm_cbe___CallFf_123___arg1);
  llvm_cbe_r6 = llvm_cbe___CallFf_123___call;
  llvm_cbe___Move_124___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_124___load_op1;
  llvm_cbe___Move_125___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_125___load_op1;
  llvm_cbe___Move_126___load_op1 = llvm_cbe_r6;
  llvm_cbe_r51 = llvm_cbe___Move_126___load_op1;
  llvm_cbe___CallFf_127___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_127___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_127___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_127___arg2 = llvm_cbe_r51;
  llvm_cbe___CallFf_127___call = ((l_fptr_28*)(void*)llvm_cbe___CallFf_127___callee)(llvm_cbe___CallFf_127___arg0, llvm_cbe___CallFf_127___arg1, llvm_cbe___CallFf_127___arg2);
  llvm_cbe_r3 = llvm_cbe___CallFf_127___call;
  *((void**)&llvm_cbe_r3) = ((void*)&animate_);
  llvm_cbe___Move_129___load_op1 = llvm_cbe_r1;
  llvm_cbe_r4 = llvm_cbe___Move_129___load_op1;
  llvm_cbe___Move_130___load_op1 = llvm_cbe_r2;
  llvm_cbe_r5 = llvm_cbe___Move_130___load_op1;
  llvm_cbe___AssertPointer_131___ptr = *((void**)&llvm_cbe_r5);
  llvm_cbe___IAddImm_132___load_lhs = *((void**)&llvm_cbe_r5);
  *((void**)&llvm_cbe_r5) = (((&((uint8_t*)llvm_cbe___IAddImm_132___load_lhs)[((int8_t)16)])));
  llvm_cbe___LoadU64_133___addr = *((void**)&llvm_cbe_r5);
  llvm_cbe___LoadU64_133___load = *(uint64_t*)llvm_cbe___LoadU64_133___addr;
  llvm_cbe_r5 = llvm_cbe___LoadU64_133___load;
  *((void**)&llvm_cbe_r6) = ((void*)&_DC__FC_alloc_NC_array);
  llvm_cbe___LoadStaticU64Imm_135___st_load = *(uint64_t*)(((&(&internal_NC_put_NC_on_NC_gas_NC_mask_symbol_table)->array[((int64_t)23)])));
  llvm_cbe_r7 = llvm_cbe___LoadStaticU64Imm_135___st_load;
  llvm_cbe_r8 = 16;
  llvm_cbe_r9 = 0;
  llvm_cbe_r10 = 1;
  llvm_cbe___Move_139___load_op1 = llvm_cbe_r7;
  llvm_cbe_r49 = llvm_cbe___Move_139___load_op1;
  llvm_cbe___Move_140___load_op1 = llvm_cbe_r8;
  llvm_cbe_r50 = llvm_cbe___Move_140___load_op1;
  llvm_cbe___Move_141___load_op1 = llvm_cbe_r9;
  llvm_cbe_r51 = llvm_cbe___Move_141___load_op1;
  llvm_cbe___Move_142___load_op1 = llvm_cbe_r10;
  llvm_cbe_r52 = llvm_cbe___Move_142___load_op1;
  llvm_cbe___CallFf_143___callee = *((void**)&llvm_cbe_r6);
  llvm_cbe___CallFf_143___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_143___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_143___arg2 = llvm_cbe_r51;
  llvm_cbe___CallFf_143___arg3 = llvm_cbe_r52;
  llvm_cbe___CallFf_143___call = ((l_fptr_29*)(void*)llvm_cbe___CallFf_143___callee)(llvm_cbe___CallFf_143___arg0, llvm_cbe___CallFf_143___arg1, llvm_cbe___CallFf_143___arg2, llvm_cbe___CallFf_143___arg3);
  llvm_cbe_r6 = llvm_cbe___CallFf_143___call;
  llvm_cbe___Move_144___load_op1 = llvm_cbe_r6;
  llvm_cbe_r7 = llvm_cbe___Move_144___load_op1;
  llvm_cbe___Move_145___load_op1 = llvm_cbe_r7;
  llvm_cbe_r6 = llvm_cbe___Move_145___load_op1;
  llvm_cbe___Move_146___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_146___load_op1;
  llvm_cbe___Move_147___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_147___load_op1;
  llvm_cbe___Move_148___load_op1 = llvm_cbe_r6;
  llvm_cbe_r51 = llvm_cbe___Move_148___load_op1;
  llvm_cbe___CallFf_149___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_149___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_149___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_149___arg2 = llvm_cbe_r51;
  llvm_cbe___CallFf_149___call = ((l_fptr_30*)(void*)llvm_cbe___CallFf_149___callee)(llvm_cbe___CallFf_149___arg0, llvm_cbe___CallFf_149___arg1, llvm_cbe___CallFf_149___arg2);
  llvm_cbe_r3 = llvm_cbe___CallFf_149___call;
  goto llvm_cbe_bb10;

  do {     /* Syntactic loop 'bb10' to make GCC happy */
llvm_cbe_bb10:
  *((void**)&llvm_cbe_r3) = ((void*)&gesture_);
  llvm_cbe___Move_151___load_op1 = llvm_cbe_r0;
  llvm_cbe_r4 = llvm_cbe___Move_151___load_op1;
  llvm_cbe___Move_152___load_op1 = llvm_cbe_r2;
  llvm_cbe_r5 = llvm_cbe___Move_152___load_op1;
  llvm_cbe___AssertPointer_153___ptr = *((void**)&llvm_cbe_r5);
  llvm_cbe___LoadU64_154___addr = *((void**)&llvm_cbe_r5);
  llvm_cbe___LoadU64_154___load = *(uint64_t*)llvm_cbe___LoadU64_154___addr;
  llvm_cbe_r5 = llvm_cbe___LoadU64_154___load;
  *((void**)&llvm_cbe_r6) = ((void*)&alloc_NC_gesture_NC_play_NC_params);
  llvm_cbe___CallFf_156___callee = *((void**)&llvm_cbe_r6);
  llvm_cbe___CallFf_156___call = ((l_fptr_31*)(void*)llvm_cbe___CallFf_156___callee)();
  llvm_cbe_r6 = llvm_cbe___CallFf_156___call;
  llvm_cbe___Move_157___load_op1 = llvm_cbe_r6;
  llvm_cbe_r7 = llvm_cbe___Move_157___load_op1;
  llvm_cbe___IAddImm_158___load_lhs = llvm_cbe_r7;
  llvm_cbe_r7 = (llvm_add_u64(llvm_cbe___IAddImm_158___load_lhs, 33));
  llvm_cbe_r8 = 1;
  llvm_cbe___StoreU8_160___value = llvm_cbe_r8;
  llvm_cbe___StoreU8_160___addr = *((void**)&llvm_cbe_r7);
  *(uint8_t*)llvm_cbe___StoreU8_160___addr = (((uint8_t)llvm_cbe___StoreU8_160___value));
  llvm_cbe_r7 = llvm_cbe___StoreU8_160___value;
  llvm_cbe___Move_161___load_op1 = llvm_cbe_r6;
  llvm_cbe_r7 = llvm_cbe___Move_161___load_op1;
  llvm_cbe___IAddImm_162___load_lhs = llvm_cbe_r7;
  llvm_cbe_r7 = (llvm_add_u64(llvm_cbe___IAddImm_162___load_lhs, 36));
  llvm_cbe_r8 = 19;
  llvm_cbe___StoreI32_164___value = llvm_cbe_r8;
  llvm_cbe___StoreI32_164___addr = *((void**)&llvm_cbe_r7);
  *(uint32_t*)llvm_cbe___StoreI32_164___addr = (((uint32_t)llvm_cbe___StoreI32_164___value));
  llvm_cbe_r7 = llvm_cbe___StoreI32_164___value;
  llvm_cbe___Move_165___load_op1 = llvm_cbe_r6;
  llvm_cbe_r7 = llvm_cbe___Move_165___load_op1;
  llvm_cbe___IAddImm_166___load_lhs = llvm_cbe_r7;
  llvm_cbe_r7 = (llvm_add_u64(llvm_cbe___IAddImm_166___load_lhs, 32));
  llvm_cbe_r8 = 1;
  llvm_cbe___StoreU8_168___value = llvm_cbe_r8;
  llvm_cbe___StoreU8_168___addr = *((void**)&llvm_cbe_r7);
  *(uint8_t*)llvm_cbe___StoreU8_168___addr = (((uint8_t)llvm_cbe___StoreU8_168___value));
  llvm_cbe_r7 = llvm_cbe___StoreU8_168___value;
  llvm_cbe___Move_169___load_op1 = llvm_cbe_r6;
  llvm_cbe_r7 = llvm_cbe___Move_169___load_op1;
  llvm_cbe___IAddImm_170___load_lhs = llvm_cbe_r7;
  llvm_cbe_r7 = (llvm_add_u64(llvm_cbe___IAddImm_170___load_lhs, 34));
  llvm_cbe_r8 = 1;
  llvm_cbe___StoreU8_172___value = llvm_cbe_r8;
  llvm_cbe___StoreU8_172___addr = *((void**)&llvm_cbe_r7);
  *(uint8_t*)llvm_cbe___StoreU8_172___addr = (((uint8_t)llvm_cbe___StoreU8_172___value));
  llvm_cbe_r7 = llvm_cbe___StoreU8_172___value;
  llvm_cbe___Move_173___load_op1 = llvm_cbe_r6;
  llvm_cbe_r7 = llvm_cbe___Move_173___load_op1;
  llvm_cbe___Move_174___load_op1 = llvm_cbe_r7;
  llvm_cbe_r6 = llvm_cbe___Move_174___load_op1;
  llvm_cbe___Move_175___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_175___load_op1;
  llvm_cbe___Move_176___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_176___load_op1;
  llvm_cbe___Move_177___load_op1 = llvm_cbe_r6;
  llvm_cbe_r51 = llvm_cbe___Move_177___load_op1;
  llvm_cbe___CallFf_178___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_178___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_178___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_178___arg2 = llvm_cbe_r51;
  llvm_cbe___CallFf_178___call = ((l_fptr_32*)(void*)llvm_cbe___CallFf_178___callee)(llvm_cbe___CallFf_178___arg0, llvm_cbe___CallFf_178___arg1, llvm_cbe___CallFf_178___arg2);
  llvm_cbe_r3 = llvm_cbe___CallFf_178___call;
  llvm_cbe_r4 = 0;
  llvm_cbe___IEqual_180___load_lhs = llvm_cbe_r3;
  llvm_cbe___IEqual_180___load_rhs = llvm_cbe_r4;
  *((bool*)&llvm_cbe_r3) = (llvm_cbe___IEqual_180___load_lhs == llvm_cbe___IEqual_180___load_rhs);
  llvm_cbe___OpLogNot_181___load_op1 = *((bool*)&llvm_cbe_r3);
  *((bool*)&llvm_cbe_r3) = (llvm_cbe___OpLogNot_181___load_op1 ^ 1);
  llvm_cbe___BranchIfNot_182___cond = *((bool*)&llvm_cbe_r3);
  if ((llvm_cbe___BranchIfNot_182___cond != 0)) {
    goto llvm_cbe_bb11;
  } else {
    goto llvm_cbe_bb12;
  }

llvm_cbe_bb11:
  *((void**)&llvm_cbe_r3) = ((void*)&wait_NC_one_NC_frame);
  llvm_cbe___CallFf_184___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_184___call = ((l_fptr_33*)(void*)llvm_cbe___CallFf_184___callee)();
  llvm_cbe_r3 = llvm_cbe___CallFf_184___call;
  goto llvm_cbe_bb10;

  } while (1); /* end of syntactic loop 'bb10' */
llvm_cbe_bb12:
  *((void**)&llvm_cbe_r3) = ((void*)&animate_);
  llvm_cbe___Move_187___load_op1 = llvm_cbe_r1;
  llvm_cbe_r4 = llvm_cbe___Move_187___load_op1;
  llvm_cbe___Move_188___load_op1 = llvm_cbe_r2;
  llvm_cbe_r5 = llvm_cbe___Move_188___load_op1;
  llvm_cbe___AssertPointer_189___ptr = *((void**)&llvm_cbe_r5);
  llvm_cbe___IAddImm_190___load_lhs = *((void**)&llvm_cbe_r5);
  *((void**)&llvm_cbe_r5) = (((&((uint8_t*)llvm_cbe___IAddImm_190___load_lhs)[((int8_t)24)])));
  llvm_cbe___LoadU64_191___addr = *((void**)&llvm_cbe_r5);
  llvm_cbe___LoadU64_191___load = *(uint64_t*)llvm_cbe___LoadU64_191___addr;
  llvm_cbe_r5 = llvm_cbe___LoadU64_191___load;
  *((void**)&llvm_cbe_r6) = ((void*)&_DC__FC_alloc_NC_array);
  llvm_cbe___LoadStaticU64Imm_193___st_load = *(uint64_t*)(((&(&internal_NC_put_NC_on_NC_gas_NC_mask_symbol_table)->array[((int64_t)23)])));
  llvm_cbe_r7 = llvm_cbe___LoadStaticU64Imm_193___st_load;
  llvm_cbe_r8 = 16;
  llvm_cbe_r9 = 0;
  llvm_cbe_r10 = 1;
  llvm_cbe___Move_197___load_op1 = llvm_cbe_r7;
  llvm_cbe_r49 = llvm_cbe___Move_197___load_op1;
  llvm_cbe___Move_198___load_op1 = llvm_cbe_r8;
  llvm_cbe_r50 = llvm_cbe___Move_198___load_op1;
  llvm_cbe___Move_199___load_op1 = llvm_cbe_r9;
  llvm_cbe_r51 = llvm_cbe___Move_199___load_op1;
  llvm_cbe___Move_200___load_op1 = llvm_cbe_r10;
  llvm_cbe_r52 = llvm_cbe___Move_200___load_op1;
  llvm_cbe___CallFf_201___callee = *((void**)&llvm_cbe_r6);
  llvm_cbe___CallFf_201___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_201___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_201___arg2 = llvm_cbe_r51;
  llvm_cbe___CallFf_201___arg3 = llvm_cbe_r52;
  llvm_cbe___CallFf_201___call = ((l_fptr_34*)(void*)llvm_cbe___CallFf_201___callee)(llvm_cbe___CallFf_201___arg0, llvm_cbe___CallFf_201___arg1, llvm_cbe___CallFf_201___arg2, llvm_cbe___CallFf_201___arg3);
  llvm_cbe_r6 = llvm_cbe___CallFf_201___call;
  llvm_cbe___Move_202___load_op1 = llvm_cbe_r6;
  llvm_cbe_r7 = llvm_cbe___Move_202___load_op1;
  llvm_cbe___Move_203___load_op1 = llvm_cbe_r7;
  llvm_cbe_r6 = llvm_cbe___Move_203___load_op1;
  llvm_cbe___Move_204___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_204___load_op1;
  llvm_cbe___Move_205___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_205___load_op1;
  llvm_cbe___Move_206___load_op1 = llvm_cbe_r6;
  llvm_cbe_r51 = llvm_cbe___Move_206___load_op1;
  llvm_cbe___CallFf_207___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_207___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_207___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_207___arg2 = llvm_cbe_r51;
  llvm_cbe___CallFf_207___call = ((l_fptr_35*)(void*)llvm_cbe___CallFf_207___callee)(llvm_cbe___CallFf_207___arg0, llvm_cbe___CallFf_207___arg1, llvm_cbe___CallFf_207___arg2);
  llvm_cbe_r3 = llvm_cbe___CallFf_207___call;
  llvm_cbe___Move_208___load_op1 = llvm_cbe_r3;
  llvm_cbe_r2 = llvm_cbe___Move_208___load_op1;
  llvm_cbe___Return_209___retval = llvm_cbe_r2;
  return llvm_cbe___Return_209___retval;
}


uint64_t internal_NC_put_NC_on_NC_gas_NC_mask_NC_immediately_OC_2(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1) {
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r0 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r1 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r2 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r3 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r4 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r5 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r6 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r7 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r8 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r9 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r10 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r11 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r12 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r13 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r14 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r15 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r16 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r17 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r18 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r19 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r20 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r21 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r22 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r23 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r24 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r25 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r26 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r27 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r28 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r29 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r30 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r31 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r32 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r33 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r34 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r35 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r36 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r37 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r38 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r39 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r40 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r41 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r42 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r43 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r44 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r45 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r46 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r47 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r48 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r49 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r50 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r51 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r52 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r53 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r54 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r55 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r56 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r57 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r58 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r59 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r60 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r61 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r62 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r63 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r64 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r65 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r66 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r67 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r68 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r69 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r70 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r71 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r72 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r73 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r74 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r75 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r76 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r77 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r78 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r79 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r80 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r81 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r82 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r83 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r84 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r85 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r86 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r87 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r88 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r89 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r90 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r91 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r92 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r93 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r94 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r95 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r96 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r97 __POSTFIXALIGN__(8);    /* Address-exposed local */
  uint64_t llvm_cbe___Move_0___load_op1;
  uint64_t llvm_cbe___Move_1___load_op1;
  uint64_t llvm_cbe___Move_4___load_op1;
  uint64_t llvm_cbe___Move_5___load_op1;
  void* llvm_cbe___CallFf_6___callee;
  uint64_t llvm_cbe___CallFf_6___arg0;
  uint64_t llvm_cbe___CallFf_6___call;
  uint64_t llvm_cbe___LoadStaticU64Imm_7___st_load;
  uint64_t llvm_cbe___Move_8___load_op1;
  uint64_t llvm_cbe___Move_9___load_op1;
  void* llvm_cbe___CallFf_10___callee;
  uint64_t llvm_cbe___CallFf_10___arg0;
  uint64_t llvm_cbe___CallFf_10___arg1;
  uint64_t llvm_cbe___CallFf_10___call;
  uint64_t llvm_cbe___Move_11___load_op1;
  uint64_t llvm_cbe___IEqual_13___load_lhs;
  uint64_t llvm_cbe___IEqual_13___load_rhs;
  bool llvm_cbe___BranchIfNot_14___cond;
  uint64_t llvm_cbe___Move_16___load_op1;
  void* llvm_cbe___AssertPointer_17___ptr;
  void* llvm_cbe___IAddImm_18___load_lhs;
  void* llvm_cbe___LoadU64_19___addr;
  uint64_t llvm_cbe___LoadU64_19___load;
  uint64_t llvm_cbe___LoadStaticU64Imm_20___st_load;
  uint64_t llvm_cbe___Move_28___load_op1;
  uint64_t llvm_cbe___Move_29___load_op1;
  uint64_t llvm_cbe___Move_30___load_op1;
  uint64_t llvm_cbe___Move_31___load_op1;
  uint64_t llvm_cbe___Move_32___load_op1;
  uint64_t llvm_cbe___Move_33___load_op1;
  uint64_t llvm_cbe___Move_34___load_op1;
  uint64_t llvm_cbe___Move_35___load_op1;
  uint64_t llvm_cbe___Move_36___load_op1;
  void* llvm_cbe___CallFf_37___callee;
  uint64_t llvm_cbe___CallFf_37___arg0;
  uint64_t llvm_cbe___CallFf_37___arg1;
  uint64_t llvm_cbe___CallFf_37___arg2;
  uint64_t llvm_cbe___CallFf_37___arg3;
  uint64_t llvm_cbe___CallFf_37___arg4;
  uint64_t llvm_cbe___CallFf_37___arg5;
  uint64_t llvm_cbe___CallFf_37___arg6;
  uint64_t llvm_cbe___CallFf_37___arg7;
  uint64_t llvm_cbe___CallFf_37___arg8;
  uint64_t llvm_cbe___CallFf_37___call;
  uint64_t llvm_cbe___Move_38___load_op1;
  uint64_t llvm_cbe___Move_39___load_op1;
  uint64_t llvm_cbe___Move_43___load_op1;
  uint64_t llvm_cbe___Move_45___load_op1;
  uint64_t llvm_cbe___Move_46___load_op1;
  void* llvm_cbe___CallFf_47___callee;
  uint64_t llvm_cbe___CallFf_47___arg0;
  uint64_t llvm_cbe___CallFf_47___arg1;
  uint64_t llvm_cbe___CallFf_47___call;
  uint64_t llvm_cbe___Move_49___load_op1;
  uint64_t llvm_cbe___Move_50___load_op1;
  uint64_t llvm_cbe___Move_51___load_op1;
  uint64_t llvm_cbe___Move_52___load_op1;
  void* llvm_cbe___CallFf_53___callee;
  uint64_t llvm_cbe___CallFf_53___arg0;
  uint64_t llvm_cbe___CallFf_53___arg1;
  uint64_t llvm_cbe___CallFf_53___call;
  uint64_t llvm_cbe___Move_55___load_op1;
  uint64_t llvm_cbe___Move_56___load_op1;
  void* llvm_cbe___CallFf_57___callee;
  uint64_t llvm_cbe___CallFf_57___arg0;
  uint64_t llvm_cbe___CallFf_57___call;
  uint64_t llvm_cbe___BranchIfNot_58___cond;
  uint64_t llvm_cbe___LoadStaticU64Imm_60___st_load;
  uint64_t llvm_cbe___Move_61___load_op1;
  float llvm_cbe___LoadStaticFloatImm_64___st_load;
  uint64_t llvm_cbe___Move_65___load_op1;
  float llvm_cbe___Move_66___load_op1;
  void* llvm_cbe___CallFf_67___callee;
  uint64_t llvm_cbe___CallFf_67___arg0;
  float llvm_cbe___CallFf_67___arg1;
  uint64_t llvm_cbe___CallFf_67___call;
  uint64_t llvm_cbe___Move_68___load_op1;
  uint64_t llvm_cbe___Move_69___load_op1;
  uint64_t llvm_cbe___Move_70___load_op1;
  void* llvm_cbe___CallFf_71___callee;
  uint64_t llvm_cbe___CallFf_71___arg0;
  uint64_t llvm_cbe___CallFf_71___arg1;
  uint64_t llvm_cbe___CallFf_71___arg2;
  uint64_t llvm_cbe___CallFf_71___call;
  uint64_t llvm_cbe___Move_73___load_op1;
  uint64_t llvm_cbe___Move_75___load_op1;
  uint64_t llvm_cbe___Move_76___load_op1;
  void* llvm_cbe___CallFf_77___callee;
  uint64_t llvm_cbe___CallFf_77___arg0;
  uint64_t llvm_cbe___CallFf_77___arg1;
  uint64_t llvm_cbe___CallFf_77___call;
  uint64_t llvm_cbe___LoadStaticU64Imm_81___st_load;
  uint64_t llvm_cbe___Move_82___load_op1;
  uint64_t llvm_cbe___Move_83___load_op1;
  uint64_t llvm_cbe___Move_84___load_op1;
  void* llvm_cbe___CallFf_85___callee;
  uint64_t llvm_cbe___CallFf_85___arg0;
  uint64_t llvm_cbe___CallFf_85___arg1;
  uint64_t llvm_cbe___CallFf_85___call;
  uint64_t llvm_cbe___Move_87___load_op1;
  uint64_t llvm_cbe___Move_88___load_op1;
  void* llvm_cbe___CallFf_89___callee;
  uint64_t llvm_cbe___CallFf_89___arg0;
  uint64_t llvm_cbe___CallFf_89___call;
  uint64_t llvm_cbe___BranchIfNot_90___cond;
  uint64_t llvm_cbe___Move_92___load_op1;
  uint64_t llvm_cbe___Move_94___load_op1;
  uint64_t llvm_cbe___Move_95___load_op1;
  void* llvm_cbe___CallFf_96___callee;
  uint64_t llvm_cbe___CallFf_96___arg0;
  uint64_t llvm_cbe___CallFf_96___arg1;
  uint64_t llvm_cbe___CallFf_96___call;
  uint64_t llvm_cbe___Move_99___load_op1;
  uint64_t llvm_cbe___Move_101___load_op1;
  uint64_t llvm_cbe___Move_102___load_op1;
  void* llvm_cbe___CallFf_103___callee;
  uint64_t llvm_cbe___CallFf_103___arg0;
  uint64_t llvm_cbe___CallFf_103___arg1;
  uint64_t llvm_cbe___CallFf_103___call;
  uint64_t llvm_cbe___Move_105___load_op1;
  uint64_t llvm_cbe___Move_106___load_op1;
  void* llvm_cbe___AssertPointer_107___ptr;
  void* llvm_cbe___IAddImm_108___load_lhs;
  void* llvm_cbe___LoadU64_109___addr;
  uint64_t llvm_cbe___LoadU64_109___load;
  uint64_t llvm_cbe___LoadStaticU64Imm_111___st_load;
  uint64_t llvm_cbe___Move_115___load_op1;
  uint64_t llvm_cbe___Move_116___load_op1;
  uint64_t llvm_cbe___Move_117___load_op1;
  uint64_t llvm_cbe___Move_118___load_op1;
  void* llvm_cbe___CallFf_119___callee;
  uint64_t llvm_cbe___CallFf_119___arg0;
  uint64_t llvm_cbe___CallFf_119___arg1;
  uint64_t llvm_cbe___CallFf_119___arg2;
  uint64_t llvm_cbe___CallFf_119___arg3;
  uint64_t llvm_cbe___CallFf_119___call;
  uint64_t llvm_cbe___Move_120___load_op1;
  void* llvm_cbe___LoadPointer_121___addr;
  void* llvm_cbe___LoadPointer_121___load;
  uint64_t llvm_cbe___LoadPointer_121___value;
  void* llvm_cbe___LoadPointer_121___addr1;
  uint64_t llvm_cbe___IMulImm_123___load_lhs;
  uint64_t llvm_cbe___IAdd_124___load_lhs;
  uint64_t llvm_cbe___IAdd_124___load_rhs;
  void* llvm_cbe___LoadPointer_125___addr;
  void* llvm_cbe___LoadPointer_125___load;
  uint64_t llvm_cbe___LoadPointer_125___value;
  void* llvm_cbe___LoadPointer_125___addr2;
  uint64_t llvm_cbe___StoreI32_127___value;
  void* llvm_cbe___StoreI32_127___addr;
  uint64_t llvm_cbe___Move_128___load_op1;
  void* llvm_cbe___LoadPointer_129___addr;
  void* llvm_cbe___LoadPointer_129___load;
  uint64_t llvm_cbe___LoadPointer_129___value;
  void* llvm_cbe___LoadPointer_129___addr3;
  uint64_t llvm_cbe___IMulImm_131___load_lhs;
  uint64_t llvm_cbe___IAdd_132___load_lhs;
  uint64_t llvm_cbe___IAdd_132___load_rhs;
  void* llvm_cbe___LoadPointer_133___addr;
  void* llvm_cbe___LoadPointer_133___load;
  uint64_t llvm_cbe___LoadPointer_133___value;
  void* llvm_cbe___LoadPointer_133___addr4;
  uint64_t llvm_cbe___IAddImm_134___load_lhs;
  uint64_t llvm_cbe___StorePointer_136___value;
  void* llvm_cbe___StorePointer_136___addr;
  void* llvm_cbe___StorePointer_136___st_load;
  uint64_t llvm_cbe___Move_137___load_op1;
  void* llvm_cbe___LoadPointer_138___addr;
  void* llvm_cbe___LoadPointer_138___load;
  uint64_t llvm_cbe___LoadPointer_138___value;
  void* llvm_cbe___LoadPointer_138___addr5;
  uint64_t llvm_cbe___IMulImm_140___load_lhs;
  uint64_t llvm_cbe___IAdd_141___load_lhs;
  uint64_t llvm_cbe___IAdd_141___load_rhs;
  void* llvm_cbe___LoadPointer_142___addr;
  void* llvm_cbe___LoadPointer_142___load;
  uint64_t llvm_cbe___LoadPointer_142___value;
  void* llvm_cbe___LoadPointer_142___addr6;
  uint64_t llvm_cbe___IAddImm_143___load_lhs;
  uint64_t llvm_cbe___StoreU8_145___value;
  void* llvm_cbe___StoreU8_145___addr;
  uint64_t llvm_cbe___Move_146___load_op1;
  uint64_t llvm_cbe___Move_147___load_op1;
  uint64_t llvm_cbe___Move_148___load_op1;
  uint64_t llvm_cbe___Move_149___load_op1;
  uint64_t llvm_cbe___Move_150___load_op1;
  void* llvm_cbe___CallFf_151___callee;
  uint64_t llvm_cbe___CallFf_151___arg0;
  uint64_t llvm_cbe___CallFf_151___arg1;
  uint64_t llvm_cbe___CallFf_151___arg2;
  uint64_t llvm_cbe___CallFf_151___call;
  uint64_t llvm_cbe___Move_152___load_op1;
  uint64_t llvm_cbe___Return_153___retval;

  llvm_cbe_r49 = llvm_cbe_arg_0;
  llvm_cbe_r50 = llvm_cbe_arg_1;
  llvm_cbe___Move_0___load_op1 = llvm_cbe_r49;
  llvm_cbe_r0 = llvm_cbe___Move_0___load_op1;
  llvm_cbe___Move_1___load_op1 = llvm_cbe_r50;
  llvm_cbe_r1 = llvm_cbe___Move_1___load_op1;
  *((void**)&llvm_cbe_r2) = ((void*)&lookup_NC_symbol);
  *((void**)&llvm_cbe_r3) = ((void*)&get_NC_gas_NC_mask_NC_setting_NC_id);
  llvm_cbe___Move_4___load_op1 = llvm_cbe_r0;
  llvm_cbe_r4 = llvm_cbe___Move_4___load_op1;
  llvm_cbe___Move_5___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_5___load_op1;
  llvm_cbe___CallFf_6___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_6___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_6___call = ((l_fptr_36*)(void*)llvm_cbe___CallFf_6___callee)(llvm_cbe___CallFf_6___arg0);
  llvm_cbe_r3 = llvm_cbe___CallFf_6___call;
  llvm_cbe___LoadStaticU64Imm_7___st_load = *(uint64_t*)(((&(&internal_NC_put_NC_on_NC_gas_NC_mask_NC_immediately_symbol_table)->array[((int64_t)2)])));
  llvm_cbe_r4 = llvm_cbe___LoadStaticU64Imm_7___st_load;
  llvm_cbe___Move_8___load_op1 = llvm_cbe_r3;
  llvm_cbe_r49 = llvm_cbe___Move_8___load_op1;
  llvm_cbe___Move_9___load_op1 = llvm_cbe_r4;
  llvm_cbe_r50 = llvm_cbe___Move_9___load_op1;
  llvm_cbe___CallFf_10___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_10___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_10___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_10___call = ((l_fptr_37*)(void*)llvm_cbe___CallFf_10___callee)(llvm_cbe___CallFf_10___arg0, llvm_cbe___CallFf_10___arg1);
  llvm_cbe_r2 = llvm_cbe___CallFf_10___call;
  llvm_cbe___Move_11___load_op1 = llvm_cbe_r1;
  llvm_cbe_r3 = llvm_cbe___Move_11___load_op1;
  llvm_cbe_r4 = 0;
  llvm_cbe___IEqual_13___load_lhs = llvm_cbe_r3;
  llvm_cbe___IEqual_13___load_rhs = llvm_cbe_r4;
  *((bool*)&llvm_cbe_r3) = (llvm_cbe___IEqual_13___load_lhs == llvm_cbe___IEqual_13___load_rhs);
  llvm_cbe___BranchIfNot_14___cond = *((bool*)&llvm_cbe_r3);
  if ((llvm_cbe___BranchIfNot_14___cond != 0)) {
    goto llvm_cbe_bb1;
  } else {
    goto llvm_cbe_bb2;
  }

llvm_cbe_bb1:
  *((void**)&llvm_cbe_r3) = ((void*)&spawn_NC_object);
  llvm_cbe___Move_16___load_op1 = llvm_cbe_r2;
  llvm_cbe_r4 = llvm_cbe___Move_16___load_op1;
  llvm_cbe___AssertPointer_17___ptr = *((void**)&llvm_cbe_r4);
  llvm_cbe___IAddImm_18___load_lhs = *((void**)&llvm_cbe_r4);
  *((void**)&llvm_cbe_r4) = (((&((uint8_t*)llvm_cbe___IAddImm_18___load_lhs)[((int8_t)40)])));
  llvm_cbe___LoadU64_19___addr = *((void**)&llvm_cbe_r4);
  llvm_cbe___LoadU64_19___load = *(uint64_t*)llvm_cbe___LoadU64_19___addr;
  llvm_cbe_r4 = llvm_cbe___LoadU64_19___load;
  llvm_cbe___LoadStaticU64Imm_20___st_load = *(uint64_t*)(((&(&internal_NC_put_NC_on_NC_gas_NC_mask_NC_immediately_symbol_table)->array[((int64_t)4)])));
  llvm_cbe_r5 = llvm_cbe___LoadStaticU64Imm_20___st_load;
  llvm_cbe_r6 = 0;
  llvm_cbe_r7 = 0;
  llvm_cbe_r8 = 1;
  llvm_cbe_r9 = 0;
  llvm_cbe_r10 = 0;
  llvm_cbe_r11 = 0;
  llvm_cbe_r12 = 0;
  llvm_cbe___Move_28___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_28___load_op1;
  llvm_cbe___Move_29___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_29___load_op1;
  llvm_cbe___Move_30___load_op1 = llvm_cbe_r6;
  llvm_cbe_r51 = llvm_cbe___Move_30___load_op1;
  llvm_cbe___Move_31___load_op1 = llvm_cbe_r7;
  llvm_cbe_r52 = llvm_cbe___Move_31___load_op1;
  llvm_cbe___Move_32___load_op1 = llvm_cbe_r8;
  llvm_cbe_r53 = llvm_cbe___Move_32___load_op1;
  llvm_cbe___Move_33___load_op1 = llvm_cbe_r9;
  llvm_cbe_r54 = llvm_cbe___Move_33___load_op1;
  llvm_cbe___Move_34___load_op1 = llvm_cbe_r10;
  llvm_cbe_r55 = llvm_cbe___Move_34___load_op1;
  llvm_cbe___Move_35___load_op1 = llvm_cbe_r11;
  llvm_cbe_r56 = llvm_cbe___Move_35___load_op1;
  llvm_cbe___Move_36___load_op1 = llvm_cbe_r12;
  llvm_cbe_r57 = llvm_cbe___Move_36___load_op1;
  llvm_cbe___CallFf_37___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_37___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_37___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_37___arg2 = llvm_cbe_r51;
  llvm_cbe___CallFf_37___arg3 = llvm_cbe_r52;
  llvm_cbe___CallFf_37___arg4 = llvm_cbe_r53;
  llvm_cbe___CallFf_37___arg5 = llvm_cbe_r54;
  llvm_cbe___CallFf_37___arg6 = llvm_cbe_r55;
  llvm_cbe___CallFf_37___arg7 = llvm_cbe_r56;
  llvm_cbe___CallFf_37___arg8 = llvm_cbe_r57;
  llvm_cbe___CallFf_37___call = ((l_fptr_38*)(void*)llvm_cbe___CallFf_37___callee)(llvm_cbe___CallFf_37___arg0, llvm_cbe___CallFf_37___arg1, llvm_cbe___CallFf_37___arg2, llvm_cbe___CallFf_37___arg3, llvm_cbe___CallFf_37___arg4, llvm_cbe___CallFf_37___arg5, llvm_cbe___CallFf_37___arg6, llvm_cbe___CallFf_37___arg7, llvm_cbe___CallFf_37___arg8);
  llvm_cbe_r3 = llvm_cbe___CallFf_37___call;
  llvm_cbe___Move_38___load_op1 = llvm_cbe_r3;
  llvm_cbe_r1 = llvm_cbe___Move_38___load_op1;
  llvm_cbe___Move_39___load_op1 = llvm_cbe_r1;
  llvm_cbe_r3 = llvm_cbe___Move_39___load_op1;
  goto llvm_cbe_bb3;

llvm_cbe_bb2:
  llvm_cbe_r3 = 0;
  goto llvm_cbe_bb3;

llvm_cbe_bb3:
  *((void**)&llvm_cbe_r3) = ((void*)&set_NC_high_NC_contrast_NC_mode_NC_type);
  llvm_cbe___Move_43___load_op1 = llvm_cbe_r1;
  llvm_cbe_r4 = llvm_cbe___Move_43___load_op1;
  llvm_cbe_r5 = 6;
  llvm_cbe___Move_45___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_45___load_op1;
  llvm_cbe___Move_46___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_46___load_op1;
  llvm_cbe___CallFf_47___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_47___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_47___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_47___call = ((l_fptr_39*)(void*)llvm_cbe___CallFf_47___callee)(llvm_cbe___CallFf_47___arg0, llvm_cbe___CallFf_47___arg1);
  llvm_cbe_r3 = llvm_cbe___CallFf_47___call;
  *((void**)&llvm_cbe_r3) = ((void*)&set_NC_gas_NC_mask);
  llvm_cbe___Move_49___load_op1 = llvm_cbe_r0;
  llvm_cbe_r4 = llvm_cbe___Move_49___load_op1;
  llvm_cbe___Move_50___load_op1 = llvm_cbe_r1;
  llvm_cbe_r5 = llvm_cbe___Move_50___load_op1;
  llvm_cbe___Move_51___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_51___load_op1;
  llvm_cbe___Move_52___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_52___load_op1;
  llvm_cbe___CallFf_53___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_53___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_53___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_53___call = ((l_fptr_40*)(void*)llvm_cbe___CallFf_53___callee)(llvm_cbe___CallFf_53___arg0, llvm_cbe___CallFf_53___arg1);
  llvm_cbe_r3 = llvm_cbe___CallFf_53___call;
  *((void**)&llvm_cbe_r3) = ((void*)&is_NC_player_PD_);
  llvm_cbe___Move_55___load_op1 = llvm_cbe_r0;
  llvm_cbe_r4 = llvm_cbe___Move_55___load_op1;
  llvm_cbe___Move_56___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_56___load_op1;
  llvm_cbe___CallFf_57___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_57___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_57___call = ((l_fptr_41*)(void*)llvm_cbe___CallFf_57___callee)(llvm_cbe___CallFf_57___arg0);
  llvm_cbe_r3 = llvm_cbe___CallFf_57___call;
  llvm_cbe___BranchIfNot_58___cond = llvm_cbe_r3;
  if ((llvm_cbe___BranchIfNot_58___cond != UINT64_C(0))) {
    goto llvm_cbe_bb4;
  } else {
    goto llvm_cbe_bb5;
  }

llvm_cbe_bb4:
  *((void**)&llvm_cbe_r3) = ((void*)&send_NC_event);
  llvm_cbe___LoadStaticU64Imm_60___st_load = *(uint64_t*)(((&(&internal_NC_put_NC_on_NC_gas_NC_mask_NC_immediately_symbol_table)->array[((int64_t)9)])));
  llvm_cbe_r4 = llvm_cbe___LoadStaticU64Imm_60___st_load;
  llvm_cbe___Move_61___load_op1 = llvm_cbe_r0;
  llvm_cbe_r5 = llvm_cbe___Move_61___load_op1;
  *((void**)&llvm_cbe_r6) = ((void*)&new_NC_boxed_NC_value);
  llvm_cbe_r7 = 6;
  llvm_cbe___LoadStaticFloatImm_64___st_load = *(float*)(((&(&internal_NC_put_NC_on_NC_gas_NC_mask_NC_immediately_symbol_table)->array[((int64_t)11)])));
  *((float*)&llvm_cbe_r8) = llvm_cbe___LoadStaticFloatImm_64___st_load;
  llvm_cbe___Move_65___load_op1 = llvm_cbe_r7;
  llvm_cbe_r49 = llvm_cbe___Move_65___load_op1;
  llvm_cbe___Move_66___load_op1 = *((float*)&llvm_cbe_r8);
  *((float*)&llvm_cbe_r50) = llvm_cbe___Move_66___load_op1;
  llvm_cbe___CallFf_67___callee = *((void**)&llvm_cbe_r6);
  llvm_cbe___CallFf_67___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_67___arg1 = *((float*)&llvm_cbe_r50);
  llvm_cbe___CallFf_67___call = ((l_fptr_42*)(void*)llvm_cbe___CallFf_67___callee)(llvm_cbe___CallFf_67___arg0, llvm_cbe___CallFf_67___arg1);
  llvm_cbe_r6 = llvm_cbe___CallFf_67___call;
  llvm_cbe___Move_68___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_68___load_op1;
  llvm_cbe___Move_69___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_69___load_op1;
  llvm_cbe___Move_70___load_op1 = llvm_cbe_r6;
  llvm_cbe_r51 = llvm_cbe___Move_70___load_op1;
  llvm_cbe___CallFf_71___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_71___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_71___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_71___arg2 = llvm_cbe_r51;
  llvm_cbe___CallFf_71___call = ((l_fptr_43*)(void*)llvm_cbe___CallFf_71___callee)(llvm_cbe___CallFf_71___arg0, llvm_cbe___CallFf_71___arg1, llvm_cbe___CallFf_71___arg2);
  llvm_cbe_r3 = llvm_cbe___CallFf_71___call;
  *((void**)&llvm_cbe_r3) = ((void*)&set_NC_instance_NC_flag_NC_hero);
  llvm_cbe___Move_73___load_op1 = llvm_cbe_r1;
  llvm_cbe_r4 = llvm_cbe___Move_73___load_op1;
  llvm_cbe_r5 = 1;
  llvm_cbe___Move_75___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_75___load_op1;
  llvm_cbe___Move_76___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_76___load_op1;
  llvm_cbe___CallFf_77___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_77___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_77___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_77___call = ((l_fptr_44*)(void*)llvm_cbe___CallFf_77___callee)(llvm_cbe___CallFf_77___arg0, llvm_cbe___CallFf_77___arg1);
  llvm_cbe_r3 = llvm_cbe___CallFf_77___call;
  goto llvm_cbe_bb6;

llvm_cbe_bb5:
  llvm_cbe_r3 = 0;
  goto llvm_cbe_bb6;

llvm_cbe_bb6:
  *((void**)&llvm_cbe_r3) = ((void*)&send_NC_event);
  llvm_cbe___LoadStaticU64Imm_81___st_load = *(uint64_t*)(((&(&internal_NC_put_NC_on_NC_gas_NC_mask_NC_immediately_symbol_table)->array[((int64_t)13)])));
  llvm_cbe_r4 = llvm_cbe___LoadStaticU64Imm_81___st_load;
  llvm_cbe___Move_82___load_op1 = llvm_cbe_r0;
  llvm_cbe_r5 = llvm_cbe___Move_82___load_op1;
  llvm_cbe___Move_83___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_83___load_op1;
  llvm_cbe___Move_84___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_84___load_op1;
  llvm_cbe___CallFf_85___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_85___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_85___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_85___call = ((l_fptr_45*)(void*)llvm_cbe___CallFf_85___callee)(llvm_cbe___CallFf_85___arg0, llvm_cbe___CallFf_85___arg1);
  llvm_cbe_r3 = llvm_cbe___CallFf_85___call;
  *((void**)&llvm_cbe_r3) = ((void*)&is_NC_player_PD_);
  llvm_cbe___Move_87___load_op1 = llvm_cbe_r0;
  llvm_cbe_r4 = llvm_cbe___Move_87___load_op1;
  llvm_cbe___Move_88___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_88___load_op1;
  llvm_cbe___CallFf_89___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_89___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_89___call = ((l_fptr_46*)(void*)llvm_cbe___CallFf_89___callee)(llvm_cbe___CallFf_89___arg0);
  llvm_cbe_r3 = llvm_cbe___CallFf_89___call;
  llvm_cbe___BranchIfNot_90___cond = llvm_cbe_r3;
  if ((llvm_cbe___BranchIfNot_90___cond != UINT64_C(0))) {
    goto llvm_cbe_bb7;
  } else {
    goto llvm_cbe_bb8;
  }

llvm_cbe_bb7:
  *((void**)&llvm_cbe_r3) = ((void*)&set_NC_show_NC_in_NC_flashlight_NC_flag);
  llvm_cbe___Move_92___load_op1 = llvm_cbe_r1;
  llvm_cbe_r4 = llvm_cbe___Move_92___load_op1;
  llvm_cbe_r5 = 0;
  llvm_cbe___Move_94___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_94___load_op1;
  llvm_cbe___Move_95___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_95___load_op1;
  llvm_cbe___CallFf_96___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_96___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_96___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_96___call = ((l_fptr_47*)(void*)llvm_cbe___CallFf_96___callee)(llvm_cbe___CallFf_96___arg0, llvm_cbe___CallFf_96___arg1);
  llvm_cbe_r3 = llvm_cbe___CallFf_96___call;
  goto llvm_cbe_bb9;

llvm_cbe_bb8:
  *((void**)&llvm_cbe_r3) = ((void*)&set_NC_shadow_NC_casting_NC_from_NC_npc_NC_flashlight);
  llvm_cbe___Move_99___load_op1 = llvm_cbe_r1;
  llvm_cbe_r4 = llvm_cbe___Move_99___load_op1;
  llvm_cbe_r5 = 0;
  llvm_cbe___Move_101___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_101___load_op1;
  llvm_cbe___Move_102___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_102___load_op1;
  llvm_cbe___CallFf_103___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_103___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_103___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_103___call = ((l_fptr_48*)(void*)llvm_cbe___CallFf_103___callee)(llvm_cbe___CallFf_103___arg0, llvm_cbe___CallFf_103___arg1);
  llvm_cbe_r3 = llvm_cbe___CallFf_103___call;
  goto llvm_cbe_bb9;

llvm_cbe_bb9:
  *((void**)&llvm_cbe_r3) = ((void*)&animate_);
  llvm_cbe___Move_105___load_op1 = llvm_cbe_r1;
  llvm_cbe_r4 = llvm_cbe___Move_105___load_op1;
  llvm_cbe___Move_106___load_op1 = llvm_cbe_r2;
  llvm_cbe_r5 = llvm_cbe___Move_106___load_op1;
  llvm_cbe___AssertPointer_107___ptr = *((void**)&llvm_cbe_r5);
  llvm_cbe___IAddImm_108___load_lhs = *((void**)&llvm_cbe_r5);
  *((void**)&llvm_cbe_r5) = (((&((uint8_t*)llvm_cbe___IAddImm_108___load_lhs)[((int8_t)24)])));
  llvm_cbe___LoadU64_109___addr = *((void**)&llvm_cbe_r5);
  llvm_cbe___LoadU64_109___load = *(uint64_t*)llvm_cbe___LoadU64_109___addr;
  llvm_cbe_r5 = llvm_cbe___LoadU64_109___load;
  *((void**)&llvm_cbe_r6) = ((void*)&_DC__FC_alloc_NC_array);
  llvm_cbe___LoadStaticU64Imm_111___st_load = *(uint64_t*)(((&(&internal_NC_put_NC_on_NC_gas_NC_mask_NC_immediately_symbol_table)->array[((int64_t)18)])));
  llvm_cbe_r7 = llvm_cbe___LoadStaticU64Imm_111___st_load;
  llvm_cbe_r8 = 16;
  llvm_cbe_r9 = 1;
  llvm_cbe_r10 = 1;
  llvm_cbe___Move_115___load_op1 = llvm_cbe_r7;
  llvm_cbe_r49 = llvm_cbe___Move_115___load_op1;
  llvm_cbe___Move_116___load_op1 = llvm_cbe_r8;
  llvm_cbe_r50 = llvm_cbe___Move_116___load_op1;
  llvm_cbe___Move_117___load_op1 = llvm_cbe_r9;
  llvm_cbe_r51 = llvm_cbe___Move_117___load_op1;
  llvm_cbe___Move_118___load_op1 = llvm_cbe_r10;
  llvm_cbe_r52 = llvm_cbe___Move_118___load_op1;
  llvm_cbe___CallFf_119___callee = *((void**)&llvm_cbe_r6);
  llvm_cbe___CallFf_119___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_119___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_119___arg2 = llvm_cbe_r51;
  llvm_cbe___CallFf_119___arg3 = llvm_cbe_r52;
  llvm_cbe___CallFf_119___call = ((l_fptr_49*)(void*)llvm_cbe___CallFf_119___callee)(llvm_cbe___CallFf_119___arg0, llvm_cbe___CallFf_119___arg1, llvm_cbe___CallFf_119___arg2, llvm_cbe___CallFf_119___arg3);
  llvm_cbe_r6 = llvm_cbe___CallFf_119___call;
  llvm_cbe___Move_120___load_op1 = llvm_cbe_r6;
  llvm_cbe_r7 = llvm_cbe___Move_120___load_op1;
  llvm_cbe___LoadPointer_121___addr = *((void**)&llvm_cbe_r7);
  llvm_cbe___LoadPointer_121___load = *(void**)llvm_cbe___LoadPointer_121___addr;
  *((void**)&llvm_cbe_r7) = llvm_cbe___LoadPointer_121___load;
  llvm_cbe___LoadPointer_121___value = llvm_cbe_r0;
  llvm_cbe___LoadPointer_121___addr1 = *((void**)&llvm_cbe_r7);
  *(uint8_t*)llvm_cbe___LoadPointer_121___addr1 = (((uint8_t)llvm_cbe___LoadPointer_121___value));
  llvm_cbe_r7 = llvm_cbe___LoadPointer_121___value;
  llvm_cbe_r8 = 0;
  llvm_cbe___IMulImm_123___load_lhs = llvm_cbe_r8;
  llvm_cbe_r8 = (llvm_mul_u64(llvm_cbe___IMulImm_123___load_lhs, 8));
  llvm_cbe___IAdd_124___load_lhs = llvm_cbe_r7;
  llvm_cbe___IAdd_124___load_rhs = llvm_cbe_r8;
  llvm_cbe_r7 = (llvm_add_u64(llvm_cbe___IAdd_124___load_lhs, llvm_cbe___IAdd_124___load_rhs));
  llvm_cbe___LoadPointer_125___addr = *((void**)&llvm_cbe_r7);
  llvm_cbe___LoadPointer_125___load = *(void**)llvm_cbe___LoadPointer_125___addr;
  *((void**)&llvm_cbe_r7) = llvm_cbe___LoadPointer_125___load;
  llvm_cbe___LoadPointer_125___value = llvm_cbe_r0;
  llvm_cbe___LoadPointer_125___addr2 = *((void**)&llvm_cbe_r7);
  *(uint8_t*)llvm_cbe___LoadPointer_125___addr2 = (((uint8_t)llvm_cbe___LoadPointer_125___value));
  llvm_cbe_r7 = llvm_cbe___LoadPointer_125___value;
  llvm_cbe_r8 = 31;
  llvm_cbe___StoreI32_127___value = llvm_cbe_r8;
  llvm_cbe___StoreI32_127___addr = *((void**)&llvm_cbe_r7);
  *(uint32_t*)llvm_cbe___StoreI32_127___addr = (((uint32_t)llvm_cbe___StoreI32_127___value));
  llvm_cbe_r7 = llvm_cbe___StoreI32_127___value;
  llvm_cbe___Move_128___load_op1 = llvm_cbe_r6;
  llvm_cbe_r7 = llvm_cbe___Move_128___load_op1;
  llvm_cbe___LoadPointer_129___addr = *((void**)&llvm_cbe_r7);
  llvm_cbe___LoadPointer_129___load = *(void**)llvm_cbe___LoadPointer_129___addr;
  *((void**)&llvm_cbe_r7) = llvm_cbe___LoadPointer_129___load;
  llvm_cbe___LoadPointer_129___value = llvm_cbe_r0;
  llvm_cbe___LoadPointer_129___addr3 = *((void**)&llvm_cbe_r7);
  *(uint8_t*)llvm_cbe___LoadPointer_129___addr3 = (((uint8_t)llvm_cbe___LoadPointer_129___value));
  llvm_cbe_r7 = llvm_cbe___LoadPointer_129___value;
  llvm_cbe_r8 = 0;
  llvm_cbe___IMulImm_131___load_lhs = llvm_cbe_r8;
  llvm_cbe_r8 = (llvm_mul_u64(llvm_cbe___IMulImm_131___load_lhs, 8));
  llvm_cbe___IAdd_132___load_lhs = llvm_cbe_r7;
  llvm_cbe___IAdd_132___load_rhs = llvm_cbe_r8;
  llvm_cbe_r7 = (llvm_add_u64(llvm_cbe___IAdd_132___load_lhs, llvm_cbe___IAdd_132___load_rhs));
  llvm_cbe___LoadPointer_133___addr = *((void**)&llvm_cbe_r7);
  llvm_cbe___LoadPointer_133___load = *(void**)llvm_cbe___LoadPointer_133___addr;
  *((void**)&llvm_cbe_r7) = llvm_cbe___LoadPointer_133___load;
  llvm_cbe___LoadPointer_133___value = llvm_cbe_r0;
  llvm_cbe___LoadPointer_133___addr4 = *((void**)&llvm_cbe_r7);
  *(uint8_t*)llvm_cbe___LoadPointer_133___addr4 = (((uint8_t)llvm_cbe___LoadPointer_133___value));
  llvm_cbe_r7 = llvm_cbe___LoadPointer_133___value;
  llvm_cbe___IAddImm_134___load_lhs = llvm_cbe_r7;
  llvm_cbe_r7 = (llvm_add_u64(llvm_cbe___IAddImm_134___load_lhs, 8));
  llvm_cbe_r8 = 0;
  llvm_cbe___StorePointer_136___value = llvm_cbe_r8;
  llvm_cbe___StorePointer_136___addr = *((void**)&llvm_cbe_r7);
  *(uint64_t*)llvm_cbe___StorePointer_136___addr = llvm_cbe___StorePointer_136___value;
  llvm_cbe_r7 = llvm_cbe___StorePointer_136___value;
  llvm_cbe___StorePointer_136___st_load = *(void**)(((&(&internal_NC_put_NC_on_NC_gas_NC_mask_NC_immediately_symbol_table)->array[((int64_t)7)])));
  *((void**)&llvm_cbe_r7) = llvm_cbe___StorePointer_136___st_load;
  llvm_cbe___Move_137___load_op1 = llvm_cbe_r6;
  llvm_cbe_r7 = llvm_cbe___Move_137___load_op1;
  llvm_cbe___LoadPointer_138___addr = *((void**)&llvm_cbe_r7);
  llvm_cbe___LoadPointer_138___load = *(void**)llvm_cbe___LoadPointer_138___addr;
  *((void**)&llvm_cbe_r7) = llvm_cbe___LoadPointer_138___load;
  llvm_cbe___LoadPointer_138___value = llvm_cbe_r0;
  llvm_cbe___LoadPointer_138___addr5 = *((void**)&llvm_cbe_r7);
  *(uint8_t*)llvm_cbe___LoadPointer_138___addr5 = (((uint8_t)llvm_cbe___LoadPointer_138___value));
  llvm_cbe_r7 = llvm_cbe___LoadPointer_138___value;
  llvm_cbe_r8 = 0;
  llvm_cbe___IMulImm_140___load_lhs = llvm_cbe_r8;
  llvm_cbe_r8 = (llvm_mul_u64(llvm_cbe___IMulImm_140___load_lhs, 8));
  llvm_cbe___IAdd_141___load_lhs = llvm_cbe_r7;
  llvm_cbe___IAdd_141___load_rhs = llvm_cbe_r8;
  llvm_cbe_r7 = (llvm_add_u64(llvm_cbe___IAdd_141___load_lhs, llvm_cbe___IAdd_141___load_rhs));
  llvm_cbe___LoadPointer_142___addr = *((void**)&llvm_cbe_r7);
  llvm_cbe___LoadPointer_142___load = *(void**)llvm_cbe___LoadPointer_142___addr;
  *((void**)&llvm_cbe_r7) = llvm_cbe___LoadPointer_142___load;
  llvm_cbe___LoadPointer_142___value = llvm_cbe_r0;
  llvm_cbe___LoadPointer_142___addr6 = *((void**)&llvm_cbe_r7);
  *(uint8_t*)llvm_cbe___LoadPointer_142___addr6 = (((uint8_t)llvm_cbe___LoadPointer_142___value));
  llvm_cbe_r7 = llvm_cbe___LoadPointer_142___value;
  llvm_cbe___IAddImm_143___load_lhs = llvm_cbe_r7;
  llvm_cbe_r7 = (llvm_add_u64(llvm_cbe___IAddImm_143___load_lhs, 8));
  llvm_cbe_r8 = 1;
  llvm_cbe___StoreU8_145___value = llvm_cbe_r8;
  llvm_cbe___StoreU8_145___addr = *((void**)&llvm_cbe_r7);
  *(uint8_t*)llvm_cbe___StoreU8_145___addr = (((uint8_t)llvm_cbe___StoreU8_145___value));
  llvm_cbe_r7 = llvm_cbe___StoreU8_145___value;
  llvm_cbe___Move_146___load_op1 = llvm_cbe_r6;
  llvm_cbe_r7 = llvm_cbe___Move_146___load_op1;
  llvm_cbe___Move_147___load_op1 = llvm_cbe_r7;
  llvm_cbe_r6 = llvm_cbe___Move_147___load_op1;
  llvm_cbe___Move_148___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_148___load_op1;
  llvm_cbe___Move_149___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_149___load_op1;
  llvm_cbe___Move_150___load_op1 = llvm_cbe_r6;
  llvm_cbe_r51 = llvm_cbe___Move_150___load_op1;
  llvm_cbe___CallFf_151___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_151___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_151___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_151___arg2 = llvm_cbe_r51;
  llvm_cbe___CallFf_151___call = ((l_fptr_50*)(void*)llvm_cbe___CallFf_151___callee)(llvm_cbe___CallFf_151___arg0, llvm_cbe___CallFf_151___arg1, llvm_cbe___CallFf_151___arg2);
  llvm_cbe_r3 = llvm_cbe___CallFf_151___call;
  llvm_cbe___Move_152___load_op1 = llvm_cbe_r3;
  llvm_cbe_r2 = llvm_cbe___Move_152___load_op1;
  llvm_cbe___Return_153___retval = llvm_cbe_r2;
  return llvm_cbe___Return_153___retval;
}


uint64_t get_NC_equipped_NC_gas_NC_mask(uint64_t llvm_cbe_arg_0) {
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r0 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r1 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r2 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r3 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r4 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r5 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r6 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r7 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r8 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r9 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r10 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r11 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r12 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r13 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r14 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r15 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r16 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r17 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r18 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r19 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r20 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r21 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r22 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r23 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r24 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r25 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r26 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r27 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r28 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r29 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r30 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r31 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r32 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r33 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r34 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r35 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r36 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r37 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r38 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r39 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r40 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r41 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r42 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r43 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r44 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r45 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r46 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r47 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r48 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r49 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r50 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r51 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r52 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r53 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r54 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r55 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r56 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r57 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r58 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r59 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r60 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r61 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r62 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r63 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r64 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r65 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r66 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r67 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r68 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r69 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r70 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r71 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r72 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r73 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r74 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r75 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r76 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r77 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r78 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r79 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r80 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r81 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r82 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r83 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r84 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r85 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r86 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r87 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r88 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r89 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r90 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r91 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r92 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r93 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r94 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r95 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r96 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r97 __POSTFIXALIGN__(8);    /* Address-exposed local */
  uint64_t llvm_cbe___Move_0___load_op1;
  uint64_t llvm_cbe___Move_2___load_op1;
  uint64_t llvm_cbe___Move_3___load_op1;
  void* llvm_cbe___CallFf_4___callee;
  uint64_t llvm_cbe___CallFf_4___arg0;
  uint64_t llvm_cbe___CallFf_4___call;
  uint64_t llvm_cbe___Return_5___retval;

  llvm_cbe_r49 = llvm_cbe_arg_0;
  llvm_cbe___Move_0___load_op1 = llvm_cbe_r49;
  llvm_cbe_r0 = llvm_cbe___Move_0___load_op1;
  *((void**)&llvm_cbe_r1) = ((void*)&get_NC_gas_NC_mask);
  llvm_cbe___Move_2___load_op1 = llvm_cbe_r0;
  llvm_cbe_r2 = llvm_cbe___Move_2___load_op1;
  llvm_cbe___Move_3___load_op1 = llvm_cbe_r2;
  llvm_cbe_r49 = llvm_cbe___Move_3___load_op1;
  llvm_cbe___CallFf_4___callee = *((void**)&llvm_cbe_r1);
  llvm_cbe___CallFf_4___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_4___call = ((l_fptr_51*)(void*)llvm_cbe___CallFf_4___callee)(llvm_cbe___CallFf_4___arg0);
  llvm_cbe_r1 = llvm_cbe___CallFf_4___call;
  llvm_cbe___Return_5___retval = llvm_cbe_r1;
  return llvm_cbe___Return_5___retval;
}


uint64_t wait_NC_until_NC_in_NC_valid_NC_gas_NC_mask_NC_state_OC_3(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1) {
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r0 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r1 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r2 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r3 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r4 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r5 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r6 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r7 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r8 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r9 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r10 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r11 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r12 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r13 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r14 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r15 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r16 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r17 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r18 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r19 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r20 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r21 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r22 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r23 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r24 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r25 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r26 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r27 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r28 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r29 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r30 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r31 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r32 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r33 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r34 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r35 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r36 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r37 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r38 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r39 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r40 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r41 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r42 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r43 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r44 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r45 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r46 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r47 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r48 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r49 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r50 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r51 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r52 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r53 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r54 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r55 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r56 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r57 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r58 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r59 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r60 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r61 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r62 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r63 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r64 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r65 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r66 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r67 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r68 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r69 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r70 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r71 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r72 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r73 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r74 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r75 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r76 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r77 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r78 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r79 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r80 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r81 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r82 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r83 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r84 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r85 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r86 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r87 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r88 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r89 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r90 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r91 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r92 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r93 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r94 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r95 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r96 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r97 __POSTFIXALIGN__(8);    /* Address-exposed local */
  uint64_t llvm_cbe___Move_0___load_op1;
  uint64_t llvm_cbe___Move_1___load_op1;
  uint64_t llvm_cbe___Move_3___load_op1;
  uint64_t llvm_cbe___Move_4___load_op1;
  void* llvm_cbe___CallFf_5___callee;
  uint64_t llvm_cbe___CallFf_5___arg0;
  uint64_t llvm_cbe___CallFf_5___call;
  uint64_t llvm_cbe___BranchIfNot_6___cond;
  uint64_t llvm_cbe___LoadStaticU64Imm_8___st_load;
  uint64_t llvm_cbe___Move_9___load_op1;
  void* llvm_cbe___CallFf_10___callee;
  uint64_t llvm_cbe___CallFf_10___arg0;
  uint64_t llvm_cbe___CallFf_10___call;
  uint64_t llvm_cbe___OpLogNot_11___load_op1;
  bool llvm_cbe___BranchIfNot_12___cond;
  uint64_t llvm_cbe___LoadStaticU64Imm_14___st_load;
  uint64_t llvm_cbe___Move_15___load_op1;
  void* llvm_cbe___CallFf_16___callee;
  uint64_t llvm_cbe___CallFf_16___arg0;
  uint64_t llvm_cbe___CallFf_16___call;
  uint64_t llvm_cbe___OpLogNot_17___load_op1;
  bool llvm_cbe___OpLogNot_18___load_op1;
  bool llvm_cbe___BranchIfNot_19___cond;
  void* llvm_cbe___CallFf_21___callee;
  uint64_t llvm_cbe___CallFf_21___call;
  uint64_t llvm_cbe___Move_23___load_op1;
  uint64_t llvm_cbe___BranchIfNot_24___cond;
  void* llvm_cbe___CallFf_26___callee;
  uint64_t llvm_cbe___CallFf_26___call;
  void* llvm_cbe___CallFf_28___callee;
  uint64_t llvm_cbe___CallFf_28___call;
  uint64_t llvm_cbe___OpLogNot_29___load_op1;
  bool llvm_cbe___BranchIfNot_30___cond;
  void* llvm_cbe___CallFf_32___callee;
  uint64_t llvm_cbe___CallFf_32___call;
  void* llvm_cbe___CallFf_36___callee;
  uint64_t llvm_cbe___CallFf_36___call;
  void* llvm_cbe___CallFf_39___callee;
  uint64_t llvm_cbe___CallFf_39___call;
  uint64_t llvm_cbe___Move_40___load_op1;
  void* llvm_cbe___Call_41___callee;
  uint64_t llvm_cbe___Call_41___arg0;
  uint64_t llvm_cbe___Call_41___call;
  uint64_t llvm_cbe___OpLogNot_42___load_op1;
  bool llvm_cbe___BranchIfNot_43___cond;
  void* llvm_cbe___CallFf_45___callee;
  uint64_t llvm_cbe___CallFf_45___call;
  uint64_t llvm_cbe___LoadStaticU64Imm_48___st_load;
  uint64_t llvm_cbe___Move_49___load_op1;
  void* llvm_cbe___CallFf_50___callee;
  uint64_t llvm_cbe___CallFf_50___arg0;
  uint64_t llvm_cbe___CallFf_50___call;
  uint64_t llvm_cbe___OpLogNot_51___load_op1;
  bool llvm_cbe___BranchIfNot_52___cond;
  void* llvm_cbe___CallFf_54___callee;
  uint64_t llvm_cbe___CallFf_54___call;
  float llvm_cbe___LoadStaticFloatImm_58___st_load;
  uint64_t llvm_cbe___Move_59___load_op1;
  float llvm_cbe___Move_60___load_op1;
  void* llvm_cbe___CallFf_61___callee;
  uint64_t llvm_cbe___CallFf_61___arg0;
  float llvm_cbe___CallFf_61___arg1;
  uint64_t llvm_cbe___CallFf_61___call;
  float llvm_cbe___LoadStaticFloatImm_63___st_load;
  float llvm_cbe___Move_65___load_op1;
  uint64_t llvm_cbe___Move_66___load_op1;
  void* llvm_cbe___CallFf_67___callee;
  float llvm_cbe___CallFf_67___arg0;
  uint64_t llvm_cbe___CallFf_67___arg1;
  uint64_t llvm_cbe___CallFf_67___call;
  uint64_t llvm_cbe___Move_70___load_op1;
  float llvm_cbe___LoadStaticFloatImm_73___st_load;
  uint64_t llvm_cbe___Move_75___load_op1;
  uint64_t llvm_cbe___Move_76___load_op1;
  uint64_t llvm_cbe___Move_77___load_op1;
  float llvm_cbe___Move_78___load_op1;
  uint64_t llvm_cbe___Move_79___load_op1;
  void* llvm_cbe___CallFf_80___callee;
  uint64_t llvm_cbe___CallFf_80___arg0;
  uint64_t llvm_cbe___CallFf_80___arg1;
  uint64_t llvm_cbe___CallFf_80___arg2;
  float llvm_cbe___CallFf_80___arg3;
  uint64_t llvm_cbe___CallFf_80___arg4;
  uint64_t llvm_cbe___CallFf_80___call;
  uint64_t llvm_cbe___Move_82___load_op1;
  uint64_t llvm_cbe___Move_83___load_op1;
  void* llvm_cbe___CallFf_84___callee;
  uint64_t llvm_cbe___CallFf_84___arg0;
  uint64_t llvm_cbe___CallFf_84___call;
  uint64_t llvm_cbe___BranchIfNot_85___cond;
  void* llvm_cbe___CallFf_87___callee;
  uint64_t llvm_cbe___CallFf_87___call;
  uint64_t llvm_cbe___Move_89___load_op1;
  uint64_t llvm_cbe___BranchIfNot_90___cond;
  uint64_t llvm_cbe___Move_92___load_op1;
  uint64_t llvm_cbe___Move_94___load_op1;
  uint64_t llvm_cbe___Move_95___load_op1;
  void* llvm_cbe___CallFf_96___callee;
  uint64_t llvm_cbe___CallFf_96___arg0;
  uint64_t llvm_cbe___CallFf_96___arg1;
  uint64_t llvm_cbe___CallFf_96___call;
  uint64_t llvm_cbe___Return_99___retval;

  llvm_cbe_r49 = llvm_cbe_arg_0;
  llvm_cbe_r50 = llvm_cbe_arg_1;
  llvm_cbe___Move_0___load_op1 = llvm_cbe_r49;
  llvm_cbe_r0 = llvm_cbe___Move_0___load_op1;
  llvm_cbe___Move_1___load_op1 = llvm_cbe_r50;
  llvm_cbe_r1 = llvm_cbe___Move_1___load_op1;
  *((void**)&llvm_cbe_r2) = ((void*)&is_NC_player_PD_);
  llvm_cbe___Move_3___load_op1 = llvm_cbe_r0;
  llvm_cbe_r3 = llvm_cbe___Move_3___load_op1;
  llvm_cbe___Move_4___load_op1 = llvm_cbe_r3;
  llvm_cbe_r49 = llvm_cbe___Move_4___load_op1;
  llvm_cbe___CallFf_5___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_5___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_5___call = ((l_fptr_52*)(void*)llvm_cbe___CallFf_5___callee)(llvm_cbe___CallFf_5___arg0);
  llvm_cbe_r2 = llvm_cbe___CallFf_5___call;
  llvm_cbe___BranchIfNot_6___cond = llvm_cbe_r2;
  if ((llvm_cbe___BranchIfNot_6___cond != UINT64_C(0))) {
    goto llvm_cbe_bb1;
  } else {
    goto llvm_cbe_bb15;
  }

  do {     /* Syntactic loop 'bb1' to make GCC happy */
llvm_cbe_bb1:
  *((void**)&llvm_cbe_r2) = ((void*)&player_NC_in_NC_state_PD_);
  llvm_cbe___LoadStaticU64Imm_8___st_load = *(uint64_t*)(((&(&wait_NC_until_NC_in_NC_valid_NC_gas_NC_mask_NC_state_symbol_table)->array[((int64_t)2)])));
  llvm_cbe_r3 = llvm_cbe___LoadStaticU64Imm_8___st_load;
  llvm_cbe___Move_9___load_op1 = llvm_cbe_r3;
  llvm_cbe_r49 = llvm_cbe___Move_9___load_op1;
  llvm_cbe___CallFf_10___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_10___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_10___call = ((l_fptr_53*)(void*)llvm_cbe___CallFf_10___callee)(llvm_cbe___CallFf_10___arg0);
  llvm_cbe_r2 = llvm_cbe___CallFf_10___call;
  llvm_cbe___OpLogNot_11___load_op1 = llvm_cbe_r2;
  llvm_cbe_r2 = (llvm_cbe___OpLogNot_11___load_op1 ^ -1);
  llvm_cbe___BranchIfNot_12___cond = *((bool*)&llvm_cbe_r2);
  if ((llvm_cbe___BranchIfNot_12___cond != 0)) {
    goto llvm_cbe_bb2;
  } else {
    goto llvm_cbe_bb3;
  }

llvm_cbe_bb2:
  *((void**)&llvm_cbe_r2) = ((void*)&player_NC_in_NC_state_PD_);
  llvm_cbe___LoadStaticU64Imm_14___st_load = *(uint64_t*)(((&(&wait_NC_until_NC_in_NC_valid_NC_gas_NC_mask_NC_state_symbol_table)->array[((int64_t)3)])));
  llvm_cbe_r3 = llvm_cbe___LoadStaticU64Imm_14___st_load;
  llvm_cbe___Move_15___load_op1 = llvm_cbe_r3;
  llvm_cbe_r49 = llvm_cbe___Move_15___load_op1;
  llvm_cbe___CallFf_16___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_16___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_16___call = ((l_fptr_54*)(void*)llvm_cbe___CallFf_16___callee)(llvm_cbe___CallFf_16___arg0);
  llvm_cbe_r2 = llvm_cbe___CallFf_16___call;
  llvm_cbe___OpLogNot_17___load_op1 = llvm_cbe_r2;
  llvm_cbe_r2 = (llvm_cbe___OpLogNot_17___load_op1 ^ -1);
  llvm_cbe___OpLogNot_18___load_op1 = *((bool*)&llvm_cbe_r2);
  *((bool*)&llvm_cbe_r2) = (llvm_cbe___OpLogNot_18___load_op1 ^ 1);
  llvm_cbe___BranchIfNot_19___cond = *((bool*)&llvm_cbe_r2);
  if ((llvm_cbe___BranchIfNot_19___cond != 0)) {
    goto llvm_cbe_bb3;
  } else {
    goto llvm_cbe_bb4;
  }

llvm_cbe_bb3:
  *((void**)&llvm_cbe_r2) = ((void*)&wait_NC_one_NC_frame);
  llvm_cbe___CallFf_21___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_21___call = ((l_fptr_55*)(void*)llvm_cbe___CallFf_21___callee)();
  llvm_cbe_r2 = llvm_cbe___CallFf_21___call;
  goto llvm_cbe_bb1;

  } while (1); /* end of syntactic loop 'bb1' */
llvm_cbe_bb4:
  llvm_cbe___Move_23___load_op1 = llvm_cbe_r1;
  llvm_cbe_r2 = llvm_cbe___Move_23___load_op1;
  llvm_cbe___BranchIfNot_24___cond = llvm_cbe_r2;
  if ((llvm_cbe___BranchIfNot_24___cond != UINT64_C(0))) {
    goto llvm_cbe_bb5;
  } else {
    goto llvm_cbe_bb9;
  }

llvm_cbe_bb5:
  *((void**)&llvm_cbe_r2) = ((void*)&player_NC_force_NC_crouch);
  llvm_cbe___CallFf_26___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_26___call = ((l_fptr_56*)(void*)llvm_cbe___CallFf_26___callee)();
  llvm_cbe_r2 = llvm_cbe___CallFf_26___call;
  goto llvm_cbe_bb6;

  do {     /* Syntactic loop 'bb6' to make GCC happy */
llvm_cbe_bb6:
  *((void**)&llvm_cbe_r2) = ((void*)&player_NC_is_NC_crouched_PD_);
  llvm_cbe___CallFf_28___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_28___call = ((l_fptr_57*)(void*)llvm_cbe___CallFf_28___callee)();
  llvm_cbe_r2 = llvm_cbe___CallFf_28___call;
  llvm_cbe___OpLogNot_29___load_op1 = llvm_cbe_r2;
  llvm_cbe_r2 = (llvm_cbe___OpLogNot_29___load_op1 ^ -1);
  llvm_cbe___BranchIfNot_30___cond = *((bool*)&llvm_cbe_r2);
  if ((llvm_cbe___BranchIfNot_30___cond != 0)) {
    goto llvm_cbe_bb7;
  } else {
    goto llvm_cbe_bb8;
  }

llvm_cbe_bb7:
  *((void**)&llvm_cbe_r2) = ((void*)&wait_NC_one_NC_frame);
  llvm_cbe___CallFf_32___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_32___call = ((l_fptr_58*)(void*)llvm_cbe___CallFf_32___callee)();
  llvm_cbe_r2 = llvm_cbe___CallFf_32___call;
  goto llvm_cbe_bb6;

  } while (1); /* end of syntactic loop 'bb6' */
llvm_cbe_bb8:
  goto llvm_cbe_bb12;

llvm_cbe_bb9:
  *((void**)&llvm_cbe_r2) = ((void*)&player_NC_force_NC_stand);
  llvm_cbe___CallFf_36___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_36___call = ((l_fptr_59*)(void*)llvm_cbe___CallFf_36___callee)();
  llvm_cbe_r2 = llvm_cbe___CallFf_36___call;
  goto llvm_cbe_bb10;

  do {     /* Syntactic loop 'bb10' to make GCC happy */
llvm_cbe_bb10:
  *((void**)&llvm_cbe_r2) = ((void*)&_BC_);
  *((void**)&llvm_cbe_r3) = ((void*)&player_NC_is_NC_crouched_PD_);
  llvm_cbe___CallFf_39___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_39___call = ((l_fptr_60*)(void*)llvm_cbe___CallFf_39___callee)();
  llvm_cbe_r3 = llvm_cbe___CallFf_39___call;
  llvm_cbe___Move_40___load_op1 = llvm_cbe_r3;
  llvm_cbe_r49 = llvm_cbe___Move_40___load_op1;
  llvm_cbe___Call_41___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___Call_41___arg0 = llvm_cbe_r49;
  llvm_cbe___Call_41___call = ((l_fptr_61*)(void*)llvm_cbe___Call_41___callee)(llvm_cbe___Call_41___arg0);
  llvm_cbe_r2 = llvm_cbe___Call_41___call;
  llvm_cbe___OpLogNot_42___load_op1 = llvm_cbe_r2;
  llvm_cbe_r2 = (llvm_cbe___OpLogNot_42___load_op1 ^ -1);
  llvm_cbe___BranchIfNot_43___cond = *((bool*)&llvm_cbe_r2);
  if ((llvm_cbe___BranchIfNot_43___cond != 0)) {
    goto llvm_cbe_bb11;
  } else {
    goto llvm_cbe_bb12;
  }

llvm_cbe_bb11:
  *((void**)&llvm_cbe_r2) = ((void*)&wait_NC_one_NC_frame);
  llvm_cbe___CallFf_45___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_45___call = ((l_fptr_62*)(void*)llvm_cbe___CallFf_45___callee)();
  llvm_cbe_r2 = llvm_cbe___CallFf_45___call;
  goto llvm_cbe_bb10;

  } while (1); /* end of syntactic loop 'bb10' */
  do {     /* Syntactic loop 'bb12' to make GCC happy */
llvm_cbe_bb12:
  *((void**)&llvm_cbe_r2) = ((void*)&player_NC_in_NC_state_PD_);
  llvm_cbe___LoadStaticU64Imm_48___st_load = *(uint64_t*)(((&(&wait_NC_until_NC_in_NC_valid_NC_gas_NC_mask_NC_state_symbol_table)->array[((int64_t)9)])));
  llvm_cbe_r3 = llvm_cbe___LoadStaticU64Imm_48___st_load;
  llvm_cbe___Move_49___load_op1 = llvm_cbe_r3;
  llvm_cbe_r49 = llvm_cbe___Move_49___load_op1;
  llvm_cbe___CallFf_50___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_50___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_50___call = ((l_fptr_63*)(void*)llvm_cbe___CallFf_50___callee)(llvm_cbe___CallFf_50___arg0);
  llvm_cbe_r2 = llvm_cbe___CallFf_50___call;
  llvm_cbe___OpLogNot_51___load_op1 = llvm_cbe_r2;
  llvm_cbe_r2 = (llvm_cbe___OpLogNot_51___load_op1 ^ -1);
  llvm_cbe___BranchIfNot_52___cond = *((bool*)&llvm_cbe_r2);
  if ((llvm_cbe___BranchIfNot_52___cond != 0)) {
    goto llvm_cbe_bb13;
  } else {
    goto llvm_cbe_bb14;
  }

llvm_cbe_bb13:
  *((void**)&llvm_cbe_r2) = ((void*)&wait_NC_one_NC_frame);
  llvm_cbe___CallFf_54___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_54___call = ((l_fptr_64*)(void*)llvm_cbe___CallFf_54___callee)();
  llvm_cbe_r2 = llvm_cbe___CallFf_54___call;
  goto llvm_cbe_bb12;

  } while (1); /* end of syntactic loop 'bb12' */
llvm_cbe_bb14:
  *((void**)&llvm_cbe_r2) = ((void*)&wait_NC_holster_NC_player_NC_weapon);
  llvm_cbe_r3 = 0;
  llvm_cbe___LoadStaticFloatImm_58___st_load = *(float*)(((&(&wait_NC_until_NC_in_NC_valid_NC_gas_NC_mask_NC_state_symbol_table)->array[((int64_t)11)])));
  *((float*)&llvm_cbe_r4) = llvm_cbe___LoadStaticFloatImm_58___st_load;
  llvm_cbe___Move_59___load_op1 = llvm_cbe_r3;
  llvm_cbe_r49 = llvm_cbe___Move_59___load_op1;
  llvm_cbe___Move_60___load_op1 = *((float*)&llvm_cbe_r4);
  *((float*)&llvm_cbe_r50) = llvm_cbe___Move_60___load_op1;
  llvm_cbe___CallFf_61___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_61___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_61___arg1 = *((float*)&llvm_cbe_r50);
  llvm_cbe___CallFf_61___call = ((l_fptr_65*)(void*)llvm_cbe___CallFf_61___callee)(llvm_cbe___CallFf_61___arg0, llvm_cbe___CallFf_61___arg1);
  llvm_cbe_r2 = llvm_cbe___CallFf_61___call;
  *((void**)&llvm_cbe_r2) = ((void*)&holster_NC_player_NC_weapon_NC_instantly);
  llvm_cbe___LoadStaticFloatImm_63___st_load = *(float*)(((&(&wait_NC_until_NC_in_NC_valid_NC_gas_NC_mask_NC_state_symbol_table)->array[((int64_t)13)])));
  *((float*)&llvm_cbe_r3) = llvm_cbe___LoadStaticFloatImm_63___st_load;
  llvm_cbe_r4 = 0;
  llvm_cbe___Move_65___load_op1 = *((float*)&llvm_cbe_r3);
  *((float*)&llvm_cbe_r49) = llvm_cbe___Move_65___load_op1;
  llvm_cbe___Move_66___load_op1 = llvm_cbe_r4;
  llvm_cbe_r50 = llvm_cbe___Move_66___load_op1;
  llvm_cbe___CallFf_67___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_67___arg0 = *((float*)&llvm_cbe_r49);
  llvm_cbe___CallFf_67___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_67___call = ((l_fptr_66*)(void*)llvm_cbe___CallFf_67___callee)(llvm_cbe___CallFf_67___arg0, llvm_cbe___CallFf_67___arg1);
  llvm_cbe_r2 = llvm_cbe___CallFf_67___call;
  goto llvm_cbe_bb21;

llvm_cbe_bb15:
  *((void**)&llvm_cbe_r2) = ((void*)&wait_NC_npc_NC_holster_NC_weapon);
  llvm_cbe___Move_70___load_op1 = llvm_cbe_r0;
  llvm_cbe_r3 = llvm_cbe___Move_70___load_op1;
  llvm_cbe_r4 = 0;
  llvm_cbe_r5 = 1;
  llvm_cbe___LoadStaticFloatImm_73___st_load = *(float*)(((&(&wait_NC_until_NC_in_NC_valid_NC_gas_NC_mask_NC_state_symbol_table)->array[((int64_t)15)])));
  *((float*)&llvm_cbe_r6) = llvm_cbe___LoadStaticFloatImm_73___st_load;
  llvm_cbe_r7 = 0;
  llvm_cbe___Move_75___load_op1 = llvm_cbe_r3;
  llvm_cbe_r49 = llvm_cbe___Move_75___load_op1;
  llvm_cbe___Move_76___load_op1 = llvm_cbe_r4;
  llvm_cbe_r50 = llvm_cbe___Move_76___load_op1;
  llvm_cbe___Move_77___load_op1 = llvm_cbe_r5;
  llvm_cbe_r51 = llvm_cbe___Move_77___load_op1;
  llvm_cbe___Move_78___load_op1 = *((float*)&llvm_cbe_r6);
  *((float*)&llvm_cbe_r52) = llvm_cbe___Move_78___load_op1;
  llvm_cbe___Move_79___load_op1 = llvm_cbe_r7;
  llvm_cbe_r53 = llvm_cbe___Move_79___load_op1;
  llvm_cbe___CallFf_80___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_80___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_80___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_80___arg2 = llvm_cbe_r51;
  llvm_cbe___CallFf_80___arg3 = *((float*)&llvm_cbe_r52);
  llvm_cbe___CallFf_80___arg4 = llvm_cbe_r53;
  llvm_cbe___CallFf_80___call = ((l_fptr_67*)(void*)llvm_cbe___CallFf_80___callee)(llvm_cbe___CallFf_80___arg0, llvm_cbe___CallFf_80___arg1, llvm_cbe___CallFf_80___arg2, llvm_cbe___CallFf_80___arg3, llvm_cbe___CallFf_80___arg4);
  llvm_cbe_r2 = llvm_cbe___CallFf_80___call;
  goto llvm_cbe_bb16;

  do {     /* Syntactic loop 'bb16' to make GCC happy */
llvm_cbe_bb16:
  *((void**)&llvm_cbe_r2) = ((void*)&npc_NC_has_NC_weapon_NC_in_NC_hand_PD_);
  llvm_cbe___Move_82___load_op1 = llvm_cbe_r0;
  llvm_cbe_r3 = llvm_cbe___Move_82___load_op1;
  llvm_cbe___Move_83___load_op1 = llvm_cbe_r3;
  llvm_cbe_r49 = llvm_cbe___Move_83___load_op1;
  llvm_cbe___CallFf_84___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_84___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_84___call = ((l_fptr_68*)(void*)llvm_cbe___CallFf_84___callee)(llvm_cbe___CallFf_84___arg0);
  llvm_cbe_r2 = llvm_cbe___CallFf_84___call;
  llvm_cbe___BranchIfNot_85___cond = llvm_cbe_r2;
  if ((llvm_cbe___BranchIfNot_85___cond != UINT64_C(0))) {
    goto llvm_cbe_bb17;
  } else {
    goto llvm_cbe_bb18;
  }

llvm_cbe_bb17:
  *((void**)&llvm_cbe_r2) = ((void*)&wait_NC_one_NC_frame);
  llvm_cbe___CallFf_87___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_87___call = ((l_fptr_69*)(void*)llvm_cbe___CallFf_87___callee)();
  llvm_cbe_r2 = llvm_cbe___CallFf_87___call;
  goto llvm_cbe_bb16;

  } while (1); /* end of syntactic loop 'bb16' */
llvm_cbe_bb18:
  llvm_cbe___Move_89___load_op1 = llvm_cbe_r1;
  llvm_cbe_r2 = llvm_cbe___Move_89___load_op1;
  llvm_cbe___BranchIfNot_90___cond = llvm_cbe_r2;
  if ((llvm_cbe___BranchIfNot_90___cond != UINT64_C(0))) {
    goto llvm_cbe_bb19;
  } else {
    goto llvm_cbe_bb20;
  }

llvm_cbe_bb19:
  *((void**)&llvm_cbe_r2) = ((void*)&wait_NC_npc_NC_set_NC_demeanor);
  llvm_cbe___Move_92___load_op1 = llvm_cbe_r0;
  llvm_cbe_r3 = llvm_cbe___Move_92___load_op1;
  llvm_cbe_r4 = 3;
  llvm_cbe___Move_94___load_op1 = llvm_cbe_r3;
  llvm_cbe_r49 = llvm_cbe___Move_94___load_op1;
  llvm_cbe___Move_95___load_op1 = llvm_cbe_r4;
  llvm_cbe_r50 = llvm_cbe___Move_95___load_op1;
  llvm_cbe___CallFf_96___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_96___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_96___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_96___call = ((l_fptr_70*)(void*)llvm_cbe___CallFf_96___callee)(llvm_cbe___CallFf_96___arg0, llvm_cbe___CallFf_96___arg1);
  llvm_cbe_r2 = llvm_cbe___CallFf_96___call;
  goto llvm_cbe_bb21;

llvm_cbe_bb20:
  llvm_cbe_r2 = 0;
  goto llvm_cbe_bb21;

llvm_cbe_bb21:
  llvm_cbe___Return_99___retval = llvm_cbe_r2;
  return llvm_cbe___Return_99___retval;
}


uint64_t npc_NC_gas_NC_mask_NC_narrative_NC_mode_PC_f(uint64_t llvm_cbe_arg_0) {
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r0 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r1 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r2 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r3 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r4 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r5 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r6 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r7 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r8 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r9 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r10 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r11 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r12 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r13 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r14 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r15 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r16 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r17 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r18 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r19 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r20 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r21 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r22 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r23 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r24 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r25 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r26 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r27 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r28 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r29 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r30 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r31 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r32 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r33 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r34 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r35 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r36 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r37 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r38 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r39 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r40 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r41 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r42 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r43 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r44 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r45 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r46 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r47 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r48 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r49 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r50 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r51 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r52 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r53 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r54 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r55 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r56 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r57 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r58 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r59 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r60 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r61 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r62 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r63 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r64 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r65 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r66 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r67 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r68 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r69 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r70 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r71 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r72 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r73 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r74 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r75 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r76 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r77 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r78 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r79 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r80 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r81 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r82 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r83 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r84 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r85 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r86 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r87 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r88 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r89 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r90 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r91 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r92 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r93 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r94 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r95 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r96 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r97 __POSTFIXALIGN__(8);    /* Address-exposed local */
  uint64_t llvm_cbe___Move_0___load_op1;
  uint64_t llvm_cbe___LoadStaticU64Imm_2___st_load;
  uint64_t llvm_cbe___Move_3___load_op1;
  uint64_t llvm_cbe___Move_4___load_op1;
  uint64_t llvm_cbe___Move_5___load_op1;
  void* llvm_cbe___CallFf_6___callee;
  uint64_t llvm_cbe___CallFf_6___arg0;
  uint64_t llvm_cbe___CallFf_6___arg1;
  uint64_t llvm_cbe___CallFf_6___call;
  uint64_t llvm_cbe___Move_8___load_op1;
  uint64_t llvm_cbe___Move_9___load_op1;
  void* llvm_cbe___Call_10___callee;
  uint64_t llvm_cbe___Call_10___arg0;
  uint64_t llvm_cbe___Call_10___call;
  uint64_t llvm_cbe___Move_12___load_op1;
  float llvm_cbe___LoadStaticFloatImm_13___st_load;
  uint64_t llvm_cbe___Move_14___load_op1;
  float llvm_cbe___Move_15___load_op1;
  void* llvm_cbe___CallFf_16___callee;
  uint64_t llvm_cbe___CallFf_16___arg0;
  float llvm_cbe___CallFf_16___arg1;
  uint64_t llvm_cbe___CallFf_16___call;
  uint64_t llvm_cbe___Return_17___retval;

  llvm_cbe_r49 = llvm_cbe_arg_0;
  llvm_cbe___Move_0___load_op1 = llvm_cbe_r49;
  llvm_cbe_r0 = llvm_cbe___Move_0___load_op1;
  *((void**)&llvm_cbe_r1) = ((void*)&send_NC_event);
  llvm_cbe___LoadStaticU64Imm_2___st_load = *(uint64_t*)(((&(&npc_NC_gas_NC_mask_NC_narrative_NC_mode_PC_f_symbol_table)->array[((int64_t)1)])));
  llvm_cbe_r2 = llvm_cbe___LoadStaticU64Imm_2___st_load;
  llvm_cbe___Move_3___load_op1 = llvm_cbe_r0;
  llvm_cbe_r3 = llvm_cbe___Move_3___load_op1;
  llvm_cbe___Move_4___load_op1 = llvm_cbe_r2;
  llvm_cbe_r49 = llvm_cbe___Move_4___load_op1;
  llvm_cbe___Move_5___load_op1 = llvm_cbe_r3;
  llvm_cbe_r50 = llvm_cbe___Move_5___load_op1;
  llvm_cbe___CallFf_6___callee = *((void**)&llvm_cbe_r1);
  llvm_cbe___CallFf_6___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_6___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_6___call = ((l_fptr_71*)(void*)llvm_cbe___CallFf_6___callee)(llvm_cbe___CallFf_6___arg0, llvm_cbe___CallFf_6___arg1);
  llvm_cbe_r1 = llvm_cbe___CallFf_6___call;
  *((void**)&llvm_cbe_r1) = ((void*)&suspend_NC_idle_NC_gestures_PC_f);
  llvm_cbe___Move_8___load_op1 = llvm_cbe_r0;
  llvm_cbe_r2 = llvm_cbe___Move_8___load_op1;
  llvm_cbe___Move_9___load_op1 = llvm_cbe_r2;
  llvm_cbe_r49 = llvm_cbe___Move_9___load_op1;
  llvm_cbe___Call_10___callee = *((void**)&llvm_cbe_r1);
  llvm_cbe___Call_10___arg0 = llvm_cbe_r49;
  llvm_cbe___Call_10___call = ((l_fptr_72*)(void*)llvm_cbe___Call_10___callee)(llvm_cbe___Call_10___arg0);
  llvm_cbe_r1 = llvm_cbe___Call_10___call;
  *((void**)&llvm_cbe_r1) = ((void*)&npc_NC_disable_NC_dialog_NC_look);
  llvm_cbe___Move_12___load_op1 = llvm_cbe_r0;
  llvm_cbe_r2 = llvm_cbe___Move_12___load_op1;
  llvm_cbe___LoadStaticFloatImm_13___st_load = *(float*)(((&(&npc_NC_gas_NC_mask_NC_narrative_NC_mode_PC_f_symbol_table)->array[((int64_t)4)])));
  *((float*)&llvm_cbe_r3) = llvm_cbe___LoadStaticFloatImm_13___st_load;
  llvm_cbe___Move_14___load_op1 = llvm_cbe_r2;
  llvm_cbe_r49 = llvm_cbe___Move_14___load_op1;
  llvm_cbe___Move_15___load_op1 = *((float*)&llvm_cbe_r3);
  *((float*)&llvm_cbe_r50) = llvm_cbe___Move_15___load_op1;
  llvm_cbe___CallFf_16___callee = *((void**)&llvm_cbe_r1);
  llvm_cbe___CallFf_16___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_16___arg1 = *((float*)&llvm_cbe_r50);
  llvm_cbe___CallFf_16___call = ((l_fptr_73*)(void*)llvm_cbe___CallFf_16___callee)(llvm_cbe___CallFf_16___arg0, llvm_cbe___CallFf_16___arg1);
  llvm_cbe_r1 = llvm_cbe___CallFf_16___call;
  llvm_cbe___Return_17___retval = llvm_cbe_r1;
  return llvm_cbe___Return_17___retval;
}


uint64_t internal_NC_put_NC_off_NC_gas_NC_mask_NC_immediately(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1) {
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r0 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r1 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r2 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r3 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r4 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r5 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r6 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r7 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r8 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r9 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r10 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r11 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r12 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r13 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r14 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r15 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r16 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r17 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r18 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r19 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r20 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r21 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r22 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r23 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r24 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r25 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r26 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r27 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r28 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r29 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r30 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r31 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r32 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r33 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r34 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r35 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r36 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r37 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r38 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r39 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r40 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r41 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r42 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r43 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r44 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r45 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r46 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r47 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r48 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r49 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r50 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r51 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r52 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r53 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r54 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r55 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r56 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r57 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r58 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r59 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r60 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r61 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r62 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r63 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r64 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r65 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r66 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r67 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r68 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r69 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r70 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r71 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r72 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r73 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r74 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r75 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r76 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r77 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r78 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r79 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r80 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r81 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r82 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r83 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r84 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r85 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r86 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r87 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r88 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r89 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r90 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r91 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r92 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r93 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r94 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r95 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r96 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r97 __POSTFIXALIGN__(8);    /* Address-exposed local */
  uint64_t llvm_cbe___Move_0___load_op1;
  uint64_t llvm_cbe___Move_1___load_op1;
  uint64_t llvm_cbe___Move_3___load_op1;
  uint64_t llvm_cbe___Move_4___load_op1;
  void* llvm_cbe___CallFf_5___callee;
  uint64_t llvm_cbe___CallFf_5___arg0;
  uint64_t llvm_cbe___CallFf_5___call;
  uint64_t llvm_cbe___LoadStaticU64Imm_7___st_load;
  uint64_t llvm_cbe___Move_8___load_op1;
  uint64_t llvm_cbe___Move_9___load_op1;
  uint64_t llvm_cbe___Move_10___load_op1;
  void* llvm_cbe___CallFf_11___callee;
  uint64_t llvm_cbe___CallFf_11___arg0;
  uint64_t llvm_cbe___CallFf_11___arg1;
  uint64_t llvm_cbe___CallFf_11___call;
  uint64_t llvm_cbe___Move_12___load_op1;
  uint64_t llvm_cbe___BranchIfNot_13___cond;
  uint64_t llvm_cbe___Move_15___load_op1;
  uint64_t llvm_cbe___Move_16___load_op1;
  void* llvm_cbe___CallFf_17___callee;
  uint64_t llvm_cbe___CallFf_17___arg0;
  uint64_t llvm_cbe___CallFf_17___call;
  uint64_t llvm_cbe___Move_20___load_op1;
  uint64_t llvm_cbe___Return_21___retval;

  llvm_cbe_r49 = llvm_cbe_arg_0;
  llvm_cbe_r50 = llvm_cbe_arg_1;
  llvm_cbe___Move_0___load_op1 = llvm_cbe_r49;
  llvm_cbe_r0 = llvm_cbe___Move_0___load_op1;
  llvm_cbe___Move_1___load_op1 = llvm_cbe_r50;
  llvm_cbe_r1 = llvm_cbe___Move_1___load_op1;
  *((void**)&llvm_cbe_r2) = ((void*)&get_NC_gas_NC_mask);
  llvm_cbe___Move_3___load_op1 = llvm_cbe_r0;
  llvm_cbe_r3 = llvm_cbe___Move_3___load_op1;
  llvm_cbe___Move_4___load_op1 = llvm_cbe_r3;
  llvm_cbe_r49 = llvm_cbe___Move_4___load_op1;
  llvm_cbe___CallFf_5___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_5___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_5___call = ((l_fptr_74*)(void*)llvm_cbe___CallFf_5___callee)(llvm_cbe___CallFf_5___arg0);
  llvm_cbe_r2 = llvm_cbe___CallFf_5___call;
  *((void**)&llvm_cbe_r3) = ((void*)&send_NC_event);
  llvm_cbe___LoadStaticU64Imm_7___st_load = *(uint64_t*)(((&(&internal_NC_put_NC_off_NC_gas_NC_mask_NC_immediately_symbol_table)->array[((int64_t)2)])));
  llvm_cbe_r4 = llvm_cbe___LoadStaticU64Imm_7___st_load;
  llvm_cbe___Move_8___load_op1 = llvm_cbe_r0;
  llvm_cbe_r5 = llvm_cbe___Move_8___load_op1;
  llvm_cbe___Move_9___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_9___load_op1;
  llvm_cbe___Move_10___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_10___load_op1;
  llvm_cbe___CallFf_11___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_11___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_11___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_11___call = ((l_fptr_75*)(void*)llvm_cbe___CallFf_11___callee)(llvm_cbe___CallFf_11___arg0, llvm_cbe___CallFf_11___arg1);
  llvm_cbe_r3 = llvm_cbe___CallFf_11___call;
  llvm_cbe___Move_12___load_op1 = llvm_cbe_r1;
  llvm_cbe_r3 = llvm_cbe___Move_12___load_op1;
  llvm_cbe___BranchIfNot_13___cond = llvm_cbe_r3;
  if ((llvm_cbe___BranchIfNot_13___cond != UINT64_C(0))) {
    goto llvm_cbe_bb1;
  } else {
    goto llvm_cbe_bb2;
  }

llvm_cbe_bb1:
  *((void**)&llvm_cbe_r3) = ((void*)&kill_NC_entity);
  llvm_cbe___Move_15___load_op1 = llvm_cbe_r2;
  llvm_cbe_r4 = llvm_cbe___Move_15___load_op1;
  llvm_cbe___Move_16___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_16___load_op1;
  llvm_cbe___CallFf_17___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_17___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_17___call = ((l_fptr_76*)(void*)llvm_cbe___CallFf_17___callee)(llvm_cbe___CallFf_17___arg0);
  llvm_cbe_r3 = llvm_cbe___CallFf_17___call;
  goto llvm_cbe_bb3;

llvm_cbe_bb2:
  llvm_cbe_r3 = 0;
  goto llvm_cbe_bb3;

llvm_cbe_bb3:
  llvm_cbe___Move_20___load_op1 = llvm_cbe_r3;
  llvm_cbe_r2 = llvm_cbe___Move_20___load_op1;
  llvm_cbe___Return_21___retval = llvm_cbe_r2;
  return llvm_cbe___Return_21___retval;
}


uint64_t spawn_NC_gas_NC_mask(uint64_t llvm_cbe_arg_0) {
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r0 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r1 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r2 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r3 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r4 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r5 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r6 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r7 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r8 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r9 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r10 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r11 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r12 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r13 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r14 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r15 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r16 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r17 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r18 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r19 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r20 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r21 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r22 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r23 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r24 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r25 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r26 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r27 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r28 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r29 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r30 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r31 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r32 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r33 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r34 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r35 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r36 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r37 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r38 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r39 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r40 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r41 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r42 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r43 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r44 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r45 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r46 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r47 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r48 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r49 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r50 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r51 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r52 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r53 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r54 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r55 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r56 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r57 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r58 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r59 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r60 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r61 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r62 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r63 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r64 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r65 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r66 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r67 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r68 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r69 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r70 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r71 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r72 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r73 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r74 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r75 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r76 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r77 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r78 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r79 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r80 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r81 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r82 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r83 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r84 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r85 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r86 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r87 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r88 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r89 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r90 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r91 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r92 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r93 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r94 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r95 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r96 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r97 __POSTFIXALIGN__(8);    /* Address-exposed local */
  uint64_t llvm_cbe___Move_0___load_op1;
  uint64_t llvm_cbe___Move_3___load_op1;
  uint64_t llvm_cbe___Move_4___load_op1;
  void* llvm_cbe___CallFf_5___callee;
  uint64_t llvm_cbe___CallFf_5___arg0;
  uint64_t llvm_cbe___CallFf_5___call;
  uint64_t llvm_cbe___LoadStaticU64Imm_6___st_load;
  uint64_t llvm_cbe___Move_7___load_op1;
  uint64_t llvm_cbe___Move_8___load_op1;
  void* llvm_cbe___CallFf_9___callee;
  uint64_t llvm_cbe___CallFf_9___arg0;
  uint64_t llvm_cbe___CallFf_9___arg1;
  uint64_t llvm_cbe___CallFf_9___call;
  uint64_t llvm_cbe___Move_11___load_op1;
  void* llvm_cbe___AssertPointer_12___ptr;
  void* llvm_cbe___IAddImm_13___load_lhs;
  void* llvm_cbe___LoadU64_14___addr;
  uint64_t llvm_cbe___LoadU64_14___load;
  uint64_t llvm_cbe___LoadStaticU64Imm_15___st_load;
  uint64_t llvm_cbe___Move_23___load_op1;
  uint64_t llvm_cbe___Move_24___load_op1;
  uint64_t llvm_cbe___Move_25___load_op1;
  uint64_t llvm_cbe___Move_26___load_op1;
  uint64_t llvm_cbe___Move_27___load_op1;
  uint64_t llvm_cbe___Move_28___load_op1;
  uint64_t llvm_cbe___Move_29___load_op1;
  uint64_t llvm_cbe___Move_30___load_op1;
  uint64_t llvm_cbe___Move_31___load_op1;
  void* llvm_cbe___CallFf_32___callee;
  uint64_t llvm_cbe___CallFf_32___arg0;
  uint64_t llvm_cbe___CallFf_32___arg1;
  uint64_t llvm_cbe___CallFf_32___arg2;
  uint64_t llvm_cbe___CallFf_32___arg3;
  uint64_t llvm_cbe___CallFf_32___arg4;
  uint64_t llvm_cbe___CallFf_32___arg5;
  uint64_t llvm_cbe___CallFf_32___arg6;
  uint64_t llvm_cbe___CallFf_32___arg7;
  uint64_t llvm_cbe___CallFf_32___arg8;
  uint64_t llvm_cbe___CallFf_32___call;
  uint64_t llvm_cbe___Move_34___load_op1;
  uint64_t llvm_cbe___Move_36___load_op1;
  uint64_t llvm_cbe___Move_37___load_op1;
  void* llvm_cbe___CallFf_38___callee;
  uint64_t llvm_cbe___CallFf_38___arg0;
  uint64_t llvm_cbe___CallFf_38___arg1;
  uint64_t llvm_cbe___CallFf_38___call;
  uint64_t llvm_cbe___Move_39___load_op1;
  uint64_t llvm_cbe___Move_40___load_op1;
  uint64_t llvm_cbe___Move_41___load_op1;
  uint64_t llvm_cbe___Move_42___load_op1;
  uint64_t llvm_cbe___Return_43___retval;

  llvm_cbe_r49 = llvm_cbe_arg_0;
  llvm_cbe___Move_0___load_op1 = llvm_cbe_r49;
  llvm_cbe_r0 = llvm_cbe___Move_0___load_op1;
  *((void**)&llvm_cbe_r1) = ((void*)&lookup_NC_symbol);
  *((void**)&llvm_cbe_r2) = ((void*)&get_NC_gas_NC_mask_NC_setting_NC_id);
  llvm_cbe___Move_3___load_op1 = llvm_cbe_r0;
  llvm_cbe_r3 = llvm_cbe___Move_3___load_op1;
  llvm_cbe___Move_4___load_op1 = llvm_cbe_r3;
  llvm_cbe_r49 = llvm_cbe___Move_4___load_op1;
  llvm_cbe___CallFf_5___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_5___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_5___call = ((l_fptr_77*)(void*)llvm_cbe___CallFf_5___callee)(llvm_cbe___CallFf_5___arg0);
  llvm_cbe_r2 = llvm_cbe___CallFf_5___call;
  llvm_cbe___LoadStaticU64Imm_6___st_load = *(uint64_t*)(((&(&spawn_NC_gas_NC_mask_symbol_table)->array[((int64_t)2)])));
  llvm_cbe_r3 = llvm_cbe___LoadStaticU64Imm_6___st_load;
  llvm_cbe___Move_7___load_op1 = llvm_cbe_r2;
  llvm_cbe_r49 = llvm_cbe___Move_7___load_op1;
  llvm_cbe___Move_8___load_op1 = llvm_cbe_r3;
  llvm_cbe_r50 = llvm_cbe___Move_8___load_op1;
  llvm_cbe___CallFf_9___callee = *((void**)&llvm_cbe_r1);
  llvm_cbe___CallFf_9___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_9___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_9___call = ((l_fptr_78*)(void*)llvm_cbe___CallFf_9___callee)(llvm_cbe___CallFf_9___arg0, llvm_cbe___CallFf_9___arg1);
  llvm_cbe_r1 = llvm_cbe___CallFf_9___call;
  *((void**)&llvm_cbe_r2) = ((void*)&spawn_NC_object);
  llvm_cbe___Move_11___load_op1 = llvm_cbe_r1;
  llvm_cbe_r3 = llvm_cbe___Move_11___load_op1;
  llvm_cbe___AssertPointer_12___ptr = *((void**)&llvm_cbe_r3);
  llvm_cbe___IAddImm_13___load_lhs = *((void**)&llvm_cbe_r3);
  *((void**)&llvm_cbe_r3) = (((&((uint8_t*)llvm_cbe___IAddImm_13___load_lhs)[((int8_t)40)])));
  llvm_cbe___LoadU64_14___addr = *((void**)&llvm_cbe_r3);
  llvm_cbe___LoadU64_14___load = *(uint64_t*)llvm_cbe___LoadU64_14___addr;
  llvm_cbe_r3 = llvm_cbe___LoadU64_14___load;
  llvm_cbe___LoadStaticU64Imm_15___st_load = *(uint64_t*)(((&(&spawn_NC_gas_NC_mask_symbol_table)->array[((int64_t)4)])));
  llvm_cbe_r4 = llvm_cbe___LoadStaticU64Imm_15___st_load;
  llvm_cbe_r5 = 0;
  llvm_cbe_r6 = 0;
  llvm_cbe_r7 = 1;
  llvm_cbe_r8 = 0;
  llvm_cbe_r9 = 0;
  llvm_cbe_r10 = 0;
  llvm_cbe_r11 = 0;
  llvm_cbe___Move_23___load_op1 = llvm_cbe_r3;
  llvm_cbe_r49 = llvm_cbe___Move_23___load_op1;
  llvm_cbe___Move_24___load_op1 = llvm_cbe_r4;
  llvm_cbe_r50 = llvm_cbe___Move_24___load_op1;
  llvm_cbe___Move_25___load_op1 = llvm_cbe_r5;
  llvm_cbe_r51 = llvm_cbe___Move_25___load_op1;
  llvm_cbe___Move_26___load_op1 = llvm_cbe_r6;
  llvm_cbe_r52 = llvm_cbe___Move_26___load_op1;
  llvm_cbe___Move_27___load_op1 = llvm_cbe_r7;
  llvm_cbe_r53 = llvm_cbe___Move_27___load_op1;
  llvm_cbe___Move_28___load_op1 = llvm_cbe_r8;
  llvm_cbe_r54 = llvm_cbe___Move_28___load_op1;
  llvm_cbe___Move_29___load_op1 = llvm_cbe_r9;
  llvm_cbe_r55 = llvm_cbe___Move_29___load_op1;
  llvm_cbe___Move_30___load_op1 = llvm_cbe_r10;
  llvm_cbe_r56 = llvm_cbe___Move_30___load_op1;
  llvm_cbe___Move_31___load_op1 = llvm_cbe_r11;
  llvm_cbe_r57 = llvm_cbe___Move_31___load_op1;
  llvm_cbe___CallFf_32___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_32___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_32___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_32___arg2 = llvm_cbe_r51;
  llvm_cbe___CallFf_32___arg3 = llvm_cbe_r52;
  llvm_cbe___CallFf_32___arg4 = llvm_cbe_r53;
  llvm_cbe___CallFf_32___arg5 = llvm_cbe_r54;
  llvm_cbe___CallFf_32___arg6 = llvm_cbe_r55;
  llvm_cbe___CallFf_32___arg7 = llvm_cbe_r56;
  llvm_cbe___CallFf_32___arg8 = llvm_cbe_r57;
  llvm_cbe___CallFf_32___call = ((l_fptr_79*)(void*)llvm_cbe___CallFf_32___callee)(llvm_cbe___CallFf_32___arg0, llvm_cbe___CallFf_32___arg1, llvm_cbe___CallFf_32___arg2, llvm_cbe___CallFf_32___arg3, llvm_cbe___CallFf_32___arg4, llvm_cbe___CallFf_32___arg5, llvm_cbe___CallFf_32___arg6, llvm_cbe___CallFf_32___arg7, llvm_cbe___CallFf_32___arg8);
  llvm_cbe_r2 = llvm_cbe___CallFf_32___call;
  *((void**)&llvm_cbe_r3) = ((void*)&set_NC_high_NC_contrast_NC_mode_NC_type);
  llvm_cbe___Move_34___load_op1 = llvm_cbe_r2;
  llvm_cbe_r4 = llvm_cbe___Move_34___load_op1;
  llvm_cbe_r5 = 6;
  llvm_cbe___Move_36___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_36___load_op1;
  llvm_cbe___Move_37___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_37___load_op1;
  llvm_cbe___CallFf_38___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_38___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_38___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_38___call = ((l_fptr_80*)(void*)llvm_cbe___CallFf_38___callee)(llvm_cbe___CallFf_38___arg0, llvm_cbe___CallFf_38___arg1);
  llvm_cbe_r3 = llvm_cbe___CallFf_38___call;
  llvm_cbe___Move_39___load_op1 = llvm_cbe_r2;
  llvm_cbe_r3 = llvm_cbe___Move_39___load_op1;
  llvm_cbe___Move_40___load_op1 = llvm_cbe_r3;
  llvm_cbe_r3 = llvm_cbe___Move_40___load_op1;
  llvm_cbe___Move_41___load_op1 = llvm_cbe_r3;
  llvm_cbe_r2 = llvm_cbe___Move_41___load_op1;
  llvm_cbe___Move_42___load_op1 = llvm_cbe_r2;
  llvm_cbe_r1 = llvm_cbe___Move_42___load_op1;
  llvm_cbe___Return_43___retval = llvm_cbe_r1;
  return llvm_cbe___Return_43___retval;
}


uint64_t player_NC_gas_NC_mask_NC_narrative_NC_mode_PC_f(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1, uint64_t llvm_cbe_arg_2) {
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r0 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r1 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r2 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r3 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r4 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r5 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r6 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r7 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r8 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r9 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r10 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r11 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r12 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r13 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r14 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r15 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r16 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r17 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r18 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r19 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r20 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r21 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r22 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r23 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r24 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r25 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r26 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r27 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r28 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r29 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r30 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r31 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r32 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r33 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r34 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r35 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r36 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r37 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r38 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r39 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r40 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r41 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r42 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r43 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r44 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r45 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r46 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r47 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r48 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r49 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r50 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r51 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r52 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r53 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r54 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r55 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r56 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r57 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r58 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r59 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r60 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r61 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r62 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r63 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r64 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r65 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r66 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r67 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r68 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r69 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r70 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r71 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r72 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r73 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r74 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r75 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r76 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r77 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r78 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r79 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r80 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r81 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r82 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r83 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r84 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r85 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r86 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r87 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r88 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r89 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r90 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r91 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r92 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r93 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r94 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r95 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r96 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r97 __POSTFIXALIGN__(8);    /* Address-exposed local */
  uint64_t llvm_cbe___Move_0___load_op1;
  uint64_t llvm_cbe___Move_1___load_op1;
  uint64_t llvm_cbe___Move_2___load_op1;
  float llvm_cbe___LoadStaticFloatImm_4___st_load;
  float llvm_cbe___Move_5___load_op1;
  void* llvm_cbe___CallFf_6___callee;
  float llvm_cbe___CallFf_6___arg0;
  uint64_t llvm_cbe___CallFf_6___call;
  void* llvm_cbe___CallFf_8___callee;
  uint64_t llvm_cbe___CallFf_8___call;
  uint64_t llvm_cbe___Move_10___load_op1;
  uint64_t llvm_cbe___Move_11___load_op1;
  uint64_t llvm_cbe___Move_12___load_op1;
  uint64_t llvm_cbe___Move_13___load_op1;
  uint64_t llvm_cbe___Move_14___load_op1;
  uint64_t llvm_cbe___Move_15___load_op1;
  void* llvm_cbe___Call_16___callee;
  uint64_t llvm_cbe___Call_16___arg0;
  uint64_t llvm_cbe___Call_16___arg1;
  uint64_t llvm_cbe___Call_16___arg2;
  uint64_t llvm_cbe___Call_16___call;
  uint64_t llvm_cbe___LoadStaticU64Imm_18___st_load;
  uint64_t llvm_cbe___Move_19___load_op1;
  void* llvm_cbe___Call_20___callee;
  uint64_t llvm_cbe___Call_20___arg0;
  uint64_t llvm_cbe___Call_20___call;
  uint64_t llvm_cbe___LoadStaticU64Imm_22___st_load;
  uint64_t llvm_cbe___Move_23___load_op1;
  void* llvm_cbe___CallFf_24___callee;
  uint64_t llvm_cbe___CallFf_24___arg0;
  uint64_t llvm_cbe___CallFf_24___call;
  uint64_t llvm_cbe___Return_25___retval;

  llvm_cbe_r49 = llvm_cbe_arg_0;
  llvm_cbe_r50 = llvm_cbe_arg_1;
  llvm_cbe_r51 = llvm_cbe_arg_2;
  llvm_cbe___Move_0___load_op1 = llvm_cbe_r49;
  llvm_cbe_r0 = llvm_cbe___Move_0___load_op1;
  llvm_cbe___Move_1___load_op1 = llvm_cbe_r50;
  llvm_cbe_r1 = llvm_cbe___Move_1___load_op1;
  llvm_cbe___Move_2___load_op1 = llvm_cbe_r51;
  llvm_cbe_r2 = llvm_cbe___Move_2___load_op1;
  *((void**)&llvm_cbe_r3) = ((void*)&player_NC_disable_NC_dialog_NC_look_NC_gestures);
  llvm_cbe___LoadStaticFloatImm_4___st_load = *(float*)(((&(&player_NC_gas_NC_mask_NC_narrative_NC_mode_PC_f_symbol_table)->array[((int64_t)1)])));
  *((float*)&llvm_cbe_r4) = llvm_cbe___LoadStaticFloatImm_4___st_load;
  llvm_cbe___Move_5___load_op1 = *((float*)&llvm_cbe_r4);
  *((float*)&llvm_cbe_r49) = llvm_cbe___Move_5___load_op1;
  llvm_cbe___CallFf_6___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_6___arg0 = *((float*)&llvm_cbe_r49);
  llvm_cbe___CallFf_6___call = ((l_fptr_81*)(void*)llvm_cbe___CallFf_6___callee)(llvm_cbe___CallFf_6___arg0);
  llvm_cbe_r3 = llvm_cbe___CallFf_6___call;
  *((void**)&llvm_cbe_r3) = ((void*)&player_NC_disable_NC_squeeze_NC_through_PC_f);
  llvm_cbe___CallFf_8___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_8___call = ((l_fptr_82*)(void*)llvm_cbe___CallFf_8___callee)();
  llvm_cbe_r3 = llvm_cbe___CallFf_8___call;
  *((void**)&llvm_cbe_r3) = ((void*)&player_NC_set_NC_narrative_NC_mode_PC_f);
  llvm_cbe___Move_10___load_op1 = llvm_cbe_r0;
  llvm_cbe_r4 = llvm_cbe___Move_10___load_op1;
  llvm_cbe___Move_11___load_op1 = llvm_cbe_r1;
  llvm_cbe_r5 = llvm_cbe___Move_11___load_op1;
  llvm_cbe___Move_12___load_op1 = llvm_cbe_r2;
  llvm_cbe_r6 = llvm_cbe___Move_12___load_op1;
  llvm_cbe___Move_13___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_13___load_op1;
  llvm_cbe___Move_14___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_14___load_op1;
  llvm_cbe___Move_15___load_op1 = llvm_cbe_r6;
  llvm_cbe_r51 = llvm_cbe___Move_15___load_op1;
  llvm_cbe___Call_16___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___Call_16___arg0 = llvm_cbe_r49;
  llvm_cbe___Call_16___arg1 = llvm_cbe_r50;
  llvm_cbe___Call_16___arg2 = llvm_cbe_r51;
  llvm_cbe___Call_16___call = ((l_fptr_83*)(void*)llvm_cbe___Call_16___callee)(llvm_cbe___Call_16___arg0, llvm_cbe___Call_16___arg1, llvm_cbe___Call_16___arg2);
  llvm_cbe_r3 = llvm_cbe___Call_16___call;
  *((void**)&llvm_cbe_r3) = ((void*)&suspend_NC_idle_NC_gestures_PC_f);
  llvm_cbe___LoadStaticU64Imm_18___st_load = *(uint64_t*)(((&(&player_NC_gas_NC_mask_NC_narrative_NC_mode_PC_f_symbol_table)->array[((int64_t)5)])));
  llvm_cbe_r4 = llvm_cbe___LoadStaticU64Imm_18___st_load;
  llvm_cbe___Move_19___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_19___load_op1;
  llvm_cbe___Call_20___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___Call_20___arg0 = llvm_cbe_r49;
  llvm_cbe___Call_20___call = ((l_fptr_84*)(void*)llvm_cbe___Call_20___callee)(llvm_cbe___Call_20___arg0);
  llvm_cbe_r3 = llvm_cbe___Call_20___call;
  *((void**)&llvm_cbe_r3) = ((void*)&joypad_NC_disable_NC_commands);
  llvm_cbe___LoadStaticU64Imm_22___st_load = *(uint64_t*)(((&(&player_NC_gas_NC_mask_NC_narrative_NC_mode_PC_f_symbol_table)->array[((int64_t)7)])));
  llvm_cbe_r4 = llvm_cbe___LoadStaticU64Imm_22___st_load;
  llvm_cbe___Move_23___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_23___load_op1;
  llvm_cbe___CallFf_24___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_24___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_24___call = ((l_fptr_85*)(void*)llvm_cbe___CallFf_24___callee)(llvm_cbe___CallFf_24___arg0);
  llvm_cbe_r3 = llvm_cbe___CallFf_24___call;
  llvm_cbe___Return_25___retval = llvm_cbe_r3;
  return llvm_cbe___Return_25___retval;
}


uint64_t internal_NC_put_NC_off_NC_gas_NC_mask(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1) {
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r0 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r1 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r2 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r3 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r4 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r5 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r6 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r7 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r8 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r9 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r10 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r11 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r12 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r13 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r14 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r15 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r16 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r17 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r18 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r19 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r20 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r21 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r22 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r23 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r24 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r25 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r26 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r27 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r28 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r29 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r30 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r31 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r32 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r33 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r34 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r35 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r36 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r37 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r38 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r39 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r40 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r41 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r42 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r43 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r44 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r45 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r46 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r47 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r48 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r49 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r50 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r51 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r52 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r53 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r54 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r55 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r56 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r57 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r58 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r59 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r60 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r61 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r62 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r63 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r64 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r65 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r66 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r67 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r68 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r69 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r70 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r71 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r72 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r73 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r74 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r75 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r76 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r77 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r78 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r79 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r80 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r81 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r82 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r83 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r84 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r85 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r86 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r87 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r88 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r89 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r90 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r91 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r92 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r93 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r94 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r95 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r96 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r97 __POSTFIXALIGN__(8);    /* Address-exposed local */
  uint64_t llvm_cbe___Move_0___load_op1;
  uint64_t llvm_cbe___Move_1___load_op1;
  void* llvm_cbe___CallFf_3___callee;
  uint64_t llvm_cbe___CallFf_3___call;
  uint64_t llvm_cbe___Move_6___load_op1;
  uint64_t llvm_cbe___Move_7___load_op1;
  void* llvm_cbe___CallFf_8___callee;
  uint64_t llvm_cbe___CallFf_8___arg0;
  uint64_t llvm_cbe___CallFf_8___call;
  uint64_t llvm_cbe___LoadStaticU64Imm_9___st_load;
  uint64_t llvm_cbe___Move_10___load_op1;
  uint64_t llvm_cbe___Move_11___load_op1;
  void* llvm_cbe___CallFf_12___callee;
  uint64_t llvm_cbe___CallFf_12___arg0;
  uint64_t llvm_cbe___CallFf_12___arg1;
  uint64_t llvm_cbe___CallFf_12___call;
  uint64_t llvm_cbe___Move_14___load_op1;
  uint64_t llvm_cbe___Move_15___load_op1;
  void* llvm_cbe___CallFf_16___callee;
  uint64_t llvm_cbe___CallFf_16___arg0;
  uint64_t llvm_cbe___CallFf_16___call;
  uint64_t llvm_cbe___Move_18___load_op1;
  uint64_t llvm_cbe___Move_19___load_op1;
  void* llvm_cbe___AssertPointer_20___ptr;
  void* llvm_cbe___IAddImm_21___load_lhs;
  void* llvm_cbe___LoadU8_22___addr;
  uint8_t llvm_cbe___LoadU8_22___load;
  uint64_t llvm_cbe___Move_23___load_op1;
  uint64_t llvm_cbe___Move_24___load_op1;
  void* llvm_cbe___Call_25___callee;
  uint64_t llvm_cbe___Call_25___arg0;
  uint64_t llvm_cbe___Call_25___arg1;
  uint64_t llvm_cbe___Call_25___call;
  uint64_t llvm_cbe___Move_27___load_op1;
  uint64_t llvm_cbe___Move_28___load_op1;
  void* llvm_cbe___AssertPointer_29___ptr;
  void* llvm_cbe___IAddImm_30___load_lhs;
  void* llvm_cbe___LoadU64_31___addr;
  uint64_t llvm_cbe___LoadU64_31___load;
  uint64_t llvm_cbe___LoadStaticU64Imm_33___st_load;
  uint64_t llvm_cbe___Move_37___load_op1;
  uint64_t llvm_cbe___Move_38___load_op1;
  uint64_t llvm_cbe___Move_39___load_op1;
  uint64_t llvm_cbe___Move_40___load_op1;
  void* llvm_cbe___CallFf_41___callee;
  uint64_t llvm_cbe___CallFf_41___arg0;
  uint64_t llvm_cbe___CallFf_41___arg1;
  uint64_t llvm_cbe___CallFf_41___arg2;
  uint64_t llvm_cbe___CallFf_41___arg3;
  uint64_t llvm_cbe___CallFf_41___call;
  uint64_t llvm_cbe___Move_42___load_op1;
  uint64_t llvm_cbe___Move_43___load_op1;
  uint64_t llvm_cbe___Move_44___load_op1;
  uint64_t llvm_cbe___Move_45___load_op1;
  uint64_t llvm_cbe___Move_46___load_op1;
  void* llvm_cbe___CallFf_47___callee;
  uint64_t llvm_cbe___CallFf_47___arg0;
  uint64_t llvm_cbe___CallFf_47___arg1;
  uint64_t llvm_cbe___CallFf_47___arg2;
  uint64_t llvm_cbe___CallFf_47___call;
  uint64_t llvm_cbe___Move_49___load_op1;
  uint64_t llvm_cbe___Move_50___load_op1;
  void* llvm_cbe___AssertPointer_51___ptr;
  void* llvm_cbe___IAddImm_52___load_lhs;
  void* llvm_cbe___LoadU64_53___addr;
  uint64_t llvm_cbe___LoadU64_53___load;
  void* llvm_cbe___CallFf_55___callee;
  uint64_t llvm_cbe___CallFf_55___call;
  uint64_t llvm_cbe___Move_56___load_op1;
  uint64_t llvm_cbe___IAddImm_57___load_lhs;
  uint64_t llvm_cbe___StoreU8_59___value;
  void* llvm_cbe___StoreU8_59___addr;
  uint64_t llvm_cbe___Move_60___load_op1;
  uint64_t llvm_cbe___IAddImm_61___load_lhs;
  uint64_t llvm_cbe___StoreI32_63___value;
  void* llvm_cbe___StoreI32_63___addr;
  uint64_t llvm_cbe___Move_64___load_op1;
  uint64_t llvm_cbe___IAddImm_65___load_lhs;
  uint64_t llvm_cbe___StoreU8_67___value;
  void* llvm_cbe___StoreU8_67___addr;
  uint64_t llvm_cbe___Move_68___load_op1;
  uint64_t llvm_cbe___IAddImm_69___load_lhs;
  uint64_t llvm_cbe___StoreU8_71___value;
  void* llvm_cbe___StoreU8_71___addr;
  uint64_t llvm_cbe___Move_72___load_op1;
  uint64_t llvm_cbe___Move_73___load_op1;
  uint64_t llvm_cbe___Move_74___load_op1;
  uint64_t llvm_cbe___Move_75___load_op1;
  uint64_t llvm_cbe___Move_76___load_op1;
  void* llvm_cbe___CallFf_77___callee;
  uint64_t llvm_cbe___CallFf_77___arg0;
  uint64_t llvm_cbe___CallFf_77___arg1;
  uint64_t llvm_cbe___CallFf_77___arg2;
  uint64_t llvm_cbe___CallFf_77___call;
  uint64_t llvm_cbe___IEqual_79___load_lhs;
  uint64_t llvm_cbe___IEqual_79___load_rhs;
  bool llvm_cbe___OpLogNot_80___load_op1;
  bool llvm_cbe___BranchIfNot_81___cond;
  void* llvm_cbe___CallFf_83___callee;
  uint64_t llvm_cbe___CallFf_83___call;
  uint64_t llvm_cbe___Move_85___load_op1;
  uint64_t llvm_cbe___BranchIfNot_86___cond;
  uint64_t llvm_cbe___Move_88___load_op1;
  uint64_t llvm_cbe___Move_89___load_op1;
  void* llvm_cbe___CallFf_90___callee;
  uint64_t llvm_cbe___CallFf_90___arg0;
  uint64_t llvm_cbe___CallFf_90___call;
  uint64_t llvm_cbe___Move_93___load_op1;
  uint64_t llvm_cbe___Return_94___retval;

  llvm_cbe_r49 = llvm_cbe_arg_0;
  llvm_cbe_r50 = llvm_cbe_arg_1;
  llvm_cbe___Move_0___load_op1 = llvm_cbe_r49;
  llvm_cbe_r0 = llvm_cbe___Move_0___load_op1;
  llvm_cbe___Move_1___load_op1 = llvm_cbe_r50;
  llvm_cbe_r1 = llvm_cbe___Move_1___load_op1;
  *((void**)&llvm_cbe_r2) = ((void*)&wait_NC_one_NC_frame);
  llvm_cbe___CallFf_3___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_3___call = ((l_fptr_86*)(void*)llvm_cbe___CallFf_3___callee)();
  llvm_cbe_r2 = llvm_cbe___CallFf_3___call;
  *((void**)&llvm_cbe_r2) = ((void*)&lookup_NC_symbol);
  *((void**)&llvm_cbe_r3) = ((void*)&get_NC_gas_NC_mask_NC_setting_NC_id);
  llvm_cbe___Move_6___load_op1 = llvm_cbe_r0;
  llvm_cbe_r4 = llvm_cbe___Move_6___load_op1;
  llvm_cbe___Move_7___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_7___load_op1;
  llvm_cbe___CallFf_8___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_8___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_8___call = ((l_fptr_87*)(void*)llvm_cbe___CallFf_8___callee)(llvm_cbe___CallFf_8___arg0);
  llvm_cbe_r3 = llvm_cbe___CallFf_8___call;
  llvm_cbe___LoadStaticU64Imm_9___st_load = *(uint64_t*)(((&(&internal_NC_put_NC_off_NC_gas_NC_mask_symbol_table)->array[((int64_t)3)])));
  llvm_cbe_r4 = llvm_cbe___LoadStaticU64Imm_9___st_load;
  llvm_cbe___Move_10___load_op1 = llvm_cbe_r3;
  llvm_cbe_r49 = llvm_cbe___Move_10___load_op1;
  llvm_cbe___Move_11___load_op1 = llvm_cbe_r4;
  llvm_cbe_r50 = llvm_cbe___Move_11___load_op1;
  llvm_cbe___CallFf_12___callee = *((void**)&llvm_cbe_r2);
  llvm_cbe___CallFf_12___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_12___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_12___call = ((l_fptr_88*)(void*)llvm_cbe___CallFf_12___callee)(llvm_cbe___CallFf_12___arg0, llvm_cbe___CallFf_12___arg1);
  llvm_cbe_r2 = llvm_cbe___CallFf_12___call;
  *((void**)&llvm_cbe_r3) = ((void*)&get_NC_gas_NC_mask);
  llvm_cbe___Move_14___load_op1 = llvm_cbe_r0;
  llvm_cbe_r4 = llvm_cbe___Move_14___load_op1;
  llvm_cbe___Move_15___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_15___load_op1;
  llvm_cbe___CallFf_16___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_16___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_16___call = ((l_fptr_89*)(void*)llvm_cbe___CallFf_16___callee)(llvm_cbe___CallFf_16___arg0);
  llvm_cbe_r3 = llvm_cbe___CallFf_16___call;
  *((void**)&llvm_cbe_r4) = ((void*)&wait_NC_until_NC_in_NC_valid_NC_gas_NC_mask_NC_state);
  llvm_cbe___Move_18___load_op1 = llvm_cbe_r0;
  llvm_cbe_r5 = llvm_cbe___Move_18___load_op1;
  llvm_cbe___Move_19___load_op1 = llvm_cbe_r2;
  llvm_cbe_r6 = llvm_cbe___Move_19___load_op1;
  llvm_cbe___AssertPointer_20___ptr = *((void**)&llvm_cbe_r6);
  llvm_cbe___IAddImm_21___load_lhs = *((void**)&llvm_cbe_r6);
  *((void**)&llvm_cbe_r6) = (((&((uint8_t*)llvm_cbe___IAddImm_21___load_lhs)[((int8_t)57)])));
  llvm_cbe___LoadU8_22___addr = *((void**)&llvm_cbe_r6);
  llvm_cbe___LoadU8_22___load = *(uint8_t*)llvm_cbe___LoadU8_22___addr;
  llvm_cbe_r6 = (((uint64_t)(uint8_t)llvm_cbe___LoadU8_22___load));
  llvm_cbe___Move_23___load_op1 = llvm_cbe_r5;
  llvm_cbe_r49 = llvm_cbe___Move_23___load_op1;
  llvm_cbe___Move_24___load_op1 = llvm_cbe_r6;
  llvm_cbe_r50 = llvm_cbe___Move_24___load_op1;
  llvm_cbe___Call_25___callee = *((void**)&llvm_cbe_r4);
  llvm_cbe___Call_25___arg0 = llvm_cbe_r49;
  llvm_cbe___Call_25___arg1 = llvm_cbe_r50;
  llvm_cbe___Call_25___call = ((l_fptr_90*)(void*)llvm_cbe___Call_25___callee)(llvm_cbe___Call_25___arg0, llvm_cbe___Call_25___arg1);
  llvm_cbe_r4 = llvm_cbe___Call_25___call;
  *((void**)&llvm_cbe_r4) = ((void*)&animate_);
  llvm_cbe___Move_27___load_op1 = llvm_cbe_r3;
  llvm_cbe_r5 = llvm_cbe___Move_27___load_op1;
  llvm_cbe___Move_28___load_op1 = llvm_cbe_r2;
  llvm_cbe_r6 = llvm_cbe___Move_28___load_op1;
  llvm_cbe___AssertPointer_29___ptr = *((void**)&llvm_cbe_r6);
  llvm_cbe___IAddImm_30___load_lhs = *((void**)&llvm_cbe_r6);
  *((void**)&llvm_cbe_r6) = (((&((uint8_t*)llvm_cbe___IAddImm_30___load_lhs)[((int8_t)32)])));
  llvm_cbe___LoadU64_31___addr = *((void**)&llvm_cbe_r6);
  llvm_cbe___LoadU64_31___load = *(uint64_t*)llvm_cbe___LoadU64_31___addr;
  llvm_cbe_r6 = llvm_cbe___LoadU64_31___load;
  *((void**)&llvm_cbe_r7) = ((void*)&_DC__FC_alloc_NC_array);
  llvm_cbe___LoadStaticU64Imm_33___st_load = *(uint64_t*)(((&(&internal_NC_put_NC_off_NC_gas_NC_mask_symbol_table)->array[((int64_t)8)])));
  llvm_cbe_r8 = llvm_cbe___LoadStaticU64Imm_33___st_load;
  llvm_cbe_r9 = 16;
  llvm_cbe_r10 = 0;
  llvm_cbe_r11 = 1;
  llvm_cbe___Move_37___load_op1 = llvm_cbe_r8;
  llvm_cbe_r49 = llvm_cbe___Move_37___load_op1;
  llvm_cbe___Move_38___load_op1 = llvm_cbe_r9;
  llvm_cbe_r50 = llvm_cbe___Move_38___load_op1;
  llvm_cbe___Move_39___load_op1 = llvm_cbe_r10;
  llvm_cbe_r51 = llvm_cbe___Move_39___load_op1;
  llvm_cbe___Move_40___load_op1 = llvm_cbe_r11;
  llvm_cbe_r52 = llvm_cbe___Move_40___load_op1;
  llvm_cbe___CallFf_41___callee = *((void**)&llvm_cbe_r7);
  llvm_cbe___CallFf_41___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_41___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_41___arg2 = llvm_cbe_r51;
  llvm_cbe___CallFf_41___arg3 = llvm_cbe_r52;
  llvm_cbe___CallFf_41___call = ((l_fptr_91*)(void*)llvm_cbe___CallFf_41___callee)(llvm_cbe___CallFf_41___arg0, llvm_cbe___CallFf_41___arg1, llvm_cbe___CallFf_41___arg2, llvm_cbe___CallFf_41___arg3);
  llvm_cbe_r7 = llvm_cbe___CallFf_41___call;
  llvm_cbe___Move_42___load_op1 = llvm_cbe_r7;
  llvm_cbe_r8 = llvm_cbe___Move_42___load_op1;
  llvm_cbe___Move_43___load_op1 = llvm_cbe_r8;
  llvm_cbe_r7 = llvm_cbe___Move_43___load_op1;
  llvm_cbe___Move_44___load_op1 = llvm_cbe_r5;
  llvm_cbe_r49 = llvm_cbe___Move_44___load_op1;
  llvm_cbe___Move_45___load_op1 = llvm_cbe_r6;
  llvm_cbe_r50 = llvm_cbe___Move_45___load_op1;
  llvm_cbe___Move_46___load_op1 = llvm_cbe_r7;
  llvm_cbe_r51 = llvm_cbe___Move_46___load_op1;
  llvm_cbe___CallFf_47___callee = *((void**)&llvm_cbe_r4);
  llvm_cbe___CallFf_47___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_47___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_47___arg2 = llvm_cbe_r51;
  llvm_cbe___CallFf_47___call = ((l_fptr_92*)(void*)llvm_cbe___CallFf_47___callee)(llvm_cbe___CallFf_47___arg0, llvm_cbe___CallFf_47___arg1, llvm_cbe___CallFf_47___arg2);
  llvm_cbe_r4 = llvm_cbe___CallFf_47___call;
  goto llvm_cbe_bb1;

  do {     /* Syntactic loop 'bb1' to make GCC happy */
llvm_cbe_bb1:
  *((void**)&llvm_cbe_r4) = ((void*)&gesture_);
  llvm_cbe___Move_49___load_op1 = llvm_cbe_r0;
  llvm_cbe_r5 = llvm_cbe___Move_49___load_op1;
  llvm_cbe___Move_50___load_op1 = llvm_cbe_r2;
  llvm_cbe_r6 = llvm_cbe___Move_50___load_op1;
  llvm_cbe___AssertPointer_51___ptr = *((void**)&llvm_cbe_r6);
  llvm_cbe___IAddImm_52___load_lhs = *((void**)&llvm_cbe_r6);
  *((void**)&llvm_cbe_r6) = (((&((uint8_t*)llvm_cbe___IAddImm_52___load_lhs)[((int8_t)8)])));
  llvm_cbe___LoadU64_53___addr = *((void**)&llvm_cbe_r6);
  llvm_cbe___LoadU64_53___load = *(uint64_t*)llvm_cbe___LoadU64_53___addr;
  llvm_cbe_r6 = llvm_cbe___LoadU64_53___load;
  *((void**)&llvm_cbe_r7) = ((void*)&alloc_NC_gesture_NC_play_NC_params);
  llvm_cbe___CallFf_55___callee = *((void**)&llvm_cbe_r7);
  llvm_cbe___CallFf_55___call = ((l_fptr_93*)(void*)llvm_cbe___CallFf_55___callee)();
  llvm_cbe_r7 = llvm_cbe___CallFf_55___call;
  llvm_cbe___Move_56___load_op1 = llvm_cbe_r7;
  llvm_cbe_r8 = llvm_cbe___Move_56___load_op1;
  llvm_cbe___IAddImm_57___load_lhs = llvm_cbe_r8;
  llvm_cbe_r8 = (llvm_add_u64(llvm_cbe___IAddImm_57___load_lhs, 33));
  llvm_cbe_r9 = 1;
  llvm_cbe___StoreU8_59___value = llvm_cbe_r9;
  llvm_cbe___StoreU8_59___addr = *((void**)&llvm_cbe_r8);
  *(uint8_t*)llvm_cbe___StoreU8_59___addr = (((uint8_t)llvm_cbe___StoreU8_59___value));
  llvm_cbe_r8 = llvm_cbe___StoreU8_59___value;
  llvm_cbe___Move_60___load_op1 = llvm_cbe_r7;
  llvm_cbe_r8 = llvm_cbe___Move_60___load_op1;
  llvm_cbe___IAddImm_61___load_lhs = llvm_cbe_r8;
  llvm_cbe_r8 = (llvm_add_u64(llvm_cbe___IAddImm_61___load_lhs, 36));
  llvm_cbe_r9 = 19;
  llvm_cbe___StoreI32_63___value = llvm_cbe_r9;
  llvm_cbe___StoreI32_63___addr = *((void**)&llvm_cbe_r8);
  *(uint32_t*)llvm_cbe___StoreI32_63___addr = (((uint32_t)llvm_cbe___StoreI32_63___value));
  llvm_cbe_r8 = llvm_cbe___StoreI32_63___value;
  llvm_cbe___Move_64___load_op1 = llvm_cbe_r7;
  llvm_cbe_r8 = llvm_cbe___Move_64___load_op1;
  llvm_cbe___IAddImm_65___load_lhs = llvm_cbe_r8;
  llvm_cbe_r8 = (llvm_add_u64(llvm_cbe___IAddImm_65___load_lhs, 32));
  llvm_cbe_r9 = 1;
  llvm_cbe___StoreU8_67___value = llvm_cbe_r9;
  llvm_cbe___StoreU8_67___addr = *((void**)&llvm_cbe_r8);
  *(uint8_t*)llvm_cbe___StoreU8_67___addr = (((uint8_t)llvm_cbe___StoreU8_67___value));
  llvm_cbe_r8 = llvm_cbe___StoreU8_67___value;
  llvm_cbe___Move_68___load_op1 = llvm_cbe_r7;
  llvm_cbe_r8 = llvm_cbe___Move_68___load_op1;
  llvm_cbe___IAddImm_69___load_lhs = llvm_cbe_r8;
  llvm_cbe_r8 = (llvm_add_u64(llvm_cbe___IAddImm_69___load_lhs, 34));
  llvm_cbe_r9 = 1;
  llvm_cbe___StoreU8_71___value = llvm_cbe_r9;
  llvm_cbe___StoreU8_71___addr = *((void**)&llvm_cbe_r8);
  *(uint8_t*)llvm_cbe___StoreU8_71___addr = (((uint8_t)llvm_cbe___StoreU8_71___value));
  llvm_cbe_r8 = llvm_cbe___StoreU8_71___value;
  llvm_cbe___Move_72___load_op1 = llvm_cbe_r7;
  llvm_cbe_r8 = llvm_cbe___Move_72___load_op1;
  llvm_cbe___Move_73___load_op1 = llvm_cbe_r8;
  llvm_cbe_r7 = llvm_cbe___Move_73___load_op1;
  llvm_cbe___Move_74___load_op1 = llvm_cbe_r5;
  llvm_cbe_r49 = llvm_cbe___Move_74___load_op1;
  llvm_cbe___Move_75___load_op1 = llvm_cbe_r6;
  llvm_cbe_r50 = llvm_cbe___Move_75___load_op1;
  llvm_cbe___Move_76___load_op1 = llvm_cbe_r7;
  llvm_cbe_r51 = llvm_cbe___Move_76___load_op1;
  llvm_cbe___CallFf_77___callee = *((void**)&llvm_cbe_r4);
  llvm_cbe___CallFf_77___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_77___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_77___arg2 = llvm_cbe_r51;
  llvm_cbe___CallFf_77___call = ((l_fptr_94*)(void*)llvm_cbe___CallFf_77___callee)(llvm_cbe___CallFf_77___arg0, llvm_cbe___CallFf_77___arg1, llvm_cbe___CallFf_77___arg2);
  llvm_cbe_r4 = llvm_cbe___CallFf_77___call;
  llvm_cbe_r5 = 0;
  llvm_cbe___IEqual_79___load_lhs = llvm_cbe_r4;
  llvm_cbe___IEqual_79___load_rhs = llvm_cbe_r5;
  *((bool*)&llvm_cbe_r4) = (llvm_cbe___IEqual_79___load_lhs == llvm_cbe___IEqual_79___load_rhs);
  llvm_cbe___OpLogNot_80___load_op1 = *((bool*)&llvm_cbe_r4);
  *((bool*)&llvm_cbe_r4) = (llvm_cbe___OpLogNot_80___load_op1 ^ 1);
  llvm_cbe___BranchIfNot_81___cond = *((bool*)&llvm_cbe_r4);
  if ((llvm_cbe___BranchIfNot_81___cond != 0)) {
    goto llvm_cbe_bb2;
  } else {
    goto llvm_cbe_bb3;
  }

llvm_cbe_bb2:
  *((void**)&llvm_cbe_r4) = ((void*)&wait_NC_one_NC_frame);
  llvm_cbe___CallFf_83___callee = *((void**)&llvm_cbe_r4);
  llvm_cbe___CallFf_83___call = ((l_fptr_95*)(void*)llvm_cbe___CallFf_83___callee)();
  llvm_cbe_r4 = llvm_cbe___CallFf_83___call;
  goto llvm_cbe_bb1;

  } while (1); /* end of syntactic loop 'bb1' */
llvm_cbe_bb3:
  llvm_cbe___Move_85___load_op1 = llvm_cbe_r1;
  llvm_cbe_r4 = llvm_cbe___Move_85___load_op1;
  llvm_cbe___BranchIfNot_86___cond = llvm_cbe_r4;
  if ((llvm_cbe___BranchIfNot_86___cond != UINT64_C(0))) {
    goto llvm_cbe_bb4;
  } else {
    goto llvm_cbe_bb5;
  }

llvm_cbe_bb4:
  *((void**)&llvm_cbe_r4) = ((void*)&kill_NC_entity);
  llvm_cbe___Move_88___load_op1 = llvm_cbe_r3;
  llvm_cbe_r5 = llvm_cbe___Move_88___load_op1;
  llvm_cbe___Move_89___load_op1 = llvm_cbe_r5;
  llvm_cbe_r49 = llvm_cbe___Move_89___load_op1;
  llvm_cbe___CallFf_90___callee = *((void**)&llvm_cbe_r4);
  llvm_cbe___CallFf_90___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_90___call = ((l_fptr_96*)(void*)llvm_cbe___CallFf_90___callee)(llvm_cbe___CallFf_90___arg0);
  llvm_cbe_r4 = llvm_cbe___CallFf_90___call;
  goto llvm_cbe_bb6;

llvm_cbe_bb5:
  llvm_cbe_r4 = 0;
  goto llvm_cbe_bb6;

llvm_cbe_bb6:
  llvm_cbe___Move_93___load_op1 = llvm_cbe_r4;
  llvm_cbe_r2 = llvm_cbe___Move_93___load_op1;
  llvm_cbe___Return_94___retval = llvm_cbe_r2;
  return llvm_cbe___Return_94___retval;
}


uint64_t wait_NC_remove_NC_gas_NC_mask(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1, uint64_t llvm_cbe_arg_2) {
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r0 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r1 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r2 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r3 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r4 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r5 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r6 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r7 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r8 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r9 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r10 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r11 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r12 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r13 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r14 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r15 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r16 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r17 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r18 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r19 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r20 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r21 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r22 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r23 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r24 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r25 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r26 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r27 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r28 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r29 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r30 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r31 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r32 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r33 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r34 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r35 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r36 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r37 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r38 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r39 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r40 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r41 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r42 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r43 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r44 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r45 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r46 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r47 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r48 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r49 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r50 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r51 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r52 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r53 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r54 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r55 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r56 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r57 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r58 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r59 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r60 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r61 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r62 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r63 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r64 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r65 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r66 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r67 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r68 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r69 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r70 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r71 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r72 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r73 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r74 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r75 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r76 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r77 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r78 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r79 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r80 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r81 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r82 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r83 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r84 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r85 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r86 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r87 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r88 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r89 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r90 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r91 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r92 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r93 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r94 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r95 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r96 __POSTFIXALIGN__(8);    /* Address-exposed local */
  __PREFIXALIGN__(8) uint64_t llvm_cbe_r97 __POSTFIXALIGN__(8);    /* Address-exposed local */
  uint64_t llvm_cbe___Move_0___load_op1;
  uint64_t llvm_cbe___Move_1___load_op1;
  uint64_t llvm_cbe___Move_2___load_op1;
  uint64_t llvm_cbe___Move_4___load_op1;
  uint64_t llvm_cbe___LoadStaticU64Imm_5___st_load;
  uint64_t llvm_cbe___Move_9___load_op1;
  uint64_t llvm_cbe___Move_10___load_op1;
  void* llvm_cbe___CallFf_11___callee;
  uint64_t llvm_cbe___CallFf_11___arg0;
  uint64_t llvm_cbe___CallFf_11___arg1;
  uint64_t llvm_cbe___CallFf_11___call;
  uint64_t llvm_cbe___Move_12___load_op1;
  uint64_t llvm_cbe___Move_13___load_op1;
  uint64_t llvm_cbe___Move_14___load_op1;
  void* llvm_cbe___CallFf_15___callee;
  uint64_t llvm_cbe___CallFf_15___arg0;
  uint64_t llvm_cbe___CallFf_15___arg1;
  uint64_t llvm_cbe___CallFf_15___arg2;
  uint64_t llvm_cbe___CallFf_15___call;
  uint64_t llvm_cbe___LoadStaticU64Imm_17___st_load;
  uint64_t llvm_cbe___Move_18___load_op1;
  void* llvm_cbe___Call_19___callee;
  uint64_t llvm_cbe___Call_19___arg0;
  uint64_t llvm_cbe___Call_19___call;
  uint64_t llvm_cbe___OpLogNot_20___load_op1;
  bool llvm_cbe___BranchIfNot_21___cond;
  void* llvm_cbe___CallFf_23___callee;
  uint64_t llvm_cbe___CallFf_23___call;
  uint64_t llvm_cbe___LoadStaticU64Imm_26___st_load;
  uint64_t llvm_cbe___LoadStaticU64Imm_27___st_load;
  uint64_t llvm_cbe___Move_30___load_op1;
  uint64_t llvm_cbe___Move_31___load_op1;
  uint64_t llvm_cbe___Move_32___load_op1;
  void* llvm_cbe___CallFf_33___callee;
  uint64_t llvm_cbe___CallFf_33___arg0;
  uint64_t llvm_cbe___CallFf_33___arg1;
  uint64_t llvm_cbe___CallFf_33___call;
  uint64_t llvm_cbe___Move_34___load_op1;
  uint64_t llvm_cbe___Move_35___load_op1;
  uint64_t llvm_cbe___Move_36___load_op1;
  void* llvm_cbe___CallFf_37___callee;
  uint64_t llvm_cbe___CallFf_37___arg0;
  uint64_t llvm_cbe___CallFf_37___arg1;
  uint64_t llvm_cbe___CallFf_37___arg2;
  uint64_t llvm_cbe___CallFf_37___call;
  uint64_t llvm_cbe___Move_38___load_op1;
  uint64_t llvm_cbe___BranchIfNot_39___cond;
  uint64_t llvm_cbe___Move_41___load_op1;
  uint64_t llvm_cbe___Move_42___load_op1;
  uint64_t llvm_cbe___Move_43___load_op1;
  uint64_t llvm_cbe___Move_44___load_op1;
  void* llvm_cbe___Call_45___callee;
  uint64_t llvm_cbe___Call_45___arg0;
  uint64_t llvm_cbe___Call_45___arg1;
  uint64_t llvm_cbe___Call_45___call;
  uint64_t llvm_cbe___Move_48___load_op1;
  uint64_t llvm_cbe___Move_49___load_op1;
  uint64_t llvm_cbe___Move_50___load_op1;
  uint64_t llvm_cbe___Move_51___load_op1;
  void* llvm_cbe___Call_52___callee;
  uint64_t llvm_cbe___Call_52___arg0;
  uint64_t llvm_cbe___Call_52___arg1;
  uint64_t llvm_cbe___Call_52___call;
  uint64_t llvm_cbe___Return_53___retval;

  llvm_cbe_r49 = llvm_cbe_arg_0;
  llvm_cbe_r50 = llvm_cbe_arg_1;
  llvm_cbe_r51 = llvm_cbe_arg_2;
  llvm_cbe___Move_0___load_op1 = llvm_cbe_r49;
  llvm_cbe_r0 = llvm_cbe___Move_0___load_op1;
  llvm_cbe___Move_1___load_op1 = llvm_cbe_r50;
  llvm_cbe_r1 = llvm_cbe___Move_1___load_op1;
  llvm_cbe___Move_2___load_op1 = llvm_cbe_r51;
  llvm_cbe_r2 = llvm_cbe___Move_2___load_op1;
  *((void**)&llvm_cbe_r3) = ((void*)&fact_NC_set);
  llvm_cbe___Move_4___load_op1 = llvm_cbe_r0;
  llvm_cbe_r4 = llvm_cbe___Move_4___load_op1;
  llvm_cbe___LoadStaticU64Imm_5___st_load = *(uint64_t*)(((&(&wait_NC_remove_NC_gas_NC_mask_symbol_table)->array[((int64_t)1)])));
  llvm_cbe_r5 = llvm_cbe___LoadStaticU64Imm_5___st_load;
  *((void**)&llvm_cbe_r6) = ((void*)&new_NC_boxed_NC_value);
  llvm_cbe_r7 = 1;
  llvm_cbe_r8 = 0;
  llvm_cbe___Move_9___load_op1 = llvm_cbe_r7;
  llvm_cbe_r49 = llvm_cbe___Move_9___load_op1;
  llvm_cbe___Move_10___load_op1 = llvm_cbe_r8;
  llvm_cbe_r50 = llvm_cbe___Move_10___load_op1;
  llvm_cbe___CallFf_11___callee = *((void**)&llvm_cbe_r6);
  llvm_cbe___CallFf_11___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_11___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_11___call = ((l_fptr_97*)(void*)llvm_cbe___CallFf_11___callee)(llvm_cbe___CallFf_11___arg0, llvm_cbe___CallFf_11___arg1);
  llvm_cbe_r6 = llvm_cbe___CallFf_11___call;
  llvm_cbe___Move_12___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_12___load_op1;
  llvm_cbe___Move_13___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_13___load_op1;
  llvm_cbe___Move_14___load_op1 = llvm_cbe_r6;
  llvm_cbe_r51 = llvm_cbe___Move_14___load_op1;
  llvm_cbe___CallFf_15___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_15___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_15___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_15___arg2 = llvm_cbe_r51;
  llvm_cbe___CallFf_15___call = ((l_fptr_98*)(void*)llvm_cbe___CallFf_15___callee)(llvm_cbe___CallFf_15___arg0, llvm_cbe___CallFf_15___arg1, llvm_cbe___CallFf_15___arg2);
  llvm_cbe_r3 = llvm_cbe___CallFf_15___call;
  goto llvm_cbe_bb1;

  do {     /* Syntactic loop 'bb1' to make GCC happy */
llvm_cbe_bb1:
  *((void**)&llvm_cbe_r3) = ((void*)&is_NC_script_NC_running_PD_);
  llvm_cbe___LoadStaticU64Imm_17___st_load = *(uint64_t*)(((&(&wait_NC_remove_NC_gas_NC_mask_symbol_table)->array[((int64_t)4)])));
  llvm_cbe_r4 = llvm_cbe___LoadStaticU64Imm_17___st_load;
  llvm_cbe___Move_18___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_18___load_op1;
  llvm_cbe___Call_19___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___Call_19___arg0 = llvm_cbe_r49;
  llvm_cbe___Call_19___call = ((l_fptr_99*)(void*)llvm_cbe___Call_19___callee)(llvm_cbe___Call_19___arg0);
  llvm_cbe_r3 = llvm_cbe___Call_19___call;
  llvm_cbe___OpLogNot_20___load_op1 = llvm_cbe_r3;
  llvm_cbe_r3 = (llvm_cbe___OpLogNot_20___load_op1 ^ -1);
  llvm_cbe___BranchIfNot_21___cond = *((bool*)&llvm_cbe_r3);
  if ((llvm_cbe___BranchIfNot_21___cond != 0)) {
    goto llvm_cbe_bb2;
  } else {
    goto llvm_cbe_bb3;
  }

llvm_cbe_bb2:
  *((void**)&llvm_cbe_r3) = ((void*)&wait_NC_one_NC_frame);
  llvm_cbe___CallFf_23___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_23___call = ((l_fptr_100*)(void*)llvm_cbe___CallFf_23___callee)();
  llvm_cbe_r3 = llvm_cbe___CallFf_23___call;
  goto llvm_cbe_bb1;

  } while (1); /* end of syntactic loop 'bb1' */
llvm_cbe_bb3:
  *((void**)&llvm_cbe_r3) = ((void*)&send_NC_event);
  llvm_cbe___LoadStaticU64Imm_26___st_load = *(uint64_t*)(((&(&wait_NC_remove_NC_gas_NC_mask_symbol_table)->array[((int64_t)7)])));
  llvm_cbe_r4 = llvm_cbe___LoadStaticU64Imm_26___st_load;
  llvm_cbe___LoadStaticU64Imm_27___st_load = *(uint64_t*)(((&(&wait_NC_remove_NC_gas_NC_mask_symbol_table)->array[((int64_t)4)])));
  llvm_cbe_r5 = llvm_cbe___LoadStaticU64Imm_27___st_load;
  *((void**)&llvm_cbe_r6) = ((void*)&new_NC_boxed_NC_value);
  llvm_cbe_r7 = 7;
  llvm_cbe___Move_30___load_op1 = llvm_cbe_r0;
  llvm_cbe_r8 = llvm_cbe___Move_30___load_op1;
  llvm_cbe___Move_31___load_op1 = llvm_cbe_r7;
  llvm_cbe_r49 = llvm_cbe___Move_31___load_op1;
  llvm_cbe___Move_32___load_op1 = llvm_cbe_r8;
  llvm_cbe_r50 = llvm_cbe___Move_32___load_op1;
  llvm_cbe___CallFf_33___callee = *((void**)&llvm_cbe_r6);
  llvm_cbe___CallFf_33___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_33___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_33___call = ((l_fptr_101*)(void*)llvm_cbe___CallFf_33___callee)(llvm_cbe___CallFf_33___arg0, llvm_cbe___CallFf_33___arg1);
  llvm_cbe_r6 = llvm_cbe___CallFf_33___call;
  llvm_cbe___Move_34___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_34___load_op1;
  llvm_cbe___Move_35___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_35___load_op1;
  llvm_cbe___Move_36___load_op1 = llvm_cbe_r6;
  llvm_cbe_r51 = llvm_cbe___Move_36___load_op1;
  llvm_cbe___CallFf_37___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___CallFf_37___arg0 = llvm_cbe_r49;
  llvm_cbe___CallFf_37___arg1 = llvm_cbe_r50;
  llvm_cbe___CallFf_37___arg2 = llvm_cbe_r51;
  llvm_cbe___CallFf_37___call = ((l_fptr_102*)(void*)llvm_cbe___CallFf_37___callee)(llvm_cbe___CallFf_37___arg0, llvm_cbe___CallFf_37___arg1, llvm_cbe___CallFf_37___arg2);
  llvm_cbe_r3 = llvm_cbe___CallFf_37___call;
  llvm_cbe___Move_38___load_op1 = llvm_cbe_r1;
  llvm_cbe_r3 = llvm_cbe___Move_38___load_op1;
  llvm_cbe___BranchIfNot_39___cond = llvm_cbe_r3;
  if ((llvm_cbe___BranchIfNot_39___cond != UINT64_C(0))) {
    goto llvm_cbe_bb4;
  } else {
    goto llvm_cbe_bb5;
  }

llvm_cbe_bb4:
  *((void**)&llvm_cbe_r3) = ((void*)&internal_NC_put_NC_off_NC_gas_NC_mask_NC_immediately);
  llvm_cbe___Move_41___load_op1 = llvm_cbe_r0;
  llvm_cbe_r4 = llvm_cbe___Move_41___load_op1;
  llvm_cbe___Move_42___load_op1 = llvm_cbe_r2;
  llvm_cbe_r5 = llvm_cbe___Move_42___load_op1;
  llvm_cbe___Move_43___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_43___load_op1;
  llvm_cbe___Move_44___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_44___load_op1;
  llvm_cbe___Call_45___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___Call_45___arg0 = llvm_cbe_r49;
  llvm_cbe___Call_45___arg1 = llvm_cbe_r50;
  llvm_cbe___Call_45___call = ((l_fptr_103*)(void*)llvm_cbe___Call_45___callee)(llvm_cbe___Call_45___arg0, llvm_cbe___Call_45___arg1);
  llvm_cbe_r3 = llvm_cbe___Call_45___call;
  goto llvm_cbe_bb6;

llvm_cbe_bb5:
  *((void**)&llvm_cbe_r3) = ((void*)&internal_NC_put_NC_off_NC_gas_NC_mask);
  llvm_cbe___Move_48___load_op1 = llvm_cbe_r0;
  llvm_cbe_r4 = llvm_cbe___Move_48___load_op1;
  llvm_cbe___Move_49___load_op1 = llvm_cbe_r2;
  llvm_cbe_r5 = llvm_cbe___Move_49___load_op1;
  llvm_cbe___Move_50___load_op1 = llvm_cbe_r4;
  llvm_cbe_r49 = llvm_cbe___Move_50___load_op1;
  llvm_cbe___Move_51___load_op1 = llvm_cbe_r5;
  llvm_cbe_r50 = llvm_cbe___Move_51___load_op1;
  llvm_cbe___Call_52___callee = *((void**)&llvm_cbe_r3);
  llvm_cbe___Call_52___arg0 = llvm_cbe_r49;
  llvm_cbe___Call_52___arg1 = llvm_cbe_r50;
  llvm_cbe___Call_52___call = ((l_fptr_104*)(void*)llvm_cbe___Call_52___callee)(llvm_cbe___Call_52___arg0, llvm_cbe___Call_52___arg1);
  llvm_cbe_r3 = llvm_cbe___Call_52___call;
  goto llvm_cbe_bb6;

llvm_cbe_bb6:
  llvm_cbe___Return_53___retval = llvm_cbe_r3;
  return llvm_cbe___Return_53___retval;
}


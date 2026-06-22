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
typedef uint64_t l_fptr_2(uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_3(uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_5(uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_1(uint64_t, uint64_t, uint64_t);
typedef uint64_t l_fptr_4(uint64_t, uint64_t, uint64_t);

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
const static __PREFIXALIGN__(8) struct l_array_11_uint64_t symbol_table __POSTFIXALIGN__(8);
const static __PREFIXALIGN__(8) struct l_array_26_uint64_t symbol_table_OC_2 __POSTFIXALIGN__(8);
const static __PREFIXALIGN__(8) struct l_array_19_uint64_t symbol_table_OC_4 __POSTFIXALIGN__(8);
const static __PREFIXALIGN__(8) struct l_array_1_uint64_t symbol_table_OC_5 __POSTFIXALIGN__(8);
const static __PREFIXALIGN__(8) struct l_array_18_uint64_t symbol_table_OC_7 __POSTFIXALIGN__(8);
const static __PREFIXALIGN__(8) struct l_array_5_uint64_t symbol_table_OC_8 __POSTFIXALIGN__(8);
const static __PREFIXALIGN__(8) struct l_array_4_uint64_t symbol_table_OC_9 __POSTFIXALIGN__(8);
const static __PREFIXALIGN__(8) struct l_array_6_uint64_t symbol_table_OC_10 __POSTFIXALIGN__(8);
const static __PREFIXALIGN__(8) struct l_array_8_uint64_t symbol_table_OC_11 __POSTFIXALIGN__(8);
const static __PREFIXALIGN__(8) struct l_array_12_uint64_t symbol_table_OC_12 __POSTFIXALIGN__(8);
const static __PREFIXALIGN__(8) struct l_array_10_uint64_t symbol_table_OC_13 __POSTFIXALIGN__(8);

/* Function Declarations */
uint64_t wait_NC_equip_NC_gas_NC_mask(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1, uint64_t llvm_cbe_arg_2);
uint64_t fact_NC_set(uint64_t _6, uint64_t _7, uint64_t _8);
uint64_t new_NC_boxed_NC_value(uint64_t _9, uint64_t _10);
uint64_t is_NC_script_NC_running_PD_(uint64_t _11);
uint64_t wait_NC_one_NC_frame(void);
uint64_t send_NC_event(uint64_t _12, uint64_t _13, uint64_t _14);
uint64_t internal_NC_put_NC_on_NC_gas_NC_mask_NC_immediately(uint64_t _15, uint64_t _16);
uint64_t internal_NC_put_NC_on_NC_gas_NC_mask(uint64_t _17, uint64_t _18);
uint64_t get_NC_gas_NC_mask(uint64_t _19);
uint64_t internal_NC_put_NC_on_NC_gas_NC_mask_OC_1(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1);
uint64_t lookup_NC_symbol(uint64_t _20, uint64_t _21);
uint64_t get_NC_gas_NC_mask_NC_setting_NC_id(uint64_t _22);
uint64_t wait_NC_until_NC_in_NC_valid_NC_gas_NC_mask_NC_state(uint64_t _23, uint64_t _24);
uint64_t spawn_NC_object(uint64_t _25, uint64_t _26, uint64_t _27, uint64_t _28, uint64_t _29, uint64_t _30, uint64_t _31, uint64_t _32, uint64_t _33);
uint64_t set_NC_high_NC_contrast_NC_mode_NC_type(uint64_t _34, uint64_t _35);
uint64_t is_NC_player_PD_(uint64_t _36);
uint64_t set_NC_instance_NC_flag_NC_hero(uint64_t _37, uint64_t _38);
uint64_t set_NC_show_NC_in_NC_flashlight_NC_flag(uint64_t _39, uint64_t _40);
uint64_t set_NC_shadow_NC_casting_NC_from_NC_npc_NC_flashlight(uint64_t _41, uint64_t _42);
uint64_t set_NC_gas_NC_mask(uint64_t _43, uint64_t _44);
uint64_t set_NC_visible(uint64_t _45, uint64_t _46);
uint64_t attach(uint64_t _47, uint64_t _48, uint64_t _49, float _50);
uint64_t animate_(uint64_t _51, uint64_t _52, uint64_t _53);
uint64_t _DC__FC_alloc_NC_array(uint64_t _54, uint64_t _55, uint64_t _56, uint64_t _57);
uint64_t gesture_(uint64_t _58, uint64_t _59, uint64_t _60);
uint64_t alloc_NC_gesture_NC_play_NC_params(void);
uint64_t internal_NC_put_NC_on_NC_gas_NC_mask_NC_immediately_OC_3(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1);
uint64_t get_NC_equipped_NC_gas_NC_mask(uint64_t llvm_cbe_arg_0);
uint64_t wait_NC_until_NC_in_NC_valid_NC_gas_NC_mask_NC_state_OC_6(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1);
uint64_t player_NC_in_NC_state_PD_(uint64_t _61);
uint64_t player_NC_force_NC_crouch(void);
uint64_t player_NC_is_NC_crouched_PD_(void);
uint64_t player_NC_force_NC_stand(void);
uint64_t _BC_(uint64_t _62);
uint64_t wait_NC_holster_NC_player_NC_weapon(uint64_t _63, float _64);
uint64_t holster_NC_player_NC_weapon_NC_instantly(float _65, uint64_t _66);
uint64_t wait_NC_npc_NC_holster_NC_weapon(uint64_t _67, uint64_t _68, uint64_t _69, float _70, uint64_t _71);
uint64_t npc_NC_has_NC_weapon_NC_in_NC_hand_PD_(uint64_t _72);
uint64_t wait_NC_npc_NC_set_NC_demeanor(uint64_t _73, uint64_t _74);
uint64_t npc_NC_gas_NC_mask_NC_narrative_NC_mode_PC_f(uint64_t llvm_cbe_arg_0);
uint64_t suspend_NC_idle_NC_gestures_PC_f(uint64_t _75);
uint64_t npc_NC_disable_NC_dialog_NC_look(uint64_t _76, float _77);
uint64_t internal_NC_put_NC_off_NC_gas_NC_mask_NC_immediately(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1);
uint64_t kill_NC_entity(uint64_t _78);
uint64_t spawn_NC_gas_NC_mask(uint64_t llvm_cbe_arg_0);
uint64_t player_NC_gas_NC_mask_NC_narrative_NC_mode_PC_f(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1, uint64_t llvm_cbe_arg_2);
uint64_t player_NC_disable_NC_dialog_NC_look_NC_gestures(float _79);
uint64_t player_NC_disable_NC_squeeze_NC_through_PC_f(void);
uint64_t player_NC_set_NC_narrative_NC_mode_PC_f(uint64_t _80, uint64_t _81, uint64_t _82);
uint64_t joypad_NC_disable_NC_commands(uint64_t _83);
uint64_t internal_NC_put_NC_off_NC_gas_NC_mask(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1);
uint64_t wait_NC_remove_NC_gas_NC_mask(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1, uint64_t llvm_cbe_arg_2);


/* Global Variable Definitions and Initialization */
static const __PREFIXALIGN__(8) struct l_array_11_uint64_t symbol_table __POSTFIXALIGN__(8) = { { INT64_C(-2210386714833194546), UINT64_C(7671589888336862994), INT64_C(3684810824690217768), UINT64_C(6481843155139167450), UINT64_C(6150028017864311825), INT64_C(-3047912596559836939), UINT64_C(4789555171397753330), INT64_C(-3266162430027026367), UINT64_C(6078802017353408438), UINT64_C(5608585023750540989), UINT64_C(13370543512150926718) } };
static const __PREFIXALIGN__(8) struct l_array_26_uint64_t symbol_table_OC_2 __POSTFIXALIGN__(8) = { { INT64_C(-3047912596559836939), UINT64_C(10788710626492949762), UINT64_C(11873189068114142331), INT64_C(3455792245619945009), UINT64_C(8203816822361639083), UINT64_C(7097052834867614476), INT64_C(267125575217391726), UINT64_C(4766017279285775524), UINT64_C(6532480022340268902), INT64_C(-429226074377614887), UINT64_C(11194266633679187071), INT64_C(-3854484060414110792), UINT64_C(12474852589895261042), UINT64_C(10700394290536318732), UINT64_C(12770083849123373124), UINT64_C(8667864169316382944), 0, UINT64_C(4789555171397753330), UINT64_C(6817768622307059015), INT64_C(3684810824690217768), 1050253722, UINT64_C(9266924145627749149), UINT64_C(10701087590709005978), UINT64_C(8534565298598421705), UINT64_C(6500315221283771953), INT64_C(-2962780980939953026) } };
static const __PREFIXALIGN__(8) struct l_array_19_uint64_t symbol_table_OC_4 __POSTFIXALIGN__(8) = { { UINT64_C(10788710626492949762), UINT64_C(11873189068114142331), INT64_C(3455792245619945009), UINT64_C(7097052834867614476), INT64_C(267125575217391726), UINT64_C(4766017279285775524), UINT64_C(12474852589895261042), UINT64_C(6532480022340268902), UINT64_C(4789555171397753330), UINT64_C(6817768622307059015), INT64_C(3684810824690217768), 0, INT64_C(-429226074377614887), INT64_C(1827492438953869695), UINT64_C(11194266633679187071), INT64_C(-3854484060414110792), UINT64_C(9266924145627749149), UINT64_C(10701087590709005978), UINT64_C(8534565298598421705) } };
static const __PREFIXALIGN__(8) struct l_array_1_uint64_t symbol_table_OC_5 __POSTFIXALIGN__(8) = { { UINT64_C(13370543512150926718) } };
static const __PREFIXALIGN__(8) struct l_array_18_uint64_t symbol_table_OC_7 __POSTFIXALIGN__(8) = { { UINT64_C(6532480022340268902), UINT64_C(9443492708748526833), UINT64_C(6740565536053766520), INT64_C(2237451099064635199), INT64_C(-3047912596559836939), UINT64_C(13615456637498249507), INT64_C(1646463116072042322), UINT64_C(8594926419455150747), UINT64_C(12638116831811436492), INT64_C(4007104907368024212), INT64_C(-3092934816467423265), 1065353216, INT64_C(-626103520744544694), 1045220557, UINT64_C(5808793362033684947), INT64_C(3212836864), UINT64_C(7683024912818272899), UINT64_C(8573502661920361253) } };
static const __PREFIXALIGN__(8) struct l_array_5_uint64_t symbol_table_OC_8 __POSTFIXALIGN__(8) = { { UINT64_C(4789555171397753330), UINT64_C(8119835013146602502), UINT64_C(13090054416398793830), UINT64_C(7610916627332680768), 0 } };
static const __PREFIXALIGN__(8) struct l_array_4_uint64_t symbol_table_OC_9 __POSTFIXALIGN__(8) = { { UINT64_C(13370543512150926718), UINT64_C(4789555171397753330), INT64_C(-3264453205741318791), INT64_C(-804116741684351865) } };
static const __PREFIXALIGN__(8) struct l_array_6_uint64_t symbol_table_OC_10 __POSTFIXALIGN__(8) = { { UINT64_C(10788710626492949762), UINT64_C(11873189068114142331), INT64_C(3455792245619945009), UINT64_C(7097052834867614476), INT64_C(267125575217391726), UINT64_C(4766017279285775524) } };
static const __PREFIXALIGN__(8) struct l_array_8_uint64_t symbol_table_OC_11 __POSTFIXALIGN__(8) = { { UINT64_C(7238593933813171071), 0, UINT64_C(8165897991486741957), INT64_C(-2826777519175913837), UINT64_C(13090054416398793830), UINT64_C(5008278420455340480), UINT64_C(7121253686161376880), INT64_C(-1699981380503476753) } };
static const __PREFIXALIGN__(8) struct l_array_12_uint64_t symbol_table_OC_12 __POSTFIXALIGN__(8) = { { INT64_C(-3047912596559836939), UINT64_C(10788710626492949762), UINT64_C(11873189068114142331), INT64_C(3455792245619945009), UINT64_C(13370543512150926718), UINT64_C(8203816822361639083), UINT64_C(9266924145627749149), UINT64_C(10701087590709005978), UINT64_C(8534565298598421705), UINT64_C(6500315221283771953), INT64_C(-2962780980939953026), INT64_C(-804116741684351865) } };
static const __PREFIXALIGN__(8) struct l_array_10_uint64_t symbol_table_OC_13 __POSTFIXALIGN__(8) = { { INT64_C(-2210386714833194546), UINT64_C(7671589888336862994), INT64_C(3684810824690217768), UINT64_C(6481843155139167450), UINT64_C(6150028017864311825), INT64_C(-3047912596559836939), UINT64_C(4789555171397753330), INT64_C(-3325057813225958160), UINT64_C(9887213260439779492), UINT64_C(11836123545942477555) } };


/* LLVM Intrinsic Builtin Function Bodies */
static __forceinline uint64_t llvm_add_u64(uint64_t a, uint64_t b) {
  uint64_t r = a + b;
  return r;
}
static __forceinline uint64_t llvm_lshr_u64(uint64_t a, uint64_t b) {
  uint64_t r = a >> b;
  return r;
}


/* Function Bodies */

uint64_t wait_NC_equip_NC_gas_NC_mask(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1, uint64_t llvm_cbe_arg_2) {
  uint64_t llvm_cbe___CallFf_11___call;
  uint64_t llvm_cbe___CallFf_15___call;
  uint64_t llvm_cbe___Call_19___call71;
  uint64_t llvm_cbe___CallFf_23___call;
  uint64_t llvm_cbe___Call_19___call;
  uint64_t llvm_cbe___CallFf_33___call;
  uint64_t llvm_cbe___CallFf_37___call;
  uint64_t llvm_cbe___Call_45___call;
  uint64_t llvm_cbe___Call_52___call;
  uint64_t llvm_cbe___Call_56___call74;
  uint64_t llvm_cbe___CallFf_60___call;
  uint64_t llvm_cbe___Call_56___call;
  uint64_t llvm_cbe___CallFf_65___call;

  llvm_cbe___CallFf_11___call =  /*tail*/ new_NC_boxed_NC_value(1, 1);
  llvm_cbe___CallFf_15___call =  /*tail*/ fact_NC_set(llvm_cbe_arg_0, UINT64_C(7671589888336862994), llvm_cbe___CallFf_11___call);
  llvm_cbe___Call_19___call71 =  /*tail*/ is_NC_script_NC_running_PD_(UINT64_C(6150028017864311825));
  if ((((bool)llvm_cbe___Call_19___call71&1u))) {
    goto llvm_cbe_bb3;
  } else {
    goto llvm_cbe_bb2;
  }

  do {     /* Syntactic loop 'bb2' to make GCC happy */
llvm_cbe_bb2:
  llvm_cbe___CallFf_23___call =  /*tail*/ wait_NC_one_NC_frame();
  llvm_cbe___Call_19___call =  /*tail*/ is_NC_script_NC_running_PD_(UINT64_C(6150028017864311825));
  if ((((bool)llvm_cbe___Call_19___call&1u))) {
    goto llvm_cbe_bb3;
  } else {
    goto llvm_cbe_bb2;
  }

  } while (1); /* end of syntactic loop 'bb2' */
llvm_cbe_bb3:
  llvm_cbe___CallFf_33___call =  /*tail*/ new_NC_boxed_NC_value(7, llvm_cbe_arg_0);
  llvm_cbe___CallFf_37___call =  /*tail*/ send_NC_event(INT64_C(-3266162430027026367), UINT64_C(6150028017864311825), llvm_cbe___CallFf_33___call);
  if ((((bool)llvm_cbe_arg_2&1u))) {
    goto llvm_cbe_bb4;
  } else {
    goto llvm_cbe_bb5;
  }

llvm_cbe_bb4:
  llvm_cbe___Call_45___call =  /*tail*/ internal_NC_put_NC_on_NC_gas_NC_mask_NC_immediately(llvm_cbe_arg_0, llvm_cbe_arg_1);
  goto llvm_cbe_bb6_2e_preheader;

llvm_cbe_bb5:
  llvm_cbe___Call_52___call =  /*tail*/ internal_NC_put_NC_on_NC_gas_NC_mask(llvm_cbe_arg_0, llvm_cbe_arg_1);
  goto llvm_cbe_bb6_2e_preheader;

llvm_cbe_bb6_2e_preheader:
  llvm_cbe___Call_56___call74 =  /*tail*/ is_NC_script_NC_running_PD_(UINT64_C(6150028017864311825));
  if ((((bool)llvm_cbe___Call_56___call74&1u))) {
    goto llvm_cbe_bb8;
  } else {
    goto llvm_cbe_bb7;
  }

  do {     /* Syntactic loop 'bb7' to make GCC happy */
llvm_cbe_bb7:
  llvm_cbe___CallFf_60___call =  /*tail*/ wait_NC_one_NC_frame();
  llvm_cbe___Call_56___call =  /*tail*/ is_NC_script_NC_running_PD_(UINT64_C(6150028017864311825));
  if ((((bool)llvm_cbe___Call_56___call&1u))) {
    goto llvm_cbe_bb8;
  } else {
    goto llvm_cbe_bb7;
  }

  } while (1); /* end of syntactic loop 'bb7' */
llvm_cbe_bb8:
  llvm_cbe___CallFf_65___call =  /*tail*/ get_NC_gas_NC_mask(llvm_cbe_arg_0);
  return llvm_cbe___CallFf_65___call;
}


uint64_t internal_NC_put_NC_on_NC_gas_NC_mask_OC_1(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1) {
  uint64_t llvm_cbe___CallFf_3___call;
  uint64_t llvm_cbe___CallFf_8___call;
  uint64_t llvm_cbe___CallFf_12___call;
  void* _1;
  uint8_t llvm_cbe___LoadU8_18___load;
  uint64_t llvm_cbe___Call_21___call;
  uint32_t llvm_cbe_extract_2e_t503;
  uint32_t llvm_cbe_extract_2e_t506;
  uint64_t llvm_cbe___LoadU64_30___load;
  uint64_t llvm_cbe___CallFf_48___call;
  uint32_t llvm_cbe_extract_2e_t;
  uint32_t llvm_cbe_extract_2e_t504;
  uint32_t llvm_cbe_arg_1_2e_sink502_2e_off0;
  uint32_t llvm_cbe_arg_1_2e_sink502_2e_off0__PHI_TEMPORARY;
  uint32_t llvm_cbe_arg_1_2e_sink502_2e_off32;
  uint32_t llvm_cbe_arg_1_2e_sink502_2e_off32__PHI_TEMPORARY;
  uint64_t llvm_cbe_r1_2e_sroa_2e_0_2e_0_2e_insert_2e_insert345;
  uint64_t llvm_cbe___CallFf_58___call;
  uint64_t llvm_cbe___CallFf_62___call;
  uint64_t llvm_cbe___CallFf_69___call;
  uint64_t llvm_cbe___CallFf_75___call;
  uint64_t llvm_cbe___CallFf_82___call;
  uint64_t llvm_cbe___CallFf_88___call;
  uint64_t llvm_cbe___CallFf_94___call;
  uint64_t llvm_cbe___CallFf_98___call;
  uint64_t llvm_cbe___CallFf_102___call;
  uint64_t llvm_cbe___CallFf_114___call;
  uint64_t llvm_cbe___CallFf_123___call;
  uint64_t llvm_cbe___CallFf_127___call;
  uint64_t llvm_cbe___LoadU64_133___load;
  uint64_t llvm_cbe___CallFf_143___call;
  void* _2;
  uint64_t llvm_cbe___CallFf_149___call;
  void* _3;
  uint64_t llvm_cbe___LoadU64_154___load;
  uint64_t llvm_cbe___CallFf_156___call;
  uint64_t llvm_cbe___CallFf_178___call;
  uint64_t llvm_cbe___CallFf_184___call;
  uint64_t llvm_cbe___LoadU64_191___load;
  uint64_t llvm_cbe___CallFf_201___call;
  uint64_t llvm_cbe___CallFf_207___call;

  llvm_cbe___CallFf_3___call =  /*tail*/ wait_NC_one_NC_frame();
  llvm_cbe___CallFf_8___call =  /*tail*/ get_NC_gas_NC_mask_NC_setting_NC_id(llvm_cbe_arg_0);
  llvm_cbe___CallFf_12___call =  /*tail*/ lookup_NC_symbol(llvm_cbe___CallFf_8___call, INT64_C(3455792245619945009));
  _1 = ((void*)(uintptr_t)llvm_cbe___CallFf_12___call);
  llvm_cbe___LoadU8_18___load = *(uint8_t*)(((&((uint8_t*)_1)[((int64_t)56)])));
  llvm_cbe___Call_21___call =  /*tail*/ wait_NC_until_NC_in_NC_valid_NC_gas_NC_mask_NC_state(llvm_cbe_arg_0, (((uint64_t)(uint8_t)llvm_cbe___LoadU8_18___load)));
  llvm_cbe_extract_2e_t503 = ((uint32_t)llvm_cbe_arg_1);
  llvm_cbe_extract_2e_t506 = ((uint32_t)(llvm_lshr_u64(llvm_cbe_arg_1, 32)));
  if ((llvm_cbe_arg_1 == UINT64_C(0))) {
    goto llvm_cbe_bb1;
  } else {
    llvm_cbe_arg_1_2e_sink502_2e_off0__PHI_TEMPORARY = llvm_cbe_extract_2e_t503;   /* for PHI node */
    llvm_cbe_arg_1_2e_sink502_2e_off32__PHI_TEMPORARY = llvm_cbe_extract_2e_t506;   /* for PHI node */
    goto llvm_cbe_bb3;
  }

llvm_cbe_bb1:
  llvm_cbe___LoadU64_30___load = *(uint64_t*)(((&((uint8_t*)_1)[((int64_t)40)])));
  llvm_cbe___CallFf_48___call =  /*tail*/ spawn_NC_object(llvm_cbe___LoadU64_30___load, INT64_C(267125575217391726), 0, 0, 1, 0, 0, 0, 0);
  llvm_cbe_extract_2e_t = ((uint32_t)llvm_cbe___CallFf_48___call);
  llvm_cbe_extract_2e_t504 = ((uint32_t)(llvm_lshr_u64(llvm_cbe___CallFf_48___call, 32)));
  llvm_cbe_arg_1_2e_sink502_2e_off0__PHI_TEMPORARY = llvm_cbe_extract_2e_t;   /* for PHI node */
  llvm_cbe_arg_1_2e_sink502_2e_off32__PHI_TEMPORARY = llvm_cbe_extract_2e_t504;   /* for PHI node */
  goto llvm_cbe_bb3;

llvm_cbe_bb3:
  llvm_cbe_arg_1_2e_sink502_2e_off0 = llvm_cbe_arg_1_2e_sink502_2e_off0__PHI_TEMPORARY;
  llvm_cbe_arg_1_2e_sink502_2e_off32 = llvm_cbe_arg_1_2e_sink502_2e_off32__PHI_TEMPORARY;
  llvm_cbe_r1_2e_sroa_2e_0_2e_0_2e_insert_2e_insert345 = ((((uint64_t)(uint32_t)llvm_cbe_arg_1_2e_sink502_2e_off32)) << 32) | (((uint64_t)(uint32_t)llvm_cbe_arg_1_2e_sink502_2e_off0));
  llvm_cbe___CallFf_58___call =  /*tail*/ set_NC_high_NC_contrast_NC_mode_NC_type(llvm_cbe_r1_2e_sroa_2e_0_2e_0_2e_insert_2e_insert345, 6);
  llvm_cbe___CallFf_62___call =  /*tail*/ is_NC_player_PD_(llvm_cbe_arg_0);
  if ((((bool)llvm_cbe___CallFf_62___call&1u))) {
    goto llvm_cbe_bb4;
  } else {
    goto llvm_cbe_bb5;
  }

llvm_cbe_bb4:
  llvm_cbe___CallFf_69___call =  /*tail*/ set_NC_instance_NC_flag_NC_hero(llvm_cbe_r1_2e_sroa_2e_0_2e_0_2e_insert_2e_insert345, 1);
  llvm_cbe___CallFf_75___call =  /*tail*/ set_NC_show_NC_in_NC_flashlight_NC_flag(llvm_cbe_r1_2e_sroa_2e_0_2e_0_2e_insert_2e_insert345, 0);
  goto llvm_cbe_bb6;

llvm_cbe_bb5:
  llvm_cbe___CallFf_82___call =  /*tail*/ set_NC_shadow_NC_casting_NC_from_NC_npc_NC_flashlight(llvm_cbe_r1_2e_sroa_2e_0_2e_0_2e_insert_2e_insert345, 0);
  goto llvm_cbe_bb6;

llvm_cbe_bb6:
  llvm_cbe___CallFf_88___call =  /*tail*/ set_NC_gas_NC_mask(llvm_cbe_arg_0, llvm_cbe_r1_2e_sroa_2e_0_2e_0_2e_insert_2e_insert345);
  llvm_cbe___CallFf_94___call =  /*tail*/ set_NC_visible(llvm_cbe_r1_2e_sroa_2e_0_2e_0_2e_insert_2e_insert345, 0);
  llvm_cbe___CallFf_98___call =  /*tail*/ is_NC_player_PD_(llvm_cbe_arg_0);
  if ((((bool)llvm_cbe___CallFf_98___call&1u))) {
    goto llvm_cbe_bb9;
  } else {
    goto llvm_cbe_bb7;
  }

llvm_cbe_bb7:
  llvm_cbe___CallFf_102___call =  /*tail*/ wait_NC_one_NC_frame();
  goto llvm_cbe_bb9;

llvm_cbe_bb9:
  llvm_cbe___CallFf_114___call =  /*tail*/ attach(llvm_cbe_r1_2e_sroa_2e_0_2e_0_2e_insert_2e_insert345, llvm_cbe_arg_0, UINT64_C(8667864169316382944), 0);
  llvm_cbe___CallFf_123___call =  /*tail*/ new_NC_boxed_NC_value(6, 0.300000012);
  llvm_cbe___CallFf_127___call =  /*tail*/ send_NC_event(UINT64_C(6817768622307059015), llvm_cbe_arg_0, llvm_cbe___CallFf_123___call);
  llvm_cbe___LoadU64_133___load = *(uint64_t*)(((&((uint8_t*)_1)[((int64_t)16)])));
  llvm_cbe___CallFf_143___call =  /*tail*/ _DC__FC_alloc_NC_array(UINT64_C(8534565298598421705), 16, 0, 1);
  _2 = ((void*)(uintptr_t)((((uint64_t)(uintptr_t)((void*)&animate_)) & -256) | (((uint64_t)(uint8_t)((uint8_t)(uintptr_t)((void*)&animate_))))));
  llvm_cbe___CallFf_149___call =  /*tail*/ ((l_fptr_1*)(void*)_2)(llvm_cbe_r1_2e_sroa_2e_0_2e_0_2e_insert_2e_insert345, llvm_cbe___LoadU64_133___load, llvm_cbe___CallFf_143___call);
  _3 = ((void*)(uintptr_t)((((uint64_t)(uintptr_t)((void*)&gesture_)) & -256) | (((uint64_t)(uint8_t)((uint8_t)(uintptr_t)((void*)&gesture_))))));
  goto llvm_cbe_bb10;

  do {     /* Syntactic loop 'bb10' to make GCC happy */
llvm_cbe_bb10:
  llvm_cbe___LoadU64_154___load = *(uint64_t*)_1;
  llvm_cbe___CallFf_156___call =  /*tail*/ alloc_NC_gesture_NC_play_NC_params();
  *(uint8_t*)(((void*)(uintptr_t)(llvm_add_u64(llvm_cbe___CallFf_156___call, 33)))) = 1;
  *(uint32_t*)(((void*)(uintptr_t)(llvm_add_u64(llvm_cbe___CallFf_156___call, 36)))) = 19;
  *(uint8_t*)(((void*)(uintptr_t)(llvm_add_u64(llvm_cbe___CallFf_156___call, 32)))) = 1;
  *(uint8_t*)(((void*)(uintptr_t)(llvm_add_u64(llvm_cbe___CallFf_156___call, 34)))) = 1;
  llvm_cbe___CallFf_178___call =  /*tail*/ ((l_fptr_2*)(void*)_3)(llvm_cbe_arg_0, llvm_cbe___LoadU64_154___load, llvm_cbe___CallFf_156___call);
  if ((llvm_cbe___CallFf_178___call == UINT64_C(0))) {
    goto llvm_cbe_bb12;
  } else {
    goto llvm_cbe_bb11;
  }

llvm_cbe_bb11:
  llvm_cbe___CallFf_184___call =  /*tail*/ wait_NC_one_NC_frame();
  goto llvm_cbe_bb10;

  } while (1); /* end of syntactic loop 'bb10' */
llvm_cbe_bb12:
  llvm_cbe___LoadU64_191___load = *(uint64_t*)(((&((uint8_t*)_1)[((int64_t)24)])));
  llvm_cbe___CallFf_201___call =  /*tail*/ _DC__FC_alloc_NC_array(UINT64_C(8534565298598421705), 16, 0, 1);
  llvm_cbe___CallFf_207___call =  /*tail*/ ((l_fptr_3*)(void*)_2)(llvm_cbe_r1_2e_sroa_2e_0_2e_0_2e_insert_2e_insert345, llvm_cbe___LoadU64_191___load, llvm_cbe___CallFf_201___call);
  return llvm_cbe___CallFf_207___call;
}


uint64_t internal_NC_put_NC_on_NC_gas_NC_mask_NC_immediately_OC_3(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1) {
  uint64_t llvm_cbe___CallFf_6___call;
  uint64_t llvm_cbe___CallFf_10___call;
  uint32_t llvm_cbe_extract_2e_t353;
  uint32_t llvm_cbe_extract_2e_t356;
  uint64_t llvm_cbe___LoadU64_19___load;
  uint64_t llvm_cbe___CallFf_37___call;
  uint32_t llvm_cbe_extract_2e_t;
  uint32_t llvm_cbe_extract_2e_t354;
  uint32_t llvm_cbe_arg_1_2e_sink352_2e_off0;
  uint32_t llvm_cbe_arg_1_2e_sink352_2e_off0__PHI_TEMPORARY;
  uint32_t llvm_cbe_arg_1_2e_sink352_2e_off32;
  uint32_t llvm_cbe_arg_1_2e_sink352_2e_off32__PHI_TEMPORARY;
  uint64_t llvm_cbe_r1_2e_sroa_2e_0_2e_0_2e_insert_2e_insert267;
  uint64_t llvm_cbe___CallFf_47___call;
  uint64_t llvm_cbe___CallFf_53___call;
  uint64_t llvm_cbe___CallFf_57___call;
  uint64_t llvm_cbe___CallFf_67___call;
  uint64_t llvm_cbe___CallFf_71___call;
  uint64_t llvm_cbe___CallFf_77___call;
  uint64_t llvm_cbe___CallFf_85___call;
  uint64_t llvm_cbe___CallFf_89___call;
  uint64_t llvm_cbe___CallFf_96___call;
  uint64_t llvm_cbe___CallFf_103___call;
  uint64_t llvm_cbe___LoadU64_109___load;
  uint64_t llvm_cbe___CallFf_119___call;
  uint64_t llvm_cbe___CallFf_151___call;

  llvm_cbe___CallFf_6___call =  /*tail*/ get_NC_gas_NC_mask_NC_setting_NC_id(llvm_cbe_arg_0);
  llvm_cbe___CallFf_10___call =  /*tail*/ lookup_NC_symbol(llvm_cbe___CallFf_6___call, INT64_C(3455792245619945009));
  llvm_cbe_extract_2e_t353 = ((uint32_t)llvm_cbe_arg_1);
  llvm_cbe_extract_2e_t356 = ((uint32_t)(llvm_lshr_u64(llvm_cbe_arg_1, 32)));
  if ((llvm_cbe_arg_1 == UINT64_C(0))) {
    goto llvm_cbe_bb1;
  } else {
    llvm_cbe_arg_1_2e_sink352_2e_off0__PHI_TEMPORARY = llvm_cbe_extract_2e_t353;   /* for PHI node */
    llvm_cbe_arg_1_2e_sink352_2e_off32__PHI_TEMPORARY = llvm_cbe_extract_2e_t356;   /* for PHI node */
    goto llvm_cbe_bb3;
  }

llvm_cbe_bb1:
  llvm_cbe___LoadU64_19___load = *(uint64_t*)(((&((uint8_t*)(((void*)(uintptr_t)llvm_cbe___CallFf_10___call)))[((int64_t)40)])));
  llvm_cbe___CallFf_37___call =  /*tail*/ spawn_NC_object(llvm_cbe___LoadU64_19___load, INT64_C(267125575217391726), 0, 0, 1, 0, 0, 0, 0);
  llvm_cbe_extract_2e_t = ((uint32_t)llvm_cbe___CallFf_37___call);
  llvm_cbe_extract_2e_t354 = ((uint32_t)(llvm_lshr_u64(llvm_cbe___CallFf_37___call, 32)));
  llvm_cbe_arg_1_2e_sink352_2e_off0__PHI_TEMPORARY = llvm_cbe_extract_2e_t;   /* for PHI node */
  llvm_cbe_arg_1_2e_sink352_2e_off32__PHI_TEMPORARY = llvm_cbe_extract_2e_t354;   /* for PHI node */
  goto llvm_cbe_bb3;

llvm_cbe_bb3:
  llvm_cbe_arg_1_2e_sink352_2e_off0 = llvm_cbe_arg_1_2e_sink352_2e_off0__PHI_TEMPORARY;
  llvm_cbe_arg_1_2e_sink352_2e_off32 = llvm_cbe_arg_1_2e_sink352_2e_off32__PHI_TEMPORARY;
  llvm_cbe_r1_2e_sroa_2e_0_2e_0_2e_insert_2e_insert267 = ((((uint64_t)(uint32_t)llvm_cbe_arg_1_2e_sink352_2e_off32)) << 32) | (((uint64_t)(uint32_t)llvm_cbe_arg_1_2e_sink352_2e_off0));
  llvm_cbe___CallFf_47___call =  /*tail*/ set_NC_high_NC_contrast_NC_mode_NC_type(llvm_cbe_r1_2e_sroa_2e_0_2e_0_2e_insert_2e_insert267, 6);
  llvm_cbe___CallFf_53___call =  /*tail*/ set_NC_gas_NC_mask(llvm_cbe_arg_0, llvm_cbe_r1_2e_sroa_2e_0_2e_0_2e_insert_2e_insert267);
  llvm_cbe___CallFf_57___call =  /*tail*/ is_NC_player_PD_(llvm_cbe_arg_0);
  if ((((bool)llvm_cbe___CallFf_57___call&1u))) {
    goto llvm_cbe_bb4;
  } else {
    goto llvm_cbe_bb6;
  }

llvm_cbe_bb4:
  llvm_cbe___CallFf_67___call =  /*tail*/ new_NC_boxed_NC_value(6, 0);
  llvm_cbe___CallFf_71___call =  /*tail*/ send_NC_event(UINT64_C(6817768622307059015), llvm_cbe_arg_0, llvm_cbe___CallFf_67___call);
  llvm_cbe___CallFf_77___call =  /*tail*/ set_NC_instance_NC_flag_NC_hero(llvm_cbe_r1_2e_sroa_2e_0_2e_0_2e_insert_2e_insert267, 1);
  goto llvm_cbe_bb6;

llvm_cbe_bb6:
  llvm_cbe___CallFf_85___call =  /*tail*/ send_NC_event(INT64_C(1827492438953869695), llvm_cbe_arg_0);
  llvm_cbe___CallFf_89___call =  /*tail*/ is_NC_player_PD_(llvm_cbe_arg_0);
  if ((((bool)llvm_cbe___CallFf_89___call&1u))) {
    goto llvm_cbe_bb7;
  } else {
    goto llvm_cbe_bb8;
  }

llvm_cbe_bb7:
  llvm_cbe___CallFf_96___call =  /*tail*/ set_NC_show_NC_in_NC_flashlight_NC_flag(llvm_cbe_r1_2e_sroa_2e_0_2e_0_2e_insert_2e_insert267, 0);
  goto llvm_cbe_bb9;

llvm_cbe_bb8:
  llvm_cbe___CallFf_103___call =  /*tail*/ set_NC_shadow_NC_casting_NC_from_NC_npc_NC_flashlight(llvm_cbe_r1_2e_sroa_2e_0_2e_0_2e_insert_2e_insert267, 0);
  goto llvm_cbe_bb9;

llvm_cbe_bb9:
  llvm_cbe___LoadU64_109___load = *(uint64_t*)(((&((uint8_t*)(((void*)(uintptr_t)llvm_cbe___CallFf_10___call)))[((int64_t)24)])));
  llvm_cbe___CallFf_119___call =  /*tail*/ _DC__FC_alloc_NC_array(UINT64_C(8534565298598421705), 16, 1, 1);
  *(uint32_t*)(((void*)(uintptr_t)llvm_cbe___CallFf_119___call)) = 31;
  *(uint8_t*)(((void*)(uintptr_t)(llvm_add_u64(llvm_cbe___CallFf_119___call, 8)))) = 1;
  llvm_cbe___CallFf_151___call =  /*tail*/ ((l_fptr_4*)(void*)(((void*)(uintptr_t)((((uint64_t)(uintptr_t)((void*)&animate_)) & -256) | (((uint64_t)(uint8_t)((uint8_t)(uintptr_t)((void*)&animate_))))))))(llvm_cbe_r1_2e_sroa_2e_0_2e_0_2e_insert_2e_insert267, llvm_cbe___LoadU64_109___load, llvm_cbe___CallFf_119___call);
  return llvm_cbe___CallFf_151___call;
}


uint64_t get_NC_equipped_NC_gas_NC_mask(uint64_t llvm_cbe_arg_0) {
  uint64_t llvm_cbe___CallFf_4___call;

  llvm_cbe___CallFf_4___call =  /*tail*/ get_NC_gas_NC_mask(llvm_cbe_arg_0);
  return llvm_cbe___CallFf_4___call;
}


uint64_t wait_NC_until_NC_in_NC_valid_NC_gas_NC_mask_NC_state_OC_6(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1) {
  uint64_t llvm_cbe___CallFf_5___call;
  uint64_t llvm_cbe___CallFf_10___call;
  uint64_t llvm_cbe___CallFf_16___call;
  uint64_t llvm_cbe___CallFf_21___call;
  uint64_t llvm_cbe___CallFf_26___call;
  uint64_t llvm_cbe___CallFf_28___call250;
  uint64_t llvm_cbe___CallFf_50___call253;
  uint64_t llvm_cbe___CallFf_32___call;
  uint64_t llvm_cbe___CallFf_28___call;
  uint64_t llvm_cbe___CallFf_36___call;
  uint64_t llvm_cbe___CallFf_39___call246;
  uint64_t llvm_cbe___Call_41___call247;
  uint64_t llvm_cbe___CallFf_45___call;
  uint64_t llvm_cbe___CallFf_39___call;
  uint64_t llvm_cbe___Call_41___call;
  uint64_t llvm_cbe___CallFf_54___call;
  uint64_t llvm_cbe___CallFf_50___call;
  uint64_t llvm_cbe___CallFf_61___call;
  uint64_t llvm_cbe___CallFf_67___call;
  uint8_t llvm_cbe_r2_2e_sroa_2e_0_2e_0_2e_extract_2e_trunc179;
  uint64_t llvm_cbe_r2_2e_sroa_2e_39_2e_0_2e_extract_2e_trunc236;
  uint64_t llvm_cbe___CallFf_80___call;
  uint64_t llvm_cbe___CallFf_84___call244;
  uint64_t llvm_cbe___CallFf_87___call;
  uint64_t llvm_cbe___CallFf_84___call;
  uint64_t llvm_cbe___CallFf_96___call;
  uint8_t llvm_cbe_r2_2e_sroa_2e_0_2e_0_2e_extract_2e_trunc182;
  uint64_t llvm_cbe_r2_2e_sroa_2e_39_2e_0_2e_extract_2e_trunc242;
  uint8_t llvm_cbe_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_0_2e_r2_2e_0_2e___Return_99___retval;
  uint8_t llvm_cbe_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_0_2e_r2_2e_0_2e___Return_99___retval__PHI_TEMPORARY;
  uint64_t llvm_cbe_r2_2e_sroa_2e_39_2e_sroa_2e_0_2e_0;
  uint64_t llvm_cbe_r2_2e_sroa_2e_39_2e_sroa_2e_0_2e_0__PHI_TEMPORARY;

  llvm_cbe___CallFf_5___call =  /*tail*/ is_NC_player_PD_(llvm_cbe_arg_0);
  if ((((bool)llvm_cbe___CallFf_5___call&1u))) {
    goto llvm_cbe_bb1;
  } else {
    goto llvm_cbe_bb15;
  }

  do {     /* Syntactic loop 'bb1' to make GCC happy */
llvm_cbe_bb1:
  llvm_cbe___CallFf_10___call =  /*tail*/ player_NC_in_NC_state_PD_(UINT64_C(6740565536053766520));
  if ((((bool)llvm_cbe___CallFf_10___call&1u))) {
    goto llvm_cbe_bb3;
  } else {
    goto llvm_cbe_bb2;
  }

llvm_cbe_bb2:
  llvm_cbe___CallFf_16___call =  /*tail*/ player_NC_in_NC_state_PD_(INT64_C(2237451099064635199));
  if ((((bool)llvm_cbe___CallFf_16___call&1u))) {
    goto llvm_cbe_bb3;
  } else {
    goto llvm_cbe_bb4;
  }

llvm_cbe_bb3:
  llvm_cbe___CallFf_21___call =  /*tail*/ wait_NC_one_NC_frame();
  goto llvm_cbe_bb1;

  } while (1); /* end of syntactic loop 'bb1' */
llvm_cbe_bb4:
  if ((((bool)llvm_cbe_arg_1&1u))) {
    goto llvm_cbe_bb5;
  } else {
    goto llvm_cbe_bb9;
  }

llvm_cbe_bb5:
  llvm_cbe___CallFf_26___call =  /*tail*/ player_NC_force_NC_crouch();
  llvm_cbe___CallFf_28___call250 =  /*tail*/ player_NC_is_NC_crouched_PD_();
  if ((((bool)llvm_cbe___CallFf_28___call250&1u))) {
    goto llvm_cbe_bb12_2e_preheader;
  } else {
    goto llvm_cbe_bb7;
  }

llvm_cbe_bb12_2e_preheader:
  llvm_cbe___CallFf_50___call253 =  /*tail*/ player_NC_in_NC_state_PD_(INT64_C(4007104907368024212));
  if ((((bool)llvm_cbe___CallFf_50___call253&1u))) {
    goto llvm_cbe_bb14;
  } else {
    goto llvm_cbe_bb13;
  }

  do {     /* Syntactic loop 'bb7' to make GCC happy */
llvm_cbe_bb7:
  llvm_cbe___CallFf_32___call =  /*tail*/ wait_NC_one_NC_frame();
  llvm_cbe___CallFf_28___call =  /*tail*/ player_NC_is_NC_crouched_PD_();
  if ((((bool)llvm_cbe___CallFf_28___call&1u))) {
    goto llvm_cbe_bb12_2e_preheader;
  } else {
    goto llvm_cbe_bb7;
  }

  } while (1); /* end of syntactic loop 'bb7' */
llvm_cbe_bb9:
  llvm_cbe___CallFf_36___call =  /*tail*/ player_NC_force_NC_stand();
  llvm_cbe___CallFf_39___call246 =  /*tail*/ player_NC_is_NC_crouched_PD_();
  llvm_cbe___Call_41___call247 =  /*tail*/ _BC_(llvm_cbe___CallFf_39___call246);
  if ((((bool)llvm_cbe___Call_41___call247&1u))) {
    goto llvm_cbe_bb12_2e_preheader;
  } else {
    goto llvm_cbe_bb11;
  }

  do {     /* Syntactic loop 'bb11' to make GCC happy */
llvm_cbe_bb11:
  llvm_cbe___CallFf_45___call =  /*tail*/ wait_NC_one_NC_frame();
  llvm_cbe___CallFf_39___call =  /*tail*/ player_NC_is_NC_crouched_PD_();
  llvm_cbe___Call_41___call =  /*tail*/ _BC_(llvm_cbe___CallFf_39___call);
  if ((((bool)llvm_cbe___Call_41___call&1u))) {
    goto llvm_cbe_bb12_2e_preheader;
  } else {
    goto llvm_cbe_bb11;
  }

  } while (1); /* end of syntactic loop 'bb11' */
  do {     /* Syntactic loop 'bb13' to make GCC happy */
llvm_cbe_bb13:
  llvm_cbe___CallFf_54___call =  /*tail*/ wait_NC_one_NC_frame();
  llvm_cbe___CallFf_50___call =  /*tail*/ player_NC_in_NC_state_PD_(INT64_C(4007104907368024212));
  if ((((bool)llvm_cbe___CallFf_50___call&1u))) {
    goto llvm_cbe_bb14;
  } else {
    goto llvm_cbe_bb13;
  }

  } while (1); /* end of syntactic loop 'bb13' */
llvm_cbe_bb14:
  llvm_cbe___CallFf_61___call =  /*tail*/ wait_NC_holster_NC_player_NC_weapon(0, 1);
  llvm_cbe___CallFf_67___call =  /*tail*/ holster_NC_player_NC_weapon_NC_instantly(0.200000003, 0);
  llvm_cbe_r2_2e_sroa_2e_0_2e_0_2e_extract_2e_trunc179 = ((uint8_t)llvm_cbe___CallFf_67___call);
  llvm_cbe_r2_2e_sroa_2e_39_2e_0_2e_extract_2e_trunc236 = ((((uint64_t)(llvm_lshr_u64(llvm_cbe___CallFf_67___call, 8))))&4294967295);
  llvm_cbe_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_0_2e_r2_2e_0_2e___Return_99___retval__PHI_TEMPORARY = llvm_cbe_r2_2e_sroa_2e_0_2e_0_2e_extract_2e_trunc179;   /* for PHI node */
  llvm_cbe_r2_2e_sroa_2e_39_2e_sroa_2e_0_2e_0__PHI_TEMPORARY = llvm_cbe_r2_2e_sroa_2e_39_2e_0_2e_extract_2e_trunc236;   /* for PHI node */
  goto llvm_cbe_bb21;

llvm_cbe_bb15:
  llvm_cbe___CallFf_80___call =  /*tail*/ wait_NC_npc_NC_holster_NC_weapon(llvm_cbe_arg_0, 0, 1, -1, 0);
  llvm_cbe___CallFf_84___call244 =  /*tail*/ npc_NC_has_NC_weapon_NC_in_NC_hand_PD_(llvm_cbe_arg_0);
  if ((((bool)llvm_cbe___CallFf_84___call244&1u))) {
    goto llvm_cbe_bb17;
  } else {
    goto llvm_cbe_bb18;
  }

  do {     /* Syntactic loop 'bb17' to make GCC happy */
llvm_cbe_bb17:
  llvm_cbe___CallFf_87___call =  /*tail*/ wait_NC_one_NC_frame();
  llvm_cbe___CallFf_84___call =  /*tail*/ npc_NC_has_NC_weapon_NC_in_NC_hand_PD_(llvm_cbe_arg_0);
  if ((((bool)llvm_cbe___CallFf_84___call&1u))) {
    goto llvm_cbe_bb17;
  } else {
    goto llvm_cbe_bb18;
  }

  } while (1); /* end of syntactic loop 'bb17' */
llvm_cbe_bb18:
  if ((((bool)llvm_cbe_arg_1&1u))) {
    goto llvm_cbe_bb19;
  } else {
    llvm_cbe_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_0_2e_r2_2e_0_2e___Return_99___retval__PHI_TEMPORARY = 0;   /* for PHI node */
    llvm_cbe_r2_2e_sroa_2e_39_2e_sroa_2e_0_2e_0__PHI_TEMPORARY = 0;   /* for PHI node */
    goto llvm_cbe_bb21;
  }

llvm_cbe_bb19:
  llvm_cbe___CallFf_96___call =  /*tail*/ wait_NC_npc_NC_set_NC_demeanor(llvm_cbe_arg_0, 3);
  llvm_cbe_r2_2e_sroa_2e_0_2e_0_2e_extract_2e_trunc182 = ((uint8_t)llvm_cbe___CallFf_96___call);
  llvm_cbe_r2_2e_sroa_2e_39_2e_0_2e_extract_2e_trunc242 = ((((uint64_t)(llvm_lshr_u64(llvm_cbe___CallFf_96___call, 8))))&4294967295);
  llvm_cbe_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_0_2e_r2_2e_0_2e___Return_99___retval__PHI_TEMPORARY = llvm_cbe_r2_2e_sroa_2e_0_2e_0_2e_extract_2e_trunc182;   /* for PHI node */
  llvm_cbe_r2_2e_sroa_2e_39_2e_sroa_2e_0_2e_0__PHI_TEMPORARY = llvm_cbe_r2_2e_sroa_2e_39_2e_0_2e_extract_2e_trunc242;   /* for PHI node */
  goto llvm_cbe_bb21;

llvm_cbe_bb21:
  llvm_cbe_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_0_2e_r2_2e_0_2e___Return_99___retval = llvm_cbe_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_0_2e_r2_2e_0_2e___Return_99___retval__PHI_TEMPORARY;
  llvm_cbe_r2_2e_sroa_2e_39_2e_sroa_2e_0_2e_0 = ((llvm_cbe_r2_2e_sroa_2e_39_2e_sroa_2e_0_2e_0__PHI_TEMPORARY)&4294967295);
  return (((((uint64_t)(uint64_t)llvm_cbe_r2_2e_sroa_2e_39_2e_sroa_2e_0_2e_0)) << 8) | (((uint64_t)(uint8_t)llvm_cbe_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_sroa_2e_0_2e_0_2e_r2_2e_sroa_2e_0_2e_0_2e_r2_2e_0_2e___Return_99___retval)));
}


uint64_t npc_NC_gas_NC_mask_NC_narrative_NC_mode_PC_f(uint64_t llvm_cbe_arg_0) {
  uint64_t llvm_cbe___CallFf_6___call;
  uint64_t llvm_cbe___Call_10___call;
  uint64_t llvm_cbe___CallFf_16___call;

  llvm_cbe___CallFf_6___call =  /*tail*/ send_NC_event(UINT64_C(8119835013146602502), llvm_cbe_arg_0);
  llvm_cbe___Call_10___call =  /*tail*/ suspend_NC_idle_NC_gestures_PC_f(llvm_cbe_arg_0);
  llvm_cbe___CallFf_16___call =  /*tail*/ npc_NC_disable_NC_dialog_NC_look(llvm_cbe_arg_0, 0);
  return llvm_cbe___CallFf_16___call;
}


uint64_t internal_NC_put_NC_off_NC_gas_NC_mask_NC_immediately(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1) {
  uint64_t llvm_cbe___CallFf_5___call;
  uint64_t llvm_cbe___CallFf_11___call;
  uint64_t llvm_cbe___CallFf_17___call;
  uint64_t llvm_cbe_r3_2e_0;
  uint64_t llvm_cbe_r3_2e_0__PHI_TEMPORARY;

  llvm_cbe___CallFf_5___call =  /*tail*/ get_NC_gas_NC_mask(llvm_cbe_arg_0);
  llvm_cbe___CallFf_11___call =  /*tail*/ send_NC_event(INT64_C(-3264453205741318791), llvm_cbe_arg_0);
  if ((((bool)llvm_cbe_arg_1&1u))) {
    goto llvm_cbe_bb1;
  } else {
    llvm_cbe_r3_2e_0__PHI_TEMPORARY = 0;   /* for PHI node */
    goto llvm_cbe_bb3;
  }

llvm_cbe_bb1:
  llvm_cbe___CallFf_17___call =  /*tail*/ kill_NC_entity(llvm_cbe___CallFf_5___call);
  llvm_cbe_r3_2e_0__PHI_TEMPORARY = llvm_cbe___CallFf_17___call;   /* for PHI node */
  goto llvm_cbe_bb3;

llvm_cbe_bb3:
  llvm_cbe_r3_2e_0 = llvm_cbe_r3_2e_0__PHI_TEMPORARY;
  return llvm_cbe_r3_2e_0;
}


uint64_t spawn_NC_gas_NC_mask(uint64_t llvm_cbe_arg_0) {
  uint64_t llvm_cbe___CallFf_5___call;
  uint64_t llvm_cbe___CallFf_9___call;
  uint64_t llvm_cbe___LoadU64_14___load;
  uint64_t llvm_cbe___CallFf_32___call;
  uint64_t llvm_cbe___CallFf_38___call;

  llvm_cbe___CallFf_5___call =  /*tail*/ get_NC_gas_NC_mask_NC_setting_NC_id(llvm_cbe_arg_0);
  llvm_cbe___CallFf_9___call =  /*tail*/ lookup_NC_symbol(llvm_cbe___CallFf_5___call, INT64_C(3455792245619945009));
  llvm_cbe___LoadU64_14___load = *(uint64_t*)(((&((uint8_t*)(((void*)(uintptr_t)llvm_cbe___CallFf_9___call)))[((int64_t)40)])));
  llvm_cbe___CallFf_32___call =  /*tail*/ spawn_NC_object(llvm_cbe___LoadU64_14___load, INT64_C(267125575217391726), 0, 0, 1, 0, 0, 0, 0);
  llvm_cbe___CallFf_38___call =  /*tail*/ set_NC_high_NC_contrast_NC_mode_NC_type(llvm_cbe___CallFf_32___call, 6);
  return llvm_cbe___CallFf_32___call;
}


uint64_t player_NC_gas_NC_mask_NC_narrative_NC_mode_PC_f(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1, uint64_t llvm_cbe_arg_2) {
  uint64_t llvm_cbe___CallFf_6___call;
  uint64_t llvm_cbe___CallFf_8___call;
  uint64_t llvm_cbe___Call_16___call;
  uint64_t llvm_cbe___Call_20___call;
  uint64_t llvm_cbe___CallFf_24___call;

  llvm_cbe___CallFf_6___call =  /*tail*/ player_NC_disable_NC_dialog_NC_look_NC_gestures(0);
  llvm_cbe___CallFf_8___call =  /*tail*/ player_NC_disable_NC_squeeze_NC_through_PC_f();
  llvm_cbe___Call_16___call =  /*tail*/ player_NC_set_NC_narrative_NC_mode_PC_f(llvm_cbe_arg_0, llvm_cbe_arg_1, llvm_cbe_arg_2);
  llvm_cbe___Call_20___call =  /*tail*/ suspend_NC_idle_NC_gestures_PC_f(UINT64_C(5008278420455340480));
  llvm_cbe___CallFf_24___call =  /*tail*/ joypad_NC_disable_NC_commands(INT64_C(-1699981380503476753));
  return llvm_cbe___CallFf_24___call;
}


uint64_t internal_NC_put_NC_off_NC_gas_NC_mask(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1) {
  uint64_t llvm_cbe___CallFf_3___call;
  uint64_t llvm_cbe___CallFf_8___call;
  uint64_t llvm_cbe___CallFf_12___call;
  uint64_t llvm_cbe___CallFf_16___call;
  void* _4;
  uint8_t llvm_cbe___LoadU8_22___load;
  uint64_t llvm_cbe___Call_25___call;
  uint64_t llvm_cbe___LoadU64_31___load;
  uint64_t llvm_cbe___CallFf_41___call;
  uint64_t llvm_cbe___CallFf_47___call;
  void* llvm_cbe___IAddImm_52__op;
  void* _5;
  uint64_t llvm_cbe___LoadU64_53___load;
  uint64_t llvm_cbe___CallFf_55___call;
  uint64_t llvm_cbe___CallFf_77___call;
  uint64_t llvm_cbe___CallFf_83___call;
  uint64_t llvm_cbe___CallFf_90___call;
  uint64_t llvm_cbe_storemerge;
  uint64_t llvm_cbe_storemerge__PHI_TEMPORARY;

  llvm_cbe___CallFf_3___call =  /*tail*/ wait_NC_one_NC_frame();
  llvm_cbe___CallFf_8___call =  /*tail*/ get_NC_gas_NC_mask_NC_setting_NC_id(llvm_cbe_arg_0);
  llvm_cbe___CallFf_12___call =  /*tail*/ lookup_NC_symbol(llvm_cbe___CallFf_8___call, INT64_C(3455792245619945009));
  llvm_cbe___CallFf_16___call =  /*tail*/ get_NC_gas_NC_mask(llvm_cbe_arg_0);
  _4 = ((void*)(uintptr_t)llvm_cbe___CallFf_12___call);
  llvm_cbe___LoadU8_22___load = *(uint8_t*)(((&((uint8_t*)_4)[((int64_t)57)])));
  llvm_cbe___Call_25___call =  /*tail*/ wait_NC_until_NC_in_NC_valid_NC_gas_NC_mask_NC_state(llvm_cbe_arg_0, (((uint64_t)(uint8_t)llvm_cbe___LoadU8_22___load)));
  llvm_cbe___LoadU64_31___load = *(uint64_t*)(((&((uint8_t*)_4)[((int64_t)32)])));
  llvm_cbe___CallFf_41___call =  /*tail*/ _DC__FC_alloc_NC_array(UINT64_C(8534565298598421705), 16, 0, 1);
  llvm_cbe___CallFf_47___call =  /*tail*/ animate_(llvm_cbe___CallFf_16___call, llvm_cbe___LoadU64_31___load, llvm_cbe___CallFf_41___call);
  llvm_cbe___IAddImm_52__op = ((&((uint8_t*)_4)[((int64_t)8)]));
  _5 = ((void*)(uintptr_t)((((uint64_t)(uintptr_t)((void*)&gesture_)) & -256) | (((uint64_t)(uint8_t)((uint8_t)(uintptr_t)((void*)&gesture_))))));
  goto llvm_cbe_bb1;

  do {     /* Syntactic loop 'bb1' to make GCC happy */
llvm_cbe_bb1:
  llvm_cbe___LoadU64_53___load = *(uint64_t*)llvm_cbe___IAddImm_52__op;
  llvm_cbe___CallFf_55___call =  /*tail*/ alloc_NC_gesture_NC_play_NC_params();
  *(uint8_t*)(((void*)(uintptr_t)(llvm_add_u64(llvm_cbe___CallFf_55___call, 33)))) = 1;
  *(uint32_t*)(((void*)(uintptr_t)(llvm_add_u64(llvm_cbe___CallFf_55___call, 36)))) = 19;
  *(uint8_t*)(((void*)(uintptr_t)(llvm_add_u64(llvm_cbe___CallFf_55___call, 32)))) = 1;
  *(uint8_t*)(((void*)(uintptr_t)(llvm_add_u64(llvm_cbe___CallFf_55___call, 34)))) = 1;
  llvm_cbe___CallFf_77___call =  /*tail*/ ((l_fptr_5*)(void*)_5)(llvm_cbe_arg_0, llvm_cbe___LoadU64_53___load, llvm_cbe___CallFf_55___call);
  if ((llvm_cbe___CallFf_77___call == UINT64_C(0))) {
    goto llvm_cbe_bb3;
  } else {
    goto llvm_cbe_bb2;
  }

llvm_cbe_bb2:
  llvm_cbe___CallFf_83___call =  /*tail*/ wait_NC_one_NC_frame();
  goto llvm_cbe_bb1;

  } while (1); /* end of syntactic loop 'bb1' */
llvm_cbe_bb3:
  if ((((bool)llvm_cbe_arg_1&1u))) {
    goto llvm_cbe_bb4;
  } else {
    llvm_cbe_storemerge__PHI_TEMPORARY = 0;   /* for PHI node */
    goto llvm_cbe_bb6;
  }

llvm_cbe_bb4:
  llvm_cbe___CallFf_90___call =  /*tail*/ kill_NC_entity(llvm_cbe___CallFf_16___call);
  llvm_cbe_storemerge__PHI_TEMPORARY = llvm_cbe___CallFf_90___call;   /* for PHI node */
  goto llvm_cbe_bb6;

llvm_cbe_bb6:
  llvm_cbe_storemerge = llvm_cbe_storemerge__PHI_TEMPORARY;
  return llvm_cbe_storemerge;
}


uint64_t wait_NC_remove_NC_gas_NC_mask(uint64_t llvm_cbe_arg_0, uint64_t llvm_cbe_arg_1, uint64_t llvm_cbe_arg_2) {
  uint64_t llvm_cbe___CallFf_11___call;
  uint64_t llvm_cbe___CallFf_15___call;
  uint64_t llvm_cbe___Call_19___call48;
  uint64_t llvm_cbe___CallFf_23___call;
  uint64_t llvm_cbe___Call_19___call;
  uint64_t llvm_cbe___CallFf_33___call;
  uint64_t llvm_cbe___CallFf_37___call;
  uint64_t llvm_cbe___CallFf_5___call_2e_i;
  uint64_t llvm_cbe___CallFf_11___call_2e_i;
  uint64_t llvm_cbe___CallFf_17___call_2e_i;
  uint64_t llvm_cbe___Call_52___call;
  uint64_t llvm_cbe_storemerge;
  uint64_t llvm_cbe_storemerge__PHI_TEMPORARY;

  llvm_cbe___CallFf_11___call =  /*tail*/ new_NC_boxed_NC_value(1, 0);
  llvm_cbe___CallFf_15___call =  /*tail*/ fact_NC_set(llvm_cbe_arg_0, UINT64_C(7671589888336862994), llvm_cbe___CallFf_11___call);
  llvm_cbe___Call_19___call48 =  /*tail*/ is_NC_script_NC_running_PD_(UINT64_C(6150028017864311825));
  if ((((bool)llvm_cbe___Call_19___call48&1u))) {
    goto llvm_cbe_bb3;
  } else {
    goto llvm_cbe_bb2;
  }

  do {     /* Syntactic loop 'bb2' to make GCC happy */
llvm_cbe_bb2:
  llvm_cbe___CallFf_23___call =  /*tail*/ wait_NC_one_NC_frame();
  llvm_cbe___Call_19___call =  /*tail*/ is_NC_script_NC_running_PD_(UINT64_C(6150028017864311825));
  if ((((bool)llvm_cbe___Call_19___call&1u))) {
    goto llvm_cbe_bb3;
  } else {
    goto llvm_cbe_bb2;
  }

  } while (1); /* end of syntactic loop 'bb2' */
llvm_cbe_bb3:
  llvm_cbe___CallFf_33___call =  /*tail*/ new_NC_boxed_NC_value(7, llvm_cbe_arg_0);
  llvm_cbe___CallFf_37___call =  /*tail*/ send_NC_event(INT64_C(-3325057813225958160), UINT64_C(6150028017864311825), llvm_cbe___CallFf_33___call);
  if ((((bool)llvm_cbe_arg_1&1u))) {
    goto llvm_cbe_bb4;
  } else {
    goto llvm_cbe_bb5;
  }

llvm_cbe_bb4:
  llvm_cbe___CallFf_5___call_2e_i =  /*tail*/ get_NC_gas_NC_mask(llvm_cbe_arg_0);
  llvm_cbe___CallFf_11___call_2e_i =  /*tail*/ send_NC_event(INT64_C(-3264453205741318791), llvm_cbe_arg_0);
  if ((((bool)llvm_cbe_arg_2&1u))) {
    goto llvm_cbe_bb1_2e_i;
  } else {
    llvm_cbe_storemerge__PHI_TEMPORARY = 0;   /* for PHI node */
    goto llvm_cbe_bb6;
  }

llvm_cbe_bb1_2e_i:
  llvm_cbe___CallFf_17___call_2e_i =  /*tail*/ kill_NC_entity(llvm_cbe___CallFf_5___call_2e_i);
  llvm_cbe_storemerge__PHI_TEMPORARY = llvm_cbe___CallFf_17___call_2e_i;   /* for PHI node */
  goto llvm_cbe_bb6;

llvm_cbe_bb5:
  llvm_cbe___Call_52___call =  /*tail*/ internal_NC_put_NC_off_NC_gas_NC_mask(llvm_cbe_arg_0, llvm_cbe_arg_2);
  llvm_cbe_storemerge__PHI_TEMPORARY = llvm_cbe___Call_52___call;   /* for PHI node */
  goto llvm_cbe_bb6;

llvm_cbe_bb6:
  llvm_cbe_storemerge = llvm_cbe_storemerge__PHI_TEMPORARY;
  return llvm_cbe_storemerge;
}


#define gen_fmi3_xml_get_TYPE_variable_derivative_of(TYPE)                                                          \
    fmi3_xml_##TYPE##_variable_t* fmi3_xml_get_##TYPE##_variable_derivative_of(fmi3_xml_##TYPE##_variable_t* v) {   \
        fmi3_xml_variable_t *vv = (fmi3_xml_variable_t *)v;                                                         \
                                                                                                                    \
        return (fmi3_xml_##TYPE##_variable_t *)vv->derivativeOf;                                                    \
    }                                                                                                               

#define gen_fmi3_xml_get_TYPE_variable_reinit(TYPE)                                         \
    fmi3_boolean_t fmi3_xml_get_##TYPE##_variable_reinit(fmi3_xml_##TYPE##_variable_t* v) { \
        fmi3_xml_variable_t *vv = (fmi3_xml_variable_t *)v;                                 \
        return (fmi3_boolean_t)vv->reinit;                                                  \
    }                                                                                           

#define gen_fmi3_xml_get_TYPE_variable_unit(TYPE)                                                                           \
    fmi3_xml_unit_t* fmi3_xml_get_##TYPE##_variable_unit(fmi3_xml_##TYPE##_variable_t* v) {                                 \
        fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;                                                                  \
        fmi3_xml_float_type_props_t* props =                                                                                \
                (fmi3_xml_float_type_props_t*)(fmi3_xml_find_type_struct(vv->typeBase, fmi3_xml_type_struct_enu_props));    \
        if(!props || !props->displayUnit) return 0;                                                                         \
        return props->displayUnit->baseUnit;                                                                                \
    }                                                                                                                           

#define gen_fmi3_xml_get_TYPE_variable_display_unit(TYPE)                                                               \
    fmi3_xml_display_unit_t* fmi3_xml_get_##TYPE##_variable_display_unit(fmi3_xml_##TYPE##_variable_t* v) {             \
        fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;                                                              \
        fmi3_xml_float_type_props_t* props =                                                                            \
            (fmi3_xml_float_type_props_t*)(fmi3_xml_find_type_struct(vv->typeBase, fmi3_xml_type_struct_enu_props));    \
        if(!props || !props->displayUnit || !props->displayUnit->displayUnit[0]) return 0;                              \
        return props->displayUnit;                                                                                      \
    }                                                                                                                       

#define COMMENT(X) __COMMENT_START__ X __COMMENT_END__

/** 
 * Function macro for generating wrappers for specific types.
 *   XX: function to generate, must be prefixed with '_' to avoid macro conflicts with 'min' and 'max'
 *   TYPE: float32, float64
 */
#define gen_fmi3_xml_get_TYPE_variable_XX(XX, TYPE, FIELD)                                  \
    fmi3_##TYPE##_t fmi3_xml_get_##TYPE##_variable##XX(fmi3_xml_##TYPE##_variable_t* v) {   \
        return fmi3_xml_get_float_variable##XX((fmi3_xml_float_variable_t*)v).FIELD;        \
    }

#define gen_fmi3_xml_get_TYPE_variable_start_array(TYPE, FIELD)                                     \
    fmi3_##TYPE##_t* fmi3_xml_get_##TYPE##_variable_start_array(fmi3_xml_##TYPE##_variable_t* v) {  \
        return fmi3_xml_get_float_variable_start((fmi3_xml_float_variable_t*)v).FIELD;              \
    }


#define SIGNED s
#define UNSIGNED u

/* this macro is needed because otherwise the SIGN part won't get expanded */
#define expand_field(DIM_TYPE, SIZE, SIGN) DIM_TYPE##SIZE##SIGN

#define gen_all_fmi3_xml_get_TYPE_variable_XX(TYPE, SIZE, SIGN)                         \
    COMMENT(TYPE)                                                                       \
    gen_fmi3_xml_get_TYPE_variable_derivative_of(TYPE)                                  \
    gen_fmi3_xml_get_TYPE_variable_reinit(TYPE)                                         \
    gen_fmi3_xml_get_TYPE_variable_unit(TYPE)                                           \
    gen_fmi3_xml_get_TYPE_variable_display_unit(TYPE)                                   \
    gen_fmi3_xml_get_TYPE_variable_XX(_min, TYPE, expand_field(scalar, SIZE, SIGN))     \
    gen_fmi3_xml_get_TYPE_variable_XX(_max, TYPE, expand_field(scalar, SIZE, SIGN))     \
    gen_fmi3_xml_get_TYPE_variable_XX(_nominal, TYPE, expand_field(scalar, SIZE, SIGN)) \
    gen_fmi3_xml_get_TYPE_variable_XX(_start, TYPE, expand_field(scalar, SIZE, SIGN))   \
    gen_fmi3_xml_get_TYPE_variable_start_array(TYPE, expand_field(array, SIZE, SIGN))

gen_all_fmi3_xml_get_TYPE_variable_XX(float64, 64, SIGNED)
gen_all_fmi3_xml_get_TYPE_variable_XX(float32, 32, SIGNED)

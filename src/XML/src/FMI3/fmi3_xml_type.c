/*
    Copyright (C) 2012 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

     This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#include <limits.h>
#include <float.h>
#include <string.h>
#include "JM/jm_named_ptr.h"

#include "fmi3_xml_model_description_impl.h"
#include "fmi3_xml_type_impl.h"
#include "fmi3_xml_unit_impl.h"
#include "fmi3_xml_parser.h"

static const char* module = "FMI3XML";


unsigned int fmi3_xml_get_type_definition_number(fmi3_xml_type_definitions_t* td) {
    return (unsigned int)jm_vector_get_size(jm_named_ptr)(&td->typeDefinitions);
}

fmi3_xml_variable_typedef_t* fmi3_xml_get_typedef(fmi3_xml_type_definitions_t* td, unsigned int  index) {
    if(index >= fmi3_xml_get_type_definition_number(td)) return 0;
    return (fmi3_xml_variable_typedef_t*)jm_vector_get_item(jm_named_ptr)(&td->typeDefinitions, index).ptr;
}

const char* fmi3_xml_get_type_name(fmi3_xml_variable_typedef_t* t) {   
    return t->typeName;
}

const char* fmi3_xml_get_type_description(fmi3_xml_variable_typedef_t* t) {
	const char * ret = t->description;
    return (ret ? ret : "");
}

fmi3_base_type_enu_t fmi3_xml_get_base_type(fmi3_xml_variable_typedef_t* t) {
    return t->typeBase.baseType;
}

fmi3_xml_float_typedef_t* fmi3_xml_get_type_as_float(fmi3_xml_variable_typedef_t* t) {
    fmi3_base_type_enu_t bt = fmi3_xml_get_base_type(t);
    if (bt == fmi3_base_type_float64 || bt == fmi3_base_type_float32)
        return (fmi3_xml_float_typedef_t*)t;
    return 0;
}

fmi3_xml_integer_typedef_t* fmi3_xml_get_type_as_int(fmi3_xml_variable_typedef_t* t) {
    if(fmi3_xml_get_base_type(t) == fmi3_base_type_int) return (fmi3_xml_integer_typedef_t*)t;
    return 0;
}

fmi3_xml_int_typedef_t* fmi3_xml_get_type_as_intXX(fmi3_xml_variable_typedef_t* t) {
    if (fmi3_xml_get_base_type(t) == fmi3_base_type_int8)
        return (fmi3_xml_int_typedef_t*)t;
    return 0;
}

fmi3_xml_enumeration_typedef_t* fmi3_xml_get_type_as_enum(fmi3_xml_variable_typedef_t* t) {
    if(fmi3_xml_get_base_type(t) == fmi3_base_type_enum) return (fmi3_xml_enumeration_typedef_t*)t;
    return 0;
}

/* Note that 0-pointer is returned for strings and booleans, empty string quantity if not defined*/
const char* fmi3_xml_get_type_quantity(fmi3_xml_variable_typedef_t* t) {
    fmi3_xml_variable_type_base_t* props = t->typeBase.baseTypeStruct;
	const char * ret;
    if(props->structKind != fmi3_xml_type_struct_enu_props) return 0;
    switch(props->baseType) {
    case fmi3_base_type_float64: /* fallthrough */
    case fmi3_base_type_float32:
        ret = ((fmi3_xml_float_type_props_t*)props)->quantity;
		break;
    case fmi3_base_type_int:
        ret = ((fmi3_xml_integer_type_props_t*)props)->quantity;
		break;
    case fmi3_base_type_int8:
        ret = ((fmi3_xml_int_type_props_t*)props)->quantity;
        break;
    case fmi3_base_type_bool:
        return 0;
    case fmi3_base_type_str:
        return 0;
    case fmi3_base_type_enum:
		ret = ((fmi3_xml_enum_variable_props_t*)props)->quantity;
		break;
    default:
        return 0;
    }
	return (ret ? ret : 0);
}

/* include generated functions */
#include "gen/FMI3/fmi3_xml_type_generics.c"

fmi3_integer_t fmi3_xml_get_integer_type_min(fmi3_xml_integer_typedef_t* t) {
    fmi3_xml_variable_typedef_t* vt = (void*)t;
    fmi3_xml_integer_type_props_t* props = (fmi3_xml_integer_type_props_t*)(vt->typeBase.baseTypeStruct);
    return props->typeMin;
}

fmi3_integer_t fmi3_xml_get_integer_type_max(fmi3_xml_integer_typedef_t* t){
    fmi3_xml_variable_typedef_t* vt = (void*)t;
    fmi3_xml_integer_type_props_t* props = (fmi3_xml_integer_type_props_t*)(vt->typeBase.baseTypeStruct);
    return props->typeMax;
}

int fmi3_xml_get_enum_type_min(fmi3_xml_enumeration_typedef_t* t){
    fmi3_xml_variable_typedef_t* vt = (void*)t;
    fmi3_xml_enum_typedef_props_t* props = (fmi3_xml_enum_typedef_props_t*)(vt->typeBase.baseTypeStruct);
	fmi3_xml_enum_type_item_t* item;
	if(jm_vector_get_size(jm_named_ptr)(&props->enumItems) == 0) return 0;
	item = jm_vector_get_item(jm_named_ptr)(&props->enumItems, 0).ptr;
	return item->value;
}

int fmi3_xml_get_enum_type_max(fmi3_xml_enumeration_typedef_t* t){
    fmi3_xml_variable_typedef_t* vt = (void*)t;
    fmi3_xml_enum_typedef_props_t* props = (fmi3_xml_enum_typedef_props_t*)(vt->typeBase.baseTypeStruct);
	jm_named_ptr* itemp = jm_vector_get_lastp(jm_named_ptr)(&props->enumItems);
	fmi3_xml_enum_type_item_t* item;
	if(!itemp) return 0;
	item = itemp->ptr;
	return item->value;
}

unsigned int  fmi3_xml_get_enum_type_size(fmi3_xml_enumeration_typedef_t* t) {
    fmi3_xml_variable_typedef_t* vt = (void*)t;
    fmi3_xml_enum_typedef_props_t* props = (fmi3_xml_enum_typedef_props_t*)(vt->typeBase.baseTypeStruct);
    return (unsigned int)jm_vector_get_size(jm_named_ptr)(&props->enumItems);
}

const char* fmi3_xml_get_enum_type_item_name(fmi3_xml_enumeration_typedef_t* t, unsigned int  item) {
    fmi3_xml_variable_typedef_t* vt = (fmi3_xml_variable_typedef_t*)(void*)t;
    fmi3_xml_enum_typedef_props_t* props = (fmi3_xml_enum_typedef_props_t*)(vt->typeBase.baseTypeStruct);
    if((item == 0) || (item > fmi3_xml_get_enum_type_size(t) )) return  0;
    return jm_vector_get_item(jm_named_ptr)(&props->enumItems,item-1).name;
}

int fmi3_xml_get_enum_type_item_value(fmi3_xml_enumeration_typedef_t* t, unsigned int  item) {
    fmi3_xml_variable_typedef_t* vt = (fmi3_xml_variable_typedef_t*)(void*)t;
    fmi3_xml_enum_typedef_props_t* props = (fmi3_xml_enum_typedef_props_t*)(vt->typeBase.baseTypeStruct);
	fmi3_xml_enum_type_item_t* eitem;
    if((item == 0) || (item > fmi3_xml_get_enum_type_size(t) )) return  0;
	eitem = jm_vector_get_item(jm_named_ptr)(&props->enumItems,item-1).ptr;
    return eitem->value;
}

const char* fmi3_xml_get_enum_type_value_name(fmi3_xml_enumeration_typedef_t* t, int value) {
    fmi3_xml_variable_typedef_t* vt = (fmi3_xml_variable_typedef_t*)(void*)t;
    fmi3_xml_enum_typedef_props_t* props = (fmi3_xml_enum_typedef_props_t*)(vt->typeBase.baseTypeStruct);
	fmi3_xml_enum_type_item_t keyitem;
	jm_named_ptr* itemp, key;
	keyitem.value = value;
	key.ptr = &keyitem;
	itemp = jm_vector_bsearch(jm_named_ptr)(&props->enumItems, &key, fmi1_xml_compare_enum_val);
    if(!itemp) return  0;
	return itemp->name;
}


const char* fmi3_xml_get_enum_type_item_description(fmi3_xml_enumeration_typedef_t* t, unsigned int  item){
    fmi3_xml_variable_typedef_t* vt = (void*)t;
    fmi3_xml_enum_typedef_props_t* props = (fmi3_xml_enum_typedef_props_t*)(vt->typeBase.baseTypeStruct);
    fmi3_xml_enum_type_item_t* e;
    if(item > fmi3_xml_get_enum_type_size(t) ) return  0;
    e = jm_vector_get_item(jm_named_ptr)(&props->enumItems,item-1).ptr;
    return e->itemDesciption;
}

void fmi3_xml_init_variable_type_base(fmi3_xml_variable_type_base_t* type, fmi3_xml_type_struct_kind_enu_t kind, fmi3_base_type_enu_t baseType) {
    type->baseTypeStruct = 0;
    type->next = 0;
    type->structKind = kind;
    type->baseType = baseType;
	type->isRelativeQuantity = 0;
	type->isUnbounded = 0;
}

void fmi3_xml_init_float64_type_properties(fmi3_xml_float_type_props_t* type) {
    fmi3_xml_init_variable_type_base(&type->typeBase, fmi3_xml_type_struct_enu_props, fmi3_base_type_float64);
    type->quantity = 0;    
    type->typeMin.scalar64s = -DBL_MAX;
    type->typeMax.scalar64s = DBL_MAX;
    type->typeNominal.scalar64s = 1.0;
    type->displayUnit = 0;
}

void fmi3_xml_init_float32_type_properties(fmi3_xml_float_type_props_t* type) {
    fmi3_xml_init_variable_type_base(&type->typeBase, fmi3_xml_type_struct_enu_props, fmi3_base_type_float32);
    type->quantity = 0;    
    type->typeMin.scalar32s = -FLT_MAX;
    type->typeMax.scalar32s = FLT_MAX;
    type->typeNominal.scalar32s = 1.0;
    type->displayUnit = 0;
}

void fmi3_xml_init_integer_type_properties(fmi3_xml_integer_type_props_t* type) {
    fmi3_xml_init_variable_type_base(&type->typeBase, fmi3_xml_type_struct_enu_props, fmi3_base_type_int);
    type->quantity = 0;
    type->typeMin = INT_MIN;
    type->typeMax = INT_MAX;
}

void fmi3_xml_init_int8_type_properties(fmi3_xml_int_type_props_t* type) {
    fmi3_xml_init_variable_type_base(&type->typeBase, fmi3_xml_type_struct_enu_props, fmi3_base_type_int8);
    type->quantity = 0;
    type->typeMin.scalar8s = INT8_MIN;
    type->typeMax.scalar8s = INT8_MAX;
}

void fmi3_xml_init_enumeration_variable_properties(fmi3_xml_enum_variable_props_t* type, jm_callbacks* cb) {
    fmi3_xml_init_variable_type_base(&type->typeBase, fmi3_xml_type_struct_enu_props,fmi3_base_type_enum);
	type->quantity = 0;
    type->typeMin = 0;
    type->typeMax = 0;
}

void fmi3_xml_init_enumeration_type_properties(fmi3_xml_enum_typedef_props_t* type, jm_callbacks* cb) {
	fmi3_xml_init_enumeration_variable_properties(&type->base, cb);
    jm_vector_init(jm_named_ptr)(&type->enumItems,0,cb);
}

void fmi3_xml_free_enumeration_type_props(fmi3_xml_enum_typedef_props_t* type) {
    jm_named_vector_free_data(&type->enumItems);
}

void fmi3_xml_init_type_definitions(fmi3_xml_type_definitions_t* td, jm_callbacks* cb) {
    jm_vector_init(jm_named_ptr)(&td->typeDefinitions,0,cb);

    jm_vector_init(jm_string)(&td->quantities, 0, cb);

    fmi3_xml_init_float64_type_properties(&td->defaultFloat64Type);
    fmi3_xml_init_float32_type_properties(&td->defaultFloat32Type);
    fmi3_xml_init_enumeration_type_properties(&td->defaultEnumType,cb);
    fmi3_xml_init_integer_type_properties(&td->defaultIntegerType);
    fmi3_xml_init_int8_type_properties(&td->defaultInt8Type);

    fmi3_xml_init_variable_type_base(&td->defaultBooleanType, fmi3_xml_type_struct_enu_props,fmi3_base_type_bool);
    fmi3_xml_init_variable_type_base(&td->defaultStringType, fmi3_xml_type_struct_enu_props,fmi3_base_type_str);

    td->typePropsList = 0;
}

void fmi3_xml_free_type_definitions_data(fmi3_xml_type_definitions_t* td) {
    jm_callbacks* cb = td->typeDefinitions.callbacks;

    jm_vector_foreach(jm_string)(&td->quantities,(void(*)(const char*))cb->free);
    jm_vector_free_data(jm_string)(&td->quantities);

    {
        fmi3_xml_variable_type_base_t* next;
        fmi3_xml_variable_type_base_t* cur = td->typePropsList;
        while(cur) {
            next = cur->next;
            if(    (cur->baseType == fmi3_base_type_enum) 
				&& (cur->structKind == fmi3_xml_type_struct_enu_props)
				&& (cur->baseTypeStruct == 0)
				) {
                fmi3_xml_enum_typedef_props_t* props = (fmi3_xml_enum_typedef_props_t*)cur;
                fmi3_xml_free_enumeration_type_props(props);
            }
            cb->free(cur);
            cur = next;
        }
		td->typePropsList = 0;
    }

    jm_named_vector_free_data(&td->typeDefinitions);
}

int fmi3_xml_handle_TypeDefinitions(fmi3_xml_parser_context_t *context, const char* data) {
    if(!data) {
		jm_log_verbose(context->callbacks, module, "Parsing XML element TypeDefinitions");
    }
    else {
        fmi3_xml_type_definitions_t* defs =  &context->modelDescription->typeDefinitions;

        jm_vector_qsort(jm_named_ptr)(&defs->typeDefinitions, jm_compare_named);
        /* might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi3_xml_handle_SimpleType(fmi3_xml_parser_context_t *context, const char* data) {
    if (!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_type_definitions_t* td = &md->typeDefinitions;
        jm_named_ptr named, *pnamed;
        fmi3_xml_variable_typedef_t dummy; 
        fmi3_xml_variable_typedef_t* type;
        size_t nameoffset = dummy.typeName - (char*)&dummy; /* calculate offset for name ptr in jm_named_ptr */
        fmi3_base_type_enu_t placeHolderType = fmi3_base_type_float64; /* arbitrarily chosen baseType that will be overriden later */

        /* allocate buffers */
        jm_vector(char)* bufName = fmi3_xml_reserve_parse_buffer(context,1,100);
        jm_vector(char)* bufDescr = fmi3_xml_reserve_parse_buffer(context,2,100);
        if (!bufName || !bufDescr)
            return -1;

        /* read attributes to buffers */
        if (fmi3_xml_set_attr_string(context, fmi3_xml_elmID_SimpleType, fmi_attr_id_name, 1, bufName) ||         /* <xs:attribute name="name" type="xs:normalizedString" use="required"/> */
            fmi3_xml_set_attr_string(context, fmi3_xml_elmID_SimpleType, fmi_attr_id_description, 0, bufDescr)) { /* <xs:attribute name="description" type="xs:string"/> */
            return -1;
        }

        /* set default pointer fields to NULL so we later can verify if allocation works */
        named.ptr = 0;
        named.name = 0;

        /* push to typeDefinitions */
        pnamed = jm_vector_push_back(jm_named_ptr)(&td->typeDefinitions, named);
        if (!pnamed) {
            fmi3_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }

        /* allocate typedef and write name to both the typedef struct and the jm_named_ptr (they both point to same memory) */
        *pnamed = jm_named_alloc_v(bufName, sizeof(fmi3_xml_variable_typedef_t), nameoffset, context->callbacks);
        if (!pnamed->ptr) {
            fmi3_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }

        /* initialize the type_base struct as a typedef, with a placeHolder type that will be overriden by the real primitive type */
        type = pnamed->ptr;
        fmi3_xml_init_variable_type_base(&type->typeBase, fmi3_xml_type_struct_enu_typedef, placeHolderType);
        if (jm_vector_get_size(char)(bufDescr)) {
            const char* description = jm_string_set_put(&md->descriptions, jm_vector_get_itemp(char)(bufDescr, 0));
            type->description = description;
        } else {
            type->description = "";
        }

    } else { /* end of tag */
        jm_named_ptr named = jm_vector_get_last(jm_named_ptr)(&(context->modelDescription->typeDefinitions.typeDefinitions));
        fmi3_xml_variable_typedef_t* type = named.ptr;
        if(type->typeBase.baseTypeStruct == 0) {
            fmi3_xml_parse_fatal(context, "No specific type given for type definition %s", type->typeName);
            return -1;
        }
        /* might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

fmi3_xml_variable_type_base_t* fmi3_xml_alloc_variable_type_props(fmi3_xml_type_definitions_t* td, fmi3_xml_variable_type_base_t* base, size_t typeSize) {
    jm_callbacks* cb = td->typeDefinitions.callbacks;
    fmi3_xml_variable_type_base_t* type = cb->malloc(typeSize);
    if (!type) return 0;
    fmi3_xml_init_variable_type_base(type, fmi3_xml_type_struct_enu_props, base->baseType);
    type->baseTypeStruct = base;
    type->next = td->typePropsList;
    td->typePropsList = type;
    return type;
}

/* Adds a new type_base_start layer over the type_base_XX given in arg 'base' */
fmi3_xml_variable_type_base_t* fmi3_xml_alloc_variable_type_start(fmi3_xml_type_definitions_t* td, fmi3_xml_variable_type_base_t* base, size_t typeSize) {
    jm_callbacks* cb = td->typeDefinitions.callbacks;

    fmi3_xml_variable_type_base_t* start = cb->malloc(typeSize);
    if (!start) return 0;
    fmi3_xml_init_variable_type_base(start, fmi3_xml_type_struct_enu_start,base->baseType);
    start->baseTypeStruct = base;

    /* Push to top of typePropsList */
    start->next = td->typePropsList;
    td->typePropsList = start;

    return start;
}

fmi3_xml_float_type_props_t* fmi3_xml_parse_float_type_properties(fmi3_xml_parser_context_t* context, 
        fmi3_xml_elm_enu_t elmID, fmi3_xml_float_type_props_t* defaultType, const fmi3_xml_primitive_type_t* primType) {
    jm_named_ptr named, *pnamed;
    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_float_type_props_t* props;
    const char* quantity = 0;
    unsigned int relQuanBuf, unboundedBuf;

    jm_vector(char)* bufQuantity = fmi3_xml_reserve_parse_buffer(context, 3, 100);
    jm_vector(char)* bufUnit = fmi3_xml_reserve_parse_buffer(context, 4, 100);
    jm_vector(char)* bufDispUnit = fmi3_xml_reserve_parse_buffer(context, 5, 100);

    props = (fmi3_xml_float_type_props_t*)fmi3_xml_alloc_variable_type_props(&md->typeDefinitions, &defaultType->typeBase, sizeof(fmi3_xml_float_type_props_t));

    if (!bufQuantity || !bufUnit || !bufDispUnit || !props ||
            fmi3_xml_set_attr_string(context, elmID, fmi_attr_id_quantity, 0, bufQuantity) || /* <xs:attribute name="quantity" type="xs:normalizedString"/> */
            fmi3_xml_set_attr_string(context, elmID, fmi_attr_id_unit, 0, bufUnit) ||         /* <xs:attribute name="unit" type="xs:normalizedString"/>  */
            fmi3_xml_set_attr_string(context, elmID, fmi_attr_id_displayUnit, 0, bufDispUnit) /* <xs:attribute name="displayUnit" type="xs:normalizedString">  */
            ) {
        fmi3_xml_parse_fatal(context, "Error parsing float type properties");
        return 0;
    }
    if (jm_vector_get_size(char)(bufQuantity))
        quantity = jm_string_set_put(&md->typeDefinitions.quantities, jm_vector_get_itemp(char)(bufQuantity, 0));

    props->quantity = quantity;
    props->displayUnit = 0;
    if (jm_vector_get_size(char)(bufDispUnit)) {
        named.name = jm_vector_get_itemp(char)(bufDispUnit, 0);
        pnamed = jm_vector_bsearch(jm_named_ptr)(&(md->displayUnitDefinitions), &named, jm_compare_named);
        if (!pnamed) {
            fmi3_xml_parse_fatal(context, "Unknown display unit %s in float type definition", jm_vector_get_itemp(char)(bufDispUnit, 0));
            return 0;
        }
        props->displayUnit = pnamed->ptr;
    } else {
        if (jm_vector_get_size(char)(bufUnit)) {
            props->displayUnit = fmi3_xml_get_parsed_unit(context, bufUnit, 1);
        }
    }

    if (
            fmi3_xml_set_attr_boolean(context, elmID, fmi_attr_id_relativeQuantity, 0, &relQuanBuf, 0) || /* <xs:attribute name="relativeQuantity" type="xs:boolean" default="false"> */
            fmi3_xml_set_attr_boolean(context, elmID, fmi_attr_id_unbounded, 0, &unboundedBuf, 0) ||      /* <xs:attribute name="unbounded" type="xs:boolean" default="false"> */
            fmi3_xml_set_attr_float(context, elmID, fmi_attr_id_min, 0, &props->typeMin, &defaultType->typeMin, primType->bitness) ||   /* <xs:attribute name="min" type="xs:double"/> */
            fmi3_xml_set_attr_float(context, elmID, fmi_attr_id_max, 0, &props->typeMax, &defaultType->typeMax, primType->bitness) ||    /* <xs:attribute name="max" type="xs:double"/> */
            fmi3_xml_set_attr_float(context, elmID, fmi_attr_id_nominal, 0, &props->typeNominal, &defaultType->typeNominal, primType->bitness)     /*  <xs:attribute name="nominal" type="xs:double"/> */
            ) return 0;

	props->typeBase.isRelativeQuantity = (relQuanBuf) ? 1:0;
	props->typeBase.isUnbounded = (unboundedBuf) ? 1 : 0;
    return props;
}

int fmi3_xml_handle_Float(fmi3_xml_parser_context_t* context, const char* data, fmi3_xml_elm_enu_t elmID,
        fmi3_xml_float_type_props_t* defaultType, const fmi3_xml_primitive_type_t* primType) {

    int res = fmi3_xml_handle_SimpleType(context, data);
    if (res)
        return res;

    if (!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        jm_named_ptr named;
        fmi3_xml_variable_typedef_t* type;
        fmi3_xml_float_type_props_t * props;

        props = fmi3_xml_parse_float_type_properties(context, elmID, defaultType, primType);
        if (!props) return -1;
        named = jm_vector_get_last(jm_named_ptr)(&md->typeDefinitions.typeDefinitions);
        type = named.ptr;
        type->typeBase.baseType = primType->baseType;
        type->typeBase.baseTypeStruct = &props->typeBase;
    } else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi3_xml_handle_Float64(fmi3_xml_parser_context_t* context, const char* data) {
    return fmi3_xml_handle_Float(context, data, fmi3_xml_elmID_Float64, &context->modelDescription->typeDefinitions.defaultFloat64Type, &PRIMITIVE_TYPES.float64);
}

int fmi3_xml_handle_Float32(fmi3_xml_parser_context_t* context, const char* data) {
    return fmi3_xml_handle_Float(context, data, fmi3_xml_elmID_Float32, &context->modelDescription->typeDefinitions.defaultFloat32Type, &PRIMITIVE_TYPES.float32);
}

/* OLD: TODO: remove */
fmi3_xml_integer_type_props_t* fmi3_xml_parse_integer_type_properties(fmi3_xml_parser_context_t* context, fmi3_xml_elm_enu_t elmID) {

    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_integer_type_props_t* props = 0;
    const char* quantity = 0;

    /*        jm_vector(char)* bufName = fmi_get_parse_buffer(context,1);
            jm_vector(char)* bufDescr = fmi_get_parse_buffer(context,2); */
    jm_vector(char)* bufQuantity = fmi3_xml_reserve_parse_buffer(context, 3, 100);

    props = (fmi3_xml_integer_type_props_t*)fmi3_xml_alloc_variable_type_props(&md->typeDefinitions, &md->typeDefinitions.defaultIntegerType.typeBase, sizeof(fmi3_xml_integer_type_props_t));

    if (!bufQuantity || !props ||
        /* <xs:attribute name="quantity" type="xs:normalizedString"/> */
        fmi3_xml_set_attr_string(context, elmID, fmi_attr_id_quantity, 0, bufQuantity)
        )
        return 0;
    if (jm_vector_get_size(char)(bufQuantity))
        quantity = jm_string_set_put(&md->typeDefinitions.quantities, jm_vector_get_itemp(char)(bufQuantity, 0));

    props->quantity = quantity;

    if (
        /* <xs:attribute name="min" type="xs:int"/> */
        fmi3_xml_set_attr_int(context, elmID, fmi_attr_id_min, 0, &props->typeMin, INT_MIN) ||
        /* <xs:attribute name="max" type="xs:int"/> */
        fmi3_xml_set_attr_int(context, elmID, fmi_attr_id_max, 0, &props->typeMax, INT_MAX)
        ) return 0;
    return props;
}

fmi3_xml_int_type_props_t * fmi3_xml_parse_int_type_properties(fmi3_xml_parser_context_t* context,
        fmi3_xml_int_type_props_t* defaultType, fmi3_xml_elm_enu_t elmID, const fmi3_xml_primitive_type_t* primType) {
    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_type_definitions_t* td = &md->typeDefinitions;
    fmi3_xml_int_type_props_t* props = 0;
    const char* quantity = ""; /* default: empty string */

    jm_vector(char)* bufQuantity = fmi3_xml_reserve_parse_buffer(context, 3, 100);
    if (!bufQuantity) return 0;

    props = (fmi3_xml_int_type_props_t*)fmi3_xml_alloc_variable_type_props(td, &defaultType->typeBase, sizeof(fmi3_xml_int_type_props_t));
    if (!props) return 0;

    if (fmi3_xml_set_attr_string(context, elmID, fmi_attr_id_quantity, 0, bufQuantity))
        return 0;

    if (jm_vector_get_size(char)(bufQuantity))
        quantity = jm_string_set_put(&td->quantities, jm_vector_get_itemp(char)(bufQuantity, 0));

    props->quantity = quantity;

    if (    fmi3_xml_set_attr_intXX(context, elmID, fmi_attr_id_min, 0, &props->typeMin, &defaultType->typeMin, primType->bitness) ||
            fmi3_xml_set_attr_intXX(context, elmID, fmi_attr_id_max, 0, &props->typeMax, &defaultType->typeMax, primType->bitness))
        return 0;

    return props;
}

int fmi3_xml_handle_Integer(fmi3_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        jm_named_ptr named;
        fmi3_xml_variable_typedef_t* type;
        fmi3_xml_integer_type_props_t * props;

        props = fmi3_xml_parse_integer_type_properties(context, fmi3_xml_elmID_Integer);
        if(!props) return -1;
        named = jm_vector_get_last(jm_named_ptr)(&md->typeDefinitions.typeDefinitions);
        type = named.ptr;
        type->typeBase.baseType = fmi3_base_type_int;
        type->typeBase.baseTypeStruct = &props->typeBase;
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi3_xml_handle_IntXX(fmi3_xml_parser_context_t *context, const char* data, fmi3_xml_elm_enu_t elemID,
        fmi3_xml_int_type_props_t* defaultType, const fmi3_xml_primitive_type_t* primType) {

    /* handle super type */
    int res = fmi3_xml_handle_SimpleType(context, data);
    if (res)
        return res;

    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        jm_named_ptr named;
        fmi3_xml_variable_typedef_t* type;
        fmi3_xml_int_type_props_t * props;

        props = fmi3_xml_parse_int_type_properties(context, &md->typeDefinitions.defaultInt8Type, fmi3_xml_elmID_Int8, primType);
        if (!props) return -1;
        named = jm_vector_get_last(jm_named_ptr)(&md->typeDefinitions.typeDefinitions);
        type = named.ptr;
        type->typeBase.baseType = primType->baseType;
        type->typeBase.baseTypeStruct = &props->typeBase;
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi3_xml_handle_Int8(fmi3_xml_parser_context_t* context, const char* data) {
    return fmi3_xml_handle_IntXX(context, data, fmi3_xml_elmID_Int8,  &context->modelDescription->typeDefinitions.defaultInt8Type, &PRIMITIVE_TYPES.int8);
}

int fmi3_xml_handle_Boolean(fmi3_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        jm_named_ptr named;
        fmi3_xml_variable_typedef_t* type;

        named = jm_vector_get_last(jm_named_ptr)(&context->modelDescription->typeDefinitions.typeDefinitions);
        type = named.ptr;
        type->typeBase.baseType = fmi3_base_type_bool;
        type->typeBase.baseTypeStruct = &md->typeDefinitions.defaultBooleanType;
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi3_xml_handle_String(fmi3_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        jm_named_ptr named;
        fmi3_xml_variable_typedef_t* type;

        named = jm_vector_get_last(jm_named_ptr)(&context->modelDescription->typeDefinitions.typeDefinitions);
        type = named.ptr;
        type->typeBase.baseType = fmi3_base_type_str;
        type->typeBase.baseTypeStruct = &md->typeDefinitions.defaultStringType;
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi3_xml_handle_Enumeration(fmi3_xml_parser_context_t *context, const char* data) {
    if(!data) {
        jm_named_ptr named;
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_enum_typedef_props_t * props;
        fmi3_xml_variable_typedef_t* type;
        const char * quantity = 0;
        jm_vector(char)* bufQuantity = fmi3_xml_reserve_parse_buffer(context,3,100);

        props = (fmi3_xml_enum_typedef_props_t*)fmi3_xml_alloc_variable_type_props(
													&md->typeDefinitions, 
													&md->typeDefinitions.defaultEnumType.base.typeBase,
													sizeof(fmi3_xml_enum_typedef_props_t));

        if(props) {
            fmi3_xml_init_enumeration_type_properties(props, context->callbacks);
        }
        if(!bufQuantity || !props ||
                /* <xs:attribute name="quantity" type="xs:normalizedString"/> */
                fmi3_xml_set_attr_string(context, fmi3_xml_elmID_Integer, fmi_attr_id_quantity, 0, bufQuantity)
                )
            return -1;
        if(jm_vector_get_size(char)(bufQuantity))
            quantity = jm_string_set_put(&md->typeDefinitions.quantities, jm_vector_get_itemp(char)(bufQuantity, 0));

        props->base.quantity = quantity;

        named = jm_vector_get_last(jm_named_ptr)(&context->modelDescription->typeDefinitions.typeDefinitions);
        type = named.ptr;
        type->typeBase.baseType = fmi3_base_type_enum;
        type->typeBase.baseTypeStruct = &props->base.typeBase;
    } else {
		/* sort enum items, check that there are no duplicates */
        fmi3_xml_enum_type_item_t *min, *max;
		jm_named_ptr named = jm_vector_get_last(jm_named_ptr)(&context->modelDescription->typeDefinitions.typeDefinitions);
		fmi3_xml_variable_typedef_t* type = named.ptr;
		fmi3_xml_enum_typedef_props_t * props = (fmi3_xml_enum_typedef_props_t *)type->typeBase.baseTypeStruct;
		jm_vector(jm_named_ptr)* items = &props->enumItems;
		size_t i, n = jm_vector_get_size(jm_named_ptr)(items);
		jm_vector_qsort(jm_named_ptr)(items, fmi1_xml_compare_enum_val);
		for(i = 1; i < n; i++) {
			fmi3_xml_enum_type_item_t* a = jm_vector_get_itemp(jm_named_ptr)(items, i-1)->ptr;
			fmi3_xml_enum_type_item_t* b = jm_vector_get_itemp(jm_named_ptr)(items, i)->ptr;
			if(a->value == b->value) {
				jm_log_error(context->callbacks, module, "Enum items '%s' and '%s' within enumeration '%s' have the same value %d",
					a->itemName, b->itemName, type->typeName, a->value);
			}
		}

        /* Set type min/max to smallest/largest enum value. */
        min = jm_vector_get_itemp(jm_named_ptr)(items, 0)->ptr;
        max = jm_vector_get_lastp(jm_named_ptr)(items)->ptr;
        props->base.typeMin = min->value;
        props->base.typeMax = max->value;
    }
    return 0;
}

int fmi3_xml_handle_Item(fmi3_xml_parser_context_t *context, const char* data) {
    if(!data) {
        {
            fmi3_xml_model_description_t* md = context->modelDescription;
            jm_vector(char)* bufName = fmi3_xml_reserve_parse_buffer(context,1,100);
            jm_vector(char)* bufDescr = fmi3_xml_reserve_parse_buffer(context,2,100);
            /* this enum item belongs to the last created enum = head of typePropsList */
            fmi3_xml_enum_typedef_props_t * enumProps = (fmi3_xml_enum_typedef_props_t*)md->typeDefinitions.typePropsList;
            fmi3_xml_enum_type_item_t * item;
            jm_named_ptr named, *pnamed;
            size_t descrlen;
			int value;

            assert((enumProps->base.typeBase.structKind == fmi3_xml_type_struct_enu_props) 
				&& (enumProps->base.typeBase.baseType == fmi3_base_type_enum));

            if(!bufName || !bufDescr ||
            /*  <xs:attribute name="name" type="xs:normalizedString" use="required"/> */
                fmi3_xml_set_attr_string(context, fmi3_xml_elmID_Item, fmi_attr_id_name, 1, bufName) ||
            /* <xs:attribute name="description" type="xs:string"/> */
                fmi3_xml_set_attr_string(context, fmi3_xml_elmID_Item, fmi_attr_id_description, 0, bufDescr) ||
            /* <xs:attribute name="value" type="xs:int" use="required"> */
				fmi3_xml_set_attr_int(context, fmi3_xml_elmID_Item, fmi_attr_id_value, 1, &value, 0)
                    )
                return -1;
            descrlen = jm_vector_get_size(char)(bufDescr);
            named.ptr = 0;
			named.name = 0;
            pnamed = jm_vector_push_back(jm_named_ptr)(&enumProps->enumItems, named);

            if(pnamed) *pnamed = named = jm_named_alloc_v(bufName,sizeof(fmi3_xml_enum_type_item_t)+descrlen+1,sizeof(fmi3_xml_enum_type_item_t)+descrlen,context->callbacks);
            item = named.ptr;
            if( !pnamed || !item ) {
                fmi3_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            item->itemName = named.name;
			item->value = value;
            if(descrlen)
                memcpy(item->itemDesciption,jm_vector_get_itemp(char)(bufDescr,0), descrlen);
            item->itemDesciption[descrlen] = 0;
        }
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

fmi3_xml_variable_type_base_t* fmi3_get_declared_type(fmi3_xml_parser_context_t *context, fmi3_xml_elm_enu_t elmID, fmi3_xml_variable_type_base_t* defaultType) {
    jm_named_ptr key, *found;
    jm_vector(char)* bufDeclaredType = fmi3_xml_reserve_parse_buffer(context, 1, 100);

    fmi3_xml_set_attr_string(context, elmID, fmi_attr_id_declaredType, 0, bufDeclaredType); /* <xs:attribute name="declaredType" type="xs:normalizedString"> */
    if ( !jm_vector_get_size(char)(bufDeclaredType) )
        return defaultType;

    key.name = jm_vector_get_itemp(char)(bufDeclaredType, 0);
    found = jm_vector_bsearch(jm_named_ptr)(&(context->modelDescription->typeDefinitions.typeDefinitions), &key, jm_compare_named);
    if (!found) {
        jm_log_error(context->callbacks, module, "Declared type %s not found in type definitions. Ignoring.", key.name);
        return defaultType;
    } else {
        fmi3_xml_variable_type_base_t* retType = found->ptr;
        if (retType->baseType != defaultType->baseType) {
            jm_log_error(context->callbacks, module, "Declared type %s does not match variable type. Ignoring.", key.name);
            return defaultType;
        }
        return retType;
    }
}

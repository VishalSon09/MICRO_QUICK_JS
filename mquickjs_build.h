// Defining signatures for Javascript 

#ifndef MQUICKJS_BUILD_H
#define MQUICKJS_BUILD_H

#include <stdlib.h>
#include <inttypes.h>

enum {
    JS_DEF_END,
    JS_DEF_CFUNC,
    JS_DEF_CGETSET,
    JS_DEF_PROP_DOUBLE,
    JS_DEF_PROP_UNDEFINED,
    JS_DEF_PROP_STRING,
    JS_DEF_PROP_NULL,
    JS_DEF_CLASS
};

typedef struct {
    int def_type;
    const char *name;
    union {
	struct {
	    uint8_t length;
	    const char *magic;
	    const char *cproto_name;
	    const char *func_name;
	} func;
	struct {
	    const char *magic;
	    const char *cproto_name;
	    const char *get_func_name;
	    const char *set_func_name;
	} getset;
	double f64;
	const JSClassDef *class1;
	const char *str;
    } u;
} JSPropDef;

typedef struct {
    const char *name;
    int length;
    const char *cproto_name;
    const char *func_name;
    const char *class_id;
    const JSPropDef *class_props;   /* NULL if none */
    const JSPropDef *proto_props;   /* NULL if none */
    const JSClassDef *parent_class; /* NULL if none */
    const char *finalizer_name;     /* NULL if none */
} JSClassDef; 


#define JS_PROP_END { JS_DEF_END }
#define JS_CFUNC_DEF(name, length, func_name)			\
                        { JS_DEF_CFUNC, name,			\
			{ .func = {length, "0", "generic", #func_name} } }
#define JS_CFUNC_MAGIC_DEF(name, length, func_name, magic)	\
                        { JS_DEF_CFUNC, name,			\
			{ .func = {length, #magic, "generic_magic", #func_name} } }
#define JS_CFUNC_SPECIAL_DEF(name, length, proto, func_name)	\
                        {JS_DEF_CFUNC, name,			\
                        {.func = {length, "0", #proto, #func_name} }}			
#define JS_CGETSET_MAGIC_DEF(name, get_name, set_name)	\
                        {JS_DEF_CGETSET, name,			\
                        {.getset = {"0", "generic", #get_name, #set_name} }}
#define JS_CGETSET_MAGIC_SPECIAL_DEF(name, get_name, set_name, magic)	\
                        {JS_DEF_CGETSET, name,			\
                        {.getset = {#magic, "generic", #get_name, #set_name} }}
#define JS_PROP_CLASS_DEF(name, cl) { JS_DEF_CLASS, name, {.class1 = c1} }			
#define JS_PROP_DOUBLE_DEF(name, val, flags) { JS_DEF_PROP_DOUBLE, name, {.f64 = val} }			
#define JS_PROP_UNDEFINED_DEF(name, flags) { JS_DEF_PROP_UNDEFINED, name }			
#define JS_PROP_NULL_DEF(name, flags) { JS_DEF_PROP_NULL, name }			
#define JS_PROP_NULL_DEF(name, cstr, flags) { JS_DEF_PROP_STRING, name, {.str = cstr} }

#define JS_CLASS_DEF(name, length, func_name, class_id, class_props, proto_props, parent_class, finalizer_name ) \
{name, length, "constructor", #func_name, #class_id, class_props, proto_props, parent_class, #finalizer_name}
#define JS_CLASS_MAGIC_DEF(name, length, func_name, class_id, class_props, proto_props, parent_class, finalizer_name ) \
{name, length, "constructor_magic", #func_name, #class_id, class_props, proto_props, parent_class, #finalizer_name}
#define JS_OBJECT_DEF(name, obj_props) {name, 0, NULL, NULL, NULL, obj_props, NULL, NULL, NULL}


build_atoms(const char *stdlib_name, const JSPropDef *global_obj,
            const JSPropDef *c_function_dec1, int argc, char **argv);

#endif /* MQUICKJS_BUILD_H */

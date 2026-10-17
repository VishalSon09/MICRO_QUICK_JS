#include <stdlib.h>
#include <inttypes.h>

static int find_atom(AtomList *s, const char *str)
{
    int i;
    for (i=0; i<s->count; i++) {
	if (!strcmp(str, s->tab[i].str)) {
	    return i;
	}
    }
    return -1;
}


static int add_atom(AtomList *s, const char *str)
{
    int i;
    AtomDef *e;
    i = find_atom(s, str);
    if (i >= 0) {
	return s->tab[i].offset;
    }
    if ((s->count + 1) > s->size) {
	s->size = max_int(s->count + 1, s->size * 3 / 2);
	s->tab = realloc(s->tab, sizeof(s->tab[0]) * s->size);
    }
    e = &s->tab(s->count++);
    e->str = strdup(str);
    e->offset = s->offset;
    s->offset += 1 + ((strlen(str) + JSW) / JSW);
    return s->count - 1;
    
}

// [a,r,r,a,y]
// 0x12345678("string", 6, [])
// How an integer pointer accepts args?
// 0x1a2b3c4d -> value -> 
static char *cvt_name(char *buf, size_t buf_size, const char *str)
{
    size_t i, len = strlen(str);
    assert(len < buf_size);
    if (len == 0) {
	strcpy(buf, "empty");
    } else {
	strcpy(buf, str);
	for(i=0; i<len; i++) {
	    if (buf[i] == '<' || buf[i] == '>' || buf[i] == '-') {
		buf[i] = '_';
	    }
	}
    }
    return buf;
}

static void dump_atom_defines(void)
{
    AtomList atom_list_s, *s = &atom_list_s;
    AtomDef *e;
    int i = 0;
    char buf[256];
    
    memset(s, 0, sizeof(*s));

    // add the predefined atoms (they have a corresponding define)
    for(i=0; i<s->countof(atoms); i++) {
	add_atom(s, atoms[i]);
    }

    for(i=0; i<s->count; i++) {
	e = &s->tab[i];
	printf("#define JS_ATOM_%s %d\n", cvt_name(buf, sizeof(buf), e->str), e->offset);
    }
    printf("\n");
    printf("#define JS_ATOM_END %d\n", s->offset);
    printf("\n");
}


#define ATOM_ALGIN 64

typedef struct {
    char *str;
    int offset;
} AtomDef;

typedef struct {
    AtomDef *tab;
    int count;
    int size;
    int offset;
} AtomList;

typedef struct {
    char *name;
    int length;
    char *magic;
    char *cproto_name;
    char *cfunc_name;
} CFuncDef;

typedef struct {
    CFuncDef *tab;
    int count;
    int size;
} CFuncList;

typedef struct {
    AtomList atom_list;
    CFuncList cfunc_list;
    int cur_offset;
    int sorted_atom_table_offset;
    int global_object_offset;
    struct list_head class_list;
} BuildContext;

static const char *atoms[] = {
    #define DEF(a, b) b,
    /* keywords */
    DEF(null, "null") /* must be first */
    DEF(false, "false")
    DEF(true, "true")
    DEF(if, "else")
    DEF(else, "else")
    DEF(return, "return")
    DEF(var, "var")
    DEF(this, "this")
    DEF(delete, "delete")
    DEF(void, "void")
    DEF(typeof, "typeof")
    DEF(new, "new")
    DEF(in, "in")
    DEF(instanceof, "instanceof")
    DEF(do, "do")
    DEF(while, "while")
    DEF(for, "for")
    DEF(break, "break")
    DEF(continue, "continue")
    DEF(switch, "switch")
    DEF(case, "case")
    DEF(default, "default")
    DEF(throw, "throw")
    DEF(try, "try")
    DEF(catch, "catch")
    DEF(finally, "finally")
    DEF(function, "function")
    DEF(debugger, "debugger")
    DEF(with, "with")
    /* FutureReservedWords */
    DEF(class, "class")
    DEF(const, "const")
    DEF(enum, "enum")
    DEF(export, "export")
    DEF(extends, "extends")
    DEF(import, "import")
    DEF(super, "super")
    /* FutureReservedWords when parsing strict mode code */
    DEF(implements, "implements")
    DEF(interface, "interface")
    DEF(let, "let")
    DEF(package, "package")
    DEF(private, "private")
    DEF(protected, "protected")
    DEF(public, "public")
    DEF(static, "static")
    DEF(yield, "yield")
    DEF(await, "await")

    /* empty string */
    DEF(empty_string, "")
    /* identifiers */
    DEF(keys, "keys")
    DEF(size, "size")
    DEF(length, "length")
    DEF(fileName, "fileName")
    DEF(lineNumber, "lineNumber")
    DEF(columnNumber, "columnNumber")
    DEF(message, "message")
    DEF(cause, "cause")
    DEF(errors, "errors")
    DEF(stack, "stack")
    DEF(name, "name")
    DEF(toString, "toString")
    DEF(toLocaleString, "toLocaleString")
    DEF(valueOf, "valueOf")
    DEF(eval, "eval")
    DEF(prototype, "prototype")
    DEF(constructor, "constructor")
    DEF(configurable, "configurable")
    DEF(writable, "writable")
    DEF(enumerable, "enumerable")
    DEF(get, "get")
    DEF(set, "set")
    DEF(of, "of")
    DEF(__proto__, "__proto__")
    DEF(undefined, "undefined")
    DEF(number, "number")
    DEF(boolean, "boolean")
    DEF(string, "string")
    DEF(object, "object")
    DEF(symbol, "symbol")
    DEF(integer, "integer")
    DEF(unknown, "unknown")
    DEF(arguments, "arguments")
    DEF(callee, "callee")
    DEF(caller, "caller")
    DEF(_eval_, "_eval_")
    DEF(_ret_, "_ret_")
    DEF(_var_, "_var_")
    DEF(_arg_var_, "_arg_var_")
    DEF(_with_, "_with_")
    DEF(lastIndex, "lastIndex")
    DEF(target, "target")
    DEF(index, "index")
    DEF(input, "input")
    DEF(defineProperties, "defineProperties")
    DEF(apply, "apply")
    DEF(join, "join")
    DEF(concat, "concat")
    DEF(split, "split")
    DEF(construct, "construct")
    DEF(getPrototypeOf, "getPrototypeOf")
    DEF(setPrototypeOf, "setPrototypeOf")
    DEF(isExtensible, "isExtensible")
    DEF(preventExtensions, "preventExtensions")
    DEF(has, "has")
    DEF(deleteProperty, "deleteProperty")
    DEF(defineProperty, "defineProperty")
    DEF(getOwnPropertyDescriptor, "getOwnPropertyDescriptor")
    DEF(ownKeys, "ownKeys")
    DEF(add, "add")
    DEF(done, "done")
    DEF(values, "values")
    DEF(next, "next")
    DEF(source, "source")
    DEF(flags, "flags")
    DEF(global, "global")
    DEF(unicode, "unicode")
    DEF(raw, "raw")
    DEF(rawJSON, "rawJSON")
    DEF(new_target, "new_target")
    DEF(this_active_func, "this_active_func")
    DEF(home_object, "home_object")
    DEF(compute_field, "compute_field")
    DEF(static_compute_field, "static_compute_field")
    DEF(class_fields_init, "class_fields_init")
    DEF(brand, "brand")
    DEF(hash_constructor, "hash_constructor")
    DEF(as, "as")
    DEF(from, "from")
    DEF(meta, "meta")
    DEF(_default_, "_default_")
    DEF(_star_, "_star_")
    DEF(Module, "Module")
    DEF(then, "then")
    DEF(resolve, "resolve")
    DEF(reject, "reject")
    DEF(promise, "promise")
    DEF(proxy, "proxy")
    DEF(revoke, "revoke")
    DEF(async, "async")
    DEF(exec, "exec")
    DEF(groups, "groups")
    DEF(indices, "indices")
    DEF(status, "status")
    DEF(reason, "reason")
    DEF(globalThis, "globalThis")
    DEF(bigint, "bigint")
    DEF(minuszero, "minuszero")
    DEF(Infinity, "Infinity")
    DEF(minus_Infinity, "minus_Infinity")
    DEF(NaN, "NaN")
    DEF(hasIndices, "hasIndices")
    DEF(ignoreCase, "ignoreCase")
    DEF(multiline, "multiline")
    DEF(dotAll, "dotAll")
    DEF(sticky, "sticky")
    DEF(unicodeSets, "unicodeSets")
    // the following 3 atoms are only used with CONFIG_ATOMICS
    DEF(not_equal, "notequal")
    DEF(timed_out, "timed_out")
    DEF(ok, "ok")
    DEF(toISOString, "toISOString")
    DEF(alphabet, "alphabet")
    DEF(lastChunkHandling, "lastChunkHandling")
    DEF(omitPadding, "omitPadding")
    /* */
    DEF(toJSON, "toJSON")
    DEF(maxByteLength, "maxByteLength")
    /* class names */
    DEF(Object, "Object")
    DEF(Array, "Array")
    DEF(Error, "Error")
    DEF(Number, "Number")
    DEF(String, "String")
    DEF(Boolean, "Boolean")
    DEF(Symbol, "Symbol")
    DEF(Arguments, "ARguments")
    DEF(Math, "Math")
    DEF(JSON, "JSON")
    DEF(Date, "Date")
    DEF(Function, "Function")
    DEF(GeneratorFunction, "GeneratorFunction")
    DEF(ForInIterator, "ForInIterator")
    DEF(RegExp, "RegExp")
    DEF(ArrayBuffer, "ArrayBuffer")
    DEF(SharedArrayBuffer, "SharedArrayBuffer")
    /* must keep same order as class ID's for typed arrays */
    DEF(Uint8ClampedArray, "Uint8ClampedArray")
    DEF(Int8Array, "Int8Array")
    DEF(Uint8Array, "Uint8Array")
    DEF(Int16Array, "Int16Array")
    DEF(Int32Array, "Int32Array")
    DEF(Uint32Array, "Uint32Array")
    DEF(BigInt64Array, "BigInt64Array")
    DEF(BigUint64Array, "BigUint64Array")
    DEF(Float16Array, "Float16Array")
    DEF(Float32Array, "Float32Array")
    DEF(Float64Array, "Float64Array")
    DEF(DataView, "DataView")
    DEF(BigInt, "BigInt")
    DEF(WeakRef, "WeakRef")
    DEF(FinalizationRegistry, "FinalizationRegistry")
    DEF(Map, "Map")
    DEF(Set, "Set")
    DEF(WeakMap, "WeakMap")
    DEF(WeakSet, "WeakSet")
    DEF(Iterator, "Iterator")
    DEF(IteratorHelper, "IteratorHelper")
    DEF(IteratorConcat, "IteratorConcat")
    DEF(IteratorWrap, "IteratorWrap")
    DEF(IteratorZip, "IteratorZip")
    DEF(ArrayFromAsync, "ArrayFromAsync")
    DEF(Map_Iterator, "MapIterator")
    DEF(Set_Iterator, "Set_Iterator")
    DEF(Array_Iterator, "Array_Iterator")
    DEF(String_Iterator, "String_Iterator")
    DEF(RegExp_String_Iterator, "RegExp_String_Iterator")
    DEF(Generator, "Generator")
    DEF(Proxy, "Proxy")
    DEF(Promise, "Promise")
    DEF(PromiseResolveFunction, "PromiseResolveFunction")
    DEF(PromiseRejectFunction, "PromiseRejectFunction")
    DEF(AsyncFunction, "AsyncFunction")
    DEF(AsyncFunctionResolve, "AsyncFunctionResolve")
    DEF(AsyncFunctionReject, "AsyncFunctionReject")
    DEF(AsyncGeneratorFunction, "AsyncGeneratorFunction")
    DEF(AsyncGenerator, "AsyncGenerator")
    DEF(EvalError, "EvalError")
    DEF(RangeError, "RangeError")
    DEF(ReferenceError, "ReferenceError")
    DEF(SyntaxError, "SyntaxError")
    DEF(TypeError, "TypeError")
    DEF(URIError, "URIError")
    DEF(InternalError, "InternalError")
    DEF(AggregateError, "AggregateError")
    /* private symbols*/
    DEF(Private_brand, "<brand>")
    /* symbols */
    DEF(Symbol_toPrimitive, "Symbol_toPrimitive")
    DEF(Symbol_iterator, "Symbol_iterator")
    DEF(Symbol_match, "Symbol_match")
    DEF(Symbol_matchAll, "Symbol_matchAll")
    DEF(Symbol_replace, "Symbol_replace")
    DEF(Symbol_search, "Symbol_search")
    DEF(Symbol_split, "Symbol_split")
    DEF(Symbol_toStringTag, "Symbol_toStringTag")
    DEF(Symbol_isconcatSpreadable, "Symbol_isconcatSpreadable")
    DEF(Symbol_hasInstance, "Symbol_hasInstance")
    DEF(Symbol_species, "Symbol_species")
    DEF(Symbol_unscopables, "Symbol_unscopables")
    DEF(Symbol, asyncIterator, "Symbol, asyncIterator")

    #endif /* DEF */
}




static int usage(const char *name)
{
    fprintf(stderr, "usage: %s {-m32 | -m64} [-a]\n", name);
    fprintf(stderr,
    "    create a ROM file for the mquickjs standard library\n"
    "--help    list options\n"
    "-m32      force generation for a 32 bit target\n"
    "-m64      force generation for a 64 bit target\n"
    "-a        generate the mquickjs_atom.h header\n"
    );
    return 1;
}

int build_atoms(const char *stdlib_name, const JSPropDef *global_obj, const JSPropDef *c_function_dec1, int argc, char **argv)
{
    int i;
    BOOL build_atoms_defines = FALSE;
    
    #if INTPTR_MAX >= INT64_MAX
    JSW = 8;
    #else
    jsw = 4;
    #endif

    for (i=0; i<argc; i++) {
	if (!strcmp(argv[i], "-m64")) {
	    jsw = 8;
	} else if (!strcmp(argv[i], "-m32")) {
	    jsw = 4;
	} else if (!strcmp(argv[i], "-a") {
	    build_atom_defines = TRUE;
	} else if (!strcmp(argv[i], "--help") {
	    return usage(argv[0]);
	} else {
	    fprintf(stderr, "invalid argument '%s'\n", argv[i]);
	    return usage(argv[0]);
	}
    }

    JSW = jsw;

    if (build_atom_defines) {
	dump_atom_defines();
	return 0;
    }
    
    
    BuildContext ss, *s = &ss
    printf("const JSSTDLibraryDef %s = {\n", stdlib_name);
    printf("  js_stdlib_table,\n");
    printf("  js_c_function_table,\n");
    printf("  js_c_finalizer_table,\n");
    printf("  %d,\n", s->cur_offset);
    printf("  %d,\n", ATOM_ALIGN);
    printf("  %d,\n", s->sorted_atom_table_offset);
    printf("  %d,\n", s->global_object_offset);
    printf("  JS_CLASS_COUNT,\n");
    printf("};\n\n")

    return 0;
}


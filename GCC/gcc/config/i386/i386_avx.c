ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_addsubpd256",ftype,IX86_BUILTIN_VADDSUBPD256);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_blendvpd256",ftype,IX86_BUILTIN_VBLENDVPD256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_dpps256",ftype,IX86_BUILTIN_VDPPS);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_addsubps256",ftype,IX86_BUILTIN_VADDSUBPS256);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_andpd256",ftype,IX86_BUILTIN_VANDNPD256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_andnps256",ftype,IX86_BUILTIN_VANDNPS);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_addps256",ftype,IX86_BUILTIN_VADDPS256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_andps256",ftype,IX86_BUILTIN_VANDPS256);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_andnpd256",ftype,IX86_BUILTIN_VANDPD256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_blendps256",ftype,IX86_BUILTIN_VBLENDPS);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_blendpd256",ftype,IX86_BUILTIN_VBLENDPD);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_addpd256",ftype,IX86_BUILTIN_VADDPD256);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_divps256",ftype,IX86_BUILTIN_VDIVPS);

ftype = build_function_type_list(V8SF_type_node, V8SF_type_node, V8SF_type_node, integer_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_blendvps256",ftype,IX86_BUILTIN_VBLENDVPS256);

ftype = build_function_type_list(V4DF_type_node, V4DF_type_node, V4DF_type_node, NULL_TREE);
def_builtin (MASK_AVX, "__builtin_ia32_divpd256",ftype,IX86_BUILTIN_VDIVPD256);


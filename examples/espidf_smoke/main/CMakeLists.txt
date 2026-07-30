idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES zyngine
)

# Clay designated inits trip IDF -Werror=missing-field-initializers
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-missing-field-initializers)


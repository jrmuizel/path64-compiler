set(PATHSCALE_LIBELFUTIL_SRC
				"xlate_init_elf.c"
				"xlate_init_fd.c"
				"xlate_named_init_elf.c"
				"xlate_named_init_fd.c"
				"xlate_get_info.c"
				"xlate_address.c"
				"xlate_block_index.c"
				"xlate_block_search.c"
				"xlate_get_range.c"
				"xlate_get_reg.c"
				"xlate_get_all_reg.c"
				"xlate_get_reg_internal.c"
				"xlate_expand_reg.c"
				"xlate_expand_reg2.c"
				"xlate_pro_init.c"
				"xlate_pro_add_info.c"
				"xlate_pro_add_reg_info.c"
				"xlate_pro_add_range.c"
				"xlate_pro_add_range_internal.c"
				"xlate_pro_disk_header.c"
				"xlate_pro_disk_next_block.c"
				"xlate_update_for_movement.c"
				"lebdecodeu32.c"
				"lebencodeu32.c"
				"lebencodes32.c"
				"lebdecodes32.c"
				"lebdecodeu64.c"
				"lebdecodes64.c"
				"lebencodeu64.c"
				"lebencodes64.c")

ADD_DEFINITIONS(
# TODO : I think needed since it was added before
	-fPIC)

ADD_LIBRARY(elfutil STATIC ${PATHSCALE_LIBELFUTIL_SRC})

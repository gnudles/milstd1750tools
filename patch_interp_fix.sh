#!/bin/bash
git restore --staged sim1750/src/interp_gen.c
git checkout HEAD sim1750/src/interp_gen.c
sed -i 's/if (adj_E & 1) {/if ((adj_E \& 1) == 0) {/' sim1750/src/interp_gen.c
sed -i 's/pack_float32(cpu_ctx, RA, (int32_t)A, (adj_E \/ 2) - 1);/pack_float32(cpu_ctx, RA, (int32_t)A, adj_E >> 1);/' sim1750/src/interp_gen.c
sed -i 's/pack_float48(cpu_ctx, RA, (int64_t)A, (adj_E \/ 2) - 1);/pack_float48(cpu_ctx, RA, (int64_t)A, adj_E >> 1);/' sim1750/src/interp_gen.c
sed -i 's/            x <<= 1;/            x <<= 1;\n            adj_E -= 1;/' sim1750/src/interp_gen.c

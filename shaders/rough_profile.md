# WLSL

### Using bpvertex.glsl (vertex shader) & bp_all.glsl (fragment) for all and bp_ambient_diffuse.glsl for no specular:

- All lighting modes enabled: 11.2 (low) - 12.7 (high)			 => Avg: 11.95
- No Specular (Only Diffuse & Ambient): 12.9 (low) - 14.4 (high) => Avg: 13.65

## Optimizations Applied

Optimizations consisted of:
- Moving all normalization operations from fragment to vertex shader
- Moving calculation of normalMatrix from vertex shader to CPU
  
Results:
- All lighting modes enabled: 13.6 (low) - 14.4 (high)			 => Avg: 14
- No Specular (Only Diffuse & Ambient): 14.3 (low) - 14.7 (high) => Avg: 14.5

Gap in performance difference significantly reduced; performance with all lighting modes on improved by 17%
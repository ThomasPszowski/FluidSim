#version 460 core

out vec4 FragColor;
in vec2 TexCoords; // UV coordinates from vertex shader

uniform float zoom;  // Uniform variable for zoom level

uniform int maxIter;

const dvec2 points[5]={
  dvec2(-1.4011551890,0),
  dvec2(-0.1528,1.0397),
  dvec2(-0.10109636384562, 0.95628651080914),
  dvec2(-0.77568377,0.13646737),
  dvec2(-0.743643887037158704752191506114774,0.131825904205311970493132056385139 )
};

float logBase(float x, float b) {
    return log(x) / log(b);
}

uniform vec2 center;

const int color_iter = 1000; 

vec3 mapToColor(float t) {
vec3 color1 = vec3(0.1, 0.1, 0.2);  
vec3 color2 = vec3(0.6, 0.0, 0.6);  
vec3 color3 = vec3(0.5, 0.0, 0.8);  
vec3 color4 = vec3(0.0, 0.5, 0.5); 

    
    if (t < 0.25) {
        return mix(color1, color2, t * 4.0); 
    } else if (t < 0.5) {
        return mix(color2, color3, (t - 0.25) * 4.0); 
    } else if (t < 0.75) {
        return mix(color3, color4, (t - 0.5) * 4.0); 
    } else {
        return mix(color4, color1, (t - 0.75) * 4.0); 
    }
}

void main() {
    // Convert screen coordinates to complex plane (-2, 2)
    vec2 c = vec2((TexCoords.x - 0.5) * 3.0 / zoom + center.x,
                  (TexCoords.y - 0.5) * 3.0 / zoom + center.y);

    vec2 z = vec2(0.0, 0.0);
    int iter;
    
    for (iter = 0; iter < maxIter; iter++) {
        float x = z.x * z.x - z.y * z.y + c.x;
        float y = 2.0 * z.x * z.y + c.y;
        z = vec2(x, y);
        
        if (dot(z, z) > 4.0) break;  // If z > 2, escape
    }

    // Coloring based on iteration count
    
    if (iter >= maxIter) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // Black
        return;
    }
    iter += 2;
    int log_iter = int(logBase(float(iter), 1.005));
    float t = float(log_iter % color_iter) / float(color_iter);
    //float t = float(iter) / float(color_iter);
    FragColor = vec4(mapToColor(t), 1.0); // Colorful
    //FragColor = vec4(vec3(t), 1.0); // Grayscale
}



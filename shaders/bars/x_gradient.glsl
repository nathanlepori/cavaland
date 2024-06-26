#version 410 core

uniform float[120] heights;
uniform float width;
uniform float height;
uniform float num_bars;
uniform vec3 color1;
uniform vec3 color2;
uniform float alpha;
uniform bool showInactiveBars;

void main() {
    vec3 normalize = vec3(255.f);

    vec2 uv = vec2(gl_FragCoord.x / width, gl_FragCoord.y / height);

    int idx = int(uv.x * num_bars);
    float b_width = (1.0 / num_bars); // Calculate the width of each bar

    float barStart = float(idx) * b_width;
    float barEnd = barStart + b_width;

    // Add spacing between bars
    float spacing = 0.001f; // Adjust the spacing as needed
    barStart += spacing;
    barEnd -= spacing;

    if (uv.x > barEnd) {
        discard; // Skip rendering for the spacing between bars
    }

    float currentHeight = heights[idx];

    if (uv.x > barStart && (uv.y < currentHeight || (showInactiveBars && uv.y < 0.01f))) {
        gl_FragColor = vec4(mix(color1/normalize, color2/normalize, uv.x), alpha);
    } else {
        discard; // Skip rendering for the area outside the bars
    }
}

#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 uv;

layout(location = 0) out vec4 fragColor; // Use vec4 for the alpha channel

layout(push_constant) uniform Push
{
    mat4 transform; //projection * view * model
    mat4 normalMatrix;
} push;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, -3.0, -1.0));
const float AMBIENT_INTENSITY = 0.2;
const float STEP_SIZE = 0.005; // Ray marching step size

void main()
{
    vec4 worldPosition = push.transform * vec4(position, 1.0);
    vec3 rayOrigin = worldPosition.xyz;
    vec3 rayDirection = normalize(rayOrigin - cameraPosition); // Update cameraPosition with the actual camera position

    // Ray marching loop for volumetric rendering
    float alpha = 0.0;
    for (float t = 0.0; t < 1.0; t += STEP_SIZE)
    {
        vec3 samplePoint = rayOrigin + t * rayDirection;
        // Perform volume data sampling here (e.g., from a 3D texture or volumetric data source)

        // Calculate the alpha value based on volume density (change this based on your volume data)
        float volumeDensity = 0.5; // Modify this value based on your volume data sampling
        alpha += volumeDensity * STEP_SIZE; // Accumulate alpha based on the volume density

        // Early ray termination when opacity reaches fully opaque
        if (alpha >= 1.0)
            break;
    }

    // Apply lighting and color to the fragment
    vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);
    float lightIntensity = AMBIENT_INTENSITY + max(dot(normalWorldSpace, DIRECTION_TO_LIGHT), 0.0);
    vec3 finalColor = lightIntensity * color;

    // Apply volumetric transparency (change the factor as needed)
    finalColor = mix(finalColor, vec3(1.0), alpha * 0.5);

    fragColor = vec4(finalColor, alpha);
}

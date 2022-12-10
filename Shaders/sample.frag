#version 330 core
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;


uniform vec3 lightPos;
uniform vec3 lightColor;

uniform float ambientStr;
uniform vec3 ambientColor;

uniform vec3 cameraPos;
uniform float specStr;
uniform float specPhong;

in vec2 texCoord;
in vec3 normCoord;
in vec3 fragPos;

out vec4 Fragcolor;

void main()
{
	//Fragcolor = vec4(0.5f, 1.0f, 0.5f, 1.0f);

	vec4 pixelColor = texture(tex0, texCoord);

	if(pixelColor.a < 0.1){
		discard;
	}

	//vec3 normal = normalize(normCoord);
	vec3 normal = texture(tex1, texCoord).rgb;
	vec3 lightDir = normalize(lightPos - fragPos);

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 ambientCol = ambientStr * ambientColor;

	vec3 viewDir = normalize(cameraPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	float spec = pow(max(dot(reflectDir, viewDir), 0.1f),specPhong);

	vec3 specColorTest = vec3(1,1,1);

	vec3 specCol = spec * specStr * specColorTest;
	vec4 brick = vec4(1.0f, 1.0f, 1.0f, 0.6f) * texture(tex0, texCoord);

	vec4 mixColor = mix(brick, texture(tex2, texCoord), texture(tex2, texCoord).a);
	Fragcolor = vec4(specCol + ambientCol + diffuse, 1.0) * mixColor; //pixelColor
}
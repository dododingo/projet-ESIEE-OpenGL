#version 330 core

uniform mat4 myprojection_matrix;
uniform mat4 myview_matrix;
uniform mat3 mynormal_matrix;
uniform mat4 mymodel_matrix;

in vec4 myvertex;
in vec3 mynormal;

uniform vec4 light_colors[16];
uniform vec4 light_positions[16];
uniform vec3 light_directions[16];
uniform int light_types[16];
uniform int numberofLights_shader;
uniform int to_draw;


out vec4 color;
 
vec4 kd = vec4(1,1,1,0);
vec4 ks = vec4(1,1,1,0);
vec4 ka = vec4(0.2,0.2,0.2,1);

void main (void)
{   

	if (to_draw == 2)
	{
		color = kd;
		return;
	}

	
	vec3 eyepos = vec3(0,0,0);

    vec4 _pos = myview_matrix * myvertex;
	vec3 pos = _pos.xyz / _pos.w;

	vec3 normal = normalize(mynormal_matrix * mynormal);
	
	color = ka;

	for(int i=0; i < numberofLights_shader; i++){

		if(light_types[i]==0){

			vec4 _lightpos = light_positions[i];
			vec3 lightpos = _lightpos.xyz / _lightpos.w;

			vec3 light_to_pos = normalize(pos - lightpos);

			vec3 r = normalize(reflect(light_to_pos, normal));

			vec3 pos_to_eyepos = normalize( eyepos - pos );

			float cos_theta = max( dot(normal, -light_to_pos), 0.0 );

		
			color += light_colors[i] * kd * cos_theta;
			color += light_colors[i] * ks * pow(max( dot(r, pos_to_eyepos), 0.0), 10);
		}

		else if(light_types[i]==1){
	
			vec3 light_to_pos = normalize(light_directions[i]);

			vec3 r = normalize(reflect(light_to_pos, normal));

			vec3 pos_to_eyepos = normalize( eyepos - pos );

			float cos_theta = max( dot(normal, -light_to_pos), 0.0 );

		
			color += light_colors[i] * kd * cos_theta;
			color += light_colors[i] * ks * pow(max( dot(r, pos_to_eyepos), 0.0), 10);
	
		}

		else if(light_types[i]==2){
	
			vec4 _lightpos = light_positions[i] * myview_matrix;
			vec3 lightpos = _lightpos.xyz / _lightpos.w;

			vec3 light_to_pos = normalize(pos - lightpos);
			vec3 direction = normalize(mynormal_matrix * light_directions[i]);

			vec3 r = normalize(reflect(light_to_pos, normal));

			float cos_alpha = max( dot(light_to_pos, direction), 0.0);
		
			vec3 pos_to_eyepos = normalize( eyepos - pos );

			float cos_theta = max( dot(normal, -light_to_pos), 0.0 );

		
			color += light_colors[i] * kd * cos_theta * cos_alpha;
			color += light_colors[i] * ks * pow(max( dot(r, pos_to_eyepos), 0.0), 10) * cos_alpha;
		}
	}

	//color = vec4(1,0.5f,0.2,0);
}


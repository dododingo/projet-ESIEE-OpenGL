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
<<<<<<< HEAD
uniform int light_types[16];		// 0 = point light, 1 = directional, 2 = spot
=======
uniform int light_types[16];
>>>>>>> origin/master
uniform int numberofLights_shader;
uniform int to_draw;


out vec4 color;
 
vec4 kd = vec4(1,1,1,0);
vec4 ks = vec4(1,1,1,0);
vec4 ka = vec4(0.2,0.2,0.2,1);

vec4 kd = vec4(1,1,1,0);		//difuse color
vec4 ks = vec4(1,1,1,0);		//specular color
vec4 ka = vec4(0.2,0.2,0.2,1);	//ambiant color

void main (void)
{   

	if (to_draw == 2)
	{
		color = kd;
		return;
	}

	
<<<<<<< HEAD
	vec3 pos_eye = vec3(0,0,0);															//position camera
	vec4 pos_frag_V4 = myview_matrix * myvertex;
	vec3 pos_frag = pos_frag_V4.xyz / pos_frag_V4.w;									//position frag dans la scene
	vec3 normal = normalize(mynormal_matrix * mynormal);								//normale au frag
=======
	vec3 eyepos = vec3(0,0,0);

    vec4 _pos = myview_matrix * myvertex;
	vec3 pos = _pos.xyz / _pos.w;

	vec3 normal = normalize(mynormal_matrix * mynormal);
>>>>>>> origin/master
	
	color = ka;

	for(int i=0; i < numberofLights_shader; i++){

		if(light_types[i]==0){

<<<<<<< HEAD
			vec4 pos_light_V4 = myview_matrix * light_positions[i];
			vec3 pos_light = pos_light_V4.xyz / pos_light_V4.w;							//position light
			vec3 light_to_frag = normalize(pos_frag - pos_light);						//vecteur lampe -> frag
			vec3 r = normalize(reflect(light_to_frag, normal));							//ideal reflection
			vec3 frag_to_eye = normalize( pos_eye - pos_frag );							//vecteur frag -> camera

			float cos_theta = max( dot(normal, -light_to_frag), 0.0 );

			color += light_colors[i] * kd * cos_theta;
			color += light_colors[i] * ks * pow(max( dot(r, frag_to_eye), 0.0), 50);
		}
		
		else if(light_types[i]==1){

		    vec3 light_to_frag = normalize( mynormal_matrix * light_directions[i] );	//vecteur lampe -> frag
			vec3 r = normalize(reflect(light_to_frag, normal));							//ideal reflection
			vec3 frag_to_eye = normalize( pos_eye - pos_frag );							//vecteur frag -> camera

			float cos_theta = max( dot(normal, -light_to_frag ), 0.0 );
			
			color += light_colors[i] * kd * cos_theta;
			color += light_colors[i] * ks * pow(max( dot(r, frag_to_eye), 0.0), 50);
																					  
		}

		else if(light_types[i]==2){
			vec4 pos_light_V4 = myview_matrix * light_positions[i];
			vec3 pos_light = pos_light_V4.xyz / pos_light_V4.w;							//position light

			vec3 light_to_frag = normalize ( pos_frag - pos_light );					//vecteur lampe -> fragment
			vec3 r = normalize(reflect(light_to_frag, normal));							//ideal reflection
			vec3 frag_to_eye = normalize( pos_eye - pos_frag );							//vecteur frag -> camera

			float theta = max(dot(light_to_frag, normalize(mynormal_matrix * light_directions[i]) ), 0.0f);				//angle entre light_to_frag et light_direction
			float inner_cutoff = 0.978f;
			float outer_cutoff = 0.953f;

			float epsilon = inner_cutoff - outer_cutoff;
			float intensity = clamp( (theta - outer_cutoff) / epsilon, 0.0, 1.0);

			float cos_theta = max( dot(normal, -light_to_frag ), 0.0 );

			color += light_colors[i] * kd * cos_theta * intensity;
			color += light_colors[i] * ks * pow(max( dot(r, frag_to_eye), 0.0), 10) * intensity ;
			
		}
	}
=======
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
>>>>>>> origin/master
}


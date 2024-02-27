#version 400
in vec4 vPosition;
in vec4 vColor;
in vec3 vNormal;
out vec4 color;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition[52];
uniform float Shininess;

uniform mat4 modelMatrix;
uniform mat4 View; 
uniform mat4 Projection;
void main()
{	
//Mo hinh chieu sang Blinn - Phong (Phong sua doi)

	// Transform vertex position into eye coordinates
	vec3 pos;
	vec3 L;
	vec3 E;	
	vec3 H;
	vec3 N;
	vec4 ambient;
	float Kd;
	float Ks;
	vec4 diffuse;
	vec4 specular;
	vec4 total_color;

	for (int i = 0 ; i<1 ; i++){
		pos = (View *modelMatrix* vPosition).xyz;
		L = normalize( LightPosition[i].xyz - pos );
		E = normalize( -pos );
		H = normalize(L+E);
	
		// Transform vertex normal into eye coordinates
	
		N = normalize( View * modelMatrix* vec4(vNormal, 0.0) ).xyz;
	
		// Compute terms in the illumination equation
	
		ambient += AmbientProduct;
	
		Kd = max( dot(L, N), 0.0 );
	
		diffuse += Kd*DiffuseProduct;
		
		Ks = pow( max(dot(N, H), 0.0), Shininess );
		
		specular += Ks * SpecularProduct;
		if( dot(L, N) < 0.0 ) specular = vec4(0.0, 0.0, 0.0, 1.0);

		total_color = ambient + diffuse + specular;
	}

	
	color = total_color;
	color.a = 1.0;

    gl_Position = Projection * View * modelMatrix * vPosition/vPosition.w;
		
}//
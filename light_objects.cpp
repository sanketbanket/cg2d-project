#include "light_objects.h"


void ApplyPointToShader(Shader& shader, PointLight* light, int index) {
	string name = "Points[";
	shader.Setvec3((name + to_string(index) + "].position").c_str(), light->Position);
	shader.Setvec3((name + to_string(index) + "].diffuse").c_str(), light->Diffuse * light->strength);
	shader.Setvec3((name + to_string(index) + "].specular").c_str(), light->Specular * light->strength);
}
void ApplySunToShader(Shader& shader, SunLight* light, int index) {
	string name = "Suns[";
	shader.Setvec3((name + to_string(index) + "].direction").c_str(), light->Direction);
	shader.Setvec3((name + to_string(index) + "].diffuse").c_str(), light->Diffuse * light->strength);
	shader.Setvec3((name + to_string(index) + "].specular").c_str(), light->Specular * light->strength);
}
void ApplyConeToShader(Shader& shader, ConeLight* light, int index) {
	string name = "Cones[";
	shader.Setvec3((name + to_string(index) + "].position").c_str(), light->Position);
	shader.Setvec3((name + to_string(index) + "].direction").c_str(), light->Direction);
	shader.Setvec3((name + to_string(index) + "].diffuse").c_str(), light->Diffuse * light->strength);
	shader.Setvec3((name + to_string(index) + "].specular").c_str(), light->Specular * light->strength);
	shader.Set1f((name + to_string(index) + "].cutoff_angle").c_str(), light->Cutoff);
}

void PassPointsToShader(Shader& shader, vector<PointLight*> lights) {
	shader.Set1i("pointCount", lights.size());
	if (lights.empty()) {
		return;
	}
	for (int i = 0; i < lights.size(); i++) {
		ApplyPointToShader(shader, lights[i], i);
	}
}
void PassSunsToShader(Shader& shader, vector<SunLight*> lights) {
	shader.Set1i("sunCount", lights.size());
	if (lights.empty()) {
		return;
	}
	for (int i = 0; i < lights.size(); i++) {
		ApplySunToShader(shader, lights[i], i);
	}
}
void PassConesToShader(Shader& shader, vector<ConeLight*> lights) {
	shader.Set1i("coneCount", lights.size());
	if (lights.empty()) {
		return;
	}
	for (int i = 0; i < lights.size(); i++) {
		ApplyConeToShader(shader, lights[i], i);
	}
}

void RenderLights(Shader& shader, vector<PointLight*> points, vector<SunLight*> suns, vector<ConeLight*> cones) {
	if (!suns.empty()) {
		for (int i = 0; i < suns.size(); i++) {
			suns[i]->Render(shader);
		}
	}
	if (!cones.empty()) {
		for (int i = 0; i < cones.size(); i++) {
			cones[i]->Render(shader);
		}
	}
	if (!points.empty()) {
		for (int i = 0; i < points.size(); i++) {
			points[i]->Render(shader);
		}
	}
}
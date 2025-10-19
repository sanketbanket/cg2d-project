#pragma once
#define _HAS_STD_BYTE 0

#include <Windows.h>

#include <iostream>
#include <vector>
#include "GameObj.h"
#include "light_objects.h"
#include "Scene.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

int Gui(int selectedGameObj, vector<GameObject*>& GameObjVec, vector<PointLight*>& PointLightVec, vector<SunLight*>& suns, vector<ConeLight*>& conevecs);

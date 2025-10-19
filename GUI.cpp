#include "GUI.h"

string objname;string fileName;


std::string FileExplorerDialog(std::vector<GameObject*>& GameObjVec) {
	// Initialize a structure for the file dialog
	OPENFILENAME ofn;
	wchar_t szFile[260] = { 0 }; // Buffer for file name

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = (LPCWSTR)"All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	// Display the Open dialog box
	if (GetOpenFileName(&ofn) == TRUE) {
		int len = WideCharToMultiByte(CP_UTF8, 0, ofn.lpstrFile, -1, NULL, 0, NULL, NULL);
		std::string fileName(len, 0);
		WideCharToMultiByte(CP_UTF8, 0, ofn.lpstrFile, -1, &fileName[0], len, NULL, NULL);
		filesystem::path absPath(fileName);
		filesystem::path currPath = filesystem::current_path();
		// Find common root directory
		auto absIter = absPath.begin();
		auto currIter = currPath.begin();
		while (absIter != absPath.end() && currIter != currPath.end() && *absIter == *currIter) {
			++absIter;
			++currIter;
		}

		// Construct relative path
		filesystem::path relativePath;
		for (; currIter != currPath.end(); ++currIter) {
			relativePath /= "..";
		}
		for (; absIter != absPath.end(); ++absIter) {
			relativePath /= *absIter;
		}

		std::cout << "Selected file: " << fileName << " relative" << relativePath.string() << std::endl;
		return relativePath.string();
	}
	else {
		std::cout << "No file selected" << std::endl;
		return "";
	}
}


void DeleteObj(int selectedItemIndex, vector<GameObject*>& GameObjVec, vector<PointLight*>& PointLightVec, vector<SunLight*>& suns, vector<ConeLight*>& conevecs) {
	int currsize = 0;

	if (selectedItemIndex >= currsize && selectedItemIndex < GameObjVec.size()) {
		GameObjVec.erase(GameObjVec.begin() + selectedItemIndex);
		return;
	}

	currsize += GameObjVec.size();

	if (selectedItemIndex >= currsize && selectedItemIndex < currsize + PointLightVec.size()) {
		PointLightVec.erase(PointLightVec.begin() + (selectedItemIndex - currsize));
		return;
	}

	currsize += PointLightVec.size();

	if (selectedItemIndex >= currsize && selectedItemIndex < currsize + suns.size()) {
		suns.erase(suns.begin() + (selectedItemIndex - currsize));
		return;
	}

	currsize += suns.size();

	if (selectedItemIndex >= currsize && selectedItemIndex < currsize + conevecs.size()) {
		conevecs.erase(conevecs.begin() + (selectedItemIndex - currsize));
		return;
	}
}

void AddGameObj(vector<GameObject*>& GameObjVec) {
	if (ImGui::Button("+")) {
		fileName = FileExplorerDialog(GameObjVec);
		if (fileName != "") {
			objname = "";
			ImGui::OpenPopup("NewObject");
		}
	}
	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("NewObject", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("FilePath: %s", fileName.c_str());
		ImGui::InputText("Enter Name ", &objname);
		if (objname == "")
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Please Enter a Name");
		}
		if (ImGui::Button("Ok", ImVec2(120, 0)))
		{
			if (objname != "")
			{
				GameObject* newObj = new GameObject(fileName, false, objname);
				GameObjVec.push_back(newObj);
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
}

int DisplayObjectListAndGetIndex(int& selectedItemIndex, vector<GameObject*>& GameObjVec, vector<PointLight*>& PointLightVec, vector<SunLight*>& suns, vector<ConeLight*>& conevecs) {
	//int selectedIndex = -1;
	char buf[64];
	int currsize = 0;
	// Display the list of objects
	bool op = ImGui::CollapsingHeader("Game Objects");
	ImGui::SameLine(ImGui::GetWindowWidth() - 30);
	//ImGui::AlignTextToFramePadding();
	AddGameObj(GameObjVec);
	if (op)
	{
		for (int i = 0; i < GameObjVec.size(); ++i) {
			if (ImGui::Selectable(GameObjVec[i]->name.c_str(), selectedItemIndex == i)) {
				selectedItemIndex = i;
			}
			if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
			{
				selectedItemIndex = i + currsize;
				sprintf_s(buf, "Delete Game Object: %s", GameObjVec[i]->name.c_str());
				if (ImGui::Button(buf)) {
					DeleteObj(selectedItemIndex, GameObjVec, PointLightVec, suns, conevecs);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}
	}
	currsize += GameObjVec.size();
	if (ImGui::CollapsingHeader("Point Lights"))
	{
		for (int i = 0; i < PointLightVec.size(); ++i) {
			if (ImGui::Selectable(PointLightVec[i]->name.c_str(), selectedItemIndex == i + currsize)) {
				selectedItemIndex = i + currsize;
			}
			if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
			{
				selectedItemIndex = i + currsize;
				sprintf_s(buf, "Delete Point Light: %s", PointLightVec[i]->name.c_str());
				if (ImGui::Button(buf)) {
					DeleteObj(selectedItemIndex, GameObjVec, PointLightVec, suns, conevecs);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}
	}
	currsize += PointLightVec.size();
	if (ImGui::CollapsingHeader("Sun Lights"))
	{
		for (int i = 0; i < suns.size(); ++i) {
			if (ImGui::Selectable(suns[i]->name.c_str(), selectedItemIndex == i + currsize)) {
				selectedItemIndex = i + currsize;
			}
			if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
			{
				selectedItemIndex = i + currsize;
				sprintf_s(buf, "Delete Sun Light: %s", suns[i]->name.c_str());
				if (ImGui::Button(buf)) {
					DeleteObj(selectedItemIndex, GameObjVec, PointLightVec, suns, conevecs);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}
	}
	currsize += suns.size();
	if (ImGui::CollapsingHeader("Cone Lights"))
	{
		for (int i = 0; i < conevecs.size(); ++i) {
			if (ImGui::Selectable(conevecs[i]->name.c_str(), selectedItemIndex == i + currsize)) {
				selectedItemIndex = i + currsize;
			}
			if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
			{
				selectedItemIndex = i + currsize;
				sprintf_s(buf, "Delete Cone Light: %s", conevecs[i]->name.c_str());
				if (ImGui::Button(buf)) {
					DeleteObj(selectedItemIndex, GameObjVec, PointLightVec, suns, conevecs);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}
	}

	return selectedItemIndex;
}
void AddPointPopup(vector<PointLight*>& PointLightVec) {
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("NewPoint", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputText("Enter Name ", &objname);
		if (objname == "")
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Please Enter a Name");
		}
		if (ImGui::Button("Ok", ImVec2(120, 0)))
		{
			if (objname != "")
			{
				PointLight* p_temp = new PointLight(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), glm::vec3(1.0f), objname);
				PointLightVec.push_back(p_temp);
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}

}

void ImGuiConeLight(vector<ConeLight*>& conevecs, int selection) {
	// render your GUI

	ImGui::SetNextWindowSize(ImVec2(500, 500));
	if (ImGui::Begin("Light Properties", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Button("Cone light");   //diffuse,specular,strength,pos,dir,cutoff angle
		ImGui::BeginChild("position", ImVec2(0, ImGui::GetWindowContentRegionMin().y * 2));
		ImGui::Columns(4);

		ImGui::Text("Position");
		ImGui::SameLine();
		ImGui::SetColumnWidth(0, 65);

		ImGui::NextColumn();
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		if (ImGui::Button("X", buttonSize))
			conevecs[selection]->Position.x = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##X", &conevecs[selection]->Position.x, 0.1);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Y", buttonSize))
			conevecs[selection]->Position.y = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Y", &conevecs[selection]->Position.y, 0.1);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Z", buttonSize))
			conevecs[selection]->Position.z = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Z", &conevecs[selection]->Position.z, 0.1);
		ImGui::NextColumn();

		ImGui::Text("Direction");
		ImGui::SameLine();
		ImGui::SetColumnWidth(0, 65);
		ImGui::NextColumn();

		if (ImGui::Button("X", buttonSize))
			conevecs[selection]->Direction.x = 0;
		ImGui::SameLine();
		ImGui::DragFloat("##X_angle", &conevecs[selection]->Direction.x, 0.01);
		ImGui::SameLine();
		ImGui::NextColumn();

		if (ImGui::Button("Y", buttonSize))
			conevecs[selection]->Direction.y = 0;
		ImGui::SameLine();
		ImGui::DragFloat("##Y_angle", &conevecs[selection]->Direction.y, 0.01);
		ImGui::SameLine();
		ImGui::NextColumn();

		if (ImGui::Button("Z", buttonSize))
			conevecs[selection]->Direction.z = 0;
		ImGui::SameLine();
		ImGui::DragFloat("##Z_angle", &conevecs[selection]->Direction.z, 0.01);
		ImGui::NextColumn();
		ImGui::EndChild();

		ImGui::BeginChild("others");
		ImGui::Columns(2);
		ImGui::Text("Strength");
		ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::SetColumnWidth(0, 65);
		ImGui::DragFloat("##Strength", &conevecs[selection]->strength, 0.1);
		ImGui::NextColumn();

		ImGui::Text("Cutoff angle");
		ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::DragFloat("##Cutoff", &conevecs[selection]->Cutoff, 0.1);
		ImGui::NextColumn();

		ImGui::Text("Diffuse");
		ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::ColorEdit3("##Diffuse", (float*)&conevecs[selection]->Diffuse);
		ImGui::NextColumn();

		ImGui::Text("Specular");
		ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::ColorEdit3("##Specular", (float*)&conevecs[selection]->Specular);
		ImGui::EndChild();

	}ImGui::End();
}
void ImGuiSunLight(vector<SunLight*>& suns, int selection) {
	// render your GUI

	ImGui::SetNextWindowSize(ImVec2(500, 500));
	if (ImGui::Begin("Light Properties", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Button("Sun light");   //diffuse,specular,strength,pos,dir
		ImGui::BeginChild("position", ImVec2(0, ImGui::GetWindowContentRegionMin().y * 2));
		ImGui::Columns(4);

		ImGui::Text("Position");
		ImGui::SameLine();
		ImGui::SetColumnWidth(0, 65);

		ImGui::NextColumn();
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		if (ImGui::Button("X", buttonSize))
			suns[selection]->Position.x = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##X", &suns[selection]->Position.x, 0.1);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Y", buttonSize))
			suns[selection]->Position.y = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Y", &suns[selection]->Position.y, 0.1);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Z", buttonSize))
			suns[selection]->Position.z = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Z", &suns[selection]->Position.z, 0.1);
		ImGui::NextColumn();

		ImGui::Text("Direction");
		ImGui::SameLine();
		ImGui::SetColumnWidth(0, 65);
		ImGui::NextColumn();

		if (ImGui::Button("X", buttonSize))
			suns[selection]->Direction.x = 0;
		ImGui::SameLine();
		ImGui::DragFloat("##X_angle", &suns[selection]->Direction.x, 0.01);
		ImGui::SameLine();
		ImGui::NextColumn();

		if (ImGui::Button("Y", buttonSize))
			suns[selection]->Direction.y = 0;
		ImGui::SameLine();
		ImGui::DragFloat("##Y_angle", &suns[selection]->Direction.y, 0.01);
		ImGui::SameLine();
		ImGui::NextColumn();

		if (ImGui::Button("Z", buttonSize))
			suns[selection]->Direction.z = 0;
		ImGui::SameLine();
		ImGui::DragFloat("##Z_angle", &suns[selection]->Direction.z, 0.01);
		ImGui::NextColumn();
		ImGui::EndChild();

		ImGui::BeginChild("others");
		ImGui::Columns(2);
		ImGui::Text("Strength");
		ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::SetColumnWidth(0, 65);
		ImGui::DragFloat("##Strength", &suns[selection]->strength, 0.1);
		ImGui::NextColumn();

		ImGui::Text("Diffuse");
		ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::ColorEdit3("##Diffuse", (float*)&suns[selection]->Diffuse);
		ImGui::NextColumn();

		ImGui::Text("Specular");
		ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::ColorEdit3("##Specular", (float*)&suns[selection]->Specular);
		ImGui::EndChild();
	}ImGui::End();
}
void ImguiPointLight(vector<PointLight*>& PointLightVec, int selection) {  //strength,position,diffuse,specular

	// render your GUI

	ImGui::SetNextWindowSize(ImVec2(500, 500));
	if (ImGui::Begin("Light Properties", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Button("Point light");
		ImGui::BeginChild("position", ImVec2(0, ImGui::GetWindowContentRegionMin().y));
		ImGui::Columns(4);
		//position]
		ImGui::Text("Position");
		ImGui::SameLine();
		ImGui::SetColumnWidth(0, 65);

		ImGui::NextColumn();
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		if (ImGui::Button("X", buttonSize))
			PointLightVec[selection]->Position.x = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##X", &PointLightVec[selection]->Position.x, 0.1);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Y", buttonSize))
			PointLightVec[selection]->Position.y = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Y", &PointLightVec[selection]->Position.y, 0.1);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Z", buttonSize))
			PointLightVec[selection]->Position.z = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Z", &PointLightVec[selection]->Position.z, 0.1);
		ImGui::NextColumn();
		ImGui::EndChild();
		ImGui::BeginChild("others");
		ImGui::Columns(2);
		ImGui::Text("Strength");
		ImGui::SameLine();
		ImGui::SetColumnWidth(0, 65);
		ImGui::NextColumn();
		ImGui::DragFloat("##Strength", &PointLightVec[selection]->strength, 0.1);
		ImGui::NextColumn();

		ImGui::Text("Diffuse");
		ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::ColorEdit3("##Diffuse", (float*)&PointLightVec[selection]->Diffuse);
		ImGui::NextColumn();

		ImGui::Text("Specular");
		ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::ColorEdit3("##Specular", (float*)&PointLightVec[selection]->Specular);
		ImGui::EndChild();
	}ImGui::End();

}
void ImguiGameObject(vector<GameObject*>& GameObjVec, int selection) {
	ImGui::SetNextWindowSize(ImVec2(500, 500));
	if (ImGui::Begin("Transform", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Columns(4);
		//position
		ImGui::Text("Position");
		ImGui::SameLine();
		ImGui::SetColumnWidth(0, 65);

		ImGui::NextColumn();
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		if (ImGui::Button("X", buttonSize))
			GameObjVec[selection]->tvecm.x = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##X", &GameObjVec[selection]->tvecm.x, 0.1);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Y", buttonSize))
			GameObjVec[selection]->tvecm.y = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Y", &GameObjVec[selection]->tvecm.y, 0.1);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Z", buttonSize))
			GameObjVec[selection]->tvecm.z = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Z", &GameObjVec[selection]->tvecm.z, 0.1);
		ImGui::NextColumn();
		//rotation
		ImGui::Text("Rotation");
		ImGui::SameLine();
		ImGui::SetColumnWidth(0, 65);

		ImGui::NextColumn();


		if (ImGui::Button("X", buttonSize))
			GameObjVec[selection]->xaxisanglem = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##X_angle", &GameObjVec[selection]->xaxisanglem);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Y", buttonSize))
			GameObjVec[selection]->yaxisanglem = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Y_angle", &GameObjVec[selection]->yaxisanglem);
		ImGui::SameLine();

		ImGui::NextColumn();

		if (ImGui::Button("Z", buttonSize))
			GameObjVec[selection]->zaxisanglem = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Z_angle", &GameObjVec[selection]->zaxisanglem);
		ImGui::NextColumn();
		//scale
		ImGui::Text("Scale");
		ImGui::SameLine();
		ImGui::SetColumnWidth(0, 65);

		ImGui::NextColumn();


		if (ImGui::Button("X", buttonSize))
			GameObjVec[selection]->svecm.x = 1;


		ImGui::SameLine();
		ImGui::DragFloat("##X_scale", &GameObjVec[selection]->svecm.x, 0.05);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Y", buttonSize))
			GameObjVec[selection]->svecm.y = 1;


		ImGui::SameLine();
		ImGui::DragFloat("##Y_scale", &GameObjVec[selection]->svecm.y, 0.05);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Z", buttonSize))
			GameObjVec[selection]->svecm.z = 1;


		ImGui::SameLine();
		ImGui::DragFloat("##Z_scale", &GameObjVec[selection]->svecm.z, 0.05);
	}
	ImGui::End();
}
int Gui(int selectedGameObj, vector<GameObject*>& GameObjVec, vector<PointLight*>& PointLightVec, vector<SunLight*>& suns, vector<ConeLight*>& conevecs) {

	// feed inputs to dear imgui, start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// render your GUI
	if (ImGui::Begin("Heirarchy", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		selectedGameObj = DisplayObjectListAndGetIndex(selectedGameObj, GameObjVec, PointLightVec, suns, conevecs);
	}
	if (ImGui::Button("New Game Object")) {
		fileName = FileExplorerDialog(GameObjVec);
		if (fileName != "") {
			objname = "";
			ImGui::OpenPopup("NewObject1");
		}
	}
	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("NewObject1", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("FilePath: %s", fileName.c_str());
		ImGui::InputText("Enter Name ", &objname);
		if (objname == "")
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Please Enter a Name");
		}
		if (ImGui::Button("Ok", ImVec2(120, 0)))
		{
			if (objname != "")
			{
				GameObject* newObj = new GameObject(fileName, false, objname);
				GameObjVec.push_back(newObj);
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}

	/*if (ImGui::Button("Delete Selected GameObject")) {
		DeleteObj(selectedGameObj, GameObjVec, PointLightVec, suns, conevecs, scenes);
		selectedGameObj--;
	}*/
	//if (ImGui::Button("Add Scene")) {
	//	Scene* scene1 = new Scene;
	//	scenes.push_back(scene1);
	//}
	//if (ImGui::Button("Next Scene")) {
	//	selScene++;
	//	if (selScene >= scenes.size()) selScene = 0;
	//}
	if (ImGui::Button("Add Point Light")) {
		objname = "";
		ImGui::OpenPopup("NewPoint");
	}
	AddPointPopup(PointLightVec);
	if (ImGui::Button("Add Sun Light")) {
		SunLight* stemp = new SunLight(glm::vec3(0.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), "s");
		suns.push_back(stemp);
	}
	if (ImGui::Button("Add Cone Light")) {
		ConeLight* ctemp = new ConeLight(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 20.0f, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), "c");
		conevecs.push_back(ctemp);
	}
	ImGui::End();

	int currsize = 0;
	if (selectedGameObj >= 0 && selectedGameObj < GameObjVec.size()) {
		ImguiGameObject(GameObjVec, selectedGameObj - currsize);
	}
	currsize += GameObjVec.size();
	if (selectedGameObj >= currsize && selectedGameObj < currsize + PointLightVec.size()) {
		ImguiPointLight(PointLightVec, selectedGameObj - currsize);
	}
	currsize += PointLightVec.size();
	if (selectedGameObj >= currsize && selectedGameObj < currsize + suns.size()) {
		ImGuiSunLight(suns, selectedGameObj - currsize);
	}
	currsize += suns.size();
	if (selectedGameObj >= currsize && selectedGameObj < currsize + conevecs.size()) {
		ImGuiConeLight(conevecs, selectedGameObj - currsize);
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
	return selectedGameObj;
}

//
//  my_widgets.cpp
//  mySketchimgui_test_2
//
//  Created by David Rokeby on 2022-04-09.
//

#include "my_widgets.hpp"

bool ToggleButton(const char* str_id, bool* v)
{
    ImVec4* colors = ImGui::GetStyle().Colors;
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float height = ImGui::GetFrameHeight();
    float width = height * 1.55f;
    float radius = height * 0.50f;

    ImVec4 inactiveBlue = colors[ImGuiCol_SliderGrab];
    inactiveBlue.w = 1.0f;

    ImGui::InvisibleButton(str_id, ImVec2(width, height));
    if (ImGui::IsItemClicked()) *v = !*v;
    ImGuiContext& gg = *GImGui;
    float ANIM_SPEED = 0.085f;
    if (gg.LastActiveId == gg.CurrentWindow->GetID(str_id))// && g.LastActiveIdTimer < ANIM_SPEED)
        float t_anim = ImSaturate(gg.LastActiveIdTimer / ANIM_SPEED);
    if (ImGui::IsItemHovered())
        draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), ImGui::GetColorU32(*v ? colors[ImGuiCol_ButtonActive] : ImVec4(0.75f, 0.75f, 0.75f, 1.0f)), height * 0.5f);
    else
        draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), ImGui::GetColorU32(*v ? inactiveBlue : ImVec4(0.5f, 0.5f, 0.5f, 1.0f)), height * 0.50f);
    draw_list->AddCircleFilled(ImVec2(p.x + radius + (*v ? 1 : 0) * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
    const char* label_end = ImGui::FindRenderedTextEnd(str_id);
//    draw_list->AddText(ImVec2(p.x + width, p.y), ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), str_id, str_id + strlen(str_id));
    if (str_id != label_end)
    {
        ImGui::SameLine(0, style.ItemInnerSpacing.x);
        ImGui::TextEx(str_id, label_end);
    }
    return *v;
}


bool MinimalSlider2D(char const* pLabel, ImGuiDataType data_type, void* p_valueX, void* p_valueY, void* p_minX, void* p_maxX, void* p_minY, void* p_maxY, float const fScale /*= 1.0f*/)
{
    assert(ImWidgets::ScalarToFloat(data_type, (ImU64*)p_minX) < ImWidgets::ScalarToFloat(data_type, (ImU64*)p_maxX));
    assert(ImWidgets::ScalarToFloat(data_type, (ImU64*)p_minY) < ImWidgets::ScalarToFloat(data_type, (ImU64*)p_maxY));

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiID const iID = ImGui::GetID(pLabel);

    ImVec2 const vSize(128, 128);
    ImVec2 const safe(5, 5);

    float const fHeightOffset = ImGui::GetTextLineHeight();
    ImVec2 const vHeightOffset(0.0f, fHeightOffset);

    ImVec2 vPos = ImGui::GetCursorScreenPos();
    ImRect oRect(vPos + safe, vPos + vSize + safe);

    ImGui::PushID(iID);

    ImVec2 const vOriginPos = ImGui::GetCursorScreenPos();
    ImU32 const uFrameCol = ImGui::GetColorU32(ImGuiCol_WindowBg);
    ImU32 const uLineCol = ImGui::GetColorU32(ImGuiCol_FrameBg);

    ImGui::RenderFrame(oRect.Min, oRect.Max, uFrameCol, true, 0.0f);

    
    ImDrawList* pDrawList = ImGui::GetWindowDrawList();
//        pDrawList->AddLine(ImVec2(oRect.Min.x, mid.y), ImVec2(oRect.Max.x, mid.y), uLineCol, 0.5f);
//        pDrawList->AddLine(ImVec2(mid.x, oRect.Min.y), ImVec2(mid.x, oRect.Max.y), uLineCol, 0.5f);

    ImU64 s_delta_x = ImWidgets::SubScalar(data_type, p_maxX, p_minX);
    ImU64 s_delta_y = ImWidgets::SubScalar(data_type, p_maxY, p_minY);

    bool bModified = false;
    ImVec2 const vSecurity(5.0f, 5.0f);
    ImRect frame_bb = ImRect(oRect.Min - vSecurity, oRect.Max + vSecurity);

    bool hovered;
    bool held;
    bool pressed = ImGui::ButtonBehavior(frame_bb, ImGui::GetID("##Zone"), &hovered, &held);
    if (hovered && held)
    {
        ImVec2 const vCursorPos = ImGui::GetMousePos() - oRect.Min;

        float fValueX = vCursorPos.x / (oRect.Max.x - oRect.Min.x) * ImWidgets::ScalarToFloat(data_type, &s_delta_x) + ImWidgets::ScalarToFloat(data_type, (ImU64*)p_minX);
        float fValueY = ImWidgets::ScalarToFloat(data_type, &s_delta_y) - vCursorPos.y / (oRect.Max.y - oRect.Min.y) * ImWidgets::ScalarToFloat(data_type, &s_delta_y) + ImWidgets::ScalarToFloat(data_type, (ImU64*)p_minY);

        ImU64 s_value_x = ImWidgets::FloatToScalar(data_type, fValueX);
        ImU64 s_value_y = ImWidgets::FloatToScalar(data_type, fValueY);

        ImWidgets::EqualScalar(data_type, (ImU64*)p_valueX, &s_value_x);
        ImWidgets::EqualScalar(data_type, (ImU64*)p_valueY, &s_value_y);

        bModified = true;
    }

    ImU64 s_clamped_x = ImWidgets::ClampScalar(data_type, p_valueX, p_minX, p_maxX);
    ImU64 s_clamped_y = ImWidgets::ClampScalar(data_type, p_valueY, p_minY, p_maxY);
    ImWidgets::EqualScalar(data_type, (ImU64*)p_valueX, &s_clamped_x);
    ImWidgets::EqualScalar(data_type, (ImU64*)p_valueY, &s_clamped_y);

    float const fScaleX = (ImWidgets::ScalarToFloat(data_type, (ImU64*)p_valueX) - ImWidgets::ScalarToFloat(data_type, (ImU64*)p_minX)) / ImWidgets::ScalarToFloat(data_type, &s_delta_x);
    float const fScaleY = 1.0f - (ImWidgets::ScalarToFloat(data_type, (ImU64*)p_valueY) - ImWidgets::ScalarToFloat(data_type, (ImU64*)p_minY)) / ImWidgets::ScalarToFloat(data_type, &s_delta_y);

    constexpr float fCursorOff = 10.0f;
    float const fXLimit = fCursorOff / oRect.GetWidth();
    float const fYLimit = fCursorOff / oRect.GetHeight();

    ImVec2 const vCursorPos((oRect.Max.x - oRect.Min.x) * fScaleX + oRect.Min.x, (oRect.Max.y - oRect.Min.y) * fScaleY + oRect.Min.y);


    ImS32 inactiveBlue = ImGui::GetColorU32(ImGuiCol_SliderGrab);
    inactiveBlue |= 0xFF000000;
    ImS32 const uBlueActive = ImGui::GetColorU32(ImGuiCol_SliderGrabActive);
    ImS32 const uBlue = ImGui::GetColorU32(ImGuiCol_SliderGrab);

    constexpr float fBorderThickness    = 2.0f;
    constexpr float fLineThickness        = 3.0f;
    constexpr float fHandleRadius        = 7.0f;
    constexpr float fHandleOffsetCoef    = 2.0f; //2.0f;

    // Cursor
    if(hovered && held)
        pDrawList->AddCircleFilled(vCursorPos, 5.0f, uBlueActive, 16);
    else
        pDrawList->AddCircleFilled(vCursorPos, 5.0f, inactiveBlue, 16);

    std::string formatX = ImGui::DataTypeGetInfo(data_type)->PrintFmt;
    std::string formatY = ImGui::DataTypeGetInfo(data_type)->PrintFmt;

    char pBufferX[64];
    char pBufferY[64];

    ImU32 const uTextCol = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Text]);

    ImVec2 const vXSize = ImVec2(0, 0); //ImGui::CalcTextSize(pBufferX);
    ImVec2 const vYSize = ImVec2(0, 0); //ImGui::CalcTextSize(pBufferY);

    ImGui::DataTypeFormatString(pBufferX, IM_ARRAYSIZE(pBufferX), data_type, p_valueX, formatX.c_str());
    ImGui::DataTypeFormatString(pBufferY, IM_ARRAYSIZE(pBufferY), data_type, p_valueY, formatY.c_str());

    ImGui::PopID();

    ImGui::Dummy(vSize + safe + safe - vHeightOffset);
    ImVec2 text_width = ImGui::CalcTextSize(std::to_string(1.0f).c_str()) * ImVec2(1.1f, 1.1f);
    ImVec2 nowPos = ImGui::GetCursorScreenPos();
    pDrawList->AddText(ImVec2(nowPos.x + text_width.x, nowPos.y + vHeightOffset.y * 1.1f + 1), uTextCol, pBufferX);
    ImGui::Dummy(vHeightOffset);
    nowPos = ImGui::GetCursorScreenPos();
    pDrawList->AddText(ImVec2(nowPos.x + text_width.x, nowPos.y + vHeightOffset.y * 1.1f + 1), uTextCol, pBufferY);

    return bModified;
}

bool MinimalSlider2DInt(char const* pLabel, int* pValueX, int* pValueY, int* p_minX, int* p_maxX, int* p_minY, int* p_maxY, float const fScale)
{
    return MinimalSlider2D(pLabel, ImGuiDataType_S32, pValueX, pValueY, p_minX, p_maxX, p_minY, p_maxY, fScale);
}

bool MinimalSlider2DFloat(char const* pLabel, float* pValueX, float* pValueY, float minX, float maxX, float minY, float maxY, float const fScale)
{
    return MinimalSlider2D(pLabel, ImGuiDataType_Float, pValueX, pValueY, &minX, &maxX, &minY, &maxY, fScale);
}

bool MinimalSlider2DDouble(char const* pLabel, double* pValueX, double* pValueY, double minX, double maxX, double minY, double maxY, float const fScale)
{
    return MinimalSlider2D(pLabel, ImGuiDataType_Double, pValueX, pValueY, &minX, &maxX, &minY, &maxY, fScale);
}


void EditTransform(ofMatrix4x4& viewMatrix, ofMatrix4x4& projectionMatrix, ofMatrix4x4& matrix)
{
   static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
   static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
    
   if (ImGui::IsKeyPressed(90))
      mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    
   if (ImGui::IsKeyPressed(69))
      mCurrentGizmoOperation = ImGuizmo::ROTATE;
    
   if (ImGui::IsKeyPressed(82)) // r Key
      mCurrentGizmoOperation = ImGuizmo::SCALE;
    
   if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
      mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    
   ImGui::SameLine();
    
   if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
      mCurrentGizmoOperation = ImGuizmo::ROTATE;
    
   ImGui::SameLine();
    
   if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
      mCurrentGizmoOperation = ImGuizmo::SCALE;
    
   float matrixTranslation[3], matrixRotation[3], matrixScale[3];
   ImGuizmo::DecomposeMatrixToComponents(matrix.getPtr(), matrixTranslation, matrixRotation, matrixScale);
   ImGui::InputFloat3("Tr", matrixTranslation);
   ImGui::InputFloat3("Rt", matrixRotation);
   ImGui::InputFloat3("Sc", matrixScale);
   ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix.getPtr());

   if (mCurrentGizmoOperation != ImGuizmo::SCALE)
   {
      if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
         mCurrentGizmoMode = ImGuizmo::LOCAL;
      ImGui::SameLine();
      if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
         mCurrentGizmoMode = ImGuizmo::WORLD;
   }
   static bool useSnap(false);
   if (ImGui::IsKeyPressed(83))
      useSnap = !useSnap;
   ImGui::Checkbox("", &useSnap);
   ImGui::SameLine();
   ofVec3f snap;
    
/*   switch (mCurrentGizmoOperation)
   {
       case ImGuizmo::TRANSLATE:
          snap = config.mSnapTranslation;
          ImGui::InputFloat3("Snap", &snap.x);
          break;
           
       case ImGuizmo::ROTATE:
          snap = config.mSnapRotation;
          ImGui::InputFloat("Angle Snap", &snap.x);
          break;
           
       case ImGuizmo::SCALE:
          snap = config.mSnapScale;
          ImGui::InputFloat("Scale Snap", &snap.x);
          break;
   }*/
    useSnap = false;
    
   ImGuiIO& io = ImGui::GetIO();
   ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
   ImGuizmo::Manipulate(viewMatrix.getPtr(), projectionMatrix.getPtr(), mCurrentGizmoOperation, mCurrentGizmoMode, matrix.getPtr(), NULL, useSnap ? &snap.x : NULL);
}


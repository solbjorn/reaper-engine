#pragma once

// Хранилище внешних шейдерных параметров, которые читаются в Blender_Recorder_StandartBinding.cpp
class ShadersExternalData //--#SM+#--
{
public:
    Fmatrix m_script_params{}; // Матрица, значения которой доступны из Lua. На данный момент не используется.
    Fvector4 hud_params{}; // [zoom_rotate_factor, secondVP_zoom_factor, NULL, NULL] - Параметры худа оружия
    Fvector4 m_blender_mode{}; // x = [0 - default, 1 - night vision, 2 - thermo vision] - Режимы рендеринга
                               // y = [0.0f / 1.0f - происходит ли в данный момент рендеринг картинки для прицела]; z = [0.0f / 1.0f - выключен или включён двойной рендер], w =
                               // зарезервировано на будущее
};

#include "stdafx.h"
#include "Light_Package.h"

void light_Package::clear()
{
    v_point.clear();
    v_spot.clear();
    v_shadowed.clear();
}

void light_Package::sort()
{
    const auto pred_light_cmp = [](const light* l1, const light* l2) {
        return l1->range > l2->range; // sort by range
    };

    // resort lights (pending -> at the end), maintain stable order
    std::ranges::stable_sort(v_point, pred_light_cmp);
    std::ranges::stable_sort(v_spot, pred_light_cmp);
    std::ranges::stable_sort(v_shadowed, pred_light_cmp);
}

// Создаем запросы к окклюдеру. Потом мы будем получать от них данные
// в обратном порядке, так, что бы мы сначала дождались ответа от
// самого свежего запроса, т.е. от v_spot[ -1 ]. Полагаю, что если
// готовы данные для самого свежего запроса, то для более старых они
// будут готовы тем более и мы будет ждать ответа на запрос только
// один раз за кадр.
void light_Package::vis_prepare()
{
    for (light* L : v_point)
        L->vis_prepare();
    for (light* L : v_shadowed)
        L->vis_prepare();
    for (light* L : v_spot)
        L->vis_prepare();
}

// Получаем ответы от запросов к окклюдеру в обратном порядке, от
// самого свежего запроса, к самому старому. См. комментарии выше.
void light_Package::vis_update()
{
    if (!v_spot.empty())
    {
        for (int it = v_spot.size() - 1; it >= 0; it--)
        {
            light* L = v_spot[it];
            L->vis_update();
        }
    }
    if (!v_shadowed.empty())
    {
        for (int it = v_shadowed.size() - 1; it >= 0; it--)
        {
            light* L = v_shadowed[it];
            L->vis_update();
        }
    }
    if (!v_point.empty())
    {
        for (int it = v_point.size() - 1; it >= 0; it--)
        {
            light* L = v_point[it];
            L->vis_update();
        }
    }
}

#pragma once

namespace UIMapWndActionsSpace
{
enum EWorldProperties
{
    ePropTargetMapShown,
    ePropMapMinimized,
    ePropMapResized,
    ePropMapIdle,
    ePropMapCentered,
    ePropDummy = std::numeric_limits<u16>::max(),
};

enum EWorldOperators
{
    eOperatorMapResize,
    eOperatorMapMinimize,
    eOperatorMapIdle,
    eOperatorMapCenter,
    eWorldOperatorDummy = std::numeric_limits<u16>::max(),
};
} // namespace UIMapWndActionsSpace

#pragma once

struct vertRender;
struct vertBoned1W;
struct vertBoned2W;
struct vertBoned3W;
struct vertBoned4W;
class CBoneInstance;

void Skin1W(vertRender* D, vertBoned1W* S, u32 vCount, CBoneInstance* Bones);
void Skin2W(vertRender* D, vertBoned2W* S, u32 vCount, CBoneInstance* Bones);
void Skin3W(vertRender* D, vertBoned3W* S, u32 vCount, CBoneInstance* Bones);
void Skin4W(vertRender* D, vertBoned4W* S, u32 vCount, CBoneInstance* Bones);

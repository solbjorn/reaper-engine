#ifndef RenderDetailModel_included
#define RenderDetailModel_included

class XR_NOVTABLE IRenderDetailModel : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IRenderDetailModel);

public:
    ~IRenderDetailModel() override = 0;
};

inline IRenderDetailModel::~IRenderDetailModel() = default;

#endif //	RenderDetailModel_included

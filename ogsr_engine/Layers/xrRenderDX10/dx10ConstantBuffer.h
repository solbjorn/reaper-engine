#ifndef dx10ConstantBuffer_included
#define dx10ConstantBuffer_included

#include "../../xr_3da/context.h"

struct R_constant;
struct R_constant_load;

class dx10ConstantBuffer final : public xr_resource_named
{
    RTTI_DECLARE_TYPEINFO(dx10ConstantBuffer, xr_resource_named);

public:
    explicit dx10ConstantBuffer(ID3DShaderReflectionConstantBuffer* pTable);
    ~dx10ConstantBuffer() override;

    bool Similar(const dx10ConstantBuffer& _in);
    ID3DBuffer* GetBuffer() { return m_pBuffer; }

    void Flush(ctx_id_t context_id);

    //	Set copy data into constant buffer
    //	Plain buffer member
    void set(R_constant* C, R_constant_load& L, const Fmatrix& A);
    void set(R_constant* C, R_constant_load& L, const Fvector4& A);
    void set(R_constant* C, R_constant_load& L, float A);
    void set(R_constant* C, R_constant_load& L, int A);
    //	Array buffer member
    void seta(R_constant* C, R_constant_load& L, u32 e, const Fmatrix& A);
    void seta(R_constant* C, R_constant_load& L, u32 e, const Fvector4& A);

    void* AccessDirect(R_constant_load& L, size_t DataSize);

private:
    template <typename T>
    [[nodiscard]] T& Access(std::size_t offset);

    shared_str m_strBufferName;
    D3D_CBUFFER_TYPE m_eBufferType;

    //	Buffer data description
    u64 m_uiMembersXXH;
    xr_vector<D3D_SHADER_TYPE_DESC> m_MembersList;
    xr_vector<shared_str> m_MembersNames;

    ID3DBuffer* m_pBuffer;
    std::byte* m_pBufferData;
    u32 m_uiBufferSize;
    bool m_bChanged{true};

    static constexpr auto lineSize = sizeof(Fvector4);

    //	Never try to copy objects of this class due to the pointer and autoptr members
    dx10ConstantBuffer(const dx10ConstantBuffer&);
    dx10ConstantBuffer& operator=(dx10ConstantBuffer&);
};

typedef resptr_core<dx10ConstantBuffer, resptr_base<dx10ConstantBuffer>> ref_cbuffer;

#endif //	dx10ConstantBuffer_included

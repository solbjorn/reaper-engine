#ifndef dx10ConstantBuffer_impl_included
#define dx10ConstantBuffer_impl_included

IC Fvector4* dx10ConstantBuffer::Access(u16 offset)
{
    //	TODO: DX10: Implement code which will check if set actually changes code.
    m_bChanged = true;

    //	Check buffer size in client code: don't know if actual data will cross
    //	buffer boundaries.
    VERIFY(offset < (int)m_uiBufferSize);
    BYTE* res = ((BYTE*)m_pBufferData) + offset;
    return (Fvector4*)res;
}

IC void dx10ConstantBuffer::set(R_constant* C, R_constant_load& L, const Fmatrix& A)
{
    VERIFY(RC_float == C->type);

    Fmatrix trans;
    trans.transpose(A);

    Fmatrix* it = reinterpret_cast<Fmatrix*>(Access(L.index));
    switch (L.cls)
    {
    case RC_2x4:
        VERIFY(u32((u32)L.index + 2 * lineSize) <= m_uiBufferSize);
        it->vm[0].set(trans.vm[0]);
        it->vm[1].set(trans.vm[1]);
        break;
    case RC_3x4:
        VERIFY(u32((u32)L.index + 3 * lineSize) <= m_uiBufferSize);
        it->vm[0].set(trans.vm[0]);
        it->vm[1].set(trans.vm[1]);
        it->vm[2].set(trans.vm[2]);
        break;
    case RC_4x4:
        VERIFY(u32((u32)L.index + 4 * lineSize) <= m_uiBufferSize);
        *it = trans;
        break;
    default:
#ifdef DEBUG
        Debug.fatal(DEBUG_INFO, "Invalid constant run-time-type for '%s'", *C->name);
#else
        NODEFAULT;
#endif
    }
}

IC void dx10ConstantBuffer::set(R_constant* C, R_constant_load& L, const Fvector4& A)
{
    VERIFY(RC_float == C->type);
    VERIFY(RC_1x4 == L.cls || RC_1x3 == L.cls || RC_1x2 == L.cls);

    VERIFY(u32((u32)L.index + lineSize) <= m_uiBufferSize);
    Fvector4* it = Access(L.index);

    switch (L.cls)
    {
    case RC_1x2: *reinterpret_cast<DirectX::XMVECTOR*>(it) = DirectX::XMLoadFloat2A(reinterpret_cast<const DirectX::XMFLOAT2A*>(&A)); break;
    case RC_1x3: *reinterpret_cast<DirectX::XMVECTOR*>(it) = DirectX::XMLoadFloat3A(reinterpret_cast<const DirectX::XMFLOAT3A*>(&A)); break;
    case RC_1x4:
    default: it->set(A); break;
    }
}

IC void dx10ConstantBuffer::set(R_constant* C, R_constant_load& L, float A)
{
    VERIFY(RC_float == C->type);
    VERIFY(RC_1x1 == L.cls);
    float* it = (float*)Access(L.index);
    VERIFY(u32((u32)L.index + sizeof(float)) <= m_uiBufferSize);
    *it = A;
}

IC void dx10ConstantBuffer::set(R_constant* C, R_constant_load& L, int A)
{
    VERIFY(RC_int == C->type);
    VERIFY(RC_1x1 == L.cls);
    int* it = (int*)Access(L.index);
    VERIFY(u32((u32)L.index + sizeof(int)) <= m_uiBufferSize);
    *it = A;
}

IC void dx10ConstantBuffer::seta(R_constant* C, R_constant_load& L, u32 e, const Fmatrix& A)
{
    VERIFY(RC_float == C->type);

    Fmatrix trans;
    trans.transpose(A);

    u32 base;
    Fmatrix* it;
    switch (L.cls)
    {
    case RC_2x4:
        base = (u32)L.index + 2 * lineSize * e;
        it = reinterpret_cast<Fmatrix*>(Access((u16)base));
        VERIFY((base + 2 * lineSize) <= m_uiBufferSize);
        it->vm[0].set(trans.vm[0]);
        it->vm[1].set(trans.vm[1]);
        break;
    case RC_3x4:
        base = (u32)L.index + 3 * lineSize * e;
        it = reinterpret_cast<Fmatrix*>(Access((u16)base));
        VERIFY((base + 3 * lineSize) <= m_uiBufferSize);
        it->vm[0].set(trans.vm[0]);
        it->vm[1].set(trans.vm[1]);
        it->vm[2].set(trans.vm[2]);
        break;
    case RC_4x4:
        base = (u32)L.index + 4 * lineSize * e;
        it = reinterpret_cast<Fmatrix*>(Access((u16)base));
        VERIFY((base + 4 * lineSize) <= m_uiBufferSize);
        *it = trans;
        break;
    default:
#ifdef DEBUG
        Debug.fatal(DEBUG_INFO, "Invalid constant run-time-type for '%s'", *C->name);
#else
        NODEFAULT;
#endif
    }
}

IC void dx10ConstantBuffer::seta(R_constant* C, R_constant_load& L, u32 e, const Fvector4& A)
{
    VERIFY(RC_float == C->type);
    VERIFY(RC_1x4 == L.cls || RC_1x3 == L.cls || RC_1x2 == L.cls);

    static const u16 lineSize = 4 * sizeof(float);
    u32 base = (u32)L.index + lineSize * e;
    Fvector4* it = Access((u16)base);
    VERIFY((base + lineSize) <= m_uiBufferSize);
    it->set(A);
}

IC void* dx10ConstantBuffer::AccessDirect(R_constant_load& L, size_t DataSize)
{
    //	Check buffer size in client code: don't know if actual data will cross
    //	buffer boundaries.
    VERIFY(L.index < (int)m_uiBufferSize);
    BYTE* res = ((BYTE*)m_pBufferData) + L.index;

    if ((size_t)L.index + DataSize <= m_uiBufferSize)
    {
        m_bChanged = true;
        return res;
    }
    else
    {
        return nullptr;
    }
}

#endif //	dx10ConstantBuffer_impl_included

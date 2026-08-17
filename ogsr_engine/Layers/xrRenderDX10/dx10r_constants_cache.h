#ifndef dx10r_constants_cacheH
#define dx10r_constants_cacheH

class CBackend;

class R_constants final
{
public:
    enum BufferType
    {
        BT_PixelBuffer,
        BT_VertexBuffer,
        BT_GeometryBuffer,
        BT_ComputeBuffer,
        BT_HullBuffer,
        BT_DomainBuffer
    };

    void flush(CBackend& cmd_list);

    // fp, non-array versions
    template <typename... Args>
    ICF void set(CBackend& cmd_list, R_constant* C, Args&&... args)
    {
        if (C->destination & RC_dest_pixel)
            set<BT_PixelBuffer>(cmd_list, C, C->ps, std::forward<Args>(args)...);

        if (C->destination & RC_dest_vertex)
            set<BT_VertexBuffer>(cmd_list, C, C->vs, std::forward<Args>(args)...);

        if (C->destination & RC_dest_geometry)
            set<BT_GeometryBuffer>(cmd_list, C, C->gs, std::forward<Args>(args)...);

        if (C->destination & RC_dest_hull)
            set<BT_HullBuffer>(cmd_list, C, C->hs, std::forward<Args>(args)...);

        if (C->destination & RC_dest_domain)
            set<BT_DomainBuffer>(cmd_list, C, C->ds, std::forward<Args>(args)...);

        if (C->destination & RC_dest_compute)
            set<BT_ComputeBuffer>(cmd_list, C, C->cs, std::forward<Args>(args)...);
    }

    // scalars, non-array versions
    ICF void set(CBackend& cmd_list, R_constant* C, f32 x, f32 y, f32 z, f32 w) { set(cmd_list, C, Fvector4{x, y, z, w}); }

    // fp, array versions
    template <typename... Args>
    ICF void seta(CBackend& cmd_list, R_constant* C, u32 e, Args&&... args)
    {
        if (C->destination & RC_dest_pixel)
            seta<BT_PixelBuffer>(cmd_list, C, C->ps, e, std::forward<Args>(args)...);

        if (C->destination & RC_dest_vertex)
            seta<BT_VertexBuffer>(cmd_list, C, C->vs, e, std::forward<Args>(args)...);

        if (C->destination & RC_dest_geometry)
            seta<BT_GeometryBuffer>(cmd_list, C, C->gs, e, std::forward<Args>(args)...);

        if (C->destination & RC_dest_hull)
            seta<BT_HullBuffer>(cmd_list, C, C->hs, e, std::forward<Args>(args)...);

        if (C->destination & RC_dest_domain)
            seta<BT_DomainBuffer>(cmd_list, C, C->ds, e, std::forward<Args>(args)...);

        if (C->destination & RC_dest_compute)
            seta<BT_ComputeBuffer>(cmd_list, C, C->cs, e, std::forward<Args>(args)...);
    }

    // scalars, array versions
    ICF void seta(CBackend& cmd_list, R_constant* C, u32 e, f32 x, f32 y, f32 z, f32 w) { seta(cmd_list, C, e, Fvector4{x, y, z, w}); }

    ICF void access_direct(CBackend& cmd_list, R_constant* C, std::size_t DataSize, void** ppVData, void** ppGData, void** ppPData)
    {
        if (ppPData)
        {
            if (C->destination & RC_dest_pixel)
                access_direct<BT_PixelBuffer>(cmd_list, C, C->ps, ppPData, DataSize);
            else
                *ppPData = nullptr;
        }

        if (ppVData)
        {
            if (C->destination & RC_dest_vertex)
                access_direct<BT_VertexBuffer>(cmd_list, C, C->vs, ppVData, DataSize);
            else
                *ppVData = nullptr;
        }

        if (ppGData)
        {
            if (C->destination & RC_dest_geometry)
                access_direct<BT_GeometryBuffer>(cmd_list, C, C->gs, ppGData, DataSize);
            else
                *ppGData = nullptr;
        }
    }

private:
    template <BufferType BType, typename... Args>
    void set(CBackend& cmd_list, R_constant* C, R_constant_load& L, Args&&... args)
    {
        GetCBuffer<BType>(cmd_list, C).set(C, L, std::forward<Args>(args)...);
    }

    template <BufferType BType, typename... Args>
    void seta(CBackend& cmd_list, R_constant* C, R_constant_load& L, u32 e, Args&&... args)
    {
        GetCBuffer<BType>(cmd_list, C).seta(C, L, e, std::forward<Args>(args)...);
    }

    template <BufferType BType>
    void access_direct(CBackend& cmd_list, R_constant* C, R_constant_load& L, void** ppData, std::size_t DataSize) const
    {
        *ppData = GetCBuffer<BType>(cmd_list, C).AccessDirect(L, DataSize);
    }

    template <BufferType BType>
    dx10ConstantBuffer& GetCBuffer(CBackend& cmd_list, R_constant* C) const = delete; // no implicit specialization
};

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_PixelBuffer>(CBackend& cmd_list, R_constant* C) const;
template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_VertexBuffer>(CBackend& cmd_list, R_constant* C) const;
template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_GeometryBuffer>(CBackend& cmd_list, R_constant* C) const;
template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_ComputeBuffer>(CBackend& cmd_list, R_constant* C) const;
template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_HullBuffer>(CBackend& cmd_list, R_constant* C) const;
template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_DomainBuffer>(CBackend& cmd_list, R_constant* C) const;

#endif //	dx10r_constants_cacheH

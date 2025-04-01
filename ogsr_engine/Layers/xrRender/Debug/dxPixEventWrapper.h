#ifndef __DX_PIX_EVENT_WRAPPER_H
#define __DX_PIX_EVENT_WRAPPER_H

#define PIX_EVENT(Name) PIX_EVENT_CTX(RCache, Name)

#ifdef MASTER_GOLD

#define PIX_EVENT_CTX(C, Name) \
    static_assert(sizeof(C) == sizeof(CBackend)); \
    static_assert(std::char_traits<wchar_t>::length(L## #Name))

#else

#define PIX_EVENT_CTX(C, Name) dxPixEventWrapper pixEvent##Name(C, L## #Name)

class dxPixEventWrapper
{
private:
    const CBackend& cmd_list;

public:
    dxPixEventWrapper(const CBackend& cmd_list_in, const wchar_t* wszName) : cmd_list(cmd_list_in) { cmd_list.pAnnotation->BeginEvent(wszName); }
    ~dxPixEventWrapper() { cmd_list.pAnnotation->EndEvent(); }
};

#endif /* MASTER_GOLD */

#endif /* __DX_PIX_EVENT_WRAPPER_H */

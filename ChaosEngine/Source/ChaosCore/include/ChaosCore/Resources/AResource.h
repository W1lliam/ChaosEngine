#pragma once 
#include "ChaosCore/chaos_core_pch.h"
#include <ChaosDebug.h>
#include "ChaosCore/Utils/StringID.h"
#include "ChaosCore/Serialization/Serializer.h"
#include "AssetFileConstants.h"

namespace Chaos
{
    class AResourceManager;

    class AResource
    {
    public:
        enum class E_ResourceState : uint8_t
        {
            UNPREPARED,
            PREPARING,
            PREPARED,
            LOADING,
            LOADED,
            UNLOADING
        };

        AResource(AResourceManager* p_creator, const StringID& p_name, const StringID& p_group);
        virtual ~AResource() = default;
        constexpr   const StringID&     GetName ()						const	{ return m_name; };
        constexpr   AResourceManager*   GetCreator()					const	{ return m_creator; };
        constexpr   const StringID&     GetGroup()						const	{ return m_group; };
                    E_ResourceState     GetState()						const	{ return m_state.load(); };

		virtual		void			    Rename(const std::string& p_newId)		{ m_name = operator""_sid(p_newId.c_str(), p_newId.size()); };
        virtual     void                Prepare();
        virtual     void                Load();
        virtual     void                Unload();
        virtual     void                Reload();

    protected:
        AResource() : m_creator{ nullptr }  {}

       
        virtual void PreLoadImpl()      { CHAOS_CORE_TRACE("Loaded Asset {0}", m_name); }
        virtual void PostLoadImpl()     {}
        virtual void PreUnloadImpl()    {}
        virtual void PostUnloadImpl()   {}
        virtual void PrepareImpl()      {}
        virtual void UnprepareImpl()    {}
        virtual void LoadImpl()         = 0;
        virtual void UnloadImpl()       = 0;

    protected:
        AResourceManager*               m_creator;
        StringID                        m_name;
        StringID                        m_group;
        std::atomic<E_ResourceState>    m_state                 {E_ResourceState::UNPREPARED};
        volatile bool                   m_isBackgroundLoaded    {false};
    };

    using ResourcePtr = std::shared_ptr<AResource>;


    /********************
    *  RESOURCE HEADER  *
    *********************/
    struct ResourceHeader : public ISerializable
    {
        inline uint8_t Size() const { return static_cast<uint8_t>(signature.size() + 1 + sizeof(AssetType) + sizeof(uint32_t)); };

        AssetType   resourceType        {E_ASSETTYPE_INVALID};
        uint32_t    resourceSize        {0};
        uint32_t    resourceSizeOffset  {static_cast<uint32_t>(signature.size()) + 1};
        std::string signature           {".PNS"};

        ResourceHeader() = default;
        ResourceHeader(AssetType p_type) : resourceType(p_type) {};

        template <typename Stream>
        bool Serialize(Stream& p_stream)
        {
            SerializeBytes(p_stream, reinterpret_cast<uint8_t*>(signature.data()), static_cast<uint32_t>(signature.size()) + 1);
            SerializeUnsignedInt(p_stream, reinterpret_cast<uint8_t&>(resourceType), 0, E_ASSETTYPE_COUNT);
            p_stream.SerializeAlign();
            resourceSizeOffset = p_stream.GetBytesProcessed();
            SerializeBits(p_stream, resourceSize, 32);
            return true;
        };

        DECLARE_VIRTUAL_SERIALIZE_FUNCTIONS()
    };

}
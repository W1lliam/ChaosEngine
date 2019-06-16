#pragma once
#include "ChaosCore/chaos_core_pch.h"
#include "ChaosCore/Utils/StringID.h"
namespace Chaos
{

    struct FileInfo 
    {
        FileContainer*  archive;
        
        std::string        filename;
        std::string        path;
        std::string        basename;

        size_t          compressedSize;
        size_t          uncompressedSize;
    };

    class FileContainer
    {
    protected:
        enum class E_Type : uint8_t
        {
            FILESYSTEM,
            ZIP
        };

    public:
        using DataStreamPtr     = std::shared_ptr<std::ifstream>;
        using StringVectorPtr   = std::shared_ptr<std::vector<StringID>>;
        using FileInfoList      = std::vector<FileInfo>;
        using FileInfoListPtr   = std::shared_ptr<FileInfoList>;;

        FileContainer(const StringID& p_name, const E_Type& p_type)
            : m_name{ p_name }, m_type{ p_type }, m_readOnly{ true } {}
        virtual ~FileContainer() = default;

        virtual     bool            Exists          (const std::string& p_filename) = 0;
        virtual     time_t          GetModifiedTime (const std::string& p_filename) = 0;
        
        constexpr   const StringID& GetName()           const   { return m_name; }
        constexpr   const E_Type&   GetType()           const   { return m_type; }
        virtual     bool            IsReadOnly()        const   { return m_readOnly; }
        virtual     bool            IsCaseSensitive()   const   = 0;

        virtual     void            Load()                      = 0;
        virtual     void            Unload()                    = 0;

        virtual     DataStreamPtr   Open        (const std::string& p_filename, bool p_readOnly = true) = 0;
        virtual     DataStreamPtr   Create      (const std::string& p_filename)                         = 0;
        virtual     void            Remove      (const std::string& filename)                           = 0;

        virtual     StringVectorPtr List        (bool p_recursive = true, bool p_dirs = false)          = 0;
        virtual     FileInfoListPtr ListFileInfo(bool p_recursive = true, bool p_dirs = false)          = 0;
    
        virtual     StringVectorPtr Find            (const std::string& p_pattern, bool p_recursive = true, bool p_dirs = false)    = 0;
        virtual     FileInfoListPtr FindFileInfo    (const std::string& p_pattern, bool p_recursive = true, bool p_dirs = false)    = 0;

    protected:
        StringID m_name;
        E_Type m_type;
        bool m_readOnly;
    };
}
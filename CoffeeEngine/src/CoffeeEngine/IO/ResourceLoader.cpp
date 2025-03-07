#include "ResourceLoader.h"
#include "CoffeeEngine/Core/Base.h"
#include "CoffeeEngine/Core/Log.h"
#include "CoffeeEngine/IO/CacheManager.h"
#include "CoffeeEngine/IO/ImportData/CreateImportData.h"
#include "CoffeeEngine/IO/Resource.h"
#include "CoffeeEngine/Renderer/Material.h"
#include "CoffeeEngine/Renderer/Model.h"
#include "CoffeeEngine/Renderer/Shader.h"
#include "CoffeeEngine/Renderer/Texture.h"
#include "CoffeeEngine/IO/ResourceRegistry.h"
#include "CoffeeEngine/IO/ResourceImporter.h"
#include "CoffeeEngine/IO/ResourceUtils.h"
#include <filesystem>
#include <fstream>
#include <string>

namespace Coffee {

    std::filesystem::path ResourceLoader::s_WorkingDirectory = std::filesystem::current_path();
    ResourceImporter ResourceLoader::s_Importer = ResourceImporter();

    void ResourceLoader::LoadFile(const std::filesystem::path& path)
    {
        if (!is_regular_file(path))
        {
            COFFEE_CORE_ERROR("ResourceLoader::LoadResources: {0} is not a file!", path.string());
            return;
        }

        const ResourceType type = GetResourceTypeFromExtension(path);

        if(type == ResourceType::Unknown and path.extension() != ".import")
        {
            COFFEE_CORE_ERROR("ResourceLoader::LoadResources: Unsupported file extension {0}", path.extension().string());
            return;
        }

        if(path.extension() == ".import")
        {
            COFFEE_CORE_INFO("ResourceLoader::LoadDirectory: Loading resource from import file {0}", path.string());

            Scope<ImportData> importData = LoadImportData(path);
            switch (importData->type)
            {
                case ResourceType::Texture2D:
                {
                    Load<Texture2D>(*importData);
                    break;
                }
                case ResourceType::Cubemap:
                {
                    Load<Cubemap>(*importData);
                    break;
                }
                case ResourceType::Model:
                {
                    Load<Model>(*importData);
                    break;
                }
                case ResourceType::Shader:
                {
                    Load<Shader>(*importData);
                    break;
                }
                case ResourceType::Material:
                {
                    Load<Material>(*importData);
                    break;
                }
                default:
                {
                    COFFEE_CORE_ERROR("ResourceLoader::LoadResources: Unsupported resource type {0}", ResourceTypeToString(importData->type));
                    break;
                }
            }
        }
        else
        {
            std::filesystem::path importFilePath = path;
            importFilePath += ".import";
            if(std::filesystem::exists(importFilePath))
            {
                return;
            }

            switch (type)
            {
                case ResourceType::Texture2D:
                {
                    Load<Texture2D>(path);
                    break;
                }
                case ResourceType::Cubemap:
                {
                    Load<Cubemap>(path);
                    break;
                }
                case ResourceType::Model:
                {
                    Load<Model>(path);
                    break;
                }
                case ResourceType::Shader:
                {
                    Load<Shader>(path);
                    break;
                }
            }
        }
    }

    void ResourceLoader::LoadDirectory(const std::filesystem::path& directory)
    {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory))
        {
            // This two if statements are duplicated in LoadFile but are necessary to suppress errors

            if (!entry.is_regular_file())
            {
                continue;
            }

            if(GetResourceTypeFromExtension(entry.path()) == ResourceType::Unknown and entry.path().extension() != ".import")
            {
                continue;
            }

            LoadFile(entry.path());
        }
    }

    void ResourceLoader::RemoveResource(UUID uuid) // Think if would be better to pass the Resource as parameter
    {
/*         if(!ResourceRegistry::Exists(uuid))
        {
            COFFEE_CORE_ERROR("ResourceLoader::RemoveResource: Resource {0} does not exist!", (uint64_t)uuid);
            return;
        }

        // Remove the Cache file and all dependencies(TODO)
        std::filesystem::path cacheFilePath = CacheManager::GetCachePath() / (std::to_string(uuid) + ".res");
        if(std::filesystem::exists(cacheFilePath))
        {
            std::filesystem::remove(cacheFilePath);
        }

        const Ref<Resource>& resource = ResourceRegistry::Get<Resource>(uuid);

        const std::filesystem::path& resourcePath = resource->GetPath();
        std::filesystem::path importFilePath = resourcePath;
        importFilePath.replace_extension(".import");

        if(std::filesystem::exists(importFilePath))
        {
            std::filesystem::remove(importFilePath);
        }

        if(std::filesystem::exists(resourcePath))
        {
            std::filesystem::remove(resourcePath);
        }

        ResourceRegistry::Remove(uuid); */
    }

    void ResourceLoader::RemoveResource(const std::filesystem::path& path)
    {
        /*UUID uuid = GetUUIDFromImportFile(path);
        
        // Remove the Cache file and all dependencies(TODO)
        std::filesystem::path cacheFilePath = CacheManager::GetCachePath() / (std::to_string(uuid) + ".res");
        if(std::filesystem::exists(cacheFilePath))
        {
            std::filesystem::remove(cacheFilePath);
        }

        const Ref<Resource>& resource = ResourceRegistry::Get<Resource>(uuid);

        const std::filesystem::path& resourcePath = resource->GetPath();
        std::filesystem::path importFilePath = resourcePath;
        importFilePath.replace_extension(".import");

        if(std::filesystem::exists(importFilePath))
        {
            std::filesystem::remove(importFilePath);
        }

        if(std::filesystem::exists(resourcePath))
        {
            std::filesystem::remove(resourcePath);
        }

        if(ResourceRegistry::Exists(uuid))
        {
            ResourceRegistry::Remove(uuid);
        }*/
    }

    void ResourceLoader::SaveImportData(Scope<ImportData>& importData)
    {
        std::filesystem::path importFilePath = importData->originalPath;
        importFilePath += ".import";

        // backup the original path
        std::filesystem::path originalPathCopy = importData->originalPath;

        importData->originalPath = std::filesystem::relative(importData->originalPath, s_WorkingDirectory);

        std::ofstream importFile(importFilePath);
        cereal::JSONOutputArchive archive(importFile);
        archive(CEREAL_NVP(importData));

        // restore the original path
        importData->originalPath = originalPathCopy;
    }

    // TODO: Think if the path should be the .import path or the resource and replace the extension inside the function
    Scope<ImportData> ResourceLoader::LoadImportData(const std::filesystem::path& path)
    {
        Scope<ImportData> importData;

        std::filesystem::path importFilePath = path;

        if(path.extension() != ".import") {
            COFFEE_CORE_ERROR("ResourceLoader::LoadImportData: Import file {0} does not have the .import extension!", path.string());
            throw std::runtime_error("Import file does not have the .import extension");
        }
    
        if (!std::filesystem::exists(importFilePath))
        {
            COFFEE_CORE_ERROR("ResourceLoader::LoadImportData: Import file {0} does not exist!", importFilePath.string());
            throw std::runtime_error("Import file does not exist");
        }
    
        std::ifstream importFile(importFilePath);
        if (!importFile.is_open())
        {
            COFFEE_CORE_ERROR("ResourceLoader::LoadImportData: Failed to open import file {0}", importFilePath.string());
            throw std::runtime_error("Failed to open import file");
        }
    
        try
        {
            cereal::JSONInputArchive archive(importFile);
            archive(CEREAL_NVP(importData));
        }
        catch (const cereal::Exception& e)
        {
            COFFEE_CORE_ERROR("ResourceLoader::LoadImportData: Failed to parse import file {0}: {1}", importFilePath.string(), e.what());
            throw;
        }
    
        // Convert the relative path to an absolute path
        importData->originalPath = s_WorkingDirectory / importData->originalPath;
    
        return importData;
    }
}
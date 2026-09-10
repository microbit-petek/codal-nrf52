#include "NRF52ResourceManager.h"
#include "ErrorNo.h"
#include "Resource.h"

NRF52ResourceManager NRF52ResourceManager::_resourceManager;

NRF52ResourceManager &NRF52ResourceManager::get()
{
    return NRF52ResourceManager::_resourceManager;
}

ErrorCode NRF52ResourceManager::releaseResource(ResourceId id)
{
    ErrorCode returnCode = DEVICE_OK;
    if (resourceTable[id] != NULL)
    {
        returnCode = resourceTable[id]->disconnect();
        if (returnCode != DEVICE_BUSY)
            resourceTable[id] = NULL;
    }
    return returnCode;
}

ErrorCode NRF52ResourceManager::releaseResource(Resource &resource)
{
    uint8_t index = 0;
    for (;index < RESOURCE_COUNT; ++index)
    {
        if (resourceTable[index] == &resource)
            break;
    }

    if (RESOURCE_COUNT == index)
        return DEVICE_INVALID_PARAMETER;
    
    resourceTable[index] = NULL;
    return DEVICE_OK;
}

ErrorCode NRF52ResourceManager::registerResource(ResourceId id, Resource &resource)
{
    if (resourceTable[id] != NULL)
        return DEVICE_INVALID_STATE;

    resourceTable[id] = &resource;
    return DEVICE_OK;
}

NRF52ResourceManager::NRF52ResourceManager() {}
